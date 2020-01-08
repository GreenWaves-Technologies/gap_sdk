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

static pi_task_t *__cb_task_array[ARCHI_NB_PWM] = {NULL};

/*******************************************************************************
 * Internal functions.
 ******************************************************************************/

static void __pi_pwm_timer_conf_set(uint8_t pwm_id, struct pi_pwm_conf *conf)
{
    uint32_t config = (uint32_t) conf->timer_conf;
    config |= (conf->input_src << PI_PWM_CONFIG_INSEL_OFFSET);
    config |= (conf->prescaler << PI_PWM_CONFIG_PRESCALER_OFFSET);
    hal_pwm_config_reg_set(pwm_id, config);
}

static void __pi_pwm_threshold_set(uint8_t pwm_id, uint16_t counter_start,
                                   uint16_t counter_end)
{
    hal_pwm_threshold_set(pwm_id, counter_start, counter_end);
}

static void __pi_pwm_channel_config_set(uint8_t pwm_id, pi_pwm_channel_e channel,
                                        uint16_t threshold, pi_pwm_ch_config_e config)
{
    hal_pwm_channel_th_mode_set(pwm_id, channel, threshold, config);
}

static void __pi_pwm_output_event_set(pi_pwm_evt_sel_e evt_sel, pi_pwm_output_evt_e evt_output)
{
    hal_pwm_ctrl_evt_cfg_set(evt_sel, evt_output);
}

static void __pi_pwm_output_event_clear(pi_pwm_evt_sel_e evt_sel)
{
    hal_pwm_ctrl_evt_cfg_disable(evt_sel);
}

static void __pi_pwm_user_cb_attach(uint8_t pwm_id, pi_task_t *cb)
{
    __cb_task_array[pwm_id] = cb;
}

static void __pi_pwm_command_set(uint8_t pwm_id, pi_pwm_cmd_e cmd)
{
    hal_pwm_cmd_set(pwm_id, cmd);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pwm_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t periph_id = event - SOC_EVENT_PWM(0);

    if (__cb_task_array[periph_id] != NULL)
    {
        pmsis_event_push(pmsis_event_get_default_scheduler(), __cb_task_array[periph_id]);
    }
}

void __pi_pwm_conf_init(struct pi_pwm_conf *conf)
{
    conf->device = PI_DEVICE_PWM_TYPE;
    conf->pwm_id = 0;
    conf->ch_id = PI_PWM_CHANNEL0;
    conf->input_src = 0;
    conf->timer_conf = PI_PWM_EVT_EACH_CLK_CYCLE |
                       PI_PWM_CLKSEL_REFCLK_32K |
                       PI_PWM_UPDOWNSEL_RESET;
    conf->prescaler = 0;
}

int32_t __pi_pwm_open(uint8_t pwm_id, struct pi_pwm_conf *conf)
{
    /* Set handler. */
    pi_fc_event_handler_set(SOC_EVENT_PWM(pwm_id), pwm_handler);
    /* Enable SOC event propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_PWM(pwm_id));

    /* Disable PWM CG. */
    hal_pwm_ctrl_cg_disable(pwm_id);

    /* Setup PWM timer. */
    __pi_pwm_timer_conf_set(pwm_id, conf);

    return 0;
}

void __pi_pwm_close(uint8_t pwm_id)
{
    /* Clear handler. */
    pi_fc_event_handler_clear(SOC_EVENT_PWM(pwm_id));
    /* Disable SOC event propagation to FC. */
    hal_soc_eu_clear_fc_mask(SOC_EVENT_PWM(pwm_id));

    /* Enable PWM CG. */
    hal_pwm_ctrl_cg_enable(pwm_id);

    /* Clear any attached callback. */
    __cb_task_array[pwm_id] = NULL;
}

int32_t __pi_pwm_ioctl(uint8_t pwm_id, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    pi_pwm_cmd_e timer_cmd = (pi_pwm_cmd_e) arg;
    struct pi_pwm_conf *conf = (struct pi_pwm_conf *) arg;
    uint32_t threshold = (uint32_t) arg;
    struct pi_pwm_ioctl_ch_config *ch_conf = (struct pi_pwm_ioctl_ch_config *) arg;
    struct pi_pwm_ioctl_evt *evt = (struct pi_pwm_ioctl_evt *) arg;
    pi_task_t *cb_task = (pi_task_t *) arg;

    switch (cmd)
    {
    case PI_PWM_TIMER_COMMAND :
        __pi_pwm_command_set(pwm_id, timer_cmd);
        return 0;

    case PI_PWM_TIMER_CONFIG :
        __pi_pwm_timer_conf_set(pwm_id, conf);
        return 0;

    case PI_PWM_TIMER_THRESH :
        __pi_pwm_threshold_set(pwm_id, threshold & 0xFFFF, threshold >> 16);
        return 0;

    case PI_PWM_CH_CONFIG :
        __pi_pwm_channel_config_set(pwm_id, ch_conf->channel, ch_conf->ch_threshold,
                                    ch_conf->config);
        return 0;

    case PI_PWM_EVENT_SET :
        __pi_pwm_output_event_set(evt->evt_sel, evt->evt_output);
        return 0;

    case PI_PWM_EVENT_CLEAR :
        __pi_pwm_output_event_clear(evt->evt_sel);
        return 0;

    case PI_PWM_ATTACH_CB :
        __pi_pwm_user_cb_attach(pwm_id, cb_task);
    default :
        return -1;
    }
}

uint32_t __pi_pwm_counter_get(uint8_t pwm_id)
{
    return hal_pwm_counter_get(pwm_id);
}
