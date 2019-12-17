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

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "properties.h"


#define NB_SW_EVENTS                     (8)

/*! @brief FC events */
#define FC_EVENT_SW(id)                  (id & (NB_SW_EVENTS - 1))
#define FC_EVENT_DMA_EVT                 (8)
#define FC_EVENT_DECOMPR_DONE_IRQ        (9)
#define FC_EVENT_TIMER0_LO               (10)
#define FC_EVENT_TIMER0_HI               (11)
#define FC_EVENT_TIMER1_LO               (12)
#define FC_EVENT_TIMER1_HI               (13)
#define FC_EVENT_REF32K_CLK              (14)
#define FC_EVENT_GPIO                    (15)

#define FC_EVENT_PWM(id)                 ((FC_EVENT_GPIO + 2) + id)
#define FC_EVENT_PMU_CLUSTER_BUSY        (21)
#define FC_EVENT_PMU_CLUSTER_POWER       (22)
#define FC_EVENT_PMU_CLUSTER_CG          (23)
#define FC_EVENT_PMU_DLC_BRIDGE_PICL     (24)
#define FC_EVENT_PMU_DLC_BRIDGE_SCU      (25)
#define FC_EVENT_SOC_EVENT               (26)
#define FC_EVENT_QUIDDIKEY_IRQ           (27)
#define FC_EVENT_MPU_ERROR               (28)
#define FC_EVENT_FC_QUEUE_ERROR          (29)
#define FC_EVENT_HP0                     (30)
#define FC_EVENT_HP1                     (31)


/*! @name SoC events  */
/*! @brief UDMA events */
/* SPIM */
#define SOC_EVENT_UDMA_SPIM_RX(id)       ((UDMA_SPIM_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_SPIM_TX(id)       ((UDMA_SPIM_ID(id) << 2) + 1)
#define SOC_EVENT_UDMA_SPIM_CMD(id)      ((UDMA_SPIM_ID(id) << 2) + 2)
#define SOC_EVENT_UDMA_SPIM_EOT(id)      ((UDMA_SPIM_ID(id) << 2) + 3)
/* UART */
#define SOC_EVENT_UDMA_UART_RX(id)       ((UDMA_UART_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_UART_TX(id)       ((UDMA_UART_ID(id) << 2) + 1)
#define SOC_EVENT_UDMA_UART_RX_DATA(id)  ((UDMA_UART_ID(id) << 2) + 2)
#define SOC_EVENT_UDMA_UART_ERROR(id)    ((UDMA_UART_ID(id) << 2) + 3)
/* I2C */
#define SOC_EVENT_UDMA_I2C_RX(id)        ((UDMA_I2C_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_I2C_TX(id)        ((UDMA_I2C_ID(id) << 2) + 1)
#define SOC_EVENT_UDMA_I2C_CMD(id)       ((UDMA_I2C_ID(id) << 2) + 2)
#define SOC_EVENT_UDMA_I2C_EOT(id)       ((UDMA_I2C_ID(id) << 2) + 3)
/* HYPER */
#define SOC_EVENT_UDMA_HYPER_RX(id)      ((UDMA_HYPER_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_HYPER_TX(id)      ((UDMA_HYPER_ID(id) << 2) + 1)
/* JTAG_FIFO */
#define SOC_EVENT_UDMA_JTAG_FIFO_RX(id)  ((UDMA_JTAG_FIFO_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_JTAG_FIFO_TX(id)  ((UDMA_JTAG_FIFO_ID(id) << 2) + 1)
/* SDIO */
#define SOC_EVENT_UDMA_SDIO_RX(id)       ((UDMA_SDIO_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_SDIO_TX(id)       ((UDMA_SDIO_ID(id) << 2) + 1)
#define SOC_EVENT_UDMA_SDIO_EOT(id)      ((UDMA_SDIO_ID(id) << 2) + 2)
#define SOC_EVENT_UDMA_SDIO_ERROR(id)    ((UDMA_SDIO_ID(id) << 2) + 3)
/* I2S */
#define SOC_EVENT_UDMA_I2S_RX(id)        ((UDMA_I2S_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_I2S_TX(id)        ((UDMA_I2S_ID(id) << 2) + 1)
/* CPI */
#define SOC_EVENT_UDMA_CPI_RX(id)        ((UDMA_CPI_ID(id) << 2) + 0)
/* CSI2 */
#define SOC_EVENT_UDMA_CSI2_RX(id)       ((UDMA_CSI2_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_CSI2_TX(id)       ((UDMA_CSI2_ID(id) << 2) + 1)
/* MRAM */
#define SOC_EVENT_UDMA_MRAM_RX(id)       ((UDMA_MRAM_ID(id) << 2) + 0)
#define SOC_EVENT_UDMA_MRAM_TX(id)       ((UDMA_MRAM_ID(id) << 2) + 1)
/* FILTER */
#define SOC_EVENT_UDMA_FILTER_RX(id)     ((UDMA_FILTER_ID(id) << 2))
#define SOC_EVENT_UDMA_FILTER_TX(id)     ((UDMA_FILTER_ID(id) << 2) + 1)

/*! @brief PMU events */
#define SOC_EVENT_PMU_CLUSTER_POWER      (128)
#define SOC_EVENT_I3C_APB_EVT_0          (129)
#define SOC_EVENT_I3C_APB_EVT_1          (130)
#define SOC_EVENT_PMU_CLUSTER_BUSY       (131)
#define SOC_EVENT_PMU_CLUSTER_CG         (132)
#define SOC_EVENT_PMU_DLC_BRIDGE_PICL    (133)
#define SOC_EVENT_PMU_DLC_BRIDGE_SCU     (134)
#define SOC_EVENT_PWM(id)                ((SOC_EVENT_PMU_DLC_BRIDGE_SCU + 1) + id)
#define SOC_EVENT_GPIO                   (139)
#define SOC_EVENT_RTC_APB                (140)
#define SOC_EVENT_RTC                    (141)

#define SOC_EVENT_SW(id)                 (160 + (id & (NB_SW_EVENTS - 1)))
#define SOC_EVENT_REF32K_CLK             (168)


/* @brief Cluster events */
#define CL_EVENT_SW(id)                  (id & (NB_SW_EVENTS - 1))
#define CL_EVENT_DMA0                    (8)
#define CL_EVENT_DMA1                    (9)
#define CL_EVENT_TIMER0_LO               (10)
#define CL_EVENT_TIMER0_HI               (11)
#define CL_EVENT_ACC0                    (12)
#define CL_EVENT_ACC1                    (13)
#define CL_EVENT_ACC2                    (14)
#define CL_EVENT_ACC3                    (15)
#define CL_EVENT_BAR                     (16)
#define CL_EVENT_MUTEX                   (17)
#define CL_EVENT_DISPATCH                (18)
#define CL_EVENT_CLUSTER0                (22)
#define CL_EVENT_CLUSTER1                (23)
#define CL_EVENT_SOC_EVT                 (27)


#endif  /* __EVENTS_H__ */
