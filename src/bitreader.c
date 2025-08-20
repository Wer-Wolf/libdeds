// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * bitreader - utility for bitwise reading from a buffer.
 *
 * Copyright (C) 2023 Armin Wolf <W_Armin@gmx.de>
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <bitreader.h>

#define MAX_BITS	16

void bitreader_init(struct bitreader *reader, uint8_t *buffer, size_t buffer_size)
{
	reader->buffer = buffer;
	reader->buffer_size = buffer_size;
	reader->bit_offset = 0;
	reader->index = 0;
}

static size_t available_bytes(struct bitreader *reader)
{
	return reader->buffer_size - reader->index;
}

static uint8_t required_bytes(struct bitreader *reader, uint8_t bits)
{
	uint8_t total_bits = reader->bit_offset + bits;
	uint8_t bytes = total_bits / 8;

	if (total_bits % 8)
		bytes++;

	return bytes;
}

int read_bits(struct bitreader *reader, uint16_t *bits, uint8_t num_bits)
{
	uint32_t value;
	uint8_t bytes;

	assert(num_bits <= MAX_BITS);

	bytes = required_bytes(reader, num_bits);
	if (available_bytes(reader) < bytes)
		return -ENODATA;

	memcpy(&value, &reader->buffer[reader->index], bytes);

	value >>= reader->bit_offset;
	*bits = value & (0xffff >> (MAX_BITS - num_bits));

	reader->bit_offset += num_bits;
	reader->index += reader->bit_offset / 8;
	reader->bit_offset %= 8;

	return 0;
}

int ffs_bits(struct bitreader *reader)
{
	uint32_t value = 0;
	size_t available;

	available = available_bytes(reader);
	if (available > MAX_BITS / 8 + 1)
		available = MAX_BITS / 8 + 1;

	memcpy(&value, &reader->buffer[reader->index], available);

	value >>= reader->bit_offset;
	value &= 0xffff;

	return ffs(value);
}

bool bits_available(struct bitreader *reader, uint8_t bits)
{
	assert(bits <= MAX_BITS);

	return !(available_bytes(reader) < required_bytes(reader, bits));
}
