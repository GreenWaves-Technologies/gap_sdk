
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

#ifndef __ARCHI_RISCV_DBG_DM_REGFIELDS_ACCESSORS__
#define __ARCHI_RISCV_DBG_DM_REGFIELDS_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline void riscv_dbg_dm_data0_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DATA0_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_data0_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA0_OFFSET);
}

static inline int32_t riscv_dbg_dm_data0_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA0_OFFSET);
}


static inline void riscv_dbg_dm_data1_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DATA1_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_data1_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA1_OFFSET);
}

static inline int32_t riscv_dbg_dm_data1_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_DATA1_OFFSET);
}


static inline void riscv_dbg_dm_dmcontrol_dmactive_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 0));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_dmactive_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 0);
}

static inline int32_t riscv_dbg_dm_dmcontrol_dmactive_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 0);
}


static inline void riscv_dbg_dm_dmcontrol_ndmreset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 1));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_ndmreset_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 1);
}

static inline int32_t riscv_dbg_dm_dmcontrol_ndmreset_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 1);
}


static inline void riscv_dbg_dm_dmcontrol_clrresethaltreq_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 2));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_clrresethaltreq_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 2);
}

static inline int32_t riscv_dbg_dm_dmcontrol_clrresethaltreq_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 2);
}


static inline void riscv_dbg_dm_dmcontrol_setresethaltreq_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 3));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_setresethaltreq_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 3);
}

static inline int32_t riscv_dbg_dm_dmcontrol_setresethaltreq_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 3);
}


static inline void riscv_dbg_dm_dmcontrol_hartselhi_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 10, 6));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_hartselhi_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 10, 6);
}

static inline int32_t riscv_dbg_dm_dmcontrol_hartselhi_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 10, 6);
}


static inline void riscv_dbg_dm_dmcontrol_hartsello_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 10, 16));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_hartsello_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 10, 16);
}

static inline int32_t riscv_dbg_dm_dmcontrol_hartsello_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 10, 16);
}


static inline void riscv_dbg_dm_dmcontrol_hasel_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 26));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_hasel_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 26);
}

static inline int32_t riscv_dbg_dm_dmcontrol_hasel_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 26);
}


static inline void riscv_dbg_dm_dmcontrol_ackhavereset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 28));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_ackhavereset_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 28);
}

static inline int32_t riscv_dbg_dm_dmcontrol_ackhavereset_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 28);
}


static inline void riscv_dbg_dm_dmcontrol_hartreset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 29));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_hartreset_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 29);
}

static inline int32_t riscv_dbg_dm_dmcontrol_hartreset_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 29);
}


static inline void riscv_dbg_dm_dmcontrol_resumereq_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 30));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_resumereq_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 30);
}

static inline int32_t riscv_dbg_dm_dmcontrol_resumereq_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 30);
}


static inline void riscv_dbg_dm_dmcontrol_haltreq_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCONTROL_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), value, 1, 31));
}

static inline uint32_t riscv_dbg_dm_dmcontrol_haltreq_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 31);
}

static inline int32_t riscv_dbg_dm_dmcontrol_haltreq_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCONTROL_OFFSET), 1, 31);
}


static inline void riscv_dbg_dm_dmstatus_version_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 4, 0));
}

static inline uint32_t riscv_dbg_dm_dmstatus_version_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 4, 0);
}

static inline int32_t riscv_dbg_dm_dmstatus_version_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 4, 0);
}


static inline void riscv_dbg_dm_dmstatus_confstrptrvalid_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 4));
}

static inline uint32_t riscv_dbg_dm_dmstatus_confstrptrvalid_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 4);
}

static inline int32_t riscv_dbg_dm_dmstatus_confstrptrvalid_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 4);
}


static inline void riscv_dbg_dm_dmstatus_hasresethaltreq_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 5));
}

static inline uint32_t riscv_dbg_dm_dmstatus_hasresethaltreq_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 5);
}

static inline int32_t riscv_dbg_dm_dmstatus_hasresethaltreq_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 5);
}


static inline void riscv_dbg_dm_dmstatus_authbusy_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 6));
}

static inline uint32_t riscv_dbg_dm_dmstatus_authbusy_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 6);
}

