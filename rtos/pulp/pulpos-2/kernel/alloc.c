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
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include <string.h>
#include <stdio.h>


// Allocate at least 4 bytes to avoid misaligned accesses when parsing free blocks
// and actually 8 to fit free chunk header size and make sure a e free block to always have
// at least the size of the header.
// This also requires the initial chunk to be correctly aligned.
#define MIN_CHUNK_SIZE 8

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))

#ifndef Max
#define Max(x, y) (((x)>(y))?(x):(y))
#endif


void __attribute__((noinline)) pos_free(pos_alloc_t *a, void *_chunk, int size);


/*
  A semi general purpose memory allocator based on the assumption that when something is freed it's size is known.
  The rationnal is to get rid of the usual meta data overhead attached to traditionnal memory allocators.
*/

void pos_alloc_info(pos_alloc_t *a, int *_size, void **first_chunk, int *_nb_chunks)
{
    if (first_chunk)
        *first_chunk = a->first_free;

    if (_size || _nb_chunks)
    {
        int size = 0;
        int nb_chunks = 0;

        pos_alloc_chunk_t *pt = a->first_free;

        for (pt = a->first_free; pt; pt = pt->next)
        {
            size += pt->size;
            nb_chunks++;
        }

        if (_size)
            *_size = size;

        if (_nb_chunks)
            *_nb_chunks = nb_chunks;
    }
}

void pos_alloc_dump(pos_alloc_t *a)
{
    pos_alloc_chunk_t *pt = a->first_free;

    printf("======== Memory allocator state: ============\n");
    for (pt = a->first_free; pt; pt = pt->next)
    {
        printf("Free Block at %8X, size: %8x, Next: %8X ", (unsigned int) pt, pt->size, (unsigned int) pt->next);
        if (pt == pt->next)
        {
            printf(" CORRUPTED\n"); break;
        }
        else
            printf("\n");
    }
    printf("=============================================\n");
}



#if 0 //#ifdef ARCHI_MEMORY_POWER
static __attribute__((noinline)) void pos_alloc_account(pos_alloc_t *a, void *chunk, uint32_t size, int factor, uint32_t standby)
{
    uint32_t bank_size_log2 = a->bank_size_log2;
    uint32_t bank_size = 1<<bank_size_log2;
    uint32_t chunk_addr = ((uint32_t)chunk) - a->first_bank_addr;
    uint32_t addr = ((uint32_t)chunk_addr) & ~(bank_size - 1);
    uint32_t iter_size = addr + bank_size - ((uint32_t)chunk_addr);
    uint32_t bank = addr >> bank_size_log2;
    uint32_t banks_ctrl = 0;

    if (iter_size > size)
        iter_size = size;

    while (size)
    {
        uint32_t *count = standby ? &a->ret_count[bank] : &a->pwd_count[bank];

        if (factor == -1 && *count == bank_size)
        {
            banks_ctrl |= 1<<bank;
        }

        *count += iter_size*factor;

        if (*count == bank_size)
        {
            banks_ctrl |= 1<<bank;
        }

        addr += bank_size;
        size -= iter_size;
        iter_size = bank_size;
        bank++;

        if (iter_size > size)
            iter_size = size;
    }

    if (banks_ctrl)
        pos_alloc_power_ctrl(banks_ctrl, factor == -1 ? 1 : 0, standby);
}
#endif

static void pos_alloc_account_alloc(pos_alloc_t *a, void *chunk, int size)
{
#if 0 //#ifdef ARCHI_MEMORY_POWER
    if (a->track_pwd)
    {
        pos_alloc_account(a, chunk, size, -1, 0);
    }
#endif
}

static void pos_alloc_account_free(pos_alloc_t *a, void *chunk, int size)
{
#if 0 //#ifdef ARCHI_MEMORY_POWER
    if (a->track_pwd)
    {
        pos_alloc_account(a, chunk, size, 1, 0);
    }
#endif
}


#if 0
void pos_alloc_conf(pos_alloc_t *a, void *chunk, int size, pos_alloc_conf_e conf)
{
#ifdef ARCHI_MEMORY_POWER
    if (a->track_pwd)
    {

        if (conf == pos_alloc_CONF_POWER_RET || conf == pos_alloc_CONF_POWER_NON_RET)
            pos_alloc_account(a, chunk, size, conf == pos_alloc_CONF_POWER_RET ? 1 : -1, 1);
        else
            pos_alloc_account(a, chunk, size, conf == pos_alloc_CONF_POWER_DOWN ? 1 : -1, 0);
    }
#endif
}
#endif



void pos_alloc_init(pos_alloc_t *a, void *_chunk, int size)
{
    pos_alloc_chunk_t *chunk = (pos_alloc_chunk_t *)ALIGN_UP((int)_chunk, MIN_CHUNK_SIZE);
#ifdef ARCHI_MEMORY_POWER
    a->track_pwd = 0;
#endif
    a->first_free = chunk;
    size = size - ((int)chunk - (int)_chunk);
    if (size > 0)
    {
        chunk->size = ALIGN_DOWN(size, MIN_CHUNK_SIZE);
        chunk->next = NULL;
    }
}

