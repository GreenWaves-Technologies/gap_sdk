/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "sa_iss.hpp"

#include <stdexcept>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <utime.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>

#define IO_SIZE_MAX 32767
// #define IO_SIZE_MAX 500000
#define MAX_FNAME_LENGTH 1024

// Riscv SYScalls from libgloss/riscv/machine/syscall.h

#define RV_SYS_exit 93
#define RV_SYS_exit_group 94
#define RV_SYS_getpid 172
#define RV_SYS_kill 129
#define RV_SYS_read 63
#define RV_SYS_write 64
#define RV_SYS_open 1024
#define RV_SYS_openat 56
#define RV_SYS_close 57
#define RV_SYS_lseek 62
#define RV_SYS_brk 214
#define RV_SYS_link 1025
#define RV_SYS_unlink 1026
#define RV_SYS_mkdir 1030
#define RV_SYS_chdir 49
#define RV_SYS_getcwd 17
#define RV_SYS_stat 1038
#define RV_SYS_fstat 80
#define RV_SYS_lstat 1039
#define RV_SYS_fstatat 79
#define RV_SYS_access 1033
#define RV_SYS_faccessat 48
#define RV_SYS_pread 67
#define RV_SYS_pwrite 68
#define RV_SYS_uname 160
#define RV_SYS_getuid 174
#define RV_SYS_geteuid 175
#define RV_SYS_getgid 176
#define RV_SYS_getegid 177
#define RV_SYS_mmap 222
#define RV_SYS_munmap 215
#define RV_SYS_mremap 216
#define RV_SYS_time 1062
#define RV_SYS_getmainvars 2011
#define RV_SYS_rt_sigaction 134
#define RV_SYS_writev 66
#define RV_SYS_gettimeofday 169
#define RV_SYS_times 153
#define RV_SYS_fcntl 25
#define RV_SYS_getdents 61
#define RV_SYS_dup 23

static char IO_Buffer[IO_SIZE_MAX];

static void sim_io_error(iss_insn_t *At_PC, const char *Message, ...)

{
  va_list Args;

  va_start(Args, Message);
  fprintf(stderr, "Error At PC=%X:", At_PC->addr);
  vfprintf(stderr, Message, Args);
  fprintf(stderr, ". Aborting simulation\n");
  exit(0);
}

static void sim_io_eprintf(const char *Message, ...)

{
  va_list Args;

  va_start(Args, Message);
  vfprintf(stderr, Message, Args);
}

static int copy_name(iss_t *cpu, iss_insn_t *pc, unsigned int rv32_buff, char *Host_buff)

{
  iss_t *iss = (iss_t *)cpu;
  char c;
  int i = 0;

  while (1) {
    loadByte(iss, (rv32_buff+i), (uint8_t *)&c);
    if (c == 0) break;
    Host_buff[i++] = c;
    if (i == (MAX_FNAME_LENGTH-1)) {
          sim_io_error (pc, "Max file/path name length exceed");
      i--;
      break;
    }
  }
  Host_buff[i] = 0;
  return 0;
}


static int transpose_code(int code)

{
  int alt = 0;

  if ((code & 0x0) == 0x0) alt |= O_RDONLY;
  if ((code & 0x1) == 0x1) alt |= O_WRONLY;
  if ((code & 0x2) == 0x2) alt |= O_RDWR;
  if ((code & 0x8) == 0x8) alt |= O_APPEND;
  if ((code & 0x200) == 0x200) alt |= O_CREAT;
  if ((code & 0x400) == 0x400) alt |= O_TRUNC;
  if ((code & 0x800) == 0x800) alt |= O_EXCL;
  if ((code & 0x2000) == 0x2000) alt |= O_SYNC;
  if ((code & 0x4000) == 0x4000) alt |= O_NONBLOCK;
  if ((code & 0x8000) == 0x8000) alt |= O_NOCTTY;

  return alt;
}

