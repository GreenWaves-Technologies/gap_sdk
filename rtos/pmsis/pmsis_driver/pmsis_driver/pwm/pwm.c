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
    struct pi_pwm_conf *conf = (struct pi_pwm_conf *) device->config;
    /* Data will contain PWM ID, no special actions. */
    uint32_t pwm_ch = ((((uint8_t) conf->ch_id) << 16) | ((uint8_t) conf->pwm_id));
    device->data = (void *) ((uint32_t) pwm_ch);

    return __pi_pwm_open(conf->pwm_id, conf);
}

void pi_pwm_close(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    __pi_pwm_close(pwm_id);
}

int32_t pi_pwm_ioctl(struct pi_device *device, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    return __pi_pwm_ioctl(pwm_id, cmd, arg);
}

uint32_t pi_pwm_counter_get(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    return __pi_pwm_counter_get(pwm_id);
}


int32_t pi_pwm_duty_cycle_set(struct pi_device *device,
                              uint32_t frequency, uint8_t duty_cycle)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFFFF;
    pi_pwm_channel_e ch_id = (((uint32_t) device->data) >> 16) & 0xFFFF;
    uint16_t th_hi = pi_freq_get(PI_FREQ_DOMAIN_FC) / frequency;
    uint16_t th_lo = 1;
    /* threshold holds frequency value. */
    uint32_t threshold = (th_hi << 16) | th_lo;
    /* th_channel holds duty cycle. */
    uint16_t th_channel = (th_hi * duty_cycle) / 100;

    /* Set counter start, end. */
    __pi_pwm_ioctl(pwm_id, PI_PWM_TIMER_THRESH, (void *) threshold);

    /* Set channel threshold, mode. */
    /* Channel config mode: toggle -> regular periods. Easier to setup duty cycle/pulse width. */
    struct pi_pwm_ioctl_ch_config ch_conf = {0};
    ch_conf.ch_threshold = th_channel;
    ch_conf.config = PI_PWM_TOGGLE;
    ch_conf.channel = ch_id;
    __pi_pwm_ioctl(pwm_id, PI_PWM_CH_CONFIG, (void *) &ch_conf);
    return 0;
}
