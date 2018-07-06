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

#include "rt/rt_api.h"
#include <string.h>
#include <stdio.h>

// Allocate at least 4 bytes to avoid misaligned accesses when parsing free blocks 
// and actually 8 to fit free chunk header size and make sure a e free block to always have
// at least the size of the header.
// This also requires the initial chunk to be correctly aligned.
#define MIN_CHUNK_SIZE 8

#if defined(ARCHI_HAS_L1)
rt_alloc_t *__rt_alloc_l1;
#endif

#if defined(ARCHI_HAS_FC_TCDM)
rt_alloc_t __rt_alloc_fc_tcdm;
#endif

#if defined(ARCHI_HAS_L2)
rt_alloc_t __rt_alloc_l2[__RT_NB_ALLOC_L2];
#endif

#define ALIGN_UP(addr,size)   (((addr) + (size) - 1) & ~((size) - 1))
#define ALIGN_DOWN(addr,size) ((addr) & ~((size) - 1))

#define Max(x, y) (((x)>(y))?(x):(y))

/*
  A semi general purpose memory allocator based on the assumption that when something is freed it's size is known.
  The rationnal is to get rid of the usual meta data overhead attached to traditionnal memory allocators.
*/

void rt_user_alloc_info(rt_alloc_t *a, int *_size, void **first_chunk, int *_nb_chunks)
{
  if (first_chunk) *first_chunk = a->first_free;

  if (_size || _nb_chunks) {
    int size = 0;
    int nb_chunks = 0;

    rt_alloc_chunk_t *pt = a->first_free;

    for (pt = a->first_free; pt; pt = pt->next) {
      size += pt->size;
      nb_chunks++;
    }

    if (_size) *_size = size;
    if (_nb_chunks) *_nb_chunks = nb_chunks;
  }
}

void rt_user_alloc_dump(rt_alloc_t *a)
{
  rt_alloc_chunk_t *pt = a->first_free;

  printf("======== Memory allocator state: ============\n");
  for (pt = a->first_free; pt; pt = pt->next) {
    printf("Free Block at %8X, size: %8x, Next: %8X ", (unsigned int) pt, pt->size, (unsigned int) pt->next);
    if (pt == pt->next) {
      printf(" CORRUPTED\n"); break;
    } else printf("\n");
  }
  printf("=============================================\n");
}

void rt_user_alloc_init(rt_alloc_t *a, void *_chunk, int size)
{
  rt_alloc_chunk_t *chunk = (rt_alloc_chunk_t *)ALIGN_UP((int)_chunk, MIN_CHUNK_SIZE);
  a->first_free = chunk;
  size = size - ((int)chunk - (int)_chunk);
  if (size > 0) {
    chunk->size = ALIGN_DOWN(size, MIN_CHUNK_SIZE);
    chunk->next = NULL;
  }
}

void *rt_user_alloc(rt_alloc_t *a, int size)
{
  rt_trace(RT_TRACE_ALLOC, "Allocating memory chunk (alloc: %p, size: 0x%8x)\n", a, size);
  
  rt_alloc_chunk_t *pt = a->first_free, *prev = 0;

  size = ALIGN_UP(size, MIN_CHUNK_SIZE);

  while (pt && (pt->size < size)) { prev = pt; pt = pt->next; }

  if (pt) {
    if (pt->size == size) {
      // Special case where the whole block disappears
      // This special case is interesting to support when we allocate aligned pages, to limit fragmentation
      if (prev) prev->next = pt->next; else a->first_free = pt->next;
      return (void *)pt;
    } else {
      // The free block is bigger than needed
      // Return the end of the block in order to just update the free block size
      void *result = (void *)((char *)pt + pt->size - size);
      pt->size = pt->size - size;
      rt_trace(RT_TRACE_ALLOC, "Allocated memory chunk (alloc: %p, base: %p)\n", a, result);
      return result;
    }
  } else {
    rt_trace(RT_TRACE_ALLOC, "Not enough memory to allocate\n");
  
    //rt_warning("Not enough memory to allocate\n");
    return NULL;
  }
}

