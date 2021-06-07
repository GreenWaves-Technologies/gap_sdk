/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PMSIS_TARGETS_VEGA_PERIPH_H__
#define __PMSIS_TARGETS_VEGA_PERIPH_H__

/* ----------------------------------------------------------------------------
   -- VEGA SoC Configuration
   ---------------------------------------------------------------------------- */

#define __MPU_PRESENT                  1 /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               0 /**< Number of priority bits implemented in the NVIC */
#define __FPU_PRESENT                  0 /**< Defines if an FPU is present or not */

/* Include memory map and events for Vega. */
#include "memory_map.h"
#include "events.h"

#include "core_gap.h"                     /* Core Peripheral Access Layer */
#include "core_gap_memory_define.h"       /* Core Memory regions definitions */

#if defined(FEATURE_CLUSTER)
#include "core_gap_cluster.h"              /* Cluster Access Layer */
#endif                                     /* FEATURE_CLUSTER */


/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
   -- FLL_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/fll_ctrl/fll_ctrl.h"
#define fll(id) ((fll_ctrl_t *) FLL_ADDR + (id * 0x10U))

/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/gpio_periph.h"
#define gpio(id) ((gpio_t *) GPIO_ADDR)

/* ----------------------------------------------------------------------------
   -- UDMA Global Configuration Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_core_periph.h"
#include "periph/udma_core_cmd_periph.h"

#include "periph/udma_ctrl_periph.h"
#define udma_ctrl ((udma_ctrl_t *) UDMA_CTRL_ADDR)

/* ----------------------------------------------------------------------------
   -- SPIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/spi_periph.h"

/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_hyper_periph.h"
#define hyper(id) ((hyper_t *) UDMA_HYPER(id))


/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_uart_periph.h"
#define uart(id) ((uart_t *) UDMA_UART(id))


/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_i2c_periph.h"
#define i2c(id) ((i2c_t *) UDMA_I2C(id))


/* ----------------------------------------------------------------------------
   -- DMACPY Peripheral Access Layer
   ---------------------------------------------------------------------------- */
//#include "periph/dmacpy_periph.h"
//#define dmacpy(id) ((dmacpy_t *) UDMA_DMACPY(id))


/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/i2s_periph.h"
#define i2s(id) ((i2s_t *) UDMA_I2S(id))


/* ----------------------------------------------------------------------------
   -- CPI Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/cpi_periph.h"
#define cpi(id) ((cpi_t *) UDMA_CPI(id))


/* ----------------------------------------------------------------------------
   -- Regulator Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/regulator_periph.h"


/* ----------------------------------------------------------------------------
   -- SOC_CTRL Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/soc_ctrl_periph.h"
#define soc_ctrl ((soc_ctrl_t *) APB_SOC_CTRL_ADDR)

/* TODO: Remove this instance.  */
/* SOC_CTRL - Peripheral instance base addresses */
/** Peripheral SOC_CTRL base address */
#define SOC_CTRL_BASE                                (SOC_PERI_BASE + 0x4000u)


/* ----------------------------------------------------------------------------
   -- PMU CTRL Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMU_CTRL_Peripheral_Access_Layer PMU_CTRL Peripheral Access Layer
 * @{
 */

/** PMU - General Register Layout Typedef */
typedef struct {
  __IO uint32_t RAR_DCDC;                     /**< PMU CTRL control register, offset: 0x000 */
  __IO uint32_t SLEEP_CTRL;                   /**< PMU CTRL sleep control register, offset: 0x004 */
  __IO uint32_t FORCE;                        /**< PMU CTRL register, offset: 0x008 */

} PMU_CTRL_Type;


/* ----------------------------------------------------------------------------
   -- PMU_CTRL Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMU_CTRL_Register_Masks PMU_CTRL Register Masks
 * @{
 */
/*! @name RAR_DCDC - PMU control register */
#define PMU_CTRL_RAR_DCDC_NV_MASK         (0x1FU)
#define PMU_CTRL_RAR_DCDC_NV_SHIFT        (0U)
#define PMU_CTRL_RAR_DCDC_NV(x)           (((uint32_t)(((uint32_t)(x)) /* << PMU_CTRL_RAR_DCDC_NV_SHIFT*/)) & PMU_CTRL_RAR_DCDC_NV_MASK)
#define READ_PMU_CTRL_RAR_DCDC_NV(x)      (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_RAR_DCDC_NV_MASK)) /*>> PMU_CTRL_RAR_DCDC_NV_SHIFT*/)
#define PMU_CTRL_RAR_DCDC_MV_MASK         (0x1F00U)
#define PMU_CTRL_RAR_DCDC_MV_SHIFT        (8U)
#define PMU_CTRL_RAR_DCDC_MV(x)           (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_RAR_DCDC_MV_SHIFT)) & PMU_CTRL_RAR_DCDC_MV_MASK)
#define READ_PMU_CTRL_RAR_DCDC_MV(x)      (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_RAR_DCDC_MV_MASK)) >> PMU_CTRL_RAR_DCDC_MV_SHIFT)
#define PMU_CTRL_RAR_DCDC_LV_MASK         (0x1F0000U)
#define PMU_CTRL_RAR_DCDC_LV_SHIFT        (16U)
#define PMU_CTRL_RAR_DCDC_LV(x)           (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_RAR_DCDC_LV_SHIFT)) & PMU_CTRL_RAR_DCDC_LV_MASK)
#define READ_PMU_CTRL_RAR_DCDC_LV(x)      (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_RAR_DCDC_LV_MASK)) >> PMU_CTRL_RAR_DCDC_LV_SHIFT)
#define PMU_CTRL_RAR_DCDC_RV_MASK         (0x1F000000U)
#define PMU_CTRL_RAR_DCDC_RV_SHIFT        (24U)
#define PMU_CTRL_RAR_DCDC_RV(x)           (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_RAR_DCDC_RV_SHIFT)) & PMU_CTRL_RAR_DCDC_RV_MASK)
#define READ_PMU_CTRL_RAR_DCDC_RV(x)      (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_RAR_DCDC_RV_MASK)) >> PMU_CTRL_RAR_DCDC_RV_SHIFT)

/*! @name SLEEP_CTRL - PMU control register */
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_MASK         (0xFU)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_SHIFT        (0U)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET(x)           (((uint32_t)(((uint32_t)(x)) /* << PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_SHIFT*/)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET(x)      (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_MASK)) /*>> PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET_SHIFT*/)

