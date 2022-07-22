
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

#ifndef __ARCHI_SOC_EU_REGFIELD__
#define __ARCHI_SOC_EU_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided. (access: W)
#define SOC_EU_SW_EVENT_EVENT_BIT                                    0
#define SOC_EU_SW_EVENT_EVENT_WIDTH                                  8
#define SOC_EU_SW_EVENT_EVENT_MASK                                   0xff
#define SOC_EU_SW_EVENT_EVENT_RESET                                  0x0

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_0_MASK_BIT                                    0
#define SOC_EU_FC_MASK_0_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_0_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_0_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_1_MASK_BIT                                    0
#define SOC_EU_FC_MASK_1_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_1_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_1_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_2_MASK_BIT                                    0
#define SOC_EU_FC_MASK_2_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_2_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_2_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_3_MASK_BIT                                    0
#define SOC_EU_FC_MASK_3_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_3_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_3_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_4_MASK_BIT                                    0
#define SOC_EU_FC_MASK_4_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_4_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_4_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_5_MASK_BIT                                    0
#define SOC_EU_FC_MASK_5_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_5_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_5_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_6_MASK_BIT                                    0
#define SOC_EU_FC_MASK_6_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_6_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_6_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
#define SOC_EU_FC_MASK_7_MASK_BIT                                    0
#define SOC_EU_FC_MASK_7_MASK_WIDTH                                  32
#define SOC_EU_FC_MASK_7_MASK_MASK                                   0xffffffff
#define SOC_EU_FC_MASK_7_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_0_MASK_BIT                                    0
#define SOC_EU_CL_MASK_0_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_0_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_0_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_1_MASK_BIT                                    0
#define SOC_EU_CL_MASK_1_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_1_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_1_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_2_MASK_BIT                                    0
#define SOC_EU_CL_MASK_2_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_2_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_2_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_3_MASK_BIT                                    0
#define SOC_EU_CL_MASK_3_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_3_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_3_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_4_MASK_BIT                                    0
#define SOC_EU_CL_MASK_4_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_4_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_4_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_5_MASK_BIT                                    0
#define SOC_EU_CL_MASK_5_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_5_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_5_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_6_MASK_BIT                                    0
#define SOC_EU_CL_MASK_6_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_6_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_6_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
#define SOC_EU_CL_MASK_7_MASK_BIT                                    0
#define SOC_EU_CL_MASK_7_MASK_WIDTH                                  32
#define SOC_EU_CL_MASK_7_MASK_MASK                                   0xffffffff
#define SOC_EU_CL_MASK_7_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_0_MASK_BIT                                    0
#define SOC_EU_PR_MASK_0_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_0_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_0_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_1_MASK_BIT                                    0
#define SOC_EU_PR_MASK_1_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_1_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_1_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_2_MASK_BIT                                    0
#define SOC_EU_PR_MASK_2_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_2_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_2_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_3_MASK_BIT                                    0
#define SOC_EU_PR_MASK_3_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_3_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_3_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_4_MASK_BIT                                    0
#define SOC_EU_PR_MASK_4_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_4_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_4_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_5_MASK_BIT                                    0
#define SOC_EU_PR_MASK_5_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_5_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_5_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_6_MASK_BIT                                    0
#define SOC_EU_PR_MASK_6_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_6_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_6_MASK_RESET                                  0xffffffff

// Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
#define SOC_EU_PR_MASK_7_MASK_BIT                                    0
#define SOC_EU_PR_MASK_7_MASK_WIDTH                                  32
#define SOC_EU_PR_MASK_7_MASK_MASK                                   0xffffffff
#define SOC_EU_PR_MASK_7_MASK_RESET                                  0xffffffff

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_0_MASK_BIT                                        0
#define SOC_EU_ERR_0_MASK_WIDTH                                      32
#define SOC_EU_ERR_0_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_0_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_1_MASK_BIT                                        0
#define SOC_EU_ERR_1_MASK_WIDTH                                      32
#define SOC_EU_ERR_1_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_1_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_2_MASK_BIT                                        0
#define SOC_EU_ERR_2_MASK_WIDTH                                      32
#define SOC_EU_ERR_2_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_2_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_3_MASK_BIT                                        0
#define SOC_EU_ERR_3_MASK_WIDTH                                      32
#define SOC_EU_ERR_3_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_3_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_4_MASK_BIT                                        0
#define SOC_EU_ERR_4_MASK_WIDTH                                      32
#define SOC_EU_ERR_4_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_4_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_5_MASK_BIT                                        0
#define SOC_EU_ERR_5_MASK_WIDTH                                      32
#define SOC_EU_ERR_5_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_5_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_6_MASK_BIT                                        0
#define SOC_EU_ERR_6_MASK_WIDTH                                      32
#define SOC_EU_ERR_6_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_6_MASK_RESET                                      0x0

// Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R/W)
#define SOC_EU_ERR_7_MASK_BIT                                        0
#define SOC_EU_ERR_7_MASK_WIDTH                                      32
#define SOC_EU_ERR_7_MASK_MASK                                       0xffffffff
#define SOC_EU_ERR_7_MASK_RESET                                      0x0

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_HI_EVT_BIT                                 0
#define SOC_EU_TIMER1_SEL_HI_EVT_WIDTH                               8
#define SOC_EU_TIMER1_SEL_HI_EVT_MASK                                0xff
#define SOC_EU_TIMER1_SEL_HI_EVT_RESET                               0x0

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_HI_ENA_BIT                                 31
#define SOC_EU_TIMER1_SEL_HI_ENA_WIDTH                               1
#define SOC_EU_TIMER1_SEL_HI_ENA_MASK                                0x80000000
#define SOC_EU_TIMER1_SEL_HI_ENA_RESET                               0x0

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_LO_EVT_BIT                                 0
#define SOC_EU_TIMER1_SEL_LO_EVT_WIDTH                               8
#define SOC_EU_TIMER1_SEL_LO_EVT_MASK                                0xff
#define SOC_EU_TIMER1_SEL_LO_EVT_RESET                               0x0

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER1_SEL_LO_ENA_BIT                                 31
#define SOC_EU_TIMER1_SEL_LO_ENA_WIDTH                               1
#define SOC_EU_TIMER1_SEL_LO_ENA_MASK                                0x80000000
#define SOC_EU_TIMER1_SEL_LO_ENA_RESET                               0x0

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_HI_EVT_BIT                                 0
#define SOC_EU_TIMER2_SEL_HI_EVT_WIDTH                               8
#define SOC_EU_TIMER2_SEL_HI_EVT_MASK                                0xff
#define SOC_EU_TIMER2_SEL_HI_EVT_RESET                               0x0

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_HI_ENA_BIT                                 31
#define SOC_EU_TIMER2_SEL_HI_ENA_WIDTH                               1
#define SOC_EU_TIMER2_SEL_HI_ENA_MASK                                0x80000000
#define SOC_EU_TIMER2_SEL_HI_ENA_RESET                               0x0

// Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_LO_EVT_BIT                                 0
#define SOC_EU_TIMER2_SEL_LO_EVT_WIDTH                               8
#define SOC_EU_TIMER2_SEL_LO_EVT_MASK                                0xff
#define SOC_EU_TIMER2_SEL_LO_EVT_RESET                               0x0

// Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
#define SOC_EU_TIMER2_SEL_LO_ENA_BIT                                 31
#define SOC_EU_TIMER2_SEL_LO_ENA_WIDTH                               1
#define SOC_EU_TIMER2_SEL_LO_ENA_MASK                                0x80000000
#define SOC_EU_TIMER2_SEL_LO_ENA_RESET                               0x0

#endif