void handle_syscall(iss_t *iss, iss_insn_t *pc)
{
  static int Trace = 0;

  int sys_fun = iss_get_reg(iss, 17);    // SYScall opcode in a7

  if (Trace) printf("SYSCall with sys_fun = %d\n", sys_fun);

  switch (sys_fun) {
    case RV_SYS_brk:
      {
        unsigned int head_ptr = iss_get_reg(iss, 10);
        unsigned int incr = iss_get_reg(iss, 11);
        unsigned int stack = iss_get_reg(iss, 12);
        unsigned int sp = iss_get_reg(iss, 2);
        unsigned int gp = iss_get_reg(iss, 3);
        printf("Mem request: Head: %8X, Incr: %8X, Stack: %8X, Sp: %8X, Gp: %8X, New Head: %8X, Gap Frame/Stack: %d\n",
          head_ptr, incr, stack, sp, gp, (head_ptr+incr), (int) (sp - (head_ptr+incr)));

      }
      break;
    case RV_SYS_open:
      {
        unsigned int filename = iss_get_reg(iss, 10);  // filename in a0
        int flags = transpose_code(iss_get_reg(iss, 11));  // flags in a1
        int mode = iss_get_reg(iss, 12);     // mode in a2
        int Res;
        char fname[MAX_FNAME_LENGTH];

        copy_name(iss, pc, filename, fname);
        Res = open(fname, flags, mode);
        iss_set_reg(iss, 10, Res);       // Ret in a0
      }
      break;
    case RV_SYS_exit:
      iss_exit(iss, iss_get_reg(iss, 10));
      break;
    case RV_SYS_close:
      {
        int fd = iss_get_reg(iss, 10);     // fd in a0
        int Status=0;
      
        if (!(fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO))
          Status = close(fd); // To avoid closing stdin/out/err when shutting down the simulated program ....
        iss_set_reg(iss, 10, Status);      // Ret in a0
      }
      break;
    case RV_SYS_write:
      {
        int fd = iss_get_reg(iss, 10);     // fd in a0
        unsigned int buffer = iss_get_reg(iss, 11);  // buffer in a1
        size_t Len = iss_get_reg(iss, 12);     // length in a2
        ssize_t Write_Len=0;
        unsigned int Off=0;
        unsigned int i;

/*
        if (Len > IO_SIZE_MAX)
          sim_io_error (pc, "Max IO buffer size exceeded (write), Length: %d, MaxLength= %d", (int) Len,  IO_SIZE_MAX);
        for (i = 0; i<Len; i++) IO_Buffer[i] = loadByte(iss, pc, (buffer+i));
        Write_Len = write(fd, IO_Buffer, Len);
*/
        while (Len != 0) {
          unsigned int L = (Len > IO_SIZE_MAX)?IO_SIZE_MAX:Len;
          for (i = 0; i<L; i++) loadByte(iss, (buffer+i+Off), (uint8_t *)&IO_Buffer[i]);
          Off += L;
          Write_Len += write(fd, IO_Buffer, L);
          Len -= L;
        }
        iss_set_reg(iss, 10, Write_Len);   // Ret in a0
      }
      break;
    case RV_SYS_read:
      {
        int fd = iss_get_reg(iss, 10);     // fd in a0
        unsigned int buffer = iss_get_reg(iss, 11);  // buffer in a1
        size_t Len = iss_get_reg(iss, 12);   // length in a2
        ssize_t Read_Len=0;
        unsigned int Off=0;
        int i;
/*
        if (Len > IO_SIZE_MAX)
          sim_io_error (pc, "Max IO buffer size exceeded (read), Length: %d, MaxLength= %d", (int) Len,  IO_SIZE_MAX);
        Read_Len = read(fd, IO_Buffer, Len);
        for (i = 0; i<Read_Len; i++) storeByte(iss, pc, (buffer+i), IO_Buffer[i]);
*/

        while (Len != 0) {
          unsigned int L = (Len > IO_SIZE_MAX)?IO_SIZE_MAX:Len;
          ssize_t L1 = read(fd, IO_Buffer, L);
          Read_Len += L1;
          for (i = 0; i<L1; i++) storeByte(iss, (buffer+i+Off), IO_Buffer[i]);
          Len -= L;
          Off += L;
        }
        iss_set_reg(iss, 10, Read_Len);    // Ret in a0

      }
      break;
    case RV_SYS_lseek:
      {
        int fd = iss_get_reg(iss, 10);
        off_t off = iss_get_reg(iss, 11);
        int whence = iss_get_reg(iss, 12);
        off_t Res = lseek(fd, off, whence);

        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_link:
      {
        unsigned int Old_Path = iss_get_reg(iss, 10);
        unsigned int New_Path = iss_get_reg(iss, 11);
        char Host_Old_Path[MAX_FNAME_LENGTH];
        char Host_New_Path[MAX_FNAME_LENGTH];
        int Res;

        copy_name(iss, pc, Old_Path, Host_Old_Path);
        copy_name(iss, pc, New_Path, Host_New_Path);
        Res = link(Host_Old_Path, Host_New_Path);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_unlink:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        char Host_Path[MAX_FNAME_LENGTH];
        int Res;

        copy_name(iss, pc, Path, Host_Path);
        Res = unlink(Host_Path);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_gettimeofday:
      {
        unsigned int tv_buf = iss_get_reg(iss, 10);
        struct timeval Tv;
        int Res;

        Res = gettimeofday(&Tv, NULL);
        storeWord(iss, tv_buf,     (int) Tv.tv_sec);
        storeWord(iss, (tv_buf+4), (int) (Tv.tv_sec>>32));
        storeWord(iss, (tv_buf+8), (int) Tv.tv_usec);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_stat:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        unsigned int Stat_Buf = iss_get_reg(iss, 11);
        char Host_Path[MAX_FNAME_LENGTH];
        struct stat B;
        int Res;

        copy_name(iss, pc, Path, Host_Path);
        Res = stat(Host_Path, &B);
        storeWord(iss, (Stat_Buf   ), (int)  B.st_dev);
        storeWord(iss, (Stat_Buf+4 ), (int) (B.st_dev>>32));

        storeWord(iss, (Stat_Buf+8 ), (int)  B.st_ino);
        storeWord(iss, (Stat_Buf+12), (int) (B.st_ino>>32));

        storeWord(iss, (Stat_Buf+16), (int)  B.st_mode);
        storeWord(iss, (Stat_Buf+20), (int)  B.st_nlink);
        storeWord(iss, (Stat_Buf+24), (int)  B.st_uid);
        storeWord(iss, (Stat_Buf+28), (int)  B.st_gid);

        storeWord(iss, (Stat_Buf+32), (int)  B.st_rdev);
        storeWord(iss, (Stat_Buf+36), (int) (B.st_rdev>>32));

        storeWord(iss, (Stat_Buf+40), (int)  B.st_size);
        storeWord(iss, (Stat_Buf+44), (int) (B.st_size>>32));

        storeWord(iss, (Stat_Buf+48), (int)   B.st_atime);
        storeWord(iss, (Stat_Buf+52), (int)  (B.st_atime>>32));
        // storeWord(iss, pc, (Stat_Buf+56), (int)   B.st_spare1);

        storeWord(iss, (Stat_Buf+60), (int)   B.st_mtime);
        storeWord(iss, (Stat_Buf+64), (int)  (B.st_mtime>>32));
        // storeWord(iss, pc, (Stat_Buf+68), (int)   B.st_spare2);

        storeWord(iss, (Stat_Buf+72), (int)   B.st_ctime);
        storeWord(iss, (Stat_Buf+76), (int)  (B.st_ctime>>32));
        // storeWord(iss, pc, (Stat_Buf+80), (int)   B.st_spare3);

        storeWord(iss, (Stat_Buf+84), (int)   B.st_blksize);
        storeWord(iss, (Stat_Buf+88), (int)   B.st_blocks);

        // storeWord(iss, pc, (Stat_Buf+92), (int)   B.st_spare4[0]);
        // storeWord(iss, pc, (Stat_Buf+96), (int)   B.st_spare4[1]);

        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_fstat:
      {
        int fd = iss_get_reg(iss, 10);
        unsigned int Stat_Buf = iss_get_reg(iss, 11);
        struct stat B;
        int Res;

        Res = fstat(fd, &B);
        storeWord(iss, (Stat_Buf   ), (int)  B.st_dev);
        storeWord(iss, (Stat_Buf+4 ), (int) (B.st_dev>>32));

        storeWord(iss, (Stat_Buf+8 ), (int)  B.st_ino);
        storeWord(iss, (Stat_Buf+12), (int) (B.st_ino>>32));

        storeWord(iss, (Stat_Buf+16), (int)  B.st_mode);
        storeWord(iss, (Stat_Buf+20), (int)  B.st_nlink);
        storeWord(iss, (Stat_Buf+24), (int)  B.st_uid);
        storeWord(iss, (Stat_Buf+28), (int)  B.st_gid);

        storeWord(iss, (Stat_Buf+32), (int)  B.st_rdev);
        storeWord(iss, (Stat_Buf+36), (int) (B.st_rdev>>32));

        storeWord(iss, (Stat_Buf+40), (int)  B.st_size);
        storeWord(iss, (Stat_Buf+44), (int) (B.st_size>>32));

        storeWord(iss, (Stat_Buf+48), (int)   B.st_atime);
        storeWord(iss, (Stat_Buf+52), (int)  (B.st_atime>>32));
        // storeWord(iss, pc, (Stat_Buf+56), (int)   B.st_spare1);

        storeWord(iss, (Stat_Buf+60), (int)   B.st_mtime);
        storeWord(iss, (Stat_Buf+64), (int)  (B.st_mtime>>32));
        // storeWord(iss, pc, (Stat_Buf+68), (int)   B.st_spare2);

        storeWord(iss, (Stat_Buf+72), (int)   B.st_ctime);
        storeWord(iss, (Stat_Buf+76), (int)  (B.st_ctime>>32));
        // storeWord(iss, pc, (Stat_Buf+80), (int)   B.st_spare3);

        storeWord(iss, (Stat_Buf+84), (int)   B.st_blksize);
        storeWord(iss, (Stat_Buf+88), (int)   B.st_blocks);

        // storeWord(iss, pc, (Stat_Buf+92), (int)   B.st_spare4[0]);
        // storeWord(iss, pc, (Stat_Buf+96), (int)   B.st_spare4[1]);

        iss_set_reg(iss, 10, Res);
      }
      break;
/*
    case RV_SYS_chmod:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        char Host_Path[MAX_FNAME_LENGTH];
        mode_t mode = iss_get_reg(iss, 11);
        int Res;

        copy_name(iss, pc, Path, Host_Path);
        Res = chmod(Host_Path, mode);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_fchmod:
      {
        int fd = iss_get_reg(iss, 10);
        mode_t mode = iss_get_reg(iss, 11);
        int Res;

        Res = fchmod(fd, mode);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_chown:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        char Host_Path[MAX_FNAME_LENGTH];
        uid_t uid = iss_get_reg(iss, 11);
        gid_t gid = iss_get_reg(iss, 12);
        int Res;

        copy_name(iss, pc, Path, Host_Path);
        Res = chown(Host_Path, uid, gid);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_fchown:
      {
        int fd = iss_get_reg(iss, 10);
        uid_t uid = iss_get_reg(iss, 11);
        gid_t gid = iss_get_reg(iss, 12);
        int Res;

        Res = fchown(fd, uid, gid);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_utime:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        char Host_Path[MAX_FNAME_LENGTH];
        unsigned int tv_buf = iss_get_reg(iss, 11);
        struct utimbuf Tv;
        int Res;
        unsigned long long Tmp;

        Tmp = (unsigned long long) loadWord(iss, pc, (tv_buf)) |
              ((unsigned long long) loadWord(iss, pc, (tv_buf+4))<<32);
        Tv.actime = (time_t) Tmp;
        Tmp = (unsigned long long) loadWord(iss, pc, (tv_buf+8)) |
              ((unsigned long long) loadWord(iss, pc, (tv_buf+12))<<32);
        Tv.modtime = (time_t) Tmp;
        copy_name(iss, pc, Path, Host_Path);
        Res = utime(Host_Path, &Tv);
        iss_set_reg(iss, 10, Res);
      }
      break;
    case RV_SYS_utimes:
      {
        unsigned int Path = iss_get_reg(iss, 10);
        char Host_Path[MAX_FNAME_LENGTH];
        unsigned int tv_buf = iss_get_reg(iss, 11);
        struct timeval Tv[2];
        int Res;
        unsigned long long Tmp;

        Tmp = (unsigned long long) loadWord(iss, pc, (tv_buf)) |
              ((unsigned long long) loadWord(iss, pc, (tv_buf+4))<<32);
        Tv[0].tv_sec  = (time_t) Tmp;
        Tv[0].tv_usec = (suseconds_t) loadWord(iss, pc, (tv_buf+8));
        Tmp = (unsigned long long) loadWord(iss, pc, (tv_buf+12)) |
              ((unsigned long long) loadWord(iss, pc, (tv_buf+16))<<32);
        Tv[1].tv_sec  = (time_t) Tmp;
        Tv[1].tv_usec = (suseconds_t) loadWord(iss, pc, (tv_buf+20));
        copy_name(iss, pc, Path, Host_Path);
        Res = utimes(Host_Path, Tv);
        iss_set_reg(iss, 10, Res);
      }
      break;
*/
    default:
      errno = EBADRQC;
      iss_set_reg(iss, 10, -1);
          sim_io_error (pc, "SYS call %X (%d) not supported", sys_fun, sys_fun);
      break;
  }
}