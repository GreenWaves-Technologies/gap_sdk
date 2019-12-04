/*
** ###################################################################
**     Processors:          GAP9
**
**     Compilers:           GNU C Compiler
**
**     Reference manual:    riscv-spec-v2.1, January 2017
**     Version:             rev. 2.9, 2017-07-19
**
**     Abstract:
**         CMSIS Peripheral Access Layer for GAP9
**
**     Copyright (c) 2015 - 2018 GreenWave Technologies, Inc.
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
 * @file GAP9.h
 * @version 1.0
 * @date 2018-04-2
 * @brief CMSIS Peripheral Access Layer for GAP9
 *
 * CMSIS Peripheral Access Layer for GAP9
 */

#ifndef _GAP9_H_
#define _GAP9_H_                              /**< Symbol preventing repeated inclusion */
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
  CLUSTER_NOTIFY_FC_IRQn       = 4,                /**< Software event interrupt */
  PendSV_IRQn                  = 7,                /**< Software event U -> M PendSV interrupt */

  /* Device specific interrupts */
  DMA_EVT_IRQn                 = 8,                /**< DMA event interrupt */
  DMA_IRQn                     = 9,                /**< DMA interrupt */
  FC_TIMER0_IRQn               = 10,               /**< FC timer0 event interrupt */
  SysTick_IRQn                 = 10,               /**< GAP9 U -> M System Tick Interrupt */
  FC_TIMER1_IRQn               = 11,               /**< FC timer1 interrupt */

  EU_HWCE_EVENT                = 15,              /**< GAP9 HWCE SW Event */
  EU_HW_BARRIER_EVENT          = 16,              /**< GAP9 Hardware Barrier SW Event */
  EU_MUTEX_EVENT               = 17,              /**< GAP9 Mutex SW Event */
  EU_DISPATCH_EVENT            = 18,              /**< GAP9 Dispatch SW Event */
  EU_LOOP_EVENT                = 19,              /**< GAP9 Loop SW Event */

  PICL_OK_IRQn                 = 24,              /**< GAP9 Interrupt */
  SCU_OK_IRQn                  = 25,              /**< GAP9 Interrupt */

  /* Fault interrupts */
  FC_SOC_EVENT_IRQn            = 26,              /**< GAP9 SoC Event Interrupt */
  FC_SOC_QUIDDIKEY_IRQn        = 27,              /**< GAP9 QuiddiKey Interrupt */
  MPU_ERROR_IRQn               = 28,              /**< GAP9 MPU Error Interrupt */
  ERR_EVENT_IRQn               = 29,              /**< GAP9 Event Error Interrupt */

  /* Core interrupts */
  Rst_handler_IRQn             = 32,              /**< GAP9 Reset handler Interrupt */
  Ill_ins_IRQn                 = 33,              /**< GAP9 Usage Fault Interrupt */
  SVCall_IRQn                  = 34               /**< GAP9 SV Call Interrupt */
} IRQn_Type;

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers */


/* ----------------------------------------------------------------------------
   -- GAP9 Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GAP9_Core_Configuration GAP9 Core Configuration
 * @{
 */

#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               0         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */
#define __FPU_PRESENT                  0         /**< Defines if an FPU is present or not */

#include "core_gap.h"              /* Core Peripheral Access Layer */
#include "core_gap_memory_define.h"       /* Core Memory regions definitions */

#ifdef FEATURE_CLUSTER
#include "core_gap_cluster.h"              /* Cluster Access Layer */
#endif

#include "system_GAP9.h"            /* Device specific configuration file */

