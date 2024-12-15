/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stddef.h>
#include <stdint.h>

int ds_decompress(uint8_t *input, size_t input_length, uint8_t *output, size_t output_length,
		  size_t *result_length);
