
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

#ifndef __ARCHI_SOC_EU_ACCESSORS__
#define __ARCHI_SOC_EU_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline __attribute__((always_inline)) uint32_t soc_eu_sw_event_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_SW_EVENT_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_sw_event_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_SW_EVENT_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_0_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_0_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_1_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_1_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_2_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_2_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_3_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_3_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_3_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_4_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_4_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_4_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_5_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_5_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_5_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_6_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_6_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_6_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_fc_mask_7_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_FC_MASK_7_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_fc_mask_7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_FC_MASK_7_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_0_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_0_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_1_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_1_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_2_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_2_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_3_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_3_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_3_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_4_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_4_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_4_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_5_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_5_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_5_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_6_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_6_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_6_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_cl_mask_7_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_CL_MASK_7_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_cl_mask_7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_CL_MASK_7_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_0_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_0_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_1_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_1_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_2_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_2_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_3_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_3_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_3_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_4_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_4_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_4_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_5_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_5_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_5_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_6_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_6_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_6_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_pr_mask_7_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_PR_MASK_7_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_pr_mask_7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_PR_MASK_7_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_0_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_0_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_0_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_1_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_1_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_1_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_2_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_2_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_2_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_2_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_3_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_3_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_3_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_3_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_4_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_4_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_4_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_4_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_5_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_5_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_5_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_5_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_6_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_6_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_6_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_6_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_err_7_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_ERR_7_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_err_7_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_ERR_7_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_timer1_sel_hi_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_TIMER1_SEL_HI_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_timer1_sel_hi_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_TIMER1_SEL_HI_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_timer1_sel_lo_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_TIMER1_SEL_LO_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_timer1_sel_lo_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_TIMER1_SEL_LO_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_timer2_sel_hi_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_TIMER2_SEL_HI_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_timer2_sel_hi_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_TIMER2_SEL_HI_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t soc_eu_timer2_sel_lo_get(uint32_t base)
{
    return GAP_READ(base, SOC_EU_TIMER2_SEL_LO_OFFSET);
}

static inline __attribute__((always_inline)) void soc_eu_timer2_sel_lo_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, SOC_EU_TIMER2_SEL_LO_OFFSET, value);
}


#endif

#endif
