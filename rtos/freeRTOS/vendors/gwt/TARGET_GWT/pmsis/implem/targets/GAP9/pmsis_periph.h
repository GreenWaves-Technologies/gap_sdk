/*
** ###################################################################
**     Processors:          VEGA
**
**     Compilers:           GNU C Compiler
**
**     Reference manual:    riscv-spec-v2.1, January 2017
**     Version:             rev. 2.9, 2017-07-19
**
**     Abstract:
**         PMSIS Peripheral Access Layer for VEGA
**
**     Copyright (c) 2015 - 2019 GreenWave Technologies, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of GreenWaves Technologies, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 http://greenwaves-technologies.com
**     mail:                 jie.chen@greenwaves-technologies.com
**
**     Revisions:
**     - rev. 1.0 (2017-07-19)
**         Initial version.
** ###################################################################
*/

/*!
 * @file pmsis_periph.h
 * @version 1.0
 * @date 2018-04-2
 * @brief CMSIS Peripheral Access Layer for VEGA
 *
 * PMSIS Peripheral Access Layer for VEGA
 */

#ifndef _VEGA_H_
#define _VEGA_H_                              /**< Symbol preventing repeated inclusion */
/* ----------------------------------------------------------------------------
   -- Interrupt vector numbers
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Interrupt_vector_numbers Interrupt vector numbers
 * @{
 */
/** Interrupt Number Definitions */
#define NUMBER_OF_INT_VECTORS  32                  /**< Number of interrupts in the Vector table */

typedef enum IRQn {
  FC_NOTIFY_CLUSTER_EVENT      = 0,                /**< Software event interrupt */
  CLUSTER_NOTIFY_FC_EVENT      = 1,                /**< Software event interrupt */
  FC_SW_NOTIFY_BRIDGE_EVENT    = 2,                /**< Software event interrupt */
  FC_SW_NOTIFY_EVENT           = 3,                /**< Software event interrupt */
  CLUSTER_NOTIFY_FC_IRQN       = 4,                /**< Software event interrupt */
  PENDSV_IRQN                  = 7,                /**< Software event U -> M PendSV interrupt */

  /* Device specific interrupts */
  DMA_EVT_IRQN                 = 8,                /**< DMA event interrupt */
  DMA_IRQN                     = 9,                /**< DMA interrupt */
  FC_TIMER0_IRQN               = 10,               /**< FC timer0 event interrupt */
  SYSTICK_IRQN                 = 10,               /**< VEGA U -> M System Tick Interrupt */
  FC_TIMER1_IRQN               = 11,               /**< FC timer1 interrupt */

  EU_HWCE_EVENT                = 15,              /**< VEGA HWCE SW Event */
  EU_HW_BARRIER_EVENT          = 16,              /**< VEGA Hardware Barrier SW Event */
  EU_MUTEX_EVENT               = 17,              /**< VEGA Mutex SW Event */
  EU_DISPATCH_EVENT            = 18,              /**< VEGA Dispatch SW Event */
  EU_LOOP_EVENT                = 19,              /**< VEGA Loop SW Event */

  CLUSTER_NOT_BUSY_IRQN        = 21,              /**< Cluster not busy Interrupt */
  CLUSTER_POWER_OK_IRQN        = 22,              /**< Cluster Power ok Interrupt */
  CLUSTER_CLOCK_GATE_OK_IRQN   = 23,              /**< Cluster clock gating ok Interrupt */
  PICL_OK_IRQN                 = 24,              /**< VEGA Interrupt */
  SCU_IRQN                     = 25,              /**< VEGA Interrupt */

  /* Fault interrupts */
  FC_SOC_EVENT_IRQN            = 26,              /**< VEGA SoC Event Interrupt */
  FC_SOC_QUIDDIKEY_IRQN        = 27,              /**< VEGA QuiddiKey Interrupt */
  MPU_ERROR_IRQN               = 28,              /**< VEGA MPU Error Interrupt */
  ERR_EVENT_IRQN               = 29,              /**< VEGA Event Error Interrupt */

  /* Core interrupts */
  RST_HANDLER_IRQN             = 32,              /**< VEGA Reset handler Interrupt */
  ILL_INS_IRQN                 = 33,              /**< VEGA Usage Fault Interrupt */
  SVCALL_IRQN                  = 34               /**< VEGA SV Call Interrupt */
} IRQn_Type;

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers */


/* ----------------------------------------------------------------------------
   -- VEGA Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup VEGA_Core_Configuration VEGA Core Configuration
 * @{
 */

#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               0         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */
#define __FPU_PRESENT                  0         /**< Defines if an FPU is present or not */

#include "core_gap.h"              /* Core Peripheral Access Layer */
#include "core_gap_memory_define.h"       /* Core Memory regions definitions */

/* Include memory map and events for Vega. */
//#include "hal/pmsis_hal.h"
#include "memory_map.h"
#include "events.h"

#ifdef FEATURE_CLUSTER
#include "core_gap_cluster.h"              /* Cluster Access Layer */
#endif

/*!
 * @}
 */ /* end of group VEGA_Core_Configuration */


/* ----------------------------------------------------------------------------
   -- Mapping Information
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Mapping_Information Mapping Information
 * @{
 */

/** Mapping Information */
/*!
 * @addtogroup udma_request
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*!
 * @}
 */ /* end of group udma_request */



/*!
 * @}
 */ /* end of group Mapping_Information */


/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{
 */

/* ----------------------------------------------------------------------------
   -- FLL_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLL_CTRL_Peripheral_Access_Layer FLL_CTRL Peripheral Access Layer
 * @{
 */

/** FLL_CTRL - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t FLL_STATUS;            /**< FLL_CTRL Status register, offset: 0x00 */
  __IO  uint32_t CONF1;                 /**< FLL_CTRL Configuration1 register, offset: 0x04 */
  __IO  uint32_t CONF2;                 /**< FLL_CTRL Configuration2 register, offset: 0x08 */
  __IO  uint32_t INTEGRATOR;            /**< FLL_CTRL INTEGRATOR register, offset: 0x0C */

} FLL_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- FLL_CTRL Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLL_CTRL_Register_Masks FLL_CTRL Register Masks
 * @{
 */
/*! @name FLL_STATUS - FLL_CTRL status register */
#define FLL_CTRL_STATUS_MULTI_FACTOR_MASK              (0xFFFFU)
#define FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT             (0U)
#define FLL_CTRL_STATUS_MULTI_FACTOR(x)                (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT */)) & FLL_CTRL_STATUS_MULTI_FACTOR_MASK)
#define READ_FLL_CTRL_STATUS_MULTI_FACTOR(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_STATUS_MULTI_FACTOR_MASK)) /*>> FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT*/)

/*! @name SOC_CONF1 - FLL_CTRL configuration 1 register */
#define FLL_CTRL_CONF1_MULTI_FACTOR_MASK           (0xFFFFU)
#define FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT          (0U)
#define FLL_CTRL_CONF1_MULTI_FACTOR(x)             (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT */)) & FLL_CTRL_CONF1_MULTI_FACTOR_MASK)
#define READ_FLL_CTRL_CONF1_MULTI_FACTOR(x)        (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_MULTI_FACTOR_MASK)) /*>> FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT*/)

#define FLL_CTRL_CONF1_DCO_INPUT_MASK              (0x3FF0000U)
#define FLL_CTRL_CONF1_DCO_INPUT_SHIFT             (16U)
#define FLL_CTRL_CONF1_DCO_INPUT(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_DCO_INPUT_SHIFT)) & FLL_CTRL_CONF1_DCO_INPUT_MASK)
#define READ_FLL_CTRL_CONF1_DCO_INPUT(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_DCO_INPUT_MASK)) >> FLL_CTRL_CONF1_DCO_INPUT_SHIFT)

#define FLL_CTRL_CONF1_CLK_OUT_DIV_MASK            (0x3C000000U)
#define FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT           (26U)
#define FLL_CTRL_CONF1_CLK_OUT_DIV(x)              (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT)) & FLL_CTRL_CONF1_CLK_OUT_DIV_MASK)
#define READ_FLL_CTRL_CONF1_CLK_OUT_DIV(x)         (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_CLK_OUT_DIV_MASK)) >> FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT)

#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK         (0x40000000U)
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT        (30U)
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN(x)           (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT)) & FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK)
#define READ_FLL_CTRL_CONF1_OUTPUT_LOCK_EN(x)      (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK)) >> FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT)

#define FLL_CTRL_CONF1_MODE_MASK                   (0x80000000U)
#define FLL_CTRL_CONF1_MODE_SHIFT                  (31U)
#define FLL_CTRL_CONF1_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_MODE_SHIFT)) & FLL_CTRL_CONF1_MODE_MASK)
#define READ_FLL_CTRL_CONF1_MODE(x)                (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_MODE_MASK)) >> FLL_CTRL_CONF1_MODE_SHIFT)

/*! @name SOC_CONF2 - FLL_CTRL configuration 2 register */
#define FLL_CTRL_CONF2_LOOPGAIN_MASK               (0xFU)
#define FLL_CTRL_CONF2_LOOPGAIN_SHIF  T            (0U)
#define FLL_CTRL_CONF2_LOOPGAIN(x)                 (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_CONF2_LOOPGAIN_SHIFT */)) & FLL_CTRL_CONF2_LOOPGAIN_MASK)
#define READ_FLL_CTRL_CONF2_LOOPGAIN(x)            (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_LOOPGAIN_MASK))/* >> FLL_CTRL_CONF2_LOOPGAIN_SHIFT*/)

#define FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK        (0x3F0U)
#define FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT       (4U)
#define FLL_CTRL_CONF2_DEASSERT_CYCLES(x)          (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT)) & FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK)
#define READ_FLL_CTRL_CONF2_DEASSERT_CYCLES(x)     (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK)) >> FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT)

#define FLL_CTRL_CONF2_ASSERT_CYCLES_MASK          (0xFC00U)
#define FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT         (10U)
#define FLL_CTRL_CONF2_ASSERT_CYCLES(x)            (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT)) & FLL_CTRL_CONF2_ASSERT_CYCLES_MASK)
#define READ_FLL_CTRL_CONF2_ASSERT_CYCLES(x)       (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_ASSERT_CYCLES_MASK)) >> FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT)

#define FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK         (0xFFF0000U)
#define FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT        (16U)
#define FLL_CTRL_CONF2_LOCK_TOLERANCE(x)           (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT)) & FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK)
#define READ_FLL_CTRL_CONF2_LOCK_TOLERANCE(x)      (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK)) >> FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT)

#define FLL_CTRL_CONF2_CONF_CLK_SEL_MASK           (0x20000000U)
#define FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT          (29U)
#define FLL_CTRL_CONF2_CONF_CLK_SEL(x)             (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT)) & FLL_CTRL_CONF2_CONF_CLK_SEL_MASK)
#define READ_FLL_CTRL_CONF2_CONF_CLK_SEL(x)        (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_CONF_CLK_SEL_MASK)) >> FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT)

#define FLL_CTRL_CONF2_OPEN_LOOP_MASK              (0x40000000U)
#define FLL_CTRL_CONF2_OPEN_LOOP_SHIFT             (30U)
#define FLL_CTRL_CONF2_OPEN_LOOP(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_OPEN_LOOP_SHIFT)) & FLL_CTRL_CONF2_OPEN_LOOP_MASK)
#define READ_FLL_CTRL_CONF2_OPEN_LOOP(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_OPEN_LOOP_MASK)) >> FLL_CTRL_CONF2_OPEN_LOOP_SHIFT)

#define FLL_CTRL_CONF2_DITHERING_MASK              (0x80000000U)
#define FLL_CTRL_CONF2_DITHERING_SHIFT             (31U)
#define FLL_CTRL_CONF2_DITHERING(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_DITHERING_SHIFT)) & FLL_CTRL_CONF2_DITHERING_MASK)
#define READ_FLL_CTRL_CONF2_DITHERING(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_DITHERING_MASK)) >> FLL_CTRL_CONF2_DITHERING_SHIFT)

/*! @name SOC_INTEGRATOR - FLL_CTRL configuration 2 register */
#define FLL_CTRL_INTEGRATOR_FRACT_PART_MASK        (0xFFC0U)
#define FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT       (6U)
#define FLL_CTRL_INTEGRATOR_FRACT_PART(x)          (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT)) & FLL_CTRL_INTEGRATOR_FRACT_PART_MASK)
#define READ_FLL_CTRL_INTEGRATOR_FRACT_PART(x)     (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_INTEGRATOR_FRACT_PART_MASK)) >> FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT)

#define FLL_CTRL_INTEGRATOR_INT_PART_MASK          (0x3FF0000U)
#define FLL_CTRL_INTEGRATOR_INT_PART_SHIFT         (16U)
#define FLL_CTRL_INTEGRATOR_INT_PART(x)            (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_INTEGRATOR_INT_PART_SHIFT)) & FLL_CTRL_INTEGRATOR_INT_PART_MASK)
#define READ_FLL_CTRL_INTEGRATOR_INT_PART(x)       (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_INTEGRATOR_INT_PART_MASK)) >> FLL_CTRL_INTEGRATOR_INT_PART_SHIFT)

/*! @name FLL_CONVERGE - FLL_CTRL configuration 2 register */
#define FLL_CTRL_SOC_FLL_CONV_MASK                 (0x1U)
#define FLL_CTRL_SOC_FLL_CONV_SHIFT                (0U)
#define FLL_CTRL_SOC_FLL_CONV(x)                   (((uint32_t)(((uint32_t)(x)) /*<< FLL_CTRL_SOC_FLL_CONV_SHIFT */)) & FLL_CTRL_SOC_FLL_CONV_MASK)
#define READ_FLL_CTRL_SOC_FLL_CONV(x)              (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_SOC_FLL_CONV_MASK)) /*>> FLL_CTRL_SOC_FLL_CONV_SHIFT*/)

#define FLL_CTRL_CLUSTER_FLL_CONV_MASK             (0x2U)
#define FLL_CTRL_CLUSTER_FLL_CONV_SHIFT            (1U)
#define FLL_CTRL_CLUSTER_FLL_CONV(x)               (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CLUSTER_FLL_CONV_SHIFT)) & FLL_CTRL_CLUSTER_FLL_CONV_MASK)
#define READ_FLL_CTRL_CLUSTER_FLL_CONV(x)          (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CLUSTER_FLL_CONV_MASK)) >> FLL_CTRL_CLUSTER_FLL_CONV_SHIFT)


/*!
 * @}
 */ /* end of group FLL_CTRL_Register_Masks */

/* The number of FLL */
#define FLL_NUM        3
/* The FLL reference frequency*/
#define FLL_REF_CLK    32768


/* FLL_CTRL - Peripheral instance base addresses */
/** Peripheral FLL_CTRL0 base address */
#define FLL_CTRL0_OFFSET                             (0U)
#define FLL_CTRL0_BASE                               (SOC_PERI_BASE + FLL_CTRL0_OFFSET)
/** Peripheral FLL_CTRL0 base pointer */
#define FLL_CTRL0                                    ((FLL_CTRL_Type *)FLL_CTRL0_BASE)
/** Peripheral FLL_CTRL1 base address */
#define FLL_CTRL1_OFFSET                             (0x10U)
#define FLL_CTRL1_BASE                               (SOC_PERI_BASE + FLL_CTRL1_OFFSET)
/** Peripheral FLL_CTRL1 base pointer */
#define FLL_CTRL1                                    ((FLL_CTRL_Type *)FLL_CTRL1_BASE)
/** Peripheral FLL_CTRL2 base address */
#define FLL_CTRL2_OFFSET                             (0x20U)
#define FLL_CTRL2_BASE                               (SOC_PERI_BASE + FLL_CTRL2_OFFSET)
/** Peripheral FLL_CTRL2 base pointer */
#define FLL_CTRL2                                    ((FLL_CTRL_Type *)FLL_CTRL2_BASE)
/** Array initializer of FLL_CTRL base addresses */
#define FLL_CTRL_BASE_ADDRS                          { FLL_CTRL0_BASE, FLL_CTRL1_BASE, FLL_CTRL2_BASE }
/** Array initializer of FLL_CTRL base pointers */
#define FLL_CTRL_BASE_PTRS                           { FLL_CTRL0, FLL_CTRL1, FLL_CTRL2 }

/*!
 * @}
 */ /* end of group FLL_CTRL_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/gpio_periph.h"
#define gpio(id) ((gpio_t *) (GPIO_ADDR + id * sizeof(gpio_t)))


/* ----------------------------------------------------------------------------
   -- UDMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDMA_Peripheral_Access_Layer  UDMA Peripheral Access Layer
 * @{
 */

#if 0
/****** DEPRECATED *******/
/** UDMA - General Register Layout Typedef */
typedef struct {
  __IO uint32_t RX_SADDR;                          /**< RX UDMA buffer transfer address register, offset: 0x0 */
  __IO uint32_t RX_SIZE;                           /**< RX UDMA buffer transfer size register, offset: 0x4 */
  __IO uint32_t RX_CFG;                            /**< RX UDMA transfer configuration register, offset: 0x8 */
  __IO uint32_t RX_INITCFG;                        /**< Reserved, offset: 0xC */
  __IO uint32_t TX_SADDR;                          /**< TX UDMA buffer transfer address register, offset: 0x10 */
  __IO uint32_t TX_SIZE;                           /**< TX UDMA buffer transfer size register, offset: 0x14 */
  __IO uint32_t TX_CFG;                            /**< TX UDMA transfer configuration register, offset: 0x18 */
  __IO uint32_t TX_INITCFG;                        /**< Reserved, offset: 0x1C */

} UDMA_Type;
/****** END DEPRECATED *******/
#endif
#include "udma_core_periph.h"
#include "udma_core_cmd_periph.h"

#define UDMA_Type udma_core_t

#define UDMA_PERIPH_OFFSET                   (0x80U)

/* ----------------------------------------------------------------------------
   -- UDMA Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDMA_Register_Masks UDMA Register Masks
 * @{
 */
/*! @name RX_SADDR - RX TX UDMA buffer transfer address register */
#define UDMA_SADDR_ADDR_MASK                 (0xFFFFU)
#define UDMA_SADDR_ADDR_SHIFT                (0U)
#define UDMA_SADDR_ADDR(x)                   (((uint32_t)(((uint32_t)(x)) /*<< UDMA_SADDR_ADDR_SHIFT*/)) & UDMA_SADDR_ADDR_MASK)

#define UDMA_SIZE_SIZE_MASK                  (0x1FFFFU)
#define UDMA_SIZE_SIZE_SHIFT                 (0U)
#define UDMA_SIZE_SIZE(x)                    (((uint32_t)(((uint32_t)(x)) << UDMA_SIZE_SIZE_SHIFT)) & UDMA_SIZE_SIZE_MASK)


/*! @name RX_CFG - RX TX UDMA transfer configuration register */
#define UDMA_CFG_CONTINOUS_MASK              (0x1U)
#define UDMA_CFG_CONTINOUS_SHIFT             (0U)
#define UDMA_CFG_CONTINOUS(x)                (((uint32_t)(((uint32_t)(x)) /*<< UDMA_CFG_CONTINOUS_SHIFT*/)) & UDMA_CFG_CONTINOUS_MASK)
#define UDMA_CFG_DATA_SIZE_MASK              (0x6U)
#define UDMA_CFG_DATA_SIZE_SHIFT             (1U)
#define UDMA_CFG_DATA_SIZE(x)                (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_DATA_SIZE_SHIFT)) & UDMA_CFG_DATA_SIZE_MASK)
#define UDMA_CFG_FILTER_MASK                 (0x8U)
#define UDMA_CFG_FILTER_SHIFT                (3U)
#define UDMA_CFG_FILTER(x)                   (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_FILTER_SHIFT)) & UDMA_CFG_FILTER_MASK)
#define UDMA_CFG_EN_MASK                     (0x10U)
#define UDMA_CFG_EN_SHIFT                    (4U)
#define UDMA_CFG_EN(x)                       (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_EN_SHIFT)) & UDMA_CFG_EN_MASK)
#define UDMA_CFG_PENDING_MASK                (0x20U)
#define UDMA_CFG_PENDING_SHIFT               (5U)
#define UDMA_CFG_PENDING(x)                  (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_PENDING_SHIFT)) & UDMA_CFG_PENDING_MASK)
#define UDMA_CFG_CLR_MASK                    (0x20U)
#define UDMA_CFG_CLR_SHIFT                   (5U)
#define UDMA_CFG_CLR(x)                      (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_CLR_SHIFT)) & UDMA_CFG_CLR_MASK)

/*!
 * @}
 */ /* end of group UDMA_Register_Masks */


/* UDMA - Peripheral instance base addresses */
/** Peripheral UDMA base address 0x1A102000 */
#define UDMA_BASE                                (SOC_PERI_BASE + UDMA_OFFSET + UDMA_PERIPH_OFFSET)
/** Peripheral UDMA events number */
#define UDMA_EVENTS_NUM                          16*8
/** Peripheral UDMA channel number */
#define UDMA_CHANNEL_NUM                         15

/*!
 * @}
 */ /* end of group UDMA_Peripheral_Access_Layer */


#ifndef __UDMA_CTRL_PERIPH__
/* ----------------------------------------------------------------------------
   -- UDMA Global Configuration Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDMA_GC_Peripheral_Access_Layer UDMA_GC Peripheral Access Layer
 * @{
 */

/** UDMA Global configuration - Register Layout Typedef */
typedef struct {
  __IO uint32_t CG;                          /**< UDMA_GC clock gating register, offset: 0x0 */
  __IO uint32_t EVTIN;                       /**< UDMA_GC input event register, offset: 0x04 */
  __IO uint32_t RST;                         /**< UDMA_GC reset register, offset: 0x08 */
} UDMA_GC_Type;

/* ----------------------------------------------------------------------------
   -- UDMA_GC Register Masks
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup UDMA_GC_Register_Masks UDMA_GC Register Masks
 * @{
 */

/*! @name UDMA_GC - UDMA event in register, User chooses which events can come to UDMA as reference events, support up to 4 choices */
#define UDMA_GC_EVTIN_CHOICE0_MASK                  (0xFFU)
#define UDMA_GC_EVTIN_CHOICE0_SHIFT                 (0U)
#define UDMA_GC_EVTIN_CHOICE0(x)                    (((uint32_t)(((uint32_t)(x)) << UDMA_GC_EVTIN_CHOICE0_SHIFT)) & UDMA_GC_EVTIN_CHOICE0_MASK)

