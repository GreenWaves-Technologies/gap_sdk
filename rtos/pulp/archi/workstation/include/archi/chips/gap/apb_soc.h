
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __INCLUDE_ARCHI_CHIPS_GAP_APB_SOC_H__
#define __INCLUDE_ARCHI_CHIPS_GAP_APB_SOC_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS
//

// Core information register
#define APB_SOC_INFO_OFFSET                      0x0

// RFU
#define APB_SOC_NOTUSED0_OFFSET                  0x4

// RFU
#define APB_SOC_NOTUSED1_OFFSET                  0x8

// Isolate cluster register
#define APB_SOC_CL_ISOLATE_OFFSET                0xc

// Cluster busy register
#define APB_SOC_CL_BUSY_OFFSET                   0x6c

// PMU bypass configuration register
#define APB_SOC_CL_BYPASS_OFFSET                 0x70

// JTAG external register
#define APB_SOC_JTAGREG_OFFSET                   0x74

// L2 sleep configuration register
#define APB_SOC_L2_SLEEP_OFFSET                  0x78

// Alias for SAFE_PMU_SLEEPCTRL
#define APB_SOC_SLEEP_CTRL_OFFSET                0x7c

// EOC and chip status register
#define APB_SOC_CORESTATUS_OFFSET                0xa0

// EOC and chip status register read mirror
#define APB_SOC_CORESTATUS_RO_OFFSET             0xc0

// DC/DC configuration register
#define APB_SOC_SAFE_PMU_RAR_OFFSET              0x100

// Sleep modes configuration register
#define APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET        0x104

// L2 rententive state configuration
#define APB_SOC_SAFE_PMU_FORCE_OFFSET            0x108

// Mux config register (pad 0-15)
#define APB_SOC_SAFE_PADFUN0_OFFSET              0x140

// Mux config register (pad 16-31)
#define APB_SOC_SAFE_PADFUN1_OFFSET              0x144

// Mux config register (pad 32-47)
#define APB_SOC_SAFE_PADFUN2_OFFSET              0x148

// Mux config register (pad 48-63)
#define APB_SOC_SAFE_PADFUN3_OFFSET              0x14c

// Sleep config register (pad 0-15)
#define APB_SOC_SAFE_SLEEPPADCFG0_OFFSET         0x150

// Mux config register (pad 16-31)
#define APB_SOC_SAFE_SLEEPPADCFG1_OFFSET         0x154

// Mux config register (pad 32-47)
#define APB_SOC_SAFE_SLEEPPADCFG2_OFFSET         0x158

// Mux config register (pad 48-63)
#define APB_SOC_SAFE_SLEEPPADCFG3_OFFSET         0x15c

// Enable Sleep mode for pads
#define APB_SOC_SAFE_PADSLEEP_OFFSET             0x160

// Function register (pad 0 to 3)
#define APB_SOC_SAFE_PADCFG0_OFFSET              0x180

// Function register (pad 4 to 7)
#define APB_SOC_SAFE_PADCFG1_OFFSET              0x184

// Function register (pad 8 to 11)
#define APB_SOC_SAFE_PADCFG2_OFFSET              0x188

// Function register (pad 12 to 15)
#define APB_SOC_SAFE_PADCFG3_OFFSET              0x18c

// Function register (pad 16 to 19)
#define APB_SOC_SAFE_PADCFG4_OFFSET              0x190

// Function register (pad 20 to 23)
#define APB_SOC_SAFE_PADCFG5_OFFSET              0x194

// Function register (pad 24 to 27)
#define APB_SOC_SAFE_PADCFG6_OFFSET              0x198

// Function register (pad 28 to 31)
#define APB_SOC_SAFE_PADCFG7_OFFSET              0x19c

// Function register (pad 32 to 35)
#define APB_SOC_SAFE_PADCFG8_OFFSET              0x1a0

// Function register (pad 36 to 39)
#define APB_SOC_SAFE_PADCFG9_OFFSET              0x1a4

// Function register (pad 40 to 43)
#define APB_SOC_SAFE_PADCFG10_OFFSET             0x1a8

// Function register (pad 44 to 47)
#define APB_SOC_SAFE_PADCFG11_OFFSET             0x1ac

// Function register (pad 48 to 51)
#define APB_SOC_SAFE_PADCFG12_OFFSET             0x1b0

// Function register (pad 52 to 55)
#define APB_SOC_SAFE_PADCFG13_OFFSET             0x1b4

// Function register (pad 56 to 59)
#define APB_SOC_SAFE_PADCFG14_OFFSET             0x1b8

// Function register (pad 60 to 63)
#define APB_SOC_SAFE_PADCFG15_OFFSET             0x1bc

// GPIO power domain pad input isolation register
#define APB_SOC_REG_GPIO_ISO_OFFSET              0x1c0

// CAM power domain pad input isolation register
#define APB_SOC_REG_CAM_ISO_OFFSET               0x1c4

// LVDS power domain pad input isolation register
#define APB_SOC_REG_LVDS_ISO_OFFSET              0x1c8



//
// REGISTERS FIELDS
//

// Number of clusters (access: R)
#define APB_SOC_INFO_NB_CL_BIT                                       0
#define APB_SOC_INFO_NB_CL_WIDTH                                     16
#define APB_SOC_INFO_NB_CL_MASK                                      0xffff

// Number of cores (access: R)
#define APB_SOC_INFO_NB_CORES_BIT                                    16
#define APB_SOC_INFO_NB_CORES_WIDTH                                  16
#define APB_SOC_INFO_NB_CORES_MASK                                   0xffff0000

// Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable (access: R/W)
#define APB_SOC_CL_ISOLATE_EN_BIT                                    0
#define APB_SOC_CL_ISOLATE_EN_WIDTH                                  1
#define APB_SOC_CL_ISOLATE_EN_MASK                                   0x1

// Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
#define APB_SOC_CL_BUSY_BUSY_BIT                                     0
#define APB_SOC_CL_BUSY_BUSY_WIDTH                                   1
#define APB_SOC_CL_BUSY_BUSY_MASK                                    0x1

// Bypass Maestro PMU controller configuration bitfield: - 1b0: disabled - 1b1: enabled (access: R/W)
#define APB_SOC_CL_BYPASS_BYP_POW_BIT                                0
#define APB_SOC_CL_BYPASS_BYP_POW_WIDTH                              1
#define APB_SOC_CL_BYPASS_BYP_POW_MASK                               0x1

// Bypass Maestro PMU configuration selection configuration bitfield: - 1b0: use default - 1b1: use user configuration (bitfields from bits 3 to 15 of CL_BYPASS register) (access: R/W)
#define APB_SOC_CL_BYPASS_BYP_CFG_BIT                                1
#define APB_SOC_CL_BYPASS_BYP_CFG_WIDTH                              1
#define APB_SOC_CL_BYPASS_BYP_CFG_MASK                               0x2

// Cluster state configuration and status bitfield: - 1b0: off - 1b1: on Status information is correct only when bypass mode is enabled. (access: R/W)
#define APB_SOC_CL_BYPASS_CL_STATE_BIT                               3
#define APB_SOC_CL_BYPASS_CL_STATE_WIDTH                             1
#define APB_SOC_CL_BYPASS_CL_STATE_MASK                              0x8

// Max current allowed on cluster TRC configuration bitfield. (access: R/W)
#define APB_SOC_CL_BYPASS_CURRSET_BIT                                4
#define APB_SOC_CL_BYPASS_CURRSET_WIDTH                              3
#define APB_SOC_CL_BYPASS_CURRSET_MASK                               0x70

// Number of REFCLK 32kHz after cluster power ok has arised to release TR isolation configuration bitfield. (access: R/W)
#define APB_SOC_CL_BYPASS_PROG_DEL_BIT                               7
#define APB_SOC_CL_BYPASS_PROG_DEL_WIDTH                             2
#define APB_SOC_CL_BYPASS_PROG_DEL_MASK                              0x180

// Bypass cluster clock and reset control by Maestro PMU configuration bitfield: - 1b0: disabled - 1b1: enabled (access: R/W)
#define APB_SOC_CL_BYPASS_BYP_CLK_BIT                                9
#define APB_SOC_CL_BYPASS_BYP_CLK_WIDTH                              1
#define APB_SOC_CL_BYPASS_BYP_CLK_MASK                               0x200

// Cluster clock gate configuration bitfield: - 1b0: disabled - 1b1: enabled It should always be used before switching cluster FLL to shutdown or retentive mode. (access: R/W)
#define APB_SOC_CL_BYPASS_CG_BIT                                     10
#define APB_SOC_CL_BYPASS_CG_WIDTH                                   1
#define APB_SOC_CL_BYPASS_CG_MASK                                    0x400

// Cluster FLL shutdown configuration bitfield: - 1b0: FLL on - 1b1: FLL shutdown mode (access: R/W)
#define APB_SOC_CL_BYPASS_FLL_PWD_BIT                                11
#define APB_SOC_CL_BYPASS_FLL_PWD_WIDTH                              1
#define APB_SOC_CL_BYPASS_FLL_PWD_MASK                               0x800

