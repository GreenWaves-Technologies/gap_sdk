/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#ifndef _GAP_SOC_EVENT_UNIT_H_
#define _GAP_SOC_EVENT_UNIT_H_

#include "gap_util.h"

/*!
 * @addtogroup soc_eu
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief UDMA Event Number List */
#define    UDMA_EVENT_SPIM0_RX            ((PER_ID_SPIM0 << 2))
#define    UDMA_EVENT_SPIM0_TX            ((PER_ID_SPIM0 << 2) + 1)
#define    UDMA_EVENT_SPIM0_CMD           ((PER_ID_SPIM0 << 2) + 2)
#define    UDMA_EVENT_SPIM0_EOT           ((PER_ID_SPIM0 << 2) + 3)

#define    UDMA_EVENT_SPIM1_RX            ((PER_ID_SPIM1 << 2))
#define    UDMA_EVENT_SPIM1_TX            ((PER_ID_SPIM1 << 2) + 1)
#define    UDMA_EVENT_SPIM1_CMD           ((PER_ID_SPIM1 << 2) + 2)
#define    UDMA_EVENT_SPIM1_EOT           ((PER_ID_SPIM1 << 2) + 3)

#define    UDMA_EVENT_SPIM2_RX            ((PER_ID_SPIM2 << 2))
#define    UDMA_EVENT_SPIM2_TX            ((PER_ID_SPIM2 << 2) + 1)
#define    UDMA_EVENT_SPIM2_CMD           ((PER_ID_SPIM2 << 2) + 2)
#define    UDMA_EVENT_SPIM2_EOT           ((PER_ID_SPIM2 << 2) + 3)

#define    UDMA_EVENT_SPIM3_RX            ((PER_ID_SPIM3 << 2))
#define    UDMA_EVENT_SPIM3_TX            ((PER_ID_SPIM3 << 2) + 1)
#define    UDMA_EVENT_SPIM3_CMD           ((PER_ID_SPIM3 << 2) + 2)
#define    UDMA_EVENT_SPIM3_EOT           ((PER_ID_SPIM3 << 2) + 3)

#define    UDMA_EVENT_UART0_RX            ((PER_ID_UART0 << 2))
#define    UDMA_EVENT_UART0_TX            ((PER_ID_UART0 << 2) + 1)
#define    UDMA_EVENT_UART0_RX_IRQ        ((PER_ID_UART0 << 2) + 2)
#define    UDMA_EVENT_UART0_ERROR         ((PER_ID_UART0 << 2) + 3)

#define    UDMA_EVENT_UART1_RX            ((PER_ID_UART1 << 2))
#define    UDMA_EVENT_UART1_TX            ((PER_ID_UART1 << 2) + 1)
#define    UDMA_EVENT_UART1_RX_IRQ        ((PER_ID_UART1 << 2) + 2)
#define    UDMA_EVENT_UART1_ERROR         ((PER_ID_UART1 << 2) + 3)

#define    UDMA_EVENT_UART2_RX            ((PER_ID_UART2 << 2))
#define    UDMA_EVENT_UART2_TX            ((PER_ID_UART2 << 2) + 1)
#define    UDMA_EVENT_UART2_RX_IRQ        ((PER_ID_UART2 << 2) + 2)
#define    UDMA_EVENT_UART2_ERROR         ((PER_ID_UART2 << 2) + 3)

#define    UDMA_EVENT_I2C0_RX             ((PER_ID_I2C0 << 2))
#define    UDMA_EVENT_I2C0_TX             ((PER_ID_I2C0 << 2) + 1)
#define    UDMA_EVENT_I2C0_CMD            ((PER_ID_I2C0 << 2) + 2)
#define    UDMA_EVENT_I2C0_EOT            ((PER_ID_I2C0 << 2) + 3)

#define    UDMA_EVENT_HYPERBUS_RX         ((PER_ID_HYPERBUS << 2))
#define    UDMA_EVENT_HYPERBUS_TX         ((PER_ID_HYPERBUS << 2) + 1)
#define    UDMA_EVENT_HYPERBUS_EOT        ((PER_ID_HYPERBUS << 2) + 3)

#define    UDMA_EVENT_JTAG_RX             ((PER_ID_JTAG << 2))
#define    UDMA_EVENT_JTAG_TX             ((PER_ID_JTAG << 2) + 1)

#define    UDMA_EVENT_SDIO_RX             ((PER_ID_SDIO << 2))
#define    UDMA_EVENT_SDIO_TX             ((PER_ID_SDIO << 2) + 1)
#define    UDMA_EVENT_SDIO_EOT            ((PER_ID_SDIO << 2) + 2)
#define    UDMA_EVENT_SDIO_ERROR          ((PER_ID_SDIO << 2) + 3)

#define    UDMA_EVENT_I2S_RX              ((PER_ID_I2S << 2))
#define    UDMA_EVENT_I2S_TX              ((PER_ID_I2S << 2) + 1)

#define    UDMA_EVENT_CPI_RX              ((PER_ID_CPI << 2))

