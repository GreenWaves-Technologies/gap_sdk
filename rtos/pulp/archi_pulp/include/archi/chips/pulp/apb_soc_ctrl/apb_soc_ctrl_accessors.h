
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_APB_SOC_CTRL_ACCESSORS__
#define __ARCHI_APB_SOC_CTRL_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t apb_soc_ctrl_info_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET);
}

static inline void apb_soc_ctrl_info_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_INFO_OFFSET, value);
}


static inline uint32_t apb_soc_ctrl_fc_boot_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_FC_BOOT_OFFSET);
}

static inline void apb_soc_ctrl_fc_boot_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_FC_BOOT_OFFSET, value);
}


static inline uint32_t apb_soc_ctrl_fc_fetch_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_FC_FETCH_OFFSET);
}

static inline void apb_soc_ctrl_fc_fetch_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_FC_FETCH_OFFSET, value);
}


static inline uint32_t apb_soc_ctrl_corestatus_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET);
}

static inline void apb_soc_ctrl_corestatus_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_CORESTATUS_OFFSET, value);
}


#endif

#endif