// Cluster FLL retentive configuration bitfield: - 1b0: FLL on - 1b1: FLL retentive mode (access: R/W)
#define APB_SOC_CL_BYPASS_FLL_RET_BIT                                12
#define APB_SOC_CL_BYPASS_FLL_RET_WIDTH                              1
#define APB_SOC_CL_BYPASS_FLL_RET_MASK                               0x1000

// Cluster reset configuration bitfield: - 1b0: nothing - 1b1: reset the cluster (access: R/W)
#define APB_SOC_CL_BYPASS_RST_BIT                                    13
#define APB_SOC_CL_BYPASS_RST_WIDTH                                  1
#define APB_SOC_CL_BYPASS_RST_MASK                                   0x2000

// ? (access: R/W)
#define APB_SOC_CL_BYPASS_BYP_ISO_BIT                                14
#define APB_SOC_CL_BYPASS_BYP_ISO_WIDTH                              1
#define APB_SOC_CL_BYPASS_BYP_ISO_MASK                               0x4000

// ? (access: R/W)
#define APB_SOC_CL_BYPASS_PWISO_BIT                                  15
#define APB_SOC_CL_BYPASS_PWISO_WIDTH                                1
#define APB_SOC_CL_BYPASS_PWISO_MASK                                 0x8000

// Cluster power ok from cluster TRC status bitfield (access: R/W)
#define APB_SOC_CL_BYPASS_TRCPOWOK_BIT                               16
#define APB_SOC_CL_BYPASS_TRCPOWOK_WIDTH                             1
#define APB_SOC_CL_BYPASS_TRCPOWOK_MASK                              0x10000

// Cluster power down from Maestro PMU status bitfield. (access: R/W)
#define APB_SOC_CL_BYPASS_PMUPOWDOWN_BIT                             17
#define APB_SOC_CL_BYPASS_PMUPOWDOWN_WIDTH                           1
#define APB_SOC_CL_BYPASS_PMUPOWDOWN_MASK                            0x20000

// JTAG internal register used for synchronisation from external debugger (access: R/W)
#define APB_SOC_JTAGREG_INT_SYNC_BIT                                 0
#define APB_SOC_JTAGREG_INT_SYNC_WIDTH                               1
#define APB_SOC_JTAGREG_INT_SYNC_MASK                                0x1

// JTAG internal register used for selecting boot mode configuration from external debugger (access: R/W)
#define APB_SOC_JTAGREG_INT_BT_MD_BIT                                1
#define APB_SOC_JTAGREG_INT_BT_MD_WIDTH                              3
#define APB_SOC_JTAGREG_INT_BT_MD_MASK                               0xe

// JTAG external register used for synchronisation from external debugger (access: R)
#define APB_SOC_JTAGREG_EXT_SYNC_BIT                                 8
#define APB_SOC_JTAGREG_EXT_SYNC_WIDTH                               1
#define APB_SOC_JTAGREG_EXT_SYNC_MASK                                0x100

// JTAG external register used for selecting boot mode configuration from external debugger (access: R)
#define APB_SOC_JTAGREG_EXT_BT_MD_BIT                                9
#define APB_SOC_JTAGREG_EXT_BT_MD_WIDTH                              3
#define APB_SOC_JTAGREG_EXT_BT_MD_MASK                               0xe00

// L2 memory sleep configuration (access: R/W)
#define APB_SOC_L2_SLEEP_L2_SLEEP_BIT                                0
#define APB_SOC_L2_SLEEP_L2_SLEEP_WIDTH                              1
#define APB_SOC_L2_SLEEP_L2_SLEEP_MASK                               0x1

// Alias for SAFE_PMU_SLEEPCTRL(i.e. will be accessible in 1 clock cycle) (access: R)
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_BIT                            0
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_WIDTH                          32
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_MASK                           0xffffffff

// EOC and chip status register (access: R/W)
#define APB_SOC_CORESTATUS_STATUS_BIT                                0
#define APB_SOC_CORESTATUS_STATUS_WIDTH                              32
#define APB_SOC_CORESTATUS_STATUS_MASK                               0xffffffff

// EOC and chip status register (access: R)
#define APB_SOC_CORESTATUS_RO_STATUS_BIT                             0
#define APB_SOC_CORESTATUS_RO_STATUS_WIDTH                           32
#define APB_SOC_CORESTATUS_RO_STATUS_MASK                            0xffffffff

// DC/DC Nominal Voltage setting (access: R/W)
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_BIT                             0
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_WIDTH                           5
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_MASK                            0x1f

// DC/DC Medium Voltage setting (not used) (access: R/W)
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_BIT                             8
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_WIDTH                           5
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_MASK                            0x1f00

// DC/DC Low Voltage setting (access: R/W)
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_BIT                             16
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_WIDTH                           5
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_MASK                            0x1f0000

// DC/DC Retentive Voltage setting (access: R/W)
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_BIT                             24
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_WIDTH                           5
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_MASK                            0x1f000000

// Configure retention mode for region 0 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_BIT                         0
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_WIDTH                       1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_MASK                        0x1

// Configure retention mode for region 1 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_BIT                         1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_WIDTH                       1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_MASK                        0x2

// Configure retention mode for region 2 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_BIT                         2
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_WIDTH                       1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_MASK                        0x4

// Configure retention mode for region 3 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_BIT                         3
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_WIDTH                       1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_MASK                        0x8

// Configure retention mode for SoC FLL: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_BIT                       4
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_WIDTH                     1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_MASK                      0x10

// Configure retention mode for cluster FLL: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_BIT                        5
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_WIDTH                      1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_MASK                       0x20

// Select external wake-up source (GPIO ID): - 5'b00000: GPIO 0 - 5'b00001: GPIO 1 - 5'b00010: GPIO 2 - 5'b00011: GPIO 3 - 5'b00100: GPIO 4 - 5'b00101: GPIO 5 - 5'b00110: GPIO 6 - 5'b00111: GPIO 7 - 5'b01000: GPIO 8 - 5'b01001: GPIO 9 - 5'b01010: GPIO 10 - 5'b01011: GPIO 11 - 5'b01100: GPIO 12 - 5'b01101: GPIO 13 - 5'b01110: GPIO 14 - 5'b01111: GPIO 15 - 5'b10000: GPIO 16 - 5'b10001: GPIO 17 - 5'b10010: GPIO 18 - 5'b10011: GPIO 19 - 5'b10100: GPIO 20 - 5'b10101: GPIO 21 - 5'b10110: GPIO 22 - 5'b10111: GPIO 23 - 5'b11000: GPIO 24 - 5'b11001: GPIO 25 - 5'b11010: GPIO 26 - 5'b11011: GPIO 27 - 5'b11100: GPIO 28 - 5'b11101: GPIO 29 - 5'b11110: GPIO 30 - 5'b11111: GPIO 31 (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_BIT                   6
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_WIDTH                 5
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_MASK                  0x7c0

// Select external wake-up  mode: - 2'b00: rise event - 2'b01: fall event - 2'b10: high level - 2'b11: low level (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT                  11
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH                2
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_MASK                 0x1800

// Enable external wake-up; - 1'b0; external wake-up disabled - 1'b1: external wake-up enabled (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT                    13
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH                  1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_MASK                   0x2000

// - (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_BIT                     15
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_WIDTH                   1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_MASK                    0x8000

// Warm bootmode: - 1'b0: Boot from ROM - 1'b1: Boot from L2 (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_BIT                         16
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_WIDTH                       1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_MASK                        0x10000

// External wake-up interrupt status (automatically resetted after read) - 1'b0: wake-up triggered by RTC - 1'b1: wake-up triggered by external event (access: R)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_BIT                        17
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_WIDTH                      1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_MASK                       0x20000

// Select boot type: - 2'b00: cold boot - 2'b01: deep sleep - 2'b10: retentive deep sleep (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_BIT                        18
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_WIDTH                      2
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_MASK                       0xc0000

// Cluster state to restore after warm boot: - 1b0: off - 1b1: on (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_BIT                       20
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_WIDTH                     1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_MASK                      0x100000

// Force retentive state on region 0 of L2 memory: 1'b0: not state retentive 1'b1: state retentive (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_BIT                         0
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_WIDTH                       1
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_MASK                        0x1

// Force retentive state on region 1 of L2 memory: 1'b0: not state retentive 1'b1: state retentive (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_BIT                         1
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_WIDTH                       1
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_MASK                        0x2

// Force retentive state on region 2 of L2 memory: 1'b0: not state retentive 1'b1: state retentive (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_BIT                         2
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_WIDTH                       1
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_MASK                        0x4

// Force retentive state on region 3 of L2 memory: 1'b0: not state retentive 1'b1: state retentive (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_BIT                         3
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_WIDTH                       1
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_MASK                        0x8

// Force power down on region 0 of L2 memory: 1'b0: power up 1'b1: power down (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_BIT                          4
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_WIDTH                        1
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_MASK                         0x10

// Force power down on region 1 of L2 memory: 1'b0: power up 1'b1: power down (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_BIT                          5
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_WIDTH                        1
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_MASK                         0x20

// Force power down on region 2 of L2 memory: 1'b0: power up 1'b1: power down (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_BIT                          6
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_WIDTH                        1
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_MASK                         0x40

