/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __AT__AT_API_PMSIS_H__
#define __AT__AT_API_PMSIS_H__

#include "pmsis.h"
#include <bsp/bsp.h>
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash/spiflash.h"
#ifdef __GAP9__
#include "bsp/flash/mram.h"
#endif
#include "bsp/fs.h"



/*
 * Utils
 */

#define AT_COREID()        __builtin_pulp_CoreId()
#define AT_CLUSTERID()     __builtin_pulp_ClusterId()
#define AT_NCORE()         __builtin_pulp_CoreCount()

static inline void gap_fc_starttimer()
{
  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_fc_start();
}

static inline void gap_fc_resethwtimer()
{
  pi_perf_fc_reset();
}

static inline uint32_t gap_fc_readhwtimer()
{
  return pi_perf_fc_read(PI_PERF_CYCLES);
}

static inline void gap_cl_starttimer()
{
  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_cl_start();
}

static inline void gap_cl_resethwtimer()
{
  pi_perf_cl_reset();
}

static inline uint32_t gap_cl_readhwtimer()
{
  return pi_perf_cl_read(PI_PERF_CYCLES);
}



/*
 * Allocators
 */

#define AT_DEFAULTRAM_ALLOC(dev,size) ({ uint32_t ptr; int err = pi_ram_alloc((dev), &ptr, (size)); if (!err && ptr == 0) err = pi_ram_alloc((dev), &ptr, (size)); if (err) ptr = 0; ptr; })

#define AT_DEFAULTRAM_FREE(dev,ptr,size) pi_ram_free((dev), (ptr), (size))


#define AT_HYPERRAM_ALLOC(dev,size) ({ uint32_t ptr; int err = pi_ram_alloc((dev), &ptr, (size)); if (!err && ptr == 0) err = pi_ram_alloc((dev), &ptr, (size)); if (err) ptr = 0; ptr; })

#define AT_HYPERRAM_FREE(dev,ptr,size) pi_ram_free((dev), (ptr), (size))

#define AT_QSPIRAM_ALLOC(dev,size) ({ uint32_t ptr; int err = pi_ram_alloc((dev), &ptr, (size)); if (!err && ptr == 0) err = pi_ram_alloc((dev), &ptr, (size)); if (err) ptr = 0; ptr; })

#define AT_QSPIRAM_FREE(dev,ptr,size) pi_ram_free((dev), (ptr), (size))

#define AT_OSPIRAM_ALLOC(dev,size) ({ uint32_t ptr; int err = pi_ram_alloc((dev), &ptr, (size)); if (!err && ptr == 0) err = pi_ram_alloc((dev), &ptr, (size)); if (err) ptr = 0; ptr; })

#define AT_OSPIRAM_FREE(dev,ptr,size) pi_ram_free((dev), (ptr), (size))

#define AT_L2_ALLOC(dev,size) pmsis_l2_malloc(size)

#define AT_L2_FREE(dev,ptr,size) pmsis_l2_malloc_free((ptr), (size))


#define AT_L1_ALLOC(dev,size) pmsis_l1_malloc(size)

#define AT_L1_FREE(dev,ptr,size) pmsis_l1_malloc_free((ptr), (size))


/*
 * DEFAULT RAM: According to the BSP
 */

typedef struct pi_default_ram_conf AT_DEFAULTRAM_CONF_T;
typedef struct pi_device     AT_DEFAULTRAM_T;
typedef uint32_t             AT_DEFAULTRAM_EXT_ADDR_TYPE;
typedef void *               AT_DEFAULTRAM_LOC_ADDR_TYPE;
typedef pi_task_t            AT_DEFAULTRAM_FC_EVENT;
typedef pi_cl_ram_req_t         AT_DEFAULTRAM_CL_EVENT;
typedef uint32_t             AT_DEFAULTRAM_POINTER;
typedef char *               AT_DEFAULTRAM_INT_ADDR_TYPE;

#define AT_DEFAULTRAM_EXT2LOC 0
#define AT_DEFAULTRAM_LOC2EXT 1

#define AT_DEFAULTRAM_CONF_INIT(dev,type,name) \
  pi_default_ram_conf_init(dev)

#define AT_DEFAULTRAM_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_ram_open(dev); } while(0)

#define AT_DEFAULTRAM_CLOSE(dev) \
  pi_ram_close(dev)

#define AT_DEFAULTRAM_FC_COPY(dev,ext,loc,size,dir,event) \
  pi_ram_copy_async(dev, (AT_DEFAULTRAM_EXT_ADDR_TYPE)(ext), (AT_DEFAULTRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), pi_task_block(event))

#define AT_DEFAULTRAM_FC_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_ram_copy_2d_async(dev, (AT_DEFAULTRAM_EXT_ADDR_TYPE)(ext), (AT_DEFAULTRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), pi_task_block(event))

#define AT_DEFAULTRAM_FC_WAIT(dev,event) \
  pi_task_wait_on(event)

#define AT_DEFAULTRAM_CL_COPY(dev,ext,loc,size,dir,event) \
  pi_cl_ram_copy(dev, (AT_DEFAULTRAM_EXT_ADDR_TYPE)(ext), (AT_DEFAULTRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), (event))

