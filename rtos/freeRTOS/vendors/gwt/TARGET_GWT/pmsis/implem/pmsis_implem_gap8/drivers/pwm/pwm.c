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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_pwm_conf_init(struct pi_pwm_conf *conf)
{
    __pi_pwm_conf_init(conf);
}

int32_t pi_pwm_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_pwm_conf *conf = (struct pi_pwm_conf *) device->config;
    /* Data will contain PWM ID and channel ID. */
    status = __pi_pwm_open(conf, (uint32_t **) &(device->data));
    uint32_t pwm_ch = ((uint32_t) device->data);
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
    PWM_TRACE("PWM(%d) open status %ld, data = %lx.\n",
              pwm_id, status, pwm_ch);
    return status;
}

void pi_pwm_close(struct pi_device *device)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    __pi_pwm_close(pwm_ch);
}

int32_t pi_pwm_ioctl(struct pi_device *device, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    return __pi_pwm_ioctl(pwm_ch, cmd, arg);
}

uint32_t pi_pwm_counter_get(struct pi_device *device)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    return __pi_pwm_counter_get(pwm_ch);
}

int32_t pi_pwm_duty_cycle_set(struct pi_device *device, uint32_t frequency,
                              uint8_t duty_cycle)
{
    uint32_t pwm_ch = ((uint32_t) device->data);
    return __pi_pwm_duty_cycle_set(pwm_ch, frequency, duty_cycle);
}
