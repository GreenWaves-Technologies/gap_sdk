
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_ACCESSORS_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_ACCESSORS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t soc_eu_sw_event_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_SW_EVENT_OFFSET); }
static inline void soc_eu_sw_event_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_SW_EVENT_OFFSET, value); }

static inline uint32_t soc_eu_fc_mask_msb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_FC_MASK_MSB_OFFSET); }
static inline void soc_eu_fc_mask_msb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_FC_MASK_MSB_OFFSET, value); }

static inline uint32_t soc_eu_fc_mask_lsb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_FC_MASK_LSB_OFFSET); }
static inline void soc_eu_fc_mask_lsb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_FC_MASK_LSB_OFFSET, value); }

static inline uint32_t soc_eu_cl_mask_msb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_CL_MASK_MSB_OFFSET); }
static inline void soc_eu_cl_mask_msb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_CL_MASK_MSB_OFFSET, value); }

static inline uint32_t soc_eu_cl_mask_lsb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_CL_MASK_LSB_OFFSET); }
static inline void soc_eu_cl_mask_lsb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_CL_MASK_LSB_OFFSET, value); }

static inline uint32_t soc_eu_pr_mask_msb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_PR_MASK_MSB_OFFSET); }
static inline void soc_eu_pr_mask_msb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_PR_MASK_MSB_OFFSET, value); }

static inline uint32_t soc_eu_pr_mask_lsb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_PR_MASK_LSB_OFFSET); }
static inline void soc_eu_pr_mask_lsb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_PR_MASK_LSB_OFFSET, value); }

static inline uint32_t soc_eu_err_msb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_ERR_MSB_OFFSET); }
static inline void soc_eu_err_msb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_ERR_MSB_OFFSET, value); }

static inline uint32_t soc_eu_err_lsb_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_ERR_LSB_OFFSET); }
static inline void soc_eu_err_lsb_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_ERR_LSB_OFFSET, value); }

static inline uint32_t soc_eu_timer1_sel_hi_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_TIMER1_SEL_HI_OFFSET); }
static inline void soc_eu_timer1_sel_hi_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_TIMER1_SEL_HI_OFFSET, value); }

static inline uint32_t soc_eu_timer1_sel_lo_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_TIMER1_SEL_LO_OFFSET); }
static inline void soc_eu_timer1_sel_lo_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_TIMER1_SEL_LO_OFFSET, value); }

static inline uint32_t soc_eu_timer2_sel_hi_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_TIMER2_SEL_HI_OFFSET); }
static inline void soc_eu_timer2_sel_hi_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_TIMER2_SEL_HI_OFFSET, value); }

static inline uint32_t soc_eu_timer2_sel_lo_get(uint32_t base) { return ARCHI_READ(base, SOC_EU_TIMER2_SEL_LO_OFFSET); }
static inline void soc_eu_timer2_sel_lo_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, SOC_EU_TIMER2_SEL_LO_OFFSET, value); }

#endif

#endif