#define UDMA_GC_EVTIN_CHOICE1_MASK                  (0xFF00U)
#define UDMA_GC_EVTIN_CHOICE1_SHIFT                 (8U)
#define UDMA_GC_EVTIN_CHOICE1(x)                    (((uint32_t)(((uint32_t)(x)) << UDMA_GC_EVTIN_CHOICE1_SHIFT)) & UDMA_GC_EVTIN_CHOICE1_MASK)

#define UDMA_GC_EVTIN_CHOICE2_MASK                  (0xFF0000U)
#define UDMA_GC_EVTIN_CHOICE2_SHIFT                 (16U)
#define UDMA_GC_EVTIN_CHOICE2(x)                    (((uint32_t)(((uint32_t)(x)) << UDMA_GC_EVTIN_CHOICE2_SHIFT)) & UDMA_GC_EVTIN_CHOICE2_MASK)

#define UDMA_GC_EVTIN_CHOICE3_MASK                  (0xFF000000)
#define UDMA_GC_EVTIN_CHOICE3_SHIFT                 (24U)
#define UDMA_GC_EVTIN_CHOICE3(x)                    (((uint32_t)(((uint32_t)(x)) << UDMA_GC_EVTIN_CHOICE3_SHIFT)) & UDMA_GC_EVTIN_CHOICE3_MASK)

#define UDMA_GC_EVTIN_MASK(evt_in)                  (evt_in&0xFF)
#define UDMA_GC_EVTIN_SHIFT_ID(id)                  (id*8)

/*!
 * @}
 */ /* end of group UDMA_GC_Register_Masks */


/* UDMA Global configuration - instance base addresses */
/** Global configuration UDMA base address */
#define UDMA_GC_OFFSET                            (0x02000U)
#define UDMA_GC_BASE                              (SOC_PERI_BASE + UDMA_GC_OFFSET)
#define UDMA_GC                                   ((UDMA_GC_Type *)UDMA_GC_BASE)

/*!
 * @}
 */ /* end of group UDMA_GC_Peripheral_Access_Layer */
#else
#include "udma_ctrl_periph.h"
#endif


/* ----------------------------------------------------------------------------
   -- SPIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPIM_Peripheral_Access_Layer SPIM Peripheral Access Layer
 * @{
 */

#include "spim_periph.h"

/* ----------------------------------------------------------------------------
   -- SPIM Register Masks
   ---------------------------------------------------------------------------- */

/*! @name uDMA - SPIM uDMA Number */
#define SPIM_NUMBER                   4

#define SPI_4_WORD_FORMAT   2
#define SPI_2_WORD_FORMAT   1
#define SPI_1_WORD_FORMAT   0

/* SPIM - Peripheral instance base addresses */
//#define UDMA_SPIM_ID(id) (SPIM0_ID+id)
/** Peripheral SPIM0 base address */
#define SPIM0_OFFSET                              (SPIM0_ID * UDMA_PERIPH_OFFSET)
#define SPIM0_BASE                                (UDMA_BASE + SPIM0_OFFSET)
/** Peripheral SPIM0 base pointer */
#define SPIM0                                     ((SPIM_Type *)SPIM0_BASE)
/** Peripheral SPIM1 base address */
#define SPIM1_OFFSET                              (SPIM1_ID * UDMA_PERIPH_OFFSET)
#define SPIM1_BASE                                (UDMA_BASE + SPIM1_OFFSET)
/** Peripheral SPIM1 base pointer */
#define SPIM1                                     ((SPIM_Type *)SPIM1_BASE)
/** Peripheral SPIM2 base address */
#define SPIM2_OFFSET                              (SPIM2_ID * UDMA_PERIPH_OFFSET)
#define SPIM2_BASE                                (UDMA_BASE + SPIM2_OFFSET)
/** Peripheral SPIM2 base pointer */
#define SPIM2                                     ((SPIM_Type *)SPIM2_BASE)
/** Peripheral SPIM3 base address */
#define SPIM3_OFFSET                              (SPIM3_ID * UDMA_PERIPH_OFFSET)
#define SPIM3_BASE                                (UDMA_BASE + SPIM3_OFFSET)
/** Peripheral SPIM3 base pointer */
#define SPIM3                                     ((SPIM_Type *)SPIM3_BASE)
/** Array initializer of SPIM peripheral base addresses */
#define SPIM_BASE_ADDRS                           { SPIM0_BASE, SPIM1_BASE, SPIM2_BASE, SPIM3_BASE }
/** Array initializer of SPIM peripheral base pointers */
#define SPIM_BASE_PTRS                            { SPIM0, SPIM1, SPIM2, SPIM3 }
/** Array initializer of SPIM peripheral base pointers */
#define QSPIM_BASE_PTRS                           { SPIM0 }
/** Interrupt vectors for the SPIM peripheral type */
#define SPIM_RX_IRQS                              { SPIM0_RX_IRQn, SPIM1_RX_IRQn, SPIM2_RX_IRQn, SPIM3_RX_IRQn }
#define SPIM_TX_IRQS                              { SPIM0_TX_IRQn, SPIM1_TX_IRQn, SPIM2_TX_IRQn, SPIM3_TX_IRQn}

#define SPIM_ID_POINTER(id) ((udma_spi_t *)(SPIM0_BASE+(id*UDMA_PERIPH_OFFSET)))

/*!
 * @}
 */ /* end of group SPIM_Peripheral_Access_Layer */


#ifndef __UART_PERIPH__
/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Peripheral_Access_Layer UART Peripheral Access Layer
 * @{
 */

/** UART - Register Layout Typedef */
typedef struct {
  UDMA_Type    UDMA_UART;                           /**< UART UDMA general register, offset: 0x0 */
  __I   uint32_t STATUS;                            /**< UART Status register, offset: 0x20 */
  __IO  uint32_t SETUP;                             /**< UART Configuration register, offset: 0x24 */
} UART_Type;

/* ----------------------------------------------------------------------------
   -- UART Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Masks UART Register Masks
 * @{
 */
/*! @name STATUS - UART Status register */
#define UART_STATUS_TX_BUSY_MASK                    (0x1U)
#define UART_STATUS_TX_BUSY_SHIFT                   (0U)
#define UART_STATUS_TX_BUSY(x)                      (((uint32_t)(((uint32_t)(x)) << UART_STATUS_TX_BUSY_SHIFT)) & UART_STATUS_TX_BUSY_MASK)
#define UART_STATUS_RX_BUSY_MASK                    (0x2U)
#define UART_STATUS_RX_BUSY_SHIFT                   (1U)
#define UART_STATUS_RX_BUSY(x)                      (((uint32_t)(((uint32_t)(x)) << UART_STATUS_RX_BUSY_SHIFT)) & UART_STATUS_RX_BUSY_MASK)
#define UART_STATUS_RX_PE_MASK                      (0x4U)
#define UART_STATUS_RX_PE_SHIFT                     (2U)
#define UART_STATUS_RX_PE(x)                        (((uint32_t)(((uint32_t)(x)) << UART_STATUS_RX_PE_SHIFT)) & UART_STATUS_RX_PE_MASK)

/*! @name SETUP - UART SETUP register */
#define UART_SETUP_PARITY_ENA_MASK                  (0x1U)
#define UART_SETUP_PARITY_ENA_SHIFT                 (0U)
#define UART_SETUP_PARITY_ENA(x)                    (((uint32_t)(((uint32_t)(x)) << UART_SETUP_PARITY_ENA_SHIFT)) & UART_SETUP_PARITY_ENA_MASK)

#define UART_SETUP_BIT_LENGTH_MASK                  (0x6U)
#define UART_SETUP_BIT_LENGTH_SHIFT                 (1U)
#define UART_SETUP_BIT_LENGTH(x)                    (((uint32_t)(((uint32_t)(x)) << UART_SETUP_BIT_LENGTH_SHIFT)) & UART_SETUP_BIT_LENGTH_MASK)

#define UART_SETUP_STOP_BITS_MASK                   (0x8U)
#define UART_SETUP_STOP_BITS_SHIFT                  (3U)
#define UART_SETUP_STOP_BITS(x)                     (((uint32_t)(((uint32_t)(x)) << UART_SETUP_STOP_BITS_SHIFT)) & UART_SETUP_STOP_BITS_MASK)

#define UART_SETUP_TX_ENA_MASK                      (0x100U)
#define UART_SETUP_TX_ENA_SHIFT                     (8U)
#define UART_SETUP_TX_ENA(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_TX_ENA_SHIFT)) & UART_SETUP_TX_ENA_MASK)

#define UART_SETUP_RX_ENA_MASK                      (0x200U)
#define UART_SETUP_RX_ENA_SHIFT                     (9U)
#define UART_SETUP_RX_ENA(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_RX_ENA_SHIFT)) & UART_SETUP_RX_ENA_MASK)

#define UART_SETUP_CLKDIV_MASK                      (0xFFFF0000U)
#define UART_SETUP_CLKDIV_SHIFT                     (16U)
#define UART_SETUP_CLKDIV(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_CLKDIV_SHIFT)) & UART_SETUP_CLKDIV_MASK)

/*!
 * @}
 */ /* end of group UART_Register_Masks */


/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base address */
#define UART0_OFFSET                              (UART0_ID * UDMA_PERIPH_OFFSET)
#define UART0_BASE                                (UDMA_BASE + UART0_OFFSET)
/** Peripheral UART0 base pointer */
#define UART0                                     ((UART_Type *)UART0_BASE)
/** Peripheral UART1 base address */
#define UART1_OFFSET                              (UART1_ID * UDMA_PERIPH_OFFSET)
#define UART1_BASE                                (UDMA_BASE + UART1_OFFSET)
/** Peripheral UART base pointer */
#define UART1                                     ((UART_Type *)UART1_BASE)
/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS                           { UART0_BASE, UART1_BASE }
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                            { UART0, UART1}
/** Interrupt vectors for the UART peripheral type */
#define UART_RX_IRQS                              { UART0_RX_IRQn, UART1_RX_IRQn}
#define UART_TX_IRQS                              { UART0_TX_IRQn, UART1_TX_IRQn }

/*!
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */
#else
#include "periph/uart_periph.h"
#define uart(id) ((uart_t *) UDMA_UART)
#endif

/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/i2c_periph.h"
#define i2c(id) ((i2c_t *) UDMA_I2C(id))


/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/hyper_periph.h"
#define hyperbus(id) ((hyperbus_t *) UDMA_HYPER(id))


/* ----------------------------------------------------------------------------
   -- VIRTUAL_JTAG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup VIRTUAL_JTAG_Peripheral_Access_Layer VIRTUAL_JTAG Peripheral Access Layer
 * @{
 */

/** VIRTUAL_JTAG - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_VIRTUAL_JTAG;                   /**< VIRTUAL_JTAG UDMA general register, offset: 0x0 */
} VIRTUAL_JTAG_Type;

/* ----------------------------------------------------------------------------
   -- VIRTUAL_JTAG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup VIRTUAL_JTAG_Register_Masks VIRTUAL_JTAG Register Masks
 * @{
 */

/*!
 * @}
 */ /* end of group VIRTUAL_JTAG_Register_Masks */


/* VIRTUAL_JTAG - Peripheral instance base addresses */
/** Peripheral VIRTUAL_JTAG base address */
#define VIRTUAL_JTAG_OFFSET                               (JTAG_FIFO_ID * UDMA_PERIPH_OFFSET)
#define VIRTUAL_JTAG_BASE                                 (UDMA_BASE + VIRTUAL_JTAG_OFFSET)
  /** Peripheral VIRTUAL_JTAG base pointer */
#define VIRTUAL_JTAG                                      ((VIRTUAL_JTAG_Type *)VIRTUAL_JTAG_BASE)
/** Array initializer of VIRTUAL_JTAG peripheral base addresses */
#define VIRTUAL_JTAG_BASE_ADDRS                           { VIRTUAL_JTAG_BASE }
/** Array initializer of VIRTUAL_JTAG peripheral base pointers */
#define VIRTUAL_JTAG_BASE_PTRS                            { VIRTUAL_JTAG }
/** Interrupt vectors for the VIRTUAL_JTAG peripheral type */
#define VIRTUAL_JTAG_RX_IRQS                              { VIRTUAL_JTAG_RX_IRQn }
#define VIRTUAL_JTAG_TX_IRQS                              { VIRTUAL_JTAG_TX_IRQn}

/*!
 * @}
 */ /* end of group VIRTUAL_JTAG_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- TCDM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup TCDM_Peripheral_Access_Layer TCDM Peripheral Access Layer
 * @{
 */

/** TCDM - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_TCDM;                           /**< TCDM UDMA general register, offset: 0x0 */
  __IO  uint32_t DST_ADDR;                           /**< TCDM destination address register, offset: 0x20 */
  __IO  uint32_t SRC_ADDR;                           /**< TCDM source address register, offset: 0x24 */
} TCDM_Type;

/* ----------------------------------------------------------------------------
   -- TCDM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup TCDM_Register_Masks TCDM Register Masks
 * @{
 */
/*! @name DST_ADDR - TCDM destination address register */
#define TCDM_DST_ADDR_MASK                    (0x1FFFFU)
#define TCDM_DST_ADDR_SHIFT                   (0U)
#define TCDM_DST_ADDR(x)                      (((uint32_t)(((uint32_t)(x)) << TCDM_DST_ADDR_SHIFT)) & TCDM_DST_ADDR_MASK)

/*! @name SRC_ADDR - TCDM source address register */
#define TCDM_SRC_ADDR_MASK                    (0x1FFFFU)
#define TCDM_SRC_ADDR_SHIFT                   (0U)
#define TCDM_SRC_ADDR(x)                      (((uint32_t)(((uint32_t)(x)) << TCDM_SRC_ADDR_SHIFT)) & TCDM_SRC_ADDR_MASK)

/*!
 * @}
 */ /* end of group TCDM_Register_Masks */


/* TCDM - Peripheral instance base addresses */
/** Peripheral TCDM base address */
#define TCDM_BASE                                 (UDMA_BASE + 7 * 128U)
/** Peripheral TCDM base pointer */
#define TCDM                                      ((TCDM_Type *)TCDM_BASE)
/** Array initializer of TCDM peripheral base addresses */
#define TCDM_BASE_ADDRS                           { TCDM_BASE }
/** Array initializer of TCDM peripheral base pointers */
#define TCDM_BASE_PTRS                            { TCDM }
/** Interrupt vectors for the TCDM peripheral type */
#define TCDM_RX_IRQS                              { TCDM_RX_IRQn }
#define TCDM_TX_IRQS                              { TCDM_TX_IRQn}

/*!
 * @}
 */ /* end of group TCDM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SDIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
   -- SDIO Register Masks
   ---------------------------------------------------------------------------- */

#include "sdio_periph.h"

/*!
 * @}
 */ /* end of group SDIO_Register_Masks */


/* SDIO - Peripheral instance base addresses */
/** Peripheral SDIO base address */
#define SDIO_OFFSET                               (UDMA_SDIO_ID(0) * UDMA_PERIPH_OFFSET)
#define SDIO_BASE                                 (UDMA_BASE + SDIO_OFFSET)
/** Peripheral SDIO base pointer */
#define SDIO                                      ((sdio_t *)SDIO_BASE)
/** Array initializer of SDIO peripheral base addresses */
#define SDIO_BASE_ADDRS                           { SDIO_BASE }
/** Array initializer of SDIO peripheral base pointers */
#define SDIO_BASE_PTRS                            { SDIO }
/** Interrupt vectors for the SDIO peripheral type */
#define SDIO_RX_IRQS                              { SDIO_RX_IRQn }
#define SDIO_TX_IRQS                              { SDIO_TX_IRQn }

/*!
 * @}
 */ /* end of group SDIO_Peripheral_Access_Layer */


