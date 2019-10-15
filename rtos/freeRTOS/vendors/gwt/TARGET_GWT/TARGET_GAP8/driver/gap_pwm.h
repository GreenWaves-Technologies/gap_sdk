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

#ifndef _GAP_PWM_H_
#define _GAP_PWM_H_

#include <assert.h>
#include "gap_util.h"
#include "gap_soc_eu.h"

/*!
 * @addtogroup PWM
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

/*! @brief PWM module timer commands. */
typedef enum
{
    uPWM_CMD_START  = 0x01, /*!< PWM module timer START command. */
    uPWM_CMD_STOP   = 0x02, /*!< PWM module timer STOP command. */
    uPWM_CMD_UPDATE = 0x04, /*!< PWM module timer UPDATE command. */
    uPWM_CMD_RESET  = 0x08, /*!< PWM module timer RESET command. */
    uPWM_CMD_ARM    = 0x10  /*!< PWM module timer ARM command. */
} pwm_cmd_t;

/*! @brief Counter reset method when threshold reached. */
typedef enum
{
    uPWM_MOUNTAIN = 0x00,       /*!< Mountain shape : Count up then down alternatively. /\/\/\ */
    uPWM_SAW      = 0x01        /*!< Saw shape : Count up then reset to 0. /|/|/|. */
} pwm_updown_mode_t;

/*! @brief PWM module's clock source. */
typedef enum
{
    uPWM_FLL     = 0x00, /*!< FLL : configurable frequency. */
    uPWM_REF_32K = 0x01  /*!< 32kHz clock. */
} pwm_clk_src_t;

/*! @brief PWM module trigger mode. */
typedef enum
{
    uPWM_EACH_CLK_CYCLE     = 0x00, /*!< PWM module timer event trigger : each clock cycle. */
    uPWM_IN_SOURCE_0        = 0x01, /*!< PWM module timer event trigger : when input source = '0'. */
    uPWM_IN_SOURCE_1        = 0x02, /*!< PWM module timer event trigger : when input source = '1'. */
    uPWM_IN_RISING_EDGE     = 0x03, /*!< PWM module timer event trigger : on input source rising edge. */
    uPWM_IN_FALLING_EDGE    = 0x04, /*!< PWM module timer event trigger : on input source falling edge. */
    uPWM_IN_RISE_FALL_EDGE  = 0x05, /*!< PWM module timer event trigger : on input source rising or falling edge. */
    uPWM_IN_RISE_EDGE_ARMED = 0x06, /*!< PWM module timer event trigger : on input source rising edge when armed. */
    uPWM_IN_FALL_EDGE_ARMED = 0x07  /*!< PWM module timer event trigger : on input source falling edge when armed. */
} pwm_trigger_mode_t;

/*! @brief Action when a PWM module's channel's threshold match. */
typedef enum
{
    uPWM_SET          = 0x00, /*!< Set.
                                IRQ when threshold is matched and only once. */
    uPWM_TOGGLE_CLEAR = 0x01, /*!< Toggle, next threshold action match is Clear.
                                IRQ when threshold is matched, and counter is reset. */
    uPWM_SET_CLEAR    = 0x02, /*!< Set, next threshold action match is Clear.
                                IRQ when threshold is matched, and counter is reset. */
    uPWM_TOGGLE       = 0x03, /*!< Toggle.
                                IRQ when threshold is matched, and counter continues. */
    uPWM_CLEAR        = 0x04, /*!< Clear.
                                No IRQ. */
    uPWM_TOGGLE_SET   = 0x05, /*!< Toggle, next threshold action match is Set.
                                IRQ when threshold is matched, next times IRQ when counter resets. */
    uPWM_CLEAR_SET    = 0x06  /*!< Clear, next threshold action match is Set.
                                IRQ when counter is reset. */
} pwm_channel_mode_t;

/*! @brief PWM module's channels. */
typedef enum
{
    uPWM_TH_CHANNEL0 = 0x00, /*!< PWM module channel 0. */
    uPWM_TH_CHANNEL1 = 0x01, /*!< PWM module channel 1. */
    uPWM_TH_CHANNEL2 = 0x02, /*!< PWM module channel 2. */
    uPWM_TH_CHANNEL3 = 0x03  /*!< PWM module channel 3. */
} pwm_channel_t;

