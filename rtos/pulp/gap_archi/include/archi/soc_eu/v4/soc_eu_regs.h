
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGS_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS
//

// SoC software events trigger command register.
#define SOC_EU_SW_EVENT_OFFSET                   0x0

// MSB FC event unit event dispatch mask configuration register.
#define SOC_EU_FC_MASK_MSB_OFFSET                0x4

// LSB FC event unit event dispatch mask configuration register.
#define SOC_EU_FC_MASK_LSB_OFFSET                0x8

// MSB Cluster event dispatch mask configuration register.
#define SOC_EU_CL_MASK_MSB_OFFSET                0xc

// LSB Cluster event dispatch mask configuration register.
#define SOC_EU_CL_MASK_LSB_OFFSET                0x10

// MSB uDMA event dispatch mask configuration register.
#define SOC_EU_PR_MASK_MSB_OFFSET                0x14

// LSB uDMA event dispatch mask configuration register.
#define SOC_EU_PR_MASK_LSB_OFFSET                0x18

// MSB event queue overflow status register.
#define SOC_EU_ERR_MSB_OFFSET                    0x1c

// LSB event queue overflow status register.
#define SOC_EU_ERR_LSB_OFFSET                    0x20

// FC High Timer1 source event configuration register.
#define SOC_EU_TIMER1_SEL_HI_OFFSET              0x84

// FC Low Timer1 source event configuration register.
#define SOC_EU_TIMER1_SEL_LO_OFFSET              0x88

// FC High Timer2 source event configuration register.
#define SOC_EU_TIMER2_SEL_HI_OFFSET              0x8c

// FC Low Timer2 source event configuration register.
#define SOC_EU_TIMER2_SEL_LO_OFFSET              0x90

#endif
