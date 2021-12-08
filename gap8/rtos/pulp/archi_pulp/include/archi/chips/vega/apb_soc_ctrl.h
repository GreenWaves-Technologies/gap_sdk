
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

#ifndef __INCLUDE_ARCHI_CHIPS_VEGA_APB_SOC_CTRL_H__
#define __INCLUDE_ARCHI_CHIPS_VEGA_APB_SOC_CTRL_H__

#ifndef LANGUAGE_ASSEMBLY

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS
//

// Core information register
#define APB_SOC_INFO_OFFSET                      0x0

// Boot address
#define APB_SOC_FC_BOOT_OFFSET                   0x4

// FC Fetch enable
#define APB_SOC_FC_FETCH_OFFSET                  0x8

// Isolate cluster register
#define APB_SOC_CL_ISOLATE_OFFSET                0xc

// Mux config register (pad 0-15)
#define APB_SOC_PADFUN0_OFFSET                   0x10

// Mux config register (pad 16-31)
#define APB_SOC_PADFUN1_OFFSET                   0x14

// Mux config register (pad 32-47)
#define APB_SOC_PADFUN2_OFFSET                   0x18

// Mux config register (pad 48-63)
#define APB_SOC_PADFUN3_OFFSET                   0x1c

// Function register (pad 0 to 3)
#define APB_SOC_PADCFG0_OFFSET                   0x20

// Function register (pad 4 to 7)
#define APB_SOC_PADCFG1_OFFSET                   0x24

// Function register (pad 8 to 11)
#define APB_SOC_PADCFG2_OFFSET                   0x28

// Function register (pad 12 to 15)
#define APB_SOC_PADCFG3_OFFSET                   0x2c

// Function register (pad 16 to 19)
#define APB_SOC_PADCFG4_OFFSET                   0x30

// Function register (pad 20 to 23)
#define APB_SOC_PADCFG5_OFFSET                   0x34

// Function register (pad 24 to 27)
#define APB_SOC_PADCFG6_OFFSET                   0x38

// Function register (pad 28 to 31)
#define APB_SOC_PADCFG7_OFFSET                   0x3c

// Function register (pad 32 to 35)
#define APB_SOC_PADCFG8_OFFSET                   0x40

// Function register (pad 36 to 39)
#define APB_SOC_PADCFG9_OFFSET                   0x44

// Function register (pad 40 to 43)
#define APB_SOC_PADCFG10_OFFSET                  0x48

// Function register (pad 44 to 47)
#define APB_SOC_PADCFG11_OFFSET                  0x4c

// Function register (pad 48 to 51)
#define APB_SOC_PADCFG12_OFFSET                  0x50

// Function register (pad 52 to 55)
#define APB_SOC_PADCFG13_OFFSET                  0x54

// Function register (pad 56 to 59)
#define APB_SOC_PADCFG14_OFFSET                  0x58

// Function register (pad 60 to 63)
#define APB_SOC_PADCFG15_OFFSET                  0x5c

// Cluster busy register
#define APB_SOC_CL_BUSY_OFFSET                   0x6c

// JTAG external register
#define APB_SOC_JTAGREG_OFFSET                   0x74

// Alias for SAFE_PMU_SLEEPCTRL
#define APB_SOC_SLEEP_CTRL_OFFSET                0x7c

// Clock divider for I3C
#define APB_SOC_CLK_DIV_I3C_OFFSET               0x80

// EOC and chip status register
#define APB_SOC_CORESTATUS_OFFSET                0xa0

// EOC and chip status register read mirror
#define APB_SOC_CORESTATUS_RO_OFFSET             0xc0

// Value of pad bootsel
#define APB_SOC_BOOTSEL_OFFSET                   0xc4

// Clear WD timer
#define APB_SOC_WD_CLR_OFFSET                    0xc8

// Clock selection for SOC,Cluster and Periph
#define APB_SOC_CLK_SEL_OFFSET                   0xd0

// SOC Clock Divider settings
#define APB_SOC_CLK_DIV_SOC_OFFSET               0xd4

// Cluster Clock Divider settings
#define APB_SOC_CLK_DIV_CLU_OFFSET               0xd8

// Peripheral Clock Divider Settings
#define APB_SOC_CLK_DIV_PER_OFFSET               0xdc

// nan
#define APB_SOC_SUPERVISOR_DBG_OFFSET            0xe0

// nan
#define APB_SOC_RWM_GRP0_OFFSET                  0xe4

// nan
#define APB_SOC_RWM_GRP1_OFFSET                  0xe8

// nan
#define APB_SOC_RWM_GRP2_OFFSET                  0xec

// nan
#define APB_SOC_RWM_GRP3_OFFSET                  0xf0

// nan
#define APB_SOC_RWM_GRP4_OFFSET                  0xf4

// nan
#define APB_SOC_RWM_GRP5_OFFSET                  0xf8

// nan
#define APB_SOC_RWM_GRP6_OFFSET                  0xfc

// Sleep modes configuration register
#define APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET        0x104

// Enables and configures WatchDog Timer
#define APB_SOC_SAFE_WD_OFFSET                   0x110

// Sleep config register (pad 0-7)
#define APB_SOC_SAFE_SLEEPPADCFG0_OFFSET         0x150

// Sleep config register (pad 8-15)
#define APB_SOC_SAFE_SLEEPPADCFG1_OFFSET         0x154

// Config timings for NEVA
#define APB_SOC_SAFE_NEVACF_OFFSET               0x164

// General purpouse register AO
#define APB_SOC_SAFE_GPREG_OFFSET                0x170

// L2 standby configuration
#define APB_SOC_SAFE_L2_BTRIM_STDBY_OFFSET       0x174

// FLL2 and FLL3 power control
#define APB_SOC_SAFE_FLL_CTRL_OFFSET             0x178

// L1 power control
#define APB_SOC_SAFE_L1_PWR_CTRL_OFFSET          0x17c

// L2 power control
#define APB_SOC_SAFE_L2_PWR_CTRL_OFFSET          0x180

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