#ifndef __I2S_PERIPH__
/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2S_Peripheral_Access_Layer I2S Peripheral Access Layer
 * @{
 */

/** I2S - Register Layout Typedef */
typedef struct {
  UDMA_Type      UDMA_I2S;                         /**< I2S UDMA general register, offset: 0x0 */
  __IO  uint32_t CLKCFG_SETUP;                     /**< I2S clock configuration register, offset: 0x20 */
  __IO  uint32_t SLV_SETUP;                        /**< I2S slave clock configuration register, offset: 0x24 */
  __IO  uint32_t MST_SETUP;                        /**< I2S master clock configuration register, offset: 0x28 */
  __IO  uint32_t PDM_SETUP;                        /**< I2S PDM generator configuration register, offset: 0x2C */
} I2S_Type;

/* ----------------------------------------------------------------------------
   -- I2S Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2S_Register_Masks I2S Register Masks
 * @{
 */
/*! @name CLKCFG_SETUP - I2S clock configuration Register */
#define I2S_CLKCFG_MASTER_CLK_DIV_MASK               (0xFFU)
#define I2S_CLKCFG_MASTER_CLK_DIV_SHIFT              (0U)
#define I2S_CLKCFG_MASTER_CLK_DIV(x)                 (((uint32_t)(((uint32_t)(x)) /*<< I2S_CLKCFG_MASTER_CLK_DIV_SHIFT */)) & I2S_CLKCFG_MASTER_CLK_DIV_MASK)
#define I2S_CLKCFG_SLAVE_CLK_DIV_MASK                (0xFF00U)
#define I2S_CLKCFG_SLAVE_CLK_DIV_SHIFT               (8U)
#define I2S_CLKCFG_SLAVE_CLK_DIV(x)                  (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_CLK_DIV_SHIFT)) & I2S_CLKCFG_SLAVE_CLK_DIV_MASK)
#define I2S_CLKCFG_COMMON_CLK_DIV_MASK               (0xFF0000U)
#define I2S_CLKCFG_COMMON_CLK_DIV_SHIFT              (16U)
#define I2S_CLKCFG_COMMON_CLK_DIV(x)                 (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_COMMON_CLK_DIV_SHIFT)) & I2S_CLKCFG_COMMON_CLK_DIV_MASK)

#define I2S_CLKCFG_PDM_CLK_EN_MASK                   (0x1000000U)
#define I2S_CLKCFG_PDM_CLK_EN_SHIFT                  (24U)
#define I2S_CLKCFG_PDM_CLK_EN(x)                     (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_PDM_CLK_EN_SHIFT)) & I2S_CLKCFG_PDM_CLK_EN_MASK)
#define I2S_CLKCFG_MASTER_CLK_EN_MASK                (0x2000000U)
#define I2S_CLKCFG_MASTER_CLK_EN_SHIFT               (25U)
#define I2S_CLKCFG_MASTER_CLK_EN(x)                  (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_CLK_EN_SHIFT)) & I2S_CLKCFG_MASTER_CLK_EN_MASK)
#define I2S_CLKCFG_SLAVE_CLK_EN_MASK                 (0x4000000U)
#define I2S_CLKCFG_SLAVE_CLK_EN_SHIFT                (26U)
#define I2S_CLKCFG_SLAVE_CLK_EN(x)                   (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_CLK_EN_SHIFT)) & I2S_CLKCFG_SLAVE_CLK_EN_MASK)
#define I2S_CLKCFG_SLAVE_SELECT_EXT_MASK             (0x10000000U)
#define I2S_CLKCFG_SLAVE_SELECT_EXT_SHIFT            (28U)
#define I2S_CLKCFG_SLAVE_SELECT_EXT(x)               (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_SELECT_EXT_SHIFT)) & I2S_CLKCFG_SLAVE_SELECT_EXT_MASK)
#define I2S_CLKCFG_SLAVE_SELECT_NUM_MASK             (0x20000000U)
#define I2S_CLKCFG_SLAVE_SELECT_NUM_SHIFT            (29U)
#define I2S_CLKCFG_SLAVE_SELECT_NUM(x)               (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_SELECT_NUM_SHIFT)) & I2S_CLKCFG_SLAVE_SELECT_NUM_MASK)
#define I2S_CLKCFG_MASTER_SELECT_EXT_MASK            (0x40000000U)
#define I2S_CLKCFG_MASTER_SELECT_EXT_SHIFT           (30U)
#define I2S_CLKCFG_MASTER_SELECT_EXT(x)              (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_SELECT_EXT_SHIFT)) & I2S_CLKCFG_MASTER_SELECT_EXT_MASK)
#define I2S_CLKCFG_MASTER_SELECT_NUM_MASK            (0x80000000U)
#define I2S_CLKCFG_MASTER_SELECT_NUM_SHIFT           (31U)
#define I2S_CLKCFG_MASTER_SELECT_NUM(x)              (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_SELECT_NUM_SHIFT)) & I2S_CLKCFG_MASTER_SELECT_NUM_MASK)

/*! @name SLV_SETUP - I2S slave clock configuration Register */
#define I2S_SLV_WORDS_MASK                          (0x7U)
#define I2S_SLV_WORDS_SHIFT                         (0U)
#define I2S_SLV_WORDS(x)                            (((uint32_t)(((uint32_t)(x)) /* << I2S_SLV_WORDS_SHIFT */)) & I2S_SLV_WORDS_MASK)
#define I2S_SLV_BITS_WORD_MASK                      (0xF00U)
#define I2S_SLV_BITS_WORD_SHIFT                     (8U)
#define I2S_SLV_BITS_WORD(x)                        (((uint32_t)(((uint32_t)(x)) << I2S_SLV_BITS_WORD_SHIFT)) & I2S_SLV_BITS_WORD_MASK)
#define I2S_SLV_LSB_FIRST_MASK                      (0x10000U)
#define I2S_SLV_LSB_FIRST_SHIFT                     (16U)
#define I2S_SLV_LSB_FIRST(x)                        (((uint32_t)(((uint32_t)(x)) << I2S_SLV_LSB_FIRST_SHIFT)) & I2S_SLV_LSB_FIRST_MASK)
#define I2S_SLV_2CH_MASK                            (0x20000U)
#define I2S_SLV_2CH_SHIFT                           (17U)
#define I2S_SLV_2CH(x)                              (((uint32_t)(((uint32_t)(x)) << I2S_SLV_2CH_SHIFT)) & I2S_SLV_2CH_MASK)

#define I2S_SLV_CLK_EN_MASK                         (0x80000000U)
#define I2S_SLV_CLK_EN_SHIFT                        (31U)
#define I2S_SLV_CLK_EN(x)                           (((uint32_t)(((uint32_t)(x)) << I2S_SLV_CLK_EN_SHIFT)) & I2S_SLV_CLK_EN_MASK)

/*! @name MST_SETUP - I2S master clock configuration Register */
#define I2S_MST_WORDS_MASK                          (0x7U)
#define I2S_MST_WORDS_SHIFT                         (0U)
#define I2S_MST_WORDS(x)                            (((uint32_t)(((uint32_t)(x)) /* << I2S_MST_WORDS_SHIFT */)) & I2S_MST_WORDS_MASK)
#define I2S_MST_BITS_WORD_MASK                      (0xF00U)
#define I2S_MST_BITS_WORD_SHIFT                     (8U)
#define I2S_MST_BITS_WORD(x)                        (((uint32_t)(((uint32_t)(x)) << I2S_MST_BITS_WORD_SHIFT)) & I2S_MST_BITS_WORD_MASK)
#define I2S_MST_LSB_FIRST_MASK                      (0x10000U)
#define I2S_MST_LSB_FIRST_SHIFT                     (16U)
#define I2S_MST_LSB_FIRST(x)                        (((uint32_t)(((uint32_t)(x)) << I2S_MST_LSB_FIRST_SHIFT)) & I2S_MST_LSB_FIRST_MASK)
#define I2S_MST_2CH_MASK                            (0x20000U)
#define I2S_MST_2CH_SHIFT                           (17U)
#define I2S_MST_2CH(x)                              (((uint32_t)(((uint32_t)(x)) << I2S_MST_2CH_SHIFT)) & I2S_MST_2CH_MASK)

#define I2S_MST_CLK_EN_MASK                         (0x80000000U)
#define I2S_MST_CLK_EN_SHIFT                        (31U)
#define I2S_MST_CLK_EN(x)                           (((uint32_t)(((uint32_t)(x)) << I2S_MST_CLK_EN_SHIFT)) & I2S_MST_CLK_EN_MASK)

/*! @name PDM_SETUP - I2S PDM configuration Register */
#define I2S_SLV_PDM_SHIFT_MASK                      (0x7U)
#define I2S_SLV_PDM_SHIFT_SHIFT                     (0U)
#define I2S_SLV_PDM_SHIFT(x)                        (((uint32_t)(((uint32_t)(x)) /*<< I2S_SLV_PDM_SHIFT_SHIFT */)) & I2S_SLV_PDM_SHIFT_MASK)
#define I2S_SLV_PDM_DECIMATION_MASK                 (0x1FF8U)
#define I2S_SLV_PDM_DECIMATION_SHIFT                (3U)
#define I2S_SLV_PDM_DECIMATION(x)                   (((uint32_t)(((uint32_t)(x)) << I2S_SLV_PDM_DECIMATION_SHIFT)) & I2S_SLV_PDM_DECIMATION_MASK)
#define I2S_SLV_PDM_MODE_MASK                       (0x6000U)
#define I2S_SLV_PDM_MODE_SHIFT                      (13U)
#define I2S_SLV_PDM_MODE(x)                         (((uint32_t)(((uint32_t)(x)) << I2S_SLV_PDM_MODE_SHIFT)) & I2S_SLV_PDM_MODE_MASK)
#define I2S_SLV_PDM_EN_MASK                         (0x80000000U)
#define I2S_SLV_PDM_EN_SHIFT                        (31U)
#define I2S_SLV_PDM_EN(x)                           (((uint32_t)(((uint32_t)(x)) << I2S_SLV_PDM_EN_SHIFT)) & I2S_SLV_PDM_EN_MASK)

/*!
 * @}
 */ /* end of group I2S_Register_Masks */


/* I2S - Peripheral instance base addresses */
/** Peripheral I2S base address */
#define I2S_OFFSET                               ( I2S_ID * UDMA_PERIPH_OFFSET )
#define I2S_BASE                                 (UDMA_BASE + I2S_OFFSET)
/** Peripheral I2S base pointer */
#define I2S                                      ((I2S_Type *)I2S_BASE)
/** Array initializer of I2S peripheral base addresses */
#define I2S_BASE_ADDRS                           { I2S_BASE }
/** Array initializer of I2S peripheral base pointers */
#define I2S_BASE_PTRS                            { I2S }
/** Interrupt vectors for the I2S peripheral type */
#define I2S_L_IRQS                               { I2S0_L_IRQn, I2S1_L_IRQn }
#define I2S_R_IRQS                               { I2S0_R_IRQn, I2S1_R_IRQn }
/*!
 * @}
 */ /* end of group I2S_Peripheral_Access_Layer */
#else
#include "i2s_periph.h"
#endif

/* ----------------------------------------------------------------------------
   -- CPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cpi_periph.h"
#define cpi(id) ((cpi_t *) UDMA_CPI(id))


/* ----------------------------------------------------------------------------
   -- CSI2 Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CSI2_Peripheral_Access_Layer CSI2 Peripheral Access Layer
 * @{
 */

/** CSI2 - Register Layout Typedef */
typedef struct {
  UDMA_Type      UDMA_CSI2;       /**< CSI2 UDMA general register, offset: 0x0 */
  __I   uint32_t STATUS;          /**< CSI2 Status register, offset: 0x20 */
  __IO  uint32_t CLK_SETUP;       /**< CSI2 Configuration register, offset: 0x24 */

} CSI2_Type;

/* ----------------------------------------------------------------------------
   -- CSI2 Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CSI2_Register_Masks CSI2 Register Masks
 * @{
 */
/*! @name STATUS - CSI2 Status register */

/*! @name CLK_SETUP - CSI2 SETUP register */
#define CSI2_CLK_SETUP_CCI_MASK                (0xFFU)
#define CSI2_CLK_SETUP_CCI_SHIFT               (0U)
#define CSI2_CLK_SETUP_CCI(x)                  (((uint32_t)(((uint32_t)(x)) /* << CSI2_CLK_SETUP_CCI_SHIFT */)) & CSI2_CLK_SETUP_CCI_MASK)

#define CSI2_CLK_SETUP_PIXEL_MASK              (0xFF00U)
#define CSI2_CLK_SETUP_PIXEL_SHIFT             (8U)
#define CSI2_CLK_SETUP_PIXEL(x)                (((uint32_t)(((uint32_t)(x)) << CSI2_CLK_SETUP_PIXEL_SHIFT)) & CSI2_CLK_SETUP_PIXEL_MASK)

/*!
 * @}
 */ /* end of group CSI2_Register_Masks */


/* CSI2 - Peripheral instance base addresses */
/** Peripheral CSI2 base address */
#define CSI2_BASE                                (UDMA_BASE + CSI20_OFFSET)
/** Peripheral CSI2 base pointer */
#define CSI2                                      ((CSI2_Type *)CSI2_BASE)
/** Array initializer of CSI2 peripheral base addresses */
#define CSI2_BASE_ADDRS                           { CSI2_BASE }
/** Array initializer of CSI2 peripheral base pointers */
#define CSI2_BASE_PTRS                            { CSI2 }
/** Interrupt vectors for the CSI2 peripheral type */
#define CSI2_RX_IRQS                              { CSI2_CH0_IRQn, CSI2_CH1_IRQn }

/*!
 * @}
 */ /* end of group CSI2_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MRAM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MRAM_Peripheral_Access_Layer MRAM Peripheral Access Layer
 * @{
 */

/** MRAM - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_MRAM;                        /**< MRAM UDMA general register, offset: 0x0 */
  __IO  uint32_t TX_DADDR;                        /**< MRAM Destination tx address register, offset: 0x20 */
  __IO  uint32_t RX_DADDR;                        /**< MRAM Destination rx address argument register, offset: 0x24 */
  __I   uint32_t STATUS;                          /**< MRAM status register, offset: 0x28 */
  __IO  uint32_t MODE;                            /**< MRAM mode register, offset: 0x2C */
  __IO  uint32_t ERASE_ADDR;                      /**< MRAM erase adddress register, offset: 0x30 */
  __IO  uint32_t ERASE_SIZE;                      /**< MRAM erase size register, offset: 0x34 */
  __IO  uint32_t CLK_DIV;                         /**< MRAM clock divider register, offset: 0x38 */
  __IO  uint32_t TRIGGER;                         /**< MRAM trigger register, offset: 0x3C */
  __I   uint32_t ISR;                             /**< MRAM Interrupt status register, offset: 0x40 */
  __IO  uint32_t IER;                             /**< MRAM Interrupt enable register, offset: 0x44 */
  __O   uint32_t ICR;                             /**< MRAM Interrupt flag clean register, offset: 0x48 */
} MRAM_Type;

/* ----------------------------------------------------------------------------
   -- MRAM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MRAM_Register_Masks MRAM Register Masks
 * @{
 */
/*! @name TX_DADDR - MRAM Destination tx address register */
#define MRAM_TX_DADDR_MASK                     (0xFFFFFU)
#define MRAM_TX_DADDR_SHIFT                    (0U)
#define MRAM_TX_DADDR(x)                       (((uint32_t)(((uint32_t)(x)) /* << MRAM_TX_DADDR_SHIFT */)) & MRAM_TX_DADDR_MASK)

/*! @name RX_DADDR - MRAM Destination rx address register */
#define MRAM_RX_DADDR_MASK                     (0xFFFFFU)
#define MRAM_RX_DADDR_SHIFT                    (0U)
#define MRAM_RX_DADDR(x)                       (((uint32_t)(((uint32_t)(x)) /* << MRAM_RX_DADDR_SHIFT */)) & MRAM_RX_DADDR_MASK)

/*! @name STATUS - MRAM status register */
#define MRAM_STATUS_ERASE_PENDING_MASK         (0x1U)
#define MRAM_STATUS_ERASE_PENDING_SHIFT        (0U)
#define READ_MRAM_STATUS_ERASE_PENDING(x)      (((uint32_t)(((uint32_t)(x)) /* & MRAM_STATUS_ERASE_PENDING_MASK */)) >> MRAM_STATUS_ERASE_PENDING_SHIFT)
#define MRAM_STATUS_TX_BUSY_MASK               (0x2U)
#define MRAM_STATUS_TX_BUSY_SHIFT              (1U)
#define READ_MRAM_STATUS_TX_BUSY(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_TX_BUSY_MASK)) & MRAM_STATUS_TX_BUSY_SHIFT)
#define MRAM_STATUS_RX_BUSY_MASK               (0x4U)
#define MRAM_STATUS_RX_BUSY_SHIFT              (2U)
#define READ_MRAM_STATUS_RX_BUSY(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_RX_BUSY_MASK)) >> MRAM_STATUS_RX_BUSY_SHIFT)
#define MRAM_STATUS_REF_LINE_PENDING_MASK      (0x8U)
#define MRAM_STATUS_REF_LINE_PENDING_SHIFT     (3U)
#define READ_MRAM_STATUS_REF_LINE_PENDING(x)   (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_REF_LINE_PENDING_MASK)) >> MRAM_STATUS_REF_LINE_PENDING_SHIFT)

/*! @name ERASE_ADDR - MRAM erase address register */
#define MRAM_ERASE_ADDR_MASK                   (0x7FFFFU)
#define MRAM_ERASE_ADDR_SHIFT                  (0U)
#define MRAM_ERASE_ADDR(x)                     (((uint32_t)(((uint32_t)(x)) /* << MRAM_ERASE_ADDR_SHIFT */)) & MRAM_ERASE_ADDR_MASK)

/*! @name ERASE_SIZE - MRAM erase size register */
#define MRAM_ERASE_SIZE_MASK                   (0x3FFU)
#define MRAM_ERASE_SIZE_SHIFT                  (0U)
#define MRAM_ERASE_SIZE(x)                     (((uint32_t)(((uint32_t)(x)) /* << MRAM_ERASE_SIZE_SHIFT */)) & MRAM_ERASE_SIZE_MASK)

/*! @name CLK_DIV - MRAM clock divider register */
#define MRAM_CLK_DIV_MASK                      (0xFFU)
#define MRAM_CLK_DIV_SHIFT                     (0U)
#define MRAM_CLK_DIV(x)                        (((uint32_t)(((uint32_t)(x)) /* << MRAM_CLK_DIV_SHIFT */)) & MRAM_CLK_DIV_MASK)
#define MRAM_CLK_DIV_EN_MASK                   (0x100U)
#define MRAM_CLK_DIV_EN_SHIFT                  (8U)
#define MRAM_CLK_DIV_EN(x)                     (((uint32_t)(((uint32_t)(x)) << MRAM_CLK_DIV_EN_SHIFT)) & MRAM_CLK_DIV_EN_MASK)

/*! @name TRIGGER - MRAM trigger register */
#define MRAM_TRIGGER_MASK                      (0x1U)
#define MRAM_TRIGGER_SHIFT                     (0U)
#define MRAM_TRIGGER(x)                        (((uint32_t)(((uint32_t)(x)) /* << MRAM_TRIGGER_SHIFT */)) & MRAM_TRIGGER_MASK)

/*! @name ISR - MRAM interrupt status register */
#define MRAM_ISR_ERASE_DONE_MASK               (0x1U)
#define MRAM_ISR_ERASE_DONE_SHIFT              (0U)
#define READ_MRAM_ISR_ERASE_DONE(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_ERASE_DONE_MASK)) >> MRAM_ISR_ERASE_DONE_SHIFT)
#define MRAM_ISR_PROGRAM_DONE_MASK             (0x2U)
#define MRAM_ISR_PROGRAM_DONE_SHIFT            (1U)
#define READ_MRAM_ISR_PROGRAM_DONE(x)          (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_PROGRAM_DONE_MASK)) >> MRAM_ISR_PROGRAM_DONE_SHIFT)
#define MRAM_ISR_TRIM_CFG_DONE_MASK            (0x4U)
#define MRAM_ISR_TRIM_CFG_DONE_SHIFT           (2U)
#define READ_MRAM_ISR_TRIM_CFG_DONE(x)         (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_TRIM_CFG_DONE_MASK)) >> MRAM_ISR_TRIM_CFG_DONE_SHIFT)
#define MRAM_ISR_REF_LINE_DONE_MASK            (0x8U)
#define MRAM_ISR_REF_LINE_DONE_SHIFT           (3U)
#define READ_MRAM_ISR_REF_LINE_DONE(x)         (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_REF_LINE_DONE_MASK)) >> MRAM_ISR_REF_LINE_DONE_SHIFT)

/*! @name IER - MRAM interrupt enable register */
#define MRAM_IFR_ERASE_EN_MASK                 (0x1U)
#define MRAM_IFR_ERASE_EN_SHIFT                (0U)
#define READ_MRAM_IFR_ERASE_EN(x)              (((uint32_t)(((uint32_t)(x)) /* & MRAM_IFR_ERASE_EN_MASK */)) & MRAM_IFR_ERASE_EN_SHIFT)
#define MRAM_IFR_PROGRAM_EN_MASK               (0x2U)
#define MRAM_IFR_PROGRAM_EN_SHIFT              (1U)
#define READ_MRAM_IFR_PROGRAM_EN(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_PROGRAM_EN_MASK )) & MRAM_IFR_PROGRAM_EN_SHIFT)
#define MRAM_IFR_TRIM_EN_MASK                  (0x4U)
#define MRAM_IFR_TRIM_EN_SHIFT                 (2U)
#define READ_MRAM_IFR_TRIM_EN(x)               (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_TRIM_EN_MASK )) & MRAM_IFR_TRIM_EN_SHIFT)
#define MRAM_IFR_REF_LINE_EN_MASK              (0x8U)
#define MRAM_IFR_REF_LINE_EN_SHIFT             (3U)
#define READ_MRAM_IFR_REF_LINE_EN(x)           (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_REF_LINE_EN_MASK )) & MRAM_IFR_REF_LINE_EN_SHIFT)

/*! @name ICR - MRAM interrupt flag clean register */
#define MRAM_ICR_ERASE_CLR_MASK                 (0x1U)
#define MRAM_ICR_ERASE_CLR_SHIFT                (0U)
#define READ_MRAM_ICR_ERASE_EN(x)              (((uint32_t)(((uint32_t)(x)) /* & MRAM_ICR_ERASE_CLR_MASK */)) & MRAM_ICR_ERASE_CLR_SHIFT)
#define MRAM_ICR_PROGRAM_CLR_MASK               (0x2U)
#define MRAM_ICR_PROGRAM_CLR_SHIFT              (1U)
#define READ_MRAM_ICR_PROGRAM_EN(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_PROGRAM_CLR_MASK )) & MRAM_ICR_PROGRAM_CLR_SHIFT)
#define MRAM_ICR_TRIM_CLR_MASK                  (0x4U)
#define MRAM_ICR_TRIM_CLR_SHIFT                 (2U)
#define READ_MRAM_ICR_TRIM_EN(x)               (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_TRIM_CLR_MASK )) & MRAM_ICR_TRIM_CLR_SHIFT)
#define MRAM_ICR_REF_LINE_CLR_MASK              (0x8U)
#define MRAM_ICR_REF_LINE_CLR_SHIFT             (3U)
#define READ_MRAM_ICR_REF_LINE_EN(x)           (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_REF_LINE_CLR_MASK )) & MRAM_ICR_REF_LINE_CLR_SHIFT)

#define MRAM_CMD_PWUP                          (0x00U)
#define MRAM_CMD_TRIM_CFG                      (0x01U)
#define MRAM_CMD_NORMAL_TX                     (0x02U)
#define MRAM_CMD_ERASE_CHIP                    (0x04U)
#define MRAM_CMD_ERASE_SECTOR                  (0x08U)
#define MRAM_CMD_ERASE_WORD                    (0x10U)
#define MRAM_CMD_PWDN                          (0x20U)
#define MRAM_CMD_READ_RX                       (0x40U)
#define MRAM_CMD_REF_LINE_P                    (0x80U)
#define MRAM_CMD_REF_LINE_AP                   (0xC0U)

// cfg[7:0] = {porb, retb, rstb, nvr, tmen, aref, dpd, eccbyps}
#define MRAM_MODE(cmd, cfg)                    ((uint32_t)(cmd << 8 | cfg))

/*!
 * @}
 */ /* end of group MRAM_Register_Masks */


/* MRAM - Peripheral instance base addresses */
/** Peripheral MRAM base address */
#define MRAM_OFFSET                               ( MRAM_ID * UDMA_PERIPH_OFFSET )
#define MRAM_BASE                                 (UDMA_BASE + MRAM_OFFSET)
/** Peripheral MRAM base pointer */
#define MRAM                                      ((MRAM_Type *)MRAM_BASE)
/** Array initializer of MRAM peripheral base addresses */
#define MRAM_BASE_ADDRS                           { MRAM_BASE }
/** Array initializer of MRAM peripheral base pointers */
#define MRAM_BASE_PTRS                            { MRAM }
/** Interrupt vectors for the MRAM peripheral type */
#define MRAM_RX_IRQS                              { MRAM_RX_IRQn }
#define MRAM_TX_IRQS                              { MRAM_TX_IRQn}

/*!
 * @}
 */ /* end of group MRAM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FILTER Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FILTER_Peripheral_Access_Layer FILTER Peripheral Access Layer
 * @{
 */

/** FILTER - Register Layout Typedef */
typedef struct {
  __IO  uint32_t TX_CH0_ADDR;                     /**< FILTER tx channel 0 address register, offset: 0x00 */
  __IO  uint32_t TX_CH0_CFG;                      /**< FILTER tx channel 0 configuration register, offset: 0x04 */
  __IO  uint32_t TX_CH0_LEN0;                     /**< FILTER tx channel 0 length 0 register, offset: 0x08 */
  __IO  uint32_t TX_CH0_LEN1;                     /**< FILTER tx channel 0 length 1 register, offset: 0x0C */
  __IO  uint32_t TX_CH0_LEN2;                     /**< FILTER tx channel 0 length 2 register, offset: 0x10 */

  __IO  uint32_t TX_CH1_ADDR;                     /**< FILTER tx channel 1 address register, offset: 0x14 */
  __IO  uint32_t TX_CH1_CFG;                      /**< FILTER tx channel 1 configuration register, offset: 0x18 */
  __IO  uint32_t TX_CH1_LEN0;                     /**< FILTER tx channel 1 length 0 register, offset: 0x1C */
  __IO  uint32_t TX_CH1_LEN1;                     /**< FILTER tx channel 1 length 1 register, offset: 0x20 */
  __IO  uint32_t TX_CH1_LEN2;                     /**< FILTER tx channel 1 length 2 register, offset: 0x24 */

  __IO  uint32_t RX_CH_ADDR;                     /**< FILTER rx channel address register, offset: 0x28 */
  __IO  uint32_t RX_CH_CFG;                      /**< FILTER rx channel configuration register, offset: 0x2C */
  __IO  uint32_t RX_CH_LEN0;                     /**< FILTER rx channel length 0 register, offset: 0x30 */
  __IO  uint32_t RX_CH_LEN1;                     /**< FILTER rx channel length 1 register, offset: 0x34 */
  __IO  uint32_t RX_CH_LEN2;                     /**< FILTER rx channel length 2 register, offset: 0x38 */

  __IO  uint32_t AU_CFG;                          /**< FILTER AU register, offset: 0x3C */
  __IO  uint32_t AU_REG0;                         /**< FILTER AU 0 register, offset: 0x40 */
  __IO  uint32_t AU_REG1;                         /**< FILTER AU 1 register, offset: 0x44 */

  __IO  uint32_t BINCU_TH;                        /**< FILTER  register, offset: 0x48 */
  __IO  uint32_t BINCU_CNT;                       /**< FILTER  register, offset: 0x4C */
  __IO  uint32_t BINCU_SETUP;                     /**< FILTER  register, offset: 0x50 */
  __IO  uint32_t BINCU_VAL;                       /**< FILTER  register, offset: 0x54 */

  __IO  uint32_t FILT;                            /**< FILTER register, offset: 0x58 */
  __IO  uint32_t FILT_CMD;                        /**< FILTER command register, offset: 0x5C */
  __IO  uint32_t STATUS;                          /**< FILTER status register, offset: 0x60 */
} FILTER_Type;

/* ----------------------------------------------------------------------------
   -- FILTER Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FILTER_Register_Masks FILTER Register Masks
 * @{
 */