/*!
 * @}
 */ /* end of group GAP9_Core_Configuration */


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
#define FLL_CTRL0_BASE                               (SOC_PERI_BASE)
/** Peripheral FLL_CTRL0 base pointer */
#define FLL_CTRL0                                    ((FLL_CTRL_Type *)FLL_CTRL0_BASE)
/** Peripheral FLL_CTRL1 base address */
#define FLL_CTRL1_BASE                               (SOC_PERI_BASE + 0x10U)
/** Peripheral FLL_CTRL1 base pointer */
#define FLL_CTRL1                                    ((FLL_CTRL_Type *)FLL_CTRL1_BASE)
/** Peripheral FLL_CTRL2 base address */
#define FLL_CTRL2_BASE                               (SOC_PERI_BASE + 0x20U)
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
   -- FLL_LOCK Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLL_LOCK_Peripheral_Access_Layer FLL_LOCK Peripheral Access Layer
 * @{
 */

/** FLL_LOCK - Registers Layout Typedef */
typedef struct {
    __IO  uint32_t STATUS;         /**< FLL_LOCK status register, offset: 0x00 */
} FLL_LOCK_Type;

/* ----------------------------------------------------------------------------
   -- FLL_LOCK Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FLL_LOCK_Register_Masks FLL_LOCK Register Masks
 * @{
 */
/*! @name STATUS - FLL_LOCK status register */
#define FLL_LOCK_STATUS_FLL0_MASK                   (0x1U)
#define FLL_LOCK_STATUS_FLL0_SHIFT                  (0U)
#define FLL_LOCK_STATUS_FLL0(x)                     (((uint32_t)(((uint32_t)(x)) /* << FLL_LOCK_STATUS_FLL0_SHIFT */)) & FLL_LOCK_STATUS_FLL0_MASK)
#define FLL_LOCK_STATUS_FLL1_MASK                   (0x2U)
#define FLL_LOCK_STATUS_FLL1_SHIFT                  (1U)
#define FLL_LOCK_STATUS_FLL1(x)                     (((uint32_t)(((uint32_t)(x)) << FLL_LOCK_STATUS_FLL1_SHIFT)) & FLL_LOCK_STATUS_FLL1_MASK)
#define FLL_LOCK_STATUS_FLL2_MASK                   (0x4U)
#define FLL_LOCK_STATUS_FLL2_SHIFT                  (2U)
#define FLL_LOCK_STATUS_FLL2(x)                     (((uint32_t)(((uint32_t)(x)) << FLL_LOCK_STATUS_FLL2_SHIFT)) & FLL_LOCK_STATUS_FLL2_MASK)

/*!
 * @}
 */ /* end of group FLL_LOCK_Register_Masks */


/* FLL_LOCK - Peripheral instance base addresses */
/** Peripheral FLL_LOCK base address */
#define FLL_LOCK_BASE                               (SOC_PERI_BASE + 0x3C)
/** Peripheral FLL_LOCK base pointer */
#define FLL_LOCK                                    ((FLL_LOCK_Type *)FLL_LOCK_BASE)
/** Array initializer of FLL_LOCK base addresses */
#define FLL_LOCK_BASE_ADDRS                         { FLL_LOCK_BASE }
  /** Array initializer of FLL_LOCK base pointers */
#define FLL_LOCK_BASE_PTRS                          { FLL_LOCK }

/*!
 * @}
 */ /* end of group FLL_LOCK_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- BBGEN_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup BBGEN_CTRL_Peripheral_Access_Layer BBGEN_CTRL Peripheral Access Layer
 * @{
 */

/** BBGEN_CTRL - Registers Layout Typedef */
typedef struct {
    __IO  uint32_t CLUSTER_CONF;         /**< BBGEN_CTRL Cluster Configuration register, offset: 0x00 */
    __IO  uint32_t SOC_CONF;             /**< BBGEN_CTRL SOC Configuration register, offset: 0x04 */
    __IO  uint32_t LOCK_STATUS;          /**< BBGEN_CTRL Lock status register, offset: 0x08 */

} BBGEN_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- BBGEN_CTRL Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup BBGEN_CTRL_Register_Masks BBGEN_CTRL Register Masks
 * @{
 */
/*! @name CLUSTER_CONF SOC_CONF - BBGEN_CTRL configuration register */
/* negative bias voltage setpoint */
#define BBGEN_CTRL_CONF_NBVS_MASK                   (0x3FU)
#define BBGEN_CTRL_CONF_NBVS_SHIFT                  (0U)
#define BBGEN_CTRL_CONF_NBVS(x)                     (((uint32_t)(((uint32_t)(x)) /* << BBGEN_CTRL_CONF_NBVS_SHIFT */)) & BBGEN_CTRL_CONF_NBVS_MASK)
/* positive bias voltage setpoint */
#define BBGEN_CTRL_CONF_PBVS_MASK                   (0x3F00U)
#define BBGEN_CTRL_CONF_PBVS_SHIFT                  (8U)
#define BBGEN_CTRL_CONF_PBVS(x)                     (((uint32_t)(((uint32_t)(x)) << BBGEN_CTRL_CONF_PBVS_SHIFT)) & BBGEN_CTRL_CONF_PBVS_MASK)
/* reference buffer enable */
#define BBGEN_CTRL_CONF_REF_BUFFER_ENABLE_MASK      (0x10000000)
#define BBGEN_CTRL_CONF_REF_BUFFER_ENABLE_SHIFT     (28U)
#define BBGEN_CTRL_CONF_REF_BUFFER_ENABLE(x)        (((uint32_t)(((uint32_t)(x)) << BBGEN_CTRL_CONF_REF_BUFFER_ENABLE_SHIFT)) & BBGEN_CTRL_CONF_REF_BUFFER_ENABLE_MASK)
/* reference buffer bypass */
#define BBGEN_CTRL_CONF_REF_BUFFER_BYPASS_MASK      (0x20000000U)
#define BBGEN_CTRL_CONF_REF_BUFFER_BYPASS_SHIFT     (29U)
#define BBGEN_CTRL_CONF_REF_BUFFER_BYPASS(x)        (((uint32_t)(((uint32_t)(x)) << BBGEN_CTRL_CONF_REF_BUFFER_BYPASS_SHIFT)) & BBGEN_CTRL_CONF_REF_BUFFER_BYPASS_MASK)
#define BBGEN_CTRL_CONF_IDLE_MODE_MASK              (0x80000000U)
#define BBGEN_CTRL_CONF_IDLE_MODE_SHIFT             (31U)
#define BBGEN_CTRL_CONF_IDLE_MODE(x)                (((uint32_t)(((uint32_t)(x)) << BBGEN_CTRL_CONF_IDLE_MODE_SHIFT)) & BBGEN_CTRL_CONF_IDLE_MODE_MASK)

/*! @name LOCK_STATUS - BBGEN_CTRL status register */
#define BBGEN_CTRL_LOCK_STATUS_SOC_MASK             (0x3U)
#define BBGEN_CTRL_LOCK_STATUS_SOC_SHIFT            (0U)
#define BBGEN_CTRL_LOCK_STATUS_SOC(x)               (((uint32_t)(((uint32_t)(x)) /* << BBGEN_CTRL_LOCK_STATUS_SOC_SHIFT */)) & BBGEN_CTRL_LOCK_STATUS_SOC_MASK)
#define READ_BBGEN_CTRL_LOCK_STATUS_SOC(x)          (((uint32_t)(((uint32_t)(x)) & BBGEN_CTRL_LOCK_STATUS_SOC_MASK)) /*>> BBGEN_CTRL_LOCK_STATUS_SOC_SHIFT*/)
#define BBGEN_CTRL_LOCK_STATUS_CLUSTER_MASK         (0xCU)
#define BBGEN_CTRL_LOCK_STATUS_CLUSTER_SHIFT        (2U)
#define BBGEN_CTRL_LOCK_STATUS_CLUSTER(x)           (((uint32_t)(((uint32_t)(x)) << BBGEN_CTRL_LOCK_STATUS_CLUSTER_SHIFT)) & BBGEN_CTRL_LOCK_STATUS_CLUSTER_MASK)
#define READ_BBGEN_CTRL_LOCK_STATUS_CLUSTER(x)      (((uint32_t)(((uint32_t)(x)) & BBGEN_CTRL_LOCK_STATUS_CLUSTER_MASK)) >> BBGEN_CTRL_LOCK_STATUS_CLUSTER_SHIFT)


/*!
 * @}
 */ /* end of group BBGEN_CTRL_Register_Masks */


/* BBGEN_CTRL - Peripheral instance base addresses */
/** Peripheral BBGEN_CTRL base address */
#define BBGEN_CTRL_BASE                               (SOC_PERI_BASE + 0x40)
/** Peripheral BBGEN_CTRL base pointer */
#define BBGEN_CTRL                                    ((BBGEN_CTRL_Type *)BBGEN_CTRL_BASE)
/** Array initializer of BBGEN_CTRL base addresses */
#define BBGEN_CTRL_BASE_ADDRS                         { BBGEN_CTRL_BASE }
  /** Array initializer of BBGEN_CTRL base pointers */
#define BBGEN_CTRL_BASE_PTRS                          { BBGEN_CTRL }

/*!
 * @}
 */ /* end of group BBGEN_CTRL_Peripheral_Access_Layer */




/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 * @{
 */

/** GPIO - Register Layout Typedef */
typedef struct {
  __IO  uint32_t DIR;                          /**< GPIO gpio direction register, offset: 0x00 */
  __IO  uint32_t EN;                           /**< GPIO gpio enable register, offset: 0x04 */
  __IO  uint32_t IN;                           /**< GPIO gpio in register, offset: 0x8 */
  __IO  uint32_t OUT;                          /**< GPIO gpio out register, offset: 0xC */
  __IO  uint32_t OUT_SET;                      /**< GPIO gpio out set register, offset: 0x10 */
  __IO  uint32_t OUT_CLR;                      /**< GPIO gpio out clear register, offset: 0x14 */
  __IO  uint32_t INTEN;                        /**< GPIO gpio inten register, offset: 0x18 */
  __IO  uint32_t INTCFG[2];                    /**< GPIO gpio int configuration register 0, offset: 0x1C */
  __IO  uint32_t INTSTATUS;                    /**< GPIO gpio int status register, offset: 0x24 */
  __IO  uint32_t PADCFG[4];                    /**< GPIO pad configuration registers, offset: 0x28 */
} GPIO_Type;

/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */
#define GPIO_NUM                                  64

/*! @name INTCFG - GPIO pad interrupt configuration registers */
#define GPIO_INTCFG_TYPE_MASK                     (0x3U)
#define GPIO_INTCFG_TYPE_SHIFT                    (0U)
#define GPIO_INTCFG_TYPE(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_INTCFG_TYPE_SHIFT)) & GPIO_INTCFG_TYPE_MASK)

/*! @name PADCFG - GPIO pad configuration registers */
#define GPIO_PADCFG_PULLDOWN_EN_MASK              (0x1U)
#define GPIO_PADCFG_PULLDOWN_EN_SHIFT             (0U)
#define GPIO_PADCFG_PULLDOWN_EN(x)                (((uint32_t)(((uint32_t)(x)) /* << GPIO_PADCFG_PULLDOWN_EN_SHIFT */)) & GPIO_PADCFG_PULLDOWN_EN_MASK)
#define GPIO_PADCFG_PULLUP_EN_MASK                (0x2U)
#define GPIO_PADCFG_PULLUP_EN_SHIFT               (1U)
#define GPIO_PADCFG_PULLUP_EN(x)                  (((uint32_t)(((uint32_t)(x)) << GPIO_PADCFG_PULLUP_EN_SHIFT)) & GPIO_PADCFG_PULLUP_EN_MASK)
#define GPIO_PADCFG_DRIVE_STRENGTH_MASK           (0xCU)
#define GPIO_PADCFG_DRIVE_STRENGTH_SHIFT          (2U)
#define GPIO_PADCFG_DRIVE_STRENGTH(x)             (((uint32_t)(((uint32_t)(x)) << GPIO_PADCFG_DRIVE_STRENGTH_SHIFT)) & GPIO_PADCFG_DRIVE_STRENGTH_MASK)

#define GPIO_INTCFG_TYPE_BITS_WIDTH_MASK          (0x3U)


/*!
 * @}
 */ /* end of group GPIO_Register_Masks */


/* GPIO - Peripheral instance base addresses */
/** Peripheral GPIOA base address */
#define GPIOA_BASE                              (SOC_PERI_BASE + 0x1000u)
/** Peripheral GPIOA base pointer */
#define GPIOA                                   ((GPIO_Type *)GPIOA_BASE)
/** Peripheral GPIOB base address */
#define GPIOB_BASE                              (GPIOA_BASE + 0x38)
/** Peripheral GPIOB base pointer */
#define GPIOB                                   ((GPIO_Type *)GPIOB_BASE)
/** Array initializer of GPIO base addresses */
#define GPIO_BASE_ADDRS                         { GPIOA_BASE, GPIOB_BASE }
/** Array initializer of GPIO base pointers */
#define GPIO_BASE_PTRS                          { GPIOA, GPIOB }

/*!
 * @}
 */ /* end of group GPIO_Peripheral_Access_Layer */




/* ----------------------------------------------------------------------------
   -- UDMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDMA_Peripheral_Access_Layer  UDMA Peripheral Access Layer
 * @{
 */

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

typedef struct {
  UDMA_Type    UDMA_SPIM;                           /**< SPIM UDMA general register, offset: 0x00 */
  __IO uint32_t CMD_SADDR;                          /**< CMD UDMA buffer transfer address register, offset: 0x20 */
  __IO uint32_t CMD_SIZE;                           /**< CMD UDMA buffer transfer size register, offset: 0x24 */
  __IO uint32_t CMD_CFG;                            /**< CMD UDMA transfer configuration register, offset: 0x28 */
  __IO uint32_t CMD_INITCFG;                        /**< Reserved, offset: 0x2C */

} UDMA_CMD_Type;

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

/*! @name RX_SIZE - RX TX UDMA buffer transfer size register */
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
#define UDMA_CFG_EN_MASK                     (0x10U)
#define UDMA_CFG_EN_SHIFT                    (4U)
#define UDMA_CFG_EN(x)                       (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_EN_SHIFT)) & UDMA_CFG_EN_MASK)
#define UDMA_CFG_PENDING_MASK                (0x20U)
#define UDMA_CFG_PENDING_SHIFT               (5U)
#define UDMA_CFG_PENDING(x)                  (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_PENDING_SHIFT)) & UDMA_CFG_PENDING_MASK)
#define UDMA_CFG_CLR_MASK                    (0x40U)
#define UDMA_CFG_CLR_SHIFT                   (6U)
#define UDMA_CFG_CLR(x)                      (((uint32_t)(((uint32_t)(x)) << UDMA_CFG_CLR_SHIFT)) & UDMA_CFG_CLR_MASK)

/*! @brief UDMA Peripheral ID List */
#define    PER_ID_SPIM0      0
#define    PER_ID_SPIM1      1
#define    PER_ID_SPIM2      2
#define    PER_ID_SPIM3      3
#define    PER_ID_UART0      4
#define    PER_ID_UART1      5
#define    PER_ID_UART2      6
#define    PER_ID_I2C0       7
#define    PER_ID_HYPERBUS   8
#define    PER_ID_JTAG       9
#define    PER_ID_SDIO       10
#define    PER_ID_I2S        11
#define    PER_ID_CPI        12
#define    PER_ID_CSI2       13
#define    PER_ID_MRAM       14
#define    PER_ID_FILTER     15
#define    PER_ID_TIMESTAMP  16

/*!
 * @}
 */ /* end of group UDMA_Register_Masks */


/* UDMA - Peripheral instance base addresses */
/** Peripheral UDMA base address 0x1A102000 */
#define UDMA_BASE                                (SOC_PERI_BASE + 0x02000u + 0x80u)
/** Peripheral UDMA events number */
#define UDMA_EVENTS_NUM                          128
/** Peripheral UDMA channel number */
#define UDMA_CHANNEL_NUM                         17

/*!
 * @}
 */ /* end of group UDMA_Peripheral_Access_Layer */




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

/*!
 * @}
 */ /* end of group UDMA_GC_Register_Masks */


/* UDMA Global configuration - instance base addresses */
/** Global configuration UDMA base address */
#define UDMA_GC_BASE                              (SOC_PERI_BASE + 0x02000u)
#define UDMA_GC                                   ((UDMA_GC_Type *)UDMA_GC_BASE)

/*!
 * @}
 */ /* end of group UDMA_GC_Peripheral_Access_Layer */

/* ----------------------------------------------------------------------------
   -- SPIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPIM_Peripheral_Access_Layer SPIM Peripheral Access Layer
 * @{
 */

/** SPIM - Register Layout Typedef */
typedef struct {
  UDMA_CMD_Type  UDMA_SPIM;                         /**< SPIM UDMA general register, offset: 0x00 */
  __I   uint32_t STATUS;                            /**< SPIM status register, offset: 0x30 */
} SPIM_Type;

/* ----------------------------------------------------------------------------
   -- SPIM Register Masks
   ---------------------------------------------------------------------------- */

/*! @name uDMA - SPIM uDMA Number */
#define SPIM_NUMBER                   4

/*! @name uDMA - SPIM uDMA control CMD */
#define SPIM_CMD_CFG_ID               0
#define SPIM_CMD_SOT_ID               1
#define SPIM_CMD_SEND_CMD_ID          2

#define SPIM_CMD_DUMMY_ID             4
#define SPIM_CMD_WAIT_ID              5
#define SPIM_CMD_TX_DATA_ID           6
#define SPIM_CMD_RX_DATA_ID           7
#define SPIM_CMD_RPT_ID               8
#define SPIM_CMD_EOT_ID               9
#define SPIM_CMD_RPT_END_ID          10
#define SPIM_CMD_RX_CHECK_ID         11
#define SPIM_CMD_DUPLEX_ID           12
#define SPIM_CMD_SETUP_UCA_ID        13
#define SPIM_CMD_SETUP_UCS_ID        14

#define SPIM_CMD_MASK                 (0xF0000000U)
#define SPIM_CMD_SHIFT                (28U)
#define SPIM_CMD(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SHIFT)) & SPIM_CMD_MASK)

#define SPI_4_WORD_FORMAT   2
#define SPI_2_WORD_FORMAT   1
#define SPI_1_WORD_FORMAT   0

/*!
 * @addtogroup SPIM_Register_Masks SPIM Register Masks
 * @{
 */
/*! @name CMD_CFG - SPIM configuration register */
#define SPIM_CMD_CFG_CLKDIV_MASK                  (0xFFU)
#define SPIM_CMD_CFG_CLKDIV_SHIFT                 (0U)
#define SPIM_CMD_CFG_CLKDIV(x)                    (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_CFG_CLKDIV_SHIFT*/)) & SPIM_CMD_CFG_CLKDIV_MASK)
#define SPIM_CMD_CFG_CPHA_MASK                    (0x100U)
#define SPIM_CMD_CFG_CPHA_SHIFT                   (8U)
#define SPIM_CMD_CFG_CPHA(x)                      (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_CFG_CPHA_SHIFT)) & SPIM_CMD_CFG_CPHA_MASK)
#define SPIM_CMD_CFG_CPOL_MASK                    (0x200U)
#define SPIM_CMD_CFG_CPOL_SHIFT                   (9U)
#define SPIM_CMD_CFG_CPOL(x)                      (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_CFG_CPOL_SHIFT)) & SPIM_CMD_CFG_CPOL_MASK)

/*! @name CMD_SOT - SPIM chip select (CS) */
#define SPIM_CMD_SOT_CS_MASK                      (0x3U)
#define SPIM_CMD_SOT_CS_SHIFT                     (0U)
#define SPIM_CMD_SOT_CS(x)                        (((uint32_t)(((uint32_t)(x)) /* << SPIM_CMD_SOT_CS_SHIFT */)) & SPIM_CMD_SOT_CS_MASK)
#define SPIM_CMD_SOT_CS_WAIT_CYCLE_MASK           (0xFF00U)
#define SPIM_CMD_SOT_CS_WAIT_CYCLE_SHIFT          (8U)
#define SPIM_CMD_SOT_CS_WAIT_CYCLE(x)             (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SOT_CS_WAIT_CYCLE_SHIFT)) & SPIM_CMD_SOT_CS_WAIT_CYCLE_MASK)

/*! @name CMD_SEND_CMD - SPIM Transmit a command */
#define SPIM_CMD_SEND_SIZE_LONG_MASK              (0xFFFFU)
#define SPIM_CMD_SEND_SIZE_LONG_SHIFT             (0U)
#define SPIM_CMD_SEND_SIZE_LONG(x)                (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_SEND_SIZE_LONG_SHIFT*/)) & SPIM_CMD_SEND_SIZE_LONG_MASK)
#define SPIM_CMD_SEND_WORD_SIZE_MASK              (0x1F0000U)
#define SPIM_CMD_SEND_WORD_SIZE_SHIFT             (16U)
#define SPIM_CMD_SEND_WORD_SIZE(x)                (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SEND_WORD_SIZE_SHIFT)) & SPIM_CMD_SEND_WORD_SIZE_MASK)
#define SPIM_CMD_SEND_WORD_FORMAT_MASK            (0x600000U)
#define SPIM_CMD_SEND_WORD_FORMAT_SHIFT           (21U)
#define SPIM_CMD_SEND_WORD_FORMAT(x)              (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SEND_WORD_FORMAT_SHIFT)) & SPIM_CMD_SEND_WORD_FORMAT_MASK)
#define SPIM_CMD_SEND_LSB_MASK                    (0x4000000U)
#define SPIM_CMD_SEND_LSB_SHIFT                   (26U)
#define SPIM_CMD_SEND_LSB(x)                      (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SEND_LSB_SHIFT)) & SPIM_CMD_SEND_LSB_MASK)
#define SPIM_CMD_SEND_QPI_MASK                    (0x8000000U)
#define SPIM_CMD_SEND_QPI_SHIFT                   (27U)
#define SPIM_CMD_SEND_QPI(x)                      (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_SEND_QPI_SHIFT)) & SPIM_CMD_SEND_QPI_MASK)

/*! @name CMD_DUMMY - SPIM number of dummy cycle */
#define SPIM_CMD_DUMMY_CYCLE_MASK                 (0x1F0000U)
#define SPIM_CMD_DUMMY_CYCLE_SHIFT                (16U)
#define SPIM_CMD_DUMMY_CYCLE(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_DUMMY_CYCLE_SHIFT)) & SPIM_CMD_DUMMY_CYCLE_MASK)

/*! @name CMD_WAIT - SPIM wait in which event - 2 bits */
#define SPIM_CMD_WAIT_EVENT_ID_MASK               (0x3U)
#define SPIM_CMD_WAIT_EVENT_ID_SHIFT              (0U)
#define SPIM_CMD_WAIT_EVENT_ID(x)                 (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_WAIT_EVENT_ID_SHIFT*/)) & SPIM_CMD_WAIT_EVENT_ID_MASK)

/*! @name CMD_TX_DATA - SPIM send datas */
#define SPIM_CMD_TX_DATA_SIZE_MASK                (0xFFFFU)
#define SPIM_CMD_TX_DATA_SIZE_SHIFT               (0U)
#define SPIM_CMD_TX_DATA_SIZE(x)                  (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_TX_DATA_SIZE_SHIFT*/)) & SPIM_CMD_TX_DATA_SIZE_MASK)
#define SPIM_CMD_TX_DATA_WORD_SIZE_MASK           (0x1F0000U)
#define SPIM_CMD_TX_DATA_WORD_SIZE_SHIFT          (16U)
#define SPIM_CMD_TX_DATA_WORD_SIZE(x)             (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_TX_DATA_WORD_SIZE_SHIFT)) & SPIM_CMD_TX_DATA_WORD_SIZE_MASK)
#define SPIM_CMD_TX_DATA_WORD_FORMAT_MASK         (0x600000U)
#define SPIM_CMD_TX_DATA_WORD_FORMAT_SHIFT        (21U)
#define SPIM_CMD_TX_DATA_WORD_FORMAT(x)           (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_TX_DATA_WORD_FORMAT_SHIFT)) & SPIM_CMD_TX_DATA_WORD_FORMAT_MASK)
#define SPIM_CMD_TX_DATA_LSB_MASK                 (0x4000000U)
#define SPIM_CMD_TX_DATA_LSB_SHIFT                (26U)
#define SPIM_CMD_TX_DATA_LSB(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_TX_DATA_LSB_SHIFT)) & SPIM_CMD_TX_DATA_LSB_MASK)
#define SPIM_CMD_TX_DATA_QPI_MASK                 (0x8000000U)
#define SPIM_CMD_TX_DATA_QPI_SHIFT                (27U)
#define SPIM_CMD_TX_DATA_QPI(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_TX_DATA_QPI_SHIFT)) & SPIM_CMD_TX_DATA_QPI_MASK)

/*! @name CMD_RX_DATA - SPIM receive datas */
#define SPIM_CMD_RX_DATA_SIZE_MASK                (0xFFFFU)
#define SPIM_CMD_RX_DATA_SIZE_SHIFT               (0U)
#define SPIM_CMD_RX_DATA_SIZE(x)                  (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_RX_DATA_SIZE_SHIFT*/)) & SPIM_CMD_RX_DATA_SIZE_MASK)
#define SPIM_CMD_RX_DATA_WORD_SIZE_MASK           (0x1F0000U)
#define SPIM_CMD_RX_DATA_WORD_SIZE_SHIFT          (16U)
#define SPIM_CMD_RX_DATA_WORD_SIZE(x)             (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_DATA_WORD_SIZE_SHIFT)) & SPIM_CMD_RX_DATA_WORD_SIZE_MASK)
#define SPIM_CMD_RX_DATA_WORD_FORMAT_MASK         (0x600000U)
#define SPIM_CMD_RX_DATA_WORD_FORMAT_SHIFT        (21U)
#define SPIM_CMD_RX_DATA_WORD_FORMAT(x)           (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_DATA_WORD_FORMAT_SHIFT)) & SPIM_CMD_RX_DATA_WORD_FORMAT_MASK)
#define SPIM_CMD_RX_DATA_LSB_MASK                 (0x4000000U)
#define SPIM_CMD_RX_DATA_LSB_SHIFT                (26U)
#define SPIM_CMD_RX_DATA_LSB(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_DATA_LSB_SHIFT)) & SPIM_CMD_RX_DATA_LSB_MASK)
#define SPIM_CMD_RX_DATA_QPI_MASK                 (0x8000000U)
#define SPIM_CMD_RX_DATA_QPI_SHIFT                (27U)
#define SPIM_CMD_RX_DATA_QPI(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_DATA_QPI_SHIFT)) & SPIM_CMD_RX_DATA_QPI_MASK)

/*! @name CMD_RPT - SPIM repeat the next transfer N times */
#define SPIM_CMD_RPT_CNT_MASK                     (0xFFFFU)
#define SPIM_CMD_RPT_CNT_SHIFT                    (0U)
#define SPIM_CMD_RPT_CNT(x)                       (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_RPT_CNT_SHIFT*/)) & SPIM_CMD_RPT_CNT_MASK)

/*! @name CMD_EOT - SPIM clears the chip select (CS), and send a end event or not  */
#define SPIM_CMD_EOT_EVENT_GEN_MASK               (0x1U)
#define SPIM_CMD_EOT_EVENT_GEN_SHIFT              (0U)
#define SPIM_CMD_EOT_EVENT_GEN(x)                 (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_EOT_EVENT_GEN_SHIFT*/)) & SPIM_CMD_EOT_EVENT_GEN_MASK)
#define SPIM_CMD_EOT_KEEP_CS_MASK                 (0x2U)
#define SPIM_CMD_EOT_KEEP_CS_SHIFT                (1U)
#define SPIM_CMD_EOT_KEEP_CS(x)                   (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_EOT_KEEP_CS_SHIFT)) & SPIM_CMD_EOT_KEEP_CS_MASK)

/*! @name CMD_RPT_END - SPIM End of the repeat loop */
#define SPIM_CMD_RPT_END_SPI_CMD_MASK             (0xFU)
#define SPIM_CMD_RPT_END_SPI_CMD_SHIFT            (0U)
#define SPIM_CMD_RPT_END_SPI_CMD(x)               (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_RPT_END_SPI_CMD_SHIFT*/)) & SPIM_CMD_RPT_END_SPI_CMD_MASK)

/*! @name CMD_RX_CHECK - SPIM check up to 16 bits of data against an expected value  */
#define SPIM_CMD_RX_CHECK_COMP_DATA_MASK          (0xFFFFU)
#define SPIM_CMD_RX_CHECK_COMP_DATA_SHIFT         (0U)
#define SPIM_CMD_RX_CHECK_COMP_DATA(x)            (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_RX_CHECK_COMP_DATA_SHIFT*/)) & SPIM_CMD_RX_CHECK_COMP_DATA_MASK)
/* The number of bits to check, maximum is 16bits */
#define SPIM_CMD_RX_CHECK_STATUS_SIZE_MASK        (0xFF0000U)
#define SPIM_CMD_RX_CHECK_STATUS_SIZE_SHIFT       (16U)
#define SPIM_CMD_RX_CHECK_STATUS_SIZE(x)          (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_CHECK_STATUS_SIZE_SHIFT)) & SPIM_CMD_RX_CHECK_STATUS_SIZE_MASK)
/* The type of checking, 0 - Equal; 1 - check the bits of one; 2 - check the bits of zero */
#define SPIM_CMD_RX_CHECK_CHECK_TYPE_MASK         (0x3000000U)
#define SPIM_CMD_RX_CHECK_CHECK_TYPE_SHIFT        (24U)
#define SPIM_CMD_RX_CHECK_CHECK_TYPE(x)           (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_CHECK_CHECK_TYPE_SHIFT)) & SPIM_CMD_RX_CHECK_CHECK_TYPE_MASK)
#define SPIM_CMD_RX_CHECK_LSB_MASK                (0x4000000U)
#define SPIM_CMD_RX_CHECK_LSB_SHIFT               (26U)
#define SPIM_CMD_RX_CHECK_LSB(x)                  (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_CHECK_LSB_SHIFT)) & SPIM_CMD_RX_CHECK_LSB_MASK)
/* The check bits trnsfered by QPI or not */
#define SPIM_CMD_RX_CHECK_QPI_MASK                (0x8000000U)
#define SPIM_CMD_RX_CHECK_QPI_SHIFT               (27U)
#define SPIM_CMD_RX_CHECK_QPI(x)                  (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_RX_CHECK_QPI_SHIFT)) & SPIM_CMD_RX_CHECK_QPI_MASK)

/*! @name CMD_FULL_DULP - SPIM Activate full duplex mode */
#define SPIM_CMD_FULL_SIZE_CMD_MASK               (0xFFFFU)
#define SPIM_CMD_FULL_SIZE_CMD_SHIFT              (0U)
#define SPIM_CMD_FULL_SIZE_CMD(x)                 (((uint32_t)(((uint32_t)(x)) /*<< SPIM_CMD_FULL_SIZE_CMD_SHIFT*/)) & SPIM_CMD_FULL_SIZE_CMD_MASK)
#define SPIM_CMD_FULL_LSB_CMD_MASK                (0x4000000U)
#define SPIM_CMD_FULL_LSB_CMD_SHIFT               (26U)
#define SPIM_CMD_FULL_LSB_CMD(x)                  (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_FULL_LSB_CMD_SHIFT)) & SPIM_CMD_FULL_LSB_CMD_MASK)

/*! @name CMD_UCA  */
#define SPIM_CMD_UCA_ADDR_MASK              (0x1FFFFFU)
#define SPIM_CMD_UCA_ADDR_SHIFT             (0U)
#define SPIM_CMD_UCA_ADDR(x)                (((uint32_t)(((uint32_t)(x)) /* << SPIM_CMD_UCA_ADDR_SHIFT */)) & SPIM_CMD_UCA_ADDR_MASK)
#define SPIM_CMD_UCA_DS_MASK                (0x6000000U)
#define SPIM_CMD_UCA_DS_SHIFT               (25U)
#define SPIM_CMD_UCA_DS(x)                  (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_UCA_DS_SHIFT)) & SPIM_CMD_UCA_DS_MASK)
#define SPIM_CMD_UCA_TXRXN_MASK             (0x8000000U)
#define SPIM_CMD_UCA_TXRXN_SHIFT            (27U)
#define SPIM_CMD_UCA_TXRXN(x)               (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_UCA_TXRXN_SHIFT)) & SPIM_CMD_UCA_TXRXN_MASK)

/*! @name CMD_UCS */
#define SPIM_CMD_UCS_SIZE_MASK              (0xFFFFFU)
#define SPIM_CMD_UCS_SIZE_SHIFT             (0U)
#define SPIM_CMD_UCS_SIZE(x)                (((uint32_t)(((uint32_t)(x)) /* << SPIM_CMD_UCS_SIZE_SHIFT */)) & SPIM_CMD_UCS_SIZE_MASK)
#define SPIM_CMD_UCS_DS_MASK                (0x6000000U)
#define SPIM_CMD_UCS_DS_SHIFT               (25U)
#define SPIM_CMD_UCS_DS(x)                  (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_UCS_DS_SHIFT)) & SPIM_CMD_UCS_DS_MASK)
#define SPIM_CMD_UCS_TXRXN_MASK             (0x8000000U)
#define SPIM_CMD_UCS_TXRXN_SHIFT            (27U)
#define SPIM_CMD_UCS_TXRXN(x)               (((uint32_t)(((uint32_t)(x)) << SPIM_CMD_UCS_TXRXN_SHIFT)) & SPIM_CMD_UCS_TXRXN_MASK)

/*! @name STATUS - SPIM status register */
#define SPIM_STATUS_POLLING_RESULT_MASK           (0x3U)
#define SPIM_STATUS_POLLING_RESULT_SHIFT          (0U)
#define SPIM_STATUS_POLLING_RESULT(x)             (((uint32_t)(((uint32_t)(x)) /*<< SPIM_STATUS_POLLING_RESULT_SHIFT*/)) & SPIM_STATUS_POLLING_RESULT_MASK)

#define SPIM_STATUS_POLLING_SUCCES           1
#define SPIM_STATUS_POLLING_FAIL             2

#define SPIM_CMD_CFG(clockDiv,cpol,cpha)   ( SPIM_CMD(SPIM_CMD_CFG_ID) \
                                           | SPIM_CMD_CFG_CLKDIV(clockDiv) \
                                           | SPIM_CMD_CFG_CPOL(cpol) \
                                           | SPIM_CMD_CFG_CPHA(cpha) )

#define SPIM_CMD_SOT(cs)                   ( SPIM_CMD(SPIM_CMD_SOT_ID) \
                                           | SPIM_CMD_SOT_CS(cs) )

#define SPIM_CMD_SEND_CMD(cmd,bits,qpi)    ( SPIM_CMD(SPIM_CMD_SEND_CMD_ID) \
                                           | SPIM_CMD_SEND_SIZE_LONG(cmd) \
                                           | SPIM_CMD_SEND_WORD_SIZE(bits - 1) \
                                           | SPIM_CMD_SEND_QPI(qpi) )

#define SPIM_CMD_SEND_BITS(addr,bits,qpi)  SPIM_CMD_SEND_CMD(addr,bits,qpi)

