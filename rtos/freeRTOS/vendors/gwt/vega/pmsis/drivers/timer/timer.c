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

#include "pmsis/implem/drivers/timer/timer.h"
#include "pmsis/targets/target.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TIMER_INIT  (0x1)
#define TIMER_ENA   (0x2)

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static uint32_t __g_timer_state = 0;

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_timer_init(timer_e timer, timer_cfg_u cfg, uint32_t cmp_val)
{
    switch (timer)
    {
    case FC_TIMER_0 :
        //fc_timer(0)->CMP_LO = cmp_val;
        hal_write32(&(fc_timer(0)->CMP_LO), cmp_val);
        hal_write32(&(fc_timer(0)->VALUE_LO), 0);
        hal_write32(&(fc_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case FC_TIMER_1 :
        hal_write32(&(fc_timer(0)->CMP_HI), cmp_val);
        hal_write32(&(fc_timer(0)->VALUE_HI), 0);
        hal_write32(&(fc_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #if defined(FEATURE_CLUSTER)
    case CL_TIMER_0 :
        hal_write32(&(cl_timer(0)->CMP_LO), cmp_val);
        hal_write32(&(cl_timer(0)->VALUE_LO), 0);
        hal_write32(&(cl_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case CL_TIMER_1 :
        hal_write32(&(cl_timer(0)->CMP_HI), cmp_val);
        hal_write32(&(cl_timer(0)->VALUE_HI), 0);
        hal_write32(&(cl_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #endif  /* FEATURE_CLUSTER */
    default :
        break;
    }
    __g_timer_state |= (TIMER_INIT << (((uint32_t) timer) << 1));
}

void pi_timer_reset(timer_e timer)
{
    timer_cfg_u cfg = {0};
    cfg.field.reset = 1;
    switch (timer)
    {
    case FC_TIMER_0 :
        hal_or32(&(fc_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case FC_TIMER_1 :
        hal_or32(&(fc_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #if defined(FEATURE_CLUSTER)
    case CL_TIMER_0 :
        hal_or32(&(cl_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case CL_TIMER_1 :
        hal_or32(&(cl_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #endif  /* FEATURE_CLUSTER */
    default :
        break;
    }
}

void pi_timer_start(timer_e timer)
{
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    switch (timer)
    {
    case FC_TIMER_0 :
        hal_or32(&(fc_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case FC_TIMER_1 :
        hal_or32(&(fc_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #if defined(FEATURE_CLUSTER)
    case CL_TIMER_0 :
        hal_or32(&(cl_timer(0)->CFG_REG_LO), cfg.word);
        break;
    case CL_TIMER_1 :
        hal_or32(&(cl_timer(0)->CFG_REG_HI), cfg.word);
        break;
    #endif  /* FEATURE_CLUSTER */
    default :
        break;
    }
    __g_timer_state |= (TIMER_ENA << (((uint32_t) timer) << 1));
}

void pi_timer_stop(timer_e timer)
{
    switch (timer)
    {
    case FC_TIMER_0 :
        hal_write32(&(fc_timer(0)->CFG_REG_LO),
                    hal_read32(&(fc_timer(0)->CFG_REG_LO)) & ~TIMERL_CFG_REG_LOW_ENABLE_Msk);
        break;
    case FC_TIMER_1 :
        hal_write32(&(fc_timer(0)->CFG_REG_HI),
                    hal_read32(&(fc_timer(0)->CFG_REG_HI)) & ~TIMERL_CFG_REG_LOW_ENABLE_Msk);
        break;
    #if defined(FEATURE_CLUSTER)
    case CL_TIMER_0 :
        hal_write32(&(cl_timer(0)->CFG_REG_LO),
                    hal_read32(&(cl_timer(0)->CFG_REG_LO)) & ~TIMERL_CFG_REG_LOW_ENABLE_Msk);
        break;
    case CL_TIMER_1 :
        hal_write32(&(cl_timer(0)->CFG_REG_HI),
                    hal_read32(&(cl_timer(0)->CFG_REG_HI)) & ~TIMERL_CFG_REG_LOW_ENABLE_Msk);
        break;
    #endif  /* FEATURE_CLUSTER */
    default :
        break;
    }
    __g_timer_state &= ~(TIMER_ENA << (((uint32_t) timer) << 1));
}

uint32_t pi_timer_value_read(timer_e timer)
{
    switch (timer)
    {
    case FC_TIMER_0 :
        return hal_read32(&(fc_timer(0)->VALUE_LO));
    case FC_TIMER_1 :
        return hal_read32(&(fc_timer(0)->VALUE_HI));
    #if defined(FEATURE_CLUSTER)
    case CL_TIMER_0 :
        return hal_read32(&(cl_timer(0)->VALUE_LO));
    case CL_TIMER_1 :
        return hal_read32(&(cl_timer(0)->VALUE_HI));
    #endif  /* FEATURE_CLUSTER */
    default :
        return 0;
    }
}

extern uint32_t system_core_clock_get();
void pi_timer_irq_set(timer_e timer, uint32_t time_us, uint8_t one_shot)
{
    uint32_t timer_tick_us = system_core_clock_get() / 1000000;
    uint32_t cmp_val = time_us * timer_tick_us;
    if (__g_timer_state & ((TIMER_ENA << (((uint32_t) timer) << 1))))
    {
        pi_timer_stop(timer);
    }
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    cfg.field.irq_en = 1;
    cfg.field.mode = 1;
    cfg.field.one_shot = one_shot;
    pi_timer_init(timer, cfg, cmp_val);
    pi_timer_start(timer);
}