#define AT_DEFAULTRAM_CL_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_ram_copy_2d(dev, (AT_DEFAULTRAM_EXT_ADDR_TYPE)(ext), (AT_DEFAULTRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), (event))

#define AT_DEFAULTRAM_CL_WAIT(dev,event) \
  pi_cl_ram_copy_wait(event)

/*
 * DEFAULT flash
 */

#define AT_DEFAULTFLASH_TYPE 1

typedef struct pi_default_flash_conf AT_DEFAULTFLASH_CONF_T;
typedef struct pi_device       AT_DEFAULTFLASH_T;
typedef uint32_t               AT_DEFAULTFLASH_EXT_ADDR_TYPE;
typedef void *                 AT_DEFAULTFLASH_LOC_ADDR_TYPE;
typedef pi_cl_ram_req_t           AT_DEFAULTFLASH_EVENT;

#define AT_DEFAULTFLASH_EXT2LOC 0
#define AT_DEFAULTFLASH_LOC2EXT 1

#define AT_DEFAULTFLASH_CONF_INIT(dev,type,name) \
  pi_default_flash_conf_init(dev)

#define AT_DEFAULTFLASH_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_flash_open(dev); } while(0)

#define AT_DEFAULTFLASH_CLOSE(dev) \
  pi_flash_close(dev)

// TODO not yet supported
#define AT_DEFAULTFLASH_COPY(dev,ext,loc,size,dir,event)

// TODO not yet supported
#define AT_DEFAULTFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

// TODO not yet supported
#define AT_DEFAULTFLASH_WAIT(dev,event)



/*
 * Default flash FS
 */

#define AT_DEFAULTFLASH_FS_TYPE 1

typedef struct pi_fs_conf AT_DEFAULTFLASH_FS_CONF_T;

typedef struct
{
  struct pi_device fs;
  struct pi_device default_flash;
  pi_fs_file_t *file;
} AT_DEFAULTFLASH_FS_T;

typedef unsigned int AT_DEFAULTFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_DEFAULTFLASH_FS_INT_ADDR_TYPE;
typedef pi_task_t AT_DEFAULTFLASH_FS_FC_EVENT;
typedef pi_cl_fs_req_t AT_DEFAULTFLASH_FS_CL_EVENT;

static inline void __at_default_flash_fs_open(AT_DEFAULTFLASH_FS_T *file, int is_write, struct pi_fs_conf *conf, const char *filename, int *err)
{
  struct pi_default_flash_conf default_flash_conf;
  pi_default_flash_conf_init(&default_flash_conf);
  pi_open_from_conf(&file->default_flash, &default_flash_conf);
  if (pi_flash_open(&file->default_flash))
  {
    *err = -1;
    return;
  }
  conf->flash = &file->default_flash;

  // Always force host FS for now in case we open for writing since readfs only supports reading
  if (is_write)
    conf->type = PI_FS_HOST;
  else
    conf->type = PI_FS_READ_ONLY;

#ifdef __FLASH_FS_SEMIHOST__
  conf->type = PI_FS_HOST;
#endif

  pi_open_from_conf(&file->fs, conf);
  if (pi_fs_mount(&file->fs))
  {
    pi_flash_close(&file->default_flash);
    *err = -1;
    return;
  }
  file->file = pi_fs_open(&file->fs, filename, is_write ? PI_FS_FLAGS_WRITE : 0);
  if (file->file == NULL)
  {
    pi_fs_unmount(&file->fs);
    pi_flash_close(&file->default_flash);
    *err = -1;
    return;
  }
  *err = 0;

  if (is_write)
    file->file->size = 4*1024*1024;
}

static inline void __at_default_flash_fs_close(AT_DEFAULTFLASH_FS_T *file)
{
  pi_fs_close(file->file);
  pi_fs_unmount(&file->fs);
  pi_flash_close(&file->default_flash);
}

#define AT_DEFAULTFLASH_FS_EXT2LOC 0
#define AT_DEFAULTFLASH_FS_LOC2EXT 1

#define AT_DEFAULTFLASH_FS_CONF_INIT(dev,type,name) \
  pi_fs_conf_init(dev)

#define AT_DEFAULTFLASH_FS_OPEN(file,conf,filename,err) \
  __at_default_flash_fs_open(file, 0, conf, filename, err)

#define AT_DEFAULTFLASH_FS_OPEN_WRITE(file,conf,filename,err) \
  __at_default_flash_fs_open(file, 1, conf, filename, err)

#define AT_DEFAULTFLASH_FS_OPEN_SET_SIZE(file, size) \
  file->file->size = size

#define AT_DEFAULTFLASH_FS_CLOSE(file) \
  __at_default_flash_fs_close(file)

#define AT_DEFAULTFLASH_FS_FC_COPY(fs,ext,loc,size,dir,event) \
  pi_fs_copy_async((fs)->file, ext, loc, size, !(dir), pi_task_block(event))

#define AT_DEFAULTFLASH_FS_FC_COPY2D(fs,ext,loc,size,stride,len,dir,event) \
  pi_fs_copy_2d_async((fs)->file, ext, loc, size, stride, len, !(dir), pi_task_block(event))

