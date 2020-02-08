/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "fs_switch.h"

extern void verify_crc_int8(int step, unsigned char verif, signed char * buf, int size);
extern void verify_crc_int16(int step, unsigned char verif, signed short * buf, int size);
extern void verify_crc_int32(int step, unsigned char verif, signed int * buf, int size);

#ifdef DONT_DUMP
#define dt_open_dump_file(_x) 0
#define dt_write_tensor(...) 0
#define dt_close_dump_file
#else

extern int write_tensor(int step, int before, const void * vbuf, int len);

typedef enum {
	TT_INPUT = 0,
	TT_OUTPUT,
	TT_WEIGHTS,
	TT_BIASES
} dt_tensor_type_t;

typedef struct {
	int step;
	dt_tensor_type_t tensor_type;
 	int size;
 	int elem_size;
	int num_dims;
	int dims[4];
} dt_tensor_t;

extern switch_file_t dt_fp;

int dt_open_dump_file(const char * filename);
int dt_write_tensor(const void * vbuf, int step, dt_tensor_type_t tensor_type,
 					int size, int elem_size, int num_dims, ...);
void dt_close_dump_file();
#endif
#endif // __HELPERS_H__
