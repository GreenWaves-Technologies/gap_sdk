
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_ACCESSORS__
#define __ARCHI_CLUSTER_CTRL_UNIT_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t cluster_ctrl_unit_eoc_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_EOC_OFFSET);
}

static inline void cluster_ctrl_unit_eoc_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_EOC_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_fetch_en_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_FETCH_EN_OFFSET);
}

static inline void cluster_ctrl_unit_fetch_en_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_FETCH_EN_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_clock_gate_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_CLOCK_GATE_OFFSET);
}

static inline void cluster_ctrl_unit_clock_gate_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_CLOCK_GATE_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_dbg_resume_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_DBG_RESUME_OFFSET);
}

static inline void cluster_ctrl_unit_dbg_resume_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_DBG_RESUME_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_dbg_halt_status_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_OFFSET);
}

static inline void cluster_ctrl_unit_dbg_halt_status_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_dbg_halt_mask_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_OFFSET);
}

static inline void cluster_ctrl_unit_dbg_halt_mask_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr0_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr1_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR1_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR1_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr2_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR2_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR2_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr3_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR3_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR3_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr4_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR4_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR4_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr5_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR5_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR5_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr6_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR6_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR6_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_boot_addr7_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_BOOT_ADDR7_OFFSET);
}

static inline void cluster_ctrl_unit_boot_addr7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_BOOT_ADDR7_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_OFFSET);
}

static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_OFFSET);
}

static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_OFFSET);
}

static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_OFFSET, value);
}


static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_get(uint32_t base)
{
    return GAP_READ(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_OFFSET);
}

static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_OFFSET, value);
}


#endif

#endif