#define AT_DEFAULTFLASH_FS_FC_WAIT(file,event) \
  pi_task_wait_on(event)

#define AT_DEFAULTFLASH_FS_CL_COPY(fs,ext,loc,size,dir,event) \
  pi_cl_fs_copy((fs)->file, ext, loc, size, !(dir), event)

#define AT_DEFAULTFLASH_FS_CL_COPY2D(fs,ext,loc,size,stride,len,dir,event) \
  pi_cl_fs_copy_2d((fs)->file, ext, loc, size, stride, len, !(dir), event)

#define AT_DEFAULTFLASH_FS_CL_WAIT(file,event) \
  pi_cl_fs_wait(event)

/*
 * Hyperram
 */

#define AT_SPIRAM_TYPE 0

typedef struct pi_hyperram_conf AT_HYPERRAM_CONF_T;
typedef struct pi_device     AT_HYPERRAM_T;
typedef uint32_t             AT_HYPERRAM_EXT_ADDR_TYPE;
typedef void *               AT_HYPERRAM_LOC_ADDR_TYPE;
typedef pi_task_t            AT_HYPERRAM_FC_EVENT;
typedef pi_cl_ram_req_t         AT_HYPERRAM_CL_EVENT;
typedef uint32_t             AT_HYPERRAM_POINTER;
typedef char *               AT_HYPERRAM_INT_ADDR_TYPE;

#define AT_HYPERRAM_EXT2LOC 0
#define AT_HYPERRAM_LOC2EXT 1

#define AT_HYPERRAM_CONF_INIT(dev,type,name) \
  pi_hyperram_conf_init(dev)

#define AT_HYPERRAM_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_ram_open(dev); } while(0)

#define AT_HYPERRAM_CLOSE(dev) \
  pi_ram_close(dev)

#define AT_HYPERRAM_FC_COPY(dev,ext,loc,size,dir,event) \
  pi_ram_copy_async(dev, (AT_HYPERRAM_EXT_ADDR_TYPE)(ext), (AT_HYPERRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), pi_task_block(event))

#define AT_HYPERRAM_FC_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_ram_copy_2d_async(dev, (AT_HYPERRAM_EXT_ADDR_TYPE)(ext), (AT_HYPERRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), pi_task_block(event))

#define AT_HYPERRAM_FC_WAIT(dev,event) \
  pi_task_wait_on(event)

#define AT_HYPERRAM_CL_COPY(dev,ext,loc,size,dir,event) \
  pi_cl_ram_copy(dev, (AT_HYPERRAM_EXT_ADDR_TYPE)(ext), (AT_HYPERRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), (event))

#define AT_HYPERRAM_CL_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_ram_copy_2d(dev, (AT_HYPERRAM_EXT_ADDR_TYPE)(ext), (AT_HYPERRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), (event))

#define AT_HYPERRAM_CL_WAIT(dev,event) \
  pi_cl_ram_copy_wait(event)


/*
 * Hyperflash
 */

#define AT_HYPERFLASH_TYPE 1

typedef struct pi_hyperflash_conf AT_HYPERFLASH_CONF_T;
typedef struct pi_device       AT_HYPERFLASH_T;
typedef uint32_t               AT_HYPERFLASH_EXT_ADDR_TYPE;
typedef void *                 AT_HYPERFLASH_LOC_ADDR_TYPE;
typedef pi_cl_ram_req_t           AT_HYPERFLASH_EVENT;

#define AT_HYPERFLASH_EXT2LOC 0
#define AT_HYPERFLASH_LOC2EXT 1

#define AT_HYPERFLASH_CONF_INIT(dev,type,name) \
  pi_hyperflash_conf_init(dev)

#define AT_HYPERFLASH_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_flash_open(dev); } while(0)

#define AT_HYPERFLASH_CLOSE(dev) \
  pi_flash_close(dev)

// TODO not yet supported
#define AT_HYPERFLASH_COPY(dev,ext,loc,size,dir,event)

// TODO not yet supported
#define AT_HYPERFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

// TODO not yet supported
#define AT_HYPERFLASH_WAIT(dev,event)



/*
 * Hyperflash FS
 */

#define AT_HYPERFLASH_FS_TYPE 1

typedef struct pi_fs_conf AT_HYPERFLASH_FS_CONF_T;

typedef struct
{
  struct pi_device fs;
  struct pi_device hyperflash;
  pi_fs_file_t *file;
} AT_HYPERFLASH_FS_T;

typedef unsigned int AT_HYPERFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_HYPERFLASH_FS_INT_ADDR_TYPE;
typedef pi_task_t AT_HYPERFLASH_FS_FC_EVENT;
typedef pi_cl_fs_req_t AT_HYPERFLASH_FS_CL_EVENT;

