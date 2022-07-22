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

#include "pmsis.h"
#include <stdint.h>
#include <stdio.h>
#include "lfsr32.h"

#define DEFAULT_SEED 0xdeadbeef
#define USE_BYTE_FEEDBACK

int glob_errors;

#ifdef USE_BYTE_FEEDBACK
// not actually extern, just down the bottom
extern uint32_t __lfsr_byte_feedback[];
#else
uint32_t *__lfsr_byte_feedback;
#endif

#ifndef USE_BYTE_FEEDBACK
uint32_t __lfsr_iter_bit(uint32_t lfsr) {
  return (lfsr & 1) ? ((lfsr >> 1) ^ FEEDBACK) : (lfsr >> 1);
}
#endif

uint32_t __lfsr_iter_byte(uint32_t lfsr, uint32_t *lfsr_byte_feedback) {
#ifdef USE_BYTE_FEEDBACK
  // this shift/lookup/xor is equivalent to 8 iterations of
  // lfsr = (lfsr & 1) ? ((lfsr >> 1) ^ 0x800000000000000D) : (lfsr >> 1);
  return (lfsr >> 8) ^ lfsr_byte_feedback[lfsr & 0xff];
#else
  uint32_t l = lfsr;
  for(int i=0; i<8; i++)
    l = __lfsr_iter_bit(l);
  return l;
#endif
}

uint32_t __lfsr_iter_word(uint32_t lfsr, uint32_t *lfsr_byte_feedback) {
  uint32_t l = __lfsr_iter_byte(lfsr, lfsr_byte_feedback);
  l = __lfsr_iter_byte(l, lfsr_byte_feedback);
  l = __lfsr_iter_byte(l, lfsr_byte_feedback);
  return __lfsr_iter_byte(l, lfsr_byte_feedback);
}

int generate_random_buffer(int addr_first, int addr_last, uint32_t seed) {
  uint32_t lfsr = seed;
  for(uint32_t addr=addr_first; addr<addr_last; addr+=4) {
    lfsr = __lfsr_iter_word(lfsr, __lfsr_byte_feedback);
    *(uint32_t *)(addr) = lfsr;
  }
  return 0;
}

int check_random_buffer(int addr_first, int addr_last, uint32_t seed) {
  int cnt = 0;
  uint32_t lfsr = seed;
  for(uint32_t addr=addr_first; addr<addr_last; addr+=4) {
    lfsr = __lfsr_iter_word(lfsr, __lfsr_byte_feedback);
    cnt += __builtin_pulp_cnt(lfsr ^ *(uint32_t *)(addr));
  }
  return cnt;
}

