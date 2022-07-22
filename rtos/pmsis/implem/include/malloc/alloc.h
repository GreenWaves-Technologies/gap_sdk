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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#pragma once

#include "malloc/alloc_types.h"
#include "malloc/cl_alloc.h"

void pos_allocs_init();

void pos_alloc_info(pos_alloc_t *a, int *_size, void **first_chunk, int *_nb_chunks);

void pos_alloc_dump(pos_alloc_t *a);

void pos_alloc_init(pos_alloc_t *a, void *_chunk, int size);

void *pos_alloc(pos_alloc_t *a, int size);

void *pos_alloc_align(pos_alloc_t *a, int size, int align);

void __attribute__((noinline)) pos_free(pos_alloc_t *a, void *_chunk, int size);

#ifdef CHIP_MEMORY_POWER
void pos_alloc_account_free(pos_alloc_t *a, void *chunk, int size);

void pos_alloc_account(pos_alloc_t *a, void *chunk, uint32_t size, int factor, uint32_t standby);

#endif