static inline void __at_hyperflash_fs_open(AT_HYPERFLASH_FS_T *file, int is_write, struct pi_fs_conf *conf, const char *filename, int *err)
{
  struct pi_hyperflash_conf hyperflash_conf;
  pi_hyperflash_conf_init(&hyperflash_conf);
  pi_open_from_conf(&file->hyperflash, &hyperflash_conf);
  if (pi_flash_open(&file->hyperflash))
  {
    *err = -1;
    return;
  }
  conf->flash = &file->hyperflash;

  // Always force host FS for now in case we open for writing since readfs only supports reading
  if (is_write)
    conf->type = PI_FS_HOST;
  else
    conf->type = PI_FS_READ_ONLY;

#ifdef __FLASH_FS_SEMIHOST__
  conf->type = PI_FS_HOST;
#endif

  pi_open_from_conf(&file->fs, conf);
  if (pi_fs_mount(&file->fs))
  {
    pi_flash_close(&file->hyperflash);
    *err = -1;
    return;
  }
  file->file = pi_fs_open(&file->fs, filename, is_write ? PI_FS_FLAGS_WRITE : 0);
  if (file->file == NULL)
  {
    pi_fs_unmount(&file->fs);
    pi_flash_close(&file->hyperflash);
    *err = -1;
    return;
  }
  *err = 0;

  if (is_write)
    file->file->size = 4*1024*1024;
}

static inline void __at_hyperflash_fs_close(AT_HYPERFLASH_FS_T *file)
{
  pi_fs_close(file->file);
  pi_fs_unmount(&file->fs);
  pi_flash_close(&file->hyperflash);
}

#define AT_HYPERFLASH_FS_EXT2LOC 0
#define AT_HYPERFLASH_FS_LOC2EXT 1

#define AT_HYPERFLASH_FS_CONF_INIT(dev,type,name) \
  pi_fs_conf_init(dev)

#define AT_HYPERFLASH_FS_OPEN(file,conf,filename,err) \
  __at_hyperflash_fs_open(file, 0, conf, filename, err)

#define AT_HYPERFLASH_FS_OPEN_WRITE(file,conf,filename,err) \
  __at_hyperflash_fs_open(file, 1, conf, filename, err)

#define AT_HYPERFLASH_FS_OPEN_SET_SIZE(file, size) \
  file->file->size = size

#define AT_HYPERFLASH_FS_CLOSE(file) \
  __at_hyperflash_fs_close(file)

#define AT_HYPERFLASH_FS_FC_COPY(fs,ext,loc,size,dir,event) \
  pi_fs_copy_async((fs)->file, ext, loc, size, !(dir), pi_task_block(event))

#define AT_HYPERFLASH_FS_FC_COPY2D(fs,ext,loc,size,stride,len,dir,event) \
  pi_fs_copy_2d_async((fs)->file, ext, loc, size, stride, len, !(dir), pi_task_block(event))

#define AT_HYPERFLASH_FS_FC_WAIT(file,event) \
  pi_task_wait_on(event)

#define AT_HYPERFLASH_FS_CL_COPY(fs,ext,loc,size,dir,event) \
  pi_cl_fs_copy((fs)->file, ext, loc, size, !(dir), event)

#define AT_HYPERFLASH_FS_CL_COPY2D(fs,ext,loc,size,stride,len,dir,event) \
  pi_cl_fs_copy_2d((fs)->file, ext, loc, size, stride, len, !(dir), event)

#define AT_HYPERFLASH_FS_CL_WAIT(file,event) \
  pi_cl_fs_wait(event)


/*
 * Spiram
 */

#define AT_QSPIRAM_TYPE 0

typedef struct pi_spiram_conf   AT_QSPIRAM_CONF_T;
typedef struct pi_device        AT_QSPIRAM_T;
typedef uint32_t                AT_QSPIRAM_EXT_ADDR_TYPE;
typedef void *                  AT_QSPIRAM_LOC_ADDR_TYPE;
typedef pi_task_t               AT_QSPIRAM_FC_EVENT;
typedef pi_cl_ram_req_t         AT_QSPIRAM_CL_EVENT;
typedef uint32_t                AT_QSPIRAM_POINTER;
typedef char *                  AT_QSPIRAM_INT_ADDR_TYPE;

#define AT_QSPIRAM_EXT2LOC 0
#define AT_QSPIRAM_LOC2EXT 1

#define AT_QSPIRAM_CONF_INIT(dev,type,name) \
  pi_spiram_conf_init(dev)

#define AT_QSPIRAM_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_ram_open(dev); } while(0)

#define AT_QSPIRAM_CLOSE(dev) \
  pi_ram_close(dev)

#define AT_QSPIRAM_FC_COPY(dev,ext,loc,size,dir,event) \
  pi_ram_copy_async(dev, (AT_QSPIRAM_EXT_ADDR_TYPE)(ext), (AT_QSPIRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), pi_task_block(event))

#define AT_QSPIRAM_FC_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_ram_copy_2d_async(dev, (AT_QSPIRAM_EXT_ADDR_TYPE)(ext), (AT_QSPIRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), pi_task_block(event))

#define AT_QSPIRAM_FC_WAIT(dev,event) \
  pi_task_wait_on(event)

#define AT_QSPIRAM_CL_COPY(dev,ext,loc,size,dir,event) \
  pi_cl_ram_copy(dev, (AT_QSPIRAM_EXT_ADDR_TYPE)(ext), (AT_QSPIRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), (event))

#define AT_QSPIRAM_CL_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_ram_copy_2d(dev, (AT_QSPIRAM_EXT_ADDR_TYPE)(ext), (AT_QSPIRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), (event))