#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_MASK     (0x10U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_SHIFT    (4U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET(x)       (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_SHIFT)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET(x)  (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_MASK)) >> PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_MASK  (0x20U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_SHIFT (5U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(x)    (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_SHIFT)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(x)   (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_MASK)) >> PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_MASK      (0x7C0U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_SHIFT     (6U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL(x)        (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_SHIFT)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL(x)   (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_MASK)) >> PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_MASK     (0x1800U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_SHIFT    (11U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE(x)       (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_SHIFT)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE(x)  (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_MASK)) >> PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_MASK       (0x2000U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_SHIFT      (13U)
#define PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN(x)         (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_SHIFT)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN(x)    (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_MASK)) >> PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_WAKEUP_MASK            (0xC000U)
#define PMU_CTRL_SLEEP_CTRL_WAKEUP_SHIFT           (14U)
#define PMU_CTRL_SLEEP_CTRL_WAKEUP(x)              (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_WAKEUP_SHIFT)) & PMU_CTRL_SLEEP_CTRL_WAKEUP_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_WAKEUP(x)         (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_WAKEUP_MASK)) >> PMU_CTRL_SLEEP_CTRL_WAKEUP_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_BOOT_L2_MASK           (0x10000U)
#define PMU_CTRL_SLEEP_CTRL_BOOT_L2_SHIFT          (16U)
#define PMU_CTRL_SLEEP_CTRL_BOOT_L2(x)             (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_BOOT_L2_SHIFT)) & PMU_CTRL_SLEEP_CTRL_BOOT_L2_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_BOOT_L2(x)        (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_BOOT_L2_MASK)) >> PMU_CTRL_SLEEP_CTRL_BOOT_L2_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_REBOOT_MASK            (0xC0000U)
#define PMU_CTRL_SLEEP_CTRL_REBOOT_SHIFT           (18U)
#define PMU_CTRL_SLEEP_CTRL_REBOOT(x)              (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_REBOOT_SHIFT)) & PMU_CTRL_SLEEP_CTRL_REBOOT_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_REBOOT(x)         (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_REBOOT_MASK)) >> PMU_CTRL_SLEEP_CTRL_REBOOT_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_MASK      (0x100000U)
#define PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_SHIFT     (20U)
#define PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP(x)        (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_SHIFT)) & PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP(x)   (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_MASK)) >> PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_SHIFT)

/*! @name FORCE - PMU control register */
#define PMU_CTRL_FORCE_MEM_RET_MASK                (0xFU)
#define PMU_CTRL_FORCE_MEM_RET_SHIFT               (0U)
#define PMU_CTRL_FORCE_MEM_RET(x)                  (((uint32_t)(((uint32_t)(x)) /* << PMU_CTRL_FORCE_MEM_RET_SHIFT*/)) & PMU_CTRL_FORCE_MEM_RET_MASK)

#define PMU_CTRL_FORCE_MEM_PWD_MASK                (0xF0U)
#define PMU_CTRL_FORCE_MEM_PWD_SHIFT               (4U)
#define PMU_CTRL_FORCE_MEM_PWD(x)                  (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_FORCE_MEM_PWD_SHIFT)) & PMU_CTRL_FORCE_MEM_PWD_MASK)

#define PMU_CTRL_FORCE_FLL_CLUSTER_RET_MASK        (0x100U)
#define PMU_CTRL_FORCE_FLL_CLUSTER_RET_SHIFT       (8U)
#define PMU_CTRL_FORCE_FLL_CLUSTER_RET(x)          (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_FORCE_FLL_CLUSTER_RET_SHIFT)) & PMU_CTRL_FORCE_FLL_CLUSTER_RET_MASK)

#define PMU_CTRL_FORCE_FLL_CLUSTER_PWD_MASK        (0x200U)
#define PMU_CTRL_FORCE_FLL_CLUSTER_PWD_SHIFT       (9U)
#define PMU_CTRL_FORCE_FLL_CLUSTER_PWD(x)          (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_FORCE_FLL_CLUSTER_PWD_SHIFT)) & PMU_CTRL_FORCE_FLL_CLUSTER_PWD_MASK)

/*!
 * @}
 */ /* end of group PMU_CTRL_Register_Masks */


/* PMU CTRL- Peripheral instance base addresses */
/** Peripheral PMU CTRL base address */
#define PMU_CTRL_BASE                               (SOC_CTRL_BASE + 0x0100u)
/** Peripheral PMU_CTRL0 base pointer */
#define PMU_CTRL                                    ((PMU_CTRL_Type *)PMU_CTRL_BASE)
/** Array initializer of PMU_CTRL base addresses */
#define PMU_CTRL_BASE_ADDRS                         { PMU_CTRL_BASE }
/** Array initializer of PMU_CTRL base pointers */
#define PMU_CTRL_BASE_PTRS                          { PMU_CTRL }

#include "periph/fc_itc_periph.h"
#define fc_itc ((fc_itc_t *) FC_ITC_ADDR)



/*!
 * @}
 */ /* end of group PMU_CTRL_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- PORT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Peripheral_Access_Layer PORT Peripheral Access Layer
 * @{
 */

/** PORT - Register Layout Typedef */
typedef struct {
  __IO  uint32_t PADFUN[4];                       /**< PORT pad function register 0, offset: 0x000 */
  __IO  uint32_t SLEEP_PADCFG[4];                 /**< PORT sleep pad configuration register 0, offset: 0x010 */
  __IO  uint32_t PAD_SLEEP;                       /**< PORT pad sleep register, offset: 0x020 */
  __IO  uint32_t _reserved0[7];                   /**< reserved, offset: 0x010 */
  __IO  uint32_t PADCFG[16];                      /**< PORT pad configuration register 0, offset: 0x040 */

} PORT_Type;

