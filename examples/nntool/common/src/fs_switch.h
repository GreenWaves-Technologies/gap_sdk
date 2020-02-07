/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __FS_SWITCH_H__
#define __FS_SWITCH_H__

#include "Gap.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>

#ifdef __EMUL__
#include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    typedef int switch_fs_t;
    typedef int switch_file_t;
    #define PPM_HEADER 40
    #define __FS_INIT(__X) 1
    #define __FS_DEINIT(__X)
    #define __OPEN_READ(__FS, __NAME) open(__NAME, O_RDONLY, 0)
    #define __OPEN_WRITE(__FS, __NAME) open(__NAME, O_RDWR | O_CREAT, S_IRWXU)
    #define __CLOSE(__FD) close(__FD)
    #define __SEEK(__FD, __POS) lseek(__FD, __POS, SEEK_SET)
    #define __READ(__FD, __BUF, __LEN) read(__FD, __BUF, __LEN)
    #define __WRITE(__FD, __BUF, __LEN) write(__FD, __BUF, __LEN)
    #define __WRITEATCLUSTER(__FD, __POS, __BUF, __LEN) write(__FD, __BUF, __LEN)
    #define __int_ssize_t ssize_t
#else
    #include "bsp/fs.h"
    typedef pi_fs_file_t * switch_file_t;
    typedef struct {
        struct pi_fs_conf conf;
        struct pi_device fs;
    } switch_fs_t;

    static inline void switch_init_fs(switch_fs_t * fs) {
        pi_fs_conf_init(&fs->conf);
        fs->conf.type = PI_FS_HOST;
        pi_open_from_conf(&fs->fs, &fs->conf);
        pi_fs_mount(&fs->fs);
    }

    static inline int32_t switch_writeat_cl(pi_fs_file_t *file, uint32_t index, void *buffer,
                                        uint32_t size) {
        pi_cl_fs_req_t evt;
        pi_cl_fs_copy(file, index, buffer, size, 0, &evt);
        return pi_cl_fs_wait(&evt);
    }

    #define __FS_INIT(__FS)                             \
    do {                                                \
        pi_fs_conf_init(&__FS.conf);                    \
        __FS.conf.type = PI_FS_HOST;                    \
        pi_open_from_conf(&__FS.fs, &__FS.conf);        \
        pi_fs_mount(&__FS.fs);                          \
    } while (0)
    #define strerror(__x) "error unknown"
    #define __FS_DEINIT(__FS)  pi_fs_unmount(&__FS.fs)
    #define __OPEN_READ(__FS, __NAME) pi_fs_open(&__FS.fs, __NAME, PI_FS_FLAGS_READ)
    #define __OPEN_WRITE(__FS, __NAME) pi_fs_open(&__FS.fs, __NAME, PI_FS_FLAGS_WRITE)
    #define __CLOSE(__FD) pi_fs_close(__FD)
    #define __SEEK(__FD, __POS) pi_fs_seek(__FD, (__POS))
    #define __READ(__FD, __BUF, __LEN) pi_fs_read(__FD, (void *)(__BUF), (__LEN))
    #define __WRITE(__FD, __BUF, __LEN) pi_fs_write(__FD, (void *)(__BUF), (__LEN))
    #define __WRITEATCLUSTER(__FD, __POS, __BUF, __LEN) switch_writeat_cl(__FD, __POS, __BUF, __LEN)
    #define __int_ssize_t unsigned int
#endif

#endif
