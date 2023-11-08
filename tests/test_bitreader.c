// SPDX-License-Identifier: GPL-2.0
/*
 * test_bitreader - short test for the internal bitreader.
 *
 * Copyright (C) 2023 Armin Wolf <W_Armin@gmx.de>
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <bitreader.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

int main(void)
{
	uint8_t buffer[] = {0xFF, 0x37, 0x09, 0xFF, 0x00, 0x11};
	uint16_t bits[] = {0xF, 0x7, 0x3, 0x849B, 0x007F, 0x22};
	uint8_t num_zeros[] = {0, 0, 0, 0, 0, 1};
	uint8_t num_bits[] = {4, 3, 2, 16, 14, 9};
	struct bitreader reader;
	uint16_t val;
	int ret;

	bitreader_init(&reader, buffer, sizeof(buffer));

	for (int i = 0; i < ARRAY_SIZE(bits); i++) {
		ret = ffs_bits(&reader);
		if (!ret) {
			fprintf(stderr, "No set bits found at interation %d\n", i);
			exit(EXIT_FAILURE);
		}

		if (ret - 1 != num_zeros[i]) {
			fprintf(stderr, "Error determining zero bits at iteration %d: %d != %u\n",
				i, ret, num_zeros[i]);
			exit(EXIT_FAILURE);
		}

		ret = read_bits(&reader, &val, num_bits[i]);
		if (ret < 0) {
			fprintf(stderr, "Error reading bits at iteration %d: %s\n", i,
				strerror(-ret));
			exit(EXIT_FAILURE);
		}

		if (val != bits[i]) {
			fprintf(stderr, "Bit error at iteration %d: 0x%x != 0x%x\n", i, val, bits[i]);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