/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Masks GPIO Register Masks
 * @{
 */
#define GPIO_NUM                                 32

/*! @name PADFUN - GPIO pad mux registers */
#define PORT_PADFUN_MUX_MASK                     (0x3U)
#define PORT_PADFUN_MUX_SHIFT                    (0U)
#define PORT_PADFUN_MUX(x)                       (((uint32_t)(((uint32_t)(x)) << PORT_PADFUN_MUX_SHIFT)) & PORT_PADFUN_MUX_MASK)

/*! @name PADCFG - GPIO pad configuration registers */
#define PORT_PADCFG_PULL_EN_MASK                 (0x1U)
#define PORT_PADCFG_PULL_EN_SHIFT                (0U)
#define PORT_PADCFG_PULL_EN(x)                   (((uint32_t)(((uint32_t)(x)) << PORT_PADCFG_PULL_EN_SHIFT)) & PORT_PADCFG_PULL_EN_MASK)
#define PORT_PADCFG_DRIVE_STRENGTH_MASK          (0x2U)
#define PORT_PADCFG_DRIVE_STRENGTH_SHIFT         (1U)
#define PORT_PADCFG_DRIVE_STRENGTH(x)            (((uint32_t)(((uint32_t)(x)) << PORT_PADCFG_DRIVE_STRENGTH_SHIFT)) & PORT_PADCFG_DRIVE_STRENGTH_MASK)

/*!
 * @}
 */ /* end of group PORT_Register_Masks */


/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base address */
#define PORTA_BASE                              (SOC_CTRL_BASE + 0x0140u)
/** Peripheral PORTA base pointer */
#define PORTA                                   ((PORT_Type *)PORTA_BASE)
/** Array initializer of PORT base addresses */
#define PORT_BASE_ADDRS                         { PORTA_BASE }
/** Array initializer of PORT base pointers */
#define PORT_BASE_PTRS                          { PORTA }

/*!
 * @}
 */ /* end of group PORT_Access_Layer */



/* ----------------------------------------------------------------------------
   -- IO POWER DOMAINS ISOLATION Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IO_ISO_Peripheral_Access_Layer IO_ISO Peripheral Access Layer
 * @{
 */

/** IO_ISO - Register Layout Typedef */
typedef struct {
  __IO  uint32_t GPIO_ISO;                       /**< IO_ISO GPIO power domains isolation, offset: 0x000 */
  __IO  uint32_t CAM_ISO;                        /**< IO_ISO Cemera power domains isolation, offset: 0x004 */
  __IO  uint32_t LVDS_ISO;                       /**< IO_ISO LVDS power domains isolation, offset: 0x008 */

} IO_ISO_Type;

/* ----------------------------------------------------------------------------
   -- IO_ISO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup IO_ISO_Register_Masks GPIO Register Masks
 * @{
 */
#define IO_ISO_GPIO_ISO_MASK                 (0x1U)
#define IO_ISO_GPIO_ISO_SHIFT                (0U)
#define IO_ISO_GPIO_ISO(x)                   (((uint32_t)(((uint32_t)(x)) /* << IO_ISO_GPIO_ISO_SHIFT */)) & IO_ISO_GPIO_ISO_MASK)

#define IO_ISO_CAM_ISO_MASK                 (0x1U)
#define IO_ISO_CAM_ISO_SHIFT                (0U)
#define IO_ISO_CAM_ISO(x)                   (((uint32_t)(((uint32_t)(x)) /* << IO_ISO_CAM_ISO_SHIFT */)) & IO_ISO_CAM_ISO_MASK)

#define IO_ISO_LVDS_ISO_MASK                 (0x1U)
#define IO_ISO_LVDS_ISO_SHIFT                (0U)
#define IO_ISO_LVDS_ISO(x)                   (((uint32_t)(((uint32_t)(x)) /* << IO_ISO_LVDS_ISO_SHIFT */)) & IO_ISO_LVDS_ISO_MASK)


/*!
 * @}
 */ /* end of group IO_ISO_Register_Masks */


/* IO_ISO - Peripheral instance base addresses */
/** Peripheral IO_ISO base address */
#define IO_ISO_BASE                               (SOC_CTRL_BASE + 0x01C0u)
/** Peripheral IO_ISO base pointer */
#define IO_ISO                                    ((IO_ISO_Type *)IO_ISO_BASE)
/** Array initializer of IO_ISO base addresses */
#define IO_ISO_BASE_ADDRS                         { IO_ISO_BASE }
/** Array initializer of IO_ISO base pointers */
#define IO_ISO_BASE_PTRS                          { IO_ISO }

/*!
 * @}
 */ /* end of group IO_ISO_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PWM Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/pwm_periph.h"
#include "periph/pwm_ctrl_periph.h"
#define pwm(id) ((pwm_t *) (ADV_TIMER_ADDR + (id << 6)))
#define pwm_ctrl ((pwm_ctrl_t *) (ADV_TIMER_ADDR + 0x100))



/* ----------------------------------------------------------------------------
   -- SOCEU (SOC EVENT UNIT) Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOCEU_Peripheral_Access_Layer SOCEU Peripheral Access Layer
 * @{
 */

/** SOCEU - Register Layout Typedef */
typedef struct {
  __IO  uint32_t EVENT;                         /**< SOCEU event register, offset: 0x00 */
  __IO  uint32_t FC_MASK[8];                    /**< SOCEU fc mask registers, offset: 0x04 */
  __IO  uint32_t CL_MASK[8];                    /**< SOCEU cluster mask registers, offset: 0x24 */
  __IO  uint32_t PR_MASK[8];                    /**< SOCEU cluster mask registers, offset: 0x44 */
  __IO  uint32_t ERR[8];                        /**< SOCEU error mask registers, offset: 0x64 */
  __IO  uint32_t TIMER_SEL_HI;                  /**< SOCEU timer high register, offset: 0x84 */
  __IO  uint32_t TIMER_SEL_LO;                  /**< SOCEU timer low register, offset: 0x88 */
} SOCEU_Type;

/* ----------------------------------------------------------------------------
   -- SOCEU Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOCEU_Register_Masks SOCEU Register Masks
 * @{
 */
/* The SOC events number */
#define SOC_EVENTS_NUM              0x08
/*!
 * @}
 */ /* end of group SOCEU_Register_Masks */


/* SOCEU - Peripheral instance base addresses */
/** Peripheral SOCEU base address */
#define SOCEU_BASE                               (SOC_PERI_BASE + 0x06000u)
/** Peripheral SOCEU base pointer */
#define SOCEU                                    ((volatile SOCEU_Type *)SOCEU_BASE)
/** Array initializer of SOCEU base addresses */
#define SOCEU_BASE_ADDRS                         { SOCEU_BASE }
/** Array initializer of SOCEU base pointers */
#define SOCEU_BASE_PTRS                          { SOCEU }

/*!
 * @}
 */ /* end of group SOCEU_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- SW EVENT TRIGGER Register Address
   ---------------------------------------------------------------------------- */

#define EU_EVT_GETCLUSTERBASE(coreId)     (0x00200800u + (coreId << 6))


/* ----------------------------------------------------------------------------
   -- PMU DLC Access Layer
   ---------------------------------------------------------------------------- */

#include "periph/pmu_dlc_periph.h"
#define pmu_dlc ((pmu_dlc_t *) PMU_DLC_BASE)

/*!
 * @addtogroup PMU_DLC_Peripheral_Access_Layer PMU_DLC Peripheral Access_Layer
 * @{
 */

/** PMU - General Register Layout Typedef */
typedef struct {
  __IO uint32_t PCTRL;                          /**< PMU DLC control register, offset: 0x00 */
  __IO uint32_t PRDATA;                         /**< PMU DLC data register, offset: 0x04 */
  __IO uint32_t DLC_SR;                         /**< PMU DLC register, offset: 0x08 */
  __IO uint32_t DLC_IMR;                        /**< PMU DLC register, offset: 0x0C */
  __IO uint32_t DLC_IFR;                        /**< PMU DLC register, offset: 0x10 */
  __IO uint32_t DLC_IOIFR;                      /**< PMU DLC register, offset: 0x14 */
  __IO uint32_t DLC_IDIFR;                      /**< PMU DLC register, offset: 0x18 */
  __IO uint32_t DLC_IMCIFR;                     /**< PMU DLC register, offset: 0x1C */

} PMU_DLC_Type;

/* ----------------------------------------------------------------------------
   -- SOCEU Register Masks
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup PMU_DLC_Register_Masks PMU_DLC Register Masks
 * @{
 */

/*! @name PCTRL - PMU DLC PICL control register */
#define PMU_DLC_PCTRL_START_MASK              (0x1U)
#define PMU_DLC_PCTRL_START_SHIFT             (0U)
#define PMU_DLC_PCTRL_START(x)                (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PCTRL_START_SHIFT */)) & PMU_DLC_PCTRL_START_MASK)
#define PMU_DLC_PCTRL_PADDR_MASK              (0x7FFEU)
#define PMU_DLC_PCTRL_PADDR_SHIFT             (1U)
#define PMU_DLC_PCTRL_PADDR(x)                (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_PADDR_SHIFT)) & PMU_DLC_PCTRL_PADDR_MASK)
#define PMU_DLC_PCTRL_DIR_MASK                (0x8000U)
#define PMU_DLC_PCTRL_DIR_SHIFT               (15U)
#define PMU_DLC_PCTRL_DIR(x)                  (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_DIR_SHIFT)) & PMU_DLC_PCTRL_DIR_MASK)
#define PMU_DLC_PCTRL_PWDATA_MASK             (0xFFFF0000U)
#define PMU_DLC_PCTRL_PWDATA_SHIFT            (16U)
#define PMU_DLC_PCTRL_PWDATA(x)               (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_PWDATA_SHIFT)) & PMU_DLC_PCTRL_PWDATA_MASK)

/*! @name PRDATA - PMU DLC PICL data read register */
#define PMU_DLC_PRDATA_PRDATA_MASK            (0xFFU)
#define PMU_DLC_PRDATA_PRDATA_SHIFT           (0U)
#define PMU_DLC_PRDATA_PRDATA(x)              (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PRDATA_PRDATA_SHIFT */)) & PMU_DLC_PRDATA_PRDATA_MASK)

/*! @name SR - PMU DLC DLC Status register */
#define PMU_DLC_SR_PICL_BUSY_MASK             (0x1U)
#define PMU_DLC_SR_PICL_BUSY_SHIFT            (0U)
#define PMU_DLC_SR_PICL_BUSY(x)               (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_SR_PICL_BUSY_SHIFT */)) & PMU_DLC_SR_PICL_BUSY_MASK)
#define PMU_DLC_SR_SCU_BUSY_MASK              (0x2U)
#define PMU_DLC_SR_SCU_BUSY_SHIFT             (1U)
#define PMU_DLC_SR_SCU_BUSY(x)                (((uint32_t)(((uint32_t)(x)) << PMU_DLC_SR_SCU_BUSY_SHIFT)) & PMU_DLC_SR_SCU_BUSY_MASK)

/*! @name IMR - PMU DLC Interrupt mask register */
#define PMU_DLC_IMR_ICU_OK_MASK_MASK          (0x1U)
#define PMU_DLC_IMR_ICU_OK_MASK_SHIFT         (0U)
#define PMU_DLC_IMR_ICU_OK_MASK(x)            (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_IMR_ICU_OK_MASK_SHIFT */)) & PMU_DLC_IMR_ICU_OK_MASK_MASK)
#define PMU_DLC_IMR_ICU_DELAYED_MASK_MASK     (0x2U)
#define PMU_DLC_IMR_ICU_DELAYED_MASK_SHIFT    (1U)
#define PMU_DLC_IMR_ICU_DELAYED_MASK(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_ICU_DELAYED_MASK_SHIFT)) & PMU_DLC_IMR_ICU_DELAYED_MASK_MASK)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_MASK     (0x4U)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_SHIFT    (2U)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_SHIFT)) & PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_MASK)
#define PMU_DLC_IMR_PICL_OK_MASK_MASK         (0x8U)
#define PMU_DLC_IMR_PICL_OK_MASK_SHIFT        (3U)
#define PMU_DLC_IMR_PICL_OK_MASK(x)           (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_PICL_OK_MASK_SHIFT)) & PMU_DLC_IMR_PICL_OK_MASK_MASK)
#define PMU_DLC_IMR_SCU_OK_MASK_MASK          (0x10U)
#define PMU_DLC_IMR_SCU_OK_MASK_SHIFT         (4U)
#define PMU_DLC_IMR_SCU_OK_MASK(x)            (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_SCU_OK_MASK_SHIFT)) & PMU_DLC_IMR_SCU_OK_MASK_MASK)

/*! @name IFR - PMU DLC Interrupt flag register */
#define PMU_DLC_IFR_ICU_OK_FLAG_MASK          (0x1U)
#define PMU_DLC_IFR_ICU_OK_FLAG_SHIFT         (0U)
#define PMU_DLC_IFR_ICU_OK_FLAG(x)            (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_IFR_ICU_OK_FLAG_SHIFT */)) & PMU_DLC_IFR_ICU_OK_FLAG_MASK)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_MASK     (0x2U)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_SHIFT    (1U)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_ICU_DELAYED_FLAG_SHIFT)) & PMU_DLC_IFR_ICU_DELAYED_FLAG_MASK)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_MASK     (0x4U)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_SHIFT    (2U)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_SHIFT)) & PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_MASK)
#define PMU_DLC_IFR_PICL_OK_FLAG_MASK         (0x8U)
#define PMU_DLC_IFR_PICL_OK_FLAG_SHIFT        (3U)
#define PMU_DLC_IFR_PICL_OK_FLAG(x)           (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_PICL_OK_FLAG_SHIFT)) & PMU_DLC_IFR_PICL_OK_FLAG_MASK)
#define PMU_DLC_IFR_SCU_OK_FLAG_MASK          (0x10U)
#define PMU_DLC_IFR_SCU_OK_FLAG_SHIFT         (4U)
#define PMU_DLC_IFR_SCU_OK_FLAG(x)            (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_SCU_OK_FLAG_SHIFT)) & PMU_DLC_IFR_SCU_OK_FLAG_MASK)

/*! @name IOIFR - PMU DLC icu_ok interrupt flag register */
#define PMU_DLC_IOIFR_ICU_OK_FLAG_MASK          (0xFFFFFFFEU)
#define PMU_DLC_IOIFR_ICU_OK_FLAG_SHIFT         (1U)
#define PMU_DLC_IOIFR_ICU_OK_FLAG(x)            (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IOIFR_ICU_OK_FLAG_SHIFT)) & PMU_DLC_IOIFR_ICU_OK_FLAG_MASK)

/*! @name IDIFR - PMU DLC icu_delayed interrupt flag register */
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG_MASK     (0xFFFFFFFEU)
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG_SHIFT    (1U)
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IDIFR_ICU_DELAYED_FLAG_SHIFT)) & PMU_DLC_IDIFR_ICU_DELAYED_FLAG_MASK)