// FC Boot Address (access: R/W)
#define APB_SOC_FC_BOOT_ADDR_BIT                                     0
#define APB_SOC_FC_BOOT_ADDR_WIDTH                                   32
#define APB_SOC_FC_BOOT_ADDR_MASK                                    0xffffffff

// FC Fetch Enable (access: R/W)
#define APB_SOC_FC_FETCH_FC_FE_BIT                                   0
#define APB_SOC_FC_FETCH_FC_FE_WIDTH                                 1
#define APB_SOC_FC_FETCH_FC_FE_MASK                                  0x1

// Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable (access: R/W)
#define APB_SOC_CL_ISOLATE_EN_BIT                                    0
#define APB_SOC_CL_ISOLATE_EN_WIDTH                                  1
#define APB_SOC_CL_ISOLATE_EN_MASK                                   0x1

// Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
#define APB_SOC_CL_BUSY_BUSY_BIT                                     0
#define APB_SOC_CL_BUSY_BUSY_WIDTH                                   1
#define APB_SOC_CL_BUSY_BUSY_MASK                                    0x1

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

// SoC domain clock selection: - 1b0: First FLL is used (FLL1) - 1b1: Second FLL is used (FLL2) (access: R/W)
#define APB_SOC_CLK_SEL_CLK_SOC_BIT                                  0
#define APB_SOC_CLK_SEL_CLK_SOC_WIDTH                                1
#define APB_SOC_CLK_SEL_CLK_SOC_MASK                                 0x1

// Cluster domain clock selection: - 2b00: First FLL is used (FLL1) - 2b01: Second FLL is used (FLL2) - 2b10: Third FLL is used (FLL3) (access: R/W)
#define APB_SOC_CLK_SEL_CLK_CLUSTER_BIT                              1
#define APB_SOC_CLK_SEL_CLK_CLUSTER_WIDTH                            2
#define APB_SOC_CLK_SEL_CLK_CLUSTER_MASK                             0x6

// User value which is kept retentive after wakeup (even in non-retentive sleep). This value is only partially interpreted by the ROM code (TBD) to distinguish betweem cold boot, non-retentive sleep and retentive sleep. (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_BIT                        0
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_WIDTH                      3
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_MASK                       0x7

// Enable smart wake-up; - 1'b0; smart wake-up disabled - 1'b1: smart wake-up enabled (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_BIT                  9
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_WIDTH                1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_MASK                 0x200

// Enable RTC wake-up; - 1'b0; rtc wake-up disabled - 1'b1: rtc wake-up enabled (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_BIT                    10
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_WIDTH                  1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_MASK                   0x400

// Select external wake-up  mode (through dedicated pin): - 2'b00: rise event - 2'b01: fall event - 2'b10: high level - 2'b11: low level (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT                  11
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH                2
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_MASK                 0x1800

// Enable external wake-up (through dedicated pin); - 1'b0; external wake-up disabled - 1'b1: external wake-up enabled (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT                    13
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH                  1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_MASK                   0x2000

// Power state of the MRAM to restore after warm boot - 1'b0: MRAM OFF - 1'b1: MRAM ON  (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_BIT                14
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_WIDTH              1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_MASK               0x4000

// Power state of the cluster to restore after warm boot - 1'b0: cluster OFF - 1'b1: cluster ON  (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_BIT             15
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_WIDTH           1
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_MASK            0x8000

// Configure retention mode of L2 memory. There is one bit per cut: - 1'b0: Non retentive - 1'b1: Retentive (access: R/W)
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_BIT                       16
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_WIDTH                     16
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_MASK                      0xffff0000

// Pad state when the chip is down with low-speed IOs ON: - 4b0000: Tristate with no pull-up and no pull-down - 4b0001: Tristate with pull-up - 4b0010: Tristate with pull-down - 4b0011: Drive 0 - 4b0100: Drive 1 (access: R/W)
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_BIT                          0
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH                        4
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_MASK                         0xf

// L2 cuts bias trim. This code is forwarded to all cuts and defines the level of current when the cut is in standby mode. 4b0000 is the least amount of current and 4b1110 is the most amount. 4bxxx1 is disabling the bias. (access: R/W)
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_BIT                        0
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_WIDTH                      4
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_MASK                       0xf

// L2 cuts standby active low. One bit per L2 cut for 16 cuts, the cut is in standby when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks. (access: R/W)
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_BIT                      4
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_WIDTH                    16
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_MASK                     0xffff0

// FLL2 power down. The FLL is powered down when this bit is 1b1. (access: R/W)
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_BIT                           0
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_WIDTH                         1
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_MASK                          0x1

// FLL3 power down. The FLL is powered down when this bit is 1b1. (access: R/W)
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_BIT                           1
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_WIDTH                         1
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_MASK                          0x2

// FLL2 reset active low. The FLL is reset when this bit is 1b0. (access: R/W)
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_BIT                          2
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_WIDTH                        1
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_MASK                         0x4

// FLL3 reset active low. The FLL is reset when this bit is 1b0. (access: R/W)
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_BIT                          3
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_WIDTH                        1
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_MASK                         0x8

// L1 cuts bias trim. This code is forwarded to all cuts and defines the level of current when the cut is in standby mode. 4b0000 is the least amount of current and 4b1110 is the most amount. 4bxxx1 is disabling the bias. (access: R/W)
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_BIT                           0
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_WIDTH                         4
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_MASK                          0xf

// L1 cuts standby active low. First bit is for first L1 64Kbytes and second bit for second L1 64Kbytes. (access: R/W)
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_BIT                         4
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_WIDTH                       2
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_MASK                        0x30

// L1 power down. The corresponding L1 part is powered down when this bit is 1b1. First bit is for first L1 64Kbytes and second bit for second L1 64Kbytes. (access: R/W)
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_BIT                             6
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_WIDTH                           2
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_MASK                            0xc0

// L2 VDDDE active low. One bit per L2 cut for 16 cuts, the cut periphery is supplied when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks. (access: R/W)
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_BIT                         0
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_WIDTH                       16
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_MASK                        0xffff

// L2 VDDME active low. One bit per L2 cut for 16 cuts, the cut array is supplied when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks. (access: R/W)
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_BIT                         16
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_WIDTH                       16
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_MASK                        0xffff0000



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

