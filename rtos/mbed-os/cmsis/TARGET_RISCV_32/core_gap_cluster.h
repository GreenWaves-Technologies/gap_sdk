/**************************************************************************//**
 * @file     core_gap.h
 * @brief    CMSIS GAP Core Cluster  Peripheral Access Layer Header File
 * @version  V0.0.1
 * @date     04. October 2017
 ******************************************************************************/
/*
 * Copyright (c) 2017 GreenWaves Technologies SAS. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __CLUSTER_GAP_H_GENERIC
#define __CLUSTER_GAP_H_GENERIC

#ifdef FEATURE_CLUSTER

#include "core_gap.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************
 *                 CMSIS definitions
 ******************************************************************************/
#define CLUSTER_ID                    0                 /**< CLuster ID */
#if defined(__GAP8__)
#define CLUSTER_CORES_NUM             8                 /**< CLuster cores number */
#define CLUSTER_TCDM_SIZE             (0x10000UL)       /*!< CLUSTER TCDM memory size */
#elif defined(__GAP9__)
#define CLUSTER_CORES_NUM             9                 /**< CLuster cores number */
#define CLUSTER_TCDM_SIZE             (0x20000UL)       /*!< CLUSTER TCDM memory size */
#endif

#define CLUSTER_HW_MUTEX_NUM           1
/** SoC events statically reserved by the runtime*/
#define FC_CLUSTER_SW_NOTIF_EVENT      0                /**< Used to notify FC*/
#define CLUSTER_CLUSTER_SW_NOTIF_EVENT 1                /**< Used to notify CLUSTER*/