#define AT_QSPIRAM_CL_WAIT(dev,event) \
  pi_cl_ram_copy_wait(event)


/*
 * OctaSpiram
 */

#ifdef __GAP9__
#define AT_OSPIRAM_TYPE 0

typedef struct pi_aps25xxxn_conf   AT_OSPIRAM_CONF_T;
typedef struct pi_device        AT_OSPIRAM_T;
typedef uint32_t                AT_OSPIRAM_EXT_ADDR_TYPE;
typedef void *                  AT_OSPIRAM_LOC_ADDR_TYPE;
typedef pi_task_t               AT_OSPIRAM_FC_EVENT;
typedef pi_cl_ram_req_t         AT_OSPIRAM_CL_EVENT;
typedef uint32_t                AT_OSPIRAM_POINTER;
typedef char *                  AT_OSPIRAM_INT_ADDR_TYPE;

#define AT_OSPIRAM_EXT2LOC 0
#define AT_OSPIRAM_LOC2EXT 1

#define AT_OSPIRAM_CONF_INIT(dev,type,name) \
  pi_aps25xxxn_conf_init(dev)

#define AT_OSPIRAM_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_ram_open(dev); } while(0)

#define AT_OSPIRAM_CLOSE(dev) \
  pi_ram_close(dev)

#define AT_OSPIRAM_FC_COPY(dev,ext,loc,size,dir,event) \
  pi_ram_copy_async(dev, (AT_OSPIRAM_EXT_ADDR_TYPE)(ext), (AT_OSPIRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), pi_task_block(event))

#define AT_OSPIRAM_FC_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_ram_copy_2d_async(dev, (AT_OSPIRAM_EXT_ADDR_TYPE)(ext), (AT_OSPIRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), pi_task_block(event))

#define AT_OSPIRAM_FC_WAIT(dev,event) \
  pi_task_wait_on(event)

#define AT_OSPIRAM_CL_COPY(dev,ext,loc,size,dir,event) \
  pi_cl_ram_copy(dev, (AT_OSPIRAM_EXT_ADDR_TYPE)(ext), (AT_OSPIRAM_LOC_ADDR_TYPE)(loc), (size), !(dir), (event))

#define AT_OSPIRAM_CL_COPY2D(dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_ram_copy_2d(dev, (AT_OSPIRAM_EXT_ADDR_TYPE)(ext), (AT_OSPIRAM_LOC_ADDR_TYPE)(loc), (size), (stride), (len), !(dir), (event))

#define AT_OSPIRAM_CL_WAIT(dev,event) \
  pi_cl_ram_copy_wait(event)
#endif

/*
 * Spiflash
 */

#define AT_QSPIFLASH_TYPE 1

typedef struct pi_spiflash_conf     AT_QSPIFLASH_CONF_T;
typedef struct pi_device            AT_QSPIFLASH_T;
typedef uint32_t                    AT_QSPIFLASH_EXT_ADDR_TYPE;
typedef void *                      AT_QSPIFLASH_LOC_ADDR_TYPE;
typedef pi_cl_ram_req_t             AT_QSPIFLASH_EVENT;

#define AT_QSPIFLASH_EXT2LOC 0
#define AT_QSPIFLASH_LOC2EXT 1

#define AT_QSPIFLASH_CONF_INIT(dev,type,name) \
  pi_spiflash_conf_init(dev)

#define AT_QSPIFLASH_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_flash_open(dev); } while(0)

#define AT_QSPIFLASH_CLOSE(dev) \
  pi_flash_close(dev)

// TODO not yet supported
#define AT_QSPIFLASH_COPY(dev,ext,loc,size,dir,event)

// TODO not yet supported
#define AT_QSPIFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

// TODO not yet supported
#define AT_QSPIFLASH_WAIT(dev,event)


/*
 * OctaSpiflash
 */

#ifdef __GAP9__
#define AT_OSPIFLASH_TYPE 1

#if defined(CONFIG_ATXP032)
typedef struct pi_atxp032_conf      AT_OSPIFLASH_CONF_T;
#else
#if defined(CONFIG_MX25U51245G)
typedef struct pi_mx25u51245g_conf      AT_OSPIFLASH_CONF_T;
#endif
#endif
typedef struct pi_device            AT_OSPIFLASH_T;
typedef uint32_t                    AT_OSPIFLASH_EXT_ADDR_TYPE;
typedef void *                      AT_OSPIFLASH_LOC_ADDR_TYPE;
typedef pi_cl_ram_req_t             AT_OSPIFLASH_EVENT;

#define AT_OSPIFLASH_EXT2LOC 0
#define AT_OSPIFLASH_LOC2EXT 1

#define AT_OSPIFLASH_CONF_INIT(dev,type,name) \
  pi_spiflash_conf_init(dev)

#define AT_OSPIFLASH_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_flash_open(dev); } while(0)

#define AT_OSPIFLASH_CLOSE(dev) \
  pi_flash_close(dev)

// TODO not yet supported
#define AT_OSPIFLASH_COPY(dev,ext,loc,size,dir,event)

// TODO not yet supported
#define AT_OSPIFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

