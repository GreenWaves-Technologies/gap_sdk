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
#include "pmsis/rtos/malloc/malloc_internal.h"

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))
#define Max(x, y)             (((x)>(y))?(x):(y))


/*******************************************************************************
 * A semi general purpose memory allocator based on the assumption that
 * when something is freed its size is known. The rationnal is to get rid of
 * the usual meta data overhead attached to traditional memory allocators.
 ******************************************************************************/

void __malloc_info(malloc_t *a, int32_t *_size, void **first_chunk, int32_t *_nb_chunks)
{
    if (first_chunk)
    {
        *first_chunk = a->first_free;
    }

    if (_size || _nb_chunks)
    {
        int32_t size = 0, nb_chunks = 0;

        malloc_chunk_t *pt = a->first_free;

        for (pt = a->first_free; pt; pt = pt->next)
        {
            size += pt->size;
            nb_chunks++;
        }

        if (_size)
        {
            *_size = size;
        }
        if (_nb_chunks)
        {
            *_nb_chunks = nb_chunks;
        }
    }
}

void __malloc_dump(malloc_t *a)
{
    malloc_chunk_t *pt = a->first_free;

    printf("======== Memory allocator state: ============\n");
    for (pt = a->first_free; pt; pt = pt->next)
    {
        printf("Free Block at %8lX, size: %8lx, Next: %8lX ",
               (uint32_t) pt, (uint32_t) pt->size, (uint32_t) pt->next);
        if (pt == pt->next)
        {
            printf(" CORRUPTED\n");
            break;
        }
        else
        {
            printf("\n");
        }
    }
    printf("=============================================\n");
}

void __malloc_init(malloc_t *a, void *_chunk, int32_t size)
{
    malloc_chunk_t *chunk = (malloc_chunk_t *) ALIGN_UP((int32_t) _chunk, MIN_CHUNK_SIZE);
    a->first_free = chunk;
    size = size - ((int32_t) chunk - (int32_t) _chunk);
    if (size > 0)
    {
        chunk->size = ALIGN_DOWN(size, MIN_CHUNK_SIZE);
        chunk->next = NULL;
    }
}

void *__malloc(malloc_t *a, int32_t size)
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
            return (void *) pt;
        }
        else
        {
            /*
             * The free block is bigger than needed. Return the end of
             * the block in order to just update the free block size.
             */
            void *result = (void *) ((int8_t *) pt + pt->size - size);
            pt->size = pt->size - size;
            return result;
        }
    }
    else
    {
        return NULL;
    }
}

void __attribute__((noinline)) __malloc_free(malloc_t *a, void *_chunk, int32_t size)
{
    malloc_chunk_t *chunk = (malloc_chunk_t *) _chunk;
    malloc_chunk_t *next = a->first_free, *prev = 0;
    size = ALIGN_UP(size, MIN_CHUNK_SIZE);

    while (next && (next < chunk))
    {
        prev = next;
        next = next->next;
    }

    if (((int8_t *) chunk + size) == (int8_t *) next)
    {
        /* Coalesce with next. */
        chunk->size = size + next->size;
        chunk->next = next->next;
    }
    else
    {
        chunk->size = size;
        chunk->next = next;
    }

    if (prev)
    {
        if (((int8_t *) prev + prev->size) == (int8_t *) chunk)
        {
            /* Coalesce with previous. */
            prev->size += chunk->size;
            prev->next = chunk->next;
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
}

void *__malloc_align(malloc_t *a, int32_t size, int32_t align)
{

    if (align < (int32_t) sizeof(malloc_chunk_t))
    {
        return __malloc(a, size);
    }

    /*
     * As the user must give back the size of the allocated chunk when freeing it,
     * we must allocate an aligned chunk with exactly the right size.
     * To do so, we allocate a bigger chunk and we free what is before and what is after.
    */
    /* We reserve enough space to free the remaining room before and after the aligned chunk. */
    int32_t size_align = size + align + sizeof(malloc_chunk_t) * 2;
    uint32_t result = (uint32_t) __malloc(a, size_align);
    if (!result)
    {
        return NULL;
    }

    uint32_t result_align = (result + align - 1) & -align;
    uint32_t headersize = result_align - result;

    /*
     * In case we don't get an aligned chunk at first, we must free the room
     * before the first aligned one.
     */
    if (headersize != 0)
    {
        /*
         * If we don't have enough room before the aligned chunk for freeing
         * the header, take the next aligned one.
         */
        if ((result_align - result) < sizeof(malloc_chunk_t))
        {
            result_align += align;
        }

        /* Free the header. */
        __malloc_free(a, (void *) result, headersize);
    }

    /* Now free what remains after. */
    __malloc_free(a, (uint8_t *) (result_align + size), (size_align - headersize - size));

    return (void *) result_align;
}