typedef union {
  struct {
    unsigned int nb_cl           :16; // Number of clusters
    unsigned int nb_cores        :16; // Number of cores
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_info_t;

typedef union {
  struct {
    unsigned int addr            :32; // FC Boot Address
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_fc_boot_t;

typedef union {
  struct {
    unsigned int fc_fe           :1 ; // FC Fetch Enable
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_fc_fetch_t;

typedef union {
  struct {
    unsigned int en              :1 ; // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_cl_isolate_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padfun0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padfun1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padfun2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padfun3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg4_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg5_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg6_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg7_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg8_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg9_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg10_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg11_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg12_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg13_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg14_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_padcfg15_t;

typedef union {
  struct {
    unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_cl_busy_t;

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
    unsigned int sleep_ctrl      :32; // Alias for SAFE_PMU_SLEEPCTRL(i.e. will be accessible in 1 clock cycle)
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_sleep_ctrl_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_clk_div_i3c_t;

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
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_bootsel_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_wd_clr_t;

typedef union {
  struct {
    unsigned int clk_soc         :1 ; // SoC domain clock selection: - 1b0: First FLL is used (FLL1) - 1b1: Second FLL is used (FLL2)
    unsigned int clk_cluster     :2 ; // Cluster domain clock selection: - 2b00: First FLL is used (FLL1) - 2b01: Second FLL is used (FLL2) - 2b10: Third FLL is used (FLL3)
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_clk_sel_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_clk_div_soc_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_clk_div_clu_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_clk_div_per_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_supervisor_dbg_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp4_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp5_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_rwm_grp6_t;

typedef union {
  struct {
    unsigned int reboot          :3 ; // User value which is kept retentive after wakeup (even in non-retentive sleep). This value is only partially interpreted by the ROM code (TBD) to distinguish betweem cold boot, non-retentive sleep and retentive sleep.
    unsigned int padding0:6 ;
    unsigned int smartwake_en    :1 ; // Enable smart wake-up; - 1'b0; smart wake-up disabled - 1'b1: smart wake-up enabled
    unsigned int rtcwake_en      :1 ; // Enable RTC wake-up; - 1'b0; rtc wake-up disabled - 1'b1: rtc wake-up enabled
    unsigned int extwake_type    :2 ; // Select external wake-up  mode (through dedicated pin): - 2'b00: rise event - 2'b01: fall event - 2'b10: high level - 2'b11: low level
    unsigned int extwake_en      :1 ; // Enable external wake-up (through dedicated pin); - 1'b0; external wake-up disabled - 1'b1: external wake-up enabled
    unsigned int mram_wakestate  :1 ; // Power state of the MRAM to restore after warm boot - 1'b0: MRAM OFF - 1'b1: MRAM ON 
    unsigned int cluster_wakestate:1 ; // Power state of the cluster to restore after warm boot - 1'b0: cluster OFF - 1'b1: cluster ON 
    unsigned int ret_mem         :16; // Configure retention mode of L2 memory. There is one bit per cut: - 1'b0: Non retentive - 1'b1: Retentive
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_pmu_sleepctrl_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_wd_t;

typedef union {
  struct {
    unsigned int state           :4 ; // Pad state when the chip is down with low-speed IOs ON: - 4b0000: Tristate with no pull-up and no pull-down - 4b0001: Tristate with pull-up - 4b0010: Tristate with pull-down - 4b0011: Drive 0 - 4b0100: Drive 1
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
} __attribute__((packed)) apb_soc_safe_nevacf_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_gpreg_t;

typedef union {
  struct {
    unsigned int btrim           :4 ; // L2 cuts bias trim. This code is forwarded to all cuts and defines the level of current when the cut is in standby mode. 4b0000 is the least amount of current and 4b1110 is the most amount. 4bxxx1 is disabling the bias.
    unsigned int stdby_n         :16; // L2 cuts standby active low. One bit per L2 cut for 16 cuts, the cut is in standby when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_l2_btrim_stdby_t;

typedef union {
  struct {
    unsigned int fll2_pwd        :1 ; // FLL2 power down. The FLL is powered down when this bit is 1b1.
    unsigned int fll3_pwd        :1 ; // FLL3 power down. The FLL is powered down when this bit is 1b1.
    unsigned int fll2_rstb       :1 ; // FLL2 reset active low. The FLL is reset when this bit is 1b0.
    unsigned int fll3_rstb       :1 ; // FLL3 reset active low. The FLL is reset when this bit is 1b0.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_fll_ctrl_t;

typedef union {
  struct {
    unsigned int btrim           :4 ; // L1 cuts bias trim. This code is forwarded to all cuts and defines the level of current when the cut is in standby mode. 4b0000 is the least amount of current and 4b1110 is the most amount. 4bxxx1 is disabling the bias.
    unsigned int stdby_n         :2 ; // L1 cuts standby active low. First bit is for first L1 64Kbytes and second bit for second L1 64Kbytes.
    unsigned int pwd             :2 ; // L1 power down. The corresponding L1 part is powered down when this bit is 1b1. First bit is for first L1 64Kbytes and second bit for second L1 64Kbytes.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_l1_pwr_ctrl_t;

typedef union {
  struct {
    unsigned int vddde_n         :16; // L2 VDDDE active low. One bit per L2 cut for 16 cuts, the cut periphery is supplied when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks.
    unsigned int vddme_n         :16; // L2 VDDME active low. One bit per L2 cut for 16 cuts, the cut array is supplied when its corresponding bit is 0. The 4 last bits are for the private banks and the rest for the shared banks.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_safe_l2_pwr_ctrl_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_reg_gpio_iso_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_reg_cam_iso_t;

typedef union {
  struct {
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

class vp_apb_soc_fc_boot : public vp::reg_32
{
public:
  inline void addr_set(uint32_t value) { this->set_field(value, APB_SOC_FC_BOOT_ADDR_BIT, APB_SOC_FC_BOOT_ADDR_WIDTH); }
  inline uint32_t addr_get() { return this->get_field(APB_SOC_FC_BOOT_ADDR_BIT, APB_SOC_FC_BOOT_ADDR_WIDTH); }
};

class vp_apb_soc_fc_fetch : public vp::reg_32
{
public:
  inline void fc_fe_set(uint32_t value) { this->set_field(value, APB_SOC_FC_FETCH_FC_FE_BIT, APB_SOC_FC_FETCH_FC_FE_WIDTH); }
  inline uint32_t fc_fe_get() { return this->get_field(APB_SOC_FC_FETCH_FC_FE_BIT, APB_SOC_FC_FETCH_FC_FE_WIDTH); }
};

class vp_apb_soc_cl_isolate : public vp::reg_32
{
public:
  inline void en_set(uint32_t value) { this->set_field(value, APB_SOC_CL_ISOLATE_EN_BIT, APB_SOC_CL_ISOLATE_EN_WIDTH); }
  inline uint32_t en_get() { return this->get_field(APB_SOC_CL_ISOLATE_EN_BIT, APB_SOC_CL_ISOLATE_EN_WIDTH); }
};

class vp_apb_soc_padfun0 : public vp::reg_32
{
public:
};

class vp_apb_soc_padfun1 : public vp::reg_32
{
public:
};

class vp_apb_soc_padfun2 : public vp::reg_32
{
public:
};

class vp_apb_soc_padfun3 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg0 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg1 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg2 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg3 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg4 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg5 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg6 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg7 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg8 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg9 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg10 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg11 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg12 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg13 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg14 : public vp::reg_32
{
public:
};

class vp_apb_soc_padcfg15 : public vp::reg_32
{
public:
};

class vp_apb_soc_cl_busy : public vp::reg_32
{
public:
  inline void busy_set(uint32_t value) { this->set_field(value, APB_SOC_CL_BUSY_BUSY_BIT, APB_SOC_CL_BUSY_BUSY_WIDTH); }
  inline uint32_t busy_get() { return this->get_field(APB_SOC_CL_BUSY_BUSY_BIT, APB_SOC_CL_BUSY_BUSY_WIDTH); }
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

class vp_apb_soc_sleep_ctrl : public vp::reg_32
{
public:
  inline void sleep_ctrl_set(uint32_t value) { this->set_field(value, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_BIT, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_WIDTH); }
  inline uint32_t sleep_ctrl_get() { return this->get_field(APB_SOC_SLEEP_CTRL_SLEEP_CTRL_BIT, APB_SOC_SLEEP_CTRL_SLEEP_CTRL_WIDTH); }
};

class vp_apb_soc_clk_div_i3c : public vp::reg_32
{
public:
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

class vp_apb_soc_bootsel : public vp::reg_32
{
public:
};

class vp_apb_soc_wd_clr : public vp::reg_32
{
public:
};

class vp_apb_soc_clk_sel : public vp::reg_32
{
public:
  inline void clk_soc_set(uint32_t value) { this->set_field(value, APB_SOC_CLK_SEL_CLK_SOC_BIT, APB_SOC_CLK_SEL_CLK_SOC_WIDTH); }
  inline uint32_t clk_soc_get() { return this->get_field(APB_SOC_CLK_SEL_CLK_SOC_BIT, APB_SOC_CLK_SEL_CLK_SOC_WIDTH); }
  inline void clk_cluster_set(uint32_t value) { this->set_field(value, APB_SOC_CLK_SEL_CLK_CLUSTER_BIT, APB_SOC_CLK_SEL_CLK_CLUSTER_WIDTH); }
  inline uint32_t clk_cluster_get() { return this->get_field(APB_SOC_CLK_SEL_CLK_CLUSTER_BIT, APB_SOC_CLK_SEL_CLK_CLUSTER_WIDTH); }
};

class vp_apb_soc_clk_div_soc : public vp::reg_32
{
public:
};

class vp_apb_soc_clk_div_clu : public vp::reg_32
{
public:
};

class vp_apb_soc_clk_div_per : public vp::reg_32
{
public:
};

class vp_apb_soc_supervisor_dbg : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp0 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp1 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp2 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp3 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp4 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp5 : public vp::reg_32
{
public:
};

class vp_apb_soc_rwm_grp6 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_pmu_sleepctrl : public vp::reg_32
{
public:
  inline void reboot_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_WIDTH); }
  inline uint32_t reboot_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_WIDTH); }
  inline void smartwake_en_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_WIDTH); }
  inline uint32_t smartwake_en_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_WIDTH); }
  inline void rtcwake_en_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_WIDTH); }
  inline uint32_t rtcwake_en_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_WIDTH); }
  inline void extwake_type_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH); }
  inline uint32_t extwake_type_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_WIDTH); }
  inline void extwake_en_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH); }
  inline uint32_t extwake_en_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_WIDTH); }
  inline void mram_wakestate_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_WIDTH); }
  inline uint32_t mram_wakestate_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_WIDTH); }
  inline void cluster_wakestate_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_WIDTH); }
  inline uint32_t cluster_wakestate_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_WIDTH); }
  inline void ret_mem_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_WIDTH); }
  inline uint32_t ret_mem_get() { return this->get_field(APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_BIT, APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_WIDTH); }
};