void *pos_alloc(pos_alloc_t *a, int size)
{
    ALLOC_TRACE(POS_LOG_TRACE, "Allocating memory chunk (alloc: %p, size: 0x%8x)\n", a, size);

    pos_alloc_chunk_t *pt = a->first_free, *prev = 0;

    size = ALIGN_UP(size, MIN_CHUNK_SIZE);

    while (pt && (pt->size < size))
    {
        prev = pt; pt = pt->next;
    }

    if (pt)
    {
        if (pt->size == size)
        {
            // Special case where the whole block disappears
            // This special case is interesting to support when we allocate aligned pages, to limit fragmentation
            if (prev)
                prev->next = pt->next;
            else
                a->first_free = pt->next;
            ALLOC_TRACE(POS_LOG_TRACE, "Allocated memory chunk (alloc: %p, base: %p)\n", a, pt);
            // As this block was the full free block, the beginning of the block was already taken
            // for the header and was accounted as allocated, so don't account it twice.
            pos_alloc_account_alloc(a, (void *)(((uint32_t)pt) + sizeof(pos_alloc_chunk_t)), size - sizeof(pos_alloc_chunk_t));
            return (void *)pt;
        }
        else
        {
            // The free block is bigger than needed
            // Return the beginning of the block to be contiguous with statically allocated data
            // and simplify memory power management
            void *result = (void *)((char *)pt);
            pos_alloc_chunk_t *new_pt = (pos_alloc_chunk_t *)((char *)pt + size);
            new_pt->size = pt->size - size;
            new_pt->next = pt->next;

            if (prev)
                prev->next = new_pt;
            else
                a->first_free = new_pt;

            ALLOC_TRACE(POS_LOG_TRACE, "Allocated memory chunk (alloc: %p, base: %p)\n", a, result);
            // Don't account the metadata which were in the newly allocated block as they were
            // already accounted when the block was freed
            pos_alloc_account_alloc(a, (void *)((uint32_t)result+ sizeof(pos_alloc_chunk_t)), size - sizeof(pos_alloc_chunk_t));
            // Instead account the metadata of the new free block
            pos_alloc_account_alloc(a, new_pt, sizeof(pos_alloc_chunk_t));

            return result;
        }
    }
    else
    {
        ALLOC_TRACE(POS_LOG_TRACE, "Not enough memory to allocate\n");

        //rt_warning("Not enough memory to allocate\n");
        return NULL;
    }
}

void *pos_alloc_align(pos_alloc_t *a, int size, int align)
{

    if (align < (int)sizeof(pos_alloc_chunk_t))
        return pos_alloc(a, size);

    // As the user must give back the size of the allocated chunk when freeing it, we must allocate
    // an aligned chunk with exactly the right size
    // To do so, we allocate a bigger chunk and we free what is before and what is after

    // We reserve enough space to free the remaining room before and after the aligned chunk
    int size_align = size + align + sizeof(pos_alloc_chunk_t) * 2;
    unsigned int result = (unsigned int)pos_alloc(a, size_align);
    if (!result)
        return NULL;

    unsigned int result_align = (result + align - 1) & -align;
    unsigned int headersize = result_align - result;

    // In case we don't get an aligned chunk at first, we must free the room before the first aligned one
    if (headersize != 0)
    {

        // If we don't have enough room before the aligned chunk for freeing the header, take the next aligned one
        if (result_align - result < sizeof(pos_alloc_chunk_t))
            result_align += align;

        // Free the header
        pos_free(a, (void *)result, headersize);
    }

    // Now free what remains after
    pos_free(a, (unsigned char *)(result_align + size), size_align - headersize - size);

    return (void *)result_align;
}

void __attribute__((noinline)) pos_free(pos_alloc_t *a, void *_chunk, int size)
{
    ALLOC_TRACE(POS_LOG_TRACE, "Freeing memory chunk (alloc: %p, base: %p, size: 0x%8x)\n", a, _chunk, size);

    pos_alloc_chunk_t *chunk = (pos_alloc_chunk_t *)_chunk;
    pos_alloc_chunk_t *next = a->first_free, *prev = 0, *new;
    size = ALIGN_UP(size, MIN_CHUNK_SIZE);

    while (next && next < chunk)
    {
        prev = next; next = next->next;
    }

    if (((char *)chunk + size) == (char *)next)
    {
        /* Coalesce with next */
        chunk->size = size + next->size;
        chunk->next = next->next;
        // The header in the next free was considered allocated, we now have to free it as it will
        // stand in the one being freed.
        pos_alloc_account_free(a, next, sizeof(pos_alloc_chunk_t));
    }
    else
    {
        chunk->size = size;
        chunk->next = next;
    }

    if (prev)
    {
        if (((char *)prev + prev->size) == (char *)chunk)
        {
            /* Coalesce with previous */
            prev->size += chunk->size;
            prev->next = chunk->next;
            // The metadata will stand in the previous chunk, we can account the whole chunk
            pos_alloc_account_free(a, _chunk, size);
        }
        else
        {
            prev->next = chunk;
            // The metadata will stand in this block, we can account ony after the metadata
            pos_alloc_account_free(a, (void *)(((uint32_t)_chunk) + sizeof(pos_alloc_chunk_t)), size - sizeof(pos_alloc_chunk_t));
        }
    }
    else
    {
        a->first_free = chunk;
        // The metadata will stand in this block, we can account ony after the metadata
        pos_alloc_account_free(a, (void *)(((uint32_t)_chunk) + sizeof(pos_alloc_chunk_t)), size - sizeof(pos_alloc_chunk_t));
    }
}