// Force power down on region 3 of L2 memory: 1'b0: power up 1'b1: power down (access: R/W)
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_BIT                          7
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_WIDTH                        1
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_MASK                         0x80

// Enable pad sleep mode: 1'b0: disable 1'b1: enable (access: R/W)
#define APB_SOC_SAFE_PADSLEEP_EN_BIT                                 0
#define APB_SOC_SAFE_PADSLEEP_EN_WIDTH                               1
#define APB_SOC_SAFE_PADSLEEP_EN_MASK                                0x1

// Configuration of GPIO domain pads isolation: - 1'b0: not isolated - 1'b1: isolated (access: R/W)
#define APB_SOC_REG_GPIO_ISO_ISO_BIT                                 0
#define APB_SOC_REG_GPIO_ISO_ISO_WIDTH                               1
#define APB_SOC_REG_GPIO_ISO_ISO_MASK                                0x1

// Configuration of CAM domain pads isolation: - 1'b0: not isolated - 1'b1: isolated (access: R/W)
#define APB_SOC_REG_CAM_ISO_ISO_BIT                                  0
#define APB_SOC_REG_CAM_ISO_ISO_WIDTH                                1
#define APB_SOC_REG_CAM_ISO_ISO_MASK                                 0x1

// Configuration of LVDS domain pads isolation: - 1'b0: not isolated - 1'b1: isolated (access: R/W)
#define APB_SOC_REG_LVDS_ISO_ISO_BIT                                 0
#define APB_SOC_REG_LVDS_ISO_ISO_WIDTH                               1
#define APB_SOC_REG_LVDS_ISO_ISO_MASK                                0x1



//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int nb_cl           :16; // Number of clusters
    unsigned int nb_cores        :16; // Number of cores
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_info_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_notused0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_notused1_t;

