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

#ifndef __PMSIS_IMPLEM_DRIVERS_PMU_PMU_API_H__
#define __PMSIS_IMPLEM_DRIVERS_PMU_PMU_API_H__

#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"
//#include "pmsis/implem/drivers/pmu/pmu.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

typedef enum
{
    PI_PMU_STATE_HV  = 0,       /*!< High voltage : 1.1V - 1.3V. */
    PI_PMU_STATE_LV  = 1,       /*!< Low voltage : 0.9V - 1.1V. */
    PI_PMU_STATE_RET = 2        /*!< Sleep mode 0.8V. */
//    PI_PMU_STATE_OFF = 3
} pi_pmu_state_e;

struct pmu_data_s
{
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
    //uint32_t cur_voltage;
    pi_pmu_domain_state_e cluster_state;
    //pi_pmu_sleep_mode_e sleep_state;
};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_pmu_init(void);

int32_t __pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage);

int32_t __pi_pmu_state_get(pi_pmu_domain_e domain);

int32_t __pi_pmu_boot_state_get(pi_pmu_domain_e domain);

int32_t __pi_pmu_sleep_mode_set(pi_pmu_domain_e domain, struct pi_pmu_sleep_conf_s *conf);

int32_t __pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain);

#if defined(FEATURE_CLUSTER)
void __pi_pmu_cluster_power_on(void);

void __pi_pmu_cluster_power_off(void);
#endif  /* FEATURE_CLUSTER */

/* Temp. */
void pi_pmu_power_domain_change(pi_pmu_domain_e pd, pi_pmu_domain_state_e state,
                                uint32_t flags);
void pi_pmu_power_domain_change_async(pi_pmu_domain_e pd, pi_pmu_domain_state_e state,
                                      uint32_t flags, pi_task_t *task);

void __pi_pmu_state_apply(uint8_t domain_id, uint8_t state, uint8_t flags);

void pi_pmu_mram_poweroff(void);
void pi_pmu_mram_poweron(void);

void __pi_pmu_wait_end_of_sequence(uint8_t domain_id);

#endif  /* __PMSIS_IMPLEM_DRIVERS_PMU_PMU_API_H__ */