void *rt_user_alloc_align(rt_alloc_t *a, int size, int align)
{

  if (align < (int)sizeof(rt_alloc_chunk_t)) return rt_user_alloc(a, size);

  // As the user must give back the size of the allocated chunk when freeing it, we must allocate
  // an aligned chunk with exactly the right size
  // To do so, we allocate a bigger chunk and we free what is before and what is after

  // We reserve enough space to free the remaining room before and after the aligned chunk
  int size_align = size + align + sizeof(rt_alloc_chunk_t) * 2;
  unsigned int result = (unsigned int)rt_user_alloc(a, size_align);
  if (!result) return NULL;

  unsigned int result_align = (result + align - 1) & -align;
  unsigned int headersize = result_align - result;

  // In case we don't get an aligned chunk at first, we must free the room before the first aligned one
  if (headersize != 0) {

    // If we don't have enough room before the aligned chunk for freeing the header, take the next aligned one
    if (result_align - result < sizeof(rt_alloc_chunk_t)) result_align += align;

    // Free the header
    rt_user_free(a, (void *)result, headersize);
  }

  // Now free what remains after
  rt_user_free(a, (unsigned char *)(result_align + size), size_align - headersize - size);

  return (void *)result_align;
}

void __attribute__((noinline)) rt_user_free(rt_alloc_t *a, void *_chunk, int size)

{
  rt_trace(RT_TRACE_ALLOC, "Freeing memory chunk (alloc: %p, base: %p, size: 0x%8x)\n", a, _chunk, size);
  
  rt_alloc_chunk_t *chunk = (rt_alloc_chunk_t *)_chunk;
  rt_alloc_chunk_t *next = a->first_free, *prev = 0, *new;
  size = ALIGN_UP(size, MIN_CHUNK_SIZE);

  while (next && next < chunk) {
    prev = next; next = next->next; 
  }

  if (((char *)chunk + size) == (char *)next) {
    /* Coalesce with next */
    chunk->size = size + next->size;
    chunk->next = next->next;
  } else {
    chunk->size = size;
    chunk->next = next;
  }

  if (prev) {
    if (((char *)prev + prev->size) == (char *)chunk) {
      /* Coalesce with previous */
      prev->size += chunk->size;
      prev->next = chunk->next;
    } else {
      prev->next = chunk;
    }
  } else {
    a->first_free = chunk;
  }
}

void *rt_alloc(rt_alloc_e flags, int size)
{
#if defined(ARCHI_HAS_L1)
  if (flags >= RT_ALLOC_CL_DATA) return rt_user_alloc(rt_alloc_l1(flags - RT_ALLOC_CL_DATA), size);
  else
#endif
#if defined(ARCHI_HAS_FC_TCDM)
  if (flags == RT_ALLOC_FC_DATA) return rt_user_alloc(rt_alloc_fc_tcdm(), size);
  else
#endif
  {
#ifdef __RT_ALLOC_L2_MULTI
    for (int i=0; i<3; i++) {
      void *result = rt_user_alloc(&__rt_alloc_l2[flags], size);
      if (result != NULL) return result;
      flags++;
      if (flags == 3) flags = 0;
    }
    return NULL;
#else
    return rt_user_alloc(rt_alloc_l2(), size);
#endif
  }
}

void rt_free(rt_alloc_e flags, void *_chunk, int size)
{
#if defined(ARCHI_HAS_L1)
  if (flags >= RT_ALLOC_CL_DATA) return rt_user_free(rt_alloc_l1(flags - RT_ALLOC_CL_DATA), _chunk, size);
  else
#endif
#if defined(ARCHI_HAS_FC_TCDM)
  if (flags >= RT_ALLOC_FC_DATA) return rt_user_free(rt_alloc_fc_tcdm(), _chunk, size);
  else
#endif
  {
#ifdef __RT_ALLOC_L2_MULTI
    rt_alloc_t *a;
    unsigned int base = (unsigned int)_chunk;
    if (base < (unsigned int)rt_l2_priv0_base() + rt_l2_priv0_size()) a = &__rt_alloc_l2[0];
    else if (base < (unsigned int)rt_l2_priv1_base() + rt_l2_priv1_size()) a = &__rt_alloc_l2[1];
    else a = &__rt_alloc_l2[2];
    rt_user_free(a, _chunk, size);
#else
    rt_user_free(rt_alloc_l2(), _chunk, size);
#endif
  }
}

void *rt_alloc_align(rt_alloc_e flags, int size, int align)
{
#if defined(ARCHI_HAS_L1)
  if (flags >= RT_ALLOC_CL_DATA) return rt_user_alloc_align(rt_alloc_l1(flags - RT_ALLOC_CL_DATA), size, align);
  else
#endif
#if defined(ARCHI_HAS_FC_TCDM)
  if (flags >= RT_ALLOC_FC_DATA) return rt_user_alloc_align(rt_alloc_fc_tcdm(), size, align);
  else
#endif
  {
#ifdef __RT_ALLOC_L2_MULTI
    for (int i=0; i<3; i++) {
      void *result = rt_user_alloc_align(&__rt_alloc_l2[flags], size, align);
      if (result != NULL) return result;
      flags++;
      if (flags == 3) flags = 0;
    }
    return NULL;
#else
    return rt_user_alloc_align(rt_alloc_l2(), size, align);
#endif
  } 
}