// TODO not yet supported
#define AT_OSPIFLASH_WAIT(dev,event)
#endif


/*
 * SPIflash FS
 */

#define AT_QSPIFLASH_FS_TYPE 1

typedef struct pi_fs_conf AT_QSPIFLASH_FS_CONF_T;

typedef struct
{
  struct pi_device fs;
  struct pi_device qspiflash;
  pi_fs_file_t *file;
} AT_QSPIFLASH_FS_T;

typedef unsigned int AT_QSPIFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_QSPIFLASH_FS_INT_ADDR_TYPE;
typedef pi_task_t AT_QSPIFLASH_FS_FC_EVENT;
typedef pi_cl_fs_req_t AT_QSPIFLASH_FS_CL_EVENT;

static inline void __at_qspiflash_fs_open(AT_QSPIFLASH_FS_T *file, int is_write, struct pi_fs_conf *conf, const char *filename, int *err)
{
  struct pi_spiflash_conf qspiflash_conf;
  pi_spiflash_conf_init(&qspiflash_conf);
  pi_open_from_conf(&file->qspiflash, &qspiflash_conf);
  if (pi_flash_open(&file->qspiflash))
  {
    *err = -1;
    return;
  }
  conf->flash = &file->qspiflash;
  if (is_write)
    conf->type = PI_FS_HOST;
  else
    conf->type = PI_FS_READ_ONLY;

  pi_open_from_conf(&file->fs, conf);
  if (pi_fs_mount(&file->fs))
  {
    pi_flash_close(&file->qspiflash);
    *err = -1;
    return;
  }
  file->file = pi_fs_open(&file->fs, filename, is_write ? PI_FS_FLAGS_WRITE : 0);
  if (file->file == NULL)
  {
    pi_fs_unmount(&file->fs);
    pi_flash_close(&file->qspiflash);
    *err = -1;
    return;
  }
  *err = 0;

  if (is_write)
    file->file->size = 4*1024*1024;
}

static inline void __at_qspiflash_fs_close(AT_QSPIFLASH_FS_T *file)
{
  pi_fs_close(file->file);
  pi_fs_unmount(&file->fs);
  pi_flash_close(&file->qspiflash);
}

#define AT_QSPIFLASH_FS_EXT2LOC 0
#define AT_QSPIFLASH_FS_LOC2EXT 1

#define AT_QSPIFLASH_FS_CONF_INIT(dev,type,name) \
  pi_fs_conf_init(dev)

#define AT_QSPIFLASH_FS_OPEN(file,conf,filename,err) \
  __at_qspiflash_fs_open(file, 0, conf, filename, err)

#define AT_QSPIFLASH_FS_OPEN_WRITE(file,conf,filename,err) \
  __at_qspiflash_fs_open(file, 1, conf, filename, err)

#define AT_QSPIFLASH_FS_OPEN_SET_SIZE(file, size) \
  file->file->size = size

#define AT_QSPIFLASH_FS_CLOSE(file) \
  __at_qspiflash_fs_close(file)

#define AT_QSPIFLASH_FS_FC_COPY(fs,ext,loc,size,dir,event) \
  pi_fs_copy_async((fs)->file, ext, loc, size, !(dir), pi_task_block(event))

#define AT_QSPIFLASH_FS_FC_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_fs_copy_2d_async(file->file, ext, loc, size, stride, len, !(dir), pi_task_block(event))

#define AT_QSPIFLASH_FS_FC_WAIT(file,event) \
  pi_task_wait_on(event)

#define AT_QSPIFLASH_FS_CL_COPY(fs,ext,loc,size,dir,event) \
  pi_cl_fs_copy((fs)->file, ext, loc, size, !(dir), event)

#define AT_QSPIFLASH_FS_CL_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_fs_copy_2d(file->file, ext, loc, size, stride, len, !(dir), event)

#define AT_QSPIFLASH_FS_CL_WAIT(file,event) \
  pi_cl_fs_wait(event)


/*
 * OctoSPIflash FS
 */

#ifdef __GAP9__
#define AT_OSPIFLASH_FS_TYPE 1

typedef struct pi_fs_conf AT_OSPIFLASH_FS_CONF_T;

typedef struct
{
  struct pi_device fs;
  struct pi_device ospiflash;
  pi_fs_file_t *file;
} AT_OSPIFLASH_FS_T;

typedef unsigned int AT_OSPIFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_OSPIFLASH_FS_INT_ADDR_TYPE;
typedef pi_task_t AT_OSPIFLASH_FS_FC_EVENT;
typedef pi_cl_fs_req_t AT_OSPIFLASH_FS_CL_EVENT;

