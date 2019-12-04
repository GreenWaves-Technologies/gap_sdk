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

#ifndef __PWM_INTERNAL_H__
#define __PWM_INTERNAL_H__

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

void __pi_pwm_conf_init(struct pi_pwm_conf *conf);

int32_t __pi_pwm_open(uint8_t pwm_id, struct pi_pwm_conf *conf);

void __pi_pwm_close(uint8_t pwm_id);

int32_t __pi_pwm_ioctl(uint8_t pwm_id, pi_pwm_ioctl_cmd_e cmd, void *arg);

uint32_t __pi_pwm_counter_get(uint8_t pwm_id);


/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_pwm_timer_start(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    __pi_pwm_ioctl(pwm_id, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);
}

static inline void pi_pwm_timer_stop(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    __pi_pwm_ioctl(pwm_id, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_STOP);
}

#endif  /* __PWM_INTERNAL_H__ */