#ifndef __PLATFORM_GVSOC__
#define SPIM_CMD_DUMMY(cycles)             (cycles ? ( SPIM_CMD(SPIM_CMD_DUMMY_ID) \
                                           | SPIM_CMD_DUMMY_CYCLE(cycles - 1)) : 0xFFFF0000u)
#else
#define SPIM_CMD_DUMMY(cycles)             (cycles ? ( SPIM_CMD(SPIM_CMD_DUMMY_ID) \
                                           | SPIM_CMD_DUMMY_CYCLE(cycles - 1)) : 0x0u)
#endif

#define SPIM_CMD_TX_DATA_COMPLETE(words,word_format,word_bits,qpi,lsb_first)  ( SPIM_CMD(SPIM_CMD_TX_DATA_ID) \
                                           | SPIM_CMD_RX_DATA_SIZE((words - 1)) \
                                           | SPIM_CMD_TX_DATA_WORD_FORMAT(word_format) \
                                           | SPIM_CMD_TX_DATA_WORD_SIZE((word_bits - 1)) \
                                           | SPIM_CMD_TX_DATA_LSB(lsb_first) \
                                           | SPIM_CMD_TX_DATA_QPI(qpi) )

#define SPIM_CMD_RX_DATA_COMPLETE(words,word_format,word_bits,qpi,lsb_first)  ( SPIM_CMD(SPIM_CMD_RX_DATA_ID) \
                                           | SPIM_CMD_RX_DATA_SIZE((words - 1)) \
                                           | SPIM_CMD_RX_DATA_WORD_FORMAT(word_format) \
                                           | SPIM_CMD_RX_DATA_WORD_SIZE((word_bits - 1)) \
                                           | SPIM_CMD_RX_DATA_LSB(lsb_first) \
                                           | SPIM_CMD_RX_DATA_QPI(qpi) )

#define SPIM_CMD_TX_DATA(bits,qpi,byte_align) SPIM_CMD_TX_DATA_COMPLETE((bits >> 3), SPI_4_WORD_FORMAT, 8, qpi, byte_align)

#define SPIM_CMD_RX_DATA(bits,qpi,byte_align) SPIM_CMD_RX_DATA_COMPLETE((bits >> 3), SPI_4_WORD_FORMAT, 8, qpi, byte_align)



#define SPIM_CMD_RPT(iter)                 ( SPIM_CMD(SPIM_CMD_RPT_ID) \
                                           | SPIM_CMD_RPT_CNT(iter) )

#define SPIM_CMD_EOT_KEEP(keep_cs, evt)    ( SPIM_CMD(SPIM_CMD_EOT_ID) \
                                           | SPIM_CMD_EOT_KEEP_CS(keep_cs) \
                                           | SPIM_CMD_EOT_EVENT_GEN(evt) )

#define SPIM_CMD_EOT(evt)                  SPIM_CMD_EOT_KEEP(0, evt)

#define SPIM_CMD_WAIT(event)               ( SPIM_CMD(SPIM_CMD_WAIT_ID) \
                                           | SPIM_CMD_WAIT_EVENT_ID(event) )

#define SPIM_CMD_RPT_END()                 ( SPIM_CMD(SPIM_CMD_RPT_END_ID) )

#define SPIM_CMD_DUPLEX(bits,byte_align)   ( SPIM_CMD(SPIM_CMD_DUPLEX_ID) \
                                           | SPIM_CMD_FULL_SIZE_CMD((bits - 1 )) \
                                           | SPIM_CMD_FULL_LSB_CMD(byte_align) )

#define SPIM_CMD_RX_CHECK(mode,bits,value,qpi,byte_align) \
                                           ( SPIM_CMD(SPIM_CMD_RX_CHECK_ID) \
                                           | SPIM_CMD_RX_CHECK_COMP_DATA(value) \
                                           | SPIM_CMD_RX_CHECK_STATUS_SIZE((bits - 1)) \
                                           | SPIM_CMD_RX_CHECK_CHECK_TYPE(mode) \
                                           | SPIM_CMD_RX_CHECK_LSB(byte_align) \
                                           | SPIM_CMD_RX_CHECK_QPI(qpi) )

#define SPI_CMD_SETUP_UCA(txrxen,ds,addr) ( SPIM_CMD(SPIM_CMD_SETUP_UCA_ID) \
                                            | SPIM_CMD_UCA_TXRXN(txrxen) \
                                            | SPIM_CMD_UCA_DS(ds) \
                                            | SPIM_CMD_UCA_ADDR(addr) )

#define SPI_CMD_SETUP_UCS(txrxen,ds,size) ( SPIM_CMD(SPIM_CMD_SETUP_UCS_ID) \
                                            | SPIM_CMD_UCS_TXRXN(txrxen) \
                                            | SPIM_CMD_UCS_DS(ds) \
                                            | SPIM_CMD_UCS_SIZE(size >> 3) )

/*!
 * @}
 */ /* end of group SPIM_Register_Masks */


/* SPIM - Peripheral instance base addresses */
/** Peripheral SPIM0 base address */
#define SPIM0_BASE                                (UDMA_BASE + PER_ID_SPIM0 * 128U)
/** Peripheral SPIM0 base pointer */
#define SPIM0                                     ((SPIM_Type *)SPIM0_BASE)
/** Peripheral SPIM1 base address */
#define SPIM1_BASE                                (UDMA_BASE + PER_ID_SPIM1 * 128U)
/** Peripheral SPIM1 base pointer */
#define SPIM1                                     ((SPIM_Type *)SPIM1_BASE)
/** Peripheral SPIM2 base address */
#define SPIM2_BASE                                (UDMA_BASE + PER_ID_SPIM2 * 128U)
/** Peripheral SPIM2 base pointer */
#define SPIM2                                     ((SPIM_Type *)SPIM2_BASE)
/** Peripheral SPIM3 base address */
#define SPIM3_BASE                                (UDMA_BASE + PER_ID_SPIM3 * 128U)
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

/*!
 * @}
 */ /* end of group SPIM_Peripheral_Access_Layer */


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
  __I   uint32_t ERROR;                             /**< UART Error status register, offset: 0x28 */
  __IO  uint32_t IRQ_EN;                            /**< UART Interrupt Configuration register, offset: 0x2C */
  __I   uint32_t VALID;                             /**< UART polling read byte, offset: 0x30 */
  __I   uint32_t DATA;                              /**< UART read byte, offset: 0x34 */
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
#define UART_STATUS_TX_BUSY(x)                      (((uint32_t)(((uint32_t)(x)) /* << UART_STATUS_TX_BUSY_SHIFT */)) & UART_STATUS_TX_BUSY_MASK)
#define UART_STATUS_RX_BUSY_MASK                    (0x2U)
#define UART_STATUS_RX_BUSY_SHIFT                   (1U)
#define UART_STATUS_RX_BUSY(x)                      (((uint32_t)(((uint32_t)(x)) << UART_STATUS_RX_BUSY_SHIFT)) & UART_STATUS_RX_BUSY_MASK)
#define UART_STATUS_RX_PE_MASK                      (0x4U)
#define UART_STATUS_RX_PE_SHIFT                     (2U)
#define UART_STATUS_RX_PE(x)                        (((uint32_t)(((uint32_t)(x)) << UART_STATUS_RX_PE_SHIFT)) & UART_STATUS_RX_PE_MASK)

/*! @name SETUP - UART SETUP register */
#define UART_SETUP_PARITY_ENA_MASK                  (0x1U)
#define UART_SETUP_PARITY_ENA_SHIFT                 (0U)
#define UART_SETUP_PARITY_ENA(x)                    (((uint32_t)(((uint32_t)(x)) /* << UART_SETUP_PARITY_ENA_SHIFT */)) & UART_SETUP_PARITY_ENA_MASK)
#define UART_SETUP_BIT_LENGTH_MASK                  (0x6U)
#define UART_SETUP_BIT_LENGTH_SHIFT                 (1U)
#define UART_SETUP_BIT_LENGTH(x)                    (((uint32_t)(((uint32_t)(x)) << UART_SETUP_BIT_LENGTH_SHIFT)) & UART_SETUP_BIT_LENGTH_MASK)
#define UART_SETUP_STOP_BITS_MASK                   (0x8U)
#define UART_SETUP_STOP_BITS_SHIFT                  (3U)
#define UART_SETUP_STOP_BITS(x)                     (((uint32_t)(((uint32_t)(x)) << UART_SETUP_STOP_BITS_SHIFT)) & UART_SETUP_STOP_BITS_MASK)
#define UART_SETUP_RX_POLLNG_EN_MASK                (0x10U)
#define UART_SETUP_RX_POLLNG_EN_SHIFT               (4U)
#define UART_SETUP_RX_POLLNG_EN(x)                  (((uint32_t)(((uint32_t)(x)) << UART_SETUP_RX_POLLNG_EN_SHIFT)) & UART_SETUP_RX_POLLNG_EN_MASK)
#define UART_SETUP_RX_CLEAN_FIFO_MASK                (0x20U)
#define UART_SETUP_RX_CLEAN_FIFO_SHIFT               (5U)
#define UART_SETUP_RX_CLEAN_FIFO(x)                  (((uint32_t)(((uint32_t)(x)) << UART_SETUP_RX_CLEAN_FIFO_SHIFT)) & UART_SETUP_RX_CLEAN_FIFO_MASK)
#define UART_SETUP_TX_ENA_MASK                      (0x100U)
#define UART_SETUP_TX_ENA_SHIFT                     (8U)
#define UART_SETUP_TX_ENA(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_TX_ENA_SHIFT)) & UART_SETUP_TX_ENA_MASK)
#define UART_SETUP_RX_ENA_MASK                      (0x200U)
#define UART_SETUP_RX_ENA_SHIFT                     (9U)
#define UART_SETUP_RX_ENA(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_RX_ENA_SHIFT)) & UART_SETUP_RX_ENA_MASK)
#define UART_SETUP_CLKDIV_MASK                      (0xFFFF0000U)
#define UART_SETUP_CLKDIV_SHIFT                     (16U)
#define UART_SETUP_CLKDIV(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SETUP_CLKDIV_SHIFT)) & UART_SETUP_CLKDIV_MASK)

/*! @name ERROR - UART Error Status register */
#define UART_ERROR_OVERFLOW_MASK             (0x1U)
#define UART_ERROR_OVERFLOW_SHIFT            (0U)
#define UART_ERROR_OVERFLOW(x)               (((uint32_t)(((uint32_t)(x)) /* << UART_ERROR_OVERFLOW_SHIFT */)) & UART_ERROR_OVERFLOW_MASK)
#define UART_ERROR_PARITY_MASK               (0x2U)
#define UART_ERROR_PARITY_SHIFT              (1U)
#define UART_ERROR_PARITY(x)                 (((uint32_t)(((uint32_t)(x)) << UART_ERROR_PARITY_SHIFT)) & UART_ERROR_PARITY_MASK)

/*! @name IRQ_EN - UART Interrupt configuration register */
#define UART_IRQ_EN_RX_MASK                  (0x1U)
#define UART_IRQ_EN_RX_SHIFT                 (0U)
#define UART_IRQ_EN_RX(x)                    (((uint32_t)(((uint32_t)(x)) /* << UART_IRQ_EN_RX_SHIFT */)) & UART_IRQ_EN_RX_MASK)
#define UART_IRQ_EN_ERROR_MASK               (0x2U)
#define UART_IRQ_EN_ERROR_SHIFT              (1U)
#define UART_IRQ_EN_ERROR(x)                 (((uint32_t)(((uint32_t)(x)) << UART_IRQ_EN_ERROR_SHIFT)) & UART_IRQ_EN_ERROR_MASK)

/*! @name VALID - UART read polling flag register */
#define UART_VALID_RX_MASK                   (0x1U)
#define UART_VALID_RX_SHIFT                  (0U)
#define UART_VALID_RX(x)                     (((uint32_t)(((uint32_t)(x)) /* << UART_VALID_RX_SHIFT */)) & UART_VALID_RX_MASK)


/*!
 * @}
 */ /* end of group UART_Register_Masks */


/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base address */
#define UART0_BASE                                (UDMA_BASE + PER_ID_UART0 * 128U)
/** Peripheral UART0 base pointer */
#define UART0                                     ((UART_Type *)UART0_BASE)
/** Peripheral UART1 base address */
#define UART1_BASE                                (UDMA_BASE + PER_ID_UART1 * 128U)
/** Peripheral UART base pointer */
#define UART1                                     ((UART_Type *)UART1_BASE)
/** Peripheral UART2 base address */
#define UART2_BASE                                (UDMA_BASE + PER_ID_UART2 * 128U)
/** Peripheral UART base pointer */
#define UART2                                     ((UART_Type *)UART2_BASE)
/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS                           { UART0_BASE, UART1_BASE, UART2_BASE }
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                            { UART0, UART1, UART2}
/** Interrupt vectors for the UART peripheral type */
#define UART_RX_IRQS                              { UART0_RX_IRQn, UART1_RX_IRQn, UART2_RX_IRQn}
#define UART_TX_IRQS                              { UART0_TX_IRQn, UART1_TX_IRQn, UART2_TX_IRQn }

/*!
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Peripheral_Access_Layer I2C Peripheral Access Layer
 * @{
 */

/** I2C - Register Layout Typedef */
typedef struct {
  UDMA_CMD_Type  UDMA_I2C;                          /**< I2C UDMA general register, offset: 0x0 */
  __I   uint32_t STATUS;                            /**< I2C Status register, offset: 0x30 */
  __IO  uint32_t SETUP;                             /**< I2C Configuration register, offset: 0x34 */

} I2C_Type;

/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Register_Masks I2C Register Masks
 * @{
 */
/*! @name STATUS - I2C Status register */
#define I2C_STATUS_BUSY_MASK         (0x1U)
#define I2C_STATUS_BUSY_SHIFT        (0U)
#define I2C_STATUS_BUSY(x)           (((uint32_t)(((uint32_t)(x)) << I2C_STATUS_BUSY_SHIFT)) & I2C_STATUS_BUSY_MASK)
#define I2C_STATUS_ARB_LOST_MASK     (0x2U)
#define I2C_STATUS_ARB_LOST_SHIFT    (1U)
#define I2C_STATUS_ARB_LOST(x)       (((uint32_t)(((uint32_t)(x)) << I2C_STATUS_ARB_LOST_SHIFT)) & I2C_STATUS_ARB_LOST_MASK)
#define I2C_STATUS_ACK_MASK          (0x4U)
#define I2C_STATUS_ACK_SHIFT         (2U)
#define I2C_STATUS_ACK(x)            (((uint32_t)(((uint32_t)(x)) << I2C_STATUS_ACK_SHIFT)) & I2C_STATUS_ACK_MASK)

/*! @name SETUP - I2C SETUP register */
#define I2C_SETUP_DO_RST_MASK        (0x1U)
#define I2C_SETUP_DO_RST_SHIFT       (0U)
#define I2C_SETUP_DO_RST(x)          (((uint32_t)(((uint32_t)(x)) << I2C_SETUP_DO_RST_SHIFT)) & I2C_SETUP_DO_RST_MASK)

/*! @name uDMA - I2C uDMA control CMD */
#define I2C_CMD_MASK                 (0xF0000000U)
#define I2C_CMD_SHIFT                (28U)
#define I2C_CMD(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_CMD_SHIFT)) & I2C_CMD_MASK)

#define I2C_CMD_START                I2C_CMD(0x0)
#define I2C_CMD_WAIT_EV              I2C_CMD(0x1)
#define I2C_CMD_STOP                 I2C_CMD(0x2)
#define I2C_CMD_UCA                  I2C_CMD(0x3)
#define I2C_CMD_RD_ACK               I2C_CMD(0x4)
#define I2C_CMD_UCS                  I2C_CMD(0x5)
#define I2C_CMD_RD_NACK              I2C_CMD(0x6)
#define I2C_CMD_WB                   I2C_CMD(0x7)
#define I2C_CMD_WR                   I2C_CMD(0x8)
#define I2C_CMD_EOT                  I2C_CMD(0x9)
#define I2C_CMD_WAIT                 I2C_CMD(0xA)
#define I2C_CMD_RPT                  I2C_CMD(0xC)
#define I2C_CMD_CFG                  I2C_CMD(0xE)

/*! @name CMD_UCA  */
#define I2C_CMD_UCA_ADDR_MASK              (0x1FFFFFU)
#define I2C_CMD_UCA_ADDR_SHIFT             (0U)
#define I2C_CMD_UCA_ADDR(x)                (((uint32_t)(((uint32_t)(x)) /* << I2C_CMD_UCA_ADDR_SHIFT */)) & I2C_CMD_UCA_ADDR_MASK)
#define I2C_CMD_UCA_TXRXN_MASK             (0x8000000U)
#define I2C_CMD_UCA_TXRXN_SHIFT            (27U)
#define I2C_CMD_UCA_TXRXN(x)               (((uint32_t)(((uint32_t)(x)) << I2C_CMD_UCA_TXRXN_SHIFT)) & I2C_CMD_UCA_TXRXN_MASK)

/*! @name CMD_UCS */
#define I2C_CMD_UCS_SIZE_MASK              (0xFFFFFU)
#define I2C_CMD_UCS_SIZE_SHIFT             (0U)
#define I2C_CMD_UCS_SIZE(x)                (((uint32_t)(((uint32_t)(x)) /* << I2C_CMD_UCS_SIZE_SHIFT */)) & I2C_CMD_UCS_SIZE_MASK)
#define I2C_CMD_UCS_TXRXN_MASK             (0x8000000U)
#define I2C_CMD_UCS_TXRXN_SHIFT            (27U)
#define I2C_CMD_UCS_TXRXN(x)               (((uint32_t)(((uint32_t)(x)) << I2C_CMD_UCS_TXRXN_SHIFT)) & I2C_CMD_UCS_TXRXN_MASK)

#define I2C_CMD_SETUP_UCA(txrxn,addr)      ( I2C_CMD_UCA                \
                                             | I2C_CMD_UCA_TXRXN(txrxn) \
                                             | I2C_CMD_UCA_ADDR(addr) )

#define I2C_CMD_SETUP_UCS(txrxn,size)      ( I2C_CMD_UCS                \
                                             | I2C_CMD_UCS_TXRXN(txrxn) \
                                             | I2C_CMD_UCS_SIZE(size) )

/*!
 * @}
 */ /* end of group I2C_Register_Masks */


/* I2C - Peripheral instance base addresses */
/** Peripheral I2C0 base address */
#define I2C0_BASE                                (UDMA_BASE + PER_ID_I2C0 * 128U)
/** Peripheral I2C0 base pointer */
#define I2C0                                     ((I2C_Type *)I2C0_BASE)
/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS                           { I2C0_BASE }
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASE_PTRS                            { I2C0 }
/** Interrupt vectors for the I2C peripheral type */
#define I2C_RX_IRQS                              { I2C0_RX_IRQn }
#define I2C_TX_IRQS                              { I2C0_TX_IRQn }

/*!
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup HYPERBUS_Peripheral_Access_Layer HYPERBUS Peripheral Access Layer
 * @{
 */

