
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_REGFIELD__
#define __ARCHI_CLUSTER_CTRL_UNIT_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached (access: R/W)
#define CLUSTER_CTRL_UNIT_EOC_EOC_BIT                                0
#define CLUSTER_CTRL_UNIT_EOC_EOC_WIDTH                              1
#define CLUSTER_CTRL_UNIT_EOC_EOC_MASK                               0x1
#define CLUSTER_CTRL_UNIT_EOC_EOC_RESET                              0x0

// Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_BIT                         0
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK                        0x1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_RESET                       0x0

// Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_BIT                         1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK                        0x2
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_RESET                       0x0

// Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_BIT                         2
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK                        0x4
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_RESET                       0x0

// Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_BIT                         3
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK                        0x8
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_RESET                       0x0

// Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_BIT                         4
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK                        0x10
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_RESET                       0x0

// Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_BIT                         5
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK                        0x20
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_RESET                       0x0

// Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_BIT                         6
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK                        0x40
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_RESET                       0x0

// Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_BIT                         7
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_WIDTH                       1
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK                        0x80
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_RESET                       0x0

// Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_BIT                          0
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_WIDTH                        1
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK                         0x1
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_RESET                        0x0

// Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_BIT                       0
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_MASK                      0x1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_RESET                     0x0

// Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_BIT                       1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_MASK                      0x2
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_RESET                     0x0

// Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_BIT                       2
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_MASK                      0x4
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_RESET                     0x0

// Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_BIT                       3
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_MASK                      0x8
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_RESET                     0x0

// Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_BIT                       4
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_MASK                      0x10
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_RESET                     0x0

// Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_BIT                       5
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_MASK                      0x20
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_RESET                     0x0

// Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_BIT                       6
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_MASK                      0x40
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_RESET                     0x0

// Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7 (access: W)
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_BIT                       7
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_WIDTH                     1
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_MASK                      0x80
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_RESET                     0x0

// Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_BIT                  0
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_MASK                 0x1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_RESET                0x0

// Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_BIT                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_MASK                 0x2
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_RESET                0x0

// Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_BIT                  2
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_MASK                 0x4
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_RESET                0x0

// Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_BIT                  3
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_MASK                 0x8
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_RESET                0x0

// Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_BIT                  4
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_MASK                 0x10
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_RESET                0x0

// Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_BIT                  5
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_MASK                 0x20
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_RESET                0x0

// Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_BIT                  6
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_MASK                 0x40
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_RESET                0x0

// Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_BIT                  7
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_WIDTH                1
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_MASK                 0x80
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_RESET                0x0

// Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_BIT                    0
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK                   0x1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_RESET                  0x0

// Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_BIT                    1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK                   0x2
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_RESET                  0x0

// Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_BIT                    2
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK                   0x4
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_RESET                  0x0

// Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_BIT                    3
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK                   0x8
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_RESET                  0x0

// Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_BIT                    4
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK                   0x10
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_RESET                  0x0

// Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_BIT                    5
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK                   0x20
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_RESET                  0x0

// Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_BIT                    6
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK                   0x40
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_RESET                  0x0

// Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_BIT                    7
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_WIDTH                  1
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK                   0x80
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_RESET                  0x0

// Cluster core 0 boot address configuration bitfield. (access: R/W)
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_BIT                          0
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_WIDTH                        32
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK                         0xffffffff
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_RESET                        0x0

// TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_BIT                0
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_WIDTH              1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK               0x1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_RESET              0x0

// TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_BIT                0
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_WIDTH              1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK               0x1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_RESET              0x0

// TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_BIT            0
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_WIDTH          1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK           0x1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_RESET          0x0

// TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_BIT            0
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_WIDTH          1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK           0x1
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_RESET          0x0

#endif
