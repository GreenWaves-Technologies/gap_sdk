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

#include "pmsis/targets/target.h"
#include "pmsis/implem/drivers/timer/timer.h"
#include "pmsis/rtos/os/pmsis_freq.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_timer_init(timer_e timer, timer_cfg_u cfg, uint32_t cmp_val)
{
    uint32_t timer_base = 0;
    if ((timer >= CL_TIMER_0))
    {
        timer_base = (uint32_t) cl_timer(0);
    }
    else
    {
        timer_base = (uint32_t) fc_timer((uint32_t) timer >> 1);
    }
    hal_timer_cfg_set(timer_base, cfg.word, cmp_val, 0, ((uint32_t) timer & 1));
}

void pi_timer_reset(timer_e timer)
{
    uint32_t timer_base = 0;
    if ((timer >= CL_TIMER_0))
    {
        timer_base = (uint32_t) cl_timer(0);
    }
    else
    {
        timer_base = (uint32_t) fc_timer((uint32_t) timer >> 1);
    }
    hal_timer_reset_set(timer_base, ((uint32_t) timer & 1));
}

void pi_timer_start(timer_e timer)
{
    uint32_t timer_base = 0;
    if ((timer >= CL_TIMER_0))
    {
        timer_base = (uint32_t) cl_timer(0);
    }
    else
    {
        timer_base = (uint32_t) fc_timer((uint32_t) timer >> 1);
    }
    hal_timer_start_set(timer_base, ((uint32_t) timer & 1));
}

void pi_timer_stop(timer_e timer)
{
    uint32_t timer_base = 0;
    if ((timer >= CL_TIMER_0))
    {
        timer_base = (uint32_t) cl_timer(0);
    }
    else
    {
        timer_base = (uint32_t) fc_timer((uint32_t) timer >> 1);
    }
    hal_timer_stop_set(timer_base, ((uint32_t) timer & 1));
}

uint32_t pi_timer_value_read(timer_e timer)
{
    uint32_t timer_base = 0;
    if ((timer >= CL_TIMER_0))
    {
        timer_base = (uint32_t) cl_timer(0);
    }
    else
    {
        timer_base = (uint32_t) fc_timer((uint32_t) timer >> 1);
    }
    uint32_t timer_value = hal_timer_value_get(timer_base, ((uint32_t) timer & 1));
    return timer_value;
}

extern uint32_t system_core_clock_get();
void pi_timer_irq_set(timer_e timer, uint32_t time_us, uint8_t one_shot)
{
    if (timer == SYS_TIMER)
    {
        /* FC_TIMER_0 already used by kernel. */
        return;
    }

    /* Stop timer if it was used. */
    pi_timer_stop(timer);
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    cfg.field.irq_en = 1;
    cfg.field.mode = 1;
    cfg.field.one_shot = one_shot;

    float periph_freq = 0.0;
    periph_freq = (float) system_core_clock_get();
    float timer_tick_us = periph_freq / 1000000.0;
    uint32_t cmp_val = time_us * timer_tick_us;
    /* printf("periph_freq=%ld, tick_us=%ld, time_us=%ld, cmp_val=%ld\n", */
    /*        (uint32_t) periph_freq, (uint32_t) timer_tick_us, time_us, cmp_val); */
    pi_timer_init(timer, cfg, cmp_val);
}
