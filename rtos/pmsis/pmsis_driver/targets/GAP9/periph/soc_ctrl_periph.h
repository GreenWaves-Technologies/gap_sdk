/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
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

#ifndef __SOC_CTRL_PERIPH_H__
#define __SOC_CTRL_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- SOC_CTRL Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** SOC_CTRL_Type Register Layout Typedef */
typedef struct soc_ctrl
{
  volatile uint32_t info; /**< Core information register */
  volatile uint32_t fc_boot; /**< Boot address */
  volatile uint32_t fc_fetch; /**< FC Fetch enable */
  volatile uint32_t cl_isolate; /**< Isolate cluster register */
  volatile uint32_t padfun[4]; /**< Mux config register (pad 0-15) */
  volatile uint32_t padcfg[16]; /**< Function register (pad 0 to 3) */
  volatile uint32_t unallocate0[3];
  volatile uint32_t cl_busy; /**< Cluster busy register */
  volatile uint32_t unallocate1[1];
  volatile uint32_t jtagreg; /**< JTAG external register */
  volatile uint32_t unallocate2; /**< Function register (pad 0 to 3) */
  volatile uint32_t sleep_ctrl; /**< Alias for SAFE_PMU_SLEEPCTRL */
  volatile uint32_t clk_div_i3c; /**< Clock divider for I3C */
  volatile uint32_t unallocate3[7];
  volatile uint32_t corestatus; /**< EOC and chip status register */
  volatile uint32_t unallocate4[7];
  volatile uint32_t corestatus_ro; /**< EOC and chip status register read mirror */
  volatile uint32_t bootsel; /**< Value of pad bootsel */
  volatile uint32_t wd_clr; /**< Clear WD timer */
  volatile uint32_t unallocate5;
  volatile uint32_t clk_sel; /**< Clock selection for SOC,Cluster and Periph */
  volatile uint32_t clk_div_soc; /**< SOC Clock Divider settings */
  volatile uint32_t clk_div_clu; /**< Cluster Clock Divider settings */
  volatile uint32_t clk_div_per; /**< Peripheral Clock Divider Settings */
  volatile uint32_t supervisor_dbg; /**< nan */
  volatile uint32_t rwm_grp[7]; /**< nan */
  volatile uint32_t unallocate6;
  volatile uint32_t safe_pmu_sleepctrl; /**< Sleep modes configuration register */
  volatile uint32_t safe_wd; /**< Enables and configures WatchDog Timer */
  volatile uint32_t safe_sleeppadcfg[4]; /**< Sleep config register (pad 0-15) (pad 16-31) (pad 32-47) (pad 48-63)*/
  volatile uint32_t safe_padsleep; /**< Enable Sleep mode for pads */
  volatile uint32_t safe_nevacf; /**< Config timings for NEVA */
  volatile uint32_t safe_gpreg; /**< General purpouse register AO */
  volatile uint32_t reg_gpio_iso; /**< GPIO power domain pad input isolation register */
  volatile uint32_t reg_cam_iso; /**< CAM power domain pad input isolation register */
  volatile uint32_t reg_lvds_iso; /**< LVDS power domain pad input isolation register */
} __attribute__((packed)) soc_ctrl_t;


/* ----------------------------------------------------------------------------
   -- SOC_CTRL Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name soc_ctrl_info_t */

