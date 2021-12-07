/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
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


#ifndef __ARCHI_CHIPS_BIGPULP_PULP_H__
#define __ARCHI_CHIPS_BIGPULP_PULP_H__

#include "archi/chips/bigpulp/properties.h"

#include "archi/riscv/priv_1_9.h"
#include "archi/riscv/pcer_v1.h"
#include "archi/dma/mchan_v6.h"

#include "archi/chips/bigpulp/memory_map.h"
#include "archi/chips/bigpulp/apb_soc.h"
#include "archi/stdout/stdout_v3.h"
#if PULP_CHIP != CHIP_BIGPULP_STANDALONE
#include "archi/mailbox/mailbox_v0.h"
#endif

#endif
