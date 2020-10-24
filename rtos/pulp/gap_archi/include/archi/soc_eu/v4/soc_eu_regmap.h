
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGMAP_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGMAP_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef struct {
  unsigned int sw_event        ; // SoC software events trigger command register.
  unsigned int fc_mask_msb     ; // MSB FC event unit event dispatch mask configuration register.
  unsigned int fc_mask_lsb     ; // LSB FC event unit event dispatch mask configuration register.
  unsigned int cl_mask_msb     ; // MSB Cluster event dispatch mask configuration register.
  unsigned int cl_mask_lsb     ; // LSB Cluster event dispatch mask configuration register.
  unsigned int pr_mask_msb     ; // MSB uDMA event dispatch mask configuration register.
  unsigned int pr_mask_lsb     ; // LSB uDMA event dispatch mask configuration register.
  unsigned int err_msb         ; // MSB event queue overflow status register.
  unsigned int err_lsb         ; // LSB event queue overflow status register.
  unsigned int timer1_sel_hi   ; // FC High Timer1 source event configuration register.
  unsigned int timer1_sel_lo   ; // FC Low Timer1 source event configuration register.
  unsigned int timer2_sel_hi   ; // FC High Timer2 source event configuration register.
  unsigned int timer2_sel_lo   ; // FC Low Timer2 source event configuration register.
} __attribute__((packed)) soc_eu_soc_eu_t;

#endif

#endif