static inline int32_t riscv_dbg_dm_dmstatus_authbusy_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 6);
}


static inline void riscv_dbg_dm_dmstatus_authenticated_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 7));
}

static inline uint32_t riscv_dbg_dm_dmstatus_authenticated_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 7);
}

static inline int32_t riscv_dbg_dm_dmstatus_authenticated_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 7);
}


static inline void riscv_dbg_dm_dmstatus_anyhalted_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 8));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anyhalted_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 8);
}

static inline int32_t riscv_dbg_dm_dmstatus_anyhalted_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 8);
}


static inline void riscv_dbg_dm_dmstatus_allhalted_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 9));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allhalted_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 9);
}

static inline int32_t riscv_dbg_dm_dmstatus_allhalted_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 9);
}


static inline void riscv_dbg_dm_dmstatus_anyrunning_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 10));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anyrunning_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 10);
}

static inline int32_t riscv_dbg_dm_dmstatus_anyrunning_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 10);
}


static inline void riscv_dbg_dm_dmstatus_allrunning_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 11));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allrunning_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 11);
}

static inline int32_t riscv_dbg_dm_dmstatus_allrunning_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 11);
}


static inline void riscv_dbg_dm_dmstatus_anyunavail_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 12));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anyunavail_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 12);
}

static inline int32_t riscv_dbg_dm_dmstatus_anyunavail_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 12);
}


static inline void riscv_dbg_dm_dmstatus_allunavail_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 13));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allunavail_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 13);
}

static inline int32_t riscv_dbg_dm_dmstatus_allunavail_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 13);
}


static inline void riscv_dbg_dm_dmstatus_anynonexistent_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 14));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anynonexistent_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 14);
}

static inline int32_t riscv_dbg_dm_dmstatus_anynonexistent_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 14);
}


static inline void riscv_dbg_dm_dmstatus_allnonexistent_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 15));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allnonexistent_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 15);
}

static inline int32_t riscv_dbg_dm_dmstatus_allnonexistent_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 15);
}


static inline void riscv_dbg_dm_dmstatus_anyresumeack_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 16));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anyresumeack_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 16);
}

static inline int32_t riscv_dbg_dm_dmstatus_anyresumeack_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 16);
}


static inline void riscv_dbg_dm_dmstatus_allresumeack_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 17));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allresumeack_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 17);
}

static inline int32_t riscv_dbg_dm_dmstatus_allresumeack_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 17);
}


static inline void riscv_dbg_dm_dmstatus_anyhavereset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 18));
}

static inline uint32_t riscv_dbg_dm_dmstatus_anyhavereset_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 18);
}

static inline int32_t riscv_dbg_dm_dmstatus_anyhavereset_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 18);
}


static inline void riscv_dbg_dm_dmstatus_allhavereset_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 19));
}

static inline uint32_t riscv_dbg_dm_dmstatus_allhavereset_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 19);
}

static inline int32_t riscv_dbg_dm_dmstatus_allhavereset_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 19);
}


static inline void riscv_dbg_dm_dmstatus_impebreak_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMSTATUS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), value, 1, 22));
}

static inline uint32_t riscv_dbg_dm_dmstatus_impebreak_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 22);
}

static inline int32_t riscv_dbg_dm_dmstatus_impebreak_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMSTATUS_OFFSET), 1, 22);
}


static inline void riscv_dbg_dm_hartinfo_dataaddr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_HARTINFO_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), value, 12, 0));
}

static inline uint32_t riscv_dbg_dm_hartinfo_dataaddr_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 12, 0);
}

static inline int32_t riscv_dbg_dm_hartinfo_dataaddr_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 12, 0);
}


static inline void riscv_dbg_dm_hartinfo_datasize_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_HARTINFO_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), value, 4, 12));
}

static inline uint32_t riscv_dbg_dm_hartinfo_datasize_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 4, 12);
}

static inline int32_t riscv_dbg_dm_hartinfo_datasize_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 4, 12);
}


static inline void riscv_dbg_dm_hartinfo_dataaccess_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_HARTINFO_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), value, 1, 16));
}

static inline uint32_t riscv_dbg_dm_hartinfo_dataaccess_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 1, 16);
}