/** HYPERBUS - Register Layout Typedef */
typedef struct {
  UDMA_Type    UDMA_HYPERBUS;              /**< HYPERBUS UDMA general register, offset: 0x0 */
  __IO  uint32_t EXT_ADDR;                 /**< HYPERBUS Memory access address register, offset: 0x20 */
  __IO  uint32_t TIMING_CFG;               /**< HYPERBUS Timing Configuration register, offset: 0x24 */
  __IO  uint32_t MBR0;                     /**< HYPERBUS Memory base address 0 register, offset: 0x28 */
  __IO  uint32_t MBR1;                     /**< HYPERBUS Memory base address 1 register, offset: 0x2C */
  __IO  uint32_t DEVICE_TYPE;              /**< HYPERBUS device type register, offset: 0x30 */
  __IO  uint32_t OSPI_CMD;                 /**< HYPERBUS Octo SPI command register, offset: 0x34 */
  __IO  uint32_t OSPI_ADDR;                /**< HYPERBUS Octo SPI address register, offset: 0x38 */
  __IO  uint32_t OSPI_CFG;                 /**< HYPERBUS Octo SPI cofiguration register, offset: 0x3C */
  __IO  uint32_t OSPI_CSN;                 /**< HYPERBUS Octo SPI chip select register, offset: 0x40 */
  __IO  uint32_t BURST_SIZE;               /**< HYPERBUS Octo SPI burst size register, offset: 0x44 */
  __IO  uint32_t BURST_ENABLE;             /**< HYPERBUS Octo SPI burst enable register, offset: 0x48 */
  __IO  uint32_t IRQ_EN;                   /**< HYPERBUS Octo SPI interrupt enable, offset: 0x4C */
  __IO  uint32_t CLK_DIV;                  /**< HYPERBUS Clock devider register, offset: 0x50 */
  __IO  uint32_t STATUS;                   /**< HYPERBUS Status register, offset: 0x54 */
} HYPERBUS_Type;

/* ----------------------------------------------------------------------------
   -- HYPERBUS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup HYPERBUS_Register_Masks HYPERBUS Register Masks
 * @{
 */

/*! @name EXT_ADDR - HYPERBUS device type register */
#define HYPERBUS_EXT_ADDR_MASK                (0x7FFFFFFFU)
#define HYPERBUS_EXT_ADDR_SHIFT               (0U)
#define HYPERBUS_EXT_ADDR(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_EXT_ADDR_SHIFT)) & HYPERBUS_EXT_ADDR_MASK)

#define HYPERBUS_EXT_ADDR_REG_ACCESS_MASK     (0x80000000U)
#define HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT    (31U)
#define HYPERBUS_EXT_ADDR_REG_ACCESS(x)       (((uint32_t)(((uint32_t)(x)) << HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT)) & HYPERBUS_EXT_ADDR_REG_ACCESS_MASK)

/*! @name TIMING_CFG - HYPERBUS Timing Configuration register */
#define HYPERBUS_TIMING_CFG_LATENCY0_MASK               (0x1FU)
#define HYPERBUS_TIMING_CFG_LATENCY0_SHIFT              (0U)
#define HYPERBUS_TIMING_CFG_LATENCY0(x)                 (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_LATENCY0_SHIFT)) & HYPERBUS_TIMING_CFG_LATENCY0_MASK)
#define HYPERBUS_TIMING_CFG_LATENCY1_MASK               (0x3E0U)
#define HYPERBUS_TIMING_CFG_LATENCY1_SHIFT              (5U)
#define HYPERBUS_TIMING_CFG_LATENCY1(x)                 (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_LATENCY1_SHIFT)) & HYPERBUS_TIMING_CFG_LATENCY1_MASK)
#define HYPERBUS_TIMING_CFG_RW_RECOVERY_MASK            (0x3C00U)
#define HYPERBUS_TIMING_CFG_RW_RECOVERY_SHIFT           (10U)
#define HYPERBUS_TIMING_CFG_RW_RECOVERY(x)              (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_RW_RECOVERY_SHIFT)) & HYPERBUS_TIMING_CFG_RW_RECOVERY_MASK)
#define HYPERBUS_TIMING_CFG_RWDS_DELAY_MASK             (0x1C000U)
#define HYPERBUS_TIMING_CFG_RWDS_DELAY_SHIFT            (14U)
#define HYPERBUS_TIMING_CFG_RWDS_DELAY(x)               (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_RWDS_DELAY_SHIFT)) & HYPERBUS_TIMING_CFG_RWDS_DELAY_MASK)
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK       (0x20000U)
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SHIFT      (17U)
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(x)         (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SHIFT)) & HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK)
#define HYPERBUS_TIMING_CFG_CSM_MASK                    (0xFFFC0000U)
#define HYPERBUS_TIMING_CFG_CSM_SHIFT                   (18U)
#define HYPERBUS_TIMING_CFG_CSM(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_TIMING_CFG_CSM_SHIFT)) & HYPERBUS_TIMING_CFG_CSM_MASK)

/*! @name DEVICE_TYPE - HYPERBUS device type register */
#define HYPERBUS_DEVICE_TYPE_MASK                (0x1U)
#define HYPERBUS_DEVICE_TYPE_SHIFT               (0U)
#define HYPERBUS_DEVICE_TYPE(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_DEVICE_TYPE_SHIFT)) & HYPERBUS_DEVICE_TYPE_MASK)
#define HYPERBUS_DEVICE_DT0_MASK                 (0x2U)
#define HYPERBUS_DEVICE_DT0_SHIFT                (1U)
#define HYPERBUS_DEVICE_DT0(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_DEVICE_DT0_SHIFT)) & HYPERBUS_DEVICE_DT0_MASK)
#define HYPERBUS_DEVICE_DT1_MASK                 (0x4U)
#define HYPERBUS_DEVICE_DT1_SHIFT                (2U)
#define HYPERBUS_DEVICE_DT1(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_DEVICE_DT1_SHIFT)) & HYPERBUS_DEVICE_DT1_MASK)

/*! @name OSPI_CMD - Octo SPI command and alternative bytes register */
#define HYPERBUS_OSPI_CFG_CMD_MASK               (0xFFFFU)
#define HYPERBUS_OSPI_CFG_CMD_SHIFT              (0U)
#define HYPERBUS_OSPI_CFG_CMD(x)                 (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_OSPI_CFG_CMD_SHIFT */)) & HYPERBUS_OSPI_CFG_CMD_MASK)
#define HYPERBUS_OSPI_CFG_ALTER_MASK             (0xFFFF0000U)
#define HYPERBUS_OSPI_CFG_ALTER_SHIFT            (16U)
#define HYPERBUS_OSPI_CFG_ALTER(x)               (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_ALTER_SHIFT)) & HYPERBUS_OSPI_CFG_ALTER_MASK)

/*! @name OSPI_CFG - Octo SPI configuration register */
#define HYPERBUS_OSPI_CFG_CMD_SIZE_MASK          (0x3U)
#define HYPERBUS_OSPI_CFG_CMD_SIZE_SHIFT         (0U)
#define HYPERBUS_OSPI_CFG_CMD_SIZE(x)            (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_OSPI_CFG_CMD_SIZE_SHIFT */)) & HYPERBUS_OSPI_CFG_CMD_SIZE_MASK)
#define HYPERBUS_OSPI_CFG_ADDR_SIZE_MASK         (0x70U)
#define HYPERBUS_OSPI_CFG_ADDR_SIZE_SHIFT        (4U)
#define HYPERBUS_OSPI_CFG_ADDR_SIZE(x)           (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_ADDR_SIZE_SHIFT)) & HYPERBUS_OSPI_CFG_ADDR_SIZE_MASK)
#define HYPERBUS_OSPI_CFG_SPI_MASK               (0x300U)
#define HYPERBUS_OSPI_CFG_SPI_SHIFT              (8U)
#define HYPERBUS_OSPI_CFG_SPI(x)                 (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_SPI_SHIFT)) & HYPERBUS_OSPI_CFG_SPI_MASK)
#define HYPERBUS_OSPI_CFG_CMD_DTR_MASK           (0x1000U)
#define HYPERBUS_OSPI_CFG_CMD_DTR_SHIFT          (12U)
#define HYPERBUS_OSPI_CFG_CMD_DTR(x)             (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_CMD_DTR_SHIFT)) & HYPERBUS_OSPI_CFG_CMD_DTR_MASK)
#define HYPERBUS_OSPI_CFG_ADDR_DTR_MASK          (0x2000U)
#define HYPERBUS_OSPI_CFG_ADDR_DTR_SHIFT         (13U)
#define HYPERBUS_OSPI_CFG_ADDR_DTR(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_ADDR_DTR_SHIFT)) & HYPERBUS_OSPI_CFG_ADDR_DTR_MASK)
#define HYPERBUS_OSPI_CFG_DATA_DTR_MASK          (0x4000U)
#define HYPERBUS_OSPI_CFG_DATA_DTR_SHIFT         (14U)
#define HYPERBUS_OSPI_CFG_DATA_DTR(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_DATA_DTR_SHIFT)) & HYPERBUS_OSPI_CFG_DATA_DTR_MASK)
#define HYPERBUS_OSPI_CFG_DATA_MSB_MASK          (0x8000U)
#define HYPERBUS_OSPI_CFG_DATA_MSB_SHIFT         (15U)
#define HYPERBUS_OSPI_CFG_DATA_MSB(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CFG_DATA_MSB_SHIFT)) & HYPERBUS_OSPI_CFG_DATA_MSB_MASK)

/*! @name OSPI_CSN - Octo SPI chip select configuration register */
#define HYPERBUS_OSPI_CSN_INDEX_MASK             (0x1U)
#define HYPERBUS_OSPI_CSN_INDEX_SHIFT            (0U)
#define HYPERBUS_OSPI_CSN_INDEX(x)               (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_OSPI_CSN_INDEX_SHIFT */)) & HYPERBUS_OSPI_CSN_INDEX_MASK)
#define HYPERBUS_OSPI_CSN_USER_CTRL_MASK         (0x10U)
#define HYPERBUS_OSPI_CSN_USER_CTRL_SHIFT        (4U)
#define HYPERBUS_OSPI_CSN_USER_CTRL(x)           (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CSN_USER_CTRL_SHIFT)) & HYPERBUS_OSPI_CSN_USER_CTRL_MASK)
#define HYPERBUS_OSPI_CSN_VAL_MASK               (0x20U)
#define HYPERBUS_OSPI_CSN_VAL_SHIFT              (5U)
#define HYPERBUS_OSPI_CSN_VAL(x)                 (((uint32_t)(((uint32_t)(x)) << HYPERBUS_OSPI_CSN_VAL_SHIFT)) & HYPERBUS_OSPI_CSN_VAL_MASK)

/*! @name IRQ_EN - HYPERBUS interrupt enable register */
#define HYPERBUS_IRQ_EN_MASK                    (0x1U)
#define HYPERBUS_IRQ_EN_SHIFT                   (0U)
#define HYPERBUS_IRQ_EN(x)                      (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_IRQ_EN_SHIFT */)) & HYPERBUS_IRQ_EN_MASK)

/*! @name CLK_DIV - HYPERBUS clock divider register */
#define HYPERBUS_CLK_DIV_MASK                    (0xFFU)
#define HYPERBUS_CLK_DIV_SHIFT                   (0U)
#define HYPERBUS_CLK_DIV(x)                      (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_CLK_DIV_SHIFT */)) & HYPERBUS_CLK_DIV_MASK)

/*! @name STATUS - HYPERBUS status register */
#define HYPERBUS_STATUS_TX_ERROR_MASK            (0x1U)
#define HYPERBUS_STATUS_TX_ERROR_SHIFT           (0U)
#define HYPERBUS_STATUS_TX_ERROR(x)              (((uint32_t)(((uint32_t)(x)) /* << HYPERBUS_STATUS_TX_ERROR_SHIFT */)) & HYPERBUS_STATUS_TX_ERROR_MASK)
#define HYPERBUS_STATUS_RX_ERROR_MASK            (0x2U)
#define HYPERBUS_STATUS_RX_ERROR_SHIFT           (1U)
#define HYPERBUS_STATUS_RX_ERROR(x)              (((uint32_t)(((uint32_t)(x)) << HYPERBUS_STATUS_RX_ERROR_SHIFT)) & HYPERBUS_STATUS_RX_ERROR_MASK)
#define HYPERBUS_STATUS_TXRX_END_MASK            (0x4U)
#define HYPERBUS_STATUS_TXRX_END_SHIFT           (2U)
#define HYPERBUS_STATUS_TXRX_END(x)              (((uint32_t)(((uint32_t)(x)) << HYPERBUS_STATUS_TXRX_END_SHIFT)) & HYPERBUS_STATUS_TXRX_END_MASK)

/*!
 * @}
 */ /* end of group HYPERBUS_Register_Masks */


/* HYPERBUS - Peripheral instance base addresses */
/** Peripheral HYPERBUS base address */
#define HYPERBUS_BASE0                                (UDMA_BASE + PER_ID_HYPERBUS * 128U)
/** Peripheral HYPERBUS base pointer */
#define HYPERBUS0                                     ((HYPERBUS_Type *)HYPERBUS_BASE0)
/** Array initializer of HYPERBUS peripheral base addresses */
#define HYPERBUS_BASE_ADDRS                           { HYPERBUS_BASE0 }
/** Array initializer of HYPERBUS peripheral base pointers */
#define HYPERBUS_BASE_PTRS                            { HYPERBUS0 }
/** Interrupt vectors for the HYPERBUS peripheral type */
#define HYPERBUS_RX_IRQS                              { HYPERBUS_RX_IRQn }
#define HYPERBUS_TX_IRQS                              { HYPERBUS_TX_IRQn }

/*!
 * @}
 */ /* end of group HYPERBUS_Peripheral_Access_Layer */


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
#define VIRTUAL_JTAG_BASE                                 (UDMA_BASE + PER_ID_JTAG * 128U)
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
   -- SDIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SDIO_Peripheral_Access_Layer SDIO Peripheral Access Layer
 * @{
 */

/** SDIO - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_SDIO;                        /**< SDIO UDMA general register, offset: 0x0 */
  __O  uint32_t CMD_OP;                           /**< SDIO command operation register, offset: 0x20 */
  __O  uint32_t CMD_ARG;                          /**< SDIO command argument register, offset: 0x24 */
  __O  uint32_t DATA_SETUP;                       /**< SDIO date register, offset: 0x28 */
  __O  uint32_t START;                            /**< SDIO start register, offset: 0x2C */

  __I  uint32_t RSP0;                             /**< SDIO response 0 register, offset: 0x30 */
  __I  uint32_t RSP1;                             /**< SDIO response 1 register, offset: 0x34 */
  __I  uint32_t RSP2;                             /**< SDIO response 2 register, offset: 0x38 */
  __I  uint32_t RSP3;                             /**< SDIO response 3 register, offset: 0x3C */
  __IO uint32_t CLK_DIV;                          /**< SDIO clock divider register, offset: 0x40 */
  __IO uint32_t STATUS;                           /**< SDIO status register, offset: 0x44 */
} SDIO_Type;

/* ----------------------------------------------------------------------------
   -- SDIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SDIO_Register_Masks SDIO Register Masks
 * @{
 */
/*! @name CMD_OP - SDIO command operation register */
#define SDIO_CMD_OP_MASK                       (0x1FU)
#define SDIO_CMD_OP_SHIFT                      (0U)
#define SDIO_CMD_OP(x)                         (((uint32_t)(((uint32_t)(x)) /* << SDIO_CMD_OP_SHIFT */)) & SDIO_CMD_OP_MASK)

#define SDIO_CMD_RSP_TYPE_MASK                 (0x700U)
#define SDIO_CMD_RSP_TYPE_SHIFT                (8U)
#define SDIO_CMD_RSP_TYPE(x)                   (((uint32_t)(((uint32_t)(x)) << SDIO_CMD_RSP_TYPE_SHIFT)) & SDIO_CMD_RSP_TYPE_MASK)

/*! @name DATA_SETUP - SDIO data register */
#define SDIO_DATA_SETUP_EN_MASK                (0x1U)
#define SDIO_DATA_SETUP_EN_SHIFT               (0U)
#define SDIO_DATA_SETUP_EN(x)                  (((uint32_t)(((uint32_t)(x)) /* << SDIO_DATA_SETUP_EN_SHIFT */)) & SDIO_DATA_SETUP_EN_MASK)
#define SDIO_DATA_SETUP_RWN_MASK               (0x2U)
#define SDIO_DATA_SETUP_RWN_SHIFT              (1U)
#define SDIO_DATA_SETUP_RWN(x)                 (((uint32_t)(((uint32_t)(x)) << SDIO_DATA_SETUP_RWN_SHIFT)) & SDIO_DATA_SETUP_RWN_MASK)
#define SDIO_DATA_SETUP_QUAD_MASK              (0x4U)
#define SDIO_DATA_SETUP_QUAD_SHIFT             (2U)
#define SDIO_DATA_SETUP_QUAD(x)                (((uint32_t)(((uint32_t)(x)) << SDIO_DATA_SETUP_QUAD_SHIFT)) & SDIO_DATA_SETUP_QUAD_MASK)
#define SDIO_DATA_SETUP_BLOCK_NUM_MASK         (0xFF00U)
#define SDIO_DATA_SETUP_BLOCK_NUM_SHIFT        (8U)
#define SDIO_DATA_SETUP_BLOCK_NUM(x)           (((uint32_t)(((uint32_t)(x)) << SDIO_DATA_SETUP_BLOCK_NUM_SHIFT)) & SDIO_DATA_SETUP_BLOCK_NUM_MASK)
#define SDIO_DATA_SETUP_BLOCK_SIZE_MASK        (0x3FF0000U)
#define SDIO_DATA_SETUP_BLOCK_SIZE_SHIFT       (16U)
#define SDIO_DATA_SETUP_BLOCK_SIZE(x)          (((uint32_t)(((uint32_t)(x)) << SDIO_DATA_SETUP_BLOCK_SIZE_SHIFT)) & SDIO_DATA_SETUP_BLOCK_SIZE_MASK)

/*! @name CLK_DIV - SDIO clock divider register */
#define SDIO_CLK_DIV_MASK                      (0xFFU)
#define SDIO_CLK_DIV_SHIFT                     (0U)
#define SDIO_CLK_DIV(x)                        (((uint32_t)(((uint32_t)(x)) /* << SDIO_CLK_DIV_SHIFT */)) & SDIO_CLK_DIV_MASK)

#define SDIO_CLK_DIV_VALID_MASK                (0x100U)
#define SDIO_CLK_DIV_VALID_SHIFT               (8U)
#define SDIO_CLK_DIV_VALID(x)                  (((uint32_t)(((uint32_t)(x)) << SDIO_CLK_DIV_VALID_SHIFT)) & SDIO_CLK_DIV_VALID_MASK)