/*! @name RX/TX_CH_CFG  - FILTER RX/TX channel configuration register */
#define FILTER_CH_CFG_SIZE_MASK                   (0x3U)
#define FILTER_CH_CFG_SIZE_SHIFT                  (0U)
#define FILTER_CH_CFG_SIZE(x)                     (((uint32_t)(((uint32_t)(x)) /* << FILTER_CH_CFG_SIZE_SHIFT */)) & FILTER_CH_CFG_SIZE_MASK)
#define FILTER_CH_CFG_MODE_MASK                   (0x300U)
#define FILTER_CH_CFG_MODE_SHIFT                  (8U)
#define FILTER_CH_CFG_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << FILTER_CH_CFG_MODE_SHIFT)) & FILTER_CH_CFG_MODE_MASK)

/*! @name AU_CFG  - FILTER AU configuration register */
#define FILTER_AU_CFG_SIGNED_MASK                    (0x1U)
#define FILTER_AU_CFG_SIGNED_SHIFT                   (0U)
#define FILTER_AU_CFG_SIGNED(x)                      (((uint32_t)(((uint32_t)(x)) /* << FILTER_AU_CFG_SIGNED_SHIFT */)) & FILTER_AU_CFG_SIGNED_MASK)
#define FILTER_AU_CFG_BYPASS_MASK                    (0x2U)
#define FILTER_AU_CFG_BYPASS_SHIFT                   (1U)
#define FILTER_AU_CFG_BYPASS(x)                      (((uint32_t)(((uint32_t)(x)) << FILTER_AU_CFG_BYPASS_SHIFT)) & FILTER_AU_CFG_BYPASS_MASK)
#define FILTER_AU_CFG_MODE_MASK                      (0xF00U)
#define FILTER_AU_CFG_MODE_SHIFT                     (8U)
#define FILTER_AU_CFG_MODE(x)                        (((uint32_t)(((uint32_t)(x)) << FILTER_AU_CFG_MODE_SHIFT)) & FILTER_AU_CFG_MODE_MASK)
#define FILTER_AU_CFG_SHIFT_MASK                     (0x1F0000U)
#define FILTER_AU_CFG_SHIFT_SHIFT                    (16U)
#define FILTER_AU_CFG_SHIFT(x)                       (((uint32_t)(((uint32_t)(x)) << FILTER_AU_CFG_SHIFT_SHIFT)) & FILTER_AU_CFG_SIGNED_MASK)

/*! @name BINCU_CNT  - FILTER Binarization configuration register */
#define FILTER_BINCU_CNT_MASK                        (0xFFFFFU)
#define FILTER_BINCU_CNT_SHIFT                       (0U)
#define FILTER_BINCU_CNT(x)                          (((uint32_t)(((uint32_t)(x)) /* << FILTER_BINCU_CNT_SHIFT */)) & FILTER_BINCU_CNT_MASK)
#define FILTER_BINCU_CNT_EN_MASK                     (0x80000000U)
#define FILTER_BINCU_CNT_EN_SHIFT                    (31U)
#define FILTER_BINCU_CNT_EN(x)                       (((uint32_t)(((uint32_t)(x)) << FILTER_BINCU_CNT_EN_SHIFT)) & FILTER_BINCU_CNT_EN_MASK)

/*! @name FILT_CMD  - FILTER command register */
#define FILTER_FILT_CMD_START_MASK                   (0x1)
#define FILTER_FILT_CMD_START_SHIFT                  (0U)
#define FILTER_FILT_CMD_START(x)                     (((uint32_t)(((uint32_t)(x)) /* << FILTER_FILT_CMD_START_SHIFT */)) & FILTER_FILT_CMD_START_MASK)

/*! @name STATUS  - FILTER status register */
#define FILTER_STATUS_DONE_MASK                      (0x1)
#define FILTER_STATUS_DONE_SHIFT                     (0U)
#define FILTER_STATUS_DONE(x)                        (((uint32_t)(((uint32_t)(x)) /* << FILTER_STATUS_DONE_SHIFT */)) & FILTER_STATUS_DONE_MASK)

/*!
 * @}
 */ /* end of group FILTER_Register_Masks */


/* FILTER - Peripheral instance base addresses */
/** Peripheral FILTER base address */
#define FILTER_OFFSET                               ( UDMA_FILTER(0) * UDMA_PERIPH_OFFSET )
#define FILTER_BASE                                 (UDMA_BASE + FILTER_OFFSET)
/** Peripheral FILTER base pointer */
#define FILTER                                      ((FILTER_Type *)FILTER_BASE)
/** Array initializer of FILTER peripheral base addresses */
#define FILTER_BASE_ADDRS                           { FILTER_BASE }
/** Array initializer of FILTER peripheral base pointers */
#define FILTER_BASE_PTRS                            { FILTER }
/** Interrupt vectors for the FILTER peripheral type */
#define FILTER_RX_IRQS                              { FILTER_RX_IRQn }
#define FILTER_TX_IRQS                              { FILTER_TX_IRQn}

/*!
 * @}
 */ /* end of group FILTER_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- SOC_CTRL  Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOC_CTRL_Peripheral_Access_Layer SOC_CTRL Peripheral Access Layer
 * @{
 */
#include "soc_ctrl_periph.h"


/* SOC_CTRL - Peripheral instance base addresses */
/** Peripheral SOC_CTRL base address */
#define SOC_CTRL_OFFSET                              (0x4000U)
#define SOC_CTRL_BASE                                (SOC_PERI_BASE + SOC_CTRL_OFFSET)
/** Peripheral SOC_CTRL base pointer */
#define SOC_CTRL                                     ((soc_ctrl_t *)SOC_CTRL_BASE)
/** Array initializer of SOC_CTRL base addresses */
#define SOC_CTRL_BASE_ADDRS                          { SOC_CTRL_BASE }
/** Array initializer of SOC_CTRL base pointers */
#define SOC_CTRL_BASE_PTRS                           { SOC_CTRL }

/*!
 * @}
 */ /* end of group SOC_CTRL_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- PORT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Peripheral_Access_Layer PORT Peripheral Access Layer
 * @{
 */

/** PORT - Register Layout Typedef */
typedef struct {
  __IO  uint32_t PADFUN[4];                       /**< PORT pad function register 0, offset: 0x00 */
  __IO  uint32_t PADCFG[16];                       /**< PORT pad configuration register 0, offset: 0x08 */

} PORT_Type;

/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PORT_Register_Masks GPIO Register Masks
 * @{
 */
/*! @name PADFUN - PORT pad mux registers */
#define PORT_PADFUN_MUX_MASK                  (0x3U)
#define PORT_PADFUN_MUX_SHIFT                 (0U)
#define PORT_PADFUN_MUX(x)                    (((uint32_t)(((uint32_t)(x)) << PORT_PADFUN_MUX_SHIFT)) & PORT_PADFUN_MUX_MASK)

/*! @name PADCFG - PORT pad configuration registers */
#define PORT_PADCFG_PULLDOWN_EN_MASK          (0x1U)
#define PORT_PADCFG_PULLDOWN_EN_SHIFT         (0U)
#define PORT_PADCFG_PULLDOWN_EN(x)            (((uint32_t)(((uint32_t)(x)) /* << PORT_PADCFG_PULLDOWN_EN_SHIFT */)) & PORT_PADCFG_PULLDOWN_EN_MASK)
#define PORT_PADCFG_PULLUP_EN_MASK            (0x2U)
#define PORT_PADCFG_PULLUP_EN_SHIFT           (1U)
#define PORT_PADCFG_PULLUP_EN(x)              (((uint32_t)(((uint32_t)(x)) << PORT_PADCFG_PULLUP_EN_SHIFT)) & PORT_PADCFG_PULLUP_EN_MASK)
#define PORT_PADCFG_DRIVE_STRENGTH_MASK       (0xCU)
#define PORT_PADCFG_DRIVE_STRENGTH_SHIFT      (2U)
#define PORT_PADCFG_DRIVE_STRENGTH(x)         (((uint32_t)(((uint32_t)(x)) << PORT_PADCFG_DRIVE_STRENGTH_SHIFT)) & PORT_PADCFG_DRIVE_STRENGTH_MASK)

/*!
 * @}
 */ /* end of group PORT_Register_Masks */


/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base address */
#define PORTA_BASE                              (SOC_CTRL_BASE + 0x010u)
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
   -- Regulator Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/regulator_periph.h"


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
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_MASK       (0x3U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_SHIFT      (0U)
#define PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET(x)         (((uint32_t)(((uint32_t)(x)) /* << PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_SHIFT*/)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET(x)    (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_MASK)) /*>> PMU_CTRL_SLEEP_CTRL_CFG_FLL_RET_SHIFT*/)

#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_MASK      (0x4U)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_SHIFT     (2U)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0(x)        (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_SHIFT)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0(x)   (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_MASK)) >> PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET0_SHIFT)

#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_MASK      (0xFFE00000U)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_SHIFT     (21U)
#define PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1(x)        (((uint32_t)(((uint32_t)(x)) << PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_SHIFT)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_MASK)
#define READ_PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1(x)   (((uint32_t)(((uint32_t)(x)) & PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_MASK)) >> PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET1_SHIFT)

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

/*!
 * @}
 */ /* end of group PMU_CTRL_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- PAD_SLEEP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PAD_SLEEP_Peripheral_Access_Layer PAD_SLEEP Peripheral Access Layer
 * @{
 */

/** PAD_SLEEP - Register Layout Typedef */
typedef struct {
  __IO  uint32_t SLEEP_PADCFG[4];                 /**< PAD_SLEEP sleep pad configuration register 0, offset: 0x010 */
  __IO  uint32_t PAD_SLEEP_EN;                    /**< PAD_SLEEP pad sleep enable register, offset: 0x020 */
  __IO  uint32_t _reserved0[3];                   /**< reserved, offset: 0x024 */
  __IO  uint32_t GPREG;                           /**< PAD_SLEEP greneral register, offset: 0x030 */

} PAD_SLEEP_Type;

/* ----------------------------------------------------------------------------
   -- PAD_SLEEP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PAD_SLEEP_Register_Masks
 * @{
 */
/*! @name PAD_SLEEP_EN - PMU control register */
#define PAD_SLEEP_ENABLE_MASK                     (0x1U)
#define PAD_SLEEP_ENABLE_SHIFT                    (0U)
#define PAD_SLEEP_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << PAD_SLEEP_ENABLE_SHIFT)) & PAD_SLEEP_ENABLE_MASK)

/*!
 * @}
 */ /* end of group PAD_SLEEP_Register_Masks */


/* PAD_SLEEP - Peripheral instance base addresses */
/** Peripheral PAD_SLEEP base address */
#define PAD_SLEEP_BASE                               (SOC_CTRL_BASE + 0x0150u)
/** Peripheral PAD_SLEEP base pointer */
#define PAD_SLEEP                                    ((PAD_SLEEP_Type *)PAD_SLEEP_BASE)
/** Array initializer of PAD_SLEEP base addresses */
#define PAD_SLEEP_BASE_ADDRS                         { PAD_SLEEP_BASE }
/** Array initializer of PAD_SLEEP base pointers */
#define PAD_SLEEP_BASE_PTRS                          { PAD_SLEEP }

/*!
 * @}
 */ /* end of group PAD_SLEEP_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PWM CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWM_CTRL_Peripheral_Access_Layer PWM_CTRL Peripheral Access Layer
 * @{
 */

/** PWM - Register Layout Typedef */
typedef struct {
  __IO  uint32_t EVENT_CFG;       /**< PWM event configuration register, offset: 0x100 */
  __IO  uint32_t CH_EN;           /**< PWM channel enable register, offset: 0x104 */
} PWM_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- PWM_CTRL Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWM_CTRL_Register_Masks PWM_CTRL Register Masks
 * @{
 */

/* Register Access. */
/* Set Event. */
#define PWM_CTRL_EVENT_TIMER_CHAN_SET_MASK               ( 0xFFFFU )
#define PWM_CTRL_EVENT_TIMER_CHAN_SET_SHIFT( x )         ( (uint32_t)(x) )
#define PWM_CTRL_EVENT_TIMER_CHAN_SET( tim, chan, evt )  (((uint32_t)((uint32_t)((uint32_t)tim << 2 | (uint32_t)chan)) << PWM_CTRL_EVENT_TIMER_CHAN_SET_SHIFT( evt << 2 )) & PWM_CTRL_EVENT_TIMER_CHAN_SET_MASK)

/* Enable Event. */
#define PWM_CTRL_EVENT_TIMER_ENA_MASK                    ( 0xF0000U )
#define PWM_CTRL_EVENT_TIMER_ENA_SHIFT                   ( 16 )
#define PWM_CTRL_EVENT_TIMER_ENA( x )                    (((uint32_t)(((uint32_t)(x)) << PWM_CTRL_EVENT_TIMER_ENA_SHIFT)) & PWM_CTRL_EVENT_TIMER_ENA_MASK)

/* Timer enable. */
#define PWM_CTRL_CG_ENA_MASK                             ( 0xFU )
#define PWM_CTRL_CG_ENA_SHIFT                            ( 0 )
#define PWM_CTRL_CG_ENA( x )                             (((uint32_t)(((uint32_t)(x)) << PWM_CTRL_CG_ENA_SHIFT)) & PWM_CTRL_CG_ENA_MASK)

/*!
 * @}
 */ /* end of group PWM_CTRL_Register_Masks */

/* PWM_CTRL - Peripheral instance base addresses */
/** Peripheral PWM_CTRL_CTRL base address */
#define PWM_CTRL_OFFSET                              (0x5100U)
#define PWM_CTRL_BASE                                (SOC_PERI_BASE + PWM_CTRL_OFFSET)
/** Peripheral PWM_CTRL_CTRL base pointer */
#define PWM_CTRL                                     ((PWM_CTRL_Type *)PWM_CTRL_BASE)
/** Array initializer of PWM_CTRL_CTRL base addresses */
#define PWM_CTRL_BASE_ADDRS                          { PWM_CTRL_BASE }
/** Array initializer of PWM_CTRL_CTRL base pointers */
#define PWM_CTRL_BASE_PTRS                           { PWM_CTRL }

/*!
 * @}
 */ /* end of group PWM_CTRL_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PWM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWM_Peripheral_Access_Layer PWM Peripheral Access Layer
 * @{
 */

/** ADV_TIMER - Register Layout Typedef */
typedef struct {
  __O   uint32_t CMD;              /**< TIMER control register, offset: 0x00 */
  __IO  uint32_t CFG;              /**< TIMER configuration register, offset: 0x04 */
  __IO  uint32_t TH;               /**< TIMER threshold register, offset: 0x08 */
  __IO  uint32_t CH_TH[4];         /**< TIMER Channles' threshold register, offset: 0x0c */
  __IO  uint32_t CH_LUT[4];        /**< TIMER Channles' lookup table, offset: 0x1c */
  __I   uint32_t COUNTER;          /**< TIMER Counter register, offset: 0x2c */
} PWM_Type;

/* ----------------------------------------------------------------------------
   -- PWM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PWM_Register_Masks PWM Register Masks
 * @{
 */

/* Register Access. */
/* Send command. */
#define PWM_CMD_MASK                                ( 0x1FU )
#define PWM_CMD_SHIFT                               ( 0 )
#define PWM_CMD( x )                                (((uint32_t)(((uint32_t)(x)) << PWM_CMD_SHIFT)) & PWM_CMD_MASK)

/* Timer config. */
#define PWM_CONFIG_INPUT_SRC_MASK                   ( 0xFFU )
#define PWM_CONFIG_INPUT_SRC_SHIFT                  ( 0 )
#define PWM_CONFIG_INPUT_SRC( x )                   (((uint32_t)(((uint32_t)(x)) << PWM_CONFIG_INPUT_SRC_SHIFT)) & PWM_CONFIG_INPUT_SRC_MASK)

#define PWM_CONFIG_INPUT_MODE_MASK                  ( 0x700U )
#define PWM_CONFIG_INPUT_MODE_SHIFT                 ( 8 )
#define PWM_CONFIG_INPUT_MODE( x )                  (((uint32_t)(((uint32_t)(x)) << PWM_CONFIG_INPUT_MODE_SHIFT)) & PWM_CONFIG_INPUT_MODE_MASK)

#define PWM_CONFIG_CLKSEL_MASK                      ( 0x800U )
#define PWM_CONFIG_CLKSEL_SHIFT                     ( 11 )
#define PWM_CONFIG_CLKSEL( x )                      (((uint32_t)(((uint32_t)(x)) << PWM_CONFIG_CLKSEL_SHIFT)) & PWM_CONFIG_CLKSEL_MASK)

#define PWM_CONFIG_UPDOWNSEL_MASK                   ( 0x1000U )
#define PWM_CONFIG_UPDOWNSEL_SHIFT                  ( 12 )
#define PWM_CONFIG_UPDOWNSEL( x )                   (((uint32_t)(((uint32_t)(x)) << PWM_CONFIG_UPDOWNSEL_SHIFT)) & PWM_CONFIG_UPDOWNSEL_MASK)

#define PWM_CONFIG_PRESCALE_MASK                   ( 0xFF0000U )
#define PWM_CONFIG_PRESCALE_SHIFT                  ( 16 )
#define PWM_CONFIG_PRESCALE( x )                   (((uint32_t)(((uint32_t)(x)) << PWM_CONFIG_PRESCALE_SHIFT)) & PWM_CONFIG_PRESCALE_MASK)

/* Channel config. */
#define PWM_THRESHOLD_LOW_MASK                      ( 0xFFFFU )
#define PWM_THRESHOLD_LOW_SHIFT                     ( 0 )
#define PWM_THRESHOLD_LOW( x )                      (((uint32_t)(((uint32_t)(x)) << PWM_THRESHOLD_LOW_SHIFT)) & PWM_THRESHOLD_LOW_MASK)

#define PWM_THRESHOLD_HIGH_MASK                     ( 0xFFFF0000U )
#define PWM_THRESHOLD_HIGH_SHIFT                    ( 16 )
#define PWM_THRESHOLD_HIGH( x )                     (((uint32_t)(((uint32_t)(x)) << PWM_THRESHOLD_HIGH_SHIFT)) & PWM_THRESHOLD_HIGH_MASK)

/* Channel config. */
#define PWM_CHANNEL_CONFIG_THRESHOLD_MASK           ( 0xFFFFU )
#define PWM_CHANNEL_CONFIG_THRESHOLD_SHIFT          ( 0 )
#define PWM_CHANNEL_CONFIG_THRESHOLD( x )           (((uint32_t)(((uint32_t)(x)) << PWM_CHANNEL_CONFIG_THRESHOLD_SHIFT)) & PWM_CHANNEL_CONFIG_THRESHOLD_MASK)

#define PWM_CHANNEL_CONFIG_MODE_MASK                ( 0x70000U )
#define PWM_CHANNEL_CONFIG_MODE_SHIFT               ( 16 )
#define PWM_CHANNEL_CONFIG_MODE( x )                (((uint32_t)(((uint32_t)(x)) << PWM_CHANNEL_CONFIG_MODE_SHIFT)) & PWM_CHANNEL_CONFIG_MODE_MASK)

/*!
 * @}
 */ /* end of group PWM_Register_Masks */


/* PWM - Peripheral instance base addresses */
/** Peripheral PWM base address */
#define PWM_PERIPH_CHANNEL_OFFSET               (0x5000U)
#define PWM_CHANNEL_OFFSET                      (0x40U)
#define PWM0_BASE                               (SOC_PERI_BASE + PWM_PERIPH_CHANNEL_OFFSET)
/** Peripheral PWM base pointer */
#define PWM0                                    ((PWM_Type *)PWM0_BASE)
/** Peripheral PWM base address */
#define PWM1_BASE                               (PWM0_BASE + PWM_CHANNEL_OFFSET)
/** Peripheral PWM base pointer */
#define PWM1                                    ((PWM_Type *)PWM1_BASE)
/** Peripheral PWM base address */
#define PWM2_BASE                               (PWM1_BASE + PWM_CHANNEL_OFFSET)
/** Peripheral PWM base pointer */
#define PWM2                                    ((PWM_Type *)PWM2_BASE)
/** Peripheral PWM base address */
#define PWM3_BASE                               (PWM2_BASE + PWM_CHANNEL_OFFSET)
/** Peripheral PWM base pointer */
#define PWM3                                    ((PWM_Type *)PWM3_BASE)
/** Array initializer of PWM base addresses */
#define PWM_BASE_ADDRS                          { PWM0_BASE, PWM1_BASE, PWM2_BASE, PWM3_BASE }
/** Array initializer of PWM base pointers */
#define PWM_BASE_PTRS                           { PWM0, PWM1, PWM2, PWM3 }

/*!
 * @}
 */ /* end of group PWM_Peripheral_Access_Layer */



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
  __IO  uint32_t TIMER1_SEL_HI;                 /**< SOCEU timer 1 high register, offset: 0x84 */
  __IO  uint32_t TIMER1_SEL_LO;                 /**< SOCEU timer 1 low register, offset: 0x88 */
  __IO  uint32_t TIMER2_SEL_HI;                 /**< SOCEU timer 2 high register, offset: 0x8C */
  __IO  uint32_t TIMER2_SEL_LO;                 /**< SOCEU timer 2 low register, offset: 0x90 */
} SOCEU_Type;

/* ----------------------------------------------------------------------------
   -- SOCEU Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOCEU_Register_Masks SOCEU Register Masks
 * @{
 */
/* The SOC events number */
#define SOC_EVENTS_REG_NUM              0x08


#define SOCEU_TIMER_SEL_ENABLE_SHIFT  (31)
#define SOCEU_TIMER_SEL_EVT_SHIFT     (0)
#define SOCEU_TIMER_SEL_EVT_MASK      (0xFFU)


#define SOCEU_TIMER_SEL_ENABLE_DIS    (0   << SOCEU_TIMER_SEL_ENABLE_SHIFT)
#define SOCEU_TIMER_SEL_ENABLE_ENA    (1   << SOCEU_TIMER_SEL_ENABLE_SHIFT)
#define SOCEU_TIMER_SEL_EVT_VAL(val)  ((val) << SOCEU_TIMER_SEL_EVT_SHIFT)

/*!
 * @}
 */ /* end of group SOCEU_Register_Masks */


/* SOCEU - Peripheral instance base addresses */
/** Peripheral SOCEU base address */
#define SOCEU_OFFSET                             (0x6000U)
#define SOCEU_BASE                               (SOC_PERI_BASE + SOCEU_OFFSET)
/** Peripheral SOCEU base pointer */
#define SOCEU                                    ((SOCEU_Type *)SOCEU_BASE)
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

/*!
 * @addtogroup PMU_DLC_Peripheral_Access_Layer PMU_DLC Peripheral Access_Layer
 * @{
 */

/** PMU - General Register Layout Typedef */
typedef struct {
  __IO uint32_t PCTRL;                      /**< PMU DLC control register, offset: 0x00 */
  __IO uint32_t PRDATA;                     /**< PMU DLC data register, offset: 0x04 */
  __IO uint32_t SR;                         /**< PMU DLC register, offset: 0x08 */
  __IO uint32_t IMR;                        /**< PMU DLC register, offset: 0x0C */
  __IO uint32_t IFR;                        /**< PMU DLC register, offset: 0x10 */
  __IO uint32_t IOIFR;                      /**< PMU DLC register, offset: 0x14 */
  __IO uint32_t IDIFR;                      /**< PMU DLC register, offset: 0x18 */
  __IO uint32_t IMCIFR;                     /**< PMU DLC register, offset: 0x1C */

} PMU_DLC_Type;

/* ----------------------------------------------------------------------------
   -- PMU_DLC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PMU_DLC_Register_Masks PMU_DLC Register Masks
 * @{
 */

/*! @name PCTRL - PMU DLC PICL control register */
#define PMU_DLC_PCTRL_START_MASK                  (0x1U)
#define PMU_DLC_PCTRL_START_SHIFT                 (0U)
#define PMU_DLC_PCTRL_START(x)                    (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PCTRL_START_SHIFT */)) & PMU_DLC_PCTRL_START_MASK)
#define PMU_DLC_PCTRL_PADDR_MASK                  (0x7FFEU)
#define PMU_DLC_PCTRL_PADDR_SHIFT                 (1U)
#define PMU_DLC_PCTRL_PADDR(x)                    (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_PADDR_SHIFT)) & PMU_DLC_PCTRL_PADDR_MASK)
#define PMU_DLC_PCTRL_DIR_MASK                    (0x8000U)
#define PMU_DLC_PCTRL_DIR_SHIFT                   (15U)
#define PMU_DLC_PCTRL_DIR(x)                      (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_DIR_SHIFT)) & PMU_DLC_PCTRL_DIR_MASK)
#define PMU_DLC_PCTRL_PWDATA_MASK                 (0xFFFF0000U)
#define PMU_DLC_PCTRL_PWDATA_SHIFT                (16U)
#define PMU_DLC_PCTRL_PWDATA(x)                   (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PCTRL_PWDATA_SHIFT)) & PMU_DLC_PCTRL_PWDATA_MASK)

/*! @name PRDATA - PMU DLC PICL data read register */
#define PMU_DLC_PRDATA_PRDATA_MASK                (0xFFU)
#define PMU_DLC_PRDATA_PRDATA_SHIFT               (0U)
#define PMU_DLC_PRDATA_PRDATA(x)                  (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PRDATA_PRDATA_SHIFT */)) & PMU_DLC_PRDATA_PRDATA_MASK)

/*! @name SR - PMU DLC DLC Status register */
#define PMU_DLC_SR_PICL_BUSY_MASK                 (0x1U)
#define PMU_DLC_SR_PICL_BUSY_SHIFT                (0U)
#define PMU_DLC_SR_PICL_BUSY(x)                   (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_SR_PICL_BUSY_SHIFT */)) & PMU_DLC_SR_PICL_BUSY_MASK)
#define PMU_DLC_SR_SCU_BUSY_MASK                  (0x2U)
#define PMU_DLC_SR_SCU_BUSY_SHIFT                 (1U)
#define PMU_DLC_SR_SCU_BUSY(x)                    (((uint32_t)(((uint32_t)(x)) << PMU_DLC_SR_SCU_BUSY_SHIFT)) & PMU_DLC_SR_SCU_BUSY_MASK)

/*! @name IMR - PMU DLC Interrupt mask register */
#define PMU_DLC_IMR_ICU_OK_MASK_MASK              (0x1U)
#define PMU_DLC_IMR_ICU_OK_MASK_SHIFT             (0U)
#define PMU_DLC_IMR_ICU_OK_MASK(x)                (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_IMR_ICU_OK_MASK_SHIFT */)) & PMU_DLC_IMR_ICU_OK_MASK_MASK)
#define PMU_DLC_IMR_ICU_DELAYED_MASK_MASK         (0x2U)
#define PMU_DLC_IMR_ICU_DELAYED_MASK_SHIFT        (1U)
#define PMU_DLC_IMR_ICU_DELAYED_MASK(x)           (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_ICU_DELAYED_MASK_SHIFT)) & PMU_DLC_IMR_ICU_DELAYED_MASK_MASK)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_MASK    (0x4U)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_SHIFT   (2U)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK(x)      (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_SHIFT)) & PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_MASK)
#define PMU_DLC_IMR_PICL_OK_MASK_MASK             (0x8U)
#define PMU_DLC_IMR_PICL_OK_MASK_SHIFT            (3U)
#define PMU_DLC_IMR_PICL_OK_MASK(x)               (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_PICL_OK_MASK_SHIFT)) & PMU_DLC_IMR_PICL_OK_MASK_MASK)
#define PMU_DLC_IMR_SCU_OK_MASK_MASK              (0x10U)
#define PMU_DLC_IMR_SCU_OK_MASK_SHIFT             (4U)
#define PMU_DLC_IMR_SCU_OK_MASK(x)                (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMR_SCU_OK_MASK_SHIFT)) & PMU_DLC_IMR_SCU_OK_MASK_MASK)

/*! @name IFR - PMU DLC Interrupt flag register */
#define PMU_DLC_IFR_ICU_OK_FLAG_MASK              (0x1U)
#define PMU_DLC_IFR_ICU_OK_FLAG_SHIFT             (0U)
#define PMU_DLC_IFR_ICU_OK_FLAG(x)                (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_IFR_ICU_OK_FLAG_SHIFT */)) & PMU_DLC_IFR_ICU_OK_FLAG_MASK)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_MASK         (0x2U)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_SHIFT        (1U)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG(x)           (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_ICU_DELAYED_FLAG_SHIFT)) & PMU_DLC_IFR_ICU_DELAYED_FLAG_MASK)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_MASK    (0x4U)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_SHIFT   (2U)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG(x)      (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_SHIFT)) & PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_MASK)
#define PMU_DLC_IFR_PICL_OK_FLAG_MASK             (0x8U)
#define PMU_DLC_IFR_PICL_OK_FLAG_SHIFT            (3U)
#define PMU_DLC_IFR_PICL_OK_FLAG(x)               (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_PICL_OK_FLAG_SHIFT)) & PMU_DLC_IFR_PICL_OK_FLAG_MASK)
#define PMU_DLC_IFR_SCU_OK_FLAG_MASK              (0x10U)
#define PMU_DLC_IFR_SCU_OK_FLAG_SHIFT             (4U)
#define PMU_DLC_IFR_SCU_OK_FLAG(x)                (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IFR_SCU_OK_FLAG_SHIFT)) & PMU_DLC_IFR_SCU_OK_FLAG_MASK)

/*! @name IOIFR - PMU DLC icu_ok interrupt flag register */
#define PMU_DLC_IOIFR_ICU_OK_FLAG_MASK            (0xFFFFFFFEU)
#define PMU_DLC_IOIFR_ICU_OK_FLAG_SHIFT           (1U)
#define PMU_DLC_IOIFR_ICU_OK_FLAG(x)              (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IOIFR_ICU_OK_FLAG_SHIFT)) & PMU_DLC_IOIFR_ICU_OK_FLAG_MASK)

/*! @name IDIFR - PMU DLC icu_delayed interrupt flag register */
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG_MASK       (0xFFFFFFFEU)
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG_SHIFT      (1U)
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAG(x)         (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IDIFR_ICU_DELAYED_FLAG_SHIFT)) & PMU_DLC_IDIFR_ICU_DELAYED_FLAG_MASK)

