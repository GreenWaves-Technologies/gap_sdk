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
#ifndef __CLUSTER_GAP_H_GENERIC__
#define __CLUSTER_GAP_H_GENERIC__

#ifdef FEATURE_CLUSTER

#include "cores/TARGET_RISCV_32/core_gap.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************
 *                 CMSIS definitions
 ******************************************************************************/
#define CLUSTER_ID                    0                 /**< CLuster ID */
#define CLUSTER_CORES_NUM             8                 /**< CLuster cores number */

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


/* Cluster Core Structrue definitions */
#define CLUSTER_EU_SW_EVENTS      ((EU_SW_EVENTS_DEMUX_Type   *)  CLUSTER_EU_SW_EVENTS_BASE)            /*!< EU_SW_EVENTS_DEMUX configuration struct */
#define EU_CORES    ((CLUSTER_EU_CORES_Type   *)   CORE_EU_BASE)              /*!< CLUSTER_EU_CORES configuration struct */
#define SCB         ((SCB_Type   *)    CLUSTER_SCB_BASE )                     /*!< CLUSTER SCB configuration struct */
#define CLUSTER_SCBC  ((SCBC_Type   *)  CLUSTER_SCBC_BASE )                    /*!< CLUSTER SCBC configuration struct */

#define CLUSTER_SysTick             ((SysTick_Type   *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */
#define CLUSTER_TIMERL              ((TimerL_Type    *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */
#define CLUSTER_TIMERH              ((TimerH_Type    *)     CLUSTER_SysTick_BASE  )   /*!< SysTick configuration struct */



#ifdef __cplusplus
}
#endif

#endif

#endif /* __CLUSTER_GAP_H_GENERIC */