/*! @name IMCIFR - PMU DLC icu_mode changed interrupt flag register */
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_MASK     (0xFFFFFFFEU)
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_SHIFT    (1U)
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_SHIFT)) & PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_MASK)

/*! @name PCTRL_PADDR The address to write in the DLC_PADDR register is CHIP_SEL_ADDR[4:0] concatenated with REG_ADDR[4:0]. */
#define PMU_DLC_PICL_REG_ADDR_MASK          (0x1FU)
#define PMU_DLC_PICL_REG_ADDR_SHIFT         (0U)
#define PMU_DLC_PICL_REG_ADDR(x)            (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PICL_REG_ADDR_SHIFT */)) & PMU_DLC_PICL_REG_ADDR_MASK)
#define PMU_DLC_PICL_CHIP_SEL_ADDR_MASK     (0x3E0U)
#define PMU_DLC_PICL_CHIP_SEL_ADDR_SHIFT    (5U)
#define PMU_DLC_PICL_CHIP_SEL_ADDR(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PICL_CHIP_SEL_ADDR_SHIFT)) & PMU_DLC_PICL_CHIP_SEL_ADDR_MASK)

/* CHIP_SEL_ADDR[4:0]*/
#define  PICL_WIU_ADDR         0x00
#define  PICL_ICU_ADDR         0x01

