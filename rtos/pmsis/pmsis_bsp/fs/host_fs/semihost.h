/*
 * Copyright (C) 2018 Greenwaves Technologies
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

#ifndef _MACHINE_SYSCALL_H
#define _MACHINE_SYSCALL_H

#include <stdlib.h>
#include <stdint.h>

enum semihosting_operation_numbers {
	/*
	 * ARM/openocd semihosting operations.
	 * extracted from openocd semihosting_commong.h file
	 */
	SEMIHOSTING_ENTER_SVC = 0x17,	/* DEPRECATED */

	SEMIHOSTING_SYS_CLOCK = 0x10,
	SEMIHOSTING_SYS_ELAPSED = 0x30,
	
	SEMIHOSTING_SYS_ERRNO = 0x13,
	
	SEMIHOSTING_SYS_EXIT = 0x18,
	SEMIHOSTING_SYS_EXIT_EXTENDED = 0x20,
	// stat
	SEMIHOSTING_SYS_FLEN = 0x0C,
	SEMIHOSTING_SYS_GET_CMDLINE = 0x15,
	SEMIHOSTING_SYS_HEAPINFO = 0x16,
	SEMIHOSTING_SYS_ISERROR = 0x08,
	SEMIHOSTING_SYS_ISTTY = 0x09,
	
	// File operations
	SEMIHOSTING_SYS_OPEN = 0x01,
	SEMIHOSTING_SYS_CLOSE = 0x02,
	SEMIHOSTING_SYS_READ = 0x06,
	SEMIHOSTING_SYS_READC = 0x07,
	SEMIHOSTING_SYS_REMOVE = 0x0E,
	SEMIHOSTING_SYS_RENAME = 0x0F,
	SEMIHOSTING_SYS_SEEK = 0x0A,
	SEMIHOSTING_SYS_WRITE = 0x05,
	SEMIHOSTING_SYS_WRITEC = 0x03,
	// roughly a printf (print a string terminated by '\0')
	SEMIHOSTING_SYS_WRITE0 = 0x04,
	
	SEMIHOSTING_SYS_SYSTEM = 0x12,
	SEMIHOSTING_SYS_TICKFREQ = 0x31,
	SEMIHOSTING_SYS_TIME = 0x11,
	SEMIHOSTING_SYS_TMPNAM = 0x0D,
};


#define SEMIHOST_EXIT_SUCCESS 0x20026
#define SEMIHOST_EXIT_ERROR   0x20023

extern long __syscall_error(long);

/* riscv semihosting standard: 
 * IN: a0 holds syscall number
 * IN: a1 holds pointer to arg struct
 * OUT: a0 holds return value (if exists)
 */
static inline long
__internal_semihost(long n, long _a1)
{
  register long a0 __asm("a0") = n;
  register long a1 __asm("a1") = _a1;

  // riscv magic values for semihosting
  __asm volatile (
          ".option norvc;\t\n"
		  "slli    zero,zero,0x1f\t\n"
		  "ebreak\t\n"
		  "srai    zero,zero,0x7\t\n"
          ".option rvc;\t\n"
		: "+r"(a0) 
		: "r"(a1)
		);

#if 0
  if (a0 < 0)
    return __syscall_error (a0);
  else
    return a0;
#else
  return a0;
#endif
}


// roughly this is the last stage of printf:
// print a string until '\0'
void semihost_write0(const char *print_string);

int semihost_open(const char *name, int mode);

int semihost_close(int fd);

int semihost_read(int fd, uint8_t *buffer, int len);

int semihost_write(int fd, uint8_t *buffer, int len);

int semihost_seek(int fd, uint32_t pos);

int semihost_flen(int fd);

int semihost_exit(int code);

#endif
