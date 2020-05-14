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
/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pmsis.h"
#include "pmsis/rtos/malloc/l2_malloc.h"

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))
#define Max(x, y)             (((x)>(y))?(x):(y))
/*******************************************************************************
 * A semi general purpose memory allocator based on the assumption that
 * when something is freed its size is known. The rationnal is to get rid of
 * the usual meta data overhead attached to traditional memory allocators.
 ******************************************************************************/

uint32_t __malloc_extern_init(malloc_t *a, void *_chunk, int32_t size)
{
    if (size)
    {
        uint32_t start_addr = ALIGN_UP((int32_t) _chunk, MIN_CHUNK_SIZE);
        malloc_chunk_t *chunk = pmsis_l2_malloc(sizeof(malloc_chunk_t));
        if (chunk == NULL)
        {
            return 1;
        }
        size = size - (start_addr - (uint32_t) _chunk);
        if (size > 0)
        {
            a->first_free = chunk;
            chunk->size = ALIGN_DOWN(size, MIN_CHUNK_SIZE);
            chunk->next = NULL;
            chunk->addr = start_addr;
        }
    }
    else
    {
        a->first_free = NULL;
        return 1;
    }
    return 0;
}

void __malloc_extern_deinit(malloc_t *a)
{
    pmsis_l2_malloc_free(a->first_free, sizeof(malloc_chunk_t));
}

void *__malloc_extern(malloc_t *a, int32_t size)
{
    malloc_chunk_t *pt = a->first_free, *prev = 0;

    size = ALIGN_UP(size, MIN_CHUNK_SIZE);

    while (pt && (pt->size < size))
    {
        prev = pt;
        pt = pt->next;
    }

    if (pt)
    {
        if (pt->size == size)
        {
            /*
             * Special case where the whole block disappears.
             * This special case is interesting to support when we allocate
             * aligned pages, to limit fragmentation.
             */
            if (prev)
            {
                prev->next = pt->next;
            }
            else
            {
                a->first_free = pt->next;
            }
            void *addr = (void *) pt->addr;
            pmsis_l2_malloc_free(pt, sizeof(malloc_chunk_t));
            return addr;
        }
        else
        {
            /*
             * The free block is bigger than needed. Return the end of
             * the block in order to just update the free block size.
             */
            void *result = (void *) ((int8_t *) pt->addr + pt->size - size);
            pt->size = pt->size - size;
            return result;
        }
    }
    else
    {
        return NULL;
    }
}

uint32_t __attribute__((noinline)) __malloc_extern_free(malloc_t *a, void *_chunk, int32_t size)
{
    malloc_chunk_t *chunk = NULL;
    malloc_chunk_t *next = a->first_free, *prev = 0;
    size = ALIGN_UP(size, MIN_CHUNK_SIZE);

    while (next && (next->addr < (uint32_t) _chunk))
    {
        prev = next;
        next = next->next;
    }

    if (next && (((int8_t *) _chunk + size) == (int8_t *) next->addr))
    {
        /* Coalesce with next */
        next->size = size + next->size;
        next->addr = (uint32_t) _chunk;
        chunk = next;
    }
    else
    {
        chunk = pmsis_l2_malloc(sizeof(malloc_chunk_t));
        if (chunk == NULL)
        {
            return 1;
        }
        chunk->size = size;
        chunk->next = next;
        chunk->addr = (uint32_t) _chunk;
    }

    if (prev)
    {
        if (((int8_t *) prev->addr + prev->size) == (int8_t *) _chunk)
        {
            /* Coalesce with previous */
            prev->size += chunk->size;
            prev->next = chunk->next;
            pmsis_l2_malloc_free(chunk, sizeof(malloc_chunk_t));
        }
        else
        {
            prev->next = chunk;
        }
    }
    else
    {
        a->first_free = chunk;
    }
    return 0;
}