#if 0
/* REG_ADDR[4:0]*/
#define  WIU_ISPMR_0           (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x00))
#define  WIU_ISPMR_1           (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x01))
#define  WIU_IFR_0             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x02))
#define  WIU_IFR_1             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x03))
#define  WIU_ICR_0             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x04))
#define  WIU_ICR_1             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x05))
#define  WIU_ICR_2             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x06))
#define  WIU_ICR_3             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x07))
#define  WIU_ICR_4             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x08))
#define  WIU_ICR_5             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x09))
#define  WIU_ICR_6             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0A))
#define  WIU_ICR_7             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0B))
#define  WIU_ICR_8             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0C))
#define  WIU_ICR_9             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0D))
#define  WIU_ICR_10            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0E))
#define  WIU_ICR_11            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x0F))
#define  WIU_ICR_12            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x10))
#define  WIU_ICR_13            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x11))
#define  WIU_ICR_14            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x12))
#define  WIU_ICR_15            (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_WIU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x13))

/* REG_ADDR[4:0]*/
#define  ICU_CR                (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x00))
#define  ICU_MR                (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x01))
#define  ICU_ISMR              (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x02))
#define  ICU_DMR_0             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x03))
#define  ICU_DMA_1             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU_ADDR) | PMU_DLC_PICL_REG_ADDR(0x04))
#endif
/*!
 * @}
 */ /* end of group PMU_DLC_Register_Masks */

/* PMU DLC- Peripheral instance base addresses */
/** Peripheral PMU DLC base address */
#define PMU_DLC_BASE                                (SOC_PERI_BASE + 0x7000u)
/** Peripheral PMU_DLC base pointer */
#define PMU_DLC                                     ((PMU_DLC_Type *)PMU_DLC_BASE)
/** Array initializer of PMU_DLC base addresses */
#define PMU_DLC_BASE_ADDRS                          { PMU_DLC_BASE }
/** Array initializer of PMU_DLC base pointers */
#define PMU_DLC_BASE_PTRS                           { PMU_DLC }

/*!
 * @}
 */ /* end of group PMU_DLC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RTC_APB Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/rtc_periph.h"
#define rtc(id) ((rtc_t *) RTC_ADDR)


/* ----------------------------------------------------------------------------
   -- EFUSE CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_CTRL_Peripheral_Access_Layer EFUSE_CTRL Peripheral Access Layer
 * @{
 */



#include "periph/efuse_periph.h"

/* EFUSE_CTRL - Peripheral instance base addresses */
/** Peripheral EFUSE_CTRL base address */
#define EFUSE_CTRL_BASE                               (SOC_PERI_BASE + 0x09000u)
/** Peripheral EFUSE_CTRL base pointer */
#define efuse_ctrl                                    ((efuse_ctrl_t *)EFUSE_CTRL_BASE)
/** Array initializer of EFUSE_CTRL base addresses */
#define EFUSE_CTRL_BASE_ADDRS                         { EFUSE_CTRL_BASE }
/** Array initializer of EFUSE_CTRL base pointers */
#define EFUSE_CTRL_BASE_PTRS                          { EFUSE_CTRL }

/*!
 * @}
 */ /* end of group EFUSE_CTRL_Peripheral_Access_Layer */