/*! @name IMCIFR - PMU DLC icu_mode changed interrupt flag register */
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_MASK     (0xFFFFFFFEU)
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_SHIFT    (1U)
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG(x)       (((uint32_t)(((uint32_t)(x)) << PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_SHIFT)) & PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAG_MASK)

/*! @name PCTRL_PADDR The address to write in the DLC_PADDR register is CHIP_SEL_ADDR[4:0] concatenated with REG_ADDR[4:0]. */
#define PMU_DLC_PICL_REG_ADDR_MASK                (0x1FU)
#define PMU_DLC_PICL_REG_ADDR_SHIFT               (0U)
#define PMU_DLC_PICL_REG_ADDR(x)                  (((uint32_t)(((uint32_t)(x)) /* << PMU_DLC_PICL_REG_ADDR_SHIFT */)) & PMU_DLC_PICL_REG_ADDR_MASK)
#define PMU_DLC_PICL_CHIP_SEL_ADDR_MASK           (0x3E0U)
#define PMU_DLC_PICL_CHIP_SEL_ADDR_SHIFT          (5U)
#define PMU_DLC_PICL_CHIP_SEL_ADDR(x)             (((uint32_t)(((uint32_t)(x)) << PMU_DLC_PICL_CHIP_SEL_ADDR_SHIFT)) & PMU_DLC_PICL_CHIP_SEL_ADDR_MASK)

/* CHIP_SEL_ADDR[4:0]*/
#define  PICL_WIU_ADDR                            0x01
#define  PICL_ICU0_ADDR                           0x02
#define  PICL_ICU1_ADDR                           0x03
#define  PICL_ICU2_ADDR                           0x04
#define  PICL_ICU3_ADDR                           0x05
#define  PICL_DMU0_ADDR                           0x20
#define  PICL_DMU1_ADDR                           0x21

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
#define  ICU_CR                (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU0_ADDR) | PMU_DLC_PICL_REG_ADDR(0x00))
#define  ICU_MR                (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU0_ADDR) | PMU_DLC_PICL_REG_ADDR(0x01))
#define  ICU_ISMR              (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU0_ADDR) | PMU_DLC_PICL_REG_ADDR(0x02))
#define  ICU_DMR_0             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU0_ADDR) | PMU_DLC_PICL_REG_ADDR(0x03))
#define  ICU_DMA_1             (PMU_DLC_PICL_CHIP_SEL_ADDR(PICL_ICU0_ADDR) | PMU_DLC_PICL_REG_ADDR(0x04))

/*!
 * @}
 */ /* end of group PMU_DLC_Register_Masks */

/* PMU DLC- Peripheral instance base addresses */
/** Peripheral PMU DLC base address */
#define PMU_DLC_OFFSET                              (0x7000U)
#define PMU_DLC_BASE                                (SOC_PERI_BASE + PMU_DLC_OFFSET)
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

/*!
 * @addtogroup RTC_APB_Peripheral_Access_Layer RTC_APB Peripheral Access Layer
 * @{
 */

/** RTC_APB - Register Layout Typedef */
typedef struct {
  __IO  uint32_t STATUS;                     /**< RTC_APB_Status register, offset: 0x00 */
  __IO  uint32_t REQUEST;                    /**< RTC_APB_Request register, offset: 0x04 */
  __IO  uint32_t DATA;                       /**< RTC_APB_Data register, offset: 0x08 */
  __IO  uint32_t _reserved;                  /**< reserved, offset: 0x0C */
  __IO  uint32_t IRQ_CTRL;                   /**< RTC_APB_IRQ_Control register, offset: 0x10 */
  __IO  uint32_t IRQ_MASK;                   /**< RTC_APB_IRQ_Mask register, offset: 0x14 */
  __IO  uint32_t IRQ_FLAG;                   /**< RTC_APB_IRQ_Flag register, offset: 0x18 */
} RTC_APB_Type;

/* ----------------------------------------------------------------------------
   -- RTC_APB Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup RTC_APB_Register_Masks RTC_APB Register Masks
 * @{
 */
/*! @name STATUS - RTC_APB STATUS register */
#define RTC_APB_STATUS_IRQ_EN_MASK                     (0x3FU)
#define RTC_APB_STATUS_IRQ_EN_SHIFT                    (0U)
#define RTC_APB_STATUS_IRQ_EN(x)                       (((uint32_t)(((uint32_t)(x))/* << RTC_APB_STATUS_IRQ_EN_SHIFT*/)) & RTC_APB_STATUS_IRQ_EN_MASK)

/*! @name REQUEST - RTC_APB REQUEST Access register */
#define RTC_APB_REQUEST_ACCESS_ADDR_MASK               (0x3FU)
#define RTC_APB_REQUEST_ACCESS_ADDR_SHIFT              (0U)
#define RTC_APB_REQUEST_ACCESS_ADDR(x)                 (((uint32_t)(((uint32_t)(x))/* << RTC_APB_REQUEST_ACCESS_ADDR_SHIFT*/)) & RTC_APB_REQUEST_ACCESS_ADDR_MASK)
#define RTC_APB_REQUEST_ACCESS_RW_MASK                 (0x10000U)
#define RTC_APB_REQUEST_ACCESS_RW_SHIFT                (16U)
#define RTC_APB_REQUEST_ACCESS_RW(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_APB_REQUEST_ACCESS_RW_SHIFT)) & RTC_APB_REQUEST_ACCESS_RW_MASK)

/*! @name IRQ_FLAG - RTC_APB IRQ_FLAG Access register */
#define RTC_APB_IRQ_FLAG_READ_MASK                     (0x1U)
#define RTC_APB_IRQ_FLAG_READ_SHIFT                    (0U)
#define RTC_APB_IRQ_FLAG_READ(x)                       (((uint32_t)(((uint32_t)(x))/* << RTC_APB_IRQ_FLAG_READ_SHIFT*/)) & RTC_APB_IRQ_FLAG_READ_MASK)
#define RTC_APB_IRQ_FLAG_WRITE_MASK                    (0x2U)
#define RTC_APB_IRQ_FLAG_WRITE_SHIFT                   (1U)
#define RTC_APB_IRQ_FLAG_WRITE(x)                      (((uint32_t)(((uint32_t)(x)) << RTC_APB_IRQ_FLAG_WRITE_SHIFT)) & RTC_APB_IRQ_FLAG_WRITE_MASK)





/*!
 * @}
 */ /* end of group RTC_APB_Register_Masks */

/*!
 * @addtogroup RTC_Register_Masks RTC Register Masks
 * @{
 */
/* Bit field of RTC indirect Access Register */
#define RTC_STATUS_ADDR                     0x00
#define RTC_CTRL_ADDR                       0x01
#define RTC_CLK_CTRL_ADDR                   0x02
#define RTC_IRQ_CTRL_ADDR                   0x08
#define RTC_IRQ_MASK_ADDR                   0x09
#define RTC_IRQ_FLAG_ADDR                   0x0A
#define RTC_CALENDAR_CTRL_ADDR              0x10
#define RTC_CALENDAR_TIME_ADDR              0x12
#define RTC_CALENDAR_DATE_ADDR              0x13
#define RTC_ALARM_CTRL_ADDR                 0x18
#define RTC_ALARM_TIME_ADDR                 0x1A
#define RTC_ALARM_DATE_ADDR                 0x1B
#define RTC_TIMER_CTRL_ADDR                 0x20
#define RTC_TIMER_INIT_ADDR                 0x21
#define RTC_TIMER_VALUE_ADDR                0x22
#define RTC_CLKIN_DIV_ADDR                  0x28
#define RTC_REF_CLK_CONF_ADDR               0x2A
#define RTC_TEST_ADDR                       0x30

/*! @name SR - RTC Status register */
#define RTC_SR_INT_RTC_MASK                 (0x1U)
#define RTC_SR_INT_RTC_SHIFT                (0U)
#define RTC_SR_INT_RTC(x)                   (((uint32_t)(((uint32_t)(x))/* << RTC_SR_INT_RTC_SHIFT*/)) & RTC_SR_INT_RTC_MASK)

/*! @name CR - RTC Control register */
#define RTC_CR_STANDBY_MASK                  (0x1U)
#define RTC_CR_STANDBY_SHIFT                 (0U)
#define RTC_CR_STANDBY(x)                    (((uint32_t)(((uint32_t)(x))/* << RTC_CR_STANDBY_SHIFT*/)) & RTC_CR_STANDBY_MASK)
#define RTC_CR_CALIBRATION_EN_MASK           (0x10U)
#define RTC_CR_CALIBRATION_EN_SHIFT          (4U)
#define RTC_CR_CALIBRATION_EN(x)             (((uint32_t)(((uint32_t)(x)) << RTC_CR_CALIBRATION_EN_SHIFT)) & RTC_CR_CALIBRATION_EN_MASK)
#define RTC_CR_SOFT_RST_MASK                 (0x100U)
#define RTC_CR_SOFT_RST_SHIFT                (8U)
#define RTC_CR_SOFT_RST(x)                   (((uint32_t)(((uint32_t)(x)) << RTC_CR_SOFT_RST_SHIFT)) & RTC_CR_SOFT_RST_MASK)

/*! @name CCR - RTC Clock Control register */
#define RTC_CCR_CKOUT_STANDBY_MASK           (0x1U)
#define RTC_CCR_CKOUT_STANDBY_SHIFT          (0U)
#define RTC_CCR_CKOUT_STANDBY(x)             (((uint32_t)(((uint32_t)(x))/* << RTC_CCR_CKOUT_STANDBY_SHIFT*/)) & RTC_CCR_CKOUT_STANDBY_MASK)
#define RTC_CCR_DIV_AUTOCAL_MASK             (0x1000U)
#define RTC_CCR_DIV_AUTOCAL_SHIFT            (12U)
#define RTC_CCR_DIV_AUTOCAL(x)               (((uint32_t)(((uint32_t)(x)) << RTC_CCR_DIV_AUTOCAL_SHIFT)) & RTC_CCR_DIV_AUTOCAL_MASK)
#define RTC_CCR_DIV_COMP_MASK                (0x1F0000U)
#define RTC_CCR_DIV_COMP_SHIFT               (16U)
#define RTC_CCR_DIV_COMP(x)                  (((uint32_t)(((uint32_t)(x)) << RTC_CCR_DIV_COMP_SHIFT)) & RTC_CCR_DIV_COMP_MASK)

/*! @name ICR - RTC IRQ Control register */
/*
  00  INT_RTC high;
  01  INT_RTC low;
  10; INT_RTC high pulse with duration of 1 CKIN cycle
  11; INT_RTC low pulse with duration of 1 CKIN cycle
*/
#define RTC_ICR_FORM_MASK                    (0x3U)
#define RTC_ICR_FORM_SHIFT                   (0U)
#define RTC_ICR_FORM(x)                      (((uint32_t)(((uint32_t)(x))/* << RTC_ICR_FORM_SHIFT*/)) & RTC_ICR_FORM_MASK)

/*! @name IMR - RTC IRQ MASK register */
#define RTC_IMR_ALARM_MASK                   (0x1U)
#define RTC_IMR_ALARM_SHIFT                  (0U)
#define RTC_IMR_ALARM(x)                     (((uint32_t)(((uint32_t)(x))/* << RTC_IMR_ALARM_SHIFT*/)) & RTC_IMR_ALARM_MASK)
#define RTC_IMR_TIMER_MASK                   (0x10U)
#define RTC_IMR_TIMER_SHIFT                  (4U)
#define RTC_IMR_TIMER(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_IMR_TIMER_SHIFT)) & RTC_IMR_TIMER_MASK)
#define RTC_IMR_CALIBRATION_MASK             (0x1000U)
#define RTC_IMR_CALIBRATION_SHIFT            (12U)
#define RTC_IMR_CALIBRATION(x)               (((uint32_t)(((uint32_t)(x)) << RTC_IMR_CALIBRATION_SHIFT)) & RTC_IMR_CALIBRATION_MASK)

/*! @name IFR - RTC IRQ Flag register */
#define RTC_IFR_ALARM_MASK                   (0x1U)
#define RTC_IFR_ALARM_SHIFT                  (0U)
#define RTC_IFR_ALARM(x)                     (((uint32_t)(((uint32_t)(x))/* << RTC_IFR_ALARM_SHIFT*/)) & RTC_IFR_ALARM_MASK)
#define RTC_IFR_TIMER_MASK                   (0x10U)
#define RTC_IFR_TIMER_SHIFT                  (4U)
#define RTC_IFR_TIMER(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_IFR_TIMER_SHIFT)) & RTC_IFR_TIMER_MASK)
#define RTC_IFR_CALIBRATION_MASK             (0x1000U)
#define RTC_IFR_CALIBRATION_SHIFT            (12U)
#define RTC_IFR_CALIBRATION(x)               (((uint32_t)(((uint32_t)(x)) << RTC_IFR_CALIBRATION_SHIFT)) & RTC_IFR_CALIBRATION_MASK)

/*! @name CALENDAR CTRL - RTC CALENDAR Control register */
#define RTC_CALENDAR_CTRL_STANDBY_MASK       (0x1U)
#define RTC_CALENDAR_CTRL_STANDBY_SHIFT      (0U)
#define RTC_CALENDAR_CTRL_STANDBY(x)         (((uint32_t)(((uint32_t)(x))/* << RTC_CALENDAR_CTRL_STANDBY_SHIFT*/)) & RTC_CALENDAR_CTRL_STANDBY_MASK)

/*! @name ALARM_CTRL - RTC Alarm control register */
#define RTC_ALARM_CTRL_STANDBY_MASK           (0x1U)
#define RTC_ALARM_CTRL_STANDBY_SHIFT          (0U)
#define RTC_ALARM_CTRL_STANDBY(x)             (((uint32_t)(((uint32_t)(x))/* << RTC_ALARM_CTRL_STANDBY_SHIFT*/)) & RTC_ALARM_CTRL_STANDBY_MASK)
#define RTC_ALARM_CTRL_MODE_MASK              (0x10U)
#define RTC_ALARM_CTRL_MODE_SHIFT             (4U)
#define RTC_ALARM_CTRL_MODE(x)                (((uint32_t)(((uint32_t)(x)) << RTC_ALARM_CTRL_MODE_SHIFT)) & RTC_ALARM_CTRL_MODE_MASK)
#define RTC_ALARM_CTRL_CONFIG_MASK            (0xF0000U)
#define RTC_ALARM_CTRL_CONFIG_SHIFT           (16U)
#define RTC_ALARM_CTRL_CONFIG(x)              (((uint32_t)(((uint32_t)(x)) << RTC_ALARM_CTRL_CONFIG_SHIFT)) & RTC_ALARM_CTRL_CONFIG_MASK)