#if (defined(CONFIG_ATXP032) || defined(CONFIG_MX25U51245G))
static inline void __at_ospiflash_fs_open(AT_OSPIFLASH_FS_T *file, int is_write, struct pi_fs_conf *conf, const char *filename, int *err)
{
  #if defined(CONFIG_ATXP032)
    struct pi_atxp032_conf flash_conf;
    pi_atxp032_conf_init(&flash_conf);
  #else
  #if defined(CONFIG_MX25U51245G)
    struct pi_mx25u51245g_conf flash_conf;
    pi_mx25u51245g_conf_init(&flash_conf);
  #endif
  #endif
  pi_open_from_conf(&file->ospiflash, &flash_conf);
  if (pi_flash_open(&file->ospiflash))
  {
    *err = -1;
    return;
  }
  conf->flash = &file->ospiflash;
  if (is_write)
    conf->type = PI_FS_HOST;
  else
    conf->type = PI_FS_READ_ONLY;

  pi_open_from_conf(&file->fs, conf);
  if (pi_fs_mount(&file->fs))
  {
    pi_flash_close(&file->ospiflash);
    *err = -1;
    return;
  }
  file->file = pi_fs_open(&file->fs, filename, is_write ? PI_FS_FLAGS_WRITE : 0);
  if (file->file == NULL)
  {
    pi_fs_unmount(&file->fs);
    pi_flash_close(&file->ospiflash);
    *err = -1;
    return;
  }
  *err = 0;

  if (is_write)
    file->file->size = 4*1024*1024;
}
#endif

static inline void __at_ospiflash_fs_close(AT_OSPIFLASH_FS_T *file)
{
  pi_fs_close(file->file);
  pi_fs_unmount(&file->fs);
  pi_flash_close(&file->ospiflash);
}

#define AT_OSPIFLASH_FS_EXT2LOC 0
#define AT_OSPIFLASH_FS_LOC2EXT 1

#define AT_OSPIFLASH_FS_CONF_INIT(dev,type,name) \
  pi_fs_conf_init(dev)

#define AT_OSPIFLASH_FS_OPEN(file,conf,filename,err) \
  __at_ospiflash_fs_open(file, 0, conf, filename, err)

#define AT_OSPIFLASH_FS_OPEN_WRITE(file,conf,filename,err) \
  __at_ospiflash_fs_open(file, 1, conf, filename, err)

#define AT_OSPIFLASH_FS_OPEN_SET_SIZE(file, size) \
  file->file->size = size

#define AT_OSPIFLASH_FS_CLOSE(file) \
  __at_ospiflash_fs_close(file)

#define AT_OSPIFLASH_FS_FC_COPY(fs,ext,loc,size,dir,event) \
  pi_fs_copy_async((fs)->file, ext, loc, size, !(dir), pi_task_block(event))

#define AT_OSPIFLASH_FS_FC_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_fs_copy_2d_async(file->file, ext, loc, size, stride, len, !(dir), pi_task_block(event))

#define AT_OSPIFLASH_FS_FC_WAIT(file,event) \
  pi_task_wait_on(event)

#define AT_OSPIFLASH_FS_CL_COPY(fs,ext,loc,size,dir,event) \
  pi_cl_fs_copy((fs)->file, ext, loc, size, !(dir), event)

#define AT_OSPIFLASH_FS_CL_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_fs_copy_2d(file->file, ext, loc, size, stride, len, !(dir), event)

#define AT_OSPIFLASH_FS_CL_WAIT(file,event) \
  pi_cl_fs_wait(event)
#endif

#ifdef __GAP9__

/*
 * EMRAM
 */

#define AT_EMRAMFLASH_TYPE 1

typedef struct pi_mram_conf         AT_EMRAMFLASH_CONF_T;
typedef struct pi_device            AT_EMRAMFLASH_T;
typedef uint32_t                    AT_EMRAMFLASH_EXT_ADDR_TYPE;
typedef void *                      AT_EMRAMFLASH_LOC_ADDR_TYPE;
typedef pi_cl_ram_req_t             AT_EMRAMFLASH_EVENT;

#define AT_EMRAMFLASH_EXT2LOC 0
#define AT_EMRAMFLASH_LOC2EXT 1

#define AT_EMRAMFLASH_CONF_INIT(dev,type,name) \
  pi_mram_conf_init(dev)

#define AT_EMRAMFLASH_OPEN(dev,conf,err) \
  do { pi_open_from_conf((dev), (conf)); *(err) = pi_flash_open(dev); } while(0)

#define AT_EMRAMFLASH_CLOSE(dev) \
  pi_flash_close(dev)

// TODO not yet supported
#define AT_EMRAMFLASH_COPY(dev,ext,loc,size,dir,event)

// TODO not yet supported
#define AT_EMRAMFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

// TODO not yet supported
#define AT_EMRAMFLASH_WAIT(dev,event)


/*
 * EMRAM FS
 */

#define AT_EMRAMFLASH_FS_TYPE 1

typedef struct pi_mram_conf AT_EMRAMFLASH_FS_CONF_T;

typedef struct
{
  struct pi_device fs;
  struct pi_device emramflash;
  pi_fs_file_t *file;
} AT_EMRAMFLASH_FS_T;

typedef unsigned int AT_EMRAMFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_EMRAMFLASH_FS_INT_ADDR_TYPE;
typedef pi_task_t AT_EMRAMFLASH_FS_FC_EVENT;
typedef pi_cl_fs_req_t AT_EMRAMFLASH_FS_CL_EVENT;