typedef union {
  struct {
    unsigned int en              :1 ; // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_cl_isolate_t;

typedef union {
  struct {
    unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_cl_busy_t;

typedef union {
  struct {
    unsigned int byp_pow         :1 ; // Bypass Maestro PMU controller configuration bitfield: - 1b0: disabled - 1b1: enabled
    unsigned int byp_cfg         :1 ; // Bypass Maestro PMU configuration selection configuration bitfield: - 1b0: use default - 1b1: use user configuration (bitfields from bits 3 to 15 of CL_BYPASS register)
    unsigned int padding0:1 ;
    unsigned int cl_state        :1 ; // Cluster state configuration and status bitfield: - 1b0: off - 1b1: on Status information is correct only when bypass mode is enabled.
    unsigned int currset         :3 ; // Max current allowed on cluster TRC configuration bitfield.
    unsigned int prog_del        :2 ; // Number of REFCLK 32kHz after cluster power ok has arised to release TR isolation configuration bitfield.
    unsigned int byp_clk         :1 ; // Bypass cluster clock and reset control by Maestro PMU configuration bitfield: - 1b0: disabled - 1b1: enabled
    unsigned int cg              :1 ; // Cluster clock gate configuration bitfield: - 1b0: disabled - 1b1: enabled It should always be used before switching cluster FLL to shutdown or retentive mode.
    unsigned int fll_pwd         :1 ; // Cluster FLL shutdown configuration bitfield: - 1b0: FLL on - 1b1: FLL shutdown mode
    unsigned int fll_ret         :1 ; // Cluster FLL retentive configuration bitfield: - 1b0: FLL on - 1b1: FLL retentive mode
    unsigned int rst             :1 ; // Cluster reset configuration bitfield: - 1b0: nothing - 1b1: reset the cluster
    unsigned int byp_iso         :1 ; // ?
    unsigned int pwiso           :1 ; // ?
    unsigned int trcpowok        :1 ; // Cluster power ok from cluster TRC status bitfield
    unsigned int pmupowdown      :1 ; // Cluster power down from Maestro PMU status bitfield.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_cl_bypass_t;

typedef union {
  struct {
    unsigned int int_sync        :1 ; // JTAG internal register used for synchronisation from external debugger
    unsigned int int_bt_md       :3 ; // JTAG internal register used for selecting boot mode configuration from external debugger
    unsigned int padding0:4 ;
    unsigned int ext_sync        :1 ; // JTAG external register used for synchronisation from external debugger
    unsigned int ext_bt_md       :3 ; // JTAG external register used for selecting boot mode configuration from external debugger
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_jtagreg_t;

typedef union {
  struct {
    unsigned int l2_sleep        :1 ; // L2 memory sleep configuration
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_l2_sleep_t;

typedef union {
  struct {
    unsigned int sleep_ctrl      :32; // Alias for SAFE_PMU_SLEEPCTRL(i.e. will be accessible in 1 clock cycle)
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_sleep_ctrl_t;

typedef union {
  struct {
    unsigned int status          :32; // EOC and chip status register
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_corestatus_t;

typedef union {
  struct {
    unsigned int status          :32; // EOC and chip status register
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_corestatus_ro_t;

typedef union {
  struct {
    unsigned int nv_volt         :5 ; // DC/DC Nominal Voltage setting
    unsigned int padding0:3 ;
    unsigned int mv_volt         :5 ; // DC/DC Medium Voltage setting (not used)
    unsigned int padding1:3 ;
    unsigned int lv_volt         :5 ; // DC/DC Low Voltage setting
    unsigned int padding2:3 ;
    unsigned int rv_volt         :5 ; // DC/DC Retentive Voltage setting
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_pmu_rar_t;

typedef union {
  struct {
    unsigned int l2_r0           :1 ; // Configure retention mode for region 0 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int l2_r1           :1 ; // Configure retention mode for region 1 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int l2_r2           :1 ; // Configure retention mode for region 2 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int l2_r3           :1 ; // Configure retention mode for region 3 of L2 memory: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int soc_fll         :1 ; // Configure retention mode for SoC FLL: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int cl_fll          :1 ; // Configure retention mode for cluster FLL: - 1'b0: Non retentive - 1'b1: Retentive
    unsigned int extwake_src     :5 ; // Select external wake-up source (GPIO ID): - 5'b00000: GPIO 0 - 5'b00001: GPIO 1 - 5'b00010: GPIO 2 - 5'b00011: GPIO 3 - 5'b00100: GPIO 4 - 5'b00101: GPIO 5 - 5'b00110: GPIO 6 - 5'b00111: GPIO 7 - 5'b01000: GPIO 8 - 5'b01001: GPIO 9 - 5'b01010: GPIO 10 - 5'b01011: GPIO 11 - 5'b01100: GPIO 12 - 5'b01101: GPIO 13 - 5'b01110: GPIO 14 - 5'b01111: GPIO 15 - 5'b10000: GPIO 16 - 5'b10001: GPIO 17 - 5'b10010: GPIO 18 - 5'b10011: GPIO 19 - 5'b10100: GPIO 20 - 5'b10101: GPIO 21 - 5'b10110: GPIO 22 - 5'b10111: GPIO 23 - 5'b11000: GPIO 24 - 5'b11001: GPIO 25 - 5'b11010: GPIO 26 - 5'b11011: GPIO 27 - 5'b11100: GPIO 28 - 5'b11101: GPIO 29 - 5'b11110: GPIO 30 - 5'b11111: GPIO 31
    unsigned int extwake_type    :2 ; // Select external wake-up  mode: - 2'b00: rise event - 2'b01: fall event - 2'b10: high level - 2'b11: low level
    unsigned int extwake_en      :1 ; // Enable external wake-up; - 1'b0; external wake-up disabled - 1'b1: external wake-up enabled
    unsigned int padding0:1 ;
    unsigned int wakestate       :1 ; // -
    unsigned int btdev           :1 ; // Warm bootmode: - 1'b0: Boot from ROM - 1'b1: Boot from L2
    unsigned int extint          :1 ; // External wake-up interrupt status (automatically resetted after read) - 1'b0: wake-up triggered by RTC - 1'b1: wake-up triggered by external event
    unsigned int bttype          :2 ; // Select boot type: - 2'b00: cold boot - 2'b01: deep sleep - 2'b10: retentive deep sleep
    unsigned int cl_wake         :1 ; // Cluster state to restore after warm boot: - 1b0: off - 1b1: on
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_pmu_sleepctrl_t;

typedef union {
  struct {
    unsigned int ret_l2_r0       :1 ; // Force retentive state on region 0 of L2 memory: 1'b0: not state retentive 1'b1: state retentive
    unsigned int ret_l2_r1       :1 ; // Force retentive state on region 1 of L2 memory: 1'b0: not state retentive 1'b1: state retentive
    unsigned int ret_l2_r2       :1 ; // Force retentive state on region 2 of L2 memory: 1'b0: not state retentive 1'b1: state retentive
    unsigned int ret_l2_r3       :1 ; // Force retentive state on region 3 of L2 memory: 1'b0: not state retentive 1'b1: state retentive
    unsigned int pd_l2_r0        :1 ; // Force power down on region 0 of L2 memory: 1'b0: power up 1'b1: power down
    unsigned int pd_l2_r1        :1 ; // Force power down on region 1 of L2 memory: 1'b0: power up 1'b1: power down
    unsigned int pd_l2_r2        :1 ; // Force power down on region 2 of L2 memory: 1'b0: power up 1'b1: power down
    unsigned int pd_l2_r3        :1 ; // Force power down on region 3 of L2 memory: 1'b0: power up 1'b1: power down
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_pmu_force_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padfun0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padfun1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padfun2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padfun3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_sleeppadcfg0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_sleeppadcfg1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_sleeppadcfg2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_sleeppadcfg3_t;

typedef union {
  struct {
    unsigned int en              :1 ; // Enable pad sleep mode: 1'b0: disable 1'b1: enable
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padsleep_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg4_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg5_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg6_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg7_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg8_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg9_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg10_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg11_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg12_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg13_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg14_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_padcfg15_t;

typedef union {
  struct {
    unsigned int iso             :1 ; // Configuration of GPIO domain pads isolation: - 1'b0: not isolated - 1'b1: isolated
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_reg_gpio_iso_t;

typedef union {
  struct {
    unsigned int iso             :1 ; // Configuration of CAM domain pads isolation: - 1'b0: not isolated - 1'b1: isolated
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_reg_cam_iso_t;

typedef union {
  struct {
    unsigned int iso             :1 ; // Configuration of LVDS domain pads isolation: - 1'b0: not isolated - 1'b1: isolated
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_reg_lvds_iso_t;

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_apb_soc_info : public vp::reg_32
{
public:
  inline void nb_cl_set(uint32_t value) { this->set_field(value, APB_SOC_INFO_NB_CL_BIT, APB_SOC_INFO_NB_CL_WIDTH); }
  inline uint32_t nb_cl_get() { return this->get_field(APB_SOC_INFO_NB_CL_BIT, APB_SOC_INFO_NB_CL_WIDTH); }
  inline void nb_cores_set(uint32_t value) { this->set_field(value, APB_SOC_INFO_NB_CORES_BIT, APB_SOC_INFO_NB_CORES_WIDTH); }
  inline uint32_t nb_cores_get() { return this->get_field(APB_SOC_INFO_NB_CORES_BIT, APB_SOC_INFO_NB_CORES_WIDTH); }
};

class vp_apb_soc_notused0 : public vp::reg_32
{
public:
};

class vp_apb_soc_notused1 : public vp::reg_32
{
public:
};

class vp_apb_soc_cl_isolate : public vp::reg_32
{
public:
  inline void en_set(uint32_t value) { this->set_field(value, APB_SOC_CL_ISOLATE_EN_BIT, APB_SOC_CL_ISOLATE_EN_WIDTH); }
  inline uint32_t en_get() { return this->get_field(APB_SOC_CL_ISOLATE_EN_BIT, APB_SOC_CL_ISOLATE_EN_WIDTH); }
};

class vp_apb_soc_cl_busy : public vp::reg_32
{
public:
  inline void busy_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BUSY_BUSY_BIT, APB_SOC_CL_BUSY_BUSY_WIDTH); }
  inline uint32_t busy_get() { return this->get_field(APB_SOC_CL_BUSY_BUSY_BIT, APB_SOC_CL_BUSY_BUSY_WIDTH); }
};

class vp_apb_soc_cl_bypass : public vp::reg_32
{
public:
  inline void byp_pow_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_BYP_POW_BIT, APB_SOC_CL_BYPASS_BYP_POW_WIDTH); }
  inline uint32_t byp_pow_get() { return this->get_field(APB_SOC_CL_BYPASS_BYP_POW_BIT, APB_SOC_CL_BYPASS_BYP_POW_WIDTH); }
  inline void byp_cfg_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_BYP_CFG_BIT, APB_SOC_CL_BYPASS_BYP_CFG_WIDTH); }
  inline uint32_t byp_cfg_get() { return this->get_field(APB_SOC_CL_BYPASS_BYP_CFG_BIT, APB_SOC_CL_BYPASS_BYP_CFG_WIDTH); }
  inline void cl_state_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_CL_STATE_BIT, APB_SOC_CL_BYPASS_CL_STATE_WIDTH); }
  inline uint32_t cl_state_get() { return this->get_field(APB_SOC_CL_BYPASS_CL_STATE_BIT, APB_SOC_CL_BYPASS_CL_STATE_WIDTH); }
  inline void currset_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_CURRSET_BIT, APB_SOC_CL_BYPASS_CURRSET_WIDTH); }
  inline uint32_t currset_get() { return this->get_field(APB_SOC_CL_BYPASS_CURRSET_BIT, APB_SOC_CL_BYPASS_CURRSET_WIDTH); }
  inline void prog_del_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_PROG_DEL_BIT, APB_SOC_CL_BYPASS_PROG_DEL_WIDTH); }
  inline uint32_t prog_del_get() { return this->get_field(APB_SOC_CL_BYPASS_PROG_DEL_BIT, APB_SOC_CL_BYPASS_PROG_DEL_WIDTH); }
  inline void byp_clk_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_BYP_CLK_BIT, APB_SOC_CL_BYPASS_BYP_CLK_WIDTH); }
  inline uint32_t byp_clk_get() { return this->get_field(APB_SOC_CL_BYPASS_BYP_CLK_BIT, APB_SOC_CL_BYPASS_BYP_CLK_WIDTH); }
  inline void cg_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_CG_BIT, APB_SOC_CL_BYPASS_CG_WIDTH); }
  inline uint32_t cg_get() { return this->get_field(APB_SOC_CL_BYPASS_CG_BIT, APB_SOC_CL_BYPASS_CG_WIDTH); }
  inline void fll_pwd_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_FLL_PWD_BIT, APB_SOC_CL_BYPASS_FLL_PWD_WIDTH); }
  inline uint32_t fll_pwd_get() { return this->get_field(APB_SOC_CL_BYPASS_FLL_PWD_BIT, APB_SOC_CL_BYPASS_FLL_PWD_WIDTH); }
  inline void fll_ret_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_FLL_RET_BIT, APB_SOC_CL_BYPASS_FLL_RET_WIDTH); }
  inline uint32_t fll_ret_get() { return this->get_field(APB_SOC_CL_BYPASS_FLL_RET_BIT, APB_SOC_CL_BYPASS_FLL_RET_WIDTH); }
  inline void rst_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_RST_BIT, APB_SOC_CL_BYPASS_RST_WIDTH); }
  inline uint32_t rst_get() { return this->get_field(APB_SOC_CL_BYPASS_RST_BIT, APB_SOC_CL_BYPASS_RST_WIDTH); }
  inline void byp_iso_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_BYP_ISO_BIT, APB_SOC_CL_BYPASS_BYP_ISO_WIDTH); }
  inline uint32_t byp_iso_get() { return this->get_field(APB_SOC_CL_BYPASS_BYP_ISO_BIT, APB_SOC_CL_BYPASS_BYP_ISO_WIDTH); }
  inline void pwiso_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_PWISO_BIT, APB_SOC_CL_BYPASS_PWISO_WIDTH); }
  inline uint32_t pwiso_get() { return this->get_field(APB_SOC_CL_BYPASS_PWISO_BIT, APB_SOC_CL_BYPASS_PWISO_WIDTH); }
  inline void trcpowok_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_TRCPOWOK_BIT, APB_SOC_CL_BYPASS_TRCPOWOK_WIDTH); }
  inline uint32_t trcpowok_get() { return this->get_field(APB_SOC_CL_BYPASS_TRCPOWOK_BIT, APB_SOC_CL_BYPASS_TRCPOWOK_WIDTH); }
  inline void pmupowdown_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BYPASS_PMUPOWDOWN_BIT, APB_SOC_CL_BYPASS_PMUPOWDOWN_WIDTH); }
  inline uint32_t pmupowdown_get() { return this->get_field(APB_SOC_CL_BYPASS_PMUPOWDOWN_BIT, APB_SOC_CL_BYPASS_PMUPOWDOWN_WIDTH); }
};

class vp_apb_soc_jtagreg : public vp::reg_32
{
public:
  inline void int_sync_set(uint32_t value) { this->set_field(value, APB_SOC_JTAGREG_INT_SYNC_BIT, APB_SOC_JTAGREG_INT_SYNC_WIDTH); }
  inline uint32_t int_sync_get() { return this->get_field(APB_SOC_JTAGREG_INT_SYNC_BIT, APB_SOC_JTAGREG_INT_SYNC_WIDTH); }
  inline void int_bt_md_set(uint32_t value) { this->set_field(value, APB_SOC_JTAGREG_INT_BT_MD_BIT, APB_SOC_JTAGREG_INT_BT_MD_WIDTH); }
  inline uint32_t int_bt_md_get() { return this->get_field(APB_SOC_JTAGREG_INT_BT_MD_BIT, APB_SOC_JTAGREG_INT_BT_MD_WIDTH); }
  inline void ext_sync_set(uint32_t value) { this->set_field(value, APB_SOC_JTAGREG_EXT_SYNC_BIT, APB_SOC_JTAGREG_EXT_SYNC_WIDTH); }
  inline uint32_t ext_sync_get() { return this->get_field(APB_SOC_JTAGREG_EXT_SYNC_BIT, APB_SOC_JTAGREG_EXT_SYNC_WIDTH); }
  inline void ext_bt_md_set(uint32_t value) { this->set_field(value, APB_SOC_JTAGREG_EXT_BT_MD_BIT, APB_SOC_JTAGREG_EXT_BT_MD_WIDTH); }
  inline uint32_t ext_bt_md_get() { return this->get_field(APB_SOC_JTAGREG_EXT_BT_MD_BIT, APB_SOC_JTAGREG_EXT_BT_MD_WIDTH); }
};

