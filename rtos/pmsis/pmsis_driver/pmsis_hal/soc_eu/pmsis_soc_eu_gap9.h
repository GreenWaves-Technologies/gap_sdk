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

#ifndef _PMSIS_SOC_EVENT_UNIT_GAP9_H_
#define _PMSIS_SOC_EVENT_UNIT_GAP9_H_


/*!
 * @addtogroup soc_eu
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* FC SOC domain events, all delegated by FC_SOC_EVENT_IRQn = 26 */

/*! @brief UDMA Peripheral ID List */

/*! @brief SoC Event List */
#define    S_EVENT_PMU_CLUSTER_POWER_ON           128

#define    S_EVENT_PMU_CLUSTER_BUSY               131
#define    S_EVENT_PMU_CLUSTER_CLOCK_GATING       132
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

#if 0
/*! @brief FC Event List */
#define    FC_EVENT_SW0                           0              /**< VEGA FC SW Event0 */
#define    FC_EVENT_SW1                           1              /**< VEGA FC SW Event1 */
#define    FC_EVENT_SW2                           2              /**< VEGA FC SW Event2 */
#define    FC_EVENT_SW3                           3              /**< VEGA FC SW Event3 */
#define    FC_EVENT_SW4                           4              /**< VEGA FC SW Event4 */
#define    FC_EVENT_SW5                           5              /**< VEGA FC SW Event5 */
#define    FC_EVENT_SW6                           6              /**< VEGA FC SW Event6 */
#define    FC_EVENT_SW7                           7              /**< VEGA FC SW Event7 */

#define    FC_EVENT_DMA_PE                        8              /**<  */
#define    FC_EVENT_DMA_PE_IRQ                    9              /**<  */
#define    FC_EVENT_TIMER_LO                      10             /**<  */
#define    FC_EVENT_TIMER_HI                      11             /**<  */

#define    FC_EVENT_REF32K_CLK_RISE_AND_FALL      14             /**< FC Event Reference 32K Clock event */
#define    FC_EVENT_GPIO                          15
//#define    FC_EVENT_RTC                           16
#define    FC_EVENT_PWM0                          17
#define    FC_EVENT_PWM1                          18
#define    FC_EVENT_PWM2                          19
#define    FC_EVENT_PWM3                          20
#define    FC_EVENT_PMU_CLUSTER_BUSY              21

#define    FC_EVENT_PMU_DLC_BRIDGE_PICL_OK        24
#define    FC_EVENT_PMU_DLC_BRIDGE_SCU_OK         25
#define    FC_EVENT_PMU_DLC_BRIDGE_SCU_OK         25
#define    FC_EVENT_QUIDDIKEY                     26

#define    FC_EVENT_MPU_ERROR                     28
#define    FC_EVENT_FC_ERROR                      29
#define    FC_EVENT_FC_HP0                        30
#define    FC_EVENT_FC_HP1                        31

#define    GPIO_EVENT                             FC_EVENT_GPIO
#endif
/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

static inline void hal_soc_eu_reset_fc_mask()
{

    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
    {
        SOCEU->FC_MASK[index] = 0xFFFFFFFF;
    }

}

static inline void hal_soc_eu_set_fc_mask(int event_num)
{

    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->FC_MASK[index] &= ~(1 << bit);
}

static inline void hal_soc_eu_reset_pr_mask() 
{
    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
    {
        SOCEU->PR_MASK[index] = 0xFFFFFFFF;
    }
}

static inline void hal_soc_eu_set_pr_mask(int event_num)
{
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->PR_MASK[index] &= ~(1 << bit);
}

static inline void hal_soc_eu_reset_cl_mask()
{
    for (int index = 0; index < SOC_EVENTS_REG_NUM; index++)
    {
        SOCEU->CL_MASK[index] = 0xFFFFFFFF;
    }
}

static inline void hal_soc_eu_set_cl_mask(int clusterId, int event_num)
{
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->CL_MASK[index] &= ~(1 << bit);
}

static inline void hal_soc_eu_clear_fc_mask(int event_num)
{
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->FC_MASK[index] |= (1 << bit);
}

static inline void hal_soc_eu_clear_pr_mask(int event_num)
{
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->PR_MASK[index] |= (1 << bit);
}

static inline void hal_soc_eu_clear_cl_mask(int clusterId, int event_num)
{
    int index = (event_num >> 5);
    int bit   = (event_num & 0x1F);

    SOCEU->CL_MASK[index] |= (1 << bit);
}


static inline void hal_soc_eu_set_mask(uint32_t mask)
{
    SOCEU->EVENT = mask;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_SOC_EVENT_UNIT_H_*/
