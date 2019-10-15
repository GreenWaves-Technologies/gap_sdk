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

#ifndef __RT_DATA_UDMA_H__
#define __RT_DATA_UDMA_H__

#ifndef LANGUAGE_ASSEMBLY

#include "pmsis/pmsis_types.h"

typedef struct {
  pi_task_t *pendings[2];
  pi_task_t *waitings_first;
  pi_task_t *waitings_last;
} rt_udma_channel_t;

#endif

#define RT_UDMA_CHANNEL_T_PENDINGS_0     0
#define RT_UDMA_CHANNEL_T_PENDINGS_1     4
#define RT_UDMA_CHANNEL_T_PENDINGS_FIRST 8
#define RT_UDMA_CHANNEL_T_PENDINGS_LAST  12

#endif
