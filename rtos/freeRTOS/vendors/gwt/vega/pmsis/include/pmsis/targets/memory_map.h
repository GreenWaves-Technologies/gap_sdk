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

#pragma once

#include "properties.h"

/* Memories */
/* L2 memory */
#define L2_PRIV0_ADDR                               (0x1c000000)
#define L2_PRIV1_ADDR                               (0x1c008000)
#define L2_SHARED_ADDR                              (0x1c010000)
#if (ARCHI_HAS_L2_ALIAS)
#define L2_PRIV0_ADDR_ALIAS                         (0x00000000)
#define L2_PRIV1_ADDR_ALIAS                         (0x00008000)
#define L2_SHARED_ADDR_ALIAS                        (0x00010000)
#endif

/* L2 TS. */
#if (ARCHI_HAS_L2_PRIV0_TS)
#define L2_PRIV0_TS_ADDR                            (0x1E000000)
#endif  /* ARCHI_HAS_L2_PRIV0_TS */
#if (ARCHI_HAS_L2_PRIV1_TS)
#define L2_PRIV1_TS_ADDR                            (0x1E008000)
#endif  /* ARCHI_HAS_L2_PRIV1_TS */

/* ROM memory */
#define ROM_ADDR                                    (0x1A000000)

/* Peripherals. */
/* SoC peripherals */
#define SOC_PERIPHERALS_ADDR                        (0x1A100000)

#define FLL_ADDR                                    (SOC_PERIPHERALS_ADDR + FLL_OFFSET)
#define GPIO_ADDR                                   (SOC_PERIPHERALS_ADDR + GPIO_OFFSET)
#define UDMA_CTRL_ADDR                              (SOC_PERIPHERALS_ADDR + UDMA_OFFSET)
#define APB_SOC_CTRL_ADDR                           (SOC_PERIPHERALS_ADDR + APB_SOC_CTRL_OFFSET)
#define ADV_TIMER_ADDR                              (SOC_PERIPHERALS_ADDR + ADV_TIMER_OFFSET)
#define SOC_EU_ADDR                                 (SOC_PERIPHERALS_ADDR + SOC_EU_OFFSET)
#define PMU_ADDR                                    (SOC_PERIPHERALS_ADDR + PMU_OFFSET)
#define RTC_ADDR                                    (SOC_PERIPHERALS_ADDR + RTC_BASE_OFFSET)
#define FC_ITC_ADDR                                 (SOC_PERIPHERALS_ADDR + FC_ITC_OFFSET)
/* I3C. */
#define I3C_ADDR(id)                                (SOC_PERIPHERALS_ADDR + I3C0_OFFSET + (I3C(id) << I3C_SIZE_LOG2))
/* TIMER. */
#define FC_TIMER_ADDR(id)                           (SOC_PERIPHERALS_ADDR + FC_TIMER_OFFSET + (FC_TIMER(id) << TIMER_SIZE_LOG2))
#define FC_ICACHE_ADDR                              (SOC_PERIPHERALS_ADDR + FC_ICACHE_OFFSET)
#define DPHY_ADDR                                   (SOC_PERIPHERALS_ADDR + DPHY_OFFSET)
#define CSI2_ADDR                                   (SOC_PERIPHERALS_ADDR + CSI2_OFFSET)
#define MPU_ADDR                                    (SOC_PERIPHERALS_ADDR + MPU_OFFSET)
#define EFUSE_ADDR                                  (SOC_PERIPHERALS_ADDR + EFUSE_OFFSET)
#define DEBUG_ADDR                                  (SOC_PERIPHERALS_ADDR + DEBUG_OFFSET)
#define STDOUT_ADDR                                 (SOC_PERIPHERALS_ADDR + STDOUT_OFFSET)
#define QUIDDIKEY_ADDR                              (SOC_PERIPHERALS_ADDR + QUIDDIKEY_OFFSET)

/* UDMA peripherals */
/* UDMA base peripheral addr = UDMA base address + UDMA ctrl. */
#define UDMA_PERIPH_BASE_ADDR                       (UDMA_CTRL_ADDR + UDMA_PERIPH_SIZE)
#define UDMA_SPIM(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_SPIM_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_UART(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_UART_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_I2C(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_I2C_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_HYPER(id)                              (UDMA_PERIPH_BASE_ADDR + (UDMA_HYPER_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_JTAG_FIFO(id)                          (UDMA_PERIPH_BASE_ADDR + (UDMA_JTAG_FIFO_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_SDIO(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_SDIO_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_I2S(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_I2S_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_CPI(id)                                (UDMA_PERIPH_BASE_ADDR + (UDMA_CPI_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_CSI2(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_CSI2_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_MRAM(id)                               (UDMA_PERIPH_BASE_ADDR + (UDMA_MRAM_ID(id) << UDMA_PERIPH_SIZE_LOG2))
#define UDMA_FILTER(id)                             (UDMA_PERIPH_BASE_ADDR + (UDMA_FILTER_ID(id) << UDMA_PERIPH_SIZE_LOG2))


/**
 * Cluster.
 */
/* Cluster base address. */
#define ARCHI_CLUSTER_BASE_ADDR                     (0x00000000)
#define ARCHI_CLUSTER_GLOBAL_ADDR(cid)              (0x10000000 + (cid) * ARCHI_CLUSTER_SIZE)

/* Memories. */
/* L1 cluster memory */
#define L1_ADDR                                     (0x10000000)
#if (ARCHI_HAS_L1_ALIAS)
#define L1_ADDR_ALIAS                               (0x00000000)
#endif


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
#define CL_GLOB_DECOMP_ADDR(cid)                    (ARCHI_CL_PERIPHERALS_GLOBAL_ADDR(cid) + CL_DECOMP_OFFSET)
#define CL_DECOMP_ADDR                              (ARCHI_CL_PERIPHERALS_ADDR + CL_DECOMP_OFFSET)

/* Demux peripherals. */
#define CL_DEMUX_EU_CORE_ADDR                       (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_ADDR                       (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_ADDR                   (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_HW_MUTEX_ADDR                   (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_HW_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_ADDR                   (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_HW_BARRIER_ADDR                 (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_HW_BARRIER_OFFSET)
#define CL_DEMUX_EU_HW_SEMA_ADDR                    (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_EU_HW_SEMA_OFFSET)
#define CL_DEMUX_DMA_ADDR                           (ARCHI_CL_PERIPHERALS_ADDR + CL_DEMUX_DMA_OFFSET)
