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

#ifndef __RT_RT_EXTERN_ALLOC_H__
#define __RT_RT_EXTERN_ALLOC_H__

#include "rt/rt_data.h"


/// @cond IMPLEM

int rt_extern_alloc_init(rt_extern_alloc_t *a, void *_chunk, int size);

void rt_extern_alloc_deinit(rt_extern_alloc_t *a);

void *rt_extern_alloc(rt_extern_alloc_t *a, int size);

int rt_extern_free(rt_extern_alloc_t *a, void *_chunk, int size);

void *rt_extern_alloc_align(rt_extern_alloc_t *a, int size, int align);

void rt_extern_alloc_info(rt_extern_alloc_t *a, int *size, void **first_chunk, int *nb_chunks);

void rt_extern_alloc_dump(rt_extern_alloc_t *a);


/// @endcond



#endif
