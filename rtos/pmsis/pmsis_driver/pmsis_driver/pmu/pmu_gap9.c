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

#include "pmsis_driver/pmu/pmsis_pmu.h"
#include "pmsis_hal/pmsis_hal.h"

#ifdef __GAP9__

pmu_state_t pmu_state = {0, 0, {0, 0, 0, 0}, {0, 0, 0}};
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
pmu_state_t pmu_state = {0, 0, {0, 0, 0, 0}, {0, 0, 0}};

#define PMU_STATE_SOC_NV_CLUSTER_OFF    0x01
#define PMU_STATE_SOC_LV_CLUSTER_OFF    0x02
#define PMU_STATE_SOC_NV_CLUSTER_NV     0x04
#define PMU_STATE_SOC_LV_CLUSTER_LV     0x08
#define PMU_STATE_DEEP_SLEEP            0x10
#define PMU_STATE_RETENTIVE_SLEEP       0x20

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
void pmu_init()
{
    #if !defined( __PLATFORM_FPGA__)

    soc_eu_set_fc_mask(S_EVENT_PMU_DLC_BRIDGE_SCU_OK);

    #endif
}

int pmu_set_voltage(uint32_t voltage, uint32_t frequency_check)
{
    return 0;
}

void pmu_deinit(int retentive, pmu_system_state_t wakeup_state)
{
}

#include <stdio.h>
#ifdef FEATURE_CLUSTER
void pmu_cluster_power_on() {
    #ifndef  __PLATFORM_FPGA__
    /* Clears previous interrupts */
    PMU_DLC->IFR = 0xFF;

    PMU_DLC->PCTRL =
        PMU_DLC_PCTRL_START(1)                      |
        PMU_DLC_PCTRL_PADDR(WIU_IFR_0)              |
        PMU_DLC_PCTRL_PWDATA(PMU_STATE_SOC_NV_CLUSTER_NV);

    //FIXME: not supposed to go like that into production
    __enable_irq();
    hal_itc_wait_for_interrupt();
    __disable_irq();

    // Unblock transactions from dc fifos to soc
    SOC_CTRL->cl_isolate = 0;

    #else
    // On the FPGA the only thing to manage is the cluster isolation
    SOC_CTRL->cl_isolate = 0;
    #endif
}

void pmu_cluster_power_off() {
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
    SOC_CTRL->cl_isolate = 1;

    // Cluster clock-gating
    ITC_WaitEvent_NOIRQ(CLUSTER_CLOCK_GATE_OK_IRQn);

    // Cluster shutdown
    ITC_WaitEvent_NOIRQ(CLUSTER_POWER_OK_IRQn);
    // We should not need to wait for power off as it is really quick but we actually do
    #endif
    #endif
}
#endif
#endif
