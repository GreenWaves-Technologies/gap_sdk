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

#ifndef __POS_IMPLEM_ALLOC_POOL_H__
#define __POS_IMPLEM_ALLOC_POOL_H__

#if defined(CHIP_HAS_L2)
#ifdef CHIP_HAS_L2_MULTI
#define POS_NB_ALLOC_L2 2
#else
#define POS_NB_ALLOC_L2 1
#endif
#endif

void pos_allocs_init();

void pos_alloc_init_l1(int cid, void *base, uint32_t size);

#endif
