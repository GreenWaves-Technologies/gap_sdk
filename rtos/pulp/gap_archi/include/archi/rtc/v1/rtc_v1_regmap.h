
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_REGMAP_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_REGMAP_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef struct {
  unsigned int apb_sr          ; // RTC APB status register.
  unsigned int apb_cr          ; // RTC APB control register.
  unsigned int apb_dr          ; // RTC APB data register.
  unsigned int reserved        ; // Reserved/Not used.
  unsigned int apb_icr         ; // RTC APB interruption control register.
  unsigned int apb_imr         ; // RTC APB interruption mask register.
  unsigned int apb_ifr         ; // RTC APB interruption flag register.
} __attribute__((packed)) rtc_rtc_t;

#endif

#endif
