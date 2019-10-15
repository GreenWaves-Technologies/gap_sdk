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
 * Copyright (C) 2018 GreenWaves Technologies
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

#include "pmsis.h"
#include <string.h>
#include <stdio.h>
#include "extern_alloc.h"

// Allocate at least 4 bytes to avoid misaligned accesses when parsing free blocks 
// and actually 8 to fit free chunk header size and make sure a e free block to always have
// at least the size of the header.
// This also requires the initial chunk to be correctly aligned.
#define MIN_CHUNK_SIZE 8

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))

#define Max(x, y) (((x)>(y))?(x):(y))

/*
  A semi general purpose memory allocator based on the assumption that when something is freed it's size is known.
  The rationnal is to get rid of the usual meta data overhead attached to traditionnal memory allocators.
*/

static inline void __free_chunk(alloc_chunk_extern_t *pt)
{
  pmsis_l2_malloc_free((void *)pt, sizeof(alloc_chunk_extern_t));
}

static inline alloc_chunk_extern_t *__alloc_chunk()
{
  return (alloc_chunk_extern_t *)pmsis_l2_malloc(sizeof(alloc_chunk_extern_t));
}

void extern_alloc_info(extern_alloc_t *a, int *_size, void **first_chunk, int *_nb_chunks)
{
  if (first_chunk) *first_chunk = a->first_free;

  if (_size || _nb_chunks) {
    int size = 0;
    int nb_chunks = 0;

    alloc_chunk_extern_t *pt = a->first_free;

    for (pt = a->first_free; pt; pt = pt->next) {
      size += pt->size;
      nb_chunks++;
    }

    if (_size) *_size = size;
    if (_nb_chunks) *_nb_chunks = nb_chunks;
  }
}

void extern_alloc_dump(extern_alloc_t *a)
{
  alloc_chunk_extern_t *pt = a->first_free;

  printf("======== Memory allocator state: ============\n");
  for (pt = a->first_free; pt; pt = pt->next) {
    printf("DUMP CHUNK %p ALLOC %p\n", pt, a);
    printf("Free Block at %8X, size: %5d, Next: %8X ", pt->addr, pt->size, (unsigned int) pt->next);
    if (pt == pt->next) {
      printf(" CORRUPTED\n"); break;
    } else printf("\n");
  }
  printf("=============================================\n");
}

int extern_alloc_init(extern_alloc_t *a, void *addr, int size)
{
  if (size)
  {
    unsigned int staaddr = ALIGN_UP((int)addr, MIN_CHUNK_SIZE);
    alloc_chunk_extern_t *chunk = __alloc_chunk();
    if (chunk == NULL) return -1;
    size = size - (staaddr - (unsigned int)addr);
    if (size > 0) {
      a->first_free = chunk;
      chunk->size = ALIGN_DOWN(size, MIN_CHUNK_SIZE);
      chunk->next = NULL;
      chunk->addr = staaddr;
    }
  }
  else
  {
    a->first_free = NULL;
  }
  return 0;
}



void extern_alloc_deinit(extern_alloc_t *a)
{
  __free_chunk(a->first_free);
}



int extern_alloc(extern_alloc_t *a, int size, void **chunk)
{
  alloc_chunk_extern_t *pt = a->first_free, *prev = 0;

  size = ALIGN_UP(size, MIN_CHUNK_SIZE);

  while (pt && (pt->size < size)) { prev = pt; pt = pt->next; }

  if (pt) {
    if (pt->size == size) {
      // Special case where the whole block disappears
      // This special case is interesting to support when we allocate aligned pages, to limit fragmentation
      if (prev) prev->next = pt->next; else a->first_free = pt->next;
      void *addr = (void *)pt->addr;
      __free_chunk(pt);
      *chunk = addr;
      return 0;
    } else {
      // The free block is bigger than needed
      // Return the end of the block in order to just update the free block size
      void *result = (void *)((char *)pt->addr + pt->size - size);
      pt->size = pt->size - size;
      *chunk = result;
      return 0;
    }
  } else {
    //warning("Not enough memory to allocate\n");
    *chunk = (void *)0xffffffff;
    return -1;
  }
}

int extern_alloc_align(extern_alloc_t *a, int size, int align, void **chunk)
{

  if (align < (int)sizeof(alloc_chunk_extern_t)) return extern_alloc(a, size, chunk);

  // As the user must give back the size of the allocated chunk when freeing it, we must allocate
  // an aligned chunk with exactly the right size
  // To do so, we allocate a bigger chunk and we free what is before and what is after

  // We reserve enough space to free the remaining room before and after the aligned chunk
  int size_align = size + align + sizeof(alloc_chunk_extern_t) * 2;
  unsigned int result;
  if (extern_alloc(a, size_align, (void **)&result))
    return -1;

  unsigned int result_align = (result + align - 1) & -align;
  unsigned int headersize = result_align - result;

  // In case we don't get an aligned chunk at first, we must free the room before the first aligned one
  if (headersize != 0) {

    // If we don't have enough room before the aligned chunk for freeing the header, take the next aligned one
    if (result_align - result < sizeof(alloc_chunk_extern_t)) result_align += align;

    // Free the header
    extern_free(a, headersize, (void *)result);
  }

  // Now free what remains after
  extern_free(a, size_align - headersize - size, (unsigned char *)(result_align + size));

  *chunk = (void *)result_align;
  return 0;
}

int __attribute__((noinline)) extern_free(extern_alloc_t *a, int size, void *addr)

{
  alloc_chunk_extern_t *chunk;
  alloc_chunk_extern_t *next = a->first_free, *prev = 0;
  size = ALIGN_UP(size, MIN_CHUNK_SIZE);

  while (next && next->addr < (unsigned int)addr) {
    prev = next; next = next->next; 
  }

  if (next && ((char *)addr + size) == (char *)next->addr) {
    /* Coalesce with next */
    next->size = size + next->size;
    next->addr = (unsigned int)addr;
    chunk = next;
  } else {
    chunk = __alloc_chunk();
    if (chunk == NULL) return -1;
    chunk->size = size;
    chunk->next = next;
    chunk->addr = (unsigned int)addr;
  }

  if (prev) {
    if (((char *)prev->addr + prev->size) == (char *)addr) {
      /* Coalesce with previous */
      prev->size += chunk->size;
      prev->next = chunk->next;
      __free_chunk(chunk);
    } else {
      prev->next = chunk;
    }
  } else {
    a->first_free = chunk;
  }

  return 0;
}