class vp_apb_soc_safe_wd : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_sleeppadcfg0 : public vp::reg_32
{
public:
  inline void state_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_SLEEPPADCFG0_STATE_BIT, APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH); }
  inline uint32_t state_get() { return this->get_field(APB_SOC_SAFE_SLEEPPADCFG0_STATE_BIT, APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH); }
};

class vp_apb_soc_safe_sleeppadcfg1 : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_nevacf : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_gpreg : public vp::reg_32
{
public:
};

class vp_apb_soc_safe_l2_btrim_stdby : public vp::reg_32
{
public:
  inline void btrim_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_BIT, APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_WIDTH); }
  inline uint32_t btrim_get() { return this->get_field(APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_BIT, APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_WIDTH); }
  inline void stdby_n_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_BIT, APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_WIDTH); }
  inline uint32_t stdby_n_get() { return this->get_field(APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_BIT, APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_WIDTH); }
};

class vp_apb_soc_safe_fll_ctrl : public vp::reg_32
{
public:
  inline void fll2_pwd_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_BIT, APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_WIDTH); }
  inline uint32_t fll2_pwd_get() { return this->get_field(APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_BIT, APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_WIDTH); }
  inline void fll3_pwd_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_BIT, APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_WIDTH); }
  inline uint32_t fll3_pwd_get() { return this->get_field(APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_BIT, APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_WIDTH); }
  inline void fll2_rstb_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_BIT, APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_WIDTH); }
  inline uint32_t fll2_rstb_get() { return this->get_field(APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_BIT, APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_WIDTH); }
  inline void fll3_rstb_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_BIT, APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_WIDTH); }
  inline uint32_t fll3_rstb_get() { return this->get_field(APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_BIT, APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_WIDTH); }
};

