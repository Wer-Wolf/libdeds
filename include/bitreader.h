/* SPDX-License-Identifier: GPL-2.0 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct bitreader {
	uint8_t bit_offset;
	uint8_t *buffer;
	size_t buffer_size;
	size_t index;
};

void bitreader_init(struct bitreader *reader, uint8_t *buffer, size_t buffer_size);
int read_bits(struct bitreader *reader, uint16_t *bits, uint8_t num_bits);
int ffs_bits(struct bitreader *reader);
bool bits_available(struct bitreader *reader, uint8_t bits);
