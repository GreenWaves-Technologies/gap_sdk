/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna and GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0   (the "License");
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


#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "properties.h"

/* Memories */
/* L2 memory */
#define L2_PRIV0_ADDR                               (0x1c000000)
#define L2_PRIV1_ADDR                               (0x1c008000)
#define L2_SHARED_ADDR                              (0x1c010000)
#if                                                 (ARCHI_HAS_L2_ALIAS)
#define L2_PRIV0_ADDR_ALIAS                         (0x00000004)
#define L2_PRIV1_ADDR_ALIAS                         (0x00008000)
#define L2_SHARED_ADDR_ALIAS                        (0x00010000)
#endif

/* L2 TS */
#define L2_PRIV0_TS_ADDR                            (0x1E000000)
#define L2_PRIV1_TS_ADDR                            (0x1E008000)

/* L1 cluster memory */

/* Cluster */
#define ARCHI_CLUSTER_ADDR                          (0x00000000)
#define ARCHI_CLUSTER_SIZE                          (0x00400000)
#define ARCHI_CLUSTER_GLOBAL_ADDR(cid)              (0x10000000 + (cid)*ARCHI_CLUSTER_SIZE)

#define L1_ADDR                                     (0x10000004)
#if                                                 (ARCHI_HAS_CL_L1_ALIAS)
#define L1_ADDR_ALIAS                               (0x00000004)
#endif

/* ROM memory */
#define ROM_ADDR                                    (0x1A000000)


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
//#define FC_ICACHE_ADDR                              (SOC_PERIPHERALS_ADDR + FC_ICACHE_OFFSET)
#define FC_ITC_ADDR                                 (SOC_PERIPHERALS_ADDR + FC_ITC_OFFSET)
#define I3C0_ADDR                                   (SOC_PERIPHERALS_ADDR + I3C0_OFFSET)
#define I3C1_ADDR                                   (SOC_PERIPHERALS_ADDR + I3C1_OFFSET)
#define FC_TIMER_ADDR                               (SOC_PERIPHERALS_ADDR + FC_TIMER_OFFSET)
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
#define UDMA_TIMESTAMP(id)                          (UDMA_PERIPH_BASE_ADDR + (UDMA_TIMESTAMP_ID(id) << UDMA_PERIPH_SIZE_LOG2))

/* GPIO peripherals */
#define PADDIR_00_31     (GPIO_ADDR)
#define GPIOEN_00_31     (GPIO_ADDR + 0x4)
#define PADIN_00_31      (GPIO_ADDR + 0x8)
#define PADOUT_00_31     (GPIO_ADDR + 0xC)
#define PADOUTSET_00_31  (GPIO_ADDR + 0x10)
#define PADOUTCLR_00_31  (GPIO_ADDR + 0x14)
#define INTEN_00_31      (GPIO_ADDR + 0x18)
#define INTTYPE_00_15    (GPIO_ADDR + 0x1C)
#define INTTYPE_16_31    (GPIO_ADDR + 0x20)
#define INTSTATUS_00_31  (GPIO_ADDR + 0x24)
#define PADCFG_00_07     (GPIO_ADDR + 0x28)
#define PADCFG_08_15     (GPIO_ADDR + 0x2C)
#define PADCFG_16_23     (GPIO_ADDR + 0x30)
#define PADCFG_24_31     (GPIO_ADDR + 0x34)
#define PADDIR_32_63     (GPIO_ADDR + 0x38)
#define GPIOEN_32_63     (GPIO_ADDR + 0x3C)
#define PADIN_32_63      (GPIO_ADDR + 0x40)
#define PADOUT_32_63     (GPIO_ADDR + 0x44)
#define PADOUTSET_32_63  (GPIO_ADDR + 0x48)
#define PADOUTCLR_32_63  (GPIO_ADDR + 0x4C)
#define INTEN_32_63      (GPIO_ADDR + 0x50)
#define INTTYPE_32_47    (GPIO_ADDR + 0x54)
#define INTTYPE_48_63    (GPIO_ADDR + 0x58)
#define INTSTATUS_32_63  (GPIO_ADDR + 0x5C)
#define PADCFG_32_39     (GPIO_ADDR + 0x60)
#define PADCFG_40_47     (GPIO_ADDR + 0x64)
#define PADCFG_48_55     (GPIO_ADDR + 0x68)
#define PADCFG_56_63     (GPIO_ADDR + 0x6C)

/* TO FIX. */

#define ARCHI_FLL_AREA_SIZE                         (0x00000010)


/* Cluster peripherals */
#define ARCHI_CLUSTER_PERIPHERALS_OFFSET            (0x00200000)

#define ARCHI_CLUSTER_CTRL_OFFSET                   (0x00000000)
#define ARCHI_TIMER_OFFSET                          (0x00000400)
#define ARCHI_EU_OFFSET                             (0x00000800)
#define ARCHI_HWCE_OFFSET                           (0x00001000)
#define ARCHI_ICACHE_CTRL_OFFSET                    (0x00001400)
#define ARCHI_MCHAN_EXT_OFFSET                      (0x00001800)

#define ARCHI_CLUSTER_PERIPHERALS_ADDR              (ARCHI_CLUSTER_ADDR + ARCHI_CLUSTER_PERIPHERALS_OFFSET)
#define ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid)  (ARCHI_CLUSTER_GLOBAL_ADDR(cid) + ARCHI_CLUSTER_PERIPHERALS_OFFSET)

#define ARCHI_CLUSTER_CTRL_ADDR                     (ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_CLUSTER_CTRL_OFFSET)
#define ARCHI_ICACHE_CTRL_ADDR                      (ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_ICACHE_CTRL_OFFSET)
#define ARCHI_EU_ADDR                               (ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_EU_OFFSET)
#define ARCHI_HWCE_ADDR                             (ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_HWCE_OFFSET)
#define ARCHI_MCHAN_EXT_ADDR                        (ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_MCHAN_EXT_OFFSET)

/* Cluster demux peripherals */
#define ARCHI_DEMUX_PERIPHERALS_OFFSET              (0x204000)

#define ARCHI_EU_DEMUX_OFFSET                       (0x00000)
#define ARCHI_MCHAN_DEMUX_OFFSET                    (0x00400)


#define ARCHI_DEMUX_PERIPHERALS_ADDR                (ARCHI_CLUSTER_ADDR + ARCHI_DEMUX_PERIPHERALS_OFFSET)

#define ARCHI_EU_DEMUX_ADDR                         (ARCHI_DEMUX_PERIPHERALS_ADDR + ARCHI_EU_DEMUX_OFFSET)
#define ARCHI_MCHAN_DEMUX_ADDR                      (ARCHI_DEMUX_PERIPHERALS_ADDR + ARCHI_MCHAN_DEMUX_OFFSET)

#endif  /* __MEMORY_MAP_H__ */
