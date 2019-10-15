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
/* FC SOC domain events, all delegated by FC_SOC_EVENT_IRQn = 27 */
#define    UDMA_EVENT_LVDS_RX              0
#define    UDMA_EVENT_LVDS_TX              1
#define    UDMA_EVENT_SPIM0_RX             2
#define    UDMA_EVENT_SPIM0_TX             3
#define    UDMA_EVENT_SPIM1_RX             4
#define    UDMA_EVENT_SPIM1_TX             5
#define    UDMA_EVENT_HYPERBUS_RX          6
#define    UDMA_EVENT_HYPERBUS_TX          7
#define    UDMA_EVENT_UART_RX              8
#define    UDMA_EVENT_UART_TX              9
#define    UDMA_EVENT_I2C0_RX              10
#define    UDMA_EVENT_I2C0_TX              11
#define    UDMA_EVENT_I2C1_RX              12
#define    UDMA_EVENT_I2C1_TX              13
#define    UDMA_EVENT_DMACPY_RX            14
#define    UDMA_EVENT_DMACPY_TX            15
#define    UDMA_EVENT_SAI_CH0              16
#define    UDMA_EVENT_SAI_CH1              17
#define    UDMA_EVENT_CPI_RX               18
#define    UDMA_EVENT_RESERVED0            19

#define    UDMA_EVENT_LVDS_GEN0            20
#define    UDMA_EVENT_LVDS_GEN1            21
#define    UDMA_EVENT_SPIM0_EOT            22
#define    UDMA_EVENT_SPIM1_EOT            23
#define    UDMA_EVENT_HYPERBUS_RESERVED    24
#define    UDMA_EVENT_UART_RESERVED        25
#define    UDMA_EVENT_I2C0_ERROR           26
#define    UDMA_EVENT_I2C1_ERROR           27
#define    UDMA_EVENT_I2S_RESERVED         28
#define    UDMA_EVENT_CAM_RESERVED         29
#define    UDMA_EVENT_RESERVED1            30

#define    PMU_EVENT_CLUSTER_POWER_ON      31
#define    PMU_EVENT_CLUSTER_RESERVED0     32
#define    PMU_EVENT_CLUSTER_RESERVED1     33
#define    PMU_EVENT_CLUSTER_RESERVED2     34
#define    PMU_EVENT_CLUSTER_CLOCK_GATING  35
#define    PMU_DLC_EVENT_BRIDGE_PICL_OK    36
#define    PMU_DLC_EVENT_BRIDGE_SCU_OK     37
#define    PMU_EVENTS_NUM                  7

#define    PWM0_EVENT                      38
#define    PWM1_EVENT                      39
#define    PWM2_EVENT                      40
#define    PWM3_EVENT                      41
#define    GPIO_EVENT                      42              /**< GPIO group interrupt */
#define    RTC_APB_EVENT                   43
#define    RTC_EVENT                       44
#define    EVENT_RESERVED0                 45
#define    EVENT_RESERVED1                 46
#define    EVENT_RESERVED2                 47

#define    SOC_SW_EVENT0                   48              /**< GAP8 SOC SW Event0 */
#define    SOC_SW_EVENT1                   49              /**< GAP8 SOC SW Event1 */
#define    SOC_SW_EVENT2                   50              /**< GAP8 SOC SW Event2 */
#define    SOC_SW_EVENT3                   51              /**< GAP8 SOC SW Event3 */
#define    SOC_SW_EVENT4                   52              /**< GAP8 SOC SW Event4 */
#define    SOC_SW_EVENT5                   53              /**< GAP8 SOC SW Event5 */
#define    SOC_SW_EVENT6                   54              /**< GAP8 SOC SW Event6 */
#define    SOC_SW_EVENT7                   55              /**< GAP8 SOC SW Event7 */
#define    REF32K_CLK_RISE_EVENT           56              /**< GAP8 SOC EU SW Event Reference 32K Clock event */

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

static inline void SOC_EU_SetFCMask(int evt) {
  if (evt >= 32)
    SOCEU->FC_MASK_MSB &= ~(1 << (evt-32));
  else
    SOCEU->FC_MASK_LSB &= ~(1 << evt);
}

static inline void SOC_EU_SetPRMask(int evt) {
  if (evt >= 32)
    SOCEU->PR_MASK_MSB &= ~(1 << (evt-32));
  else
    SOCEU->PR_MASK_LSB &= ~(1 << evt);
}

static inline void SOC_EU_SetCLMask(int clusterId, int evt) {
  if (evt >= 32)
    SOCEU->CL_MASK_MSB &= ~(1 << (evt-32));
  else
    SOCEU->CL_MASK_LSB &= ~(1 << evt);
}

static inline void SOC_EU_ClearFCMask(int evt) {
  if (evt >= 32)
    SOCEU->FC_MASK_MSB |= (1 << (evt-32));
  else
    SOCEU->FC_MASK_LSB |= (1 << evt);
}

static inline void SOC_EU_ClearPRMask(int evt) {
  if (evt >= 32)
    SOCEU->PR_MASK_MSB |= (1 << (evt-32));
  else
    SOCEU->PR_MASK_LSB |= (1 << evt);
}

static inline void SOC_EU_ClearCLMask(int clusterId, int evt) {
  if (evt >= 32)
    SOCEU->CL_MASK_MSB |= (1 << (evt-32));
  else
    SOCEU->CL_MASK_LSB |= (1 << evt);
}


static inline void SOC_EU_SetMask(uint32_t mask) {
    SOCEU->EVENT = mask;
}

static inline void SOC_EU_Configure(int cluster, int event, int active) {
#if SOC_SW_EVENT0 < 32
    uint32_t mask = (cluster == FC_CLUSTER_ID) ? (SOCEU->FC_MASK_LSB) : (SOCEU->CL_MASK_LSB);
    int fullEvent = event - SOC_SW_EVENT0;

    if (!active)
      mask = mask | (1<<fullEvent);
    else
      mask = mask & ~(1<<fullEvent);

    if (cluster == FC_CLUSTER_ID)
      SOCEU->FC_MASK_LSB = mask;
    else
      SOCEU->CL_MASK_LSB = mask;
#else
    uint32_t mask = (cluster == FC_CLUSTER_ID) ? (SOCEU->FC_MASK_MSB) : (SOCEU->CL_MASK_MSB);
    int fullEvent = event + SOC_SW_EVENT0 - 32;

    if (!active)
      mask = mask | (1<<fullEvent);
    else
      mask = mask & ~(1<<fullEvent);

    if (cluster == FC_CLUSTER_ID)
      SOCEU->FC_MASK_MSB = mask;
    else
      SOCEU->CL_MASK_MSB = mask;
#endif
}

/* static inline int SOC_EU_ReserveConfig(int cluster, int event) { */
/*   if (_bitfield_reserve(&soc_events_mask, event)) return -1; */
/*   SOC_EU_Configure(cluster, event, 1); */
/*   return 0; */
/* } */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_SOC_EVENT_UNIT_H_*/
