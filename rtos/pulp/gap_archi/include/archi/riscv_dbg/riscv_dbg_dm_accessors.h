
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

#ifndef __ARCHI_RISCV_DBG_DM_ACCESSORS__
#define __ARCHI_RISCV_DBG_DM_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_data0_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA0_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_data0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DATA0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_data1_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA1_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_data1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DATA1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_dmcontrol_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_dmcontrol_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_dmstatus_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_dmstatus_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_hartinfo_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_hartinfo_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_HARTINFO_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_abstractcs_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_abstractcs_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_command_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_command_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_COMMAND_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_sbcs_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_sbcs_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf0_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF0_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf1_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF1_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf2_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF2_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf3_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF3_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF3_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf4_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF4_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF4_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf5_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF5_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF5_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf6_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF6_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF6_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_progbuf7_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF7_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_progbuf7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF7_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_sbaddress0_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBADDRESS0_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_sbaddress0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBADDRESS0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_dmcs2_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_dmcs2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t riscv_dbg_dm_sbdata0_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBDATA0_OFFSET);
}

static inline __attribute__((always_inline)) void riscv_dbg_dm_sbdata0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBDATA0_OFFSET, value);
}


#endif

#endif
