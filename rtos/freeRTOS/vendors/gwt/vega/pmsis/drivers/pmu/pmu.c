/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage)
{
    return 0;
}

void pi_pmu_mram_poweron(void)
{
    #if !defined(__PLATFORM_FPGA__)
    /* Re set mask in case. */
    /* uint32_t mask = (POWER_MANAGER_DLCPD_IMR_ICU_OK_M(1) | */
    /*                  POWER_MANAGER_DLCPD_IMR_ICU_DLY_M(1) | */
    /*                  POWER_MANAGER_DLCPD_IMR_ICU_DEN_M(1) | */
    /*                  POWER_MANAGER_DLCPD_IMR_ICU_UPD_M(1) | */
    /*                  POWER_MANAGER_DLCPD_IMR_SCU_FL_M(1)); */
    
    //hal_pmu_irq_flag_clear(0xFF);
    //hal_pmu_irq_mask_set(0x13F);
    int irq = disable_irq();
    __pi_pmu_state_apply(PI_PMU_MRAM_ID, PI_PMU_STATE_ON, 0);
    __pi_pmu_wait_end_of_sequence(PI_PMU_MRAM_ID);
    restore_irq(irq);
    #endif  /* __PLATFORM_FPGA__ */
}

void pi_pmu_mram_poweroff(void)
{
    #if !defined(__PLATFORM_FPGA__)
    __pi_pmu_state_apply(PI_PMU_MRAM_ID, PI_PMU_STATE_OFF, 0);
    #endif  /* __PLATFORM_FPGA__ */
}

void pi_pmu_power_domain_change(pi_pmu_domain_e pd, pi_pmu_domain_state_e state,
                                uint32_t flags)
{
    uint8_t power_domain = 0;
    switch (pd)
    {
    case PI_PMU_DOMAIN_FC :
        power_domain = (uint8_t) PI_PMU_CHIP_ID;
        break;

    case PI_PMU_DOMAIN_CL :
        power_domain = (uint8_t) PI_PMU_CLUSTER_ID;
        break;

    /* case PI_PMU_DOMAIN_MRAM : */
    /*     power_domain = (uint8_t) PI_PMU_MRAM_ID; */
    /*     break; */

    default :
        break;
        //power_domain = 0;
    }
    //printf("PMU on pd=%x, power_domain=%x\n", pd, power_domain);
    //power_domain=0;
    __pi_pmu_state_apply(power_domain, state, flags);
}