/*! @name TIMER - RTC Count down register */
#define RTC_TIMER_STANDBY_MASK                (0x1U)
#define RTC_TIMER_STANDBY_SHIFT               (0U)
#define RTC_TIMER_STANDBY(x)                  (((uint32_t)(((uint32_t)(x))/* << RTC_TIMER_STANDBY_SHIFT*/)) & RTC_TIMER_STANDBY_MASK)
#define RTC_TIMER_MODE_MASK                   (0x10U)
#define RTC_TIMER_MODE_SHIFT                  (4U)
#define RTC_TIMER_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_TIMER_MODE_SHIFT)) & RTC_TIMER_MODE_MASK)

/*! @name CLKIN_DIV - RTC Clock in divider register */
#define RTC_CLKIN_DIV_VAL_MASK                (0xFFFFU)
#define RTC_CLKIN_DIV_VAL_SHIFT               (0U)
#define RTC_CLKIN_DIV_VAL(x)                  (((uint32_t)(((uint32_t)(x))/* << RTC_CLKIN_DIV_VAL_SHIFT*/)) & RTC_CLKIN_DIV_VAL_MASK)

/*! @name CKREF_CONF - RTC Reference Clock configuration */
#define RTC_CKREF_CONF_VAL_MASK               (0x3FFFFFU)
#define RTC_CKREF_CONF_VAL_SHIFT              (0U)
#define RTC_CKREF_CONF_VAL(x)                 (((uint32_t)(((uint32_t)(x))/* << RTC_CKREF_CONF_VAL_SHIFT*/)) & RTC_CKREF_CONF_VAL_MASK)


/*!
 * @}
 */ /* end of group RTC_Register_Masks */



/* RTC_APB - Peripheral instance base addresses */
/** Peripheral RTC_APB base address */
#define RTC_APB_OFFSET                             (0x8000U)
#define RTC_APB_BASE                               (SOC_PERI_BASE + RTC_APB_OFFSET)
/** Peripheral RTC_APB base pointer */
#define RTC_APB                                    ((RTC_APB_Type *)RTC_APB_BASE)
/** Array initializer of RTC_APB base addresses */
#define RTC_APB_BASE_ADDRS                         { RTC_APB_BASE }
/** Array initializer of RTC_APB base pointers */
#define RTC_APB_BASE_PTRS                          { RTC_APB }

/*!
 * @}
 */ /* end of group RTC_APB_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- FC_ICACHE Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_ICACHE_Peripheral_Access_Layer FC_ICACHE Peripheral Access Layer
 * @{
 */

/** FC_ICACHE - Register Layout Typedef */
typedef struct {
  __IO uint32_t ENABLE;            /*!< Offset: 0x00 (R/W ) FC Icache Enable Register  */
  __IO uint32_t FLUSH;             /*!< Offset: 0x04 (R/W)  FC Icache Flush Register */
  __IO uint32_t SEL_FLUSH_ADDR;    /*!< Offset: 0x08 (R/W)  FC Icache Flush Selected Address Register*/
  __IO uint32_t STATUS;            /*!< Offset: 0x0C (R/W)  FC Icache */
} FC_ICACHE_Type;

/* ----------------------------------------------------------------------------
   -- FC_ICACHE Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_ICACHE_Register_Masks FC_ICACHE Register Masks
 * @{
 */

/*!
 * @}
 */ /* end of group FC_ICACHE_Register_Masks */


/* FC_ICACHE - Peripheral instance base addresses */
/** Peripheral FC_ICACHE base address */
#define FC_ICACHE_OFFSET                             (0x8800U)
#define FC_ICACHE_BASE                               (SOC_PERI_BASE + FC_ICACHE_OFFSET)
/** Peripheral FC_ICACHE base pointer */
#define FC_ICACHE                                    ((FC_ICACHE_Type *)FC_ICACHE_BASE)
/** Array initializer of FC_ICACHE base addresses */
#define FC_ICACHE_BASE_ADDRS                         { FC_ICACHE_BASE }
/** Array initializer of FC_ICACHE base pointers */
#define FC_ICACHE_BASE_PTRS                          { FC_ICACHE }

/*!
 * @}
 */ /* end of group FC_ICACHE_Peripheral_Access_Layer */


#include "fc_itc_periph.h"
#define fc_itc ((fc_itc_t *) FC_ITC_ADDR)


/* ----------------------------------------------------------------------------
   -- I3C Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Peripheral_Access_Layer I3C Peripheral Access Layer
 * @{
 */

/** I3C - Register Layout Typedef */
typedef struct {
    __IO uint32_t MASTER_CNTRL;     /**< I3C Master control, offset 0x000. */
    __O  uint32_t CMD_TR_REQ_1;     /**< Data length or Data byte value, offset 0x004. */
    __O  uint32_t CMD_TR_REQ_2;     /**< CMD Type, TID, CCC Code, offset 0x008. */
    __I  uint32_t RESERVED0;        /**< RESERVED, offset 0x00C. */
    __I  uint32_t RESPONSE;         /**< Response register, offset 0x010. */
    __I  uint32_t IBI_RESPONSE;     /**< IBI response register, offset 0x014. */
    __I  uint32_t IBI_DATA;         /**< IBI data, offset 0x018. */
    __I  uint32_t DATA_RX_FIFO;     /**< Data received from slave, offset 0x01C. */
    __IO uint32_t DATA_TX_FIFO;     /**< Data to send to slave, offset 0x020. */
    __I  uint32_t RESERVED1[3];     /**< RESERVED, offset 0x024. */
    __I  uint32_t IRQ_STATUS;       /**< IRQ Status, offset 0x030. */
} I3C_Type;

/* ----------------------------------------------------------------------------
   -- I3C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Register_Masks I3C Register Masks
 * @{
 */
/*! @name DATA.  */
#define I3C_DATA_MASK                (0xFFFFFFFFU)
#define I3C_DATA_SHIFT               (0U)
#define I3C_DATA(x)                  (((uint32_t)(((uint32_t)(x)) << I3C_DATA_SHIFT)) & I3C_DATA_MASK)

/*! @name MASTER_CNTRL - I3C information register */
#define I3C_MASTER_CNTRL_EN_MASK     (0x1U)
#define I3C_MASTER_CNTRL_EN_SHIFT    (0U)
#define I3C_MASTER_CNTRL_EN(x)       (((uint32_t)(((uint32_t)(x)) << I3C_MASTER_CNTRL_EN_SHIFT)) & I3C_MASTER_CNTRL_EN_MASK)

#define I3C_MASTER_RESET_EN_MASK     (0x2U)
#define I3C_MASTER_RESET_EN_SHIFT    (1U)
#define I3C_MASTER_RESET_EN(x)       (((uint32_t)(((uint32_t)(x)) << I3C_MASTER_RESET_EN_SHIFT)) & I3C_MASTER_RESET_EN_MASK)

/*! @name IBI_RESPONSE - I3C information register */
#define I3C_IBI_RSP_DATA_MASK        (0xFFU)
#define I3C_IBI_RSP_DATA_SHIFT       (0U)
//#define I3C_IBI_RSP_DATA(x)        (((uint32_t)(((uint32_t)(x)) << I3C_IBI_RSP_DATA_SHIFT)) & I3C_IBI_RSP_DATA_MASK)

#define I3C_IBI_ID_MASK              (0xFF00U)
#define I3C_IBI_ID_SHIFT             (8U)
//#define I3C_IBI_ID(x)              (((uint32_t)(((uint32_t)(x)) << I3C_IBI_ID_SHIFT)) & I3C_IBI_ID_MASK)

#define I3C_TIMESTAMP_MASK           (0x2000000U)
#define I3C_TIMESTAMP_SHIFT          (24U)
//#define I3C_TIMESTAMP(x)           (((uint32_t)(((uint32_t)(x)) << I3C_TIMESTAMP_SHIFT)) & I3C_TIMESTAMP_MASK)

#define I3C_IBI_STATUS_MASK          (0x80000000U)
#define I3C_IBI_STATUS_SHIFT         (31U)
//#define I3C_IBI_STATUS(x)          (((uint32_t)(((uint32_t)(x)) << I3C_IBI_STATUS_SHIFT)) & I3C_IBI_STATUS_MASK)

/*! @name IRQ_STATUS - I3C information register */
#define I3C_RSP_DONE_MASK            (0x1U)
#define I3C_RSP_DONE_SHIFT           (0U)
//#define I3C_RSP_DONE(x)            (((uint32_t)(((uint32_t)(x)) << I3C_RSP_DONE_SHIFT)) & I3C_RSP_DONE_MASK)

#define I3C_TX_FIFO_FULL_MASK        (0x2U)
#define I3C_TX_FIFO_FULL_SHIFT       (1U)
//#define I3C_TX_FIFO_FULL(x)        (((uint32_t)(((uint32_t)(x)) << I3C_TX_FIFO_FULL_SHIFT)) & I3C_TX_FIFO_FULL_MASK)

#define I3C_TX_FIFO_EMPTY_MASK       (0x4U)
#define I3C_TX_FIFO_EMPTY_SHIFT      (2U)
//#define I3C_TX_FIFO_EMPTY(x)       (((uint32_t)(((uint32_t)(x)) << I3C_TX_FIFO_EMPTY_SHIFT)) & I3C_TX_FIFO_EMPTY_MASK)


#define I3C_CMD_REQ_FULL_MASK        (0x8U)
#define I3C_CMD_REQ_FULL_SHIFT       (3U)
//#define I3C_CMD_REQ_FULL(x)        (((uint32_t)(((uint32_t)(x)) << I3C_CMD_REQ_FULL_SHIFT)) & I3C_CMD_REQ_FULL_MASK)

#define I3C_RX_FIFO_FULL_MASK        (0x10U)
#define I3C_RX_FIFO_FULL_SHIFT       (4U)
//#define I3C_RX_FIFO_FULL(x)        (((uint32_t)(((uint32_t)(x)) << I3C_RX_FIFO_FULL_SHIFT)) & I3C_RX_FIFO_FULL_MASK)

#define I3C_RSP_FIFO_FULL_MASK       (0x20U)
#define I3C_RSP_FIFO_FULL_SHIFT      (5U)
//#define I3C_RSP_FIFO_FULL(x)       (((uint32_t)(((uint32_t)(x)) << I3C_RSP_FIFO_FULL_SHIFT)) & I3C_RSP_FIFO_FULL_MASK)

#define I3C_IBI_TRANSFER_DONE_MASK   (0x40U)
#define I3C_IBI_TRANSFER_DONE_SHIFT  (6U)
//#define I3C_IBI_TRANSFER_DONE(x)   (((uint32_t)(((uint32_t)(x)) << I3C_IBI_TRANSFER_DONE_SHIFT)) & I3C_IBI_TRANSFER_DONE_MASK)

#define I3C_IBI_RX_FIFO_FULL_MASK    (0x80U)
#define I3C_IBI_RX_FIFO_FULL_SHIFT   (7U)
//#define I3C_IBI_RX_FIFO_FULL(x)    (((uint32_t)(((uint32_t)(x)) << I3C_IBI_RX_FIFO_FULL_SHIFT)) & I3C_IBI_RX_FIFO_FULL_MASK)

#define I3C_RST_COMPLETION_MASK      (0x100U)
#define I3C_RST_COMPLETION_SHIFT     (8U)
//#define I3C_RST_COMPLETION(x)      (((uint32_t)(((uint32_t)(x)) << I3C_RST_COMPLETION_SHIFT)) & I3C_RST_COMPLETION_MASK)

/*!
 * @}
 */ /* end of group I3C_Register_Masks */

/* I3C - Peripheral instance base addresses */
#define I3C_PERIPH_OFFSET                       (0xA000U)
#define I3C_PERIPH_LENGTH                       (0x800U)
/** Peripheral I3C base address */
#define I3C0_BASE                               (SOC_PERI_BASE + I3C_PERIPH_OFFSET)
/** Peripheral I3C base pointer */
#define I3C0                                    ((I3C_Type *)I3C0_BASE)
/** Peripheral I3C base address */
#define I3C1_BASE                               (I3C0_BASE + I3C_PERIPH_LENGTH)
/** Peripheral I3C base pointer */
#define I3C1                                    ((I3C_Type *)I3C1_BASE)
/** Array initializer of I3C base addresses */
#define I3C_BASE_ADDRS                          { I3C0_BASE, I3C1_BASE }
/** Array initializer of I3C base pointers */
#define I3C_BASE_PTRS                           { I3C0, I3C1 }

/*!
 * @}
 */ /* end of group I3C_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- I3C Timer Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Timer_Peripheral_Access_Layer I3C Timer Peripheral Access Layer
 * @{
 */

/** I3C Timer - Register Layout Typedef */
typedef struct {
    __IO uint32_t TCAS_TIMER;      /**< Timer for Clock After Start condition, offset 0x040. */
    __IO uint32_t TLOW_OD_TIMER;   /**< Low period of SCL in Open-Drain mode, offset 0x044. */
    __IO uint32_t THIGH_OD_TIMER;  /**< High period of SCL in Open-Drain mode, offset 0x048. */
    __IO uint32_t TLOW_PP_TIMER;   /**< Low period of SCL in Push-Pull mode, offset 0x04C. */
    __IO uint32_t THIGH_PP_TIMER;  /**< High period of SCL in Push-Pull mode, offset 0x050. */
    __IO uint32_t TDS_TIMER;       /**< SDA setup time during both OP & PP modes, offset 0x054. */
    __IO uint32_t THD_PP_TIMER;    /**< SDA data hold time during PP mode, offset 0x058. */
    __IO uint32_t TBCP_TIMER;      /**< Timer for Clock Before Stop condition, offset 0x05C. */
    __IO uint32_t TCBSR_TIMER;     /**< Timer for Clock Before Start Repeated, offset 0x060. */
    __IO uint32_t THD_DDR_TIMER;   /**< SDA data hold during PP mode(DDR mode), offset 0x064. */
    __IO uint32_t BUS_FREE_TIMER;  /**< Bus Free time between Stop and next Start, offset 0x068. */
    __IO uint32_t BUS_AVAL_TIMER ; /**< Time to keep the SDA and SCL to high, offset 0x06C. */
    __IO uint32_t TIDLE_TIMER ;    /**< Extended duration of the Bus Free Condition, offset 0x070. */
    __IO uint32_t TSCO_TIMER;      /**< Maximum time the slave needs to drive the bus the ACK/read data after clock change, offset 0x074. */
} I3C_Timer_Type;

/* ----------------------------------------------------------------------------
   -- I3C Timer Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Timer_Register_Masks I3C Timer Register Masks
 * @{
 */
/*! @name TCAS_TIMER - I3C Timer information register */
#define I3C_START_CND_TIME_MASK       (0xFFFFFFFFU)
#define I3C_START_CND_TIME_SHIFT      (0U)
#define I3C_START_CND_TIME(x)         (((uint32_t)(((uint32_t)(x)) << I3C_START_CND_TIME_SHIFT)) & I3C_START_CND_TIME_MASK)

/*! @name TLOW_OD_TIMER - I3C Timer information register */
#define I3C_LOW_SCL_OD_MASK           (0x1FU)
#define I3C_LOW_SCL_OD_SHIFT          (0U)
#define I3C_LOW_SCL_OD(x)             (((uint32_t)(((uint32_t)(x)) << I3C_LOW_SCL_OD_SHIFT)) & I3C_LOW_SCL_OD_MASK)

/*! @name THIGH_OD_TIMER - I3C Timer information register */
#define I3C_HIGH_SCL_OD_MASK          (0x1FU)
#define I3C_HIGH_SCL_OD_SHIFT         (0U)
#define I3C_HIGH_SCL_OD(x)            (((uint32_t)(((uint32_t)(x)) << I3C_HIGH_SCL_OD_SHIFT)) & I3C_HIGH_SCL_OD_MASK)

/*! @name TLOW_PP_TIMER - I3C Timer information register */
#define I3C_LOW_SCL_PP_MASK           (0x1FU)
#define I3C_LOW_SCL_PP_SHIFT          (0U)
#define I3C_LOW_SCL_PP(x)             (((uint32_t)(((uint32_t)(x)) << I3C_LOW_SCL_PP_SHIFT)) & I3C_LOW_SCL_PP_MASK)

/*! @name THIGH_PP_TIMER - I3C Timer information register */
#define I3C_HIGH_SCL_PP_MASK          (0x1FU)
#define I3C_HIGH_SCL_PP_SHIFT         (0U)
#define I3C_HIGH_SCL_PP(x)            (((uint32_t)(((uint32_t)(x)) << I3C_HIGH_SCL_PP_SHIFT)) & I3C_HIGH_SCL_PP_MASK)

/*! @name TDS_TIMER - I3C Timer information register */
#define I3C_SETUP_TIME_MASK           (0x3U)
#define I3C_SETUP_TIME_SHIFT          (0U)
#define I3C_SETUP_TIME(x)             (((uint32_t)(((uint32_t)(x)) << I3C_SETUP_TIME_SHIFT)) & I3C_SETUP_TIME_MASK)

/*! @name THD_PP_TIMER - I3C Timer information register */
#define I3C_HOLD_TIME_MASK            (0x3U)
#define I3C_HOLD_TIME_SHIFT           (0U)
#define I3C_HOLD_TIME(x)              (((uint32_t)(((uint32_t)(x)) << I3C_HOLD_TIME_SHIFT)) & I3C_HOLD_TIME_MASK)

/*! @name TBCP_TIMER - I3C Timer information register */
#define I3C_STOP_CND_TIME_MASK        (0x3U)
#define I3C_STOP_CND_TIME_SHIFT       (0U)
#define I3C_STOP_CND_TIME(x)          (((uint32_t)(((uint32_t)(x)) << I3C_STOP_CND_TIME_SHIFT)) & I3C_STOP_CND_TIME_MASK)

/*! @name TCBSR_TIMER - I3C Timer information register */
#define I3C_RPT_START_CND_TIME_MASK   (0x3U)
#define I3C_RPT_START_CND_TIME_SHIFT  (0U)
#define I3C_RPT_START_CND_TIME(x)     (((uint32_t)(((uint32_t)(x)) << I3C_RPT_START_CND_TIME_SHIFT)) & I3C_RPT_START_CND_TIME_MASK)

/*! @name THD_DDR_TIMER - I3C Timer information register */
#define I3C_DDR_HOLD_TIME_MASK        (0x3U)
#define I3C_DDR_HOLD_TIME_SHIFT       (0U)
#define I3C_DDR_HOLD_TIME(x)          (((uint32_t)(((uint32_t)(x)) << I3C_DDR_HOLD_TIME_SHIFT)) & I3C_DDR_HOLD_TIME_MASK)

/*! @name BUS_FREE_TIMER - I3C Timer information register */
#define I3C_BUS_FREE_MASK             (0xFFFFFFFFU)
#define I3C_BUS_FREE_SHIFT            (0U)
#define I3C_BUS_FREE(x)               (((uint32_t)(((uint32_t)(x)) << I3C_BUS_FREE_SHIFT)) & I3C_BUS_FREE_MASK)

/*! @name BUS_AVAL_TIMER - I3C Timer information register */
#define I3C_BUS_AVAL_MASK             (0xFFFFFFFFU)
#define I3C_BUS_AVAL_SHIFT            (0U)
#define I3C_BUS_AVAL(x)               (((uint32_t)(((uint32_t)(x)) << I3C_BUS_AVAL_SHIFT)) & I3C_BUS_AVAL_MASK)

/*! @name TIDLE_TIMER - I3C Timer information register */
#define I3C_IDLE_TIME_MASK            (0xFFFFFFFFU)
#define I3C_IDLE_TIME_SHIFT           (0U)
#define I3C_IDLE_TIME(x)              (((uint32_t)(((uint32_t)(x)) << I3C_IDLE_TIME_SHIFT)) & I3C_IDLE_TIME_MASK)

/*! @name TSCO_TIMER - I3C Timer information register */
#define I3C_MAX_TIME_SLAVE_MASK       (0x3U)
#define I3C_MAX_TIME_SLAVE_SHIFT      (0U)
#define I3C_MAX_TIME_SLAVE(x)         (((uint32_t)(((uint32_t)(x)) << I3C_MAX_TIME_SLAVE_SHIFT)) & I3C_MAX_TIME_SLAVE_MASK)

/*!
 * @}
 */ /* end of group I3C_Timer_Register_Masks */

/* I3C TIMER - Peripheral instance base addresses */
#define I3C_TIMER_PERIPH_OFFSET                   (0x40U)
#define I3C_TIMER_PERIPH_LENGTH                   (0x38U)
/** Peripheral I3C TIMER base address */
#define I3C0_TIMER_BASE                           (I3C0_BASE + I3C_TIMER_PERIPH_OFFSET)
/** Peripheral I3C TIMER base pointer */
#define I3C0_TIMER                                ((I3C_TIMER_Type *)I3C0_TIMER_BASE)
/** Peripheral I3C TIMER base address */
#define I3C1_TIMER_BASE                           (I3C1_BASE + I3C_TIMER_PERIPH_OFFSET)
/** Peripheral I3C TIMER base pointer */
#define I3C1_TIMER                                ((I3C_TIMER_Type *)I3C1_TIMER_BASE)
/** Array initializer of I3C TIMER base addresses */
#define I3C_TIMER_BASE_ADDRS                      { I3C0_TIMER_BASE, I3C1_TIMER_BASE }
/** Array initializer of I3C TIMER base pointers */
#define I3C_TIMER_BASE_PTRS                       { I3C0_TIMER, I3C1_TIMER }

/*!
 * @}
 */ /* end of group I3C_Timer_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- I3C_I2C_Timer Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_I2C_Timer_Peripheral_Access_Layer I3C I2C Timer Peripheral Access Layer
 * @{
 */