class vp_apb_soc_safe_l1_pwr_ctrl : public vp::reg_32
{
public:
  inline void btrim_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_BIT, APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_WIDTH); }
  inline uint32_t btrim_get() { return this->get_field(APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_BIT, APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_WIDTH); }
  inline void stdby_n_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_BIT, APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_WIDTH); }
  inline uint32_t stdby_n_get() { return this->get_field(APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_BIT, APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_WIDTH); }
  inline void pwd_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L1_PWR_CTRL_PWD_BIT, APB_SOC_SAFE_L1_PWR_CTRL_PWD_WIDTH); }
  inline uint32_t pwd_get() { return this->get_field(APB_SOC_SAFE_L1_PWR_CTRL_PWD_BIT, APB_SOC_SAFE_L1_PWR_CTRL_PWD_WIDTH); }
};

class vp_apb_soc_safe_l2_pwr_ctrl : public vp::reg_32
{
public:
  inline void vddde_n_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_BIT, APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_WIDTH); }
  inline uint32_t vddde_n_get() { return this->get_field(APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_BIT, APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_WIDTH); }
  inline void vddme_n_set(uint32_t value) { this->set_field(value, APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_BIT, APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_WIDTH); }
  inline uint32_t vddme_n_get() { return this->get_field(APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_BIT, APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_WIDTH); }
};

class vp_apb_soc_reg_gpio_iso : public vp::reg_32
{
public:
};

class vp_apb_soc_reg_cam_iso : public vp::reg_32
{
public:
};

class vp_apb_soc_reg_lvds_iso : public vp::reg_32
{
public:
};

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
  unsigned int info            ; // Core information register
  unsigned int fc_boot         ; // Boot address
  unsigned int fc_fetch        ; // FC Fetch enable
  unsigned int cl_isolate      ; // Isolate cluster register
  unsigned int padfun0         ; // Mux config register (pad 0-15)
  unsigned int padfun1         ; // Mux config register (pad 16-31)
  unsigned int padfun2         ; // Mux config register (pad 32-47)
  unsigned int padfun3         ; // Mux config register (pad 48-63)
  unsigned int padcfg0         ; // Function register (pad 0 to 3)
  unsigned int padcfg1         ; // Function register (pad 4 to 7)
  unsigned int padcfg2         ; // Function register (pad 8 to 11)
  unsigned int padcfg3         ; // Function register (pad 12 to 15)
  unsigned int padcfg4         ; // Function register (pad 16 to 19)
  unsigned int padcfg5         ; // Function register (pad 20 to 23)
  unsigned int padcfg6         ; // Function register (pad 24 to 27)
  unsigned int padcfg7         ; // Function register (pad 28 to 31)
  unsigned int padcfg8         ; // Function register (pad 32 to 35)
  unsigned int padcfg9         ; // Function register (pad 36 to 39)
  unsigned int padcfg10        ; // Function register (pad 40 to 43)
  unsigned int padcfg11        ; // Function register (pad 44 to 47)
  unsigned int padcfg12        ; // Function register (pad 48 to 51)
  unsigned int padcfg13        ; // Function register (pad 52 to 55)
  unsigned int padcfg14        ; // Function register (pad 56 to 59)
  unsigned int padcfg15        ; // Function register (pad 60 to 63)
  unsigned int cl_busy         ; // Cluster busy register
  unsigned int jtagreg         ; // JTAG external register
  unsigned int sleep_ctrl      ; // Alias for SAFE_PMU_SLEEPCTRL
  unsigned int clk_div_i3c     ; // Clock divider for I3C
  unsigned int corestatus      ; // EOC and chip status register
  unsigned int corestatus_ro   ; // EOC and chip status register read mirror
  unsigned int bootsel         ; // Value of pad bootsel
  unsigned int wd_clr          ; // Clear WD timer
  unsigned int clk_sel         ; // Clock selection for SOC,Cluster and Periph
  unsigned int clk_div_soc     ; // SOC Clock Divider settings
  unsigned int clk_div_clu     ; // Cluster Clock Divider settings
  unsigned int clk_div_per     ; // Peripheral Clock Divider Settings
  unsigned int supervisor_dbg  ; // nan
  unsigned int rwm_grp0        ; // nan
  unsigned int rwm_grp1        ; // nan
  unsigned int rwm_grp2        ; // nan
  unsigned int rwm_grp3        ; // nan
  unsigned int rwm_grp4        ; // nan
  unsigned int rwm_grp5        ; // nan
  unsigned int rwm_grp6        ; // nan
  unsigned int safe_pmu_sleepctrl; // Sleep modes configuration register
  unsigned int safe_wd         ; // Enables and configures WatchDog Timer
  unsigned int safe_sleeppadcfg0; // Sleep config register (pad 0-7)
  unsigned int safe_sleeppadcfg1; // Sleep config register (pad 8-15)
  unsigned int safe_nevacf     ; // Config timings for NEVA
  unsigned int safe_gpreg      ; // General purpouse register AO
  unsigned int safe_l2_btrim_stdby; // L2 standby configuration
  unsigned int safe_fll_ctrl   ; // FLL2 and FLL3 power control
  unsigned int safe_l1_pwr_ctrl; // L1 power control
  unsigned int safe_l2_pwr_ctrl; // L2 power control
  unsigned int reg_gpio_iso    ; // GPIO power domain pad input isolation register
  unsigned int reg_cam_iso     ; // CAM power domain pad input isolation register
  unsigned int reg_lvds_iso    ; // LVDS power domain pad input isolation register
} __attribute__((packed)) apb_soc_apb_soc_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

