// SPDX-License-Identifier: GPL-2.0
/*
 * bitreader - utility for bitwise reading from a buffer.
 *
 * Copyright (C) 2023 Armin Wolf <W_Armin@gmx.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#include <bitreader.h>

#define PREFETCH_THRESHOLD	16

static const uint16_t mask[] = {
	0x0000,
	0x0001,
	0x0003,
	0x0007,
	0x000F,
	0x001F,
	0x003F,
	0x007F,
	0x00FF,
	0x01FF,
	0x03FF,
	0x07FF,
	0x0FFF,
	0x1FFF,
	0x3FFF,
	0x7FFF,
	0xFFFF
};

void bitreader_init(struct bitreader *reader, uint8_t *buffer, size_t buffer_size)
{
	reader->prefetch = 0;
	reader->prefetched_bits = 0;
	reader->buffer = buffer;
	reader->buffer_size = buffer_size;
	reader->index = 0;
}

static void prefetch(struct bitreader *reader)
{
	uint8_t val;

	if (reader->prefetched_bits >= PREFETCH_THRESHOLD)
		return;

	for (int i = 0; i < 2; i++) {
		if (reader->index >= reader->buffer_size)
			break;

		val = reader->buffer[reader->index];
		reader->index++;
		reader->prefetch |= val << (reader->prefetched_bits);
		reader->prefetched_bits += 8;
	}
}

int read_bits(struct bitreader *reader, uint16_t *bits, uint8_t num_bits)
{
	prefetch(reader);

	if (num_bits > PREFETCH_THRESHOLD)
		return -EINVAL;

	if (reader->prefetched_bits < num_bits)
		return -ENODATA;

	*bits = (reader->prefetch & mask[num_bits]);
	reader->prefetch >>= num_bits;
	reader->prefetched_bits -= num_bits;

	return 0;
}

size_t remaining_bits(struct bitreader *reader)
{
	return reader->prefetched_bits + (reader->buffer_size - reader->index) * 8;
}