/*! @brief Event output selection. */
typedef enum
{
    uPWM_EVENT_SEL0 = 0x00, /*!< PWM output event 0. */
    uPWM_EVENT_SEL1 = 0x01, /*!< PWM output event 1. */
    uPWM_EVENT_SEL2 = 0x02, /*!< PWM output event 2. */
    uPWM_EVENT_SEL3 = 0x03  /*!< PWM output event 3. */
} pwm_event_sel_t;

/*! @brief Options to configure a Timer channel's PWM signal. */
typedef struct _pwm_signal_param
{
    pwm_channel_t chnum;               /*!< The channel number.*/
    uint8_t       dutyCyclePercent;    /*!< PWM pulse width, value should be between 0 to 100
                                            0 = inactive signal(0% duty cycle)...
                                            100 = always active signal (100% duty cycle).*/
} pwm_signal_param_t;

/*!
 * @brief PWM signal configuration structure.
 * PWM timer configuration
 * Arguments:
 *      - inputsrc: Select counting source
 *                      0-31 gpio
 *                      32-35 out of timer 0
 *                      ...
 *                      44-47 out of timer 3
 *      - mode: count rules
 *                      uPWM_EACH_CLK_CYCLE         0x0 //event at each clock cycle
 *                      uPWM_IN_SOURCE_0            0x1 //event if input signal is 0
 *                      uPWM_IN_SOURCE_1            0x2 //event if input signal is 1
 *                      uPWM_IN_RISING_EDGE         0x3 //event on input signal rising edge
 *                      uPWM_IN_FALLING_EDGE        0x4 //event on input signal falling edge
 *                      uPWM_IN_RISE_FALL_EDGE      0x5 //event on input signal rising or falling edge
 *                      uPWM_IN_RISE_EDGE_ARMED     0x6 //event on input signal rising edge when armed
 *                      uPWM_IN_FALL_EDGE_ARMED     0x7 //event on input signal falling edge when armed
 *      - clksel:
 *                      uPWM_FLL                    0x00
 *                      uPWM_REF_32K                0x01
 *      - updownsel: action when reaching the TH:
 *                      uPWM_MOUNTAIN :  /\/\/\ (NOSAW)
 *                      uPWM_SAW      :  /|/|/| (SAW)
 *      - prescale
 */
typedef struct _pwm_config_t
{
    uint8_t            inputsrc;  /*!< Input source for the counter. */
    pwm_trigger_mode_t mode;      /*!< Trigger mode. */
    pwm_clk_src_t      clksel;    /*!< Clock source. */
    pwm_updown_mode_t  updownsel; /*!< Counter reset method. */
    uint8_t            prescale;  /*!< Prescaler value. */
    pwm_event_sel_t    evtSel;    /*!< Event output selection. */
} pwm_config_t;


/*!
 * @brief Completion callback function pointer type.
 *
 * When the counter of a PWM module reaches the threshold, and if the event
 * output is enabled for its channel, this callback function is called.
 *
 * @param userData       Parameter passed to the callback function by the user.
 */
typedef void (*pwm_callback_t)(void *userData);

/*!
 * @brief PWM handler structure.
 *
 * This structure holds information to handle events when a PWM module's channel
 * has reached its threshold.
 */
