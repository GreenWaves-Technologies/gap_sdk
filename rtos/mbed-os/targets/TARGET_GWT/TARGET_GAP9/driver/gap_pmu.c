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

#include "gap_pmu.h"
#include "gap_soc_eu.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef FEATURE_CLUSTER
/*! @brief For PMU events
  bit0 : MSP
  bit1 : ICU_MODE_CHANGED
  bit2 : ICU_OK
  bit3 : ICU_DELAYED
  bit4 : PICL_OK
  bit5 : SCU_OK
*/
uint32_t pmu_events_status;
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief pmu */
pmu_state_t PMU_State = {0, 0, {0, 0, 0, 0}, {0, 0, 0}};

/* Use WIU_IFR_1 */
#define PMU_STATE_CLUSTER_ON                           (13)
#define PMU_STATE_CLUSTER_OFF                          (12)
#define PMU_STATE_CSI2_ON                              (11)
#define PMU_STATE_CSI2_OFF                             (10)
#define PMU_STATE_MRAM_ON                              (9)
#define PMU_STATE_MRAM_OFF                             (8)
/* Use WIU_IFR_0 */
#define PMU_STATE_SMART_WAKE_ON                        (7)
#define PMU_STATE_SMART_WAKE_OFF                       (6)
#define PMU_STATE_DEEP_SLEEP_RETENTIVE_SMART_WAKE_OFF  (5)
#define PMU_STATE_DEEP_SLEEP_ALL_OFF_SMART_WAKE_OFF    (4)
#define PMU_STATE_SOC_ON_CLUSTER_ON_MRAM_ON            (3)
#define PMU_STATE_SOC_ON_CLUSTER_ON_MRAM_OFF           (2)
#define PMU_STATE_SOC_ON_CLUSTER_OFF_MRAM_ON           (1)
#define PMU_STATE_SOC_ON_CLUSTER_OFF_MRAM_OFF          (0)

#define READ_PMU_STATE_INDEX_MASK                      (0x7U)
#define READ_PMU_STATE_INDEX_SHIFT                     (0U)
#define READ_PMU_STATE_INDEX(x)                        (((uint32_t)(((uint32_t)(x)) & READ_PMU_STATE_INDEX_MASK)) /* >> READ_PMU_STATE_INDEX_SHIFT */)
#define READ_PMU_STATE_IFR_MASK                        (0x8U)
#define READ_PMU_STATE_IFR_SHIFT                       (3U)
#define READ_PMU_STATE_IFR(x)                          (((uint32_t)(((uint32_t)(x)) & READ_PMU_STATE_IFR_MASK)) >> READ_PMU_STATE_IFR_SHIFT)

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief pmu init.
 *
 * This function initialize the pmu.
 *
 * @note .
 */
void PMU_Init()
{
    #if !defined( __PLATFORM_FPGA__)

    SOC_EU_SetFCMask(S_EVENT_PMU_DLC_BRIDGE_SCU_OK);

    #endif
}

int PMU_SetVoltage(uint32_t voltage, uint32_t frequency_check)
{
    return 0;
}

void PMU_DeInit(int retentive, pmu_system_state_t wakeup_state)
{
}

#ifdef FEATURE_CLUSTER
void PMU_ClusterPowerOn() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_1)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_CLUSTER_ON)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif

    // On the FPGA the only thing to manage is the cluster isolation
    SOC_CTRL->CLUSTER_ISO = 0;
}

void PMU_ClusterPowerOff() {
    #if 0
    #ifndef __PLATFORM_FPGA__
    volatile int i = 0;
    for (i = 0; i < 20; i++);

    // Wait until cluster is not busy anymore as isolating it while
    // AXI transactions are sent would break everything
    // This part does not need to be done asynchronously as the caller is supposed to make
    // sure the cluster is not active anymore..
    while(SOC_CTRL->CLUSTER_BUSY)
        ITC_WaitEvent_NOIRQ(CLUSTER_NOT_BUSY_IRQn);

    // Block transactions from dc fifos to soc
    SOC_CTRL->CLUSTER_ISO = 1;
    #endif
    #endif
}
#endif

void PMU_MramPowerOn() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_1)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_MRAM_ON)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}

void PMU_MramPowerOff() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_1)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_MRAM_OFF)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}

void PMU_CSI2PowerOn() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_1)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_CSI2_ON)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}

void PMU_CSI2PowerOff() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_1)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_CSI2_OFF)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}

void PMU_SmartWakeOn() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_0)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_SMART_WAKE_ON)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}

void PMU_SmartWakeOff() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_0)              |
        PMU_DLC_PCTRL_PWDATA( (1 << READ_PMU_STATE_INDEX(PMU_STATE_SMART_WAKE_OFF)) );

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    #endif
}
