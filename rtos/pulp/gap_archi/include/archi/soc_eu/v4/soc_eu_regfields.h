
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGFIELDS_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_REGFIELDS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS
//

// Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided. (access: W)
#define SOC_EU_SW_EVENT_EVENT_BIT                                    0
#define SOC_EU_SW_EVENT_EVENT_WIDTH                                  8
#define SOC_EU_SW_EVENT_EVENT_MASK                                   0xff

// MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_BIT                           0
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_WIDTH                         32
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_MASK                          0xffffffff

// LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_BIT                           0
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_WIDTH                         32
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_MASK                          0xffffffff

// MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_BIT                           0
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_WIDTH                         32
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_MASK                          0xffffffff

// LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_BIT                           0
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_WIDTH                         32
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_MASK                          0xffffffff

// MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_BIT                           0
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_WIDTH                         32
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_MASK                          0xffffffff

// LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_BIT                           0
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_WIDTH                         32
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_MASK                          0xffffffff

// Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R)
#define SOC_EU_ERR_MSB_ERR_MSB_BIT                                   0
#define SOC_EU_ERR_MSB_ERR_MSB_WIDTH                                 32
#define SOC_EU_ERR_MSB_ERR_MSB_MASK                                  0xffffffff

// Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue. (access: R)
#define SOC_EU_ERR_LSB_ERR_LSB_BIT                                   0
#define SOC_EU_ERR_LSB_ERR_LSB_WIDTH                                 32
#define SOC_EU_ERR_LSB_ERR_LSB_MASK                                  0xffffffff

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_HI_EVT_BIT                                 0
#define SOC_EU_TIMER1_SEL_HI_EVT_WIDTH                               8
#define SOC_EU_TIMER1_SEL_HI_EVT_MASK                                0xff

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_HI_ENA_BIT                                 31
#define SOC_EU_TIMER1_SEL_HI_ENA_WIDTH                               1
#define SOC_EU_TIMER1_SEL_HI_ENA_MASK                                0x80000000

// Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_LO_EVT_BIT                                 0
#define SOC_EU_TIMER1_SEL_LO_EVT_WIDTH                               8
#define SOC_EU_TIMER1_SEL_LO_EVT_MASK                                0xff

// Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_LO_ENA_BIT                                 31
#define SOC_EU_TIMER1_SEL_LO_ENA_WIDTH                               1
#define SOC_EU_TIMER1_SEL_LO_ENA_MASK                                0x80000000

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_HI_EVT_BIT                                 0
#define SOC_EU_TIMER2_SEL_HI_EVT_WIDTH                               8
#define SOC_EU_TIMER2_SEL_HI_EVT_MASK                                0xff

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_HI_ENA_BIT                                 31
#define SOC_EU_TIMER2_SEL_HI_ENA_WIDTH                               1
#define SOC_EU_TIMER2_SEL_HI_ENA_MASK                                0x80000000

// Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_LO_EVT_BIT                                 0
#define SOC_EU_TIMER2_SEL_LO_EVT_WIDTH                               8
#define SOC_EU_TIMER2_SEL_LO_EVT_MASK                                0xff

// Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_LO_ENA_BIT                                 31
#define SOC_EU_TIMER2_SEL_LO_ENA_WIDTH                               1
#define SOC_EU_TIMER2_SEL_LO_ENA_MASK                                0x80000000

#endif