#if defined(ARCHI_HAS_L1)
void __rt_alloc_init_l1(int cid)
{
  // TODO support multu cluster
  rt_trace(RT_TRACE_INIT, "Initializing L1 allocator (cluster: %d, base: 0x%8x, size: 0x%8x)\n", cid, (int)rt_l1_base(cid), rt_l1_size(cid));
  rt_user_alloc_init(&__rt_alloc_l1[cid], rt_l1_base(cid), rt_l1_size(cid));
}

void __rt_alloc_init_l1_for_fc(int cid)
{
  // TODO support multu cluster

  int size = sizeof(rt_alloc_t)*rt_nb_cluster();
  __rt_alloc_l1 = (rt_alloc_t *)rt_l1_base(cid);

  rt_trace(RT_TRACE_INIT, "Initializing L1 allocator (cluster: %d, base: 0x%8x, size: 0x%8x)\n", cid, (int)rt_l1_base(cid)+size, rt_l1_size(cid)-size);
  rt_user_alloc_init(&__rt_alloc_l1[cid], rt_l1_base(cid)+size, rt_l1_size(cid)-size);
}

#endif

void __rt_allocs_init()
{

#if defined(ARCHI_HAS_L2)
#if defined(__RT_ALLOC_L2_MULTI)

  rt_trace(RT_TRACE_INIT, "Initializing L2 private bank0 allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_l2_priv0_base(), rt_l2_priv0_size());
  rt_user_alloc_init(&__rt_alloc_l2[0], rt_l2_priv0_base(), rt_l2_priv0_size());

#ifdef ARCHI_HAS_L2_SCM
  rt_trace(RT_TRACE_INIT, "Initializing L2 scm allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_l2_scm_base(), rt_l2_scm_size());
  rt_user_alloc_init(&__rt_alloc_l2[3], rt_l2_scm_base(), rt_l2_scm_size());
#endif

  rt_trace(RT_TRACE_INIT, "Initializing L2 private bank1 allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_l2_priv1_base(), rt_l2_priv1_size());
  rt_user_alloc_init(&__rt_alloc_l2[1], rt_l2_priv1_base(), rt_l2_priv1_size());

  rt_trace(RT_TRACE_INIT, "Initializing L2 shared banks allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_l2_shared_base(), rt_l2_shared_size());
  rt_user_alloc_init(&__rt_alloc_l2[2], rt_l2_shared_base(), rt_l2_shared_size());

#else
  rt_trace(RT_TRACE_INIT, "Initializing L2 allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_l2_base(), rt_l2_size());
  rt_user_alloc_init(&__rt_alloc_l2[0], rt_l2_base(), rt_l2_size());
#endif
#endif

#if defined(ARCHI_HAS_FC_TCDM)
  rt_trace(RT_TRACE_INIT, "Initializing FC TCDM allocator (base: 0x%8x, size: 0x%8x)\n", (int)rt_fc_tcdm_base(), rt_fc_tcdm_size());
  rt_user_alloc_init(&__rt_alloc_fc_tcdm, rt_fc_tcdm_base(), rt_fc_tcdm_size());
#endif

#if defined(ARCHI_HAS_L1)
  // If the FC is running on cluster 0, initialize now the L1 allocator
  // as it is used for FC data
  if (rt_cluster_id() == 0)
  {
    // Special case where we need L1 memory for allocating allocators descriptors
    __rt_alloc_init_l1_for_fc(0);
  }
  else
  {
    __rt_alloc_l1 = rt_alloc(__RT_ALLOC_FC_DATA, sizeof(rt_alloc_t)*rt_nb_cluster());
  }
#endif
}


#if defined(ARCHI_HAS_CLUSTER)

void __rt_alloc_cluster_req(void *_req)
{
  rt_alloc_req_t *req = (rt_alloc_req_t *)_req;
  req->result = rt_alloc(req->flags, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

void __rt_free_cluster_req(void *_req)
{
  rt_free_req_t *req = (rt_free_req_t *)_req;
  rt_free(req->flags, req->chunk, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}


void rt_alloc_cluster(rt_alloc_e flags, int size, rt_alloc_req_t *req)
{
  req->flags = flags;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_alloc_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

void rt_free_cluster(rt_alloc_e flags, void *chunk, int size, rt_free_req_t *req)
{
  req->flags = flags;
  req->size = size;
  req->chunk = chunk;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_free_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}


#endif
