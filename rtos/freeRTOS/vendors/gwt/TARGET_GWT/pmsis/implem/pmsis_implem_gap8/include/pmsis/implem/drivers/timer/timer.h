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

#ifndef __PI_TIMER_H__
#define __PI_TIMER_H__

#include <stdint.h>
#include "pmsis/targets/target.h"

/**
 * @defgroup Timer Timer
 */

/**
 * @addtogroup Timer
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define fc_timer(id)  ((timer_periph_t *) SysTick)         /*!< Pointer to FC TImer. */
#define cl_timer(id)  ((timer_periph_t *) CLUSTER_SysTick) /*!< Pointer to Cluster Timer. */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* @brief Timers. */
typedef enum
{
    SYS_TIMER  = 0,             /*!< FC_TIMER_0 used as SysTick timer by preemptive RTOS. */
    FC_TIMER_0 = 0,             /*!< FC Timer_Low. */
    FC_TIMER_1 = 1,             /*!< FC Timer_High. */
    CL_TIMER_0 = 2,             /*!< Cluster Timer_Low. */
    CL_TIMER_1 = 3              /*!< Cluster Timer_High. */
} timer_e;

/* @brief Timer config structure.  */
typedef union
{
    struct
    {
        uint32_t enable:1;      /*!< Enable/Start Timer. */
        uint32_t reset:1;       /*!< Reset Timer counter. */
        uint32_t irq_en:1;      /*!< Enable IRQ when counter matches compare value. */
        uint32_t pad_0:1;       /*!< Padding. */
        uint32_t mode:1;        /*!< Timer counting mode : increment or reset when counter = compare. */
        uint32_t one_shot:1;    /*!< Timer continous mode : enable or disable when counter = compare. */
        uint32_t presc_en:1;    /*!< Enable prescaler. */
        uint32_t clk_source:1;  /*!< Timer's clock source : FLL or Ref32kHz. */
        uint32_t presc_val:8;   /*!< Prescaler value(only Timer_0/Timer_LO : Ftim = Fclk / (1 + val). */
        uint32_t pad_1:15;      /*!< Padding. */
        uint32_t timer64_en:1;  /*!< Enable 64 bit mode timer, using both Timer_0(LO) & TImer_1(HI). */
    } field;                    /*!< Timer config bit fields.  */
    uint32_t word;              /*!< Timer config in 32bits format. */
} timer_cfg_u;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * @brief Initialize a timer.
 *
 * This function initializes a timer(on FC or Cluster) with given configuration.
 *
 * @param timer          Timer to configure.
 * @param cfg            Configuration structure to set up the timer.
 * @param cmp_val        Compare value.
 *
 * @note For preemptive RTOS using time slicing, the FC_Timer_0 should be used as
 * SysTick timer. Thus a 64 bit timer can not be used.
 */
void pi_timer_init(timer_e timer, timer_cfg_u cfg, uint32_t cmp_val);

/**
 * @brief Reset a timer counter.
 *
 * This function resets a timer's counter register.
 *
 * @param timer          Timer to reset.
 *
 * @note This function does not reset a timer's configration.
 * To fully reset a timer, stop first the timer, then reinitialize the timer using
 * pi_timer_init() function.
 */
void pi_timer_reset(timer_e timer);

/**
 * @brief Start a timer.
 *
 * This function starts a timer.
 *
 * @param timer          Timer to start.
 */
void pi_timer_start(timer_e timer);

/**
 * @brief Stop a timer.
 *
 * This function stops a timer.
 *
 * @param timer          Timer to stop.
 */
void pi_timer_stop(timer_e timer);

/**
 * @brief Get a timer's counter value.
 *
 * This function returns current counter value of a timer.
 *
 * @param timer          Timer to stop.
 *
 * @return Current counter value of the timer.
 */
uint32_t pi_timer_value_read(timer_e timer);

/**
 * \brief Set timer to trigger IRQ.
 *
 * This function is used to set a timer to trigger an IRQ when the given value is reached.
 *
 * \param timer          Timer to set.
 * \param time_us        Timer value in us.
 * \param one_shot       Timer mode : one-shot or continuous.
 *
 * \note one_shot = 1, one shot timer.
 *       one_shot = 0, continuous mode.
 * \note This function disables a timer if one is already enabled, and reconfigure it.
 */
void pi_timer_irq_set(timer_e timer, uint32_t time_us, uint8_t one_shot);

/**
 * @}
 */

#endif  /* __PI_TIMER_H__ */