/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x09200u)
/** Peripheral EFUSE_REGS base pointer */
#define efuse_regs                                     ((efuse_regs_t *)EFUSE_REGS_BASE)
#define efuse_regs_array                               ((int32_t*)EFUSE_REGS_BASE)
/** Array initializer of EFUSE_REGS base addresses */
#define EFUSE_REGS_BASE_ADDRS                          { EFUSE_REGS_BASE }
/** Array initializer of EFUSE_REGS base pointers */
#define EFUSE_REGS_BASE_PTRS                           { EFUSE_REGS }/*!
 * @}
 */ /* end of group EFUSE_CTRL_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- EFUSE REG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Peripheral_Access_Layer EFUSE_REGS Peripheral Access Layer
 * @{
 */

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t INFO;                    /**< EFUSE INFO register, offset: 0x000 */
  __IO  uint32_t INFO2;                   /**< EFUSE_INFO2 register, offset: 0x004 */
  __IO  uint32_t AES_KEY[16];             /**< EFUSE_AES_KEY registers, offset: 0x008 */
  __IO  uint32_t AES_IV[8];               /**< EFUSE_AES_IV registers, offset: 0x048 */
  __IO  uint32_t WAIT_XTAL_DELTA_LSB;     /**< EFUSE_WAIT_XTAL_DELTA_LSB register, offset: 0x068 */
  __IO  uint32_t WAIT_XTAL_DELTA_MSB;     /**< EFUSE_WAIT_XTAL_DELTA_MSB register, offset: 0x06C */
  __IO  uint32_t WAIT_XTAL_MIN;           /**< EFUSE_WAIT_XTAL_MIN registers, offset: 0x070 */
  __IO  uint32_t WAIT_XTAL_MAX;           /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x074 */
  __IO  uint32_t HYPER_RDS_DELAY;         /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x078 */
  __IO  uint32_t FLL_FREQ;                /**< EFUSE_FLL_FREQ registers, offset: 0x07C */
  __IO  uint32_t FLL_TOLERANCE;           /**< EFUSE_FLL_TOLERANCE registers, offset: 0x080 */
  __IO  uint32_t FLL_ASSERT_CYCLE;        /**< EFUSE_FLL_ASSERT_CYCLE registers, offset: 0x084 */
  __IO  uint32_t _reserved[6];            /**< EFUSE_reserved registers, offset: 0x088 */
  __IO  uint32_t USER_REG[88];            /**< EFUSE_USER_REG, offset: 0x0A0 */
} EFUSE_REGS_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Register_Masks EFUSE_REGS Register Masks
 * @{
 */
/*! @name INFO - EFUSE information register */
#define EFUSE_INFO_PLT_MASK                           (0x07U)
#define EFUSE_INFO_PLT_SHIFT                          (0U)
#define EFUSE_INFO_PLT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_INFO_PLT_SHIFT */)) & EFUSE_INFO_PLT_MASK)

#define EFUSE_INFO_BOOT_MASK                          (0x38U)
#define EFUSE_INFO_BOOT_SHIFT                         (3U)
#define EFUSE_INFO_BOOT(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_BOOT_SHIFT)) & EFUSE_INFO_BOOT_MASK)

#define EFUSE_INFO_ENCRYPTED_MASK                     (0x40U)
#define EFUSE_INFO_ENCRYPTED_SHIFT                    (6U)
#define EFUSE_INFO_ENCRYPTED(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_ENCRYPTED_SHIFT)) & EFUSE_INFO_ENCRYPTED_MASK)

#define EFUSE_INFO_WAIT_XTAL_MASK                     (0x80U)
#define EFUSE_INFO_WAIT_XTAL_SHIFT                    (7U)
#define EFUSE_INFO_WAIT_XTAL(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_WAIT_XTAL_SHIFT)) & EFUSE_INFO_WAIT_XTAL_MASK)


/*!
 * @}
 */ /* end of group EFUSE_REGS_Register_Masks */


/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x09200u)
/** Peripheral EFUSE_REGS base pointer */
#define EFUSE_REGS                                     ((EFUSE_REGS_Type *)EFUSE_REGS_BASE)
/** Array initializer of EFUSE_REGS base addresses */
#define EFUSE_REGS_BASE_ADDRS                          { EFUSE_REGS_BASE }
/** Array initializer of EFUSE_REGS base pointers */
#define EFUSE_REGS_BASE_PTRS                           { EFUSE_REGS }

/*!
 * @}
 */ /* end of group EFUSE_REGS_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- FC_STDOUT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_STDOUT_Peripheral_Access_Layer FC_STDOUT Peripheral Access Layer
 * @{
 */

/** FC_STDOUT - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t PUTC[16];                    /**< FC_STDOUT INFO register, offset: 0x000 */
} FC_STDOUT_Type;

/* ----------------------------------------------------------------------------
   -- FC_STDOUT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_STDOUT_Register_Masks FC_STDOUT Register Masks
 * @{
 */
/*! @name INFO - FC_STDOUT information register */

/*!
 * @}
 */ /* end of group FC_STDOUT_Register_Masks */


/* FC_STDOUT - Peripheral instance base addresses */
/** Peripheral FC_STDOUT base address */
#define FC_STDOUT_BASE                                (SOC_PERI_BASE + 0x20000u + (FC_CLUSTER_ID << 7))
/** Peripheral FC_STDOUT base pointer */
#define FC_STDOUT                                     ((FC_STDOUT_Type *)FC_STDOUT_BASE)
/** Array initializer of FC_STDOUT base addresses */
#define FC_STDOUT_BASE_ADDRS                          { FC_STDOUT_BASE }
/** Array initializer of FC_STDOUT base pointers */
#define FC_STDOUT_BASE_PTRS                           { FC_STDOUT }

/*!
 * @}
 */ /* end of group FC_STDOUT_Peripheral_Access_Layer */


#ifdef FEATURE_CLUSTER

