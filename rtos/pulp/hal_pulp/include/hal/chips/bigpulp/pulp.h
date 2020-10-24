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

#ifndef __HAL_CHIPS_BIGPULP_PULP_H__
#define __HAL_CHIPS_BIGPULP_PULP_H__

#include "hal/riscv/riscv_v4.h"
#include "hal/eu/eu_v3.h"
#include "hal/dma/mchan_v6.h"
#include "hal/timer/timer_v2.h"
#include "hal/cluster_ctrl/cluster_ctrl_v2.h"
#include "hal/icache/icache_ctrl_v2.h"
#include "hal/apb_soc/apb_soc_v3.h"
#if PULP_CHIP != CHIP_BIGPULP_STANDALONE
#include "hal/mailbox/mailbox_v0.h"
#include "hal/tryx/tryx_v1.h"
#endif

#endif
