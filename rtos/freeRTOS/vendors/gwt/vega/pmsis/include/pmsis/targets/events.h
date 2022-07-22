/*
 * Copyright (C) 2021 ETH Zurich, University of Bologna and GreenWaves Technologies
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
#define UDMA_EVENT_OFFSET_CMD           (2)
#define UDMA_EVENT_OFFSET_EOT           (3) /* EOT or special event. */

/* Number of events per peripheral. */
#define UDMA_CHANNEL_NB_EVENTS_LOG2     (2)
#define UDMA_CHANNEL_NB_EVENTS          (1 << UDMA_CHANNEL_NB_EVENTS_LOG2)

/* Number of SW events. */
#define NB_SW_EVENTS                    (8)

/**
 * \brief FC IRQ
 *
 * Below are listed HW IRQ FC core can receive.
 */
#define FC_IRQ_SW_EVT(id)               (id)
#define FC_IRQ_DMA_EVT                  (8)
#define FC_IRQ_DMA                      (9)
#define FC_IRQ_TIMER0_LO_EVT            (10)
#define FC_IRQ_TIMER0_HI_EVT            (11)
#define FC_IRQ_TIMER1_LO_EVT            (12)
#define FC_IRQ_TIMER1_HI_EVT            (13)
#define FC_IRQ_REF32K_CLK_EDGE_EVT      (14)
#define FC_IRQ_GPIO                     (15)
#define FC_IRQ_REF32K_CLK_RISE_EVT      (16)
#define FC_IRQ_PWM(id)                  (17 + id)
#define FC_IRQ_RTC_APB_EVT              (21)
#define FC_IRQ_RTC_EVT                  (22)
#define FC_IRQ_DLC_BRIDGE_PICL_OK_IRQ   (24)
#define FC_IRQ_DLC_BRIDGE_SCU_OK_IRQ    (25)
#define FC_IRQ_SOC_EVENT_FIFO_EVT       (26)
#define FC_IRQ_QUIDDIKEY_EVT            (27)
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
#define SOC_EU_NB_FC_EVENTS             (168)

/*! @brief UDMA events */
/* SPIM */
#define SOC_EVENT_UDMA_SPIM_RX(id)      ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_SPIM_TX(id)      ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
#define SOC_EVENT_UDMA_SPIM_CMD(id)     ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_CMD)
/* HYPER */
#define SOC_EVENT_UDMA_HYPER_RX(id)     ((UDMA_HYPER_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_HYPER_TX(id)     ((UDMA_HYPER_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* SDIO */
#define SOC_EVENT_UDMA_SDIO_RX(id)      ((UDMA_SDIO_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_SDIO_TX(id)      ((UDMA_SDIO_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* UART */
#define SOC_EVENT_UDMA_UART_RX(id)      ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_UART_TX(id)      ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* I2C */
#define SOC_EVENT_UDMA_I2C_RX(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_I2C_TX(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
#define SOC_EVENT_UDMA_I2C_CMD(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_CMD)
#define SOC_EVENT_UDMA_I2C_EOT(id)       ((UDMA_I2C_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_EOT)
/* DMACPY */
#define SOC_EVENT_UDMA_DMACPY_RX(id)    ((UDMA_DMACPY_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_DMACPY_TX(id)    ((UDMA_DMACPY_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* I2S */
#define SOC_EVENT_UDMA_I2S_RX(id)       ((UDMA_I2S_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_RX)
#define SOC_EVENT_UDMA_I2S_TX(id)       ((UDMA_I2S_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + UDMA_EVENT_OFFSET_TX)
/* MRAM */
#define SOC_EVENT_UDMA_MRAM_ERASE(id)   ((UDMA_MRAM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2))

/* UDMA EOT & error events. */
#define SOC_EVENT_UDMA_SPIM_EOT(id)     ((UDMA_SPIM_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 3)
#define SOC_EVENT_UDMA_HYPER_EOT(id)    ((UDMA_HYPER_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 3)
#define SOC_EVENT_UDMA_SDIO_EOT(id)     ((UDMA_SDIO_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 2)
#define SOC_EVENT_UDMA_SDIO_ERR(id)     ((UDMA_SDIO_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 3)
#define SOC_EVENT_UDMA_UART_DATA(id)    ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 2)
#define SOC_EVENT_UDMA_UART_ERR(id)     ((UDMA_UART_ID(id) << UDMA_CHANNEL_NB_EVENTS_LOG2) + 3)


/*! @brief PMU events */
#define SOC_EVENT_PMU_CLUSTER_POWER     (128)
#define SOC_EVENT_I3C(id)               (129 + (id))
#define SOC_EVENT_PMU_CLUSTER_NOT_BUSY  (131)
#define SOC_EVENT_PMU_CLUSTER_CG        (132)
#define SOC_EVENT_PMU_DLC_BRIDGE_PICL   (133)
#define SOC_EVENT_PMU_DLC_BRIDGE_SCU    (134)
#define SOC_EVENT_PWM(id)               (135 + id)
#define SOC_EVENT_GPIO                  (139)
#define SOC_EVENT_RTC_APB               (140)
#define SOC_EVENT_RTC                   (141)


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
#define SYSTICK_IRQN                    FC_IRQ_TIMER0_LO_EVT /*!< SysTick IRQ. */

/* FC_IRQ_SW_EVT(0)/CL_IRQ_SW_EVT(0) is not used. */

#define PENDSV_IRQN                     FC_IRQ_SW_EVT(1) /*!< Pending Supervisor Call/SysCall. */
#define CLUSTER_TO_FC_NOTIFY_IRQN       FC_IRQ_SW_EVT(2) /*!< IRQ sent by cluster to FC.
                                                          *   IRQ handler is needed.
                                                          *   Asynchronous.
                                                          */
#define FC_SOC_EVENT_NOTIFY_IRQ         FC_IRQ_SW_EVT(3) /*!< IRQ used by RTC. */


#define FC_TO_CLUSTER_NOTIFY_EVENT      CL_IRQ_SW_EVT(1) /*!< Event sent by FC to cluster.
                                                          *   A cluster core is waiting for this
                                                          *   event.
                                                          *   Synchronous.
                                                          */
#define DMA_SW_IRQN                     CL_IRQ_SW_EVT(2) /*!< Event used when emulating 2D DMA
                                                          *   transfers or large 1D ttransfers.
                                                          *   Master core waits for this SW event,
                                                          *   triggered by CL_IRQ_DMA1 handler.
                                                          */
#define PRINTF_LOCK_IRQN                CL_IRQ_SW_EVT(3) /*!< IRQ used to sync FC and cluster cores
                                                          *   to lock/unlock printf.
                                                          */
#define CL_USER_EVENT                   CL_IRQ_SW_EVT(7) /*!< Event used by user to sync cluster with
                                                          *   FC.
                                                          */


#define FC_NOTIFY_CLUSTER_EVENT         FC_TO_CLUSTER_NOTIFY_EVENT

#define FC_SOC_EVENT_IRQN               FC_IRQ_SOC_EVENT_FIFO_EVT

#define FC_SW_NOTIFY_EVENT              FC_SOC_EVENT_NOTIFY_IRQ


#endif  /* __PMSIS_TARGETS_EVENTS_H__ */
