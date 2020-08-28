/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"

typedef struct __pos_pwm_s pos_pwm_t;

typedef struct
{
    pos_pwm_t *pwm;
    int id;
    int channel_id;
    unsigned int open_count;
    uint32_t base;
} pos_pwm_timer_t;


struct __pos_pwm_s
{
    int id;
    pos_pwm_timer_t timers[ARCHI_PWM_NB_TIMERS];
    unsigned int open_count;
};


static pos_pwm_t __pos_pwm[ARCHI_PWM_NB];


void pi_pwm_conf_init(struct pi_pwm_conf *conf)
{
    conf->device = PI_DEVICE_PWM_TYPE;
    conf->pwm_id = 0;
    conf->ch_id = 0;
}

int32_t pi_pwm_open(struct pi_device *device)
{
    int irq = rt_irq_disable();

    struct pi_pwm_conf *conf = (struct pi_pwm_conf *)device->config;

    int pwm_id = conf->pwm_id / ARCHI_PWM_NB_TIMERS;
    int timer_id = conf->pwm_id % ARCHI_PWM_NB_TIMERS;

    pos_pwm_t *pwm = &__pos_pwm[pwm_id];
    pos_pwm_timer_t *timer = &pwm->timers[timer_id];

    device->data = (void *)timer;

    timer->open_count++;
    if (timer->open_count == 1)
    {
        pwm->open_count++;

        pwm_cg_set(ARCHI_PWM_ADDR, pwm_cg_get(ARCHI_PWM_ADDR) | (1<<timer_id));

        timer->base = ARCHI_PWM_ADDR + (PWM_T1_CMD_OFFSET - PWM_T0_CMD_OFFSET) * timer_id;

        timer->channel_id = conf->ch_id;

        // Configure channel mode
        ARCHI_WRITE(timer->base, PWM_T0_CONFIG_OFFSET - PWM_T0_CMD_OFFSET,
            PI_PWM_EVT_EACH_CLK_CYCLE |
            PI_PWM_CLKSEL_FLL |
            PI_PWM_UPDOWNSEL_RESET
        );
    }

    rt_irq_restore(irq);

    return 0;
}

void pi_pwm_close(struct pi_device *device)
{
    pos_pwm_timer_t *timer = (pos_pwm_timer_t *)device->data;

    int irq = rt_irq_disable();

    timer->open_count--;
    if (timer->open_count == 0)
    {
        timer->pwm->open_count--;

        pwm_cg_set(ARCHI_PWM_ADDR, pwm_cg_get(ARCHI_PWM_ADDR) & ~(1<<timer->id));
    }

    rt_irq_restore(irq);
}

int32_t pi_pwm_ioctl(struct pi_device *device, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    pi_pwm_cmd_e timer_cmd = (pi_pwm_cmd_e) arg;
    pos_pwm_timer_t *timer = (pos_pwm_timer_t *)device->data;

    unsigned int offset = 0;
    unsigned int value = 0;
    switch (cmd)
    {
        case PI_PWM_TIMER_COMMAND :
            ARCHI_WRITE(timer->base, PWM_T0_CMD_OFFSET - PWM_T0_CMD_OFFSET, timer_cmd);
            return 0;

        case PI_PWM_TIMER_THRESH :
            offset = PWM_T0_THRESHOLD_OFFSET - PWM_T0_CMD_OFFSET;
            value = (unsigned int) arg;
            ARCHI_WRITE(timer->base, offset, value);
            return 0;

        case PI_PWM_CH_CONFIG :
            offset = PWM_T0_TH_CHANNEL0_OFFSET - PWM_T0_CMD_OFFSET + (timer->channel_id)*4;
            value = (PI_PWM_SET << PWM_T3_TH_CHANNEL0_MODE_BIT) |
                    (0xFFFF << PWM_T0_TH_CHANNEL0_TH_BIT);
            ARCHI_WRITE(timer->base, offset, value);
            return 0;

        default:
            return -1;
    }

    return -1;
}

int32_t pi_pwm_duty_cycle_set(struct pi_device *device, uint32_t frequency, uint8_t duty_cycle)
{
    int irq = rt_irq_disable();

    if ((100 < duty_cycle))
    {
        printf("Error duty cycle value. It should be 0 <= dc <= 100.\n");
        return -1;
    }

    pos_pwm_timer_t *timer = (pos_pwm_timer_t *)device->data;

    uint8_t pwm_id = timer->pwm->id;
    uint16_t th_hi = pi_freq_get(PI_FREQ_DOMAIN_FC) / frequency;
    uint16_t th_lo = 1;
    /* threshold holds frequency value. */
    uint32_t threshold = (th_hi << PWM_T0_THRESHOLD_TH_HI_BIT) | th_lo;
    /* th_timer holds duty cycle. */
    uint16_t th_timer = th_hi;
    if (duty_cycle == 0)
    {
        th_timer = 0;
    }
    else if (duty_cycle != 100)
    {
        th_timer = (th_hi * (100 - duty_cycle)) / 100;
    }

    /* Set counter start, end. */
    ARCHI_WRITE(timer->base, PWM_T0_THRESHOLD_OFFSET - PWM_T0_CMD_OFFSET, threshold);

    /* Set timer threshold, mode. */
    /* timer config mode: toggle -> regular periods. Easier to setup duty cycle/pulse width. */
    ARCHI_WRITE(timer->base,
                PWM_T0_TH_CHANNEL0_OFFSET - PWM_T0_CMD_OFFSET + (timer->channel_id)*4,
                (PI_PWM_SET_CLEAR << PWM_T3_TH_CHANNEL0_MODE_BIT) |
                (th_timer << PWM_T0_TH_CHANNEL0_TH_BIT));

    rt_irq_restore(irq);

    return 0;
}


static void __attribute__((constructor)) __pos_pwm_init()
{
    for (int i=0; i<ARCHI_PWM_NB; i++)
    {
        pos_pwm_t *pwm = &__pos_pwm[i];
        pwm->open_count = 0;
        pwm->id = i;

        for (int j=0; j<ARCHI_PWM_NB_TIMERS; j++)
        {
            pos_pwm_timer_t *timer = &pwm->timers[j];
            timer->open_count = 0;
            timer->pwm = pwm;
            timer->id = j;
        }
    }
}
