/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include <stddef.h>
#include "gap_pwm.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to PWM bases for each instance. */
static PWM_Type *const s_pwmBases[] = PWM_BASE_PTRS;

/*! @brief Pointers to PWM IRQ handler for each instance. */
static pwm_isr_t s_pwmIsr[ARRAY_SIZE(s_pwmBases)];

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static void PWM_Enable(PWM_Type *base)
{
    uint32_t instance = PWM_GetInstance(base);

    PWM_CTRL->CH_EN |= PWM_CTRL_CG_ENA(0x1 << instance);
}

static void PWM_Disable(PWM_Type *base)
{
    uint32_t instance = PWM_GetInstance(base);

    PWM_CTRL->CH_EN &= ~PWM_CTRL_CG_ENA(0x1 << instance);
}

uint32_t PWM_GetInstance(PWM_Type *base)
{
    uint32_t instance = 0;
    for (instance = 0; instance < ARRAY_SIZE(s_pwmBases); instance++)
    {
        if (s_pwmBases[instance] == base )
            break;
    }
    assert(instance < ARRAY_SIZE(s_pwmBases));
    return instance;
}

void PWM_Init(PWM_Type *base, pwm_config_t *config)
{
    base->CFG = PWM_CONFIG_INPUT_SRC (config->inputsrc ) |
                PWM_CONFIG_INPUT_MODE(config->mode     ) |
                PWM_CONFIG_CLKSEL    (config->clksel   ) |
                PWM_CONFIG_UPDOWNSEL (config->updownsel) |
                PWM_CONFIG_PRESCALE  (config->prescale);
}

void PWM_Deinit(PWM_Type *base)
{
    PWM_StopTimer(base);

    base->TH = 0;

    for (uint8_t i = 0; i < 4; i ++)
    {
        base->CH_TH[i] = 0;
    }
}

void PWM_GetDefaultConfig(pwm_config_t *config)
{
    config->inputsrc  = 0;
    config->mode      = uPWM_EACH_CLK_CYCLE;
    config->clksel    = uPWM_REF_32K;
    config->updownsel = uPWM_SAW;
    config->prescale  = 0;
    config->evtSel    = uPWM_EVENT_SEL0;
}

void PWM_SetThreshold(PWM_Type *base, uint32_t threshold)
{
    base->TH = PWM_THRESHOLD_HIGH(threshold >> 16) | PWM_THRESHOLD_LOW(threshold & 0xFFFF);
}

void PWM_ChannelConfig(PWM_Type *base, pwm_channel_t channel, uint16_t threshold, pwm_channel_mode_t mode)
{
    base->CH_TH[channel] = PWM_CHANNEL_CONFIG_THRESHOLD(threshold) | PWM_CHANNEL_CONFIG_MODE(mode);
}

/*
 * Freq = Clk_src_freq / ( TH[31-16] - TH[15-0] )
 * Rcyc = ( ( CH_TH - TH[15-0] ) / ( TH[31-16] - CH_TH ) ) ? 1 : 0
 */
status_t PWM_SetupPwm(PWM_Type *base, const pwm_signal_param_t *ch_params,
                      uint8_t ch_num, uint32_t pwmFreq_Hz, pwm_clk_src_t clkSrc)
{
    /* Maximum period is 2 seconds. */
    uint16_t threshold = 0;

    if (clkSrc == uPWM_REF_32K)
    {
        threshold = (32768 / pwmFreq_Hz);
    }
    else
    {
        threshold = (SystemCoreClock / pwmFreq_Hz);
    }

    /* Low 16 bits default set 0, to have maximum 0xFFFF counter. */
    base->TH = PWM_THRESHOLD_HIGH(threshold) | PWM_THRESHOLD_LOW(1);

    for (uint8_t i = 0; i < ch_num; i++)
    {
        uint16_t ch_th = (uint16_t)(((float)threshold * (float)ch_params[i].dutyCyclePercent) / (float)100);
        PWM_ChannelConfig(base, ch_params[i].chnum, ch_th, uPWM_TOGGLE_CLEAR);
    }

    /* Configure clock source select */
    int cfg = base->CFG;

    cfg &= ~PWM_CONFIG_CLKSEL_MASK;
    cfg |= PWM_CONFIG_CLKSEL(clkSrc);

    base->CFG = cfg;

    return uStatus_Success;
}

void PWM_SetOutputEvent(PWM_Type *base, pwm_channel_t channel, pwm_event_sel_t evtSel)
{
    uint32_t instance = PWM_GetInstance(base);

    PWM_CTRL->EVENT_CFG |= PWM_CTRL_EVENT_TIMER_CHAN_SET(instance, channel, evtSel);

    SOC_EU_SetFCMask(PWM0_EVENT + instance);

    PWM_CTRL->EVENT_CFG |= PWM_CTRL_EVENT_TIMER_ENA(0x1 << evtSel);
}

void PWM_ClearOutputEvent(PWM_Type *base, pwm_channel_t channel, pwm_event_sel_t evtSel)
{
    uint32_t instance = PWM_GetInstance(base);

    PWM_CTRL->EVENT_CFG &= ~PWM_CTRL_EVENT_TIMER_CHAN_SET(instance, channel, evtSel);

    SOC_EU_ClearFCMask(PWM0_EVENT + instance);

    PWM_CTRL->EVENT_CFG &= ~PWM_CTRL_EVENT_TIMER_ENA(0x1 << evtSel);
}

void PWM_StartTimer(PWM_Type *base)
{
    PWM_Enable(base);

    base->CMD = PWM_CMD(uPWM_CMD_START);
}

void PWM_StopTimer(PWM_Type *base)
{
    base->CMD = PWM_CMD(uPWM_CMD_STOP);

    PWM_Disable(base);
}

void PWM_CreateHandler(PWM_Type *base, pwm_callback_t callback, void *userData)
{
    s_pwmIsr[PWM_GetInstance(base)].callback = callback;
    s_pwmIsr[PWM_GetInstance(base)].userData = userData;
}

void PWM_IRQHandler(uint32_t evtSel)
{
    uint32_t instance = evtSel - PWM0_EVENT;

    if (s_pwmIsr[instance].callback != NULL)
    {
        s_pwmIsr[instance].callback(s_pwmIsr[instance].userData);
    }
}
