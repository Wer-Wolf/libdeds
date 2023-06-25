// SPDX-License-Identifier: GPL-2.0
/*
 * libds - library for DoubleSpace decompression.
 *
 * Copyright (C) 2023 Armin Wolf <W_Armin@gmx.de>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <endian.h>
#include <errno.h>

#include <bitreader.h>
#include <ds.h>

#define DS_MAGIC_BITS		16
#define DS_MAGIC		0x5344

#define DS_VERSION_BITS		16

#define CMD_BITS		2
#define CMD_PAIR_STANDARD	0x0
#define CMD_LITERAL_BIG		0x1
#define CMD_LITERAL_SMALL	0x2
#define CMD_PAIR_EXTENDED	0x3

#define LITERAL_BITS		7

#define OFFSET_SMALL_BITS	6
#define OFFSET_MEDIUM_BITS	8
#define OFFSET_LARGE_BITS	12
#define OFFSET_MEDIUM_OFFSET	64
#define OFFSET_LARGE_OFFSET	320
#define OFFSET_SYNC		0x113f

#define LENGTH_MAX_BITS		9

static const uint16_t length_offset[] = {
	3,
	4,
	6,
	10,
	18,
	34,
	66,
	130,
	258
};

static int ds_check_magic(struct bitreader *reader)
{
	uint16_t magic;
	int ret;

	ret = read_bits(reader, &magic, DS_MAGIC_BITS);
	if (ret < 0)
		return ret;

	if (le16toh(magic) != DS_MAGIC)
		return -EPROTONOSUPPORT;

	return 0;
}

static int ds_get_version(struct bitreader *reader, uint16_t *version)
{
	uint16_t raw_version;
	int ret;

	ret = read_bits(reader, &raw_version, DS_VERSION_BITS);
	if (ret < 0)
		return ret;

	*version = be16toh(raw_version);

	return 0;
}

static int ds_get_offset(struct bitreader *reader, uint16_t *offset, uint8_t bits)
{
	uint16_t raw_offset;
	int ret;

	ret = read_bits(reader, &raw_offset, bits);
	if (ret < 0)
		return ret;

	*offset = le16toh(raw_offset);

	return 0;
}

static int ds_get_length(struct bitreader *reader, uint16_t *length)
{
	uint16_t val;
	int ret;

	for (uint8_t num_bits = 0; num_bits < LENGTH_MAX_BITS; num_bits++) {
		ret = read_bits(reader, &val, 1);
		if (ret < 0)
			return ret;

		if (!val)
			continue;

		ret = read_bits(reader, &val, num_bits);
		if (ret < 0)
			return ret;

		*length = le16toh(val) + length_offset[num_bits];

		return 0;
	}

	return -EINVAL;
}

static int ds_put_byte(uint8_t byte, uint8_t *output, size_t *index, size_t output_length)
{
	if (*index >= output_length)
		return -EINVAL;

	output[*index] = byte;
	(*index)++;

	return 0;
}

static int ds_decode(struct bitreader *reader, uint8_t *output, size_t *index, size_t output_length,
		     uint16_t offset)
{
	uint16_t length;
	int ret;

	if (offset == 0)
		return -EINVAL;

	if (offset == OFFSET_SYNC) {
		if (remaining_bits(reader) < 16)
			return 1;

		if (*index % 512)
			return -EINVAL;

		return 0;
	}

	if (offset > *index)
		return -EINVAL;

	ret = ds_get_length(reader, &length);
	if (ret < 0)
		return ret;

	length--;

	if (*index + length > output_length)
		return -EINVAL;

	/*
	 * We cannot use memcpy here, since the overlapping part (if any)
	 * needs to be copied too, something which does not work with memcpy.
	 */
	for (uint16_t i = 0; i < length; i++)
	{
		output[*index] = output[*index - offset];
		(*index)++;
	}

	return 0;
}

static int ds_loop(struct bitreader *reader, uint8_t *output, size_t length, size_t *result_length)
{
	uint16_t cmd, literal, offset;
	size_t index = 0;
	int ret;

	while (true) {
		ret = read_bits(reader, &cmd, CMD_BITS);
		if (ret < 0)
			return ret;

		switch (cmd) {
		case CMD_PAIR_STANDARD:
			ret = ds_get_offset(reader, &offset, OFFSET_SMALL_BITS);
			if (ret < 0)
				return ret;

			ret = ds_decode(reader, output, &index, length, offset);
			if (ret < 0)
				return ret;

			break;
		case CMD_LITERAL_BIG:
			ret = read_bits(reader, &literal, LITERAL_BITS);
			if (ret < 0)
				return ret;

			ret = ds_put_byte(literal | 0x80, output, &index, length);
			if (ret < 0)
				return ret;

			break;
		case CMD_LITERAL_SMALL:
			ret = read_bits(reader, &literal, LITERAL_BITS);
			if (ret < 0)
				return ret;

			ret = ds_put_byte(literal, output, &index, length);
			if (ret < 0)
				return ret;

			break;
		case CMD_PAIR_EXTENDED:
			ret = read_bits(reader, &literal, 1);
			if (ret < 0)
				return ret;

			if (literal) {
				ret = ds_get_offset(reader, &offset, OFFSET_LARGE_BITS);
				if (ret < 0)
					return ret;

				offset += OFFSET_LARGE_OFFSET;
			} else {
				ret = ds_get_offset(reader, &offset, OFFSET_MEDIUM_BITS);
				if (ret < 0)
					return ret;

				offset += OFFSET_MEDIUM_OFFSET;
			}

			ret = ds_decode(reader, output, &index, length, offset);
			if (ret < 0)
				return ret;

			if (ret == 1) {
				*result_length = index;	/* ds_decode did not change index in this case */
				return 0;
			}

			break;
		default:
			return -EINVAL;
		}
	}

	return -EINVAL;
}

int ds_decompress(uint8_t *input, size_t input_length, uint8_t *output, size_t output_length,
		  size_t *result_length)
{
	struct bitreader reader;
	uint16_t version;
	int ret;

	bitreader_init(&reader, input, input_length);

	ret = ds_check_magic(&reader);
	if (ret < 0)
		return ret;

	ret = ds_get_version(&reader, &version);
	if (ret < 0)
		return ret;

	ret = ds_loop(&reader, output, output_length, result_length);
	if (ret < 0)
		return ret;
			
	return version;
}