#define    UDMA_EVENT_CSI2_RX0            ((PER_ID_CSI2 << 2))
#define    UDMA_EVENT_CSI2_RX1            ((PER_ID_CSI2 << 2) + 1)

#define    UDMA_EVENT_MRAM_RX             ((PER_ID_MRAM << 2))
#define    UDMA_EVENT_MRAM_TX             ((PER_ID_MRAM << 2) + 1)
#define    UDMA_EVENT_MRAM_ERASE_REF_DONE ((PER_ID_MRAM << 2) + 2)
#define    UDMA_EVENT_MRAM_TRIM_TX_DONE   ((PER_ID_MRAM << 2) + 3)

#define    UDMA_EVENT_FILTER_EOT          ((PER_ID_FILTER << 2))
#define    UDMA_EVENT_FILTER_ACT          ((PER_ID_FILTER << 2) + 1)

#define    UDMA_EVENT_RESERVED0                   128

/*! @brief SoC Event List */
#define    S_EVENT_I3C0                           129
#define    S_EVENT_I3C1                           130

#define    S_EVENT_PMU_DLC_BRIDGE_PICL_OK         133
#define    S_EVENT_PMU_DLC_BRIDGE_SCU_OK          134

#define    S_EVENT_PWM0                           135
#define    S_EVENT_PWM1                           136
#define    S_EVENT_PWM2                           137
#define    S_EVENT_PWM3                           138
#define    S_EVENT_GPIO                           139           /**< GPIO group interrupt */

#define    S_EVENT_RTC_APB                        140
#define    S_EVENT_RTC                            141

#define    S_EVENT_REF32K_CLK_RISE                168

/*! @brief FC Event List */
#define    FC_EVENT_SW0                           0              /**< GAP9 FC SW Event0 */
#define    FC_EVENT_SW1                           1              /**< GAP9 FC SW Event1 */
#define    FC_EVENT_SW2                           2              /**< GAP9 FC SW Event2 */
#define    FC_EVENT_SW3                           3              /**< GAP9 FC SW Event3 */
#define    FC_EVENT_SW4                           4              /**< GAP9 FC SW Event4 */
#define    FC_EVENT_SW5                           5              /**< GAP9 FC SW Event5 */
#define    FC_EVENT_SW6                           6              /**< GAP9 FC SW Event6 */
#define    FC_EVENT_SW7                           7              /**< GAP9 FC SW Event7 */

#define    FC_EVENT_DMA_PE                        8              /**<  */
#define    FC_EVENT_DMA_PE_IRQ                    9              /**<  */
#define    FC_EVENT_TIMER_LO                      10             /**<  */
#define    FC_EVENT_TIMER_HI                      11             /**<  */

#define    FC_EVENT_REF32K_CLK_RISE_AND_FALL      14             /**< FC Event Reference 32K Clock event */
#define    FC_EVENT_GPIO                          15

#define    FC_EVENT_PWM0                          17
#define    FC_EVENT_PWM1                          18
#define    FC_EVENT_PWM2                          19
#define    FC_EVENT_PWM3                          20
#define    FC_EVENT_PMU_CLUSTER_BUSY              21

#define    FC_EVENT_PMU_DLC_BRIDGE_PICL_OK        24
#define    FC_EVENT_PMU_DLC_BRIDGE_SCU_OK         25

#define    FC_EVENT_QUIDDIKEY                     27

#define    FC_EVENT_MPU_ERROR                     28
#define    FC_EVENT_FC_ERROR                      29
#define    FC_EVENT_FC_HP0                        30
#define    FC_EVENT_FC_HP1                        31

#define    GPIO_EVENT                             FC_EVENT_GPIO

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

static inline void SOC_EU_ResetFCMask() {

    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
        SOCEU->FC_MASK[index] = 0xFFFFFFFF;

}

static inline void SOC_EU_SetFCMask(int event_num) {

    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->FC_MASK[index] &= ~(1 << bit);
}

static inline void SOC_EU_ResetPRMask() {

    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
        SOCEU->PR_MASK[index] = 0xFFFFFFFF;

}

static inline void SOC_EU_SetPRMask(int event_num) {

    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->PR_MASK[index] &= ~(1 << bit);
}

static inline void SOC_EU_ResetCLMask() {

    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
        SOCEU->CL_MASK[index] = 0xFFFFFFFF;

}

static inline void SOC_EU_SetCLMask(int clusterId, int event_num) {
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->CL_MASK[index] &= ~(1 << bit);
}

static inline void SOC_EU_ClearFCMask(int event_num) {
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->FC_MASK[index] |= (1 << bit);
}

static inline void SOC_EU_ClearPRMask(int event_num) {
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->PR_MASK[index] |= (1 << bit);
}

static inline void SOC_EU_ClearCLMask(int clusterId, int event_num) {
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->CL_MASK[index] |= (1 << bit);
}


static inline void SOC_EU_SetMask(uint32_t mask) {
    SOCEU->EVENT = mask;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_SOC_EVENT_UNIT_H_*/
