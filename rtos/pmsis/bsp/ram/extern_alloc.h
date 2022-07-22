/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __EXTERN_ALLOC_H__
#define __EXTERN_ALLOC_H__


typedef struct alloc_block_extern_s {
  int                      size;
  struct alloc_block_extern_s *next;
  unsigned int             addr;
} alloc_chunk_extern_t;

typedef struct {
  alloc_chunk_extern_t *first_free;
} extern_alloc_t;


/// @cond IMPLEM

int extern_alloc_init(extern_alloc_t *a, void *_chunk, int size);

void extern_alloc_deinit(extern_alloc_t *a);

int extern_alloc(extern_alloc_t *a, int size, void **chunk);

int extern_free(extern_alloc_t *a, int size, void *chunk);

int extern_alloc_align(extern_alloc_t *a, int size, int align, void **chunk);

void extern_alloc_info(extern_alloc_t *a, int *size, void **first_chunk, int *nb_chunks);

void extern_alloc_dump(extern_alloc_t *a);


/// @endcond



#endif
