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

static struct pwm_data_s *g_pwm_data[ARCHI_NB_PWM] = {NULL};

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_pwm_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t periph_id = event - SOC_EVENT_PWM(0);

    struct pwm_data_s *driver_data = g_pwm_data[periph_id];
    if (driver_data->event_task != NULL)
    {
        pi_task_push(driver_data->event_task);
    }
}

static void __pi_pwm_timer_conf_set(uint8_t pwm_id, struct pi_pwm_conf *conf)
{
    uint32_t config = (uint32_t) conf->timer_conf;
    config |= (conf->input_src << PI_PWM_CONFIG_INSEL_OFFSET);
    config |= (conf->prescaler << PI_PWM_CONFIG_PRESCALER_OFFSET);
    hal_pwm_config_mask_set(pwm_id, config);
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

static int32_t __pi_pwm_user_cb_attach(uint8_t pwm_id, pi_task_t *cb)
{
    struct pwm_data_s *driver_data = g_pwm_data[pwm_id];
    if (driver_data == NULL)
    {
        PWM_TRACE_ERR("Error PWM(%d) device not opened !\n", driver_data->device_id);
        return -11;
    }
    PWM_TRACE("PWM(%d) attaching event callback=%lx.\n", driver_data->device_id, cb);
    driver_data->event_task = cb;
    hal_soc_eu_set_fc_mask(SOC_EVENT_PWM(driver_data->device_id));
    return 0;
}

static void __pi_pwm_command_set(uint8_t pwm_id, pi_pwm_cmd_e cmd)
{
    hal_pwm_cmd_set(pwm_id, cmd);
}

static void __pi_pwm_timer_freq_reset(uint8_t pwm_id)
{
    struct pwm_data_s *driver_data = g_pwm_data[pwm_id];

    /* Stop the PWM timer first. */
    PWM_TRACE("Stop PWM(%d) timer and reset frequency to 0.\n", pwm_id);
    __pi_pwm_command_set(pwm_id, PI_PWM_CMD_STOP);
    driver_data->frequency = 0;
}

static void __pi_pwm_freq_cb(void *args)
{
    uint32_t irq = __disable_irq();
    uint32_t pwm_ch = (uint32_t) args;
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
    uint8_t ch_id = (uint8_t) PI_PWM_CHANNEL_ID(pwm_ch);
    struct pwm_data_s *driver_data = g_pwm_data[pwm_id];

    /* Counter start and end. */
    uint32_t th_hi = 0;
    uint16_t th_lo = 0;
    /* th_channel holds duty cycle. */
    uint16_t th_channel = th_hi;

    /* Stop PWM first. */
    __pi_pwm_command_set(pwm_id, PI_PWM_CMD_STOP);

    /* Counter start and end. */
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_FC);
    th_hi = periph_freq / driver_data->frequency;
    th_lo = 1;
    if (th_hi > 0xFFFF)
    {
        PWM_TRACE_ERR("PWM(%d) error : can not set frequency, SoC frequency is too high."
                      "Use prescaler to slow down clock or lower SoC frequency."
                      "SoC_freq=%ld, PWM_freq=%ld\n",
                      driver_data->device_id, periph_freq, driver_data->frequency);
        return;
    }
    PWM_TRACE("PWM(%d) updating timer threshold=%lx\n", driver_data->device_id, th_hi);
    /* Set counter start, end. */
    __pi_pwm_threshold_set(pwm_id, th_lo, th_hi);

    /* th_channel holds duty cycle. */
    th_channel = th_hi;

    for (uint8_t i=0; i < (uint8_t) ARCHI_NB_CHANNEL_PER_PWM; i++)
    {
        uint8_t duty_cycle = driver_data->duty_cycle[i];
        PWM_TRACE("PWM(%d) duty_cycle[%d]=%d\n", driver_data->device_id, i, duty_cycle);
        if (duty_cycle != 0xFF)
        {
            if (duty_cycle == 0)
            {
                th_channel = 0;
            }
            else if (duty_cycle != 100)
            {
                th_channel = (th_hi * (100 - duty_cycle)) / 100;
            }

            PWM_TRACE("PWM(%d) setting channel=%d\n", driver_data->device_id, i);

            /* Set channel threshold, mode. */
            __pi_pwm_channel_config_set(pwm_id, ch_id, th_channel, PI_PWM_SET_CLEAR);
        }
    }
    /* Restart PWM after update. */
    __pi_pwm_command_set(pwm_id, PI_PWM_CMD_START);
    __restore_irq(irq);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void __pi_pwm_conf_init(struct pi_pwm_conf *conf)
{
    conf->device = PI_DEVICE_PWM_TYPE;
    conf->pwm_id = 0;
    conf->ch_id = PI_PWM_CHANNEL0;
    conf->input_src = 0;
    conf->timer_conf = PI_PWM_EVT_EACH_CLK_CYCLE |
                       PI_PWM_CLKSEL_FLL |
                       PI_PWM_UPDOWNSEL_RESET;
    conf->prescaler = 0;
}

int32_t __pi_pwm_open(struct pi_pwm_conf *conf, uint32_t **device_data)
{
    if (((uint8_t) ARCHI_NB_PWM < conf->pwm_id) ||
        ((uint8_t) ARCHI_NB_CHANNEL_PER_PWM < conf->ch_id))
    {
        PWM_TRACE_ERR("Wrong parameters : pwm_id=%d, ch_id=%d\n", conf->pwm_id, conf->ch_id);
        return -11;
    }

    struct pwm_data_s *driver_data = g_pwm_data[(uint8_t) conf->pwm_id];
    if (driver_data == NULL)
    {
        driver_data = (struct pwm_data_s *) pi_l2_malloc(sizeof(struct pwm_data_s));
        if (driver_data == NULL)
        {
            PWM_TRACE_ERR("Error allocating PWM driver data.\n");
            return -12;
        }
        driver_data->frequency = 0;
        driver_data->nb_open = 0;
        driver_data->device_id = conf->pwm_id;
        driver_data->event_task = NULL;
        driver_data->duty_cycle[0] = 0xFF;
        driver_data->duty_cycle[1] = 0xFF;
        driver_data->duty_cycle[2] = 0xFF;
        driver_data->duty_cycle[3] = 0xFF;
        g_pwm_data[conf->pwm_id] = driver_data;

        /* Set handler. */
        pi_fc_event_handler_set(SOC_EVENT_PWM(driver_data->device_id), __pi_pwm_handler);

        /* Enable SOC event propagation to FC. */
        //hal_soc_eu_set_fc_mask(SOC_EVENT_PWM(driver_data->device_id));

        /* Disable PWM CG. */
        hal_pwm_ctrl_cg_disable(driver_data->device_id);

        /* Setup PWM timer. */
        __pi_pwm_timer_conf_set(driver_data->device_id, conf);

        /* Attach freq callback. */
        uint32_t pwm_id = conf->pwm_id;
        pi_freq_callback_init(&(driver_data->pwm_freq_cb), __pi_pwm_freq_cb, (void *) pwm_id);
        pi_freq_callback_add(&(driver_data->pwm_freq_cb));
    }
    driver_data->nb_open++;
    PWM_TRACE("PWM(%d) opened %ld times\n", driver_data->device_id, driver_data->nb_open);

    *device_data = (uint32_t *) ((((uint8_t) conf->ch_id) << PI_PWM_CHANNEL_ID_SHIFT) |
                                 (((uint8_t) conf->pwm_id) << PI_PWM_TIMER_ID_SHIFT));
    return 0;
}

void __pi_pwm_close(uint32_t pwm_ch)
{
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
    struct pwm_data_s *driver_data = g_pwm_data[pwm_id];
    driver_data->nb_open--;
    PWM_TRACE("PWM(%d) opened %ld times.\n", driver_data->device_id, driver_data->nb_open);
    if (driver_data->nb_open == 0)
    {
        PWM_TRACE("Closing and CG PWM(%d).\n", driver_data->device_id);

        /* Remove freq callback. */
        pi_freq_callback_remove(&(driver_data->pwm_freq_cb));

        /* Free allocated structure. */
        pi_l2_free(g_pwm_data[pwm_id], sizeof(struct pwm_data_s));

        /* Clear handler. */
        pi_fc_event_handler_clear(SOC_EVENT_PWM(pwm_id));

        /* Disable SOC event propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_PWM(pwm_id));

        /* Enable PWM CG. */
        hal_pwm_ctrl_cg_enable(pwm_id);
    }
}

int32_t __pi_pwm_ioctl(uint32_t pwm_ch, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
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

    case PI_PWM_RESET_FREQ :
        __pi_pwm_timer_freq_reset(pwm_id);
        return 0;

    case PI_PWM_ATTACH_CB :
        return __pi_pwm_user_cb_attach(pwm_id, cb_task);

    default :
        return -1;
    }
}

