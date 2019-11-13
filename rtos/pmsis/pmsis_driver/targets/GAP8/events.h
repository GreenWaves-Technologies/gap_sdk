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

#if __GAP8__

#include "properties.h"

/* Events offsets. */
#define UDMA_EVENT_OFFSET_RX            (0)
#define UDMA_EVENT_OFFSET_TX            (1)

/* Number of events per peripheral. */
#define UDMA_CHANNEL_NB_EVENTS_LOG2     (1)
#define UDMA_CHANNEL_NB_EVENTS          (1 << UDMA_CHANNEL_NB_EVENTS_LOG2)

/* Number of SW events. */
#define NB_SW_EVENTS                    (8)

/*! @brief FC events */
#define FC_EVENT_SW(id)                 (id & (NB_SW_EVENTS - 1))
//#define FC_EVENT_DMA_EVT              (8)
//#define FC_EVENT_DMA                  (9)
#define FC_EVENT_TIMER0                 (10) /* Timer low.  */
#define SYSTICK_IRQN                    (10)
#define FC_EVENT_TIMER1                 (11) /* Timer high. */
#define FC_EVENT_EU_HWCE                (12)

#define FC_EVENT_SOC_EVENT              (27)
#define FC_EVENT_MPU_ERROR              (28)
#define FC_EVENT_FC_QUEUE_ERROR         (29)
#define FC_EVENT_HP0                    (30)
#define FC_EVENT_HP1                    (31)

/*! @name SoC events  */
/*! @brief UDMA events */
/* SPIM */
#define SOC_EVENT_UDMA_SPIM_RX(id)      ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_SPIM_TX(id)      ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* HYPER */
#define SOC_EVENT_UDMA_HYPER_RX(id)     ((UDMA_HYPER_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_HYPER_TX(id)     ((UDMA_HYPER_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* UART */
#define SOC_EVENT_UDMA_UART_RX(id)      ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_UART_TX(id)      ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* I2C */
#define SOC_EVENT_UDMA_I2C_RX(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_I2C_TX(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* DMACPY */
#define SOC_EVENT_UDMA_DMACPY_RX(id)    ((UDMA_DMACPY_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_DMACPY_TX(id)    ((UDMA_DMACPY_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* I2S */
#define SOC_EVENT_UDMA_I2S_RX(id)       ((UDMA_I2S_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_I2S_TX(id)       ((UDMA_I2S_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* CPI */
#define SOC_EVENT_UDMA_CPI_RX(id)       ((UDMA_CPI_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)

/* UDMA EOT & error events. */
#define SOC_EVENT_UDMA_SPIM_EOT(id)     (22 + id)
//#define SOC_EVENT_UDMA_I2C_ERROR(id)  (26 + id)

/*! @brief PMU events */
#define SOC_EVENT_PMU_CLUSTER_POWER     (31)
#define SOC_EVENT_PMU_CLUSTER_CG        (35)
#define SOC_EVENT_PMU_DLC_BRIDGE_PICL   (36)
#define SOC_EVENT_PMU_DLC_BRIDGE_SCU    (37)
#define SOC_EVENT_PWM(id)               (38 + id)
#define SOC_EVENT_GPIO                  (42)
#define SOC_EVENT_RTC_APB               (43)
#define SOC_EVENT_RTC                   (44)

#define SOC_EVENT_SW(id)                (48 + (id & (NB_SW_EVENTS - 1)))
#define SOC_EVENT_REF32K_CLK_RISE       (56)

/* @brief Cluster events */
#define CL_EVENT_SW(id)                 (id & (NB_SW_EVENTS - 1))
#define CL_EVENT_DMA0                   (8)
#define CL_EVENT_DMA1                   (9)
#define CL_EVENT_TIMER0_LO              (10)
#define CL_EVENT_TIMER0_HI              (11)
#define CL_EVENT_ACC0                   (12)
#define CL_EVENT_ACC1                   (13)
#define CL_EVENT_ACC2                   (14)
#define CL_EVENT_ACC3                   (15)
#define CL_EVENT_BAR                    (16)
#define CL_EVENT_MUTEX                  (17)
#define CL_EVENT_DISPATCH               (18)
#define CL_EVENT_CLUSTER0               (22)
#define CL_EVENT_CLUSTER1               (23)
#define CL_EVENT_SOC_EVT                (27)

#endif  /* __GAP8__ */

#endif  /* __EVENTS_H__ */
