/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "mbed_assert.h"
#include "pwmout_api.h"

#if DEVICE_PWMOUT

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"

/* Array of PWM peripheral base address. */
static PWM_Type *const pwm_addrs[] = PWM_BASE_PTRS;

void pwmout_init(pwmout_t* obj, PinName pin)
{
    PWMName pwm = (PWMName)pinmap_peripheral(pin, PinMap_PWM);
    MBED_ASSERT(pwm != (PWMName)NC);

    obj->pwm_name = pwm;

    uint32_t channel = pwm & 0x3;
    uint32_t instance = pwm >> PWM_SHIFT;
    pwm_config_t pwmInfo;

    PWM_GetDefaultConfig(&pwmInfo);
    /* Start from 1 while register starts from 0 !*/
    obj->prescale    = 1;
    pwmInfo.prescale = 0;

    /* Initialize PWM module */
    PWM_Init(pwm_addrs[instance], &pwmInfo);

    pwm_signal_param_t config = {
        .chnum = (pwm_channel_t)channel,
        .dutyCyclePercent = 50
    };

    obj->clock_src = pwmInfo.clksel;

    // default to 20ms: standard for servos, and fine for e.g. brightness control
    PWM_SetupPwm(pwm_addrs[instance], &config, 1, 50, obj->clock_src);

    PWM_StartTimer(pwm_addrs[instance]);

    // Wire pinout
    pinmap_pinout(pin, PinMap_PWM);
}

void pwmout_free(pwmout_t* obj)
{
    PWM_Deinit(pwm_addrs[obj->pwm_name >> PWM_SHIFT]);
}

void pwmout_write(pwmout_t* obj, float value)
{
    if (value < 0.0f) {
        value = 0.0f;
    } else if (value > 1.0f) {
        value = 1.0f;
    }

    PWM_Type *base = pwm_addrs[obj->pwm_name >> PWM_SHIFT];

    uint32_t TH        = base->TH;
    /* uint16_t th_low    = TH & PWM_THRESHOLD_LOW_MASK; */
    uint16_t th_high   = (TH & PWM_THRESHOLD_HIGH_MASK) >> PWM_THRESHOLD_HIGH_SHIFT;

    // float ch_th = (float)(value * (float) th_high + (float)th_low) / (float)(1.0f + value);
    uint16_t ch_th = (uint16_t)(value * (float) th_high);

    uint32_t channel = obj->pwm_name & 0x3;

    uint32_t CH_TH = base->CH_TH[channel];
    CH_TH &= ~PWM_CHANNEL_CONFIG_THRESHOLD_MASK;
    CH_TH |= PWM_CHANNEL_CONFIG_THRESHOLD(ch_th);

    /* Stop then start to update counter value*/
    PWM_StopTimer(base);

    base->CH_TH[channel] = CH_TH;

    PWM_StartTimer(base);
}

float pwmout_read(pwmout_t* obj)
{
    PWM_Type *base = pwm_addrs[obj->pwm_name >> PWM_SHIFT];

    uint32_t channel   = (obj->pwm_name & 0x3);
    uint32_t TH        = base->TH;
    /* uint16_t th_low    = TH & PWM_THRESHOLD_LOW_MASK; */
    uint16_t th_high   = (TH & PWM_THRESHOLD_HIGH_MASK) >> PWM_THRESHOLD_HIGH_SHIFT;
    uint16_t ch_th     = base->CH_TH[channel] & PWM_CHANNEL_CONFIG_THRESHOLD_MASK;

    // float v = (float)(ch_th - th_low) / (float)(th_high - ch_th);
    float v = (float)ch_th / (float)th_high;

    return (v > 1.0f) ? (1.0f) : (v);
}

void pwmout_period(pwmout_t* obj, float seconds)
{
    pwmout_period_us(obj, seconds * 1000000.0f);
}

void pwmout_period_ms(pwmout_t* obj, int ms)
{
    pwmout_period_us(obj, ms * 1000);
}

// Set the PWM period, keeping the duty cycle the same.
void pwmout_period_us(pwmout_t* obj, int us)
{
    uint32_t instance = obj->pwm_name >> PWM_SHIFT;

    PWM_Type *base = pwm_addrs[instance];
    float dc = 0.0f;

    /* Maximum period is 2 second - FIX by timer chain */
    if(us == 0 ) {
        base->TH = 0;
    } else if (us > 512000000) {
        /* min_hz = 1 / 512, max_hz = (SystemCoreClock / 2) */
        return;
    } else {
        /* Get percent */
        dc = pwmout_read(obj);

        /* If too small, use SystemCoreClock */
        if (us < 1000) {
            obj->clock_src = uPWM_FLL;

            int cfg = base->CFG;

            cfg &= ~PWM_CONFIG_CLKSEL_MASK;
            cfg |= PWM_CONFIG_CLKSEL(obj->clock_src);
            base->CFG = cfg;
        } else if (us >= 2000000) {
            int prescale;

            /* Config prescale if it is too large */
            prescale = (us / 2000000) + 1;
            int cfg = base->CFG;
            cfg &= ~PWM_CONFIG_PRESCALE_MASK;
            /* Start from 1 while register starts from 0 !*/
            cfg |= PWM_CONFIG_PRESCALE(prescale - 1);
            base->CFG = cfg;

            obj->prescale = prescale;
        }

        /* Low 16 bits default set 0, to have maximum 0xFFFF counter */
        if (obj->clock_src == uPWM_REF_32K) {
            /* Can not create resolution < 30 us */
            base->TH = PWM_THRESHOLD_HIGH(((int)((float)us * 0.032768f) / obj->prescale) + 1) | PWM_THRESHOLD_LOW(1);
        } else {
            base->TH = PWM_THRESHOLD_HIGH((int)((float)us * (float)SystemCoreClock / 1000000.0f) + 1) | PWM_THRESHOLD_LOW(1);
        }
    }

    pwmout_write(obj, dc);
}

void pwmout_pulsewidth(pwmout_t* obj, float seconds)
{
    pwmout_pulsewidth_us(obj, seconds * 1000000.0f);
}

void pwmout_pulsewidth_ms(pwmout_t* obj, int ms)
{
    pwmout_pulsewidth_us(obj, ms * 1000);
}

void pwmout_pulsewidth_us(pwmout_t* obj, int us)
{
    uint32_t instance = obj->pwm_name >> PWM_SHIFT;

    PWM_Type *base = pwm_addrs[instance];

    uint32_t channel = obj->pwm_name & 0x3;

    uint16_t ch_th = 0;

    /* Low 16 bits default set 0, to have maximum 0xFFFF counter */
    if (obj->clock_src == uPWM_REF_32K) {
        /* Can not create resolution < 30 us */
        ch_th = (int)((float)us * 0.032768f) / obj->prescale;
    } else {
        /* Can create resolution us */
        ch_th = (int)((float)us * (float)SystemCoreClock / 1000000.0f);
    }

    uint32_t CH_TH = base->CH_TH[channel];
    CH_TH &= ~PWM_CHANNEL_CONFIG_THRESHOLD_MASK;
    CH_TH |= PWM_CHANNEL_CONFIG_THRESHOLD(ch_th);

    /* Stop then start to update counter value*/
    PWM_StopTimer(base);

    base->CH_TH[channel] = CH_TH;

    PWM_StartTimer(base);
}

#endif
