/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <helpers/helpers.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

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

#ifdef __EMUL__
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
	int fp = open(filename, O_WRONLY|O_CREAT, S_IRWXU);
	if (!fp) {
		printf("Can't open dump file - %s\n", strerror(errno));
		exit(1);
	}
	int idx = 0;
	while (idx < len) {
		int res = write(fp, &buf[idx], MIN(len - idx, 1024));
		if (res < 0) {
			printf("Can't write dump file - %s\n", strerror(errno));
			exit(1);
		}
		idx += res;
	}
	printf("Wrote %d bytes\n", idx);
	close(fp);
}

int dt_fp = -1;

int dt_open_dump_file(const char * filename)
{
	dt_fp = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (!dt_fp) {
		printf("Can't open dump file - %s\n", strerror(errno));
		return 1;
	}
	return 0;
}

int dt_write_tensor(const void * vbuf, int step, dt_tensor_type_t tensor_type,
 					int size, int elem_size, int num_dims, ...)
{
	const char *buf = (char *) vbuf;
	int idx = 0;
	dt_tensor_t td = {step, tensor_type, size, elem_size, num_dims, {0, 0, 0, 0}};
	va_list argp;
  	va_start(argp, num_dims);
	for (int i=0; i<num_dims; i++) {
		td.dims[i] = va_arg(argp, int);
	}	
	va_end(argp);
	int len = td.size * td.elem_size / 8;
	printf("Writing tensor step %d type %d len %d addr %p\n", td.step, td.tensor_type, len, vbuf);
	if (write(dt_fp, (char *)&td, sizeof(td)) != sizeof(td)) {
		printf("Can't write header to dump file - %s\n", strerror(errno));
		exit(1);
	}
	while (idx < len) {
		int res = write(dt_fp, &buf[idx], MIN(len - idx, 1024));
		if (res < 0) {
			printf("Can't write dump file - %s\n", strerror(errno));
			exit(1);
		}
		idx += res;
	}
	printf("Wrote %d bytes\n", idx);
}

void dt_close_dump_file()
{
	close(dt_fp);
	dt_fp = -1;
}
#endif