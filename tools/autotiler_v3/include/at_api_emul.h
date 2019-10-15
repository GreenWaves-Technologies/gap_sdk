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

#ifndef __AT__AT_API_EMUL_H__
#define __AT__AT_API_EMUL_H__


#include <stdlib.h>
#include <stdio.h>


/*
 * Utils
 */

#define AT_COREID()        0
#define AT_CLUSTERID()     0
#define AT_NCORE()         1
 
 
#define gap_fc_starttimer()
#define gap_fc_resethwtimer()
#define gap_fc_readhwtimer()    ((int) 0)

#define gap_cl_starttimer()
#define gap_cl_resethwtimer()
#define gap_cl_readhwtimer()    ((int) 0)



/*
 * Allocators
 */

#define AT_HYPERRAM_ALLOC(dev,size) malloc(size)

#define AT_HYPERRAM_FREE(dev,ptr,size) free(ptr)


#define AT_L2_ALLOC(dev,size) malloc(size)

#define AT_L2_FREE(dev,ptr,size) free(ptr)


#define AT_L1_ALLOC(dev,size) malloc(size)

#define AT_L1_FREE(dev,ptr,size) free(ptr)



/*
 * Hyperram
 */

#define AT_HYPERRAM_TYPE 0

typedef int AT_HYPERRAM_CONF_T;
typedef int AT_HYPERRAM_T;
typedef char *AT_HYPERRAM_EXT_ADDR_TYPE;
typedef char *AT_HYPERRAM_LOC_ADDR_TYPE;
typedef int AT_HYPERRAM_FC_EVENT;
typedef int AT_HYPERRAM_CL_EVENT;
typedef char *AT_HYPERRAM_POINTER;
typedef char *AT_HYPERRAM_INT_ADDR_TYPE;

#define AT_HYPERRAM_EXT2LOC 0
#define AT_HYPERRAM_LOC2EXT 1
 
#define AT_HYPERRAM_CONF_INIT(dev,type,name)

#define AT_HYPERRAM_OPEN(dev,conf,err) \
  do { *(err) = 0; } while (0)

#define AT_HYPERRAM_CLOSE(dev)

#define AT_HYPERRAM_FC_COPY(dev,ext,loc,size,dir,event) \
do { \
  int i; \
  char *To   = (dir==AT_HYPERRAM_EXT2LOC)?((char *) (loc)):((char *) (ext)); \
  char *From = (dir==AT_HYPERRAM_EXT2LOC)?((char *) (ext)):((char *) (loc)); \
 \
  if (dir==AT_HYPERRAM_EXT2LOC) { \
    if (1) __L3_Read += size; else __L2_Read += size; \
  } else { \
    if (1) __L3_Write += size; else __L2_Write += size; \
  } \
 \
  for (i=0; i<size; i++) To[i] = From[i]; \
} while (0)

#define AT_HYPERRAM_FC_COPY2D(dev,ext,loc,size,stride,length,dir,event) \
do { \
  int CopyIn = (dir==AT_HYPERRAM_EXT2LOC); \
  char *To   = CopyIn?((char *) (loc)):((char *) (ext)); \
  char *From = CopyIn?((char *) (ext)):((char *) (loc)); \
  int i, j, Chunk; \
 \
  if (dir==AT_HYPERRAM_EXT2LOC) { \
    if (1) __L3_Read += size; else __L2_Read += size; \
  } else { \
    if (1) __L3_Write += size; else __L2_Write += size; \
  } \
  for (Chunk=0; Chunk<size; Chunk+=length)  { \
    for (i=0; i<length; i++) To[i] = From[i]; \
      if (CopyIn) { \
      From += stride; To += length; \
    } else { \
      To += stride; From += length; \
    } \
  } \
} while (0)

#define AT_HYPERRAM_FC_WAIT(dev,event)

#define AT_HYPERRAM_CL_COPY(dev,ext,loc,size,dir,event) AT_HYPERRAM_FC_COPY(dev,ext,loc,size,dir,event)
#define AT_HYPERRAM_CL_COPY2D(dev,ext,loc,size,stride,length,dir,event) AT_HYPERRAM_FC_COPY2D(dev,ext,loc,size,stride,length,dir,event)
#define AT_HYPERRAM_CL_WAIT(dev,event)

/*
 * Hyperflash
 */

#define AT_HYPERFLASH_TYPE 1

typedef int AT_HYPERFLASH_CONF_T;
typedef int AT_HYPERFLASH_T;
typedef int AT_HYPERFLASH_EXT_ADDR_TYPE;
typedef int AT_HYPERFLASH_LOC_ADDR_TYPE;
typedef int AT_HYPERFLASH_EVENT;

#define AT_HYPERFLASH_EXT2LOC 0
#define AT_HYPERFLASH_LOC2EXT 1
 
#define AT_HYPERFLASH_CONF_INIT(dev,type,name)

#define AT_HYPERFLASH_OPEN(dev,conf,err)

#define AT_HYPERFLASH_CLOSE(dev)

#define AT_HYPERFLASH_COPY(dev,ext,loc,size,dir,event)

