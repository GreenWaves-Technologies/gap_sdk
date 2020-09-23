/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __PMSIS_TARGETS_MEMORY_MAP_H__
#define __PMSIS_TARGETS_MEMORY_MAP_H__

#include "properties.h"
/* Memories */
/* FC memory. */
#define FC_TCDM_ADDR                                (0x1B000000)
#if (ARCHI_HAS_FC_ALIAS)
#define FC_TCDM_ADDR_ALIAS                          (0x00000000)
#endif  /* ARCHI_HAS_FC_ALIAS */

/* L2 memory */
#define L2_SHARED_ADDR                              (0x1C000000)
#if (ARCHI_HAS_L2_ALIAS)
#define L2_SHARED_ADDR_ALIAS                        (0x00000000)
#endif  /* ARCHI_HAS_L2_ALIAS */

/* ROM memory */
#define ROM_ADDR                                    (0x1A000000)

/* Peripherals */
/* SoC peripherals */
#define SOC_PERIPHERALS_ADDR                        (0x1A100000)

#define SOC_FLL_ADDR                                (SOC_PERIPHERALS_ADDR + SOC_FLL_OFFSET)
#define CL_FLL_ADDR                                 (SOC_PERIPHERALS_ADDR + CL_FLL_OFFSET)
#define GPIO_ADDR                                   (SOC_PERIPHERALS_ADDR + GPIO_OFFSET)
#define UDMA_CTRL_ADDR                              (SOC_PERIPHERALS_ADDR + UDMA_OFFSET)
#define APB_SOC_CTRL_ADDR                           (SOC_PERIPHERALS_ADDR + APB_SOC_CTRL_OFFSET)
#define ADV_TIMER_ADDR                              (SOC_PERIPHERALS_ADDR + ADV_TIMER_OFFSET)
#define SOC_EU_ADDR                                 (SOC_PERIPHERALS_ADDR + SOC_EU_OFFSET)
#define DLC_ADDR                                    (SOC_PERIPHERALS_ADDR + DLC_OFFSET)
#define RTC_ADDR                                    (SOC_PERIPHERALS_ADDR + RTC_BASE_OFFSET)
#define EFUSE_ADDR                                  (SOC_PERIPHERALS_ADDR + EFUSE_OFFSET)

/* UDMA peripherals */
#define UDMA_GC_ADDR                                (UDMA_CTRL_ADDR + 0x780)
/* UDMA base peripheral addr = UDMA base address + UDMA ctrl. */
#define UDMA_PERIPH_BASE_ADDR                       (UDMA_CTRL_ADDR)
#define UDMA_SPIM(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_SPIM_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_HYPER(id)                              (UDMA_PERIPH_BASE_ADDR + (UDMA_HYPER_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_UART(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_UART_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_I2C(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_I2C_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_DMACPY(id)                             (UDMA_PERIPH_BASE_ADDR + (UDMA_DMACPY_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_I2S(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_I2S_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_CPI(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_CPI_ID(id) << UDMA_PERIPH_SIZE_LOG2))


/**
 * Cluster.
 */
/* Cluster base address. */
#define ARCHI_CLUSTER_BASE_ADDR                     (0x00000000)
#define ARCHI_CLUSTER_GLOBAL_ADDR(cid)              (0x10000000 + (cid) * ARCHI_CLUSTER_SIZE)

/* Memories. */
/* L1 cluster memory */
#define CL_L1_ADDR                                  (0x10000000)
#if (ARCHI_HAS_CL_L1_ALIAS)
#define CL_L1_ADDR_ALIAS                            (0x00000000)
#endif  /* ARCHI_HAS_CL_L1_ALIAS */

/* L1 cluster TS */
#if (ARCHI_HAS_CL_L1_TS)
#define CL_L1_TS_ADDR                               (CL_L1_ADDR + ARCHI_CL_L1_TS_OFFSET)
#endif  /* ARCHI_HAS_CL_L1_TS */


/* Cluster peripherals. */
#define CL_PERIPHERALS_ADDR                         (0x00200000)
#define ARCHI_CL_PERIPHERALS_ADDR                   (ARCHI_CLUSTER_BASE_ADDR + CL_PERIPHERALS_ADDR)
#define ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid)       (ARCHI_CLUSTER_GLOBAL_ADDR(cid) + CL_PERIPHERALS_ADDR)

#define CL_CTRL_ADDR                                (ARCHI_CL_PERIPHERALS_ADDR + CL_CTRL_OFFSET)
#define CL_CTRL_GLOB_ADDR(cid)                      (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_CTRL_OFFSET)
#define CL_TIMER_ADDR                               (ARCHI_CL_PERIPHERALS_ADDR + CL_TIMER_OFFSET)
#define CL_GLOB_EU_CORE_ADDR(cid)                   (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_CORE_OFFSET)
#define CL_GLOB_EU_BARRIER_ADDR(cid)                (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_BARRIER_OFFSET)
#define CL_GLOB_EU_SW_EVENT_ADDR(cid)               (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_SW_EVENT_OFFSET)
#define CL_GLOB_EU_SOC_EVENT_ADDR(cid)              (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_SOC_EVENT_OFFSET)
#define CL_GLOB_EU_DISPATCH_ADDR(cid)               (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_DISPATCH_OFFSET)
#define CL_GLOB_ICACHE_ADDR(cid)                    (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_ICACHE_CTRL_OFFSET)
#define CL_ICACHE_ADDR                              (ARCHI_CL_PERIPHERALS_ADDR + CL_ICACHE_CTRL_OFFSET)
#define CL_DMA_ADDR                                 (ARCHI_CL_PERIPHERALS_ADDR + CL_DMA_OFFSET)

/* Demux peripherals. */
#define CL_DEMUX_EU_CORE_ADDR                       (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_ADDR                       (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_ADDR                   (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_MUTEX_ADDR                      (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_ADDR                   (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_BARRIER_ADDR                    (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_BARRIER_OFFSET)
#define CL_DEMUX_DMA_ADDR                           (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_DMA_OFFSET)

#endif  /* __PMSIS_TARGETS_MEMORY_MAP_H__ */
