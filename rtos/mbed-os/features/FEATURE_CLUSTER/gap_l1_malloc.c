/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "gap_l1_malloc.h"

#ifdef FEATURE_CLUSTER

malloc_t __l1_malloc_cluster;

void *L1_Malloc(int size)
{
    void * ptr = __malloc(&__l1_malloc_cluster, size + 0x4U);

    if ((uint32_t) ptr == 0x0)
        return (void *) 0x0;

    *(uint32_t *)(ptr) = size + 0x4U;

    void *user_ptr = (void *)(((uint32_t *)ptr)+1);

    return user_ptr;
}

void L1_MallocFree(void *_chunk)
{
    void *alloc_ptr = (void *)(((uint32_t *)_chunk)-1);
    uint32_t size = *((uint32_t *)alloc_ptr);

    __malloc_free(&__l1_malloc_cluster, alloc_ptr, size);
}

void *L1_MallocAlign(int size, int align)
{
    return __malloc_align(&__l1_malloc_cluster, size, align);
}

void L1_MallocInit()
{
    __malloc_init(&__l1_malloc_cluster, (void*)&__heapsram_start, (uint32_t)&__heapsram_size);
}

#endif