/*! @name STATUS - SDIO status register */
#define SDIO_STATUS_EOT_MASK                   (0x1U)
#define SDIO_STATUS_EOT_SHIFT                  (0U)
#define SDIO_STATUS_EOT(x)                     (((uint32_t)(((uint32_t)(x)) /* << SDIO_STATUS_EOT_SHIFT */)) & SDIO_STATUS_EOT_MASK)
#define SDIO_STATUS_ERROR_MASK                 (0x2U)
#define SDIO_STATUS_ERROR_SHIFT                (1U)
#define SDIO_STATUS_ERROR(x)                   (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_ERROR_SHIFT)) & SDIO_STATUS_ERROR_MASK)
#define SDIO_STATUS_CMD_ERR_RSP_TIMEOUT_MASK   (0x10000U)
#define SDIO_STATUS_CMD_ERR_RSP_TIMEOUT_SHIFT  (16U)
#define SDIO_STATUS_CMD_ERR_RSP_TIMEOUT(x)     (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_CMD_ERR_RSP_TIMEOUT_SHIFT)) & SDIO_STATUS_CMD_ERR_RSP_TIMEOUT_MASK)
#define SDIO_STATUS_CMD_ERR_RSP_WRONG_DIR_MASK (0x20000U)
#define SDIO_STATUS_CMD_ERR_RSP_WRONG_DIR_SHIFT (17U)
#define SDIO_STATUS_CMD_ERR_RSP_WRONG_DIR(x)   (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_CMD_ERR_RSP_WRONG_DIR_SHIFT)) & SDIO_STATUS_CMD_ERR_RSP_WRONG_DIR_MASK)
#define SDIO_STATUS_CMD_ERR_RSP_BUSY_TIMEOUT_MASK (0x40000U)
#define SDIO_STATUS_CMD_ERR_RSP_BUSY_TIMEOUT_SHIFT (18U)
#define SDIO_STATUS_CMD_ERR_RSP_BUSY_TIMEOUT(x)   (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_CMD_ERR_RSP_BUSY_TIMEOUT_SHIFT)) & SDIO_STATUS_CMD_ERR_RSP_BUSY_TIMEOUT_MASK)
#define SDIO_STATUS_DATA_ERR_TIMEOUT_MASK         (0x1000000U)
#define SDIO_STATUS_DATA_ERR_TIMEOUT_SHIFT        (24U)
#define SDIO_STATUS_DATA_ERR_TIMEOUT(x)           (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_DATA_ERR_TIMEOUT_SHIFT)) & SDIO_STATUS_DATA_ERR_TIMEOUT_MASK)
#define SDIO_STATUS_DATA_ERR_W_CRC_ERR_MASK       (0x2000000U)
#define SDIO_STATUS_DATA_ERR_W_CRC_ERR_SHIFT      (25U)
#define SDIO_STATUS_DATA_ERR_W_CRC_ERR(x)         (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_DATA_ERR_W_CRC_ERR_SHIFT)) & SDIO_STATUS_DATA_ERR_W_CRC_ERR_MASK)
#define SDIO_STATUS_DATA_ERR_W_ERR_MASK           (0x4000000U)
#define SDIO_STATUS_DATA_ERR_W_ERR_SHIFT          (26U)
#define SDIO_STATUS_DATA_ERR_W_ERR(x)             (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_DATA_ERR_W_ERR_SHIFT)) & SDIO_STATUS_DATA_ERR_W_ERR_MASK)
#define SDIO_STATUS_DATA_ERR_W_OTHERS_MASK        (0x8000000U)
#define SDIO_STATUS_DATA_ERR_W_OTHERS_SHIFT       (27U)
#define SDIO_STATUS_DATA_ERR_W_OTHERS(x)          (((uint32_t)(((uint32_t)(x)) << SDIO_STATUS_DATA_ERR_W_OTHERS_SHIFT)) & SDIO_STATUS_DATA_ERR_W_OTHERS_MASK)

/*!
 * @}
 */ /* end of group SDIO_Register_Masks */


/* SDIO - Peripheral instance base addresses */
/** Peripheral SDIO base address */
#define SDIO_BASE                                 (UDMA_BASE + PER_ID_SDIO * 128U)
/** Peripheral SDIO base pointer */
#define SDIO                                      ((SDIO_Type *)SDIO_BASE)
/** Array initializer of SDIO peripheral base addresses */
#define SDIO_BASE_ADDRS                           { SDIO_BASE }
/** Array initializer of SDIO peripheral base pointers */
#define SDIO_BASE_PTRS                            { SDIO }
/** Interrupt vectors for the SDIO peripheral type */
#define SDIO_RX_IRQS                              { SDIO_RX_IRQn }
#define SDIO_TX_IRQS                              { SDIO_TX_IRQn}

/*!
 * @}
 */ /* end of group SDIO_Peripheral_Access_Layer */


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
#define I2S_CLKCFG_MASTER_CLK_DIV_MASK              (0xFFU)
#define I2S_CLKCFG_MASTER_CLK_DIV_SHIFT             (0U)
#define I2S_CLKCFG_MASTER_CLK_DIV(x)                (((uint32_t)(((uint32_t)(x)) /*<< I2S_CLKCFG_MASTER_CLK_DIV_SHIFT */)) & I2S_CLKCFG_MASTER_CLK_DIV_MASK)
#define I2S_CLKCFG_SLAVE_CLK_DIV_MASK               (0xFF00U)
#define I2S_CLKCFG_SLAVE_CLK_DIV_SHIFT              (8U)
#define I2S_CLKCFG_SLAVE_CLK_DIV(x)                 (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_CLK_DIV_SHIFT)) & I2S_CLKCFG_SLAVE_CLK_DIV_MASK)
#define I2S_CLKCFG_COMMON_CLK_DIV_MASK              (0xFF0000U)
#define I2S_CLKCFG_COMMON_CLK_DIV_SHIFT             (16U)
#define I2S_CLKCFG_COMMON_CLK_DIV(x)                (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_COMMON_CLK_DIV_SHIFT)) & I2S_CLKCFG_COMMON_CLK_DIV_MASK)

#define I2S_CLKCFG_PDM_CLK_EN_MASK                  (0x1000000U)
#define I2S_CLKCFG_PDM_CLK_EN_SHIFT                 (24U)
#define I2S_CLKCFG_PDM_CLK_EN(x)                    (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_PDM_CLK_EN_SHIFT)) & I2S_CLKCFG_PDM_CLK_EN_MASK)
#define I2S_CLKCFG_MASTER_CLK_EN_MASK               (0x2000000U)
#define I2S_CLKCFG_MASTER_CLK_EN_SHIFT              (25U)
#define I2S_CLKCFG_MASTER_CLK_EN(x)                 (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_CLK_EN_SHIFT)) & I2S_CLKCFG_MASTER_CLK_EN_MASK)
#define I2S_CLKCFG_SLAVE_CLK_EN_MASK                (0x4000000U)
#define I2S_CLKCFG_SLAVE_CLK_EN_SHIFT               (26U)
#define I2S_CLKCFG_SLAVE_CLK_EN(x)                  (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_CLK_EN_SHIFT)) & I2S_CLKCFG_SLAVE_CLK_EN_MASK)

#define I2S_CLKCFG_SLAVE_SEL_EXT_MASK               (0x10000000U)
#define I2S_CLKCFG_SLAVE_SEL_EXT_SHIFT              (28U)
#define I2S_CLKCFG_SLAVE_SEL_EXT(x)                 (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_SEL_EXT_SHIFT)) & I2S_CLKCFG_SLAVE_SEL_EXT_MASK)
#define I2S_CLKCFG_SLAVE_SEL_NUM_MASK               (0x20000000U)
#define I2S_CLKCFG_SLAVE_SEL_NUM_SHIFT              (29U)
#define I2S_CLKCFG_SLAVE_SEL_NUM(x)                 (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_SLAVE_SEL_NUM_SHIFT)) & I2S_CLKCFG_SLAVE_SEL_NUM_MASK)
#define I2S_CLKCFG_MASTER_SEL_EXT_MASK              (0x40000000U)
#define I2S_CLKCFG_MASTER_SEL_EXT_SHIFT             (30U)
#define I2S_CLKCFG_MASTER_SEL_EXT(x)                (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_SEL_EXT_SHIFT)) & I2S_CLKCFG_MASTER_SEL_EXT_MASK)
#define I2S_CLKCFG_MASTER_SEL_NUM_MASK              (0x80000000U)
#define I2S_CLKCFG_MASTER_SEL_NUM_SHIFT             (31U)
#define I2S_CLKCFG_MASTER_SEL_NUM(x)                (((uint32_t)(((uint32_t)(x)) << I2S_CLKCFG_MASTER_SEL_NUM_SHIFT)) & I2S_CLKCFG_MASTER_SEL_NUM_MASK)

/*! @name SLV_SETUP - I2S slave clock configuration Register */
#define I2S_SLV_WORDS_MASK                          (0x7U)
#define I2S_SLV_WORDS_SHIFT                         (0U)
#define I2S_SLV_WORDS(x)                            (((uint32_t)(((uint32_t)(x)) /* << I2S_SLV_WORDS_SHIFT */)) & I2S_SLV_WORDS_MASK)
#define I2S_SLV_BITS_WORD_MASK                      (0x1F00U)
#define I2S_SLV_BITS_WORD_SHIFT                     (8U)
#define I2S_SLV_BITS_WORD(x)                        (((uint32_t)(((uint32_t)(x)) << I2S_SLV_BITS_WORD_SHIFT)) & I2S_SLV_BITS_WORD_MASK)
#define I2S_SLV_LSB_MASK                            (0x10000U)
#define I2S_SLV_LSB_SHIFT                           (16U)
#define I2S_SLV_LSB(x)                              (((uint32_t)(((uint32_t)(x)) << I2S_SLV_LSB_SHIFT)) & I2S_SLV_LSB_MASK)
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
#define I2S_MST_LSB_MASK                            (0x10000U)
#define I2S_MST_LSB_SHIFT                           (16U)
#define I2S_MST_LSB(x)                              (((uint32_t)(((uint32_t)(x)) << I2S_MST_LSB_SHIFT)) & I2S_MST_LSB_MASK)
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
#define I2S_BASE                                 (UDMA_BASE + PER_ID_I2S * 128U)
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



/* ----------------------------------------------------------------------------
   -- CPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CPI_Peripheral_Access_Layer CPI Peripheral Access Layer
 * @{
 */

/** CPI - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_CPI;                           /**< CPI UDMA general register, offset: 0x0 */
  __IO  uint32_t CFG_GLOB;                          /**< CPI global configuration register, offset: 0x20 */
  __IO  uint32_t CFG_LL;                            /**< CPI lower left comer configuration register, offset: 0x24 */
  __IO  uint32_t CFG_UR;                            /**< CPI upper right comer configuration register, offset: 0x28 */
  __IO  uint32_t CFG_SIZE;                          /**< CPI horizontal resolution configuration register, offset: 0x2C */
  __IO  uint32_t CFG_FILTER;                        /**< CPI RGB coefficients configuration register, offset: 0x30 */
  __IO  uint32_t VSYNC_POLARITY;                    /**< CPI VSYNC Polarity register, offset: 0x34 */
} CPI_Type;

/* ----------------------------------------------------------------------------
   -- CPI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CPI_Register_Masks CPI Register Masks
 * @{
 */
/*! @name CFG_GLOB - CPI global configuration register */
#define CPI_CFG_GLOB_FRAMEDROP_EN_MASK             (0x1U)
#define CPI_CFG_GLOB_FRAMEDROP_EN_SHIFT            (0U)
#define CPI_CFG_GLOB_FRAMEDROP_EN(x)               (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_FRAMEDROP_EN_SHIFT)) & CPI_CFG_GLOB_FRAMEDROP_EN_MASK)
#define CPI_CFG_GLOB_FRAMEDROP_VAL_MASK            (0x7EU)
#define CPI_CFG_GLOB_FRAMEDROP_VAL_SHIFT           (1U)
#define CPI_CFG_GLOB_FRAMEDROP_VAL(x)              (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_FRAMEDROP_VAL_SHIFT)) & CPI_CFG_GLOB_FRAMEDROP_VAL_MASK)
#define CPI_CFG_GLOB_FRAMESLICE_EN_MASK            (0x80U)
#define CPI_CFG_GLOB_FRAMESLICE_EN_SHIFT           (7U)
#define CPI_CFG_GLOB_FRAMESLICE_EN(x)              (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_FRAMESLICE_EN_SHIFT)) & CPI_CFG_GLOB_FRAMESLICE_EN_MASK)
#define CPI_CFG_GLOB_FORMAT_MASK                   (0x700U)
#define CPI_CFG_GLOB_FORMAT_SHIFT                  (8U)
#define CPI_CFG_GLOB_FORMAT(x)                     (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_FORMAT_SHIFT)) & CPI_CFG_GLOB_FORMAT_MASK)
#define CPI_CFG_GLOB_SHIFT_MASK                    (0x7800U)
#define CPI_CFG_GLOB_SHIFT_SHIFT                   (11U)
#define CPI_CFG_GLOB_SHIFT(x)                      (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_SHIFT_SHIFT)) & CPI_CFG_GLOB_SHIFT_MASK)
#define CPI_CFG_GLOB_EN_MASK                       (0x80000000U)
#define CPI_CFG_GLOB_EN_SHIFT                      (31U)
#define CPI_CFG_GLOB_EN(x)                         (((uint32_t)(((uint32_t)(x)) << CPI_CFG_GLOB_EN_SHIFT)) & CPI_CFG_GLOB_EN_MASK)

/*! @name CFG_LL - CPI lower left comer configuration register */
#define CPI_CFG_LL_FRAMESLICE_LLX_MASK             (0xFFFFU)
#define CPI_CFG_LL_FRAMESLICE_LLX_SHIFT            (0U)
#define CPI_CFG_LL_FRAMESLICE_LLX(x)               (((uint32_t)(((uint32_t)(x)) << CPI_CFG_LL_FRAMESLICE_LLX_SHIFT)) & CPI_CFG_LL_FRAMESLICE_LLX_MASK)
#define CPI_CFG_LL_FRAMESLICE_LLY_MASK             (0xFFFF0000U)
#define CPI_CFG_LL_FRAMESLICE_LLY_SHIFT            (16U)
#define CPI_CFG_LL_FRAMESLICE_LLY(x)               (((uint32_t)(((uint32_t)(x)) << CPI_CFG_LL_FRAMESLICE_LLY_SHIFT)) & CPI_CFG_LL_FRAMESLICE_LLY_MASK)

/*! @name CFG_UR - CPI upper right comer configuration register */
#define CPI_CFG_UR_FRAMESLICE_URX_MASK             (0xFFFFU)
#define CPI_CFG_UR_FRAMESLICE_URX_SHIFT            (0U)
#define CPI_CFG_UR_FRAMESLICE_URX(x)               (((uint32_t)(((uint32_t)(x)) << CPI_CFG_UR_FRAMESLICE_URX_SHIFT)) & CPI_CFG_UR_FRAMESLICE_URX_MASK)
#define CPI_CFG_UR_FRAMESLICE_URY_MASK             (0xFFFF0000U)
#define CPI_CFG_UR_FRAMESLICE_URY_SHIFT            (16U)
#define CPI_CFG_UR_FRAMESLICE_URY(x)               (((uint32_t)(((uint32_t)(x)) << CPI_CFG_UR_FRAMESLICE_URY_SHIFT)) & CPI_CFG_UR_FRAMESLICE_URY_MASK)

/*! @name CFG_SIZE - CPI horizontal resolution configuration register */
#define CPI_CFG_SIZE_MASK                          (0xFFFF0000U)
#define CPI_CFG_SIZE_SHIFT                         (16U)
#define CPI_CFG_SIZE(x)                            (((uint32_t)(((uint32_t)(x)) << CPI_CFG_SIZE_SHIFT)) & CPI_CFG_SIZE_MASK)

/*! @name CFG_FILTER - CPI RGB coefficients  configuration register */
#define CPI_CFG_FILTER_B_COEFF_MASK                (0xFFU)
#define CPI_CFG_FILTER_B_COEFF_SHIFT               (0U)
#define CPI_CFG_FILTER_B_COEFF(x)                  (((uint32_t)(((uint32_t)(x)) << CPI_CFG_FILTER_B_COEFF_SHIFT)) & CPI_CFG_FILTER_B_COEFF_MASK)
#define CPI_CFG_FILTER_G_COEFF_MASK                (0xFF00U)
#define CPI_CFG_FILTER_G_COEFF_SHIFT               (8U)
#define CPI_CFG_FILTER_G_COEFF(x)                  (((uint32_t)(((uint32_t)(x)) << CPI_CFG_FILTER_G_COEFF_SHIFT)) & CPI_CFG_FILTER_G_COEFF_MASK)
#define CPI_CFG_FILTER_R_COEFF_MASK                (0xFF0000U)
#define CPI_CFG_FILTER_R_COEFF_SHIFT               (16U)
#define CPI_CFG_FILTER_R_COEFF(x)                  (((uint32_t)(((uint32_t)(x)) << CPI_CFG_FILTER_R_COEFF_SHIFT)) & CPI_CFG_FILTER_R_COEFF_MASK)

/*! @name VSYNC_POLARITY - CPI VSYNC Polarity register */
#define CPI_VSYNC_POLARITY_MASK                    (0x1U)
#define CPI_VSYNC_POLARITY_SHIFT                   (0U)
#define CPI_VSYNC_POLARITY(x)                      (((uint32_t)(((uint32_t)(x)) << CPI_VSYNC_POLARITY_SHIFT)) & CPI_VSYNC_POLARITY_MASK)

/*!
 * @}
 */ /* end of group CPI_Register_Masks */


/* CPI - Peripheral instance base addresses */
/** Peripheral CPI base address */
#define CPI_BASE                                 (UDMA_BASE + PER_ID_CPI * 128U)
/** Peripheral CPI base pointer */
#define CPI                                      ((CPI_Type *)CPI_BASE)
/** Array initializer of CPI peripheral base addresses */
#define CPI_BASE_ADDRS                           { CPI_BASE }
/** Array initializer of CPI peripheral base pointers */
#define CPI_BASE_PTRS                            { CPI }
/** Interrupt vectors for the CPI peripheral type */
#define CPI_RX_IRQS                              { CPI_RX_IRQn }

/*!
 * @}
 */ /* end of group CPI_Peripheral_Access_Layer */


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
  __IO  uint32_t RX_ENABLE;       /**< CSI2 RX Enable register, offset: 0x20 */
  __IO  uint32_t CLK_DIV;         /**< CSI2 Configuration register, offset: 0x24 */
  __IO  uint32_t SLICE_ENABLE;    /**< CSI2 Slice enable register, offset: 0x28 */
  __IO  uint32_t SLICE_ROWLEN;    /**< CSI2 Slice row length register, offset: 0x2C */
  __IO  uint32_t SLICE0_LXY;      /**< CSI2 Slice 0 left point register, offset: 0x30 */
  __IO  uint32_t SLICE0_RXY;      /**< CSI2 Slice 0 rigth point register, offset: 0x34 */
  __IO  uint32_t SLICE1_LXY;      /**< CSI2 Slice 1 left point register, offset: 0x38 */
  __IO  uint32_t SLICE1_RXY;      /**< CSI2 Slice 1 rigth point register, offset: 0x3C */

} CSI2_Type;

/* ----------------------------------------------------------------------------
   -- CSI2 Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CSI2_Register_Masks CSI2 Register Masks
 * @{
 */
/*! @name RX_ENABLE - CSI2 RX Enable register */
#define CSI2_RX_ENABLE_CH0_MASK        (0x1U)
#define CSI2_RX_ENABLE_CH0_SHIFT       (0U)
#define CSI2_RX_ENABLE_CH0(x)          (((uint32_t)(((uint32_t)(x)) /* << CSI2_RX_ENABLE_CH0_SHIFT */)) & CSI2_RX_ENABLE_CH0_MASK)
#define CSI2_RX_ENABLE_CH1_MASK        (0x2U)
#define CSI2_RX_ENABLE_CH1_SHIFT       (1U)
#define CSI2_RX_ENABLE_CH1(x)          (((uint32_t)(((uint32_t)(x)) << CSI2_RX_ENABLE_CH1_SHIFT)) & CSI2_RX_ENABLE_CH1_MASK)

