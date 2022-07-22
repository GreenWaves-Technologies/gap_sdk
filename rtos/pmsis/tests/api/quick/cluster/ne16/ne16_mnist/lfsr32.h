/*
 * Copyright 2019-2020 Francesco Conti <francesco.conti88@gmail.com>
 *
 * Adapted from https://github.com/russm/lfsr64
 * This is a simple 32-bit linear feedback shift register, printing
 * pseudo-random bytes to stdout.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#define DEFAULT_SEED 0xdeadbeef
#define USE_BYTE_FEEDBACK

int generate_random_buffer(int addr_first, int addr_last, uint32_t seed);
int check_random_buffer(int addr_first, int addr_last, uint32_t seed);
