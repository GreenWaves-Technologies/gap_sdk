
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

#ifndef __ARCHI_APB_SOC_CTRL_REGFIELDS_ACCESSORS__
#define __ARCHI_APB_SOC_CTRL_REGFIELDS_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline void apb_soc_ctrl_info_nb_cl_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_INFO_OFFSET, GAP_BINSERT(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), value, 16, 0));
}

static inline uint32_t apb_soc_ctrl_info_nb_cl_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), 16, 0);
}

static inline int32_t apb_soc_ctrl_info_nb_cl_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), 16, 0);
}


static inline void apb_soc_ctrl_info_nb_cores_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_INFO_OFFSET, GAP_BINSERT(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), value, 16, 16));
}

static inline uint32_t apb_soc_ctrl_info_nb_cores_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), 16, 16);
}

static inline int32_t apb_soc_ctrl_info_nb_cores_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, APB_SOC_CTRL_INFO_OFFSET), 16, 16);
}


static inline void apb_soc_ctrl_fc_boot_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_FC_BOOT_OFFSET, value);
}

static inline uint32_t apb_soc_ctrl_fc_boot_addr_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_FC_BOOT_OFFSET);
}

static inline int32_t apb_soc_ctrl_fc_boot_addr_gets(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_FC_BOOT_OFFSET);
}


static inline void apb_soc_ctrl_fc_fetch_fc_fe_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_FC_FETCH_OFFSET, GAP_BINSERT(GAP_READ(base, APB_SOC_CTRL_FC_FETCH_OFFSET), value, 1, 0));
}

static inline uint32_t apb_soc_ctrl_fc_fetch_fc_fe_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, APB_SOC_CTRL_FC_FETCH_OFFSET), 1, 0);
}

static inline int32_t apb_soc_ctrl_fc_fetch_fc_fe_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, APB_SOC_CTRL_FC_FETCH_OFFSET), 1, 0);
}


static inline void apb_soc_ctrl_corestatus_status_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_CORESTATUS_OFFSET, value);
}

static inline uint32_t apb_soc_ctrl_corestatus_status_get(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET);
}

static inline int32_t apb_soc_ctrl_corestatus_status_gets(uint32_t base)
{
    return GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET);
}


static inline void apb_soc_ctrl_corestatus_eoc_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, APB_SOC_CTRL_CORESTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET), value, 1, 31));
}

static inline uint32_t apb_soc_ctrl_corestatus_eoc_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET), 1, 31);
}

static inline int32_t apb_soc_ctrl_corestatus_eoc_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, APB_SOC_CTRL_CORESTATUS_OFFSET), 1, 31);
}


#endif

#endif