/*! @name CLK_DIV - CSI2 SETUP register */
#define CSI2_CLK_DIV_CCI_MASK            (0xFFU)
#define CSI2_CLK_DIV_CCI_SHIFT           (0U)
#define CSI2_CLK_DIV_CCI(x)              (((uint32_t)(((uint32_t)(x)) /* << CSI2_CLK_DIV_CCI_SHIFT */)) & CSI2_CLK_DIV_CCI_MASK)
#define CSI2_CLK_DIV_PIXEL_MASK          (0xFF00U)
#define CSI2_CLK_DIV_PIXEL_SHIFT         (8U)
#define CSI2_CLK_DIV_PIXEL(x)            (((uint32_t)(((uint32_t)(x)) << CSI2_CLK_DIV_PIXEL_SHIFT)) & CSI2_CLK_DIV_PIXEL_MASK)
#define CSI2_CLK_DIV_APB_MASK            (0xFF0000U)
#define CSI2_CLK_DIV_APB_SHIFT           (16U)
#define CSI2_CLK_DIV_APB(x)              (((uint32_t)(((uint32_t)(x)) << CSI2_CLK_DIV_APB_SHIFT)) & CSI2_CLK_DIV_APB_MASK)

/*! @name SLICE_ENABLE - CSI2 Slice Enable register */
#define CSI2_SLICE_ENABLE_CH0_MASK       (0x1U)
#define CSI2_SLICE_ENABLE_CH0_SHIFT      (0U)
#define CSI2_SLICE_ENABLE_CH0(x)         (((uint32_t)(((uint32_t)(x)) /* << CSI2_SLICE_ENABLE_CH0_SHIFT */)) & CSI2_SLICE_ENABLE_CH0_MASK)
#define CSI2_SLICE_ENABLE_CH1_MASK       (0x2U)
#define CSI2_SLICE_ENABLE_CH1_SHIFT      (1U)
#define CSI2_SLICE_ENABLE_CH1(x)         (((uint32_t)(((uint32_t)(x)) << CSI2_SLICE_ENABLE_CH1_SHIFT)) & CSI2_SLICE_ENABLE_CH1_MASK)

/*!
 * @}
 */ /* end of group CSI2_Register_Masks */


/* CSI2 - Peripheral instance base addresses */
/** Peripheral CSI2 base address */
#define CSI2_BASE                                 (UDMA_BASE + PER_ID_CSI2 * 128U)
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
#define MRAM_STATUS_ECC_ERROR_MASK             (0x30U)
#define MRAM_STATUS_ECC_ERROR_SHIFT            (4U)
#define READ_MRAM_STATUS_ECC_ERROR(x)          (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_ECC_ERROR_MASK)) >> MRAM_STATUS_ECC_ERROR_SHIFT)

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
#define MRAM_BASE                                 (UDMA_BASE + PER_ID_MRAM * 128U)
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
#define FILTER_BASE0                                (UDMA_BASE + PER_ID_FILTER * 128U)
/** Peripheral FILTER base pointer */
#define FILTER0                                     ((FILTER_Type *)FILTER_BASE0)
/** Array initializer of FILTER peripheral base addresses */
#define FILTER_BASE_ADDRS                           { FILTER_BASE0 }
/** Array initializer of FILTER peripheral base pointers */
#define FILTER_BASE_PTRS                            { FILTER0 }
/** Interrupt vectors for the FILTER peripheral type */
#define FILTER_RX_IRQS                              { FILTER_RX_IRQn }
#define FILTER_TX_IRQS                              { FILTER_TX_IRQn }

/*!
 * @}
 */ /* end of group FILTER_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- TIMESTAMP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup TIMESTAMP_Peripheral_Access_Layer TIMESTAMP Peripheral Access Layer
 * @{
 */

/** TIMESTAMP - Register Layout Typedef */
typedef struct {
  UDMA_Type     UDMA_TIMESTAMP;                   /**< TIMESTAMP UDMA general register, offset: 0x0 */
  __O   uint32_t CMD;                             /**< TIMESTAMP command register, offset: 0x20 */
  __IO  uint32_t CNT;                             /**< TIMESTAMP setup counter register, offset: 0x24 */
  __IO  uint32_t CH01;                            /**< TIMESTAMP setup channel 0 - 1 register, offset: 0x28 */
  __IO  uint32_t CH23;                            /**< TIMESTAMP setup channel 2 - 3 register, offset: 0x2C */
  __IO  uint32_t CH45;                            /**< TIMESTAMP setup channel 4 - 5 register, offset: 0x30 */
  __IO  uint32_t CH67;                            /**< TIMESTAMP setup channel 6 - 7 register, offset: 0x34 */
} TIMESTAMP_Type;

/* ----------------------------------------------------------------------------
   -- TIMESTAMP Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup TIMESTAMP_Register_Masks TIMESTAMP Register Masks
 * @{
 */
/*! @name CMD - TIMESTAMP command register */
#define TIMESTAMP_CMD_CLR_MASK                     (0x1)
#define TIMESTAMP_CMD_CLR_SHIFT                    (0U)
#define TIMESTAMP_CMD_CLR(x)                       (((uint32_t)(((uint32_t)(x)) /* << TIMESTAMP_CMD_CLR_SHIFT */)) & TIMESTAMP_CMD_CLR_MASK)
#define TIMESTAMP_CMD_STOP_MASK                    (0x2)
#define TIMESTAMP_CMD_STOP_SHIFT                   (1U)
#define TIMESTAMP_CMD_STOP(x)                      (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_CMD_STOP_SHIFT)) & TIMESTAMP_CMD_STOP_MASK)

/*! @name CNT - TIMESTAMP setup counter register */
#define TIMESTAMP_CNT_EXT_SEL_MASK                 (0x3F)
#define TIMESTAMP_CNT_EXT_SEL_SHIFT                (0U)
#define TIMESTAMP_CNT_EXT_SEL(x)                   (((uint32_t)(((uint32_t)(x)) /* << TIMESTAMP_CNT_EXT_SEL_SHIFT */)) & TIMESTAMP_CNT_EXT_SEL_MASK)
#define TIMESTAMP_CNT_EXT_TYPE_MASK                (0xC0)
#define TIMESTAMP_CNT_EXT_TYPE_SHIFT               (6U)
#define TIMESTAMP_CNT_EXT_TYPE(x)                  (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_CNT_EXT_TYPE_SHIFT)) & TIMESTAMP_CNT_EXT_TYPE_MASK)
#define TIMESTAMP_CNT_EXT_EN_MASK                  (0x100)
#define TIMESTAMP_CNT_EXT_EN_SHIFT                 (8U)
#define TIMESTAMP_CNT_EXT_EN(x)                    (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_CNT_EXT_EN_SHIFT)) & TIMESTAMP_CNT_EXT_EN_MASK)

/*! @name CH - TIMESTAMP setup channel 0-1 register */
#define TIMESTAMP_FIRST_CH_SEL_MASK                (0x3F)
#define TIMESTAMP_FIRST_CH_SEL_SHIFT               (0U)
#define TIMESTAMP_FIRST_CH_SEL(x)                  (((uint32_t)(((uint32_t)(x)) /* << TIMESTAMP_FIRST_CH_SEL_SHIFT */)) & TIMESTAMP_FIRST_CH_SEL_MASK)
#define TIMESTAMP_FIRST_CH_TYPE_MASK               (0xC0)
#define TIMESTAMP_FIRST_CH_TYPE_SHIFT              (6U)
#define TIMESTAMP_FIRST_CH_TYPE(x)                 (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_FIRST_CH_TYPE_SHIFT)) & TIMESTAMP_FIRST_CH_TYPE_MASK)
#define TIMESTAMP_FIRST_CH_EN_MASK                 (0x100)
#define TIMESTAMP_FIRST_CH_EN_SHIFT                (8U)
#define TIMESTAMP_FIRST_CH_EN(x)                   (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_FIRST_CH_EN_SHIFT)) & TIMESTAMP_FIRST_CH_EN_MASK)
#define TIMESTAMP_SECOND_CH_SEL_MASK               (0x3F0000)
#define TIMESTAMP_SECOND_CH_SEL_SHIFT              (16U)
#define TIMESTAMP_SECOND_CH_SEL(x)                 (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_SECOND_CH_SEL_SHIFT)) & TIMESTAMP_SECOND_CH_SEL_MASK)
#define TIMESTAMP_SECOND_CH_TYPE_MASK              (0xC00000)
#define TIMESTAMP_SECOND_CH_TYPE_SHIFT             (22U)
#define TIMESTAMP_SECOND_CH_TYPE(x)                (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_SECOND_CH_TYPE_SHIFT)) & TIMESTAMP_SECOND_CH_TYPE_MASK)
#define TIMESTAMP_SECOND_CH_EN_MASK                (0x1000000)
#define TIMESTAMP_SECOND_CH_EN_SHIFT               (24U)
#define TIMESTAMP_SECOND_CH_EN(x)                  (((uint32_t)(((uint32_t)(x)) << TIMESTAMP_SECOND_CH_EN_SHIFT)) & TIMESTAMP_SECOND_CH_EN_MASK)

/*!
 * @}
 */ /* end of group TIMESTAMP_Register_Masks */


/* TIMESTAMP - Peripheral instance base addresses */
/** Peripheral TIMESTAMP base address */
#define TIMESTAMP_BASE                                 (UDMA_BASE + PER_ID_TIMESTAMP * 128U)
/** Peripheral TIMESTAMP base pointer */
#define TIMESTAMP                                      ((TIMESTAMP_Type *)TIMESTAMP_BASE)
/** Array initializer of TIMESTAMP peripheral base addresses */
#define TIMESTAMP_BASE_ADDRS                           { TIMESTAMP_BASE }
/** Array initializer of TIMESTAMP peripheral base pointers */
#define TIMESTAMP_BASE_PTRS                            { TIMESTAMP }
/** Interrupt vectors for the TIMESTAMP peripheral type */
#define TIMESTAMP_RX_IRQS                              { TIMESTAMP_RX_IRQn }
#define TIMESTAMP_TX_IRQS                              { TIMESTAMP_TX_IRQn}

/*!
 * @}
 */ /* end of group TIMESTAMP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SOC_CTRL  Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOC_CTRL_Peripheral_Access_Layer SOC_CTRL Peripheral Access Layer
 * @{
 */