static inline int32_t riscv_dbg_dm_hartinfo_dataaccess_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 1, 16);
}


static inline void riscv_dbg_dm_hartinfo_nscratch_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_HARTINFO_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), value, 4, 20));
}

static inline uint32_t riscv_dbg_dm_hartinfo_nscratch_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 4, 20);
}

static inline int32_t riscv_dbg_dm_hartinfo_nscratch_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_HARTINFO_OFFSET), 4, 20);
}


static inline void riscv_dbg_dm_abstractcs_datacount_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), value, 4, 0));
}

static inline uint32_t riscv_dbg_dm_abstractcs_datacount_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 4, 0);
}

static inline int32_t riscv_dbg_dm_abstractcs_datacount_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 4, 0);
}


static inline void riscv_dbg_dm_abstractcs_cmderr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), value, 3, 8));
}

static inline uint32_t riscv_dbg_dm_abstractcs_cmderr_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 3, 8);
}

static inline int32_t riscv_dbg_dm_abstractcs_cmderr_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 3, 8);
}


static inline void riscv_dbg_dm_abstractcs_busy_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), value, 1, 12));
}

static inline uint32_t riscv_dbg_dm_abstractcs_busy_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 1, 12);
}

static inline int32_t riscv_dbg_dm_abstractcs_busy_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 1, 12);
}


static inline void riscv_dbg_dm_abstractcs_progbufsize_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), value, 5, 24));
}

static inline uint32_t riscv_dbg_dm_abstractcs_progbufsize_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 5, 24);
}

static inline int32_t riscv_dbg_dm_abstractcs_progbufsize_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_ABSTRACTCS_OFFSET), 5, 24);
}


static inline void riscv_dbg_dm_command_control_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_COMMAND_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), value, 24, 0));
}

static inline uint32_t riscv_dbg_dm_command_control_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), 24, 0);
}

static inline int32_t riscv_dbg_dm_command_control_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), 24, 0);
}


static inline void riscv_dbg_dm_command_cmdtype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_COMMAND_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), value, 8, 24));
}

static inline uint32_t riscv_dbg_dm_command_cmdtype_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), 8, 24);
}

static inline int32_t riscv_dbg_dm_command_cmdtype_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_COMMAND_OFFSET), 8, 24);
}


static inline void riscv_dbg_dm_sbcs_sbaccess8_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 0));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess8_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 0);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess8_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 0);
}


static inline void riscv_dbg_dm_sbcs_sbaccess16_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 1));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess16_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 1);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess16_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 1);
}


static inline void riscv_dbg_dm_sbcs_sbaccess32_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 2));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess32_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 2);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess32_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 2);
}


static inline void riscv_dbg_dm_sbcs_sbaccess64_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 3));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess64_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 3);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess64_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 3);
}


static inline void riscv_dbg_dm_sbcs_sbaccess128_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 4));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess128_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 4);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess128_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 4);
}


static inline void riscv_dbg_dm_sbcs_sbasize_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 7, 5));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbasize_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 7, 5);
}

static inline int32_t riscv_dbg_dm_sbcs_sbasize_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 7, 5);
}


static inline void riscv_dbg_dm_sbcs_sberror_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 3, 12));
}

static inline uint32_t riscv_dbg_dm_sbcs_sberror_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 12);
}

static inline int32_t riscv_dbg_dm_sbcs_sberror_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 12);
}


static inline void riscv_dbg_dm_sbcs_sbreadondata_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 15));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbreadondata_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 15);
}

static inline int32_t riscv_dbg_dm_sbcs_sbreadondata_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 15);
}


static inline void riscv_dbg_dm_sbcs_sbautoincrement_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 16));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbautoincrement_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 16);
}

static inline int32_t riscv_dbg_dm_sbcs_sbautoincrement_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 16);
}


static inline void riscv_dbg_dm_sbcs_sbaccess_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 3, 17));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbaccess_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 17);
}

static inline int32_t riscv_dbg_dm_sbcs_sbaccess_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 17);
}


static inline void riscv_dbg_dm_sbcs_sbreadonaddr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 20));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbreadonaddr_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 20);
}

static inline int32_t riscv_dbg_dm_sbcs_sbreadonaddr_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 20);
}