/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_CLUSTER_EU_CORES    Cluster Event Unit Cores
  \brief    Type definitions for the event unit core Registers
  @{
 */
/**
  \brief  Structure type to access the CLUSTER_EU_CORES.
 */

typedef struct {
  EU_CORE_DEMUX_Type CORES[CLUSTER_CORES_NUM];          /*!< Offset: 0x000 (R/W)  Cluster Event Unit cores */

} CLUSTER_EU_CORES_Type;

/*@} end of group CMSIS_CLUSTER_EU_CORE */



/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_CLUSTER_DMAMCHAN_COMPRESSOR   Cluster TCDM L2 compress engine
  \brief    Type definitions for the event unit core Registers
  @{
 */
/**
  \brief  Structure type to access the DMAMCHAN_COMPRESSOR.
 */

typedef struct {
  __IOM  uint32_t TCDM_ADDR;         /*!< Offset: 0x000 (R/W)  CLUSTER_COMPRESS_ENGINE tcdm address */
  __IOM  uint32_t L2_ADDR;           /*!< Offset: 0x004 (R/W)  CLUSTER_COMPRESS_ENGINE l2 address */
  __IOM  uint32_t CONFIG;            /*!< Offset: 0x008 (R/W)  CLUSTER_COMPRESS_ENGINE configuration */
  __IOM  uint32_t STATUS;            /*!< Offset: 0x00C (R/W)  CLUSTER_COMPRESS_ENGINE status */
  __IOM  uint32_t LUT;               /*!< Offset: 0x010 (R/W)  CLUSTER_COMPRESS_ENGINE look up table */
  __IOM  uint32_t SYMBOL;            /*!< Offset: 0x014 (R/W)  CLUSTER_COMPRESS_ENGINE symbol */
  __IOM  uint32_t BIT_READ;          /*!< Offset: 0x018 (R/W)  CLUSTER_COMPRESS_ENGINE bit read */
  __IOM  uint32_t MODE;              /*!< Offset: 0x01C (R/W)  CLUSTER_COMPRESS_ENGINE mode */
  __IOM  uint32_t SW_RST;            /*!< Offset: 0x020 (R/W)  CLUSTER_COMPRESS_ENGINE software reset */
  __IOM  uint32_t CLKEN;             /*!< Offset: 0x024 (R/W)  CLUSTER_COMPRESS_ENGINE clock enable */
  __IOM  uint32_t TRIGGER;           /*!< Offset: 0x028 (R/W)  CLUSTER_COMPRESS_ENGINE trigger */
  __IOM  uint32_t _reserved0;        /*!< Offset: 0x02C (R/W)  reserved Register */
  __IOM  uint32_t L2_COUNT;          /*!< Offset: 0x030 (R/W)  CLUSTER_COMPRESS_ENGINE l2 2D count */
  __IOM  uint32_t L2_STRIDE;         /*!< Offset: 0x034 (R/W)  CLUSTER_COMPRESS_ENGINE l2 2D stride */
  __IOM  uint32_t TCDM_COUNT;        /*!< Offset: 0x038 (R/W)  CLUSTER_COMPRESS_ENGINE tcdm 2D count */
  __IOM  uint32_t TCDM_STRIDE;       /*!< Offset: 0x03C (R/W)  CLUSTER_COMPRESS_ENGINE tcdm 2D stride */

} CLUSTER_DMAMCHAN_COMPRESSOR_Type;

/*@} end of group CMSIS_DMAMCHAN_COMPRESSOR */

#define CLUSTER_DISPATCH_IS_ENTRY_MASK               (0x1U)
#define CLUSTER_DISPATCH_IS_ENTRY_SHIFT              (0U)
#define CLUSTER_DISPATCH_IS_ENTRY(x)                 (((uint32_t)(((uint32_t)(x)) /* << CLUSTER_DISPATCH_IS_ENTRY_SHIFT */)) & CLUSTER_DISPATCH_IS_ENTRY_MASK)

#define CLUSTER_DISPATCH_ENTRY_ADDR_MASK             (0x7FFFFFFEU)
#define CLUSTER_DISPATCH_ENTRY_ADDR_SHIFT            (1U)
#define CLUSTER_DISPATCH_ADDR_ENTRY(x)               (((uint32_t)(((uint32_t)(x)) << CLUSTER_DISPATCH_ENTRY_ADDR_SHIFT)) & CLUSTER_DISPATCH_ENTRY_ADDR_MASK)

#define CLUSTER_DISPATCH_ENTRY_MASK                  (0x7FFFFFFFU)
#define CLUSTER_DISPATCH_ENTRY_SHIFT                 (0U)
#define CLUSTER_DISPATCH_ENTRY(x)                    (((uint32_t)(((uint32_t)(x)) /*<< CLUSTER_DISPATCH_ENTRY_SHIFT*/)) & CLUSTER_DISPATCH_ENTRY_MASK)
#define READ_CLUSTER_DISPATCH_ENTRY(x)               (((uint32_t)(((uint32_t)(x)) & CLUSTER_DISPATCH_ENTRY_MASK)) /*>> CLUSTER_DISPATCH_ENTRY_SHIFT*/)

#define CLUSTER_DISPATCH_IS_JOB_MASK                 (0x80000000U)
#define CLUSTER_DISPATCH_IS_JOB_SHIFT                (31U)
#define CLUSTER_DISPATCH_IS_JOB(x)                   (((uint32_t)(((uint32_t)(x)) << CLUSTER_DISPATCH_IS_JOB_SHIFT)) & CLUSTER_DISPATCH_IS_JOB_MASK)




#define CLUSTER_BASE                    (0x10000000UL)        /*!< CLUSTER Base Address */

#define CLUSTER_SCB_BASE                (CLUSTER_BASE + CORE_SCB_BASE)                  /*!< CLUSTER System Control Block Base Address */
#define CLUSTER_SCBC_BASE               (CLUSTER_BASE + CORE_SCBC_BASE)                 /*!< CLUSTER System Control Block Cache Base Address */
#define CLUSTER_SysTick_BASE            (CLUSTER_BASE + CORE_SysTick_BASE)              /*!< CLUSTER SysTick Base Address */

#define CLUSTER_EU_BARRIER_BASE         (CLUSTER_BASE + CORE_EU_BARRIER_BASE)    /*!< CLUSTER Event Unit HW Barrier Base Address */
#define CLUSTER_EU_SW_EVENTS_BASE       (CLUSTER_BASE + CORE_EU_SW_EVENTS_BASE)  /*!< CLUSTER Event Unit SW Events Base Address */
#define CLUSTER_EU_SOC_EVENTS_BASE      (CLUSTER_BASE + CORE_EU_SOC_EVENTS_BASE) /*!< CLUSTER Event Unit SOC Events Base Address */
#define CLUSTER_EU_EXT_EVENTS_BASE      (CLUSTER_BASE + CORE_EU_EXT_EVENTS_BASE) /*!< CLUSTER Event Unit EXT Events Base Address */


#define CLUSTER_EU_CORE_DEMUX_BASE      (CLUSTER_BASE + CORE_EU_CORE_DEMUX_BASE)        /*!< CLUSTER Event Unit Core Demux Base Address */
#define CLUSTER_EU_SEC_DEMUX_BASE       (CLUSTER_BASE + CORE_EU_SEC_DEMUX_BASE)         /*!< CLUSTER Event Unit Security Demux Base Address */
#define CLUSTER_EU_LOOP_DEMUX_BASE      (CLUSTER_BASE + CORE_EU_LOOP_DEMUX_BASE)        /*!< CLUSTER Event Unit Loop Demux Base Address */
#define CLUSTER_EU_DISPATCH_DEMUX_BASE  (CLUSTER_BASE + CORE_EU_DISPATCH_DEMUX_BASE)    /*!< CLUSTER Event Unit Dispatch Demux Base Address */
#define CLUSTER_EU_MUTEX_DEMUX_BASE     (CLUSTER_BASE + CORE_EU_MUTEX_DEMUX_BASE)       /*!< CLUSTER Event Unit Mutex Demux Base Address */
#define CLUSTER_EU_SW_EVENTS_DEMUX_BASE (CLUSTER_BASE + CORE_EU_SW_EVENTS_DEMUX_BASE)   /*!< CLUSTER Event Unit SW Events Demux Base Address */
#define CLUSTER_EU_BARRIER_DEMUX_BASE   (CLUSTER_BASE + CORE_EU_BARRIER_DEMUX_BASE)     /*!< CLUSTER Event Unit Barrier Demux Base Address */
#define CLUSTER_DMAMCHAN_COMPRESSOR_BASE CORE_MCHAN_COMPRESSOR_BASE                     /*!< CLUSTER TCDM compress engine Base Address */


/* Cluster Core Structrue definitions */
#define CLUSTER_EU_SW_EVENTS      ((EU_SW_EVENTS_DEMUX_Type   *)  CLUSTER_EU_SW_EVENTS_BASE)            /*!< EU_SW_EVENTS_DEMUX configuration struct */
#define EU_CORES    ((CLUSTER_EU_CORES_Type   *)   CORE_EU_BASE)              /*!< CLUSTER_EU_CORES configuration struct */
#define SCB         ((SCB_Type   *)    CLUSTER_SCB_BASE )                     /*!< CLUSTER SCB configuration struct */
#define CLUSTER_SCBC  ((SCBC_Type   *)  CLUSTER_SCBC_BASE )                    /*!< CLUSTER SCBC configuration struct */

#define CLUSTER_SysTick             ((SysTick_Type   *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */
#define CLUSTER_TIMERL              ((TimerL_Type    *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */
#define CLUSTER_TIMERH              ((TimerH_Type    *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */
#define CLUSTER_DMAMCHAN_COMPRESSOR ((CLUSTER_DMAMCHAN_COMPRESSOR_Type  *)   CLUSTER_DMAMCHAN_COMPRESSOR_BASE )  /*!< TCDM/L2 to L2/TCDM compress engine struct */


#ifdef __cplusplus
}
#endif

#endif

#endif /* __CLUSTER_GAP_H_GENERIC */