/** SOC_CTRL - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t INFO;                /**< SOC_CTRL INFO register, offset: 0x00 */
  __IO  uint32_t FC_BOOT_ADDR;        /**< SOC_CTRL FC Boot Address register, offset: 0x04 */
  __O   uint32_t FC_FETCH_EN;         /**< SOC_CTRL FC Fetch Enable register, offset: 0x08 */
  __IO  uint32_t CLUSTER_ISO;         /**< SOC_CTRL Cluster Isolate register, offset: 0x0C */
  __IO  uint32_t _reserved0[23];      /**< reserved, offset: 0x10 */
  __IO  uint32_t CLUSTER_BUSY;        /**< SOC_CTRL Busy register, offset: 0x6C */
  __IO  uint32_t CLUSTER_BYPASS;      /**< SOC_CTRL Cluster PMU bypass register, offset: 0x70 */
  __IO  uint32_t JTAG;                /**< SOC_CTRL Jtag register, offset: 0x74 */
  __IO  uint32_t L2_SLEEP;            /**< SOC_CTRL L2 memory sleep register, offset: 0x78 */
  __IO  uint32_t SLEEP_CTRL;          /**< SOC_CTRL Sleep control register, offset: 0x7C */
  __IO  uint32_t I3C_CTRL;            /**< SOC_CTRL I3C clock gating and frequecy devide control register, offset: 0x80 */
  __IO  uint32_t _reserved1[7];       /**< reserved, offset: 0x84 */
  __IO  uint32_t CORE_STATUS;         /**< SOC_CTRL Sleep control register, offset: 0xA0 */
  __IO  uint32_t _reserved2[7];       /**< reserved, offset: 0xA4 */
  __I   uint32_t CORE_STATUS_EOC;     /**< SOC_CTRL Core Status EOC register, offset: 0xC0 */
  __IO  uint32_t BOOT_SEL;            /**< SOC_CTRL Boot select register, offset: 0xC4 */
  __IO  uint32_t WD_RST;              /**< SOC_CTRL Watch dog reset, offset: 0xC8 */
  __IO  uint32_t _reserved3[1];       /**< reserved, offset: 0xCC */
  __IO  uint32_t CLK_SEL;             /**< SOC_CTRL Clock select register, offset: 0xD0 */
  __IO  uint32_t CLK_DIV_SOC;         /**< SOC_CTRL Clock SOC divider register, offset: 0xD4 */
  __IO  uint32_t CLK_DIV_CLU;         /**< SOC_CTRL Clock Cluster divider register, offset: 0xD8 */
  __IO  uint32_t CLK_DIV_PER;         /**< SOC_CTRL Clock Peripheral divider register, offset: 0xDC */
  __IO  uint32_t SUPERVISOR_DBG;      /**< SOC_CTRL Sleep control register, offset: 0xE0 */
  __IO  uint32_t RWM_GRP0_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 7:0, offset: 0xE4 */
  __IO  uint32_t RWM_GRP1_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 15:8, offset: 0xE8*/
  __IO  uint32_t RWM_GRP2_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 23:16, offset: 0xEC */
  __IO  uint32_t RWM_GRP3_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 31:24, offset: 0xF0 */
  __IO  uint32_t RWM_GRP4_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 39:32, offset: 0xF4 */
  __IO  uint32_t RWM_GRP5_DBG;        /**< SOC_CTRL RWM for L2 Interlevead memories 47:40, offset: 0xF8 */
  __IO  uint32_t RWM_GRP6_DBG;        /**< SOC_CTRL RWM for L2 Private memories 3:0, offset: 0xFC */

} SOC_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- SOC_CTRL Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SOC_CTRL_Register_Masks SOC_CTRL Register Masks
 * @{
 */
/*! @name INFO - SOC_CTRL information register */
#define SOC_CTRL_INFO_CLUSTER_NB_MASK          (0xFFFFU)
#define SOC_CTRL_INFO_CLUSTER_NB_SHIFT         (0U)
#define SOC_CTRL_INFO_CLUSTER_NB(x)            (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_INFO_CLUSTER_NB_SHIFT*/)) & SOC_CTRL_INFO_CLUSTER_NB_MASK)
#define SOC_CTRL_INFO_CORE_NB_MASK             (0xFFFF0000U)
#define SOC_CTRL_INFO_CORE_NB_SHIFT            (16U)
#define SOC_CTRL_INFO_CORE_NB(x)               (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_INFO_CORE_NB_SHIFT)) & SOC_CTRL_INFO_CORE_NB_MASK)

/*! @name CLUSTER_BYPASS - SOC_CTRL bypass register */
#define SOC_CTRL_CLUSTER_BYPASS_ENABLE_MASK           (0x1U)
#define SOC_CTRL_CLUSTER_BYPASS_ENABLE_SHIFT          (0U)
#define SOC_CTRL_CLUSTER_BYPASS_ENABLE(x)             (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLUSTER_BYPASS_ENABLE_SHIFT*/)) & SOC_CTRL_CLUSTER_BYPASS_ENABLE_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_POWER_MASK            (0x2U)
#define SOC_CTRL_CLUSTER_BYPASS_POWER_SHIFT           (1U)
#define SOC_CTRL_CLUSTER_BYPASS_POWER(x)              (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_POWER_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_POWER_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_CLOCK_GATING_MASK     (0x4U)
#define SOC_CTRL_CLUSTER_BYPASS_CLOCK_GATING_SHIFT    (2U)
#define SOC_CTRL_CLUSTER_BYPASS_CLOCK_GATING(x)       (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_CLOCK_GATING_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_CLOCK_GATING_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_FLL_OFF_MASK          (0x8U)
#define SOC_CTRL_CLUSTER_BYPASS_FLL_OFF_SHIFT         (3U)
#define SOC_CTRL_CLUSTER_BYPASS_FLL_OFF(x)            (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_FLL_OFF_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_FLL_OFF_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_FLL_RETENTIVE_MASK    (0x10U)
#define SOC_CTRL_CLUSTER_BYPASS_FLL_RETENTIVE_SHIFT   (4U)
#define SOC_CTRL_CLUSTER_BYPASS_FLL_RETENTIVE(x)      (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_FLL_RETENTIVE_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_FLL_RETENTIVE_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_RESET_MASK            (0x20U)
#define SOC_CTRL_CLUSTER_BYPASS_RESET_SHIFT           (5U)
#define SOC_CTRL_CLUSTER_BYPASS_RESET(x)              (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_RESET_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_RESET_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_ISOLATE_MASK          (0x40U)
#define SOC_CTRL_CLUSTER_BYPASS_ISOLATE_SHIFT         (6U)
#define SOC_CTRL_CLUSTER_BYPASS_ISOLATE(x)            (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_ISOLATE_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_ISOLATE_MASK)
#define READ_SOC_CTRL_CLUSTER_BYPASS_ISOLATE(x)       (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_CLUSTER_BYPASS_ISOLATE_MASK)) >> SOC_CTRL_CLUSTER_BYPASS_ISOLATE_SHIFT)

#define SOC_CTRL_CLUSTER_BYPASS_CLUSTER_RETENTIVE_MASK     (0x80U)
#define SOC_CTRL_CLUSTER_BYPASS_CLUSTER_RETENTIVE_SHIFT    (7U)
#define SOC_CTRL_CLUSTER_BYPASS_CLUSTER_RETENTIVE(x)       (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_CLUSTER_RETENTIVE_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_CLUSTER_RETENTIVE_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_DBG1_MASK            (0x4000U)
#define SOC_CTRL_CLUSTER_BYPASS_DBG1_SHIFT           (14U)
#define SOC_CTRL_CLUSTER_BYPASS_DBG1(x)              (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_DBG1_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_DBG1_MASK)

#define SOC_CTRL_CLUSTER_BYPASS_DBG2_MASK            (0x8000U)
#define SOC_CTRL_CLUSTER_BYPASS_DBG2_SHIFT           (15U)
#define SOC_CTRL_CLUSTER_BYPASS_DBG2(x)              (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CLUSTER_BYPASS_DBG2_SHIFT)) & SOC_CTRL_CLUSTER_BYPASS_DBG2_MASK)
#define READ_SOC_CTRL_CLUSTER_BYPASS_DBG2(x)         (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_CLUSTER_BYPASS_DBG2_MASK)) >> SOC_CTRL_CLUSTER_BYPASS_DBG2_SHIFT)

/*! @name FC_FETCH_EN - SOC_CTRL FC Fetch Enable register */
#define SOC_CTRL_FC_FETCH_EN_MASK                     (0x1U)
#define SOC_CTRL_FC_FETCH_EN_SHIFT                    (0U)
#define SOC_CTRL_FC_FETCH_EN(x)                       (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_FC_FETCH_EN_SHIFT */)) & SOC_CTRL_FC_FETCH_EN_MASK)

/*! @name CLUSTER_ISO - SOC_CTRL Cluster isolation register */
#define SOC_CTRL_CLUSTER_ISO_MASK                     (0x1U)
#define SOC_CTRL_CLUSTER_ISO_SHIFT                    (0U)
#define SOC_CTRL_CLUSTER_ISO(x)                       (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLUSTER_ISO_SHIFT */)) & SOC_CTRL_CLUSTER_ISO_MASK)

/*! @name JTAG_REG - SOC_CTRL JTAG register */
#define SOC_CTRL_JTAG_REG_MASK                        (0x3U)
#define SOC_CTRL_JTAG_REG_SHIFT                       (0U)
#define SOC_CTRL_JTAG_REG(x)                          (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_JTAG_REG_SHIFT */)) & SOC_CTRL_JTAG_REG_MASK)

/*! @name L2_SLEEP - SOC_CTRL L2 memory sleep register */
#define SOC_CTRL_L2_SLEEP_MASK                        (0x1U)
#define SOC_CTRL_L2_SLEEP_SHIFT                       (0U)
#define SOC_CTRL_L2_SLEEP(x)                          (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_L2_SLEEP_SHIFT */)) & SOC_CTRL_L2_SLEEP_MASK)

/*! @name CLK_SEL - SOC_CTRL Clock Select register */
#define SOC_CTRL_CLK_SEL_SOC_FLL1_FLL2_MASK           (0x1U)
#define SOC_CTRL_CLK_SEL_SOC_FLL1_FLL2_SHIFT          (0U)
#define SOC_CTRL_CLK_SEL_SOC_FLL1_FLL2(x)             (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_SEL_SOC_FLL1_FLL2_SHIFT */)) & SOC_CTRL_CLK_SEL_SOC_FLL1_FLL2_MASK)
#define SOC_CTRL_CLK_SEL_CLUSTER_FLL1_FLL2_MASK       (0x2U)
#define SOC_CTRL_CLK_SEL_CLUSTER_FLL1_FLL2_SHIFT      (1U)
#define SOC_CTRL_CLK_SEL_CLUSTER_FLL1_FLL2(x)         (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_SEL_CLUSTER_FLL1_FLL2_SHIFT */)) & SOC_CTRL_CLK_SEL_CLUSTER_FLL1_FLL2_MASK)
#define SOC_CTRL_CLK_SEL_CLUSTER_CLU1_FLL2_MASK       (0x4U)
#define SOC_CTRL_CLK_SEL_CLUSTER_CLU1_FLL2_SHIFT      (2U)
#define SOC_CTRL_CLK_SEL_CLUSTER_CLU1_FLL2(x)         (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_SEL_CLUSTER_CLU1_FLL2_SHIFT */)) & SOC_CTRL_CLK_SEL_CLUSTER_CLU1_FLL2_MASK)

/*! @name CLK_DIV_SOC - SOC_CTRL Clock divider SOC register */
#define SOC_CTRL_CLK_DIV_SOC_MASK                     (0x7U)
#define SOC_CTRL_CLK_DIV_SOC_SHIFT                    (0U)
#define SOC_CTRL_CLK_DIV_SOC(x)                       (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_DIV_SOC_SHIFT */)) & SOC_CTRL_CLK_DIV_SOC_MASK)

/*! @name CLK_DIV_CLU - SOC_CTRL Clock divider Cluster register */
#define SOC_CTRL_CLK_DIV_CLU_MASK                     (0x7U)
#define SOC_CTRL_CLK_DIV_CLU_SHIFT                    (0U)
#define SOC_CTRL_CLK_DIV_CLU(x)                       (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_DIV_CLU_SHIFT */)) & SOC_CTRL_CLK_DIV_CLU_MASK)

/*! @name CLK_DIV_PER - SOC_CTRL Clock divider Peripheral register */
#define SOC_CTRL_CLK_DIV_PER_MASK                     (0x7U)
#define SOC_CTRL_CLK_DIV_PER_SHIFT                    (0U)
#define SOC_CTRL_CLK_DIV_PER(x)                       (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_CLK_DIV_PER_SHIFT */)) & SOC_CTRL_CLK_DIV_PER_MASK)

/*! @name SUPERVISOR_DBG - SOC_CTRL Supervisor debug register */
#define SOC_CTRL_SUPERVISOR_DBG_MASK                  (0x1U)
#define SOC_CTRL_SUPERVISOR_DBG_SHIFT                 (0U)
#define SOC_CTRL_SUPERVISOR_DBG(x)                    (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_SUPERVISOR_DBG_SHIFT */)) & SOC_CTRL_SUPERVISOR_DBG_MASK)

/*! @name JTAG - SOC_CTRL jtag control register */
#define SOC_CTRL_JTAG_INT_SYNC_MASK          (0x1U)
#define SOC_CTRL_JTAG_INT_SYNC_SHIFT         (0U)
#define SOC_CTRL_JTAG_INT_SYNC(x)            (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_JTAG_INT_SYNC_SHIFT*/)) & SOC_CTRL_JTAG_INT_SYNC_MASK)
#define READ_SOC_CTRL_JTAG_INT_SYNC(x)       (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_JTAG_INT_SYNC_MASK)) /* >> SOC_CTRL_JTAG_INT_SYNC_SHIFT*/)

#define SOC_CTRL_JTAG_INT_BT_MD_MASK         (0xEU)
#define SOC_CTRL_JTAG_INT_BT_MD_SHIFT        (1U)
#define SOC_CTRL_JTAG_INT_BT_MD(x)           (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_JTAG_INT_BT_MD_SHIFT)) & SOC_CTRL_JTAG_INT_BT_MD_MASK)
#define READ_SOC_CTRL_JTAG_INT_BT_MD(x)      (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_JTAG_INT_BT_MD_MASK)) >> SOC_CTRL_JTAG_INT_BT_MD_SHIFT)

#define SOC_CTRL_JTAG_EXT_SYNC_MASK          (0x100U)
#define SOC_CTRL_JTAG_EXT_SYNC_SHIFT         (8U)
#define SOC_CTRL_JTAG_EXT_SYNC(x)            (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_JTAG_EXT_SYNC_SHIFT)) & SOC_CTRL_JTAG_EXT_SYNC_MASK)
#define READ_SOC_CTRL_JTAG_EXT_SYNC(x)       (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_JTAG_EXT_SYNC_MASK)) >> SOC_CTRL_JTAG_EXT_SYNC_SHIFT)

#define SOC_CTRL_JTAG_EXT_BT_MD_MASK         (0xE00U)
#define SOC_CTRL_JTAG_EXT_BT_MD_SHIFT        (9U)
#define SOC_CTRL_JTAG_EXT_BT_MD(x)           (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_JTAG_EXT_BT_MD_SHIFT)) & SOC_CTRL_JTAG_EXT_BT_MD_MASK)
#define READ_SOC_CTRL_JTAG_EXT_BT_MD(x)      (((uint32_t)(((uint32_t)(x)) & SOC_CTRL_JTAG_EXT_BT_MD_MASK)) >> SOC_CTRL_JTAG_EXT_BT_MD_SHIFT)

/*! @name I3C_CTRL - SOC_CTRL I3C clock gating and frequecy devide control register */
#define SOC_CTRL_I3C_CTRL_EN0_MASK                    (0x1U)
#define SOC_CTRL_I3C_CTRL_EN0_SHIFT                   (0U)
#define SOC_CTRL_I3C_CTRL_EN0(x)                      (((uint32_t)(((uint32_t)(x)) /* << SOC_CTRL_I3C_CTRL_EN0_SHIFT */)) & SOC_CTRL_I3C_CTRL_EN0_MASK)
#define SOC_CTRL_I3C_CTRL_EN1_MASK                    (0x2U)
#define SOC_CTRL_I3C_CTRL_EN1_SHIFT                   (1U)
#define SOC_CTRL_I3C_CTRL_EN1(x)                      (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_I3C_CTRL_EN1_SHIFT)) & SOC_CTRL_I3C_CTRL_EN1_MASK)
#define SOC_CTRL_I3C_CTRL_DIV_MASK                    (0xFF00U)
#define SOC_CTRL_I3C_CTRL_DIV_SHIFT                   (8U)
#define SOC_CTRL_I3C_CTRL_DIV(x)                      (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_I3C_CTRL_DIV_SHIFT)) & SOC_CTRL_I3C_CTRL_DIV_MASK)

/*! @name CORE_STATUS - SOC_CTRL Core Status register -records end of core */
#define SOC_CTRL_CORE_STATUS_EOC_MASK                 (0x80000000U)
#define SOC_CTRL_CORE_STATUS_EOC_SHIFT                (31U)
#define SOC_CTRL_CORE_STATUS_EOC(x)                   (((uint32_t)(((uint32_t)(x)) << SOC_CTRL_CORE_STATUS_EOC_SHIFT)) & SOC_CTRL_CORE_STATUS_EOC_MASK)

/*!
 * @}
 */ /* end of group SOC_CTRL_Register_Masks */


/* SOC_CTRL - Peripheral instance base addresses */
/** Peripheral SOC_CTRL base address */
#define SOC_CTRL_BASE                                (SOC_PERI_BASE + 0x4000u)
/** Peripheral SOC_CTRL base pointer */
#define SOC_CTRL                                     ((SOC_CTRL_Type *)SOC_CTRL_BASE)
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
#define PWM_CTRL_BASE                                (SOC_PERI_BASE + 0x05100u)
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
  __IO  uint32_t CH_LUT[4];        /**< TIMER Channles' LUT register, offset: 0x1c */
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
#define PWM0_BASE                               (SOC_PERI_BASE + 0x05000u)
/** Peripheral PWM base pointer */
#define PWM0                                    ((PWM_Type *)PWM0_BASE)
/** Peripheral PWM base address */
#define PWM1_BASE                               (PWM0_BASE + 0x40u)
/** Peripheral PWM base pointer */
#define PWM1                                    ((PWM_Type *)PWM1_BASE)
/** Peripheral PWM base address */
#define PWM2_BASE                               (PWM1_BASE + 0x40u)
/** Peripheral PWM base pointer */
#define PWM2                                    ((PWM_Type *)PWM2_BASE)
/** Peripheral PWM base address */
#define PWM3_BASE                               (PWM2_BASE + 0x40u)
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
#define SOC_EVENTS_REG_NUM              0x08
/*!
 * @}
 */ /* end of group SOCEU_Register_Masks */


/* SOCEU - Peripheral instance base addresses */
/** Peripheral SOCEU base address */
#define SOCEU_BASE                               (SOC_PERI_BASE + 0x06000u)
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
#define  PICL_ICU4_ADDR                           0x06
#define  PICL_ICU5_ADDR                           0x07
#define  PICL_ICU6_ADDR                           0x08
#define  PICL_ICU7_ADDR                           0x09
#define  PICL_ICU8_ADDR                           0x0A
#define  PICL_ICU9_ADDR                           0x0B
#define  PICL_ICU10_ADDR                          0x0C
#define  PICL_ICU11_ADDR                          0x0D
#define  PICL_DMU0_ADDR                           0x21
#define  PICL_DMU1_ADDR                           0x22

#define  SAFE_BASE_ADDR                           PICL_ICU0_ADDR
#define  MRAM_IO_BASE_ADDR                        PICL_ICU1_ADDR
#define  MRAM_CORE_BASE_ADDR                      PICL_ICU2_ADDR
#define  IO_LS_BASE_ADDR                          PICL_ICU3_ADDR
#define  SMART_WAKE_BASE_ADDR                     PICL_ICU4_ADDR
#define  SOC_BASE_ADDR                            PICL_ICU5_ADDR
#define  CLUSTER_BASE_ADDR                        PICL_ICU6_ADDR
#define  IO_HS_BASE_ADDR                          PICL_ICU7_ADDR
#define  CSI2_BASE_ADDR                           PICL_ICU8_ADDR
#define  VREF12_BASE_ADDR                         PICL_ICU9_ADDR
#define  VREF06_BASE_ADDR                         PICL_ICU10_ADDR
#define  OPMLP_BASE_ADDR                          PICL_ICU11_ADDR

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
#define  PMU_ICU_CTRL(base_addr)   (PMU_DLC_PICL_CHIP_SEL_ADDR(base_addr) | PMU_DLC_PICL_REG_ADDR(0x00))

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
#define RTC_APB_BASE                               (SOC_PERI_BASE + 0x08000u)
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
#define FC_ICACHE_BASE                               (SOC_PERI_BASE + 0x8800u)
/** Peripheral FC_ICACHE base pointer */
#define FC_ICACHE                                    ((FC_ICACHE_Type *)FC_ICACHE_BASE)
/** Array initializer of FC_ICACHE base addresses */
#define FC_ICACHE_BASE_ADDRS                         { FC_ICACHE_BASE }
/** Array initializer of FC_ICACHE base pointers */
#define FC_ICACHE_BASE_PTRS                          { FC_ICACHE }

/*!
 * @}
 */ /* end of group FC_ICACHE_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- FC_ITC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_ITC_Peripheral_Access_Layer FC_ITC Peripheral Access Layer
 * @{
 */

/** FC_ITC - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t MASK;           /**< FC_ITC Mask register, offset: 0x00 */
  __IO  uint32_t MASK_SET;       /**< FC_ITC Mask set register, offset: 0x04 */
  __IO  uint32_t MASK_CLR;       /**< FC_ITC Mask clean register, offset: 0x08 */
  __IO  uint32_t INT;            /**< FC_ITC Status register, offset: 0x0C */
  __IO  uint32_t INT_SET;        /**< FC_ITC Status set register, offset: 0x10 */
  __IO  uint32_t INT_CLR;        /**< FC_ITC Status clean register, offset: 0x14 */
  __IO  uint32_t ACK;            /**< FC_ITC ACK register, offset: 0x18 */
  __IO  uint32_t ACK_SET;        /**< FC_ITC ACK set register, offset: 0x1C */
  __IO  uint32_t ACK_CLR;        /**< FC_ITC ACK clean register, offset: 0x20 */
  __IO  uint32_t CURRENT_EVENT;  /**< FC_ITC CURRENT EVENT FIFO register, offset: 024 */
} FC_ITC_Type;

/* ----------------------------------------------------------------------------
   -- FC_ITC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FC_ITC_Register_Masks FC_ITC Register Masks
 * @{
 */
/*! @name INFO - FC_ITC information register */

/*!
 * @}
 */ /* end of group FC_ITC_Register_Masks */


/* FC_ITC - Peripheral instance base addresses */
/** Peripheral FC_ITC base address */
#define FC_ITC_BASE                                (SOC_PERI_BASE + 0x9000u)
/** Peripheral FC_ITC base pointer */
#define FC_ITC                                     ((FC_ITC_Type *)FC_ITC_BASE)
/** Array initializer of FC_ITC base addresses */
#define FC_ITC_BASE_ADDRS                          { FC_ITC_BASE }
/** Array initializer of FC_ITC base pointers */
#define FC_ITC_BASE_PTRS                           { FC_ITC }

/*!
 * @}
 */ /* end of group FC_ITC_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- I3C Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Peripheral_Access_Layer I3C Peripheral Access Layer
 * @{
 */

/** I3C - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t MST_CNTL_EN;           /**< I3C Master_cntrl_en register, offset: 0x00 */
  __IO  uint32_t CMD_TR_REQ_REG1;       /**< I3C md_tr_req_reg_1 register, offset: 0x04 */
  __IO  uint32_t CMD_TR_REQ_REG2;       /**< I3C Cmd_tr_req_reg_2, offset: 0x08 */
  __IO  uint32_t _reserved0;            /**< Non used registers, offser: 0x0C */
  __IO  uint32_t RESP;                  /**< I3C Response_reg, offset: 0x10*/
  __IO  uint32_t IBI_RESP;              /**< I3C Ibi_response_reg, offset: 0x14 */
  __IO  uint32_t IBI_DATA;              /**< I3C Ibi_data_reg, offset: 0x18 */
  __IO  uint32_t DATA_RX_FIFO;          /**< I3C Data_Rx_FIFO_reg, offset: 0x1C */
  __IO  uint32_t DATA_TX_FIFO;          /**< I3C Data_Tx_FIFO_reg, offset: 0x20 */
  __IO  uint32_t _reserved1[3];         /**< Non used registers, offser: 0x24 */
  __IO  uint32_t IRQ_STATUS;            /**< I3C Intr_sts_reg, offset: 0x30 */
  __IO  uint32_t _reserved2[3];         /**< Non used registers, offser: 0x34 */
  __IO  uint32_t TCAS_TIMER;            /**< I3C TCAS_timer_reg, offset: 0x40 */
  __IO  uint32_t TLOW_OD_TIMER;         /**< I3C TLOW_OD_timer_reg, offset: 0x44 */
  __IO  uint32_t THIGH_OD_TIMER;        /**< I3C Thigh_OD_timer_reg, offset: 0x48 */
  __IO  uint32_t TLOW_PP_TIMER;         /**< I3C TLOW_PP_timer_reg, offset: 0x4C */
  __IO  uint32_t THIGH_PP_TIMER;        /**< I3C Thigh_PP_timer_reg, offset: 0x50 */
  __IO  uint32_t TDS_TIMER_REG;         /**< I3C TDS_timer_reg, offset: 0x54 */
  __IO  uint32_t THD_PP_TIMER;          /**< I3C Thd_PP_timer_reg, offset: 0x58 */
  __IO  uint32_t TCBP_TIMER;            /**< I3C TCBP_timer_reg, offset: 0x5C */
  __IO  uint32_t TCBSR_TIMER;           /**< I3C TCBSR_timer_reg, offset: 0x60 */
  __IO  uint32_t THD_DDR_TIMER;         /**< I3C THD_DDR_timer_reg, offset: 0x64 */
  __IO  uint32_t BUS_FREE_TIMER;        /**< I3C Bus_free_timer_reg, offset: 0x68 */
  __IO  uint32_t BUS_AVAIL_TIMER;       /**< I3C Bus_avail_timer_reg, offset: 0x6C */
  __IO  uint32_t TIDLE_TIMER;           /**< I3C TIDLE_timer_reg, offset: 0x70 */
  __IO  uint32_t TSCO_TIMER;            /**< I3C Tsco_timer_reg, offset: 0x74 */
  __IO  uint32_t _reserved3[6];         /**< Non used registers, offser: 0x78 */
  __IO  uint32_t TSU_STA_TIMER;         /**< I3C TSU_STA_timer_reg, offset: 0x90 */
  __IO  uint32_t THD_STA_TIMER;         /**< I3C THD_STA_timer_reg, offset: 0x94 */
  __IO  uint32_t TLOW_TIMER;            /**< I3C TLOW_timer_reg, offset: 0x98 */
  __IO  uint32_t THIGH_TIMER;           /**< I3C THIGH_timer_reg, offset: 0x9C */
  __IO  uint32_t TVD_DATA_TIMER;        /**< I3C TVD_DATA_timer_reg, offset: 0xA0 */
  __IO  uint32_t _reserved4;            /**< Non used registers, offser: 0xA4 */
  __IO  uint32_t TSU_STOP_TIMER;        /**< I3C TSU_STO_timer_reg, offset: 0xA8 */
  __IO  uint32_t _reserved5[21];        /**< Non used registers, offser: 0xAC */
  __IO  uint32_t DEVICE_ADDR_TABLE[12]; /**< I3C Device Address Table Structure register, offset: 0x100 */
  __IO  uint32_t _reserved6[52];        /**<  Non used registers, offser: 0x130 */
  __IO  uint32_t DEVICE_CHAR_TABLE[48]; /**< I3C Device Characteristic Table Register, offset: 0x200 */
} I3C_Type;

/* ----------------------------------------------------------------------------
   -- I3C Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup I3C_Register_Masks I3C Register Masks
 * @{
 */

/*! @name MST_CNTL_EN - I3C master enable control register */
#define I3C_MST_CNTL_EN_MASK           (0x1U)
#define I3C_MST_CNTL_EN_SHIFT          (0U)
#define I3C_MST_CNTL_EN(x)             (((uint32_t)(((uint32_t)(x)) << I3C_MST_CNTL_EN_SHIFT)) & I3C_MST_CNTL_EN_MASK)

#define I3C_MST_CNTL_RESET_EN_MASK     (0x2U)
#define I3C_MST_CNTL_RESET_EN_SHIFT    (1U)
#define I3C_MST_CNTL_RESET_EN(x)       (((uint32_t)(((uint32_t)(x)) << I3C_MST_CNTL_RESET_EN_SHIFT)) & I3C_MST_CNTL_RESET_EN_MASK)