static inline uint32_t apb_soc_info_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_INFO_OFFSET); }
static inline void apb_soc_info_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_INFO_OFFSET, value); }

static inline uint32_t apb_soc_fc_boot_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_FC_BOOT_OFFSET); }
static inline void apb_soc_fc_boot_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_FC_BOOT_OFFSET, value); }

static inline uint32_t apb_soc_fc_fetch_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_FC_FETCH_OFFSET); }
static inline void apb_soc_fc_fetch_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_FC_FETCH_OFFSET, value); }

static inline uint32_t apb_soc_cl_isolate_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CL_ISOLATE_OFFSET); }
static inline void apb_soc_cl_isolate_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CL_ISOLATE_OFFSET, value); }

static inline uint32_t apb_soc_padfun0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADFUN0_OFFSET); }
static inline void apb_soc_padfun0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADFUN0_OFFSET, value); }

static inline uint32_t apb_soc_padfun1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADFUN1_OFFSET); }
static inline void apb_soc_padfun1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADFUN1_OFFSET, value); }

static inline uint32_t apb_soc_padfun2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADFUN2_OFFSET); }
static inline void apb_soc_padfun2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADFUN2_OFFSET, value); }

static inline uint32_t apb_soc_padfun3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADFUN3_OFFSET); }
static inline void apb_soc_padfun3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADFUN3_OFFSET, value); }

static inline uint32_t apb_soc_padcfg0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG0_OFFSET); }
static inline void apb_soc_padcfg0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG0_OFFSET, value); }

static inline uint32_t apb_soc_padcfg1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG1_OFFSET); }
static inline void apb_soc_padcfg1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG1_OFFSET, value); }

static inline uint32_t apb_soc_padcfg2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG2_OFFSET); }
static inline void apb_soc_padcfg2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG2_OFFSET, value); }

static inline uint32_t apb_soc_padcfg3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG3_OFFSET); }
static inline void apb_soc_padcfg3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG3_OFFSET, value); }

static inline uint32_t apb_soc_padcfg4_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG4_OFFSET); }
static inline void apb_soc_padcfg4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG4_OFFSET, value); }

static inline uint32_t apb_soc_padcfg5_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG5_OFFSET); }
static inline void apb_soc_padcfg5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG5_OFFSET, value); }

static inline uint32_t apb_soc_padcfg6_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG6_OFFSET); }
static inline void apb_soc_padcfg6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG6_OFFSET, value); }

static inline uint32_t apb_soc_padcfg7_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG7_OFFSET); }
static inline void apb_soc_padcfg7_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG7_OFFSET, value); }

static inline uint32_t apb_soc_padcfg8_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG8_OFFSET); }
static inline void apb_soc_padcfg8_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG8_OFFSET, value); }

static inline uint32_t apb_soc_padcfg9_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG9_OFFSET); }
static inline void apb_soc_padcfg9_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG9_OFFSET, value); }

static inline uint32_t apb_soc_padcfg10_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG10_OFFSET); }
static inline void apb_soc_padcfg10_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG10_OFFSET, value); }

static inline uint32_t apb_soc_padcfg11_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG11_OFFSET); }
static inline void apb_soc_padcfg11_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG11_OFFSET, value); }

static inline uint32_t apb_soc_padcfg12_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG12_OFFSET); }
static inline void apb_soc_padcfg12_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG12_OFFSET, value); }

static inline uint32_t apb_soc_padcfg13_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG13_OFFSET); }
static inline void apb_soc_padcfg13_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG13_OFFSET, value); }

static inline uint32_t apb_soc_padcfg14_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG14_OFFSET); }
static inline void apb_soc_padcfg14_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG14_OFFSET, value); }

static inline uint32_t apb_soc_padcfg15_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_PADCFG15_OFFSET); }
static inline void apb_soc_padcfg15_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_PADCFG15_OFFSET, value); }

static inline uint32_t apb_soc_cl_busy_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CL_BUSY_OFFSET); }
static inline void apb_soc_cl_busy_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CL_BUSY_OFFSET, value); }

static inline uint32_t apb_soc_jtagreg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_JTAGREG_OFFSET); }
static inline void apb_soc_jtagreg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_JTAGREG_OFFSET, value); }

static inline uint32_t apb_soc_sleep_ctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SLEEP_CTRL_OFFSET); }
static inline void apb_soc_sleep_ctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SLEEP_CTRL_OFFSET, value); }

static inline uint32_t apb_soc_clk_div_i3c_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CLK_DIV_I3C_OFFSET); }
static inline void apb_soc_clk_div_i3c_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CLK_DIV_I3C_OFFSET, value); }

static inline uint32_t apb_soc_corestatus_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CORESTATUS_OFFSET); }
static inline void apb_soc_corestatus_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CORESTATUS_OFFSET, value); }

static inline uint32_t apb_soc_corestatus_ro_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CORESTATUS_RO_OFFSET); }
static inline void apb_soc_corestatus_ro_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CORESTATUS_RO_OFFSET, value); }

static inline uint32_t apb_soc_bootsel_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_BOOTSEL_OFFSET); }
static inline void apb_soc_bootsel_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_BOOTSEL_OFFSET, value); }

static inline uint32_t apb_soc_wd_clr_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_WD_CLR_OFFSET); }
static inline void apb_soc_wd_clr_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_WD_CLR_OFFSET, value); }

static inline uint32_t apb_soc_clk_sel_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CLK_SEL_OFFSET); }
static inline void apb_soc_clk_sel_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CLK_SEL_OFFSET, value); }

static inline uint32_t apb_soc_clk_div_soc_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CLK_DIV_SOC_OFFSET); }
static inline void apb_soc_clk_div_soc_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CLK_DIV_SOC_OFFSET, value); }

static inline uint32_t apb_soc_clk_div_clu_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CLK_DIV_CLU_OFFSET); }
static inline void apb_soc_clk_div_clu_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CLK_DIV_CLU_OFFSET, value); }