/* ----------------------------------------------------------------------------
   -- CLUSTER_CTRL_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_ctrl_unit/cluster_ctrl_unit.h"
#define cl_ctrl_unit(id)         ((cluster_ctrl_unit_t *) CL_CTRL_ADDR)
#define cl_glob_ctrl_unit(cid)   ((cluster_ctrl_unit_t *) CL_CTRL_GLOB_ADDR(cid))

/* ----------------------------------------------------------------------------
   -- CLUSTER_ICACHE_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_icache_ctrl/cluster_icache_ctrl.h"
#define cl_icache_ctrl(id)       ((cluster_icache_ctrl_t *) CL_ICACHE_ADDR)
#define cl_glob_icache_ctrl(cid) ((cluster_icache_ctrl_t *) CL_GLOB_ICACHE_ADDR(cid))

/* ----------------------------------------------------------------------------
   -- CLUSTER_DEMUX_EVENT_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_event_unit/cluster_event_unit.h"
#define cl_demux_eu_core(id)     ((cluster_eu_core_demux_t *) CL_DEMUX_EU_CORE_ADDR)
#define cl_demux_eu_loop(id)     ((cluster_eu_loop_demux_t *) CL_DEMUX_EU_LOOP_ADDR)
#define cl_demux_eu_dispatch(id) ((cluster_eu_dispatch_demux_t *) CL_DEMUX_EU_DISPATCH_ADDR)
#define cl_demux_eu_mutex(id)    ((cluster_eu_mutex_demux_t *) CL_DEMUX_EU_HW_MUTEX_ADDR)
#define cl_demux_eu_sw_evt(id)   ((cluster_eu_sw_evt_demux_t *) CL_DEMUX_EU_SW_EVENT_ADDR)
#define cl_demux_eu_barrier(id)  ((cluster_eu_barrier_demux_t *) (CL_DEMUX_EU_CORE_ADDR + (id * sizeof(cluster_eu_barrier_demux_t))))

/* ----------------------------------------------------------------------------
   -- CLUSTER_EVENT_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_event_unit/cluster_event_unit.h"
#define cl_glob_eu_core(cid)     ((cluster_eu_core_demux_t *) CL_GLOB_EU_CORE_ADDR(cid))
#define cl_glob_eu_dispatch(cid) ((cluster_eu_dispatch_demux_t *) CL_GLOB_EU_DISPATCH_ADDR(cid))
#define cl_glob_eu_sw_evt(cid)   ((cluster_eu_sw_evt_demux_t *) CL_GLOB_EU_SW_EVENT_ADDR(cid))


/* ----------------------------------------------------------------------------
   -- CLUSTER_STDOUT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CLUSTER_STDOUT_Peripheral_Access_Layer CLUSTER_STDOUT Peripheral Access Layer
 * @{
 */

/** CLUSTER_STDOUT - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t PUTC[16];                    /**< CLUSTER_STDOUT INFO register, offset: 0x000 */
} CLUSTER_STDOUT_Type;

/* ----------------------------------------------------------------------------
   -- CLUSTER_STDOUT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CLUSTER_STDOUT_Register_Masks CLUSTER_STDOUT Register Masks
 * @{
 */
/*! @name INFO - CLUSTER_STDOUT information register */

/*!
 * @}
 */ /* end of group CLUSTER_STDOUT_Register_Masks */


/* CLUSTER_STDOUT - Peripheral instance base addresses */
/** Peripheral CLUSTER_STDOUT base address */
#define CLUSTER_STDOUT_BASE                                (SOC_PERI_BASE + 0x10000u)
/** Peripheral CLUSTER_STDOUT base pointer */
#define CLUSTER_STDOUT                                     ((CLUSTER_STDOUT_Type *)CLUSTER_STDOUT_BASE)
/** Array initializer of CLUSTER_STDOUT base addresses */
#define CLUSTER_STDOUT_BASE_ADDRS                          { CLUSTER_STDOUT_BASE }
/** Array initializer of CLUSTER_STDOUT base pointers */
#define CLUSTER_STDOUT_BASE_PTRS                           { CLUSTER_STDOUT }

/*!
 * @}
 */ /* end of group CLUSTER_STDOUT_Peripheral_Access_Layer */




/* ----------------------------------------------------------------------------
   -- HWCE Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup HWCE Peripheral_Access_Layer HWCE Peripheral Access Layer
 * @{
 */

/** HWCE - Registers Layout Typedef */
typedef struct {
    __IO  uint32_t HWCE_TRIGGER_REG;              /**< HWCE Trigger register, offset: 0x00 */
    __IO  uint32_t HWCE_ACQUIRE_REG;              /**< HWCE Acquire register, offset: 0x04 */
    __IO  uint32_t HWCE_FINISHED_REG;             /**< HWCE Finished register, offset: 0x08 */
    __IO  uint32_t HWCE_STATUS_REG;               /**< HWCE Status register, offset: 0x0C */
    __IO  uint32_t HWCE_RUNNING_JOB_REG;          /**< HWCE Running Job register, offset: 0x10 */
    __IO  uint32_t HWCE_SOFT_CLEAR_REG;           /**< HWCE Soft_Clear register, offset: 0x14 */
    __IO  uint32_t _reserved0[2];                 /**< HWCE Non used registers, offser: 0x18 */
    __IO  uint32_t HWCE_GEN_CONFIG0_REG;          /**< HWCE Gen_Config0 register, offset: 0x20 */
    __IO  uint32_t HWCE_GEN_CONFIG1_REG;          /**< HWCE Gen_Config1 register, offset: 0x24 */
    __IO  uint32_t _reserved1[6];                 /**< HWCE unused registers, offset: 0x28 */
    __IO  uint32_t HWCE_Y_TRANS_SIZE_REG;         /**< HWCE Y_Trans_Size register, offset: 0x40 */
    __IO  uint32_t HWCE_Y_LINE_STRIDE_LENGTH_REG; /**< HWCE Y_Line_Stride_Length register, offset: 0x44 */
    __IO  uint32_t HWCE_Y_FEAT_STRIDE_LENGTH_REG; /**< HWCE Y_Feat_Stride_Length register, offset: 0x48 */
    __IO  uint32_t HWCE_Y_OUT_3_REG;              /**< HWCE Y_Out_3 register, offset: 0x4C */
    __IO  uint32_t HWCE_Y_OUT_2_REG;              /**< HWCE Y_Out_2 register, offset: 0x50 */
    __IO  uint32_t HWCE_Y_OUT_1_REG;              /**< HWCE Y_Out_1 register, offset: 0x54 */
    __IO  uint32_t HWCE_Y_OUT_0_REG;              /**< HWCE Y_Out_0 register, offset: 0x58 */
    __IO  uint32_t HWCE_Y_IN_3_REG;               /**< HWCE Y_In_3 register, offset: 0x5C */
    __IO  uint32_t HWCE_Y_IN_2_REG;               /**< HWCE Y_In_2 register, offset: 0x60 */
    __IO  uint32_t HWCE_Y_IN_1_REG;               /**< HWCE Y_In_1 register, offset: 0x64 */
    __IO  uint32_t HWCE_Y_IN_0_REG;               /**< HWCE Y_In_0 register, offset: 0x68 */
    __IO  uint32_t HWCE_X_TRANS_SIZE_REG;         /**< HWCE X_Trans_Size register, offset: 0x6C */
    __IO  uint32_t HWCE_X_LINE_STRIDE_LENGTH_REG; /**< HWCE X_Line_Stride_Length register, offset: 0x70 */
    __IO  uint32_t HWCE_X_FEAT_STRIDE_LENGTH_REG; /**< HWCE X_Feat_Stride_Length register, offset: 0x74 */
    __IO  uint32_t HWCE_X_IN_REG;                 /**< HWCE X_In register, offset: 0x78 */
    __IO  uint32_t HWCE_W_REG;                    /**< HWCE W register, offset: 0x7C */
    __IO  uint32_t HWCE_JOB_CONFIG0_REG;          /**< HWCE Job_Config0 register, offset: 0x80 */
    __IO  uint32_t HWCE_JOB_CONFIG1_REG;          /**< HWCE Job_Config1 register, offset: 0x84 */
} HWCE_Type;


