/* SPDX-License-Identifier: GPL-2.0 */

#include <stddef.h>
#include <stdint.h>

struct bitreader {
	uint32_t prefetch;
	uint8_t prefetched_bits;
	uint8_t *buffer;
	size_t buffer_size;
	size_t index;
};

void bitreader_init(struct bitreader *reader, uint8_t *buffer, size_t buffer_size);
int read_bits(struct bitreader *reader, uint16_t *bits, uint8_t num_bits);
size_t remaining_bits(struct bitreader *reader);