/*! @name IRQ_STATUS - I3C IRQ Status register */
#define I3C_IRQ_RSP_DONE_MASK            (0x1U)
#define I3C_IRQ_RSP_DONE_SHIFT           (0U)
#define READ_I3C_IRQ_RSP_DONE(x)         (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_RSP_DONE_MASK)) >> I3C_IRQ_RSP_DONE_SHIFT)

#define I3C_IRQ_TX_FIFO_FULL_MASK        (0x2U)
#define I3C_IRQ_TX_FIFO_FULL_SHIFT       (1U)
#define READ_I3C_IRQ_TX_FIFO_FULL(x)     (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_TX_FIFO_FULL_MASK)) >> I3C_IRQ_TX_FIFO_FULL_SHIFT)

#define I3C_IRQ_TX_FIFO_EMPTY_MASK       (0x4U)
#define I3C_IRQ_TX_FIFO_EMPTY_SHIFT      (2U)
#define READ_I3C_IRQ_TX_FIFO_EMPTY(x)    (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_TX_FIFO_EMPTY_MASK)) >> I3C_IRQ_TX_FIFO_EMPTY_SHIFT)

#define I3C_IRQ_CMD_REQ_FULL_MASK        (0x8U)
#define I3C_IRQ_CMD_REQ_FULL_SHIFT       (3U)
#define READ_I3C_IRQ_CMD_REQ_FULL(x)     (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_CMD_REQ_FULL_MASK)) >> I3C_IRQ_CMD_REQ_FULL_SHIFT)

#define I3C_IRQ_RX_FIFO_FULL_MASK        (0x10U)
#define I3C_IRQ_RX_FIFO_FULL_SHIFT       (4U)
#define READ_I3C_IRQ_RX_FIFO_FULL(x)     (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_RX_FIFO_FULL_MASK)) >> I3C_IRQ_RX_FIFO_FULL_SHIFT)

#define I3C_IRQ_RSP_FIFO_FULL_MASK       (0x20U)
#define I3C_IRQ_RSP_FIFO_FULL_SHIFT      (5U)
#define READ_I3C_IRQ_RSP_FIFO_FULL(x)    (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_RSP_FIFO_FULL_MASK)) >> I3C_IRQ_RSP_FIFO_FULL_SHIFT)

#define I3C_IRQ_IBI_TRANSFER_DONE_MASK   (0x40U)
#define I3C_IRQ_IBI_TRANSFER_DONE_SHIFT  (6U)
#define READ_I3C_IRQ_IBI_TRANSFER_DONE(x) (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_IBI_TRANSFER_DONE_MASK)) >> I3C_IRQ_IBI_TRANSFER_DONE_SHIFT)

#define I3C_IRQ_IBI_RX_FIFO_FULL_MASK    (0x80U)
#define I3C_IRQ_IBI_RX_FIFO_FULL_SHIFT   (7U)
#define READ_I3C_IRQ_IBI_RX_FIFO_FULL(x) (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_IBI_RX_FIFO_FULL_MASK)) >> I3C_IRQ_IBI_RX_FIFO_FULL_SHIFT)

#define I3C_IRQ_RST_COMPLETION_MASK      (0x100U)
#define I3C_IRQ_RST_COMPLETION_SHIFT     (8U)
#define READ_I3C_IRQ_RST_COMPLETION(x)   (((uint32_t)(((uint32_t)(x)) & I3C_IRQ_RST_COMPLETION_MASK)) >> I3C_IRQ_RST_COMPLETION_SHIFT)

/*!
 * @}
 */ /* end of group I3C_Register_Masks */


/* I3C - Peripheral instance base addresses */
/** Peripheral I3C0 base address */
#define I3C0_BASE                                (SOC_PERI_BASE + 0xa000u)
/** Peripheral I3C0 base pointer */
#define I3C0                                     ((I3C_Type *)I3C0_BASE)
/** Peripheral I3C1 base address */
#define I3C1_BASE                                (SOC_PERI_BASE + 0xa800u)
/** Peripheral I3C1 base pointer */
#define I3C1                                     ((I3C_Type *)I3C1_BASE)
/** Array initializer of I3C base addresses */
#define I3C_BASE_ADDRS                          { I3C0_BASE, I3C1_BASE }
/** Array initializer of I3C base pointers */
#define I3C_BASE_PTRS                           { I3C0, I3C1 }

/*!
 * @}
 */ /* end of group I3C_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CSI2_DPHY Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CSI2_DPHY_Peripheral_Access_Layer CSI2_DPHY Peripheral Access Layer
 * @{
 */

/** CSI2_DPHY - Register Layout Typedef */
typedef struct {
  __IO  uint32_t REG00;            /**< CSI2 DPHY APB Data lane and clock lane enable register, offset: 0x00 << 2 */
  __IO  uint32_t _reserved0[12];   /**< Non used registers, offser: 0x01 << 2 */
  __IO  uint32_t REG0d;            /**< CSI2 DPHY APB Digital clock sample manual set phase register, offset: 0x0d << 2 */
  __IO  uint32_t REG0e;            /**< CSI2 DPHY APB Data lane 3 and clock lane manual set phase register, offset: 0x0e << 2 */
  __IO  uint32_t REG0f;            /**< CSI2 DPHY APB Data lane 0 1 2 lane manual set phase register, offset: 0x0f << 2 */
  __IO  uint32_t _reserved1[2];    /**< Non used registers, offser: 0x10 << 2 */
  __IO  uint32_t REG12;            /**< CSI2 DPHY APB Reverse digital sample clock register, offset: 0x12 << 2 */
  __IO  uint32_t _reserved2[13];   /**< Non used registers, offser: 0x13 << 2 */
  __IO  uint32_t REG20;            /**< CSI2 DPHY APB reg_dim_rstn register, offset: 0x20 << 2 */
  __IO  uint32_t _reserved3[41];   /**< Non used registers, offser: 0x21 << 2 */
  __IO  uint32_t REG4a;            /**< CSI2 DPHY APB clock frequency register, offset: 0x4a << 2 */
  __IO  uint32_t _reserved4[13];   /**< Non used registers, offser: 0x4b << 2 */
  __IO  uint32_t REG58;            /**< CSI2 DPHY APB clock continuous clock mode register, offset: 0x58 << 2 */
  __IO  uint32_t _reserved5[1];    /**< Non used registers, offser: 0x59 << 2 */
  __IO  uint32_t REG5a;            /**< CSI2 DPHY APB clock calibration reception register, offset: 0x5a << 2 */
  __IO  uint32_t _reserved6[15];   /**< Non used registers, offser: 0x5b << 2 */
  __IO  uint32_t REG6a;            /**< CSI2 DPHY APB data0 frequency register, offset: 0x6a << 2 */
  __IO  uint32_t _reserved7[13];   /**< Non used registers, offser: 0x6b << 2 */
  __IO  uint32_t REG78;            /**< CSI2 DPHY APB data0 continuous clock mode register, offset: 0x78 << 2 */
  __IO  uint32_t _reserved8[1];    /**< Non used registers, offser: 0x79 << 2 */
  __IO  uint32_t REG7a;            /**< CSI2 DPHY APB data0 calibration reception register, offset: 0x7a << 2 */
  __IO  uint32_t _reserved9[15];   /**< Non used registers, offser: 0x7b << 2 */
  __IO  uint32_t REG8a;            /**< CSI2 DPHY APB data1 frequency register, offset: 0x8a << 2 */
  __IO  uint32_t _reserveda[13];   /**< Non used registers, offser: 0x8b << 2 */
  __IO  uint32_t REG98;            /**< CSI2 DPHY APB data1 continuous clock mode register, offset: 0x98 << 2 */
  __IO  uint32_t _reservedb[1];    /**< Non used registers, offser: 0x99 << 2 */
  __IO  uint32_t REG9a;            /**< CSI2 DPHY APB data1 calibration reception register, offset: 0x9a << 2 */

} CSI2_DPHY_Type;

/* ----------------------------------------------------------------------------
   -- CSI2_DPHY Register Masks
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup CSI2_DPHY_Register_Masks CSI2_DPHY Register Masks
 * @{
 */

/*! @name CFG - EFUSE control configure register */
#define CSI2_DPHY_REG00_DATA0_LANE_EN_MASK            (0x04U)
#define CSI2_DPHY_REG00_DATA0_LANE_EN_SHIFT           (2U)
#define CSI2_DPHY_REG00_DATA0_LANE_EN_(x)             (((uint32_t)(((uint32_t)(x)) << CSI2_DPHY_REG00_DATA0_LANE_EN_SHIFT)) & CSI2_DPHY_REG00_DATA0_LANE_EN_MASK)
#define CSI2_DPHY_REG00_DATA1_LANE_EN_MASK            (0x80U)
#define CSI2_DPHY_REG00_DATA1_LANE_EN_SHIFT           (3U)
#define CSI2_DPHY_REG00_DATA1_LANE_EN_(x)             (((uint32_t)(((uint32_t)(x)) << CSI2_DPHY_REG00_DATA1_LANE_EN_SHIFT)) & CSI2_DPHY_REG00_DATA1_LANE_EN_MASK)
#define CSI2_DPHY_REG00_CLOCK_LANE_EN_MASK            (0x40U)
#define CSI2_DPHY_REG00_CLOCK_LANE_EN_SHIF            (6U)
#define CSI2_DPHY_REG00_CLOCK_LANE_EN_(x)             (((uint32_t)(((uint32_t)(x)) << CSI2_DPHY_REG00_CLOCK_LANE_EN_SHIFT)) & CSI2_DPHY_REG00_CLOCK_LANE_EN_MASK)

/*!
 * @}
 */ /* end of group CSI2_DPHY_Register_Masks */


/* CSI2_DPHY - Peripheral instance base addresses */
/** Peripheral CSI2_DPHY base address */
#define CSI2_DPHY_BASE                               (SOC_PERI_BASE + 0x0C000u)
/** Peripheral CSI2_DPHY base pointer */
#define CSI2_DPHY                                    ((CSI2_DPHY_Type *)CSI2_DPHY_BASE)
/** Array initializer of CSI2_DPHY base addresses */
#define CSI2_DPHY_BASE_ADDRS                         { CSI2_DPHY_BASE }
/** Array initializer of CSI2_DPHY base pointers */
#define CSI2_DPHY_BASE_PTRS                          { CSI2_DPHY }

/*!
 * @}
 */ /* end of group CSI2_DPHY_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CSI2_PIXEL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup CSI2_PIXEL_Peripheral_Access_Layer CSI2_PIXEL Peripheral Access Layer
 * @{
 */

/** CSI2_PIXEL - Register Layout Typedef */
typedef struct {
  __IO  uint32_t _reserved0[96];         /**< Non used registers, offser: 0x00 */
  __IO  uint32_t CONFIG;                 /**<  CSI2 PIXEL APB  register, offset: 0x60 << 2 */
  __IO  uint32_t ERR_MSB1;               /**<  CSI2 PIXEL APB  register, offset: 0x61 << 2 */
  __IO  uint32_t ERR_MSB;                /**<  CSI2 PIXEL APB  register, offset: 0x62 << 2 */
  __IO  uint32_t ERR_LSB;                /**<  CSI2 PIXEL APB  register, offset: 0x63 << 2 */
  __IO  uint32_t HS_RX_TIMEOUT_MSB2;     /**<  CSI2 PIXEL APB  register, offset: 0x64 << 2 */
  __IO  uint32_t HS_RX_TIMEOUT_MSB1;     /**<  CSI2 PIXEL APB  register, offset: 0x65 << 2 */
  __IO  uint32_t HS_RX_TIMEOUT_LSB;      /**<  CSI2 PIXEL APB  register, offset: 0x66 << 2 */
  __IO  uint32_t VCCFG;                  /**<  CSI2 PIXEL APB  register, offset: 0x67 << 2 */
  __IO  uint32_t POLARITY;               /**<  CSI2 PIXEL APB  register, offset: 0x68 << 2 */
  __IO  uint32_t CCI_ADDRESS;            /**<  CSI2 PIXEL APB  register, offset: 0x69 << 2 */
  __IO  uint32_t CCI_WRITE_DATA;         /**<  CSI2 PIXEL APB  register, offset: 0x6A << 2 */
  __IO  uint32_t CCI_READ_DATA;          /**<  CSI2 PIXEL APB  register, offset: 0x6B << 2 */
  __IO  uint32_t CCI_READ_WRITE;         /**<  CSI2 PIXEL APB  register, offset: 0x6C << 2 */
  __IO  uint32_t CCI_STATUS;             /**<  CSI2 PIXEL APB  register, offset: 0x6D << 2 */
  __IO  uint32_t CCI_DEV_ADDR;           /**<  CSI2 PIXEL APB  register, offset: 0x6E << 2 */
  __IO  uint32_t ULPS_STATUS;            /**<  CSI2 PIXEL APB  register, offset: 0x6F << 2 */
  __IO  uint32_t PHY_STATUS;             /**<  CSI2 PIXEL APB  register, offset: 0x70 << 2 */
} CSI2_PIXEL_Type;

/* ----------------------------------------------------------------------------
   -- CSI2_PIXEL Register Masks
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup CSI2_PIXEL_Register_Masks CSI2_PIXEL Register Masks
 * @{
 */

/*! @name CFG - EFUSE control configure register */
#define CSI2_PIXEL_SHORT_MASK                           (0x3FU)
#define CSI2_PIXEL_SHORT_SHIFT                          (0U)
#define CSI2_PIXEL_SHORT(x)                             (((uint32_t)(((uint32_t)(x)) /* << CSI2_PIXEL_SHORT_SHIFT */)) & CSI2_PIXEL_SHORT_MASK)

/*!
 * @}
 */ /* end of group CSI2_PIXEL_Register_Masks */


/* CSI2_PIXEL - Peripheral instance base addresses */
/** Peripheral CSI2_PIXEL base address */
#define CSI2_PIXEL_BASE                               (SOC_PERI_BASE + 0x0D000u)
/** Peripheral CSI2_PIXEL base pointer */
#define CSI2_PIXEL                                    ((CSI2_PIXEL_Type *)CSI2_PIXEL_BASE)
/** Array initializer of CSI2_PIXEL base addresses */
#define CSI2_PIXEL_BASE_ADDRS                         { CSI2_PIXEL_BASE }
/** Array initializer of CSI2_PIXEL base pointers */
#define CSI2_PIXEL_BASE_PTRS                          { CSI2_PIXEL }

/*!
 * @}
 */ /* end of group CSI2_PIXEL_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- EFUSE_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_CTRL_Peripheral_Access_Layer EFUSE_CTRL Peripheral Access Layer
 * @{
 */

/** EFUSE_CTRL - Register Layout Typedef */
typedef struct {
  __IO  uint32_t CMD;                       /**< EFUSE_Control register, offset: 0x00 */
  __IO  uint32_t CFG;                       /**< EFUSE_Control register, offset: 0x04 */
} EFUSE_CTRL_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_CTRL Register Masks
   ---------------------------------------------------------------------------- */
/*!
 * @addtogroup EFUSE_CTRL_Register_Masks EFUSE_CTRL Register Masks
 * @{
 */
/*! @name CFG - EFUSE control configure register */
#define EFUSE_CTRL_SHORT_MASK                           (0x3FU)
#define EFUSE_CTRL_SHORT_SHIFT                          (0U)
#define EFUSE_CTRL_SHORT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_CTRL_SHORT_SHIFT */)) & EFUSE_CTRL_SHORT_MASK)

#define EFUSE_CTRL_MEDIUM_MASK                          (0xFFC0U)
#define EFUSE_CTRL_MEDIUM_SHIFT                         (6U)
#define EFUSE_CTRL_MEDIUM(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_MEDIUM_SHIFT)) & EFUSE_CTRL_MEDIUM_MASK)

#define EFUSE_CTRL_LONG_MASK                            (0x3FFF0000U)
#define EFUSE_CTRL_LONG_SHIFT                           (16U)
#define EFUSE_CTRL_LONG(x)                              (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_LONG_SHIFT)) & EFUSE_CTRL_LONG_MASK)

#define EFUSE_CTRL_MARGIN_MASK                          (0xC0000000)
#define EFUSE_CTRL_MARGIN_SHIFT                         (30U)
#define EFUSE_CTRL_MARGIN(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_MARGIN_SHIFT)) & EFUSE_CTRL_MARGIN_MASK)

#define    EFUSE_CTRL_CMD_READ       0x1
#define    EFUSE_CTRL_CMD_WRITE      0x2
#define    EFUSE_CTRL_CMD_SLEEP      0x4
/*!
 * @}
 */ /* end of group EFUSE_CTRL_Register_Masks */


/* EFUSE_CTRL - Peripheral instance base addresses */
/** Peripheral EFUSE_CTRL base address */
#define EFUSE_CTRL_BASE                               (SOC_PERI_BASE + 0x0F000u)
/** Peripheral EFUSE_CTRL base pointer */
#define EFUSE_CTRL                                    ((EFUSE_CTRL_Type *)EFUSE_CTRL_BASE)
/** Array initializer of EFUSE_CTRL base addresses */
#define EFUSE_CTRL_BASE_ADDRS                         { EFUSE_CTRL_BASE }
/** Array initializer of EFUSE_CTRL base pointers */
#define EFUSE_CTRL_BASE_PTRS                          { EFUSE_CTRL }

/*!
 * @}
 */ /* end of group EFUSE_CTRL_Peripheral_Access_Layer */



/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Peripheral_Access_Layer EFUSE_REGS Peripheral Access Layer
 * @{
 */

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t REGS[128];            /**< EFUSE_Registers, offset: 0x000 */
} EFUSE_REGS_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Register_Masks EFUSE_REGS Register Masks
 * @{
 */


/*!
 * @}
 */ /* end of group EFUSE_REGS_Register_Masks */


/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x0F200u)
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
   -- DBG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DBG_Peripheral_Access_Layer DBG Peripheral Access Layer
 * @{
 */

/** DBG - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t REGS[2];            /**< DBG Registers, offset: 0x000 */
  __IO  uint32_t IE;                 /**< DBG Registers, offset: 0x000 */
} DBG_Type;

/* ----------------------------------------------------------------------------
   -- DBG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DBG_Register_Masks DBG Register Masks
 * @{
 */


/*!
 * @}
 */ /* end of group DBG_Register_Masks */


/* DBG - Peripheral instance base addresses */
/** Peripheral DBG base address */
#define DBG_BASE                                (SOC_PERI_BASE + 0x90000u)
/** Peripheral DBG base pointer */
#define DBG                                     ((DBG_Type *)DBG_BASE)
/** Array initializer of DBG base addresses */
#define DBG_BASE_ADDRS                          { DBG_BASE }
/** Array initializer of DBG base pointers */
#define DBG_BASE_PTRS                           { DBG }

/*!
 * @}
 */ /* end of group DBG_Peripheral_Access_Layer */



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

#if defined(__ARMCC_VERSION)
  #pragma pop
#elif defined(__CWCC__)
  #pragma pop
#elif defined(__GNUC__)
  /* leave anonymous unions enabled */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=default
#else
  #error Not supported compiler type
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */

/* ----------------------------------------------------------------------------
   -- SDK Compatibility
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SDK_Compatibility_Symbols SDK Compatibility
 * @{
 */

/*!
 * @}
 */ /* end of group SDK_Compatibility_Symbols */


#endif  /* _GAP9_H_ */