typedef struct _pwm_isr
{
    pwm_callback_t  callback; /*!< A callback function called when the threshold is reached. */
    void           *userData; /*!< A callback parameter passed to the callback function. */
} pwm_isr_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name PWM module configuration.
 * @{
 */

/*!
 * @brief Get instance number of an PWM.
 *
 * @param base           PWM base pointer.
 *
 * @return Instance number of the PWM module.
 */
uint32_t PWM_GetInstance(PWM_Type *base);

/*!
 * @brief Initialize a PWM module.
 *
 * This function initializes a PWM module.
 *
 * @param base           PWM base pointer.
 * @param config         Pointer to pwm_config_t structure.
 */
void PWM_Init(PWM_Type *base, pwm_config_t *config);

/*!
 * @brief Release a PWM module.
 *
 * This function stops a PWM module's timer and resets its counters' and thresholds' values.
 *
 * @param base           PWM base pointer.
 */
void PWM_Deinit(PWM_Type *base);

/*!
 * @brief Set the PWM configuration structure to default values.
 *
 * @param config         Pointer to pwm_config_t structure.
 */
void PWM_GetDefaultConfig(pwm_config_t *config);

/*!
 * @brief Configure timer threshold value of a PWM module.
 *
 * This function configures the timer of a PWM module.
 * - threshold[31 - 16] defines the end counter value.
 * - threshold[15 - 00] defines the start counter value.
 *
 * @param base           PWM base pointer.
 * @param threshold      Threshold value to set.
 */
void PWM_SetThreshold(PWM_Type *base, uint32_t threshold);

/*!
 * @brief Configure one channel of a PWM module.
 *
 * This function sets the threshold value of a PWM module's channel
 * and configures the next action when threshold is matched.
 *
 * @param base           PWM base pointer.
 * @param channel        Number of the channel to configure.
 * @param threshold      Channel's threshold value.
 * @param mode           Channel's mode(pwm_channel_mode_t).
 */
void PWM_ChannelConfig(PWM_Type *base, pwm_channel_t channel, uint16_t threshold, pwm_channel_mode_t mode);

/*!
 * @brief Set PWM's output event.
 *
 * @param base    PWM instance.
 * @param channel The number of channel.
 * @param evtSel  The number of event sel.
 */
void PWM_SetOutputEvent(PWM_Type *base, pwm_channel_t channel, pwm_event_sel_t evtSel);

/*!
 * @brief Clear PWM's output event.
 *
 * @param base    PWM instance.
 * @param channel The number of channel.
 * @param evtSel  The number of event sel.
 */
void PWM_ClearOutputEvent(PWM_Type *base, pwm_channel_t channel, pwm_event_sel_t evtSel);

/*!
 * @brief Configure the PWM output signal.
 *
 * @param base           PWM instance.
 * @param ch_params      Array of PWM channel parameters to configure the channel(s)
 * @param ch_num         Number of channels to configure; This should be the size of the array passed in
 * @param pwmFreq_Hz     PWM signal frequency in Hz
 * @param clkSrc         Clock source
 *
 * @retval uStatus_Success if the PWM setup was successful.
 * @retval uStatus_Error on failure.
 */
status_t PWM_SetupPwm(PWM_Type *base, const pwm_signal_param_t *ch_params,
                      uint8_t ch_num, uint32_t pwmFreq_Hz, pwm_clk_src_t clkSrc);

/* @} */

/*!
 * @name PWM controle.
 * @{
 */

/*!
 * @brief Start PWM a module.
 *
 * This function enables a PWM module and sends a Start command to the timer.
 *
 * @param base           PWM base pointer.
 */
void PWM_StartTimer(PWM_Type *base);

/*!
 * @brief Stop a PWM module.
 *
 * This function sends a Stop command to the timer and disables a PWM module.
 *
 * @param base           PWM base pointer.
 */
void PWM_StopTimer(PWM_Type *base);

/* @} */

/*!
 * @name IRQ Handler.
 * @{
 */

/*!
 * @brief Initialize the PWM IRQ handler.
 *
 * This function creates a IRQ handler to handle events from a PWM module
 * each time the threshold of a channel is reached and its corresponding output
 * event is enabled.
 *
 * @param base           PWM base pointer.
 * @param callback       Callback function.
 * @param userData       Parameter passed to the callback function.
 */
void PWM_CreateHandler(PWM_Type *base, pwm_callback_t callback, void *userData);

/*!
 * @brief PWM IRQ handler.
 *
 * This function is called when an event occurs from a PWM module.
 *
 * @param evtSel         Event number of the PWM module.
 */
void PWM_IRQHandler(uint32_t evtSel);

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_PWM_H_*/
