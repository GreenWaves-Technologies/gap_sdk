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
    soc_ctrl_safe_pmu_rar_t dcdc_regulator;
    soc_ctrl_safe_pmu_sleepctrl_t sleepctrl;
    soc_ctrl_safe_pmu_sleepctrl_t sleepcfg;
    uint32_t cur_voltage;
    pi_pmu_state_e pmu_state;
    pi_pmu_domain_state_e cluster_state;
    pi_pmu_sleep_mode_e sleep_state;
};

/* Maestro internal events */
#define PI_PMU_MAESTRO_EVENT_ICU_OK              ( 1 << 0 )
#define PI_PMU_MAESTRO_EVENT_ICU_DELAYED         ( 1 << 1 )
#define PI_PMU_MAESTRO_EVENT_MODE_CHANGED        ( 1 << 2 )
#define PI_PMU_MAESTRO_EVENT_PICL_OK             ( 1 << 3 )
#define PI_PMU_MAESTRO_EVENT_SCU_OK              ( 1 << 4 )

/**
 * Default RAR = 0x0509090d
 * Regulator setting :
 * V[4:0] = 0x05 + ((Vr - 800) / 50)
 * Vr = ((V[4:0] - 0x05) * 50) + 800
 */
#define PI_PMU_DCDC_DEFAULT_NV      ( 1200 )
#define PI_PMU_DCDC_DEFAULT_MV      ( 1000 )
#define PI_PMU_DCDC_DEFAULT_LV      ( 1000 )
#define PI_PMU_DCDC_DEFAULT_RET     ( 800 )
#define PI_PMU_DCDC_RANGE           ( 5 )
#define PI_PMU_DCDC_RANGE_MASK      ( 0x1F )
#define PI_PMU_DCDC_LOW_DCDC_VALUE  ( 0x05 )
#define PI_PMU_DCDC_LOW_MV_VALUE    ( 800 )
#define PI_PMU_DCDC_STEP_MV         ( 50 )

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_pmu_init(void);

int __pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage);

int __pi_pmu_state_get(pi_pmu_domain_e domain);

int __pi_pmu_boot_state_get(pi_pmu_domain_e domain);

int __pi_pmu_sleep_mode_set(pi_pmu_domain_e domain, struct pi_pmu_sleep_conf_s *conf);

int __pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain);

#if defined(FEATURE_CLUSTER)
void __pi_pmu_cluster_power_on(void);

void __pi_pmu_cluster_power_off(void);
#endif  /* FEATURE_CLUSTER */

#endif  /* __PMSIS_IMPLEM_DRIVERS_PMU_PMU_API_H__ */