typedef union {
  struct {
    unsigned int nb_cl           :16; // Number of clusters
    unsigned int nb_cores        :16; // Number of cores
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_info_t;

/*! @name INFO */
/* Number of clusters */
#define SOC_CTRL_INFO_NB_CL_MASK                                     0xffff
#define SOC_CTRL_INFO_NB_CL_SHIFT                                    0
#define SOC_CTRL_INFO_NB_CL(val)                                     ((val) << 0)

/* Number of cores */
#define SOC_CTRL_INFO_NB_CORES_MASK                                  0xffff0000
#define SOC_CTRL_INFO_NB_CORES_SHIFT                                 16
#define SOC_CTRL_INFO_NB_CORES(val)                                  ((val) << 16)


/*! @name soc_ctrl_fc_boot_t */

typedef union {
  struct {
    unsigned int addr            :32; // FC Boot Address
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_fc_boot_t;

/*! @name FC_BOOT */
/* FC Boot Address */
#define SOC_CTRL_FC_BOOT_ADDR_MASK                                   0xffffffff
#define SOC_CTRL_FC_BOOT_ADDR_SHIFT                                  0
#define SOC_CTRL_FC_BOOT_ADDR(val)                                   ((val) << 0)


/*! @name soc_ctrl_fc_fetch_t */

typedef union {
  struct {
    unsigned int fc_fe           :1 ; // FC Fetch Enable
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_fc_fetch_t;

/*! @name FC_FETCH */
/* FC Fetch Enable */
#define SOC_CTRL_FC_FETCH_FC_FE_MASK                                 0x1
#define SOC_CTRL_FC_FETCH_FC_FE_SHIFT                                0
#define SOC_CTRL_FC_FETCH_FC_FE(val)                                 ((val) << 0)


/*! @name soc_ctrl_cl_isolate_t */

typedef union {
  struct {
    unsigned int en              :1 ; // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_cl_isolate_t;

/*! @name CL_ISOLATE */
/* Isolate cluster. Inhibits AXI transactions from cluster to SoC:
  - 1'b0:  Disable
  - 1'b1: Enable */
#define SOC_CTRL_CL_ISOLATE_EN_MASK                                  0x1
#define SOC_CTRL_CL_ISOLATE_EN_SHIFT                                 0
#define SOC_CTRL_CL_ISOLATE_EN(val)                                  ((val) << 0)


/*! @name soc_ctrl_padfun0_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padfun0_t;

/*! @name PADFUN0 */

/*! @name soc_ctrl_padfun1_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padfun1_t;

/*! @name PADFUN1 */

/*! @name soc_ctrl_padfun2_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padfun2_t;

/*! @name PADFUN2 */

/*! @name soc_ctrl_padfun3_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padfun3_t;

/*! @name PADFUN3 */

/*! @name soc_ctrl_padcfg0_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg0_t;

/*! @name PADCFG0 */

/*! @name soc_ctrl_padcfg1_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg1_t;

/*! @name PADCFG1 */

/*! @name soc_ctrl_padcfg2_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg2_t;

/*! @name PADCFG2 */

/*! @name soc_ctrl_padcfg3_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg3_t;

/*! @name PADCFG3 */

/*! @name soc_ctrl_padcfg4_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg4_t;

/*! @name PADCFG4 */

/*! @name soc_ctrl_padcfg5_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg5_t;

/*! @name PADCFG5 */

/*! @name soc_ctrl_padcfg6_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg6_t;

/*! @name PADCFG6 */

/*! @name soc_ctrl_padcfg7_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg7_t;

/*! @name PADCFG7 */

/*! @name soc_ctrl_padcfg8_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg8_t;

/*! @name PADCFG8 */

/*! @name soc_ctrl_padcfg9_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg9_t;

/*! @name PADCFG9 */

/*! @name soc_ctrl_padcfg10_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg10_t;

/*! @name PADCFG10 */

/*! @name soc_ctrl_padcfg11_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg11_t;

/*! @name PADCFG11 */

/*! @name soc_ctrl_padcfg12_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg12_t;

/*! @name PADCFG12 */

/*! @name soc_ctrl_padcfg13_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg13_t;

/*! @name PADCFG13 */

/*! @name soc_ctrl_padcfg14_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg14_t;

/*! @name PADCFG14 */

/*! @name soc_ctrl_padcfg15_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_padcfg15_t;

/*! @name PADCFG15 */

/*! @name soc_ctrl_cl_busy_t */

typedef union {
  struct {
    unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_cl_busy_t;

/*! @name CL_BUSY */
/* Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) */
#define SOC_CTRL_CL_BUSY_BUSY_MASK                                   0x1
#define SOC_CTRL_CL_BUSY_BUSY_SHIFT                                  0
#define SOC_CTRL_CL_BUSY_BUSY(val)                                   ((val) << 0)


/*! @name soc_ctrl_jtagreg_t */

typedef union {
  struct {
    unsigned int int_sync        :1 ; // JTAG internal register used for synchronisation from external debugger
    unsigned int int_bt_md       :3 ; // JTAG internal register used for selecting boot mode configuration from external debugger
    unsigned int padding0:4 ;
    unsigned int ext_sync        :1 ; // JTAG external register used for synchronisation from external debugger
    unsigned int ext_bt_md       :3 ; // JTAG external register used for selecting boot mode configuration from external debugger
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_jtagreg_t;

/*! @name JTAGREG */
/* JTAG internal register used for synchronisation from external debugger */
#define SOC_CTRL_JTAGREG_INT_SYNC_MASK                               0x1
#define SOC_CTRL_JTAGREG_INT_SYNC_SHIFT                              0
#define SOC_CTRL_JTAGREG_INT_SYNC(val)                               ((val) << 0)

/* JTAG internal register used for selecting boot mode configuration from external debugger */
#define SOC_CTRL_JTAGREG_INT_BT_MD_MASK                              0xe
#define SOC_CTRL_JTAGREG_INT_BT_MD_SHIFT                             1
#define SOC_CTRL_JTAGREG_INT_BT_MD(val)                              ((val) << 1)

/* JTAG external register used for synchronisation from external debugger */
#define SOC_CTRL_JTAGREG_EXT_SYNC_MASK                               0x100
#define SOC_CTRL_JTAGREG_EXT_SYNC_SHIFT                              8
#define SOC_CTRL_JTAGREG_EXT_SYNC(val)                               ((val) << 8)

/* JTAG external register used for selecting boot mode configuration from external debugger */
#define SOC_CTRL_JTAGREG_EXT_BT_MD_MASK                              0xe00
#define SOC_CTRL_JTAGREG_EXT_BT_MD_SHIFT                             9
#define SOC_CTRL_JTAGREG_EXT_BT_MD(val)                              ((val) << 9)


/*! @name soc_ctrl_sleep_ctrl_t */

typedef union {
  struct {
    unsigned int sleep_ctrl      :32; // Alias for SAFE_PMU_SLEEPCTRL(i.e. will be accessible in 1 clock cycle)
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_sleep_ctrl_t;

/*! @name SLEEP_CTRL */
/* Alias for SAFE_PMU_SLEEPCTRL(i.e. will be accessible in 1 clock cycle) */
#define SOC_CTRL_SLEEP_CTRL_SLEEP_CTRL_MASK                          0xffffffff
#define SOC_CTRL_SLEEP_CTRL_SLEEP_CTRL_SHIFT                         0
#define SOC_CTRL_SLEEP_CTRL_SLEEP_CTRL(val)                          ((val) << 0)


/*! @name soc_ctrl_clk_div_i3c_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_clk_div_i3c_t;

/*! @name CLK_DIV_I3C */

/*! @name soc_ctrl_corestatus_t */

typedef union {
  struct {
    unsigned int status          :32; // EOC and chip status register
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_corestatus_t;

/*! @name CORESTATUS */
/* EOC and chip status register */
#define SOC_CTRL_CORESTATUS_STATUS_MASK                              0xffffffff
#define SOC_CTRL_CORESTATUS_STATUS_SHIFT                             0
#define SOC_CTRL_CORESTATUS_STATUS(val)                              ((val) << 0)


/*! @name soc_ctrl_corestatus_ro_t */

typedef union {
  struct {
    unsigned int status          :32; // EOC and chip status register
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_corestatus_ro_t;

/*! @name CORESTATUS_RO */
/* EOC and chip status register */
#define SOC_CTRL_CORESTATUS_RO_STATUS_MASK                           0xffffffff
#define SOC_CTRL_CORESTATUS_RO_STATUS_SHIFT                          0
#define SOC_CTRL_CORESTATUS_RO_STATUS(val)                           ((val) << 0)


/*! @name soc_ctrl_bootsel_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_bootsel_t;

/*! @name BOOTSEL */

/*! @name soc_ctrl_wd_clr_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_wd_clr_t;

/*! @name WD_CLR */

/*! @name soc_ctrl_clk_sel_t */

typedef union {
  struct {
    unsigned int clk_soc         :1 ; // SoC domain clock selection: - 1’b0: First FLL is used (FLL1) - 1’b1: Second FLL is used (FLL2)
    unsigned int clk_cluster     :2 ; // Cluster domain clock selection: - 2’b00: First FLL is used (FLL1) - 2’b01: Second FLL is used (FLL2) - 2’b10: Third FLL is used (FLL3)
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_clk_sel_t;

/*! @name CLK_SEL */
/* SoC domain clock selection:
  - 1’b0: First FLL is used (FLL1)
  - 1’b1: Second FLL is used (FLL2) */
#define SOC_CTRL_CLK_SEL_CLK_SOC_MASK                                0x1
#define SOC_CTRL_CLK_SEL_CLK_SOC_SHIFT                               0
#define SOC_CTRL_CLK_SEL_CLK_SOC(val)                                ((val) << 0)

/* Cluster domain clock selection:
  - 2’b00: First FLL is used (FLL1)
  - 2’b01: Second FLL is used (FLL2)
  - 2’b10: Third FLL is used (FLL3) */
#define SOC_CTRL_CLK_SEL_CLK_CLUSTER_MASK                            0x6
#define SOC_CTRL_CLK_SEL_CLK_CLUSTER_SHIFT                           1
#define SOC_CTRL_CLK_SEL_CLK_CLUSTER(val)                            ((val) << 1)


/*! @name soc_ctrl_clk_div_soc_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_clk_div_soc_t;

/*! @name CLK_DIV_SOC */

/*! @name soc_ctrl_clk_div_clu_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_clk_div_clu_t;

/*! @name CLK_DIV_CLU */

/*! @name soc_ctrl_clk_div_per_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_clk_div_per_t;

/*! @name CLK_DIV_PER */

/*! @name soc_ctrl_supervisor_dbg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_supervisor_dbg_t;

/*! @name SUPERVISOR_DBG */

/*! @name soc_ctrl_rwm_grp0_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp0_t;

/*! @name RWM_GRP0 */

/*! @name soc_ctrl_rwm_grp1_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp1_t;

/*! @name RWM_GRP1 */

/*! @name soc_ctrl_rwm_grp2_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp2_t;

/*! @name RWM_GRP2 */

/*! @name soc_ctrl_rwm_grp3_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp3_t;

/*! @name RWM_GRP3 */

/*! @name soc_ctrl_rwm_grp4_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp4_t;

/*! @name RWM_GRP4 */

/*! @name soc_ctrl_rwm_grp5_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp5_t;

/*! @name RWM_GRP5 */

/*! @name soc_ctrl_rwm_grp6_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_rwm_grp6_t;

/*! @name RWM_GRP6 */

/*! @name soc_ctrl_safe_pmu_sleepctrl_t */

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
} __attribute__((packed)) soc_ctrl_safe_pmu_sleepctrl_t;

/*! @name SAFE_PMU_SLEEPCTRL */
/* User value which is kept retentive after wakeup (even in non-retentive sleep). This value is only partially interpreted by the ROM code (TBD) to distinguish betweem cold boot, non-retentive sleep and retentive sleep. */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_REBOOT_MASK                      0x7
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_REBOOT_SHIFT                     0
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_REBOOT(val)                      ((val) << 0)

/* Enable smart wake-up;
  - 1'b0; smart wake-up disabled
  - 1'b1: smart wake-up enabled */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_MASK                0x200
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN_SHIFT               9
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_SMARTWAKE_EN(val)                ((val) << 9)

/* Enable RTC wake-up;
  - 1'b0; rtc wake-up disabled
  - 1'b1: rtc wake-up enabled */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_MASK                  0x400
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN_SHIFT                 10
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RTCWAKE_EN(val)                  ((val) << 10)

/* Select external wake-up  mode (through dedicated pin):
  - 2'b00: rise event
  - 2'b01: fall event
  - 2'b10: high level
  - 2'b11: low level */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_MASK                0x1800
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE_SHIFT               11
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_TYPE(val)                ((val) << 11)

/* Enable external wake-up (through dedicated pin);
  - 1'b0; external wake-up disabled
  - 1'b1: external wake-up enabled */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_MASK                  0x2000
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN_SHIFT                 13
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_EXTWAKE_EN(val)                  ((val) << 13)

/* Power state of the MRAM to restore after warm boot
  - 1'b0: MRAM OFF
  - 1'b1: MRAM ON
   */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_MASK              0x4000
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE_SHIFT             14
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_MRAM_WAKESTATE(val)              ((val) << 14)

/* Power state of the cluster to restore after warm boot
  - 1'b0: cluster OFF
  - 1'b1: cluster ON
   */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_MASK           0x8000
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE_SHIFT          15
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_CLUSTER_WAKESTATE(val)           ((val) << 15)

/* Configure retention mode of L2 memory. There is one bit per cut:
  - 1'b0: Non retentive
  - 1'b1: Retentive */
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RET_MEM_MASK                     0xffff0000
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RET_MEM_SHIFT                    16
#define SOC_CTRL_SAFE_PMU_SLEEPCTRL_RET_MEM(val)                     ((val) << 16)


/*! @name soc_ctrl_safe_wd_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_wd_t;

/*! @name SAFE_WD */

/*! @name soc_ctrl_safe_sleeppadcfg0_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_sleeppadcfg0_t;

/*! @name SAFE_SLEEPPADCFG0 */

/*! @name soc_ctrl_safe_sleeppadcfg1_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_sleeppadcfg1_t;

/*! @name SAFE_SLEEPPADCFG1 */

/*! @name soc_ctrl_safe_sleeppadcfg2_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_sleeppadcfg2_t;

/*! @name SAFE_SLEEPPADCFG2 */

/*! @name soc_ctrl_safe_sleeppadcfg3_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_sleeppadcfg3_t;

/*! @name SAFE_SLEEPPADCFG3 */

/*! @name soc_ctrl_safe_padsleep_t */

typedef union {
  struct {
    unsigned int en              :1 ; // Enable pad sleep mode: 1'b0: disable 1'b1: enable
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_padsleep_t;

/*! @name SAFE_PADSLEEP */
/* Enable pad sleep mode:
  1'b0: disable
  1'b1: enable */
#define SOC_CTRL_SAFE_PADSLEEP_EN_MASK                               0x1
#define SOC_CTRL_SAFE_PADSLEEP_EN_SHIFT                              0
#define SOC_CTRL_SAFE_PADSLEEP_EN(val)                               ((val) << 0)


/*! @name soc_ctrl_safe_nevacf_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_nevacf_t;

/*! @name SAFE_NEVACF */

/*! @name soc_ctrl_safe_gpreg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_safe_gpreg_t;

/*! @name SAFE_GPREG */

/*! @name soc_ctrl_reg_gpio_iso_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_reg_gpio_iso_t;

/*! @name REG_GPIO_ISO */

/*! @name soc_ctrl_reg_cam_iso_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_reg_cam_iso_t;

/*! @name REG_CAM_ISO */

/*! @name soc_ctrl_reg_lvds_iso_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) soc_ctrl_reg_lvds_iso_t;

/*! @name REG_LVDS_ISO */

/* ----------------------------------------------------------------------------

 CMD IDs and macros

----------------------------------------------------------------------------*/

#endif /* __SOC_CTRL_PERIPH_H__ */