class vp_apb_soc_l2_sleep : public vp::reg_32
{
public:
  inline void l2_sleep_set(uint32_t value) { this->set_field(value, APB_SOC_L2_SLEEP_L2_SLEEP_BIT, APB_SOC_L2_SLEEP_L2_SLEEP_WIDTH); }
  inline uint32_t l2_sleep_get() { return this->get_field(APB_SOC_L2_SLEEP_L2_SLEEP_BIT, APB_SOC_L2_SLEEP_L2_SLEEP_WIDTH); }
};

class vp_apb_soc_sleep_ctrl : public vp::reg_32
{
public:
  inline void sleep_ctrl_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_BIT, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_WIDTH); }
  inline uint32_t sleep_ctrl_get() { return this->get_field(APB_SOC_SLEEP_CTRL_SLEEP_CTRL_BIT, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_WIDTH); }
};

class vp_apb_soc_corestatus : public vp::reg_32
{
public:
  inline void status_set(uint32_t value) { this->set_field(value, APB_SOC_CORESTATUS_STATUS_BIT, APB_SOC_CORESTATUS_STATUS_WIDTH); }
  inline uint32_t status_get() { return this->get_field(APB_SOC_CORESTATUS_STATUS_BIT, APB_SOC_CORESTATUS_STATUS_WIDTH); }
};

class vp_apb_soc_corestatus_ro : public vp::reg_32
{
public:
  inline void status_set(uint32_t value) { this->set_field(value, APB_SOC_CORESTATUS_RO_STATUS_BIT, APB_SOC_CORESTATUS_RO_STATUS_WIDTH); }
  inline uint32_t status_get() { return this->get_field(APB_SOC_CORESTATUS_RO_STATUS_BIT, APB_SOC_CORESTATUS_RO_STATUS_WIDTH); }
};

class vp_apb_soc_safe_pmu_rar : public vp::reg_32
{
public:
  inline void nv_volt_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_RAR_NV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_NV_VOLT_WIDTH); }
  inline uint32_t nv_volt_get() { return this->get_field(APB_SOC_SAFE_PMU_RAR_NV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_NV_VOLT_WIDTH); }
  inline void mv_volt_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_RAR_MV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_MV_VOLT_WIDTH); }
  inline uint32_t mv_volt_get() { return this->get_field(APB_SOC_SAFE_PMU_RAR_MV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_MV_VOLT_WIDTH); }
  inline void lv_volt_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_RAR_LV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_LV_VOLT_WIDTH); }
  inline uint32_t lv_volt_get() { return this->get_field(APB_SOC_SAFE_PMU_RAR_LV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_LV_VOLT_WIDTH); }
  inline void rv_volt_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_RAR_RV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_RV_VOLT_WIDTH); }
  inline uint32_t rv_volt_get() { return this->get_field(APB_SOC_SAFE_PMU_RAR_RV_VOLT_BIT, APB_SOC_SAFE_PMU_RAR_RV_VOLT_WIDTH); }
};

class vp_apb_soc_safe_pmu_sleepctrl : public vp::reg_32
{
public:
  inline void l2_r0_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_WIDTH); }
  inline uint32_t l2_r0_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_WIDTH); }
  inline void l2_r1_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_WIDTH); }
  inline uint32_t l2_r1_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_WIDTH); }
  inline void l2_r2_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_WIDTH); }
  inline uint32_t l2_r2_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_WIDTH); }
  inline void l2_r3_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_WIDTH); }
  inline uint32_t l2_r3_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_WIDTH); }
  inline void soc_fll_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_WIDTH); }
  inline uint32_t soc_fll_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_WIDTH); }
  inline void cl_fll_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_WIDTH); }
  inline uint32_t cl_fll_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_WIDTH); }
  inline void extwake_src_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_WIDTH); }
  inline uint32_t extwake_src_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_WIDTH); }
  inline void extwake_type_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH); }
  inline uint32_t extwake_type_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH); }
  inline void extwake_en_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH); }
  inline uint32_t extwake_en_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH); }
  inline void wakestate_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_WIDTH); }
  inline uint32_t wakestate_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_WIDTH); }
  inline void btdev_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_WIDTH); }
  inline uint32_t btdev_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_WIDTH); }
  inline void extint_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_WIDTH); }
  inline uint32_t extint_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_WIDTH); }
  inline void bttype_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_WIDTH); }
  inline uint32_t bttype_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_WIDTH); }
  inline void cl_wake_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_WIDTH); }
  inline uint32_t cl_wake_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_WIDTH); }
};

class vp_apb_soc_safe_pmu_force : public vp::reg_32
{
public:
  inline void ret_l2_r0_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_WIDTH); }
  inline uint32_t ret_l2_r0_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_WIDTH); }
  inline void ret_l2_r1_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_WIDTH); }
  inline uint32_t ret_l2_r1_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_WIDTH); }
  inline void ret_l2_r2_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_WIDTH); }
  inline uint32_t ret_l2_r2_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_WIDTH); }
  inline void ret_l2_r3_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_WIDTH); }
  inline uint32_t ret_l2_r3_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_BIT, APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_WIDTH); }
  inline void pd_l2_r0_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_WIDTH); }
  inline uint32_t pd_l2_r0_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_WIDTH); }
  inline void pd_l2_r1_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_WIDTH); }
  inline uint32_t pd_l2_r1_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_WIDTH); }
  inline void pd_l2_r2_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_WIDTH); }
  inline uint32_t pd_l2_r2_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_WIDTH); }
  inline void pd_l2_r3_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_WIDTH); }
  inline uint32_t pd_l2_r3_get() { return this->get_field(APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_BIT, APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_WIDTH); }
};

class vp_apb_soc_safe_padfun0 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padfun1 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padfun2 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padfun3 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_sleeppadcfg0 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_sleeppadcfg1 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_sleeppadcfg2 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_sleeppadcfg3 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padsleep : public vp::reg_32
{
public:
  inline void en_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PADSLEEP_EN_BIT, APB_SOC_SAFE_PADSLEEP_EN_WIDTH); }
  inline uint32_t en_get() { return this->get_field(APB_SOC_SAFE_PADSLEEP_EN_BIT, APB_SOC_SAFE_PADSLEEP_EN_WIDTH); }
};

class vp_apb_soc_safe_padcfg0 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg1 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg2 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg3 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg4 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg5 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg6 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg7 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg8 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg9 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg10 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg11 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg12 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg13 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg14 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_padcfg15 : public vp::reg_32
{
public:
};

class vp_apb_soc_reg_gpio_iso : public vp::reg_32
{
public:
  inline void iso_set(uint32_t value) { this->set_field(value, APB_SOC_REG_GPIO_ISO_ISO_BIT, APB_SOC_REG_GPIO_ISO_ISO_WIDTH); }
  inline uint32_t iso_get() { return this->get_field(APB_SOC_REG_GPIO_ISO_ISO_BIT, APB_SOC_REG_GPIO_ISO_ISO_WIDTH); }
};

class vp_apb_soc_reg_cam_iso : public vp::reg_32
{
public:
  inline void iso_set(uint32_t value) { this->set_field(value, APB_SOC_REG_CAM_ISO_ISO_BIT, APB_SOC_REG_CAM_ISO_ISO_WIDTH); }
  inline uint32_t iso_get() { return this->get_field(APB_SOC_REG_CAM_ISO_ISO_BIT, APB_SOC_REG_CAM_ISO_ISO_WIDTH); }
};

class vp_apb_soc_reg_lvds_iso : public vp::reg_32
{
public:
  inline void iso_set(uint32_t value) { this->set_field(value, APB_SOC_REG_LVDS_ISO_ISO_BIT, APB_SOC_REG_LVDS_ISO_ISO_WIDTH); }
  inline uint32_t iso_get() { return this->get_field(APB_SOC_REG_LVDS_ISO_ISO_BIT, APB_SOC_REG_LVDS_ISO_ISO_WIDTH); }
};

#endif