#define AT_HYPERFLASH_COPY2D(dev,ext,loc,size,stride,len,dir,event)

#define AT_HYPERFLASH_WAIT(dev,event)


/*
 * Hyperflash FS
 */

#define AT_HYPERFLASH_FS_TYPE 1

static inline void __at_hyperflash_fs_copy(FILE *file, unsigned int ext, void *loc, int size)
{
  fseek(file, ext, SEEK_SET);
  fread(loc, 1, size, file);
}

static inline void __at_hyperflash_fs_copy_2d(FILE *file, unsigned int ext, void *loc, int size, int stride, int length)
{
  int Chunk;
  for (Chunk=0; Chunk<size; Chunk+=length)
  {
    if (length > size)
      length = size;

    fseek(file, ext, SEEK_SET);
    fread(loc, 1, length, file);

    loc = ((char *)loc) + length;
    ext += stride;
  }
}

typedef int AT_HYPERFLASH_FS_CONF_T;
typedef FILE *AT_HYPERFLASH_FS_T;
typedef unsigned int AT_HYPERFLASH_FS_EXT_ADDR_TYPE;
typedef void *AT_HYPERFLASH_FS_INT_ADDR_TYPE;
typedef int AT_HYPERFLASH_FS_FC_EVENT;
typedef int AT_HYPERFLASH_FS_CL_EVENT;

#define AT_HYPERFLASH_FS_EXT2LOC 0
#define AT_HYPERFLASH_FS_LOC2EXT 1
 
#define AT_HYPERFLASH_FS_CONF_INIT(dev,type,name)

#define AT_HYPERFLASH_FS_OPEN(file,conf,filename,err) \
  do { *(file) = fopen(filename, "r"); *(err) = *(file) == NULL; } while(0)

#define AT_HYPERFLASH_FS_CLOSE(file) \
  fclose(*file)

#define AT_HYPERFLASH_FS_FC_COPY(file,ext,loc,size,dir,event) \
  __at_hyperflash_fs_copy(*(file), ext, loc, size)

#define AT_HYPERFLASH_FS_FC_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  __at_hyperflash_fs_copy_2d(*(file), ext, loc, size, stride, len)

#define AT_HYPERFLASH_FS_FC_WAIT(file,event)

#define AT_HYPERFLASH_FS_CL_COPY(file,ext,loc,size,dir,event) \
  __at_hyperflash_fs_copy(*(file), ext, loc, size)

#define AT_HYPERFLASH_FS_CL_COPY2D(file, dev,ext,loc,size,stride,len,dir,event) \
  __at_hyperflash_fs_copy_2d(*(file), ext, loc, size, stride, len)

#define AT_HYPERFLASH_FS_CL_WAIT(file,event)





/*
 * DMA
 */

typedef unsigned int AT_L2_EVENT;
typedef char *AT_L2_EXT_ADDR_TYPE;
typedef char *AT_L2_LOC_ADDR_TYPE;

typedef char *AT_L2_POINTER;
typedef char *AT_L1_POINTER;
typedef char *AT_L2_INT_ADDR_TYPE;

#define AT_L2_EXT2LOC 0
#define AT_L2_LOC2EXT 1
 
/* Internal counter for debug purpose, track the amount of transfered data to/from L2/L3 */
unsigned int __L3_Read, __L3_Write, __L2_Read, __L2_Write;

#define AT_L2_COPY(dev,ext,loc,size,dir,event) \
do { \
  int i; \
  char *To   = (dir==AT_L2_EXT2LOC)?((char *) (loc)):((char *) (ext)); \
  char *From = (dir==AT_L2_EXT2LOC)?((char *) (ext)):((char *) (loc)); \
 \
  if (dir==AT_L2_EXT2LOC) { \
    if (0) __L3_Read += size; else __L2_Read += size; \
  } else { \
    if (0) __L3_Write += size; else __L2_Write += size; \
  } \
 \
  for (i=0; i<size; i++) To[i] = From[i]; \
} while (0)

#define AT_L2_COPY2D(dev,ext,loc,size,stride,length,dir,event) \
do { \
  int CopyIn = (dir==AT_L2_EXT2LOC); \
  char *To   = CopyIn?((char *) (loc)):((char *) (ext)); \
  char *From = CopyIn?((char *) (ext)):((char *) (loc)); \
  int i, j, Chunk; \
 \
  if (dir==AT_L2_EXT2LOC) { \
    if (0) __L3_Read += size; else __L2_Read += size; \
  } else { \
    if (0) __L3_Write += size; else __L2_Write += size; \
  } \
  for (Chunk=0; Chunk<size; Chunk+=length)  { \
    for (i=0; i<length; i++) To[i] = From[i]; \
      if (CopyIn) { \
      From += stride; To += length; \
    } else { \
      To += stride; From += length; \
    } \
  } \
} while (0)

#define AT_L2_WAIT(dev,event)



/*
 * Team
 */

typedef void (*AT_FORK_FUN_TYPE)(void *); 
typedef void *AT_FORK_ARG_TYPE;

#define AT_FORK(nb_cores,entry,arg)
#define AT_FORK_WAIT()

#endif