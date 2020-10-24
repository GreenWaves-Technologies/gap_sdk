
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_ACCESSORS_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_ACCESSORS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t rtc_apb_sr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_SR_OFFSET); }
static inline void rtc_apb_sr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_SR_OFFSET, value); }

static inline uint32_t rtc_apb_cr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_CR_OFFSET); }
static inline void rtc_apb_cr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_CR_OFFSET, value); }

static inline uint32_t rtc_apb_dr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_DR_OFFSET); }
static inline void rtc_apb_dr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_DR_OFFSET, value); }

static inline uint32_t rtc_reserved_get(uint32_t base) { return ARCHI_READ(base, RTC_RESERVED_OFFSET); }
static inline void rtc_reserved_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_RESERVED_OFFSET, value); }

static inline uint32_t rtc_apb_icr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_ICR_OFFSET); }
static inline void rtc_apb_icr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_ICR_OFFSET, value); }

static inline uint32_t rtc_apb_imr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_IMR_OFFSET); }
static inline void rtc_apb_imr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_IMR_OFFSET, value); }

static inline uint32_t rtc_apb_ifr_get(uint32_t base) { return ARCHI_READ(base, RTC_APB_IFR_OFFSET); }
static inline void rtc_apb_ifr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, RTC_APB_IFR_OFFSET, value); }

#endif

#endif
