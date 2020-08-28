/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage)
{
    int32_t status = -1;
    PMU_TRACE("Power domain id=%d, setting voltage=%ld\n", domain, voltage);
    status = __pi_pmu_voltage_set(domain, voltage);
    return status;
}

int pi_pmu_state_get(pi_pmu_domain_e domain)
{
    int32_t status = -1;
    status = __pi_pmu_state_get(domain);
    PMU_TRACE("Power domain id=%d state=%d\n", domain, status);
    return status;
}

int pi_pmu_boot_state_get(pi_pmu_domain_e domain)
{
    int32_t status = -1;
    status = __pi_pmu_boot_state_get(domain);
    PMU_TRACE("Power domain id=%d state=%x\n", domain, status);
    return status;
}

int pi_pmu_sleep_mode_set(pi_pmu_domain_e domain, struct pi_pmu_sleep_conf_s *conf)
{
    int32_t status = -1;
    PMU_TRACE("Power domain id=%d, setting sleep mode\n", domain);
    status = __pi_pmu_sleep_mode_set(domain, conf);
    return status;
}

int pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain)
{
    int32_t status = -1;
    PMU_TRACE("Power doman id=%d, enable sleep mode\n", domain);
    status = __pi_pmu_sleep_mode_enable(domain);
    return status;
}