static inline void riscv_dbg_dm_sbcs_sbbusy_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 1, 21));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbbusy_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 21);
}

static inline int32_t riscv_dbg_dm_sbcs_sbbusy_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 1, 21);
}


static inline void riscv_dbg_dm_sbcs_sbbusyerror_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 6, 22));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbbusyerror_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 6, 22);
}

static inline int32_t riscv_dbg_dm_sbcs_sbbusyerror_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 6, 22);
}


static inline void riscv_dbg_dm_sbcs_sbversion_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBCS_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), value, 3, 29));
}

static inline uint32_t riscv_dbg_dm_sbcs_sbversion_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 29);
}

static inline int32_t riscv_dbg_dm_sbcs_sbversion_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_SBCS_OFFSET), 3, 29);
}


static inline void riscv_dbg_dm_progbuf0_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF0_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf0_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF0_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf0_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF0_OFFSET);
}


static inline void riscv_dbg_dm_progbuf1_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF1_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf1_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF1_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf1_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF1_OFFSET);
}


static inline void riscv_dbg_dm_progbuf2_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF2_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf2_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF2_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf2_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF2_OFFSET);
}


static inline void riscv_dbg_dm_progbuf3_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF3_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf3_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF3_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf3_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF3_OFFSET);
}


static inline void riscv_dbg_dm_progbuf4_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF4_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf4_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF4_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf4_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF4_OFFSET);
}


static inline void riscv_dbg_dm_progbuf5_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF5_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf5_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF5_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf5_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF5_OFFSET);
}


static inline void riscv_dbg_dm_progbuf6_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF6_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf6_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF6_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf6_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF6_OFFSET);
}


static inline void riscv_dbg_dm_progbuf7_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_PROGBUF7_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_progbuf7_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF7_OFFSET);
}

static inline int32_t riscv_dbg_dm_progbuf7_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_PROGBUF7_OFFSET);
}


static inline void riscv_dbg_dm_sbaddress0_address_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBADDRESS0_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_sbaddress0_address_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBADDRESS0_OFFSET);
}

static inline int32_t riscv_dbg_dm_sbaddress0_address_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBADDRESS0_OFFSET);
}


static inline void riscv_dbg_dm_dmcs2_hgselect_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), value, 1, 0));
}

static inline uint32_t riscv_dbg_dm_dmcs2_hgselect_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 0);
}

static inline int32_t riscv_dbg_dm_dmcs2_hgselect_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 0);
}


static inline void riscv_dbg_dm_dmcs2_hgwrite_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), value, 1, 1));
}

static inline uint32_t riscv_dbg_dm_dmcs2_hgwrite_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 1);
}

static inline int32_t riscv_dbg_dm_dmcs2_hgwrite_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 1);
}


static inline void riscv_dbg_dm_dmcs2_group_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), value, 5, 2));
}

static inline uint32_t riscv_dbg_dm_dmcs2_group_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 5, 2);
}

static inline int32_t riscv_dbg_dm_dmcs2_group_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 5, 2);
}


static inline void riscv_dbg_dm_dmcs2_dmexttrigger_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), value, 4, 7));
}

static inline uint32_t riscv_dbg_dm_dmcs2_dmexttrigger_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 4, 7);
}

static inline int32_t riscv_dbg_dm_dmcs2_dmexttrigger_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 4, 7);
}


static inline void riscv_dbg_dm_dmcs2_grouptype_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_DMCS2_OFFSET, GAP_BINSERT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), value, 1, 11));
}

static inline uint32_t riscv_dbg_dm_dmcs2_grouptype_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 11);
}

static inline int32_t riscv_dbg_dm_dmcs2_grouptype_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, RISCV_DBG_DM_DMCS2_OFFSET), 1, 11);
}


static inline void riscv_dbg_dm_sbdata0_data_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, RISCV_DBG_DM_SBDATA0_OFFSET, value);
}

static inline uint32_t riscv_dbg_dm_sbdata0_data_get(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBDATA0_OFFSET);
}

static inline int32_t riscv_dbg_dm_sbdata0_data_gets(uint32_t base)
{
    return GAP_READ(base, RISCV_DBG_DM_SBDATA0_OFFSET);
}


#endif

#endif