/* ----------------------------------------------------------------------------
   -- HWCE Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup HWCE_Register_Masks HWCE Register Masks
 * @{
 */
/*! @name INFO - HWCE information register */
/* Internal registers */
#define HWCE_TRIGGER              ( 0x00 )
#define HWCE_ACQUIRE              ( 0x04 )
#define HWCE_FINISHED             ( 0x08 )
#define HWCE_STATUS               ( 0x0C )
#define HWCE_RUNNING_JOB          ( 0x10 )
#define HWCE_SOFT_CLEAR           ( 0x14 )
#define HWCE_GEN_CONFIG0          ( 0x20 )
#define HWCE_GEN_CONFIG1          ( 0x24 )

/* Configuration registers */
#define HWCE_Y_TRANS_SIZE         ( 0x40 )
#define HWCE_Y_LINE_STRIDE_LENGTH ( 0x44 )
#define HWCE_Y_FEAT_STRIDE_LENGTH ( 0x48 )
#define HWCE_Y_OUT_3_BASE_ADDR    ( 0x4C )
#define HWCE_Y_OUT_2_BASE_ADDR    ( 0x50 )
#define HWCE_Y_OUT_1_BASE_ADDR    ( 0x54 )
#define HWCE_Y_OUT_0_BASE_ADDR    ( 0x58 )
#define HWCE_Y_IN_3_BASE_ADDR     ( 0x5C )
#define HWCE_Y_IN_2_BASE_ADDR     ( 0x60 )
#define HWCE_Y_IN_1_BASE_ADDR     ( 0x64 )
#define HWCE_Y_IN_0_BASE_ADDR     ( 0x68 )
#define HWCE_X_TRANS_SIZE         ( 0x6C )
#define HWCE_X_LINE_STRIDE_LENGTH ( 0x70 )
#define HWCE_X_FEAT_STRIDE_LENGTH ( 0x74 )
#define HWCE_X_IN_BASE_ADDR       ( 0x78 )
#define HWCE_W_BASE_ADDR          ( 0x7C )
#define HWCE_JOB_CONFIG0          ( 0x80 )
#define HWCE_JOB_CONFIG1          ( 0x84 )

#define HWCE_NB_IO_REGS           ( 18 )

#define HWCE_ACQUIRE_CONTEXT_COPY ( -3 )
#define HWCE_ACQUIRE_LOCKED       ( -2 )
#define HWCE_ACQUIRE_QUEUE_FULL   ( -1 )
#define HWCE_ACQUIRE_READY        ( 0 )

#define HWCE_GEN_CONFIG0_WSTRIDE(x)           ((x) >> 16)
#define HWCE_GEN_CONFIG0_NCP(x)               (((x) >> 13) & 0x1)
#define HWCE_GEN_CONFIG0_CONV(x)              (((x) >> 11) & 0x3)
#define HWCE_GEN_CONFIG0_VECT(x)              (((x) >> 9) & 0x3)
#define HWCE_GEN_CONFIG0_UNS(x)               (((x) >> 8) & 1)
#define HWCE_GEN_CONFIG0_NY(x)                (((x) >> 7) & 1)
#define HWCE_GEN_CONFIG0_NF(x)                (((x) >> 6) & 1)
#define HWCE_GEN_CONFIG0_QF(x)                ((x) & 0x3f)

#define HWCE_GEN_CONFIG0_CONV_5x5 ( 0 )
#define HWCE_GEN_CONFIG0_CONV_3x3 ( 1 )
#define HWCE_GEN_CONFIG0_CONV_4x7 ( 2 )

#define HWCE_GEN_CONFIG0_VECT_1   ( 0 )
#define HWCE_GEN_CONFIG0_VECT_2   ( 1 )
#define HWCE_GEN_CONFIG0_VECT_4   ( 2 )

#define HWCE_GEN_CONFIG1_PIXSHIFTR(x)         (((x) >> 16) & 0x1F)
#define HWCE_GEN_CONFIG1_PIXMODE(x)           (((x) >> 8) & 0x3)
#define HWCE_GEN_CONFIG1_PIXSHIFTL(x)         (((x) >> 0) & 0x1F)

#define HWCE_JOB_CONFIG0_NOYCONST(x)          ((x) >> 16)
#define HWCE_JOB_CONFIG0_LBUFLEN(x)           ((x) & 0x3ff)

#define HWCE_JOB_CONFIG1_LO(x)                (((x) >> 24) & 0x1)
#define HWCE_JOB_CONFIG1_WIF(x)               (((x) >> 16) & 0x3F)
#define HWCE_JOB_CONFIG1_WOF(x)               (((x) >> 8) & 0x1F)
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK(x) (((x) >> 0) & 0xF)

#define HWCE_JOB_STRIDE(x)                    ((x) >> 16)
#define HWCE_JOB_LENGTH(x)                    ((x) & 0xffff)

  /*!
   * @}
   */ /* end of group HWCE_Register_Masks */


/* HWCE - Peripheral instance base addresses */
/** Peripheral HWCE base address */
#define HWCE_BASE                                (CORE_PERI_BASE + 0x00001000)
/** Peripheral HWCE base pointer */
#define HWCE                                     ((HWCE_Type *) HWCE_BASE)
/** Array initializer of HWCE base addresses */
#define HWCE_BASE_ADDRS                          { HWCE_BASE }
/** Array initializer of HWCE base pointers */
#define HWCE_BASE_PTRS                           { HWCE }

/*!
 * @}
 */ /* end of group HWCE_Peripheral_Access_Layer */


#endif  /* FEATURE_CLUSTER */


#endif  /* __PMSIS_TARGETS_VEGA_PERIPH_H__ */
