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

#ifndef __PMSIS_TARGETS_PROPERTIES_H__
#define __PMSIS_TARGETS_PROPERTIES_H__

/* Cores & cluster */
/* FC */
#define ARCHI_FC_CID                (32)

/* Memories */
/* FC memory */
#define ARCHI_HAS_FC_TCDM           (1)
#define ARCHI_HAS_FC_ALIAS          (1)
#define FC_TCDM_SIZE                (0x00004000) /* 16kB. */

/* L2 memory */
#define ARCHI_HAS_L2_ALIAS          (0)
#define L2_SHARED_SIZE              (0x00080000) /* 512kB. */

/* ROM memory */
#define ROM_SIZE                    (0x00002000)

/* Peripherals */
/* SoC peripherals */
#define SOC_FLL_OFFSET              (0x00000000)
#define CL_FLL_OFFSET               (0x00000800)
#define GPIO_OFFSET                 (0x00001000)
#define UDMA_OFFSET                 (0x00002000)
#define APB_SOC_CTRL_OFFSET         (0x00004000)
#define ADV_TIMER_OFFSET            (0x00005000) /* PWM. */
#define SOC_EU_OFFSET               (0x00006000)
#define DLC_OFFSET                  (0x00007000)
#define RTC_BASE_OFFSET             (0x00008000)
#define EFUSE_OFFSET                (0x00009000)

/* UDMA peripherals */
#define UDMA_HAS_SPIM               (1)
#define UDMA_HAS_HYPER              (1)
#define UDMA_HAS_UART               (1)
#define UDMA_HAS_I2C                (1)
#define UDMA_HAS_DMACPY             (1)
#define UDMA_HAS_I2S                (1)
#define UDMA_HAS_CPI                (1)

/* Number of UDMA peripherals */
#define UDMA_NB_SPIM                (2)
#define UDMA_NB_HYPER               (1)
#define UDMA_NB_UART                (1)
#define UDMA_NB_I2C                 (2)
#define UDMA_NB_DMACPY              (1)
#define UDMA_NB_I2S                 (1)
#define UDMA_NB_CPI                 (1)

#define UDMA_NB_PERIPH              ((UDMA_HAS_SPIM ? UDMA_NB_SPIM) + \
                                     (UDMA_HAS_HYPER ? UDMA_NB_HYPER) + \
                                     (UDMA_HAS_UART ? UDMA_NB_UART) + \
                                     (UDMA_HAS_I2C ? UDMA_NB_I2C) + \
                                     (UDMA_HAS_DMACPY ? UDMA_NB_DMACPY) + \
                                     (UDMA_HAS_I2S ? UDMA_NB_I2S) + \
                                     (UDMA_HAS_CPI ? UDMA_NB_CPI))

/* Size of each UDMA peripheral */
#define UDMA_PERIPH_SIZE_LOG2       (7)
#define UDMA_PERIPH_SIZE            (1 << UDMA_PERIPH_SIZE_LOG2)

/* UDMA peripherals ID */
#define UDMA_SPIM_ID(id)            (1   + (id))
#define UDMA_HYPER_ID(id)           (3   + (id))
#define UDMA_UART_ID(id)            (4   + (id))
#define UDMA_I2C_ID(id)             (5   + (id))
#define UDMA_DMACPY_ID(id)          (7   + (id))
#define UDMA_I2S_ID(id)             (8  + (id))
#define UDMA_CPI_ID(id)             (9  + (id))


/* Pads & GPIO. */
#define ARCHI_NB_PAD                (48)
#define ARCHI_NB_GPIO               (32)

/* PWM. */
#define ARCHI_NB_PWM                (4)
#define ARCHI_NB_CHANNEL_PER_PWM    (4)


/* Cluster */
/* Cores & cluster. */
#define ARCHI_HAS_CLUSTER           (1)
#define ARCHI_CL_CID(id)            (id)
#define ARCHI_CLUSTER_NB_PE         (8) /* Processing elements. */
#define ARCHI_CLUSTER_PE_MASK       (((1 << ARCHI_CLUSTER_NB_PE) - 1))
#define ARCHI_CLUSTER_MASTER_CORE   (0)
#define ARCHI_CLUSTER_SYNC_BARR_ID  (7)

#define ARCHI_CLUSTER_SIZE          (0x00400000)

/* Memories. */
/* L1 cluster memory */
#define ARCHI_HAS_CL_L1_ALIAS       (1)
#define CL_L1_SIZE                  (0x00010000) /* 64kB. */

/* L1 cluster memory TS */
#define ARCHI_HAS_CL_L1_TS          (1)
#define ARCHI_CL_L1_TS_OFFSET       (1 << 20)

/* Cluster peripherals. */
#define CL_CTRL_OFFSET              (0x00000000)
#define CL_TIMER_OFFSET             (0x00000400)
#define CL_GLOB_EU_CORE_OFFSET      (0x00000800)
#define CL_GLOB_EU_BARRIER_OFFSET   (0x00000C00)
#define CL_GLOB_EU_SW_EVENT_OFFSET  (0x00000E00)
#define CL_GLOB_EU_SOC_EVENT_OFFSET (0x00000F00)
#define CL_GLOB_EU_DISPATCH_OFFSET  (0x00000F80)
#define CL_HWCE_OFFSET              (0x00001000)
#define CL_ICACHE_CTRL_OFFSET       (0x00001400)
#define CL_DMA_OFFSET               (0x00001800)
#define CL_DEMUX_PERIPH_OFFSET      (0x00004000)

/* Cluster demux peripherals.*/
#define CL_DEMUX_EU_CORE_OFFSET     (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_OFFSET     (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_OFFSET (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_MUTEX_OFFSET    (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_OFFSET (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_BARRIER_OFFSET  (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_BARRIER_OFFSET)
#define CL_DEMUX_DMA_OFFSET         (CL_DEMUX_PERIPH_OFFSET + 0x00000400)


#endif  /* __PMSIS_TARGETS_PROPERTIES_H__ */
