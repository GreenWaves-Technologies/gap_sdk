/*
 * Copyright 2020 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HELPERS_H__
#define __HELPERS_H__

// TODO - Delete this
#ifndef __EMUL__
#define __EMUL__
#endif

extern void verify_crc_int8(int step, unsigned char verif, signed char * buf, int size);
extern void verify_crc_int16(int step, unsigned char verif, signed short * buf, int size);
extern void verify_crc_int32(int step, unsigned char verif, signed int * buf, int size);

#ifdef __EMUL__
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

extern int dt_fp;

int dt_open_dump_file(const char * filename);
int dt_write_tensor(const void * vbuf, int step, dt_tensor_type_t tensor_type,
 					int size, int elem_size, int num_dims, ...);
void dt_close_dump_file();
#endif

#endif // __HELPERS_H__