uint32_t __pi_pwm_counter_get(uint32_t pwm_ch)
{
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
    return hal_pwm_counter_get(pwm_id);
}

int32_t __pi_pwm_duty_cycle_set(uint32_t pwm_ch, uint32_t pwm_freq,
                                uint8_t duty_cycle)
{
    uint8_t pwm_id = (uint8_t) PI_PWM_TIMER_ID(pwm_ch);
    uint8_t ch_id = (uint8_t) PI_PWM_CHANNEL_ID(pwm_ch);

    struct pwm_data_s *driver_data = g_pwm_data[pwm_id];
    if (driver_data == NULL)
    {
        PWM_TRACE_ERR("Error PWM(%d) device not opened !\n", driver_data->device_id);
        return -11;
    }
    if ((100 < duty_cycle))
    {
        PWM_TRACE_ERR("Error duty cycle value. It should be 0 <= dc <= 100.\n");
        return -12;
    }

    /* Counter start and end. */
    uint32_t th_hi = 0;
    uint16_t th_lo = 1;
    /* th_channel holds duty cycle. */
    uint16_t th_channel = th_hi;

    if (driver_data->frequency == 0)
    {
        driver_data->frequency = pwm_freq;
        uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_FC);
        /* Counter start and end. */
        th_hi = periph_freq / pwm_freq;
        th_lo = 1;
        if (th_hi > 0xFFFF)
        {
            PWM_TRACE_ERR("PWM(%d) error : can not set frequency, SoC frequency is too high."
                          "Use prescaler to slow down clock or lower SoC frequency."
                          "SoC_freq=%ld, PWM_freq=%ld\n",
                          driver_data->device_id, periph_freq, pwm_freq);
            return -13;
        }
        /* Set counter start, end. */
        __pi_pwm_threshold_set(pwm_id, th_lo, th_hi);

    }
    else
    {
        th_hi = hal_pwm_threshold_get(pwm_id);
        //th_lo = 1;
        th_hi = th_hi >> 16;
    }
    if (driver_data->frequency != pwm_freq)
    {
        PWM_TRACE_ERR("PWM(%d) error : frequency in use is different, PWM_freq=%ld, freq=%ld\n",
                      driver_data->device_id, driver_data->frequency, pwm_freq);
        return -14;
    }
    driver_data->duty_cycle[ch_id] = duty_cycle;

    /* th_channel holds duty cycle. */
    th_channel = th_hi;
    if (duty_cycle == 0)
    {
        th_channel = 0;
    }
    else if (duty_cycle != 100)
    {
        th_channel = (th_hi * (100 - duty_cycle)) / 100;
    }

    PWM_TRACE("Setting PWM(%d)->CH_%d : freq=%ld, duty_cycle=%d\n",
              pwm_id, ch_id, pwm_freq, duty_cycle);
    /* Set channel threshold, mode. */
    __pi_pwm_channel_config_set(pwm_id, ch_id, th_channel, PI_PWM_SET_CLEAR);
    return 0;
}
