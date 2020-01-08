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

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__


/* FC */
#define ARCHI_FC_CID                (31)
#define ARCHI_HAS_FC_ITC            (1)

/* Cluster */
#define ARCHI_HAS_CLUSTER           (1)
#define ARCHI_CL_CID                (0)
#define ARCHI_CLUSTER_NB_PE         (8)
#define ARCHI_HAS_CLUSTER_CLK_GATE  (1)


/* Memories */
/* L2 memory */
#define ARCHI_HAS_L2_ALIAS          (1)
#define L2_PRIV0_SIZE               (0x00008000)
#define L2_PRIV1_SIZE               (0x00008000)
#define L2_SHARED_SIZE              (0x00170000)

/* L2 TS */
#define L2_PRIV0_HAS_TS             (1)
#define L2_PRIV1_HAS_TS             (1)

/* L1 cluster memory */
#define ARCHI_HAS_CL_L1_ALIAS       (1)
#define CL_L1_SIZE                  (0x00020000)

/* ROM memory */
#define ROM_SIZE                    (0x00002000)

/* SoC peripherals */
#define FLL_OFFSET                  (0x00000000)
#define CVP_OFFSET                  (0x00000000)
#define GPIO_OFFSET                 (0x00001000)
#define UDMA_OFFSET                 (0x00002000)
#define APB_SOC_CTRL_OFFSET         (0x00004000)
#define ADV_TIMER_OFFSET            (0x00005000)
#define SOC_EU_OFFSET               (0x00006000)
#define PMU_OFFSET                  (0x00007000)
#define RTC_BASE_OFFSET             (0x00008000)
//#define FC_ICACHE_OFFSET            (0x00008800)
#define FC_ITC_OFFSET               (0x00009000)
#define I3C0_OFFSET                 (0x0000A000)
#define I3C1_OFFSET                 (0x0000A800)
#define FC_TIMER_OFFSET             (0x0000B000)
#define DPHY_OFFSET                 (0x0000C000)
#define CSI2_OFFSET                 (0x0000D000)
#define MPU_OFFSET                  (0x0000E000)
#define EFUSE_OFFSET                (0x0000F000)
#define DEBUG_OFFSET                (0x00010000)
#define STDOUT_OFFSET               (0x00020000)
#define QUIDDIKEY_OFFSET            (0x00021000)

/* UDMA peripherals */
#define UDMA_HAS_SPIM               (1)
#define UDMA_HAS_UART               (1)
#define UDMA_HAS_I2C                (1)
#define UDMA_HAS_HYPER              (1)
#define UDMA_HAS_JTAG_FIFO          (1)
#define UDMA_HAS_SDIO               (1)
#define UDMA_HAS_I2S                (1)
#define UDMA_HAS_CPI                (1)
#define UDMA_HAS_CSI2               (1)
#define UDMA_HAS_MRAM               (1)
#define UDMA_HAS_FILTER             (1)
#define UDMA_HAS_TIMESTAMP          (1)

/* Number of UDMA peripherals */
#define UDMA_NB_SPIM                (4)
#define UDMA_NB_UART                (3)
#define UDMA_NB_I2C                 (1)
#define UDMA_NB_HYPER               (1)
#define UDMA_NB_JTAG_FIFO           (1)
#define UDMA_NB_SDIO                (1)
#define UDMA_NB_I2S                 (1)
#define UDMA_NB_CPI                 (1)
#define UDMA_NB_CSI2                (1)
#define UDMA_NB_MRAM                (1)
#define UDMA_NB_FILTER              (1)
#define UDMA_NB_TIMESTAMP           (1)

#define UDMA_NB_PERIPH              ((UDMA_HAS_SPIM ? UDMA_NB_SPIM) + (UDMA_HAS_UART ? UDMA_NB_UART) \
                                     (UDMA_HAS_I2C ? UDMA_NB_I2C) + (UDMA_HAS_HYPER ? UDMA_NB_HYPER) \
                                     (UDMA_HAS_JTAG_FIFO ? UDMA_NB_JTAG_FIFO) + (UDMA_HAS_SDIO ? UDMA_NB_SDIO) \
                                     (UDMA_HAS_I2S ? UDMA_NB_I2S) + (UDMA_HAS_CPI ? UDMA_NB_CPI) \
                                     (UDMA_HAS_CSI2 ? UDMA_NB_CSI2) + (UDMA_HAS_MRAM ? UDMA_NB_MRAM) \
                                     (UDMA_HAS_FILTER ? UDMA_NB_FILTER) + (UDMA_HAS_TIMESTAMP ? UDMA_NB_TIMESTAMP))

/* Size of each UDMA peripheral */
#define UDMA_PERIPH_SIZE_LOG2       (7)
#define UDMA_PERIPH_SIZE            (1 << UDMA_PERIPH_SIZE_LOG2)

/* UDMA peripherals ID */
#define UDMA_SPIM_ID(id)            (0   + (id))
#define UDMA_UART_ID(id)            (UDMA_SPIM_ID(UDMA_NB_SPIM - 1) + 1   + (id))
#define UDMA_I2C_ID(id)             (UDMA_UART_ID(UDMA_NB_UART - 1) + 1   + (id))
#define UDMA_HYPER_ID(id)           (UDMA_I2C_ID(UDMA_NB_I2C - 1) + 1   + (id))
#define UDMA_JTAG_FIFO_ID(id)       (UDMA_HYPER_ID(UDMA_NB_HYPER - 1) + 1   + (id))
#define UDMA_SDIO_ID(id)            (UDMA_JTAG_FIFO_ID(UDMA_NB_JTAG_FIFO - 1) + 1   + (id))
#define UDMA_I2S_ID(id)             (UDMA_SDIO_ID(UDMA_NB_SDIO - 1) + 1   + (id))
#define UDMA_CPI_ID(id)             (UDMA_I2S_ID(UDMA_NB_I2S - 1) + 1   + (id))
#define UDMA_CSI2_ID(id)            (UDMA_CPI_ID(UDMA_NB_CPI - 1) + 1   + (id))
#define UDMA_MRAM_ID(id)            (UDMA_CSI2_ID(UDMA_NB_CSI2 - 1) + 1   + (id))
#define UDMA_FILTER_ID(id)          (UDMA_MRAM_ID(UDMA_NB_MRAM - 1) + 1   + (id))
#define UDMA_TIMESTAMP_ID(id)       (UDMA_FILTER_ID(UDMA_NB_FILTER - 1) + 1   + (id))

/* GPIO ID */
#define GPIO_ID(id)                 (0 + id)

/* Ref32k clock */
#define ARCHI_REF_CLOCK_LOG2        (15)
#define ARCHI_REF_CLOCK             (1<<ARCHI_REF_CLOCK_LOG2)

/* FLLs */
#define ARCHI_NB_FLL                (3)


/* PWM */
#define ARCHI_HAS_PWM               (1)
#define ARCHI_NB_PWM                (4)


/* HWS */
#define ARCHI_EU_NB_HW_MUTEX        (1)


#endif  /* __PROPERTIES_H__ */