static inline uint32_t apb_soc_clk_div_per_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_CLK_DIV_PER_OFFSET); }
static inline void apb_soc_clk_div_per_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_CLK_DIV_PER_OFFSET, value); }

static inline uint32_t apb_soc_supervisor_dbg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SUPERVISOR_DBG_OFFSET); }
static inline void apb_soc_supervisor_dbg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SUPERVISOR_DBG_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP0_OFFSET); }
static inline void apb_soc_rwm_grp0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP0_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP1_OFFSET); }
static inline void apb_soc_rwm_grp1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP1_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp2_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP2_OFFSET); }
static inline void apb_soc_rwm_grp2_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP2_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp3_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP3_OFFSET); }
static inline void apb_soc_rwm_grp3_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP3_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp4_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP4_OFFSET); }
static inline void apb_soc_rwm_grp4_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP4_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp5_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP5_OFFSET); }
static inline void apb_soc_rwm_grp5_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP5_OFFSET, value); }

static inline uint32_t apb_soc_rwm_grp6_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_RWM_GRP6_OFFSET); }
static inline void apb_soc_rwm_grp6_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_RWM_GRP6_OFFSET, value); }

static inline uint32_t apb_soc_safe_pmu_sleepctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET); }
static inline void apb_soc_safe_pmu_sleepctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET, value); }

static inline uint32_t apb_soc_safe_wd_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_WD_OFFSET); }
static inline void apb_soc_safe_wd_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_WD_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg0_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg0_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET, value); }

static inline uint32_t apb_soc_safe_sleeppadcfg1_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_SLEEPPADCFG1_OFFSET); }
static inline void apb_soc_safe_sleeppadcfg1_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_SLEEPPADCFG1_OFFSET, value); }

static inline uint32_t apb_soc_safe_nevacf_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_NEVACF_OFFSET); }
static inline void apb_soc_safe_nevacf_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_NEVACF_OFFSET, value); }

static inline uint32_t apb_soc_safe_gpreg_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_GPREG_OFFSET); }
static inline void apb_soc_safe_gpreg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_GPREG_OFFSET, value); }

static inline uint32_t apb_soc_safe_l2_btrim_stdby_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_L2_BTRIM_STDBY_OFFSET); }
static inline void apb_soc_safe_l2_btrim_stdby_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_L2_BTRIM_STDBY_OFFSET, value); }

static inline uint32_t apb_soc_safe_fll_ctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_FLL_CTRL_OFFSET); }
static inline void apb_soc_safe_fll_ctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_FLL_CTRL_OFFSET, value); }

static inline uint32_t apb_soc_safe_l1_pwr_ctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_L1_PWR_CTRL_OFFSET); }
static inline void apb_soc_safe_l1_pwr_ctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_L1_PWR_CTRL_OFFSET, value); }

static inline uint32_t apb_soc_safe_l2_pwr_ctrl_get(uint32_t base) { return ARCHI_READ(base, APB_SOC_SAFE_L2_PWR_CTRL_OFFSET); }
static inline void apb_soc_safe_l2_pwr_ctrl_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, APB_SOC_SAFE_L2_PWR_CTRL_OFFSET, value); }

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

#ifndef LANGUAGE_ASSEMBLY

#define APB_SOC_INFO_NB_CL_GET(value)                      (ARCHI_BEXTRACTU((value),16,0))
#define APB_SOC_INFO_NB_CL_GETS(value)                     (ARCHI_BEXTRACT((value),16,0))
#define APB_SOC_INFO_NB_CL_SET(value,field)                (ARCHI_BINSERT((value),(field),16,0))
#define APB_SOC_INFO_NB_CL(val)                            ((val) << 0)

#define APB_SOC_INFO_NB_CORES_GET(value)                   (ARCHI_BEXTRACTU((value),16,16))
#define APB_SOC_INFO_NB_CORES_GETS(value)                  (ARCHI_BEXTRACT((value),16,16))
#define APB_SOC_INFO_NB_CORES_SET(value,field)             (ARCHI_BINSERT((value),(field),16,16))
#define APB_SOC_INFO_NB_CORES(val)                         ((val) << 16)

#define APB_SOC_FC_BOOT_ADDR_GET(value)                    (ARCHI_BEXTRACTU((value),32,0))
#define APB_SOC_FC_BOOT_ADDR_GETS(value)                   (ARCHI_BEXTRACT((value),32,0))
#define APB_SOC_FC_BOOT_ADDR_SET(value,field)              (ARCHI_BINSERT((value),(field),32,0))
#define APB_SOC_FC_BOOT_ADDR(val)                          ((val) << 0)

#define APB_SOC_FC_FETCH_FC_FE_GET(value)                  (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_FC_FETCH_FC_FE_GETS(value)                 (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_FC_FETCH_FC_FE_SET(value,field)            (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_FC_FETCH_FC_FE(val)                        ((val) << 0)

#define APB_SOC_CL_ISOLATE_EN_GET(value)                   (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CL_ISOLATE_EN_GETS(value)                  (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CL_ISOLATE_EN_SET(value,field)             (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CL_ISOLATE_EN(val)                         ((val) << 0)

#define APB_SOC_CL_BUSY_BUSY_GET(value)                    (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CL_BUSY_BUSY_GETS(value)                   (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CL_BUSY_BUSY_SET(value,field)              (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CL_BUSY_BUSY(val)                          ((val) << 0)

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

#define APB_SOC_CLK_SEL_CLK_SOC_GET(value)                 (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_CLK_SEL_CLK_SOC_GETS(value)                (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_CLK_SEL_CLK_SOC_SET(value,field)           (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_CLK_SEL_CLK_SOC(val)                       ((val) << 0)