/** I3C I2C Timer - Register Layout Typedef */
typedef struct {
    __IO uint32_t TSU_STA_TIMER;  /**< SDA data setup time during OD mode for a Repeated Start, offset 0x090. */
    __IO uint32_t THD_STA_TIMER;  /**< SDA data hold time during OD mode, offset 0x094. */
    __IO uint32_t TLOW_TIMER;     /**< Low period of SCL in OD mode, offset 0x098. */
    __IO uint32_t THIGH_TIMER;    /**< High period of SCL in OD mode, offset 0x09C. */
    __IO uint32_t THOLD_TIMER;    /**< Data hold time in OD mode, offset 0x0A0. */
    __IO uint32_t RESERVED;       /**< RESERVED, offset 0x0A4. */
    __IO uint32_t TSU_STOP_TIMER; /**< SDA data setup time during OD mode for Stop condition, offset 0x0A8. */
} I3C_I2C_Timer_Type;

/* ----------------------------------------------------------------------------
   -- I3C I2C Timer Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_I2C_Timer_Register_Masks I3C I2C Timer Register Masks
 * @{
 */
/*! @name TSU_STA_TIMER - I3C I2C Timer information register */
#define I3C_I2C_SETUP_TIME_MASK        (0xFFU)
#define I3C_I2C_SETUP_TIME_SHIFT       (0U)
#define I3C_I2C_SETUP_TIME(x)          (((uint32_t)(((uint32_t)(x)) << I3C_I2C_SETUP_TIME_SHIFT)) & I3C_I2C_SETUP_TIME_MASK)

/*! @name THD_STA_TIMER - I3C I2C Timer information register */
#define I3C_I2C_HOLD_TIME_MASK         (0xFFU)
#define I3C_I2C_HOLD_TIME_SHIFT        (0U)
#define I3C_I2C_HOLD_TIME(x)           (((uint32_t)(((uint32_t)(x)) << I3C_I2C_HOLD_TIME_SHIFT)) & I3C_I2C_HOLD_TIME_MASK)

/*! @name TLOW_TIMER - I3C I2C Timer information register */
#define I3C_I2C_SCL_LOW_MASK           (0xFFFFU)
#define I3C_I2C_SCL_LOW_SHIFT          (0U)
#define I3C_I2C_SCL_LOW(x)             (((uint32_t)(((uint32_t)(x)) << I3C_I2C_SCL_LOW_SHIFT)) & I3C_I2C_SCL_LOW_MASK)

/*! @name THIGH_TIMER - I3C I2C Timer information register */
#define I3C_I2C_SCL_HIGH_MASK          (0xFFFFU)
#define I3C_I2C_SCL_HIGH_SHIFT         (0U)
#define I3C_I2C_SCL_HIGH(x)            (((uint32_t)(((uint32_t)(x)) << I3C_I2C_SCL_HIGH_SHIFT)) & I3C_I2C_SCL_HIGH_MASK)

/*! @name THOLD_TIMER - I3C I2C Timer information register */
#define I3C_I2C_DATA_HOLD_TIME_MASK    (0x3FFU)
#define I3C_I2C_DATA_HOLD_TIME_SHIFT   (0U)
#define I3C_I2C_DATA_HOLD_TIME(x)      (((uint32_t)(((uint32_t)(x)) << I3C_I2C_DATA_HOLD_TIME_SHIFT)) & I3C_I2C_DATA_HOLD_TIME_MASK)

/*! @name TSU_STOP_TIMER - I3C I2C Timer information register */
#define I3C_I2C_STOP_SETUP_TIME_MASK   (0xFFU)
#define I3C_I2C_STOP_SETUP_TIME_SHIFT  (0U)
#define I3C_I2C_STOP_SETUP_TIME(x)     (((uint32_t)(((uint32_t)(x)) << I3C_I2C_STOP_SETUP_TIME_SHIFT)) & I3C_I2C_STOP_SETUP_TIME_MASK)

/*!
 * @}
 */ /* end of group I3C_I2C_Timer_Register_Masks */

/* I3C I2C TIMER - Peripheral instance base addresses */
#define I3C_I2C_TIMER_PERIPH_OFFSET                   (0x90U)
#define I3C_I2C_TIMER_PERIPH_LENGTH                   (0x1CU)
/** Peripheral I3C I2C TIMER base address */
#define I3C0_I2C_TIMER_BASE                           (I3C0_BASE + I3C_I2C_TIMER_PERIPH_OFFSET)
/** Peripheral I3C I2C TIMER base pointer */
#define I3C0_I2C_TIMER                                ((I3C_I2C_TIMER_Type *)I3C0_I2C_TIMER_BASE)
/** Peripheral I3C I2C TIMER base address */
#define I3C1_I2C_TIMER_BASE                           (I3C1_BASE + I3C_I2C_TIMER_PERIPH_OFFSET)
/** Peripheral I3C I2C TIMER base pointer */
#define I3C1_I2C_TIMER                                ((I3C_I2C_TIMER_Type *)I3C1_I2C_TIMER_BASE)
/** Array initializer of I3C I2C TIMER base addresses */
#define I3C_I2C_TIMER_BASE_ADDRS                      { I3C0_I2C_TIMER_BASE, I3C1_I2C_TIMER_BASE }
/** Array initializer of I3C I2C TIMER base pointers */
#define I3C_I2C_TIMER_BASE_PTRS                       { I3C0_I2C_TIMER, I3C1_I2C_TIMER }

/*!
 * @}
 */ /* end of group I3C_I2C_Timer_Peripheral_Access_Layer */

#define I3C_SLAVES 2
/* ----------------------------------------------------------------------------
   -- I3C_DAT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_DAT_Peripheral_Access_Layer I3C DAT Peripheral Access Layer
 * @{
 */

/** I3C DAT - Register Layout Typedef */
typedef struct {
    __IO uint32_t DAT[I3C_SLAVES]; /**< Device Address Table : device type, IBI handling, Dynamic address, offset 0x100. */
} I3C_DAT_Type;

/* ----------------------------------------------------------------------------
   -- I3C DAT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_DAT_Register_Masks I3C DAT Register Masks
 * @{
 */
/*! @name DAT - I3C DAT information register */
#define I3C_DAT_STATIC_ADDR_MASK    (0x7FU)
#define I3C_DAT_STATIC_ADDR_SHIFT   (0U)
#define I3C_DAT_STATIC_ADDR(x)      (((uint32_t)(((uint32_t)(x)) << I3C_DAT_STATIC_ADDR_SHIFT)) & I3C_DAT_STATIC_ADDR_MASK)

#define I3C_DAT_IBI_PAYLOAD_MASK    (0x1U)
#define I3C_DAT_IBI_PAYLOAD_SHIFT   (12U)
#define I3C_DAT_IBI_PAYLOAD(x)      (((uint32_t)(((uint32_t)(x)) << I3C_DAT_IBI_PAYLOAD_SHIFT)) & I3C_DAT_IBI_PAYLOAD_MASK)

#define I3C_DAT_IB_REQUEST_MASK     (0x1U)
#define I3C_DAT_IB_REQUEST_SHIFT    (13U)
#define I3C_DAT_IB_REQUEST(x)       (((uint32_t)(((uint32_t)(x)) << I3C_DAT_IB_REQUEST_SHIFT)) & I3C_DAT_IB_REQUEST_MASK)

#define I3C_DAT_TIMESTAMP_EN_MASK   (0x1U)
#define I3C_DAT_TIMESTAMP_EN_SHIFT  (15U)
#define I3C_DAT_TIMESTAMP_EN(x)     (((uint32_t)(((uint32_t)(x)) << I3C_DAT_TIMESTAMP_EN_SHIFT)) & I3C_DAT_TIMESTAMP_EN_MASK)

#define I3C_DAT_DYNAMIC_ADDR_MASK   (0xFFU)
#define I3C_DAT_DYNAMIC_ADDR_SHIFT  (16U)
#define I3C_DAT_DYNAMIC_ADDR(x)     (((uint32_t)(((uint32_t)(x)) << I3C_DAT_DYNAMIC_ADDR_SHIFT)) & I3C_DAT_DYNAMIC_ADDR_MASK)

#define I3C_DAT_DEVICE_TYPE_MASK    (0x1U)
#define I3C_DAT_DEVICE_TYPE_SHIFT   (31U)
#define I3C_DAT_DEVICE_TYPE(x)      (((uint32_t)(((uint32_t)(x)) << I3C_DAT_DEVICE_TYPE_SHIFT)) & I3C_DAT_DEVICE_TYPE_MASK)

/*!
 * @}
 */ /* end of group I3C_DAT_Register_Masks */

/* I3C DAT - Peripheral instance base addresses */
#define I3C_DAT_PERIPH_OFFSET                   (0x100U)
#define I3C_DAT_PERIPH_LENGTH                   (0x4U)
/** Peripheral I3C DAT base address */
#define I3C0_DAT_BASE                           (I3C0_BASE + I3C_DAT_PERIPH_OFFSET)
/** Peripheral I3C DAT base pointer */
#define I3C0_DAT                                ((I3C_DAT_Type *)I3C0_DAT_BASE)
/** Peripheral I3C DAT base address */
#define I3C1_DAT_BASE                           (I3C1_BASE + I3C_DAT_PERIPH_OFFSET)
/** Peripheral I3C DAT base pointer */
#define I3C1_DAT                                ((I3C_DAT_Type *)I3C1_DAT_BASE)
/** Array initializer of I3C DAT base addresses */
#define I3C_DAT_BASE_ADDRS                      { I3C0_DAT_BASE, I3C1_DAT_BASE }
/** Array initializer of I3C DAT base pointers */
#define I3C_DAT_BASE_PTRS                       { I3C0_DAT, I3C1_DAT }

/*!
 * @}
 */ /* end of group I3C_DAT_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- I3C_DCT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_DCT_Peripheral_Access_Layer I3C DCT Peripheral Access Layer
 * @{
 */

/** I3C DCT - Register Layout Typedef */
typedef struct {
    __I uint32_t DCT[I3C_SLAVES][4]; /**< Device Characteristic Table : BCR, DCR & PID, offset 0x200. */
} I3C_DCT_Type;

/* ----------------------------------------------------------------------------
   -- I3C DCT Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_DCT_Register_Masks I3C DCT Register Masks
 * @{
 */
/*! @name DCT - I3C DCT information register */
/* PID : HIGH=DCT[x][0] | LOW=(DCT[x][1] & 0xFFFF). */
#define I3C_DCT_PID_HIGH_MASK       (0xFFFFFFFFU)
#define I3C_DCT_PID_HIGH_SHIFT      (0U)
//#define I3C_DCT_PID_HIGH(x)       (((uint32_t)(((uint32_t)(x)) << I3C_DCT_PID_HIGH_SHIFT)) & I3C_DCT_PID_HIGH_MASK)

#define I3C_DCT_PID_LOW_MASK        (0xFFFFU)
#define I3C_DCT_PID_LOW_SHIFT       (0U)
//#define I3C_DCT_PID_LOW(x)        (((uint32_t)(((uint32_t)(x)) << I3C_DCT_PID_LOW_SHIFT)) & I3C_DCT_PID_LOW_MASK)

/* Following bits are defined in DCT[x][2]. */
#define I3C_DCT_DCR_MASK            (0xFFU)
#define I3C_DCT_DCR_SHIFT           (0U)
//#define I3C_DCT_DCR(x)            (((uint32_t)(((uint32_t)(x)) << I3C_DCT_DCR_SHIFT)) & I3C_DCT_DCR_MASK)

#define I3C_DCT_BCR_MASK            (0xFFU)
#define I3C_DCT_BCR_SHIFT           (8U)
//#define I3C_DCT_BCR(x)            (((uint32_t)(((uint32_t)(x)) << I3C_DCT_BCR_SHIFT)) & I3C_DCT_BCR_MASK)

#define I3C_DCT_DYNAMIC_ADDR_MASK   (0xFFU)
#define I3C_DCT_DYNAMIC_ADDR_SHIFT  (16U)
//#define I3C_DCT_DYNAMIC_ADDR(x)   (((uint32_t)(((uint32_t)(x)) << I3C_DYNAMIC_ADDR_SHIFT)) & I3C_DYNAMIC_ADDR_MASK)

/*!
 * @}
 */ /* end of group I3C_DCT_Register_Masks */

/* I3C DCT - Peripheral instance base addresses */
#define I3C_DCT_PERIPH_OFFSET                   (0x200U)
#define I3C_DCT_PERIPH_LENGTH                   (0x10U)
/** Peripheral I3C DCT base address */
#define I3C0_DCT_BASE                           (I3C0_BASE + I3C_DCT_PERIPH_OFFSET)
/** Peripheral I3C DCT base pointer */
#define I3C0_DCT                                ((I3C_DCT_Type *)I3C0_DCT_BASE)
/** Peripheral I3C DCT base address */
#define I3C1_DCT_BASE                           (I3C1_BASE + I3C_DCT_PERIPH_OFFSET)
/** Peripheral I3C DCT base pointer */
#define I3C1_DCT                                ((I3C_DCT_Type *)I3C1_DCT_BASE)
/** Array initializer of I3C DCT base addresses */
#define I3C_DCT_BASE_ADDRS                      { I3C0_DCT_BASE, I3C1_DCT_BASE }
/** Array initializer of I3C DCT base pointers */
#define I3C_DCT_BASE_PTRS                       { I3C0_DCT, I3C1_DCT }

/*!
 * @}
 */ /* end of group I3C_DCT_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- EFUSE CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "efuse_periph.h"


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
#define CLUSTER_STDOUT_BASE                                (SOC_PERI_BASE + 0x20000u)
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


#endif /* FEATURE_CLUSTER */

/* ----------------------------------------------------------------------------
   -- QUIDDIKEY Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup QUIDDIKEY_Peripheral_Access_Layer QUIDDIKEY Peripheral Access Layer
 * @{
 */

/** QUIDDIKEY - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t CR;               /**< QUIDDIKEY Control register, offset: 0x000 */
  __IO  uint32_t _reserved0;       /**< reserved, offset: 0x004 */
  __IO  uint32_t SR;               /**< QUIDDIKEY Status register, offset: 0x008 */
  __I   uint32_t AR;               /**< QUIDDIKEY Allow register, offset: 0x00C */
  __IO  uint32_t IER;              /**< QUIDDIKEY Interrupt Enable register, offset: 0x010 */
  __IO  uint32_t IMR;              /**< QUIDDIKEY Interrupt Mask register, offset: 0x014 */
  __IO  uint32_t ISR;              /**< QUIDDIKEY Interrupt Status register, offset: 0x018 */
  __IO  uint32_t _reserved1;       /**< reserved, offset: 0x01C */
  __IO  uint32_t KEY_DEST;         /**< QUIDDIKEY Key Destination register, offset: 0x020 */
  __IO  uint32_t _reserved2[31];     /**< reserved, offset: 0x024 */
  __O   uint32_t DIR;              /**< QUIDDIKEY Data Input register, offset: 0x0A0 */
  __IO  uint32_t _reserved3;       /**< reserved, offset: 0x0A4 */
  __I   uint32_t DOR;              /**< QUIDDIKEY Data Output register, offset: 0x0A8 */
  __IO  uint32_t _reserved4[5];     /**< reserved, offset: 0x0AC */
  __IO  uint32_t MISC;             /**< QUIDDIKEY Miscellaneous register, offset: 0x0C0 */
  __IO  uint32_t _reserved5[3];    /**< reserved, offset: 0x0C4 */
  __IO  uint32_t IF_SR;            /**< QUIDDIKEY Interface Status register, offset: 0x0D0 */
  __IO  uint32_t _reserved6;       /**< reserved, offset: 0x0D4 */
  __IO  uint32_t TEST;             /**< QUIDDIKEY Test register, offset: 0x0D8 */
  __IO  uint32_t _reserved7;       /**< reserved, offset: 0x0DC */
  __I   uint32_t HW_RUC0;          /**< QUIDDIKEY Hardware Restrict User Context 0 register, offset: 0x0E0 */
  __I   uint32_t HW_RUC1;          /**< QUIDDIKEY Hardware Restrict User Context 1 register, offset: 0x0E4 */
  __IO  uint32_t _reserved8[2];    /**< reserved, offset: 0x0E8 */
  __I   uint32_t HW_SETTINGS;      /**< QUIDDIKEY Hardware Setting register, offset: 0x0F0 */
  __I   uint32_t HW_INFO;          /**< QUIDDIKEY Hardware information register, offset: 0x0F4 */
  __I   uint32_t HW_ID;            /**< QUIDDIKEY Hardware Identifier register, offset: 0x0F8 */
  __I   uint32_t HW_VER;           /**< QUIDDIKEY Hardware Version register, offset: 0x0FC */
} QUIDDIKEY_Type;

/* ----------------------------------------------------------------------------
   -- QUIDDIKEY Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup QUIDDIKEY_Register_Masks QUIDDIKEY Register Masks
 * @{
 */
/*! @name CR - QUIDDIKEY control register */
#define QUIDDIKEY_CR_ZEROIZE_MASK                   (0x01U)
#define QUIDDIKEY_CR_ZEROIZE_SHIFT                  (0U)
#define QUIDDIKEY_CR_ZEROIZE(x)                     (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CR_ZEROIZE_SHIFT */)) & QUIDDIKEY_CR_ZEROIZE_MASK)
#define QUIDDIKEY_CR_ENROLL_MASK                    (0x02U)
#define QUIDDIKEY_CR_ENROLL_SHIFT                   (1U)
#define QUIDDIKEY_CR_ENROLL(x)                      (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_ENROLL_SHIFT)) & QUIDDIKEY_CR_ENROLL_MASK)
#define QUIDDIKEY_CR_START_MASK                     (0x04U)
#define QUIDDIKEY_CR_START_SHIFT                    (2U)
#define QUIDDIKEY_CR_START(x)                       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_START_SHIFT)) & QUIDDIKEY_CR_START_MASK)
#define QUIDDIKEY_CR_STOP_MASK                      (0x20U)
#define QUIDDIKEY_CR_STOP_SHIFT                     (5U)
#define QUIDDIKEY_CR_STOP(x)                        (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_STOP_SHIFT)) & QUIDDIKEY_CR_STOP_MASK)
#define QUIDDIKEY_CR_GET_KEY_MASK                   (0x40U)
#define QUIDDIKEY_CR_GET_KEY_SHIFT                  (6U)
#define QUIDDIKEY_CR_GET_KEY(x)                     (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_GET_KEY_SHIFT)) & QUIDDIKEY_CR_GET_KEY_MASK)
#define QUIDDIKEY_CR_UNWRAP_MASK                    (0x80U)
#define QUIDDIKEY_CR_UNWRAP_SHIFT                   (7U)
#define QUIDDIKEY_CR_UNWRAP(x)                      (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_UNWRAP_SHIFT)) & QUIDDIKEY_CR_UNWRAP_MASK)
#define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_MASK     (0x100U)
#define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_SHIFT    (8U)
#define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM(x)       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_SHIFT)) & QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_MASK)
#define QUIDDIKEY_CR_WRAP_MASK                      (0x200U)
#define QUIDDIKEY_CR_WRAP_SHIFT                     (9U)
#define QUIDDIKEY_CR_WRAP(x)                        (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_WRAP_SHIFT)) & QUIDDIKEY_CR_WRAP_MASK)
#define QUIDDIKEY_CR_GENERATED_RANDOM_MASK          (0x8000U)
#define QUIDDIKEY_CR_GENERATED_RANDOM_SHIFT         (15U)
#define QUIDDIKEY_CR_GENERATED_RANDOM(x)            (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_CR_GENERATED_RANDOM_SHIFT)) & QUIDDIKEY_CR_GENERATED_RANDOM_MASK)

/*! @name SR - QUIDDIKEY Status/Interrupt register */
#define QUIDDIKEY_SR_BUSY_MASK                      (0x01U)
#define QUIDDIKEY_SR_BUSY_SHIFT                     (0U)
#define QUIDDIKEY_SR_BUSY(x)                        (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_SR_BUSY_SHIFT */)) & QUIDDIKEY_SR_BUSY_MASK)
#define READ_QUIDDIKEY_SR_BUSY(x)                   (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_BUSY_MASK)) /* >> QUIDDIKEY_SR_BUSY_SHIFT*/)
#define QUIDDIKEY_SR_OK_MASK                        (0x02U)
#define QUIDDIKEY_SR_OK_SHIFT                       (1U)
#define QUIDDIKEY_SR_OK(x)                          (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_OK_SHIFT)) & QUIDDIKEY_SR_OK_MASK)
#define READ_QUIDDIKEY_SR_OK(x)                     (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_OK_MASK)) >> QUIDDIKEY_SR_OK_SHIFT)
#define QUIDDIKEY_SR_ERROR_MASK                     (0x04U)
#define QUIDDIKEY_SR_ERROR_SHIFT                    (2U)
#define QUIDDIKEY_SR_ERROR(x)                       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_ERROR_SHIFT)) & QUIDDIKEY_SR_ERROR_MASK)
#define READ_QUIDDIKEY_SR_ERROR(x)                  (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_ERROR_MASK)) >> QUIDDIKEY_SR_ERROR_SHIFT)
#define QUIDDIKEY_SR_ZEROIZED_MASK                  (0x08U)
#define QUIDDIKEY_SR_ZEROIZED_SHIFT                 (3U)
#define QUIDDIKEY_SR_ZEROIZED(x)                    (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_ZEROIZED_SHIFT)) & QUIDDIKEY_SR_ZEROIZED_MASK)
#define READ_QUIDDIKEY_SR_ZEROIZED(x)               (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_ZEROIZED_MASK)) >> QUIDDIKEY_SR_ZEROIZED_SHIFT)
#define QUIDDIKEY_SR_REJECTED_MASK                  (0x10U)
#define QUIDDIKEY_SR_REJECTED_SHIFT                 (4U)
#define QUIDDIKEY_SR_REJECTED(x)                    (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_REJECTED_SHIFT)) & QUIDDIKEY_SR_REJECTED_MASK)
#define READ_QUIDDIKEY_SR_REJECTED(x)               (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_REJECTED_MASK)) >> QUIDDIKEY_SR_REJECTED_SHIFT)
#define QUIDDIKEY_SR_DI_REQUEST_MASK                (0x20U)
#define QUIDDIKEY_SR_DI_REQUEST_SHIFT               (5U)
#define QUIDDIKEY_SR_DI_REQUEST(x)                  (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_DI_REQUEST_SHIFT)) & QUIDDIKEY_SR_DI_REQUEST_MASK)
#define READ_QUIDDIKEY_SR_DI_REQUEST(x)             (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_DI_REQUEST_MASK)) >> QUIDDIKEY_SR_DI_REQUEST_SHIFT)
#define QUIDDIKEY_SR_DO_REQUEST_MASK                (0x40U)
#define QUIDDIKEY_SR_DO_REQUEST_SHIFT               (6U)
#define QUIDDIKEY_SR_DO_REQUEST(x)                  (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_SR_DO_REQUEST_SHIFT)) & QUIDDIKEY_SR_DO_REQUEST_MASK)
#define READ_QUIDDIKEY_SR_DO_REQUEST(x)             (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_SR_DO_REQUEST_MASK)) >> QUIDDIKEY_SR_DO_REQUEST_SHIFT)