//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef struct {
  unsigned int info            ; // Core information register
  unsigned int notused0        ; // RFU
  unsigned int notused1        ; // RFU
  unsigned int cl_isolate      ; // Isolate cluster register
  unsigned int cl_busy         ; // Cluster busy register
  unsigned int cl_bypass       ; // PMU bypass configuration register
  unsigned int jtagreg         ; // JTAG external register
  unsigned int l2_sleep        ; // L2 sleep configuration register
  unsigned int sleep_ctrl      ; // Alias for SAFE_PMU_SLEEPCTRL
  unsigned int corestatus      ; // EOC and chip status register
  unsigned int corestatus_ro   ; // EOC and chip status register read mirror
  unsigned int safe_pmu_rar    ; // DC/DC configuration register
  unsigned int safe_pmu_sleepctrl; // Sleep modes configuration register
  unsigned int safe_pmu_force  ; // L2 rententive state configuration
  unsigned int safe_padfun0    ; // Mux config register (pad 0-15)
  unsigned int safe_padfun1    ; // Mux config register (pad 16-31)
  unsigned int safe_padfun2    ; // Mux config register (pad 32-47)
  unsigned int safe_padfun3    ; // Mux config register (pad 48-63)
  unsigned int safe_sleeppadcfg0; // Sleep config register (pad 0-15)
  unsigned int safe_sleeppadcfg1; // Mux config register (pad 16-31)
  unsigned int safe_sleeppadcfg2; // Mux config register (pad 32-47)
  unsigned int safe_sleeppadcfg3; // Mux config register (pad 48-63)
  unsigned int safe_padsleep   ; // Enable Sleep mode for pads
  unsigned int safe_padcfg0    ; // Function register (pad 0 to 3)
  unsigned int safe_padcfg1    ; // Function register (pad 4 to 7)
  unsigned int safe_padcfg2    ; // Function register (pad 8 to 11)
  unsigned int safe_padcfg3    ; // Function register (pad 12 to 15)
  unsigned int safe_padcfg4    ; // Function register (pad 16 to 19)
  unsigned int safe_padcfg5    ; // Function register (pad 20 to 23)
  unsigned int safe_padcfg6    ; // Function register (pad 24 to 27)
  unsigned int safe_padcfg7    ; // Function register (pad 28 to 31)
  unsigned int safe_padcfg8    ; // Function register (pad 32 to 35)
  unsigned int safe_padcfg9    ; // Function register (pad 36 to 39)
  unsigned int safe_padcfg10   ; // Function register (pad 40 to 43)
  unsigned int safe_padcfg11   ; // Function register (pad 44 to 47)
  unsigned int safe_padcfg12   ; // Function register (pad 48 to 51)
  unsigned int safe_padcfg13   ; // Function register (pad 52 to 55)
  unsigned int safe_padcfg14   ; // Function register (pad 56 to 59)
  unsigned int safe_padcfg15   ; // Function register (pad 60 to 63)
  unsigned int reg_gpio_iso    ; // GPIO power domain pad input isolation register
  unsigned int reg_cam_iso     ; // CAM power domain pad input isolation register
  unsigned int reg_lvds_iso    ; // LVDS power domain pad input isolation register
} __attribute__((packed)) apb_soc_apb_soc_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t apb_soc_info_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_INFO_OFFSET); }
static inline void apb_soc_info_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_INFO_OFFSET, value); }

static inline uint32_t apb_soc_notused0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_NOTUSED0_OFFSET); }
static inline void apb_soc_notused0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_NOTUSED0_OFFSET, value); }

static inline uint32_t apb_soc_notused1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_NOTUSED1_OFFSET); }
static inline void apb_soc_notused1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_NOTUSED1_OFFSET, value); }

static inline uint32_t apb_soc_cl_isolate_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CL_ISOLATE_OFFSET); }
static inline void apb_soc_cl_isolate_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CL_ISOLATE_OFFSET, value); }

static inline uint32_t apb_soc_cl_busy_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CL_BUSY_OFFSET); }
static inline void apb_soc_cl_busy_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CL_BUSY_OFFSET, value); }

static inline uint32_t apb_soc_cl_bypass_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CL_BYPASS_OFFSET); }
static inline void apb_soc_cl_bypass_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CL_BYPASS_OFFSET, value); }

static inline uint32_t apb_soc_jtagreg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_JTAGREG_OFFSET); }
static inline void apb_soc_jtagreg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_JTAGREG_OFFSET, value); }

static inline uint32_t apb_soc_l2_sleep_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_L2_SLEEP_OFFSET); }
static inline void apb_soc_l2_sleep_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_L2_SLEEP_OFFSET, value); }

static inline uint32_t apb_soc_sleep_ctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SLEEP_CTRL_OFFSET); }
static inline void apb_soc_sleep_ctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SLEEP_CTRL_OFFSET, value); }

static inline uint32_t apb_soc_corestatus_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CORESTATUS_OFFSET); }
static inline void apb_soc_corestatus_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CORESTATUS_OFFSET, value); }

static inline uint32_t apb_soc_corestatus_ro_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CORESTATUS_RO_OFFSET); }
static inline void apb_soc_corestatus_ro_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CORESTATUS_RO_OFFSET, value); }

static inline uint32_t apb_soc_safe_pmu_rar_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PMU_RAR_OFFSET); }
static inline void apb_soc_safe_pmu_rar_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PMU_RAR_OFFSET, value); }

static inline uint32_t apb_soc_safe_pmu_sleepctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET); }
static inline void apb_soc_safe_pmu_sleepctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET, value); }

static inline uint32_t apb_soc_safe_pmu_force_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PMU_FORCE_OFFSET); }
static inline void apb_soc_safe_pmu_force_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PMU_FORCE_OFFSET, value); }

static inline uint32_t apb_soc_safe_padfun0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADFUN0_OFFSET); }
static inline void apb_soc_safe_padfun0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADFUN0_OFFSET, value); }

static inline uint32_t apb_soc_safe_padfun1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADFUN1_OFFSET); }
static inline void apb_soc_safe_padfun1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADFUN1_OFFSET, value); }

static inline uint32_t apb_soc_safe_padfun2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADFUN2_OFFSET); }
static inline void apb_soc_safe_padfun2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADFUN2_OFFSET, value); }

static inline uint32_t apb_soc_safe_padfun3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADFUN3_OFFSET); }
static inline void apb_soc_safe_padfun3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADFUN3_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG1_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG1_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG2_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG2_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG3_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG3_OFFSET, value); }

static inline uint32_t apb_soc_safe_padsleep_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADSLEEP_OFFSET); }
static inline void apb_soc_safe_padsleep_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADSLEEP_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG0_OFFSET); }
static inline void apb_soc_safe_padcfg0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG0_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG1_OFFSET); }
static inline void apb_soc_safe_padcfg1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG1_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG2_OFFSET); }
static inline void apb_soc_safe_padcfg2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG2_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG3_OFFSET); }
static inline void apb_soc_safe_padcfg3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG3_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg4_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG4_OFFSET); }
static inline void apb_soc_safe_padcfg4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG4_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg5_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG5_OFFSET); }
static inline void apb_soc_safe_padcfg5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG5_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg6_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG6_OFFSET); }
static inline void apb_soc_safe_padcfg6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG6_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg7_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG7_OFFSET); }
static inline void apb_soc_safe_padcfg7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG7_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg8_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG8_OFFSET); }
static inline void apb_soc_safe_padcfg8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG8_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg9_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG9_OFFSET); }
static inline void apb_soc_safe_padcfg9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG9_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg10_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG10_OFFSET); }
static inline void apb_soc_safe_padcfg10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG10_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg11_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG11_OFFSET); }
static inline void apb_soc_safe_padcfg11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG11_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg12_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG12_OFFSET); }
static inline void apb_soc_safe_padcfg12_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG12_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg13_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG13_OFFSET); }
static inline void apb_soc_safe_padcfg13_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG13_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg14_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG14_OFFSET); }
static inline void apb_soc_safe_padcfg14_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG14_OFFSET, value); }

static inline uint32_t apb_soc_safe_padcfg15_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PADCFG15_OFFSET); }
static inline void apb_soc_safe_padcfg15_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PADCFG15_OFFSET, value); }

static inline uint32_t apb_soc_reg_gpio_iso_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_REG_GPIO_ISO_OFFSET); }
static inline void apb_soc_reg_gpio_iso_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_REG_GPIO_ISO_OFFSET, value); }

static inline uint32_t apb_soc_reg_cam_iso_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_REG_CAM_ISO_OFFSET); }
static inline void apb_soc_reg_cam_iso_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_REG_CAM_ISO_OFFSET, value); }

static inline uint32_t apb_soc_reg_lvds_iso_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_REG_LVDS_ISO_OFFSET); }
static inline void apb_soc_reg_lvds_iso_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_REG_LVDS_ISO_OFFSET, value); }

#endif



//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define APB_SOC_INFO_NB_CL_GET(value)                      (ARCHI_BEXTRACTU((value),16,0))
#define APB_SOC_INFO_NB_CL_GETS(value)                     (ARCHI_BEXTRACT((value),16,0))
#define APB_SOC_INFO_NB_CL_SET(value,field)                (ARCHI_BINSERT((value),(field),16,0))
#define APB_SOC_INFO_NB_CL(val)                            ((val) << 0)

#define APB_SOC_INFO_NB_CORES_GET(value)                   (ARCHI_BEXTRACTU((value),16,16))
#define APB_SOC_INFO_NB_CORES_GETS(value)                  (ARCHI_BEXTRACT((value),16,16))
#define APB_SOC_INFO_NB_CORES_SET(value,field)             (ARCHI_BINSERT((value),(field),16,16))
#define APB_SOC_INFO_NB_CORES(val)                         ((val) << 16)

#define APB_SOC_CL_ISOLATE_EN_GET(value)                   (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CL_ISOLATE_EN_GETS(value)                  (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CL_ISOLATE_EN_SET(value,field)             (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CL_ISOLATE_EN(val)                         ((val) << 0)

