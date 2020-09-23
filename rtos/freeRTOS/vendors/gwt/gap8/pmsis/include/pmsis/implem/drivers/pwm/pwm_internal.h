/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_DRIVERS_PWM_PWM_INTERNAL_H__
#define __PMSIS_IMPLEM_DRIVERS_PWM_PWM_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* PWM and channeld ID masks. */
#define PI_PWM_TIMER_ID_SHIFT   ( 0 )
#define PI_PWM_TIMER_ID_MASK    ( 0xFF )
#define PI_PWM_TIMER_ID(data)   ( ((data >> PI_PWM_TIMER_ID_SHIFT) & PI_PWM_TIMER_ID_MASK) )

#define PI_PWM_CHANNEL_ID_SHIFT ( 16 )
#define PI_PWM_CHANNEL_ID_MASK  ( 0xFF )
#define PI_PWM_CHANNEL_ID(data) ( ((data >> PI_PWM_CHANNEL_ID_SHIFT) & PI_PWM_CHANNEL_ID_MASK) )

struct pwm_data_s
{
    uint32_t frequency;                           /*!< Frequency set for the timer. */
    uint32_t nb_open;                             /*!< Number of open. */
    pi_task_t *event_task;                        /*!< Event task attached. */
    pi_freq_cb_t pwm_freq_cb;                     /*!< Callback associated to frequency changes. */
    uint8_t device_id;                            /*!< PWM timer ID. */
    uint8_t duty_cycle[ARCHI_NB_CHANNEL_PER_PWM]; /*!< Duty cycles. */
};

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_pwm_conf_init(struct pi_pwm_conf *conf);

int32_t __pi_pwm_open(struct pi_pwm_conf *conf, uint32_t **device_data);

void __pi_pwm_close(uint32_t pwm_ch);

int32_t __pi_pwm_ioctl(uint32_t pwm_ch, pi_pwm_ioctl_cmd_e cmd, void *arg);

uint32_t __pi_pwm_counter_get(uint32_t pwm_ch);

int32_t __pi_pwm_duty_cycle_set(uint32_t pwm_ch, uint32_t pwm_freq, uint8_t duty_cycle);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_pwm_timer_start(struct pi_device *device)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    __pi_pwm_ioctl(pwm_ch, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);
}

static inline void pi_pwm_timer_stop(struct pi_device *device)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    __pi_pwm_ioctl(pwm_ch, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_STOP);
}

#endif  /* __PMSIS_IMPLEM_DRIVERS_PWM_PWM_INTERNAL_H__ */
