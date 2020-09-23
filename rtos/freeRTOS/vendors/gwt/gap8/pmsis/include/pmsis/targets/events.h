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

#ifndef __PMSIS_TARGETS_EVENTS_H__
#define __PMSIS_TARGETS_EVENTS_H__

#include "properties.h"

/* Events offsets. */
#define UDMA_EVENT_OFFSET_RX            (0)
#define UDMA_EVENT_OFFSET_TX            (1)

/* Number of events per peripheral. */
#define UDMA_CHANNEL_NB_EVENTS_LOG2     (1)
#define UDMA_CHANNEL_NB_EVENTS          (1 << UDMA_CHANNEL_NB_EVENTS_LOG2)

/* Number of SW events. */
#define NB_SW_EVENTS                    (8)

/**
 * \brief FC IRQ
 *
 * Below are listed HW IRQ FC core can receive.
 */
#define FC_IRQ_SW_EVT(id)               (id & (NB_SW_EVENTS - 1))
#define FC_IRQ_DMA_EVT                  (8)
#define FC_IRQ_DMA                      (9)
#define FC_IRQ_TIMER0_LO_EVT            (10)
#define FC_IRQ_TIMER0_HI_EVT            (11)
#define FC_IRQ_EU_HWCE                  (12)

#define FC_IRQ_SOC_EVENT_FIFO_EVT       (27)
#define FC_IRQ_MPU_ERROR                (28)
#define FC_IRQ_FC_QUEUE_ERROR_EVT       (29)
#define FC_IRQ_HP_0                     (30)
#define FC_IRQ_HP_1                     (31)


/**
 * \brief SoC events
 *
 * Below are listed SoC peripheral events.
 */

/*! @brief Number of FC_Events. */
#define SOC_EU_NB_FC_EVENTS             (57)

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


/**
 * \brief Cluster IRQ
 *
 * Below are listed cluster IRQ.
 */
#define CL_IRQ_SW_EVT(id)               (id & (NB_SW_EVENTS - 1))
#define CL_IRQ_DMA0                     (8)
#define CL_IRQ_DMA1                     (9)
#define CL_IRQ_TIMER0_LO                (10)
#define CL_IRQ_TIMER0_HI                (11)
#define CL_IRQ_ACC_EVT_0                (12) /* HW Acc. */
#define CL_IRQ_ACC_EVT_1                (13) /* HW Acc. */
#define CL_IRQ_ACC_EVT_2                (14) /* HW Acc. */
#define CL_IRQ_ACC_EVT_3                (15) /* HW Acc. */
#define CL_IRQ_BARRIER_EVT              (16)
#define CL_IRQ_HW_MUTEX_EVT             (17)
#define CL_IRQ_DISPATCH_EVT             (18)
#define CL_IRQ_CLUSTER_EVT_0            (22)
#define CL_IRQ_CLUSTER_EVT_1            (23)
#define CL_IRQ_SOC_FIFO_EVT             (27)


/* SW IRQ used. */
/* PendSV */
#define PENDSV_IRQN                     FC_IRQ_SW_EVT(7) /*!< Pending Supervisor Call/SysCall. */
#define SYSTICK_IRQN                    FC_IRQ_TIMER0_LO_EVT /*!< SysTick IRQ. */
#define FC_SOC_EVENT_NOTIFY_IRQ         FC_IRQ_SW_EVT(3)


/* IRQ used for FC and Cluster synchronisation. */
/* FC_IRQ_SW_EVT(0)/CL_IRQ_SW_EVT(0) is not used. */
#define FC_TO_CLUSTER_NOTIFY_EVENT      CL_IRQ_SW_EVT(1) /*!< Event sent by FC to cluster.
                                                          *   A cluster core is waiting for this
                                                          *   event.
                                                          *   Synchronous.
                                                          */
#define DMA_SW_IRQN                     CL_IRQ_SW_EVT(5) /*!< Event used when emulating 2D DMA
                                                          *   transfers or large 1D ttransfers.
                                                          *   Master core waits for this SW event,
                                                          *   triggered by CL_IRQ_DMA1 handler.
                                                          */

#define CLUSTER_TO_FC_NOTIFY_IRQN       FC_IRQ_SW_EVT(4) /*!< IRQ sent by cluster to FC.
                                                          *   IRQ handler is needed.
                                                          *   Asynchronous.
                                                          */

#define FC_NOTIFY_CLUSTER_EVENT         FC_TO_CLUSTER_NOTIFY_EVENT

#define FC_SOC_EVENT_IRQN               FC_IRQ_SOC_EVENT_FIFO_EVT

#define FC_SW_NOTIFY_EVENT              FC_SOC_EVENT_NOTIFY_IRQ

#endif  /* __PMSIS_TARGETS_EVENTS_H__ */
