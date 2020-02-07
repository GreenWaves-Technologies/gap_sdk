/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <helpers.h>

static unsigned char calc_crc_int8(signed char * buf, int size)
{
	unsigned char crc = 0;
	for (int i=0; i < size; i++) {
		crc += (unsigned char) buf[i];
	}
	return crc;
}

void verify_crc_int8(int step, unsigned char verif, signed char * buf, int size)
{
	unsigned char calc = calc_crc_int8(buf, size);
	if (calc != verif) {
		printf("Failed checksum at step %d expected %hhu got %hhu\n", step, verif, calc);
	}
}

static unsigned char calc_crc_int16(signed short * buf, int size)
{
	unsigned short crc = 0;
	for (int i=0; i < size; i++) {
		crc += (&buf[0])[i];
		crc &= 0xff;
	}
	return (unsigned char) crc;
}

void verify_crc_int16(int step, unsigned char verif, signed short * buf, int size)
{
	unsigned char calc = calc_crc_int16(buf, size);
	if (calc != verif) {
		printf("Failed checksum at step %d expected %hu got %hu\n", step, verif, calc);
	}
}

static unsigned char calc_crc_int32(signed int * buf, int size)
{
	unsigned int crc = 0;
	for (int i=0; i < size; i++) {
		crc += (&buf[0])[i];
		crc &= 0xff;
	}
	return (unsigned char) crc;
}

void verify_crc_int32(int step, unsigned char verif, signed int * buf, int size)
{
	unsigned char calc = calc_crc_int32(buf, size);
	if (calc != verif) {
		printf("Failed checksum at step %d expected %u got %u\n", step, verif, calc);
	}
}

#ifndef MIN
#define MIN(x, y) (x<y?x:y)
#endif

const char * PARAM_TYPES[] = {
	"output",
	"input",
	"weights",
	"biases",
};

int write_tensor(int step, int type, const void * vbuf, int len)
{
	char filename[255];
	const char *buf = (char *) vbuf;
	sprintf(filename, "step_%d_%s.tensor", step, PARAM_TYPES[type]);
	printf("Writing tensor step %d type %d file %s len %d\n", step, type, filename, len);
	switch_fs_t fs;
	__FS_INIT(fs);
	switch_file_t fp = __OPEN_WRITE(fs, filename);
	if (!fp) {
		printf("Can't open dump file - %s\n", strerror(errno));
		exit(1);
	}
	int idx = 0;
	while (idx < len) {
		int res = __WRITE(fp, &buf[idx], MIN(len - idx, 1024));
		if (res < 0) {
			printf("Can't write dump file - %s\n", strerror(errno));
			exit(1);
		}
		idx += res;
	}
	printf("Wrote %d bytes\n", idx);
	__CLOSE(fp);
	__FS_DEINIT(fs);
}

#ifndef DONT_DUMP
switch_file_t dt_fp = (switch_file_t) 0;
switch_fs_t dt_fs;
int pos;
dt_tensor_t * td_buf = 0;

int dt_open_dump_file(const char * filename)
{
	__FS_INIT(dt_fs);
	pos = 0;
	dt_fp = __OPEN_WRITE(dt_fs, filename);
	if (!dt_fp) {
		printf("Can't open dump file - %s\n", strerror(errno));
		return 1;
	}
	td_buf = (dt_tensor_t *) AT_L2_ALLOC(0, sizeof(dt_tensor_t));
	return 0;
}

int dt_write_tensor(const void * vbuf, int step, dt_tensor_type_t tensor_type,
 					int size, int elem_size, int num_dims, ...)
{
	const char *buf = (char *) vbuf;
	int idx = 0;
	memset(td_buf, 0, sizeof(dt_tensor_t));
	td_buf->step = step;
	td_buf->tensor_type = tensor_type;
	td_buf->size = size;
	td_buf->elem_size = elem_size;
	td_buf->num_dims = num_dims;

	va_list argp;
  	va_start(argp, num_dims);
	for (int i=0; i<num_dims; i++) {
		td_buf->dims[i] = va_arg(argp, int);
	}	
	va_end(argp);
	int len = td_buf->size * td_buf->elem_size / 8;
	printf("Writing tensor td_buf %p buf_len %d pos %d step %d type %d len %d addr %p\n", td_buf, sizeof(dt_tensor_t), pos, td_buf->step, td_buf->tensor_type, len, vbuf);
	int res = __WRITEATCLUSTER(dt_fp, pos, (char *)td_buf, sizeof(dt_tensor_t));
	if (res != sizeof(dt_tensor_t)) {
		printf("Can't write header to dump file - %d - %s\n", res, strerror(errno));
		exit(1);
	}
	pos += sizeof(dt_tensor_t);
	while (idx < len) {
		int res = __WRITEATCLUSTER(dt_fp, pos, &buf[idx], MIN(len - idx, 1024));
		if (res < 0) {
			printf("Can't write dump file - %s\n", strerror(errno));
			exit(1);
		}
		pos += res;
		idx += res;
	}
	printf("Wrote %d bytes\n", idx);
}

void dt_close_dump_file()
{
	__CLOSE(dt_fp);
	__FS_DEINIT(dt_fs);
	AT_L2_FREE(0, (void *) td_buf, sizeof(dt_tensor_t));
}
#endif