#ifdef USE_BYTE_FEEDBACK
// pre-computed feedback terms
// see http://users.ece.cmu.edu/~koopman/lfsr/index.html and lfsr32_precomp.c
uint32_t __lfsr_byte_feedback[256] = {
  0x00000000, 0x1300000b, 0x26000016, 0x3500001d,
  0x4c00002c, 0x5f000027, 0x6a00003a, 0x79000031,
  0x98000058, 0x8b000053, 0xbe00004e, 0xad000045,
  0xd4000074, 0xc700007f, 0xf2000062, 0xe1000069,
  0x3000001f, 0x23000014, 0x16000009, 0x05000002,
  0x7c000033, 0x6f000038, 0x5a000025, 0x4900002e,
  0xa8000047, 0xbb00004c, 0x8e000051, 0x9d00005a,
  0xe400006b, 0xf7000060, 0xc200007d, 0xd1000076,
  0x6000003e, 0x73000035, 0x46000028, 0x55000023,
  0x2c000012, 0x3f000019, 0x0a000004, 0x1900000f,
  0xf8000066, 0xeb00006d, 0xde000070, 0xcd00007b,
  0xb400004a, 0xa7000041, 0x9200005c, 0x81000057,
  0x50000021, 0x4300002a, 0x76000037, 0x6500003c,
  0x1c00000d, 0x0f000006, 0x3a00001b, 0x29000010,
  0xc8000079, 0xdb000072, 0xee00006f, 0xfd000064,
  0x84000055, 0x9700005e, 0xa2000043, 0xb1000048,
  0xc000007c, 0xd3000077, 0xe600006a, 0xf5000061,
  0x8c000050, 0x9f00005b, 0xaa000046, 0xb900004d,
  0x58000024, 0x4b00002f, 0x7e000032, 0x6d000039,
  0x14000008, 0x07000003, 0x3200001e, 0x21000015,
  0xf0000063, 0xe3000068, 0xd6000075, 0xc500007e,
  0xbc00004f, 0xaf000044, 0x9a000059, 0x89000052,
  0x6800003b, 0x7b000030, 0x4e00002d, 0x5d000026,
  0x24000017, 0x3700001c, 0x02000001, 0x1100000a,
  0xa0000042, 0xb3000049, 0x86000054, 0x9500005f,
  0xec00006e, 0xff000065, 0xca000078, 0xd9000073,
  0x3800001a, 0x2b000011, 0x1e00000c, 0x0d000007,
  0x74000036, 0x6700003d, 0x52000020, 0x4100002b,
  0x9000005d, 0x83000056, 0xb600004b, 0xa5000040,
  0xdc000071, 0xcf00007a, 0xfa000067, 0xe900006c,
  0x08000005, 0x1b00000e, 0x2e000013, 0x3d000018,
  0x44000029, 0x57000022, 0x6200003f, 0x71000034,
  0x80000057, 0x9300005c, 0xa6000041, 0xb500004a,
  0xcc00007b, 0xdf000070, 0xea00006d, 0xf9000066,
  0x1800000f, 0x0b000004, 0x3e000019, 0x2d000012,
  0x54000023, 0x47000028, 0x72000035, 0x6100003e,
  0xb0000048, 0xa3000043, 0x9600005e, 0x85000055,
  0xfc000064, 0xef00006f, 0xda000072, 0xc9000079,
  0x28000010, 0x3b00001b, 0x0e000006, 0x1d00000d,
  0x6400003c, 0x77000037, 0x4200002a, 0x51000021,
  0xe0000069, 0xf3000062, 0xc600007f, 0xd5000074,
  0xac000045, 0xbf00004e, 0x8a000053, 0x99000058,
  0x78000031, 0x6b00003a, 0x5e000027, 0x4d00002c,
  0x3400001d, 0x27000016, 0x1200000b, 0x01000000,
  0xd0000076, 0xc300007d, 0xf6000060, 0xe500006b,
  0x9c00005a, 0x8f000051, 0xba00004c, 0xa9000047,
  0x4800002e, 0x5b000025, 0x6e000038, 0x7d000033,
  0x04000002, 0x17000009, 0x22000014, 0x3100001f,
  0x4000002b, 0x53000020, 0x6600003d, 0x75000036,
  0x0c000007, 0x1f00000c, 0x2a000011, 0x3900001a,
  0xd8000073, 0xcb000078, 0xfe000065, 0xed00006e,
  0x9400005f, 0x87000054, 0xb2000049, 0xa1000042,
  0x70000034, 0x6300003f, 0x56000022, 0x45000029,
  0x3c000018, 0x2f000013, 0x1a00000e, 0x09000005,
  0xe800006c, 0xfb000067, 0xce00007a, 0xdd000071,
  0xa4000040, 0xb700004b, 0x82000056, 0x9100005d,
  0x20000015, 0x3300001e, 0x06000003, 0x15000008,
  0x6c000039, 0x7f000032, 0x4a00002f, 0x59000024,
  0xb800004d, 0xab000046, 0x9e00005b, 0x8d000050,
  0xf4000061, 0xe700006a, 0xd2000077, 0xc100007c,
  0x1000000a, 0x03000001, 0x3600001c, 0x25000017,
  0x5c000026, 0x4f00002d, 0x7a000030, 0x6900003b,
  0x88000052, 0x9b000059, 0xae000044, 0xbd00004f,
  0xc400007e, 0xd7000075, 0xe2000068, 0xf1000063,
};
#endif

