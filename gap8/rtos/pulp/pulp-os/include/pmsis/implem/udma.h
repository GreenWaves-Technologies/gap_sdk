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

#ifndef __PMSIS_IMPLEM_UDMA_H__
#define __PMSIS_IMPLEM_UDMA_H__

#include "pmsis/data/udma.h"

extern RT_FC_TINY_DATA rt_udma_channel_t *__rt_udma_channels[ARCHI_NB_PERIPH*2];

extern void __rt_udma_channel_init(int channel_id, rt_udma_channel_t *channel);

extern void __rt_udma_copy_enqueue(pi_task_t *task, int channel_id, rt_udma_channel_t *channel, uint32_t buffer, uint32_t size, uint32_t cfg);

#endif