/*! @name AR - QUIDDIKEY register */
#define QUIDDIKEY_AR_ALLOW_ENROLL_MASK                    (0x02U)
#define QUIDDIKEY_AR_ALLOW_ENROLL_SHIFT                   (1U)
#define READ_QUIDDIKEY_AR_ALLOW_ENROLL(x)                 (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_ENROLL_MASK)) >> QUIDDIKEY_AR_ALLOW_ENROLL_SHIFT)
#define QUIDDIKEY_AR_ALLOW_START_MASK                     (0x04U)
#define QUIDDIKEY_AR_ALLOW_START_SHIFT                    (2U)
#define READ_QUIDDIKEY_AR_ALLOW_START(x)                  (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_START_MASK)) >> QUIDDIKEY_AR_ALLOW_START_SHIFT)
#define QUIDDIKEY_AR_ALLOW_STOP_MASK                      (0x20U)
#define QUIDDIKEY_AR_ALLOW_STOP_SHIFT                     (5U)
#define READ_QUIDDIKEY_AR_ALLOW_STOP(x)                   (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_STOP_MASK)) >> QUIDDIKEY_AR_ALLOW_STOP_SHIFT)
#define QUIDDIKEY_AR_ALLOW_GET_KEY_MASK                   (0x40U)
#define QUIDDIKEY_AR_ALLOW_GET_KEY_SHIFT                  (6U)
#define READ_QUIDDIKEY_AR_ALLOW_GET_KEY(x)                (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_GET_KEY_MASK)) >> QUIDDIKEY_AR_ALLOW_GET_KEY_SHIFT)
#define QUIDDIKEY_AR_ALLOW_UNWRAP_MASK                    (0x80U)
#define QUIDDIKEY_AR_ALLOW_UNWRAP_SHIFT                   (7U)
#define READ_QUIDDIKEY_AR_ALLOW_UNWRAP(x)                 (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_UNWRAP_MASK)) >> QUIDDIKEY_AR_ALLOW_UNWRAP_SHIFT)
#define QUIDDIKEY_AR_ALLOW_WRAP_GENERATED_RANDOM_MASK     (0x100U)
#define QUIDDIKEY_AR_ALLOW_WRAP_GENERATED_RANDOM_SHIFT    (8U)
#define READ_QUIDDIKEY_AR_ALLOW_WRAP_GENERATED_RANDOM(x)  (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_WRAP_GENERATED_RANDOM_MASK)) >> QUIDDIKEY_AR_ALLOW_WRAP_GENERATED_RANDOM_SHIFT)
#define QUIDDIKEY_AR_ALLOW_WRAP_MASK                      (0x200U)
#define QUIDDIKEY_AR_ALLOW_WRAP_SHIFT                     (9U)
#define READ_QUIDDIKEY_AR_ALLOW_WRAP(x)                   (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_WRAP_MASK)) >> QUIDDIKEY_AR_ALLOW_WRAP_SHIFT)
#define QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM_MASK          (0x8000U)
#define QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM_SHIFT         (15U)
#define READ_QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM(x)       (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM_MASK)) >> QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM_SHIFT)

/*! @name IER - QUIDDIKEY  register */
#define QUIDDIKEY_IER_INT_EN_MASK                (0x01U)
#define QUIDDIKEY_IER_INT_EN_SHIFT               (0U)
#define QUIDDIKEY_IER_INT_EN(x)                  (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_IER_INT_EN_SHIFT */)) & QUIDDIKEY_IER_INT_EN_MASK)

/*! @name IMR - QUIDDIKEY register */
#define QUIDDIKEY_IMR_INT_EN_BUSY_MASK           (0x01U)
#define QUIDDIKEY_IMR_INT_EN_BUSY_SHIFT          (0U)
#define QUIDDIKEY_IMR_INT_EN_BUSY(x)             (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_IMR_INT_EN_BUSY_SHIFT */)) & QUIDDIKEY_IMR_INT_EN_BUSY_MASK)
#define QUIDDIKEY_IMR_INT_EN_OK_MASK             (0x02U)
#define QUIDDIKEY_IMR_INT_EN_OK_SHIFT            (1U)
#define QUIDDIKEY_IMR_INT_EN_OK(x)               (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_OK_SHIFT)) & QUIDDIKEY_IMR_INT_EN_OK_MASK)
#define QUIDDIKEY_IMR_INT_EN_ERROR_MASK          (0x04U)
#define QUIDDIKEY_IMR_INT_EN_ERROR_SHIFT         (2U)
#define QUIDDIKEY_IMR_INT_EN_ERROR(x)            (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_ERROR_SHIFT)) & QUIDDIKEY_IMR_INT_EN_ERROR_MASK)
#define QUIDDIKEY_IMR_INT_EN_ZEROIZED_MASK       (0x08U)
#define QUIDDIKEY_IMR_INT_EN_ZEROIZED_SHIFT      (3U)
#define QUIDDIKEY_IMR_INT_EN_ZEROIZED(x)         (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_ZEROIZED_SHIFT)) & QUIDDIKEY_IMR_INT_EN_ZEROIZED_MASK)
#define QUIDDIKEY_IMR_INT_EN_REJECTED_MASK       (0x10U)
#define QUIDDIKEY_IMR_INT_EN_REJECTED_SHIFT      (4U)
#define QUIDDIKEY_IMR_INT_EN_REJECTED(x)         (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_REJECTED_SHIFT)) & QUIDDIKEY_IMR_INT_EN_REJECTED_MASK)
#define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_MASK     (0x20U)
#define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_SHIFT    (5U)
#define QUIDDIKEY_IMR_INT_EN_DI_REQUEST(x)       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_DI_REQUEST_SHIFT)) & QUIDDIKEY_IMR_INT_EN_DI_REQUEST_MASK)
#define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_MASK     (0x40U)
#define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_SHIFT    (6U)
#define QUIDDIKEY_IMR_INT_EN_DO_REQUEST(x)       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_IMR_INT_EN_DO_REQUEST_SHIFT)) & QUIDDIKEY_IMR_INT_EN_DO_REQUEST_MASK)

/*! @name ISR - QUIDDIKEY register */
#define QUIDDIKEY_ISR_INT_BUSY_MASK               (0x01U)
#define QUIDDIKEY_ISR_INT_BUSY_SHIFT              (0U)
#define QUIDDIKEY_ISR_INT_BUSY(x)                 (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_ISR_INT_BUSY_SHIFT */)) & QUIDDIKEY_ISR_INT_BUSY_MASK)
#define READ_QUIDDIKEY_ISR_INT_BUSY(x)            (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_BUSY_MASK)) /* >> QUIDDIKEY_ISR_INT_BUSY_SHIFT*/)
#define QUIDDIKEY_ISR_INT_OK_MASK                 (0x02U)
#define QUIDDIKEY_ISR_INT_OK_SHIFT                (1U)
#define QUIDDIKEY_ISR_INT_OK(x)                   (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_OK_SHIFT)) & QUIDDIKEY_ISR_INT_OK_MASK)
#define READ_QUIDDIKEY_ISR_INT_OK(x)              (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_OK_MASK)) >> QUIDDIKEY_ISR_INT_OK_SHIFT)
#define QUIDDIKEY_ISR_INT_ERROR_MASK              (0x04U)
#define QUIDDIKEY_ISR_INT_ERROR_SHIFT             (2U)
#define QUIDDIKEY_ISR_INT_ERROR(x)                (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_ERROR_SHIFT)) & QUIDDIKEY_ISR_INT_ERROR_MASK)
#define READ_QUIDDIKEY_ISR_INT_ERROR(x)           (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_ERROR_MASK)) >> QUIDDIKEY_ISR_INT_ERROR_SHIFT)
#define QUIDDIKEY_ISR_INT_ZEROIZED_MASK           (0x08U)
#define QUIDDIKEY_ISR_INT_ZEROIZED_SHIFT          (3U)
#define QUIDDIKEY_ISR_INT_ZEROIZED(x)             (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_ZEROIZED_SHIFT)) & QUIDDIKEY_ISR_INT_ZEROIZED_MASK)
#define READ_QUIDDIKEY_ISR_INT_ZEROIZED(x)        (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_ZEROIZED_MASK)) >> QUIDDIKEY_ISR_INT_ZEROIZED_SHIFT)
#define QUIDDIKEY_ISR_INT_REJECTED_MASK           (0x10U)
#define QUIDDIKEY_ISR_INT_REJECTED_SHIFT          (4U)
#define QUIDDIKEY_ISR_INT_REJECTED(x)             (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_REJECTED_SHIFT)) & QUIDDIKEY_ISR_INT_REJECTED_MASK)
#define READ_QUIDDIKEY_ISR_INT_REJECTED(x)        (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_REJECTED_MASK)) >> QUIDDIKEY_ISR_INT_REJECTED_SHIFT)
#define QUIDDIKEY_ISR_INT_DI_REQUEST_MASK         (0x20U)
#define QUIDDIKEY_ISR_INT_DI_REQUEST_SHIFT        (5U)
#define QUIDDIKEY_ISR_INT_DI_REQUEST(x)           (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_DI_REQUEST_SHIFT)) & QUIDDIKEY_ISR_INT_DI_REQUEST_MASK)
#define READ_QUIDDIKEY_ISR_INT_DI_REQUEST(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_DI_REQUEST_MASK)) >> QUIDDIKEY_ISR_INT_DI_REQUEST_SHIFT)
#define QUIDDIKEY_ISR_INT_DO_REQUEST_MASK         (0x40U)
#define QUIDDIKEY_ISR_INT_DO_REQUEST_SHIFT        (6U)
#define QUIDDIKEY_ISR_INT_DO_REQUEST(x)           (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_ISR_INT_DO_REQUEST_SHIFT)) & QUIDDIKEY_ISR_INT_DO_REQUEST_MASK)
#define READ_QUIDDIKEY_ISR_INT_DO_REQUEST(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_ISR_INT_DO_REQUEST_MASK)) >> QUIDDIKEY_ISR_INT_DO_REQUEST_SHIFT)

/*! @name KEY_DEST - QUIDDIKEY  register */
#define QUIDDIKEY_KEY_DEST_REG_MASK               (0x01U)
#define QUIDDIKEY_KEY_DEST_REG_SHIFT              (0U)
#define QUIDDIKEY_KEY_DEST_REG(x)                 (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_KEY_DEST_REG_SHIFT */)) & QUIDDIKEY_KEY_DEST_REG_MASK)
#define QUIDDIKEY_KEY_DEST_KO_MASK                (0x02U)
#define QUIDDIKEY_KEY_DEST_KO_SHIFT               (1U)
#define QUIDDIKEY_KEY_DEST_KO(x)                  (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_KEY_DEST_KO_SHIFT )) & QUIDDIKEY_KEY_DEST_KO_MASK)

/*! @name DIR - QUIDDIKEY  register */
/*! @name DOR - QUIDDIKEY  register */
/*! @name MISC - QUIDDIKEY  register */
#define QUIDDIKEY_MISC_DATA_ENDIANNESS_MASK         (0x01U)
#define QUIDDIKEY_MISC_DATA_ENDIANNESS_SHIFT        (0U)
#define QUIDDIKEY_MISC_DATA_ENDIANNESS(x)           (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_MISC_DATA_ENDIANNESS_SHIFT */)) & QUIDDIKEY_MISC_DATA_ENDIANNESS_MASK)

/*! @name IF_SR - QUIDDIKEY  register */
#define QUIDDIKEY_IF_SR_APB_ERROR_MASK              (0x01U)
#define QUIDDIKEY_IF_SR_APB_ERROR_SHIFT             (0U)
#define QUIDDIKEY_IF_SR_APB_ERROR(x)                (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_IF_SR_APB_ERROR_SHIFT */)) & QUIDDIKEY_IF_SR_APB_ERROR_MASK)

/*! @name TEST - QUIDDIKEY  register */
#define QUIDDIKEY_TEST_ENABLE_MASK                 (0x01U)
#define QUIDDIKEY_TEST_ENABLE_SHIFT                (0U)
#define QUIDDIKEY_TEST_ENABLE(x)                   (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_TEST_ENABLE_SHIFT */)) & QUIDDIKEY_TEST_ENABLE_MASK)
#define QUIDDIKEY_TEST_RUNNING_MASK                (0x10U)
#define QUIDDIKEY_TEST_RUNNING_SHIFT               (4U)
#define QUIDDIKEY_TEST_RUNNING(x)                  (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_TEST_RUNNING_SHIFT)) & QUIDDIKEY_TEST_RUNNING_MASK)
#define QUIDDIKEY_TEST_ACTIVE_MASK                 (0x20U)
#define QUIDDIKEY_TEST_ACTIVE_SHIFT                (5U)
#define QUIDDIKEY_TEST_ACTIVE(x)                   (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_TEST_ACTIVE_SHIFT)) & QUIDDIKEY_TEST_ACTIVE_MASK)
#define QUIDDIKEY_TEST_OK_MASK                     (0x40U)
#define QUIDDIKEY_TEST_OK_SHIFT                    (6U)
#define QUIDDIKEY_TEST_OK(x)                       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_TEST_OK_SHIFT)) & QUIDDIKEY_TEST_OK_MASK)
#define QUIDDIKEY_TEST_ERROR_MASK                  (0x80U)
#define QUIDDIKEY_TEST_ERROR_SHIFT                 (7U)
#define QUIDDIKEY_TEST_ERROR(x)                    (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_TEST_ERROR_SHIFT)) & QUIDDIKEY_TEST_ERROR_MASK)
#define QUIDDIKEY_TEST_ALLOW_MASK                  (0x80000000U)
#define QUIDDIKEY_TEST_ALLOW_SHIFT                 (31U)
#define QUIDDIKEY_TEST_ALLOW(x)                    (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_TEST_ALLOW_SHIFT)) & QUIDDIKEY_TEST_ALLOW_MASK)

/*! @name HW_RUC0 - QUIDDIKEY  register */
/*! @name Context Filed - 0 */
#define QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK        (0x1FFFU)
#define QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT       (0U)
#define QUIDDIKEY_CONTEXT_DATA_LENGTH(x)          (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT */)) & QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK)
#define READ_QUIDDIKEY_CONTEXT_DATA_LENGTH(x)     (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK)) /* >> QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT */)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK         (0x1FFFU)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT        (0U)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH(x)           (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT */)) & QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_LENGTH(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK)) /* >> QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT */)
#define QUIDDIKEY_CONTEXT_TYPE_MASK               (0xFFFF0000U)
#define QUIDDIKEY_CONTEXT_TYPE_SHIFT              (16U)
#define QUIDDIKEY_CONTEXT_TYPE(x)                 (((uint32_t)(((uint32_t)(x))  << QUIDDIKEY_CONTEXT_TYPE_SHIFT)) & QUIDDIKEY_CONTEXT_TYPE_MASK)
#define READ_QUIDDIKEY_CONTEXT_TYPE(x)            (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_TYPE_MASK))>> QUIDDIKEY_CONTEXT_TYPE_SHIFT)


/*! @name HW_RUC1 - QUIDDIKEY  register */
/*! @name Context Filed - 1 */
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK        (0xFF)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT       (0U)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(x)          (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT */)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(x)     (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK)) /* >> QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT*/)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK         (0xFF00)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT        (8U)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(x)           (((uint32_t)(((uint32_t)(x))  << QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK))>> QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT)
#define QUIDDIKEY_CONTEXT_KEY_TYPE_MASK                  (0xFFFF0000U)
#define QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT                 (16U)
#define QUIDDIKEY_CONTEXT_KEY_TYPE(x)                    (((uint32_t)(((uint32_t)(0x0000))  << QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT)) & QUIDDIKEY_CONTEXT_KEY_TYPE_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_TYPE(x)               (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_TYPE_MASK))>> QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT)

/*! @name HW_SETTINGS - QUIDDIKEY register */
#define QUIDDIKEY_HW_SETTINGS_DIS_ENROLL_MASK                    (0x02U)
#define QUIDDIKEY_HW_SETTINGS_DIS_ENROLL_SHIFT                   (1U)
#define QUIDDIKEY_HW_SETTINGS_DIS_ENROLL(x)                      (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_ENROLL_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_ENROLL_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_START_MASK                     (0x04U)
#define QUIDDIKEY_HW_SETTINGS_DIS_START_SHIFT                    (2U)
#define QUIDDIKEY_HW_SETTINGS_DIS_START(x)                       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_START_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_START_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_STOP_MASK                      (0x20U)
#define QUIDDIKEY_HW_SETTINGS_DIS_STOP_SHIFT                     (5U)
#define QUIDDIKEY_HW_SETTINGS_DIS_STOP(x)                        (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_STOP_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_STOP_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_GET_KEY_MASK                   (0x40U)
#define QUIDDIKEY_HW_SETTINGS_DIS_GET_KEY_SHIFT                  (6U)
#define QUIDDIKEY_HW_SETTINGS_DIS_GET_KEY(x)                     (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_GET_KEY_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_GET_KEY_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_UNWRAP_MASK                    (0x80U)
#define QUIDDIKEY_HW_SETTINGS_DIS_UNWRAP_SHIFT                   (7U)
#define QUIDDIKEY_HW_SETTINGS_DIS_UNWRAP(x)                      (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_UNWRAP_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_UNWRAP_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP_GENERATED_RANDOM_MASK     (0x100U)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP_GENERATED_RANDOM_SHIFT    (8U)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP_GENERATED_RANDOM(x)       (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_WRAP_GENERATED_RANDOM_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_WRAP_GENERATED_RANDOM_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP_MASK                      (0x200U)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP_SHIFT                     (9U)
#define QUIDDIKEY_HW_SETTINGS_DIS_WRAP(x)                        (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_WRAP_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_WRAP_MASK)
#define QUIDDIKEY_HW_SETTINGS_DIS_GENERATED_RANDOM_MASK          (0x8000U)
#define QUIDDIKEY_HW_SETTINGS_DIS_GENERATED_RANDOM_SHIFT         (15U)
#define QUIDDIKEY_HW_SETTINGS_DIS_GENERATED_RANDOM(x)            (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_SETTINGS_DIS_GENERATED_RANDOM_SHIFT)) & QUIDDIKEY_HW_SETTINGS_DIS_GENERATED_RANDOM_MASK)

/*! @name HW INFO - QUIDDIKEY  register */
#define QUIDDIKEY_HW_INFO_CONFIG_BIST_MASK         (0x400000U)
#define QUIDDIKEY_HW_INFO_CONFIG_BIST_SHIFT        (22U)
#define QUIDDIKEY_HW_INFO_CONFIG_BIST(x)           (((uint32_t)(((uint32_t)(x))  << QUIDDIKEY_HW_INFO_CONFIG_BIST_SHIFT)) & QUIDDIKEY_HW_INFO_CONFIG_BIST_MASK)
#define QUIDDIKEY_HW_INFO_CONFIG_TYPE_MASK         (0xF0000000U)
#define QUIDDIKEY_HW_INFO_CONFIG_TYPE_SHIFT        (28U)
#define QUIDDIKEY_HW_INFO_CONFIG_TYPE(x)           (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_INFO_CONFIG_TYPE_SHIFT)) & QUIDDIKEY_HW_INFO_CONFIG_TYPE_MASK)

/*! @name HW ID - QUIDDIKEY  register */

/*! @name HW_VER - QUIDDIKEY  register */
#define QUIDDIKEY_HW_VER_REV_MASK                 (0xFFU)
#define QUIDDIKEY_HW_VER_REV_SHIFT                (0U)
#define QUIDDIKEY_HW_VER_REV(x)                   (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_HW_VER_REV_SHIFT */)) & QUIDDIKEY_HW_VER_REV_MASK)
#define QUIDDIKEY_HW_VER_MIN_MASK                 (0xFF00U)
#define QUIDDIKEY_HW_VER_MIN_SHIFT                (8U)
#define QUIDDIKEY_HW_VER_MIN(x)                   (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_VER_MIN_SHIFT)) & QUIDDIKEY_HW_VER_MIN_MASK)
#define QUIDDIKEY_HW_VER_MAJ_MASK                 (0xFF0000U)
#define QUIDDIKEY_HW_VER_MAJ_SHIFT                (16U)
#define QUIDDIKEY_HW_VER_MAJ(x)                   (((uint32_t)(((uint32_t)(x)) << QUIDDIKEY_HW_VER_MAJ_SHIFT)) & QUIDDIKEY_HW_VER_MAJ_MASK)

/*! @name SW_RESET_EN - QUIDDIKEY  register */
#define QUIDDIKEY_SW_RESET_EN_MASK                (0x1U)
#define QUIDDIKEY_SW_RESET_EN_SHIFT               (0U)
#define QUIDDIKEY_SW_RESET_EN(x)                  (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_SW_RESET_EN_SHIFT */)) & QUIDDIKEY_SW_RESET_EN_MASK)

/*!
 * @}
 */ /* end of group QUIDDIKEY_Register_Masks */


/* QUIDDIKEY - Peripheral instance base addresses */
/** Peripheral QUIDDIKEY base address */
#define QUIDDIKEY_BASE                                (SOC_PERI_BASE + 0x21000u)
/** Peripheral QUIDDIKEY base pointer */
#define QUIDDIKEY                                     ((QUIDDIKEY_Type *)QUIDDIKEY_BASE)
/** Array initializer of QUIDDIKEY base addresses */
#define QUIDDIKEY_BASE_ADDRS                          { QUIDDIKEY_BASE }
/** Array initializer of QUIDDIKEY base pointers */
#define QUIDDIKEY_BASE_PTRS                           { QUIDDIKEY }

/*!
 * @}
 */ /* end of group QUIDDIKEY_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

/*!
 * @}
 */ /* end of group Peripheral_access_layer */


#endif  /* _PMSIS_COMMON_H_ */
