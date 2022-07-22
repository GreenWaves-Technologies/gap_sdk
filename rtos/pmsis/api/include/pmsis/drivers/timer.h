/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupDrivers
 *
 * \defgroup Timer Timer
 *
 * \brief Timer
 *
 * The timer driver includes API to manage different timers available on
 * chips, for both Fabric Controller side and Cluster side.
 */

/**
 * \addtogroup Timer
 * \{
 */


/**
 * \struct pi_timer_conf_s
 *
 * \brief Timer configuration structure.
 *
 * This structure is used to pass the desired timer configuration to the
 * runtime when opening the device.
 */
struct pi_timer_conf_s
{
    uint32_t time_us; /*!< Timer value to compare. */
    uint8_t timer_id; /*!< Timer ID, refer to \ref pi_timer_e. */
    uint8_t one_shot; /*!< One shot timer, after reaching time_us, timer is disabled. */
    uint8_t irq_en;   /*!< Enable timer IRQ. */
    uint8_t clk_src;  /*!< Timer clock source: FLL=0, REF_CLK=1. */
    uint8_t timer_reset;        /*!< When value is reached: CONTINUE=0, RESET=1. */
    //uint8_t timer_64; /*!< Enable a 64-bit timer, using two 32-bit timers. */
};


/**
 * \brief Initialize a timer configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the I2S
 * device is opened.
 *
 * \param conf           Pointer to the timer configuration.
 *
 * \note Only FC_TIMER_2 and FC_TIMER_3 timers are available.
 */
void pi_timer_conf_init(struct pi_timer_conf_s *conf);

/**
 * \brief Open a timer.
 *
 * This function will do all the needed configuration to initialize a timer(on
 * FC or Cluster) with given configuration.
 *
 * \param device         Pointer to device structure.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note This function must be called before the timer device can be used.
 * \note For preemptive RTOS using time slicing, the FC_Timer_0 should be used as
 *       SysTick timer. Thus a 64 bit timer can not be used on FC side.
 */
int pi_timer_open(struct pi_device *device);

/**
 * \brief Close an opened timer device.
 *
 * This function closes a timer device.
 *
 * \param device         Pointer to device structure.
 */
void pi_timer_close(struct pi_device *device);

/**
 * \brief Start a timer.
 *
 * This function starts a timer.
 *
 * \param device         Pointer to device structure.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_timer_start(struct pi_device *device);

/**
 * \brief Stop a timer.
 *
 * This function stops a timer.
 *
 * \param device         Pointer to device structure.
 */
void pi_timer_stop(struct pi_device *device);

/**
 * \brief Reset a timer counter.
 *
 * This function resets a timer's counter register.
 *
 * \param device         Pointer to device structure.
 *
 * \note This function does not reset a timer's configuration.
 *       To fully reset a timer, stop first the timer, then reinitialize
 *       the timer using pi_timer_init() function.
 */
void pi_timer_reset(struct pi_device *device);

/**
 * \brief Get a timer's counter value.
 *
 * This function reads the current counter value of a timer and stores it in the
 * given buffer.
 *
 * \param device         Pointer to device structure.
 * \param value          Buffer to store counter value.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_timer_current_value_read(struct pi_device *device, uint32_t *value);

int pi_timer_task_add(struct pi_device *device, uint32_t time_us, pi_task_t *task);

int pi_timer_task_remove(struct pi_device *device, pi_task_t *task);

/**
 * \} end of Timer
 */

#ifdef __cplusplus
}
#endif