static inline void __at_emramflash_fs_open(AT_EMRAMFLASH_FS_T *file, int is_write, struct pi_fs_conf *conf, const char *filename, int *err)
{
  struct pi_mram_conf emramflash_conf;
  pi_mram_conf_init(&emramflash_conf);
  pi_open_from_conf(&file->emramflash, &emramflash_conf);
  if (pi_flash_open(&file->emramflash))
  {
    *err = -1;
    return;
  }
  conf->flash = &file->emramflash;
  if (is_write)
    conf->type = PI_FS_HOST;
  else
    conf->type = PI_FS_READ_ONLY;

  pi_open_from_conf(&file->fs, conf);
  if (pi_fs_mount(&file->fs))
  {
    pi_flash_close(&file->emramflash);
    *err = -1;
    return;
  }
  file->file = pi_fs_open(&file->fs, filename, is_write ? PI_FS_FLAGS_WRITE : 0);
  if (file->file == NULL)
  {
    pi_fs_unmount(&file->fs);
    pi_flash_close(&file->emramflash);
    *err = -1;
    return;
  }
  *err = 0;

  if (is_write)
    file->file->size = 3*512*1024;
}

static inline void __at_emramflash_fs_close(AT_EMRAMFLASH_FS_T *file)
{
  pi_fs_close(file->file);
  pi_fs_unmount(&file->fs);
  pi_flash_close(&file->emramflash);
}

#define AT_EMRAMFLASH_FS_EXT2LOC 0
#define AT_EMRAMFLASH_FS_LOC2EXT 1

#define AT_EMRAMFLASH_FS_CONF_INIT(dev,type,name) \
  pi_fs_conf_init(dev)

#define AT_EMRAMFLASH_FS_OPEN(file,conf,filename,err) \
  __at_emramflash_fs_open(file, 0, conf, filename, err)

#define AT_EMRAMFLASH_FS_OPEN_WRITE(file,conf,filename,err) \
  __at_emramflash_fs_open(file, 1, conf, filename, err)

#define AT_EMRAMFLASH_FS_OPEN_SET_SIZE(file, size) \
  file->file->size = size

#define AT_EMRAMFLASH_FS_CLOSE(file) \
  __at_emramflash_fs_close(file)

#define AT_EMRAMFLASH_FS_FC_COPY(fs,ext,loc,size,dir,event) \
  pi_fs_copy_async((fs)->file, ext, loc, size, !(dir), pi_task_block(event))

#define AT_EMRAMFLASH_FS_FC_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_fs_copy_2d_async(file->file, ext, loc, size, stride, len, !(dir), pi_task_block(event))

#define AT_EMRAMFLASH_FS_FC_WAIT(file,event) \
  pi_task_wait_on(event)

#define AT_EMRAMFLASH_FS_CL_COPY(fs,ext,loc,size,dir,event) \
  pi_cl_fs_copy((fs)->file, ext, loc, size, !(dir), event)

#define AT_EMRAMFLASH_FS_CL_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  pi_cl_fs_copy_2d(file->file, ext, loc, size, stride, len, !(dir), event)

#define AT_EMRAMFLASH_FS_CL_WAIT(file,event) \
  pi_cl_fs_wait(event)

#endif

/*
 * DMA
 */

typedef pi_cl_dma_cmd_t AT_L2_EVENT;
typedef uint32_t AT_L2_EXT_ADDR_TYPE;
typedef uint32_t AT_L2_LOC_ADDR_TYPE;

typedef char *AT_L2_POINTER;
typedef char *AT_L1_POINTER;
typedef char *AT_L2_INT_ADDR_TYPE;

#define AT_L2_EXT2LOC 0
#define AT_L2_LOC2EXT 1

#define AT_L2_COPY(dev,ext,loc,size,dir,event) pi_cl_dma_cmd((AT_L2_EXT_ADDR_TYPE)(ext), (AT_L2_LOC_ADDR_TYPE)(loc), (size), !(dir), (event));

#define AT_L2_COPY2D(dev,ext,loc,size,stride,length,dir,event) pi_cl_dma_cmd_2d((AT_L2_EXT_ADDR_TYPE)(ext), (AT_L2_LOC_ADDR_TYPE)(loc), (size), (stride), (length), !(dir), (event));

#define AT_L2_WAIT(dev,event) pi_cl_dma_cmd_wait(event);



/*
 * Team
 */

typedef void (*AT_FORK_FUN_TYPE)(void *);
typedef void *AT_FORK_ARG_TYPE;

#define AT_FORK(nb_cores,entry,arg) pi_cl_team_fork((nb_cores),(AT_FORK_FUN_TYPE)(entry),(AT_FORK_ARG_TYPE)(arg))
#define AT_FORK_CC(nb_cores,entry,arg) pi_cl_team_fork_cc((nb_cores),(AT_FORK_FUN_TYPE)(entry),(AT_FORK_ARG_TYPE)(arg))
#define AT_FORK_WAIT()              pi_cl_team_barrier()


#define AT_FORK_ASYNC(nb_cores,entry,arg) pi_team_offload((nb_cores),(AT_FORK_FUN_TYPE)(entry),(AT_FORK_ARG_TYPE)(arg))
#define AT_FORK_ASYNC_WAIT()              pi_team_offload_wait()
#define AT_YIELD()			  pi_cl_task_yield()

#endif