#define APB_SOC_CL_BUSY_BUSY_GET(value)                    (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CL_BUSY_BUSY_GETS(value)                   (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CL_BUSY_BUSY_SET(value,field)              (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CL_BUSY_BUSY(val)                          ((val) << 0)

#define APB_SOC_CL_BYPASS_BYP_POW_GET(value)               (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CL_BYPASS_BYP_POW_GETS(value)              (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CL_BYPASS_BYP_POW_SET(value,field)         (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CL_BYPASS_BYP_POW(val)                     ((val) << 0)

#define APB_SOC_CL_BYPASS_BYP_CFG_GET(value)               (ARCHI_BEXTRACTU((value),1,1))
#define APB_SOC_CL_BYPASS_BYP_CFG_GETS(value)              (ARCHI_BEXTRACT((value),1,1))
#define APB_SOC_CL_BYPASS_BYP_CFG_SET(value,field)         (ARCHI_BINSERT((value),(field),1,1))
#define APB_SOC_CL_BYPASS_BYP_CFG(val)                     ((val) << 1)

#define APB_SOC_CL_BYPASS_CL_STATE_GET(value)              (ARCHI_BEXTRACTU((value),1,3))
#define APB_SOC_CL_BYPASS_CL_STATE_GETS(value)             (ARCHI_BEXTRACT((value),1,3))
#define APB_SOC_CL_BYPASS_CL_STATE_SET(value,field)        (ARCHI_BINSERT((value),(field),1,3))
#define APB_SOC_CL_BYPASS_CL_STATE(val)                    ((val) << 3)

#define APB_SOC_CL_BYPASS_CURRSET_GET(value)               (ARCHI_BEXTRACTU((value),3,4))
#define APB_SOC_CL_BYPASS_CURRSET_GETS(value)              (ARCHI_BEXTRACT((value),3,4))
#define APB_SOC_CL_BYPASS_CURRSET_SET(value,field)         (ARCHI_BINSERT((value),(field),3,4))
#define APB_SOC_CL_BYPASS_CURRSET(val)                     ((val) << 4)

#define APB_SOC_CL_BYPASS_PROG_DEL_GET(value)              (ARCHI_BEXTRACTU((value),2,7))
#define APB_SOC_CL_BYPASS_PROG_DEL_GETS(value)             (ARCHI_BEXTRACT((value),2,7))
#define APB_SOC_CL_BYPASS_PROG_DEL_SET(value,field)        (ARCHI_BINSERT((value),(field),2,7))
#define APB_SOC_CL_BYPASS_PROG_DEL(val)                    ((val) << 7)

#define APB_SOC_CL_BYPASS_BYP_CLK_GET(value)               (ARCHI_BEXTRACTU((value),1,9))
#define APB_SOC_CL_BYPASS_BYP_CLK_GETS(value)              (ARCHI_BEXTRACT((value),1,9))
#define APB_SOC_CL_BYPASS_BYP_CLK_SET(value,field)         (ARCHI_BINSERT((value),(field),1,9))
#define APB_SOC_CL_BYPASS_BYP_CLK(val)                     ((val) << 9)

#define APB_SOC_CL_BYPASS_CG_GET(value)                    (ARCHI_BEXTRACTU((value),1,10))
#define APB_SOC_CL_BYPASS_CG_GETS(value)                   (ARCHI_BEXTRACT((value),1,10))
#define APB_SOC_CL_BYPASS_CG_SET(value,field)              (ARCHI_BINSERT((value),(field),1,10))
#define APB_SOC_CL_BYPASS_CG(val)                          ((val) << 10)

#define APB_SOC_CL_BYPASS_FLL_PWD_GET(value)               (ARCHI_BEXTRACTU((value),1,11))
#define APB_SOC_CL_BYPASS_FLL_PWD_GETS(value)              (ARCHI_BEXTRACT((value),1,11))
#define APB_SOC_CL_BYPASS_FLL_PWD_SET(value,field)         (ARCHI_BINSERT((value),(field),1,11))
#define APB_SOC_CL_BYPASS_FLL_PWD(val)                     ((val) << 11)

#define APB_SOC_CL_BYPASS_FLL_RET_GET(value)               (ARCHI_BEXTRACTU((value),1,12))
#define APB_SOC_CL_BYPASS_FLL_RET_GETS(value)              (ARCHI_BEXTRACT((value),1,12))
#define APB_SOC_CL_BYPASS_FLL_RET_SET(value,field)         (ARCHI_BINSERT((value),(field),1,12))
#define APB_SOC_CL_BYPASS_FLL_RET(val)                     ((val) << 12)

#define APB_SOC_CL_BYPASS_RST_GET(value)                   (ARCHI_BEXTRACTU((value),1,13))
#define APB_SOC_CL_BYPASS_RST_GETS(value)                  (ARCHI_BEXTRACT((value),1,13))
#define APB_SOC_CL_BYPASS_RST_SET(value,field)             (ARCHI_BINSERT((value),(field),1,13))
#define APB_SOC_CL_BYPASS_RST(val)                         ((val) << 13)

#define APB_SOC_CL_BYPASS_BYP_ISO_GET(value)               (ARCHI_BEXTRACTU((value),1,14))
#define APB_SOC_CL_BYPASS_BYP_ISO_GETS(value)              (ARCHI_BEXTRACT((value),1,14))
#define APB_SOC_CL_BYPASS_BYP_ISO_SET(value,field)         (ARCHI_BINSERT((value),(field),1,14))
#define APB_SOC_CL_BYPASS_BYP_ISO(val)                     ((val) << 14)

#define APB_SOC_CL_BYPASS_PWISO_GET(value)                 (ARCHI_BEXTRACTU((value),1,15))
#define APB_SOC_CL_BYPASS_PWISO_GETS(value)                (ARCHI_BEXTRACT((value),1,15))
#define APB_SOC_CL_BYPASS_PWISO_SET(value,field)           (ARCHI_BINSERT((value),(field),1,15))
#define APB_SOC_CL_BYPASS_PWISO(val)                       ((val) << 15)

#define APB_SOC_CL_BYPASS_TRCPOWOK_GET(value)              (ARCHI_BEXTRACTU((value),1,16))
#define APB_SOC_CL_BYPASS_TRCPOWOK_GETS(value)             (ARCHI_BEXTRACT((value),1,16))
#define APB_SOC_CL_BYPASS_TRCPOWOK_SET(value,field)        (ARCHI_BINSERT((value),(field),1,16))
#define APB_SOC_CL_BYPASS_TRCPOWOK(val)                    ((val) << 16)

#define APB_SOC_CL_BYPASS_PMUPOWDOWN_GET(value)            (ARCHI_BEXTRACTU((value),1,17))
#define APB_SOC_CL_BYPASS_PMUPOWDOWN_GETS(value)           (ARCHI_BEXTRACT((value),1,17))
#define APB_SOC_CL_BYPASS_PMUPOWDOWN_SET(value,field)      (ARCHI_BINSERT((value),(field),1,17))
#define APB_SOC_CL_BYPASS_PMUPOWDOWN(val)                  ((val) << 17)

#define APB_SOC_JTAGREG_INT_SYNC_GET(value)                (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_JTAGREG_INT_SYNC_GETS(value)               (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_JTAGREG_INT_SYNC_SET(value,field)          (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_JTAGREG_INT_SYNC(val)                      ((val) << 0)

#define APB_SOC_JTAGREG_INT_BT_MD_GET(value)               (ARCHI_BEXTRACTU((value),3,1))
#define APB_SOC_JTAGREG_INT_BT_MD_GETS(value)              (ARCHI_BEXTRACT((value),3,1))
#define APB_SOC_JTAGREG_INT_BT_MD_SET(value,field)         (ARCHI_BINSERT((value),(field),3,1))
#define APB_SOC_JTAGREG_INT_BT_MD(val)                     ((val) << 1)

#define APB_SOC_JTAGREG_EXT_SYNC_GET(value)                (ARCHI_BEXTRACTU((value),1,8))
#define APB_SOC_JTAGREG_EXT_SYNC_GETS(value)               (ARCHI_BEXTRACT((value),1,8))
#define APB_SOC_JTAGREG_EXT_SYNC_SET(value,field)          (ARCHI_BINSERT((value),(field),1,8))
#define APB_SOC_JTAGREG_EXT_SYNC(val)                      ((val) << 8)

#define APB_SOC_JTAGREG_EXT_BT_MD_GET(value)               (ARCHI_BEXTRACTU((value),3,9))
#define APB_SOC_JTAGREG_EXT_BT_MD_GETS(value)              (ARCHI_BEXTRACT((value),3,9))
#define APB_SOC_JTAGREG_EXT_BT_MD_SET(value,field)         (ARCHI_BINSERT((value),(field),3,9))
#define APB_SOC_JTAGREG_EXT_BT_MD(val)                     ((val) << 9)

#define APB_SOC_L2_SLEEP_L2_SLEEP_GET(value)               (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_L2_SLEEP_L2_SLEEP_GETS(value)              (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_L2_SLEEP_L2_SLEEP_SET(value,field)         (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_L2_SLEEP_L2_SLEEP(val)                     ((val) << 0)