#define APB_SOC_CLK_SEL_CLK_CLUSTER_GET(value)             (ARCHI_BEXTRACTU((value),2,1))
#define APB_SOC_CLK_SEL_CLK_CLUSTER_GETS(value)            (ARCHI_BEXTRACT((value),2,1))
#define APB_SOC_CLK_SEL_CLK_CLUSTER_SET(value,field)       (ARCHI_BINSERT((value),(field),2,1))
#define APB_SOC_CLK_SEL_CLK_CLUSTER(val)                   ((val) << 1)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_GET(value)       (ARCHI_BEXTRACTU((value),3,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_GETS(value)      (ARCHI_BEXTRACT((value),3,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT_SET(value,field) (ARCHI_BINSERT((value),(field),3,0))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_REBOOT(val)             ((val) << 0)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_GET(value) (ARCHI_BEXTRACTU((value),1,9))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_GETS(value) (ARCHI_BEXTRACT((value),1,9))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,9))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN(val)       ((val) << 9)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_GET(value)   (ARCHI_BEXTRACTU((value),1,10))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_GETS(value)  (ARCHI_BEXTRACT((value),1,10))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,10))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN(val)         ((val) << 10)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_GET(value) (ARCHI_BEXTRACTU((value),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_GETS(value) (ARCHI_BEXTRACT((value),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_SET(value,field) (ARCHI_BINSERT((value),(field),2,11))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE(val)       ((val) << 11)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_GET(value)   (ARCHI_BEXTRACTU((value),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_GETS(value)  (ARCHI_BEXTRACT((value),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_SET(value,field) (ARCHI_BINSERT((value),(field),1,13))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN(val)         ((val) << 13)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_GET(value) (ARCHI_BEXTRACTU((value),1,14))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_GETS(value) (ARCHI_BEXTRACT((value),1,14))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_SET(value,field) (ARCHI_BINSERT((value),(field),1,14))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE(val)     ((val) << 14)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_GET(value) (ARCHI_BEXTRACTU((value),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_GETS(value) (ARCHI_BEXTRACT((value),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_SET(value,field) (ARCHI_BINSERT((value),(field),1,15))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE(val)  ((val) << 15)

#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_GET(value)      (ARCHI_BEXTRACTU((value),16,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_GETS(value)     (ARCHI_BEXTRACT((value),16,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM_SET(value,field) (ARCHI_BINSERT((value),(field),16,16))
#define APB_SOC_SAFE_PMU_SLEEPCTRL_RET_MEM(val)            ((val) << 16)

#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_GET(value)         (ARCHI_BEXTRACTU((value),4,0))
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_GETS(value)        (ARCHI_BEXTRACT((value),4,0))
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE_SET(value,field)   (ARCHI_BINSERT((value),(field),4,0))
#define APB_SOC_SAFE_SLEEPPADCFG0_STATE(val)               ((val) << 0)

#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_GET(value)       (ARCHI_BEXTRACTU((value),4,0))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_GETS(value)      (ARCHI_BEXTRACT((value),4,0))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM_SET(value,field) (ARCHI_BINSERT((value),(field),4,0))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_BTRIM(val)             ((val) << 0)

#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_GET(value)     (ARCHI_BEXTRACTU((value),16,4))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_GETS(value)    (ARCHI_BEXTRACT((value),16,4))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N_SET(value,field) (ARCHI_BINSERT((value),(field),16,4))
#define APB_SOC_SAFE_L2_BTRIM_STDBY_STDBY_N(val)           ((val) << 4)

#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_GET(value)          (ARCHI_BEXTRACTU((value),1,0))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_GETS(value)         (ARCHI_BEXTRACT((value),1,0))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD_SET(value,field)    (ARCHI_BINSERT((value),(field),1,0))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_PWD(val)                ((val) << 0)

#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_GET(value)          (ARCHI_BEXTRACTU((value),1,1))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_GETS(value)         (ARCHI_BEXTRACT((value),1,1))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD_SET(value,field)    (ARCHI_BINSERT((value),(field),1,1))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_PWD(val)                ((val) << 1)

#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_GET(value)         (ARCHI_BEXTRACTU((value),1,2))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_GETS(value)        (ARCHI_BEXTRACT((value),1,2))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB_SET(value,field)   (ARCHI_BINSERT((value),(field),1,2))
#define APB_SOC_SAFE_FLL_CTRL_FLL2_RSTB(val)               ((val) << 2)

#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_GET(value)         (ARCHI_BEXTRACTU((value),1,3))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_GETS(value)        (ARCHI_BEXTRACT((value),1,3))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB_SET(value,field)   (ARCHI_BINSERT((value),(field),1,3))
#define APB_SOC_SAFE_FLL_CTRL_FLL3_RSTB(val)               ((val) << 3)

#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_GET(value)          (ARCHI_BEXTRACTU((value),4,0))
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_GETS(value)         (ARCHI_BEXTRACT((value),4,0))
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM_SET(value,field)    (ARCHI_BINSERT((value),(field),4,0))
#define APB_SOC_SAFE_L1_PWR_CTRL_BTRIM(val)                ((val) << 0)

#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_GET(value)        (ARCHI_BEXTRACTU((value),2,4))
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_GETS(value)       (ARCHI_BEXTRACT((value),2,4))
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N_SET(value,field)  (ARCHI_BINSERT((value),(field),2,4))
#define APB_SOC_SAFE_L1_PWR_CTRL_STDBY_N(val)              ((val) << 4)

#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_GET(value)            (ARCHI_BEXTRACTU((value),2,6))
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_GETS(value)           (ARCHI_BEXTRACT((value),2,6))
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD_SET(value,field)      (ARCHI_BINSERT((value),(field),2,6))
#define APB_SOC_SAFE_L1_PWR_CTRL_PWD(val)                  ((val) << 6)

#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_GET(value)        (ARCHI_BEXTRACTU((value),16,0))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_GETS(value)       (ARCHI_BEXTRACT((value),16,0))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N_SET(value,field)  (ARCHI_BINSERT((value),(field),16,0))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDDE_N(val)              ((val) << 0)

#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_GET(value)        (ARCHI_BEXTRACTU((value),16,16))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_GETS(value)       (ARCHI_BEXTRACT((value),16,16))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N_SET(value,field)  (ARCHI_BINSERT((value),(field),16,16))
#define APB_SOC_SAFE_L2_PWR_CTRL_VDDME_N(val)              ((val) << 16)

#endif

#endif