#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_GET(value)           (ARCHI_BEXTRACTU((value),32,0))
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_GETS(value)          (ARCHI_BEXTRACT((value),32,0))
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL_SET(value,field)     (ARCHI_BINSERT((value),(field),32,0))
#define APB_SOC_SLEEP_CTRL_SLEEP_CTRL(val)                 ((val) << 0)

#define APB_SOC_CORESTATUS_STATUS_GET(value)               (ARCHI_BEXTRACTU((value),32,0))
#define APB_SOC_CORESTATUS_STATUS_GETS(value)              (ARCHI_BEXTRACT((value),32,0))
#define APB_SOC_CORESTATUS_STATUS_SET(value,field)         (ARCHI_BINSERT((value),(field),32,0))
#define APB_SOC_CORESTATUS_STATUS(val)                     ((val) << 0)

#define APB_SOC_CORESTATUS_RO_STATUS_GET(value)            (ARCHI_BEXTRACTU((value),32,0))
#define APB_SOC_CORESTATUS_RO_STATUS_GETS(value)           (ARCHI_BEXTRACT((value),32,0))
#define APB_SOC_CORESTATUS_RO_STATUS_SET(value,field)      (ARCHI_BINSERT((value),(field),32,0))
#define APB_SOC_CORESTATUS_RO_STATUS(val)                  ((val) << 0)

#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_GET(value)            (ARCHI_BEXTRACTU((value),5,0))
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_GETS(value)           (ARCHI_BEXTRACT((value),5,0))
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT_SET(value,field)      (ARCHI_BINSERT((value),(field),5,0))
#define APB_SOC_SAFE_PMU_RAR_NV_VOLT(val)                  ((val) << 0)

#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_GET(value)            (ARCHI_BEXTRACTU((value),5,8))
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_GETS(value)           (ARCHI_BEXTRACT((value),5,8))
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT_SET(value,field)      (ARCHI_BINSERT((value),(field),5,8))
#define APB_SOC_SAFE_PMU_RAR_MV_VOLT(val)                  ((val) << 8)

#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_GET(value)            (ARCHI_BEXTRACTU((value),5,16))
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_GETS(value)           (ARCHI_BEXTRACT((value),5,16))
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT_SET(value,field)      (ARCHI_BINSERT((value),(field),5,16))
#define APB_SOC_SAFE_PMU_RAR_LV_VOLT(val)                  ((val) << 16)

#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_GET(value)            (ARCHI_BEXTRACTU((value),5,24))
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_GETS(value)           (ARCHI_BEXTRACT((value),5,24))
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT_SET(value,field)      (ARCHI_BINSERT((value),(field),5,24))
#define APB_SOC_SAFE_PMU_RAR_RV_VOLT(val)                  ((val) << 24)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_GET(value)        (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_GETS(value)       (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0_SET(value,field)  (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R0(val)              ((val) << 0)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_GET(value)        (ARCHI_BEXTRACTU((value),1,1))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_GETS(value)       (ARCHI_BEXTRACT((value),1,1))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1_SET(value,field)  (ARCHI_BINSERT((value),(field),1,1))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R1(val)              ((val) << 1)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_GET(value)        (ARCHI_BEXTRACTU((value),1,2))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_GETS(value)       (ARCHI_BEXTRACT((value),1,2))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2_SET(value,field)  (ARCHI_BINSERT((value),(field),1,2))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R2(val)              ((val) << 2)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_GET(value)        (ARCHI_BEXTRACTU((value),1,3))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_GETS(value)       (ARCHI_BEXTRACT((value),1,3))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3_SET(value,field)  (ARCHI_BINSERT((value),(field),1,3))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_L2_R3(val)              ((val) << 3)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_GET(value)      (ARCHI_BEXTRACTU((value),1,4))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_GETS(value)     (ARCHI_BEXTRACT((value),1,4))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL_SET(value,field) (ARCHI_BINSERT((value),(field),1,4))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SOC_FLL(val)            ((val) << 4)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_GET(value)       (ARCHI_BEXTRACTU((value),1,5))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_GETS(value)      (ARCHI_BEXTRACT((value),1,5))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL_SET(value,field) (ARCHI_BINSERT((value),(field),1,5))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_FLL(val)             ((val) << 5)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_GET(value)  (ARCHI_BEXTRACTU((value),5,6))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_GETS(value) (ARCHI_BEXTRACT((value),5,6))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC_SET(value,field) (ARCHI_BINSERT((value),(field),5,6))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_SRC(val)        ((val) << 6)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_GET(value) (ARCHI_BEXTRACTU((value),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE(val)       ((val) << 11)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_GET(value)   (ARCHI_BEXTRACTU((value),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_GETS(value)  (ARCHI_BEXTRACT((value),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN(val)         ((val) << 13)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_GET(value)    (ARCHI_BEXTRACTU((value),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_GETS(value)   (ARCHI_BEXTRACT((value),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_WAKESTATE(val)          ((val) << 15)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_GET(value)        (ARCHI_BEXTRACTU((value),1,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_GETS(value)       (ARCHI_BEXTRACT((value),1,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV_SET(value,field)  (ARCHI_BINSERT((value),(field),1,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTDEV(val)              ((val) << 16)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_GET(value)       (ARCHI_BEXTRACTU((value),1,17))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_GETS(value)      (ARCHI_BEXTRACT((value),1,17))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT_SET(value,field) (ARCHI_BINSERT((value),(field),1,17))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTINT(val)             ((val) << 17)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_GET(value)       (ARCHI_BEXTRACTU((value),2,18))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_GETS(value)      (ARCHI_BEXTRACT((value),2,18))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE_SET(value,field) (ARCHI_BINSERT((value),(field),2,18))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_BTTYPE(val)             ((val) << 18)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_GET(value)      (ARCHI_BEXTRACTU((value),1,20))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_GETS(value)     (ARCHI_BEXTRACT((value),1,20))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE_SET(value,field) (ARCHI_BINSERT((value),(field),1,20))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CL_WAKE(val)            ((val) << 20)

#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_GET(value)        (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_GETS(value)       (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0_SET(value,field)  (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R0(val)              ((val) << 0)

#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_GET(value)        (ARCHI_BEXTRACTU((value),1,1))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_GETS(value)       (ARCHI_BEXTRACT((value),1,1))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1_SET(value,field)  (ARCHI_BINSERT((value),(field),1,1))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R1(val)              ((val) << 1)

#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_GET(value)        (ARCHI_BEXTRACTU((value),1,2))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_GETS(value)       (ARCHI_BEXTRACT((value),1,2))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2_SET(value,field)  (ARCHI_BINSERT((value),(field),1,2))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R2(val)              ((val) << 2)

#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_GET(value)        (ARCHI_BEXTRACTU((value),1,3))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_GETS(value)       (ARCHI_BEXTRACT((value),1,3))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3_SET(value,field)  (ARCHI_BINSERT((value),(field),1,3))
#define APB_SOC_SAFE_PMU_FORCE_RET_L2_R3(val)              ((val) << 3)

#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_GET(value)         (ARCHI_BEXTRACTU((value),1,4))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_GETS(value)        (ARCHI_BEXTRACT((value),1,4))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0_SET(value,field)   (ARCHI_BINSERT((value),(field),1,4))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R0(val)               ((val) << 4)

#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_GET(value)         (ARCHI_BEXTRACTU((value),1,5))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_GETS(value)        (ARCHI_BEXTRACT((value),1,5))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1_SET(value,field)   (ARCHI_BINSERT((value),(field),1,5))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R1(val)               ((val) << 5)

#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_GET(value)         (ARCHI_BEXTRACTU((value),1,6))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_GETS(value)        (ARCHI_BEXTRACT((value),1,6))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2_SET(value,field)   (ARCHI_BINSERT((value),(field),1,6))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R2(val)               ((val) << 6)

#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_GET(value)         (ARCHI_BEXTRACTU((value),1,7))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_GETS(value)        (ARCHI_BEXTRACT((value),1,7))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3_SET(value,field)   (ARCHI_BINSERT((value),(field),1,7))
#define APB_SOC_SAFE_PMU_FORCE_PD_L2_R3(val)               ((val) << 7)

#define APB_SOC_SAFE_PADSLEEP_EN_GET(value)                (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_SAFE_PADSLEEP_EN_GETS(value)               (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_SAFE_PADSLEEP_EN_SET(value,field)          (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_SAFE_PADSLEEP_EN(val)                      ((val) << 0)

#define APB_SOC_REG_GPIO_ISO_ISO_GET(value)                (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_REG_GPIO_ISO_ISO_GETS(value)               (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_REG_GPIO_ISO_ISO_SET(value,field)          (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_REG_GPIO_ISO_ISO(val)                      ((val) << 0)

#define APB_SOC_REG_CAM_ISO_ISO_GET(value)                 (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_REG_CAM_ISO_ISO_GETS(value)                (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_REG_CAM_ISO_ISO_SET(value,field)           (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_REG_CAM_ISO_ISO(val)                       ((val) << 0)

#define APB_SOC_REG_LVDS_ISO_ISO_GET(value)                (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_REG_LVDS_ISO_ISO_GETS(value)               (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_REG_LVDS_ISO_ISO_SET(value,field)          (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_REG_LVDS_ISO_ISO(val)                      ((val) << 0)

#endif

#endif
