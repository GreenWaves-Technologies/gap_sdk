/*
 * Copyright (C) 2022 GreenWaves Technologies
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

#include "pmsis.h"

#pragma once

/**
 * @addtogroup GPIO
 * @{
 */

/**
 * @defgroup FXL6408 FXL6408
 *
 * I2C Controlled GPIO Expander (8 configurable IOs)
 */

/**
 * @addtogroup FXL6408
 * @{
 */

/** @brief Struct holding FXL6408 display config. */
struct pi_fxl6408_conf
{
    int i2c_itf;    /*!< I2C interface number where the device is connected. */
    pi_gpio_e interrupt_pin; /*!< interrupt pin */
};

/**
 * Direction of a GPIO (input or output)
 */
typedef enum {
    FXL6408_GPIO_DIR_INPUT = 0x0,
    FXL6408_GPIO_DIR_OUTPUT = 0x1,
} fxl6408_gpio_dir_e;

/**
 * GPIO Output state
 *
 * This has no effect in input mode.
 */
typedef enum {
    /** GPIO is in High-Z (impedance) mode */
    FXL6408_GPIO_OUTPUT_STATE_DISABLED = 0x0,
    /** GPIO is in low voltage level, or 0 */
    FXL6408_GPIO_OUTPUT_STATE_LOW = 0x1,
    /** GPIO is in high voltage level, or 1 */
    FXL6408_GPIO_OUTPUT_STATE_HIGH = 0x2,
} fxl6408_gpio_output_state_e;

/**
 * GPIO Input trigger conditions (disabled, rising or falling edge)
 *
 * This has no effect in output mode.
 */
typedef enum {
    /** Input will trigger on a rising edge */
    FXL6408_GPIO_INPUT_TRIGGER_RISING = 0x0,
    /** Input will trigger on a falling edge */
    FXL6408_GPIO_INPUT_TRIGGER_FALLING = 0x1,
    /** Input will never trigger */
    FXL6408_GPIO_INPUT_TRIGGER_DISABLED = 0x2,
} fxl6408_gpio_input_trigger_e;

/** GPIO Pull up/down state
 *
 * This has no effect in output mode.
 */
typedef enum {
    /** GPIO is set to pull down */
    FXL6408_GPIO_PULL_STATE_DOWN = 0x0,
    /** GPIO is set to pull up */
    FXL6408_GPIO_PULL_STATE_UP = 0x1,
    /** GPIO pull is disabled */
    FXL6408_GPIO_PULL_STATE_DISABLED = 0x2,
} fxl6408_gpio_pull_state_e;

/** Structure holding the configuration of a FXL6408 GPIO */
typedef struct {
    /** GPIO id(from 0 to 7) */
    uint8_t id;
    /** Direction (input or output) */
    fxl6408_gpio_dir_e direction;
    /** Output State (disabled/High-Z, 0 or 1) */
    fxl6408_gpio_output_state_e output_state;
    /** Input trigger (trigger on falling edge, rising edge or disabled) */
    fxl6408_gpio_input_trigger_e input_trigger;
    /** pull state (pull-up, pull-down, disabled) */
    fxl6408_gpio_pull_state_e pull_state;
    /** task executed when an irq is detected */
    pi_task_t *irq_task;
} pi_fxl6408_gpio_conf_t;

/**
 * @brief Initialize an FXL6408 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the device is opened.
 * It can only be called from fabric-controller side.
 * It is not thread-safe and cannot be called from a pmsis task callback or
 * interrupt handler.
 *
 * @param[inout] conf           Pointer to the device configuration.
 */
void pi_fxl6408_conf_init(struct pi_fxl6408_conf *conf);

/**
 * @brief Open an FXL6408 device.
 *
 * It can only be called from fabric-controller side.
 * It is not thread-safe and cannot be called from a pmsis task callback or
 * interrupt handler.
 *
 * @param[inout] device           Pointer to the device.
 *
 * @return 0 if successfull or any other value otherwise
 */
int pi_fxl6408_open(pi_device_t *device);

/**
 * Close an FXL6408 device.
 *
 * @param[inout] device device to be closed
 */
void pi_fxl6408_close(pi_device_t *device);

/**
 * Initialize the configuration of a GPIO (Output, High-Z)
 *
 * @param[inout] gpio_conf configuration of the gpio
 */
void pi_fxl6408_gpio_conf_init(pi_fxl6408_gpio_conf_t *gpio_conf);

/**
 * @brief Set a GPIO state.
 *
 * It can only be called from fabric-controller side.
 * It is not thread-safe and cannot be called from a pmsis task callback or
 * interrupt handler.
 *
 * @param[in] device           Pointer to the device.
 * @param[in] gpio_conf        GPIO configuration
 * @return PI_OK if successfull or any other value otherwise
 */
int pi_fxl6408_gpio_set(pi_device_t *device, pi_fxl6408_gpio_conf_t *gpio_conf);

/**
 * Return the current status of inputs
 *
 * Each bit of the input status is the status of the corresponding
 * gpio input.
 *
 * @param[in] device pointer to the device
 * @param[out] input_status value of the input status register
 *
 * @return PI_OK if operation was successful, an error code otherwise.
 */
int pi_fxl6408_input_status_get(pi_device_t *device, uint8_t *input_status);

/**
 * Return the current status of interrupts
 *
 * This will clear the interrupt status register.
 *
 * Each bit of the interrupt status is the status of the corresponding
 * gpio interrupt.
 *
 * @param[in] device pointer to the device
 * @param[out] interrupt_status value of the interrupt status register
 *
 * @return PI_OK if operation was successful, an error code otherwise.
 */
int pi_fxl6408_interrupt_status_get(pi_device_t *device, uint8_t *interrupt_status);

/**
 * @}
 */

/**
 * @}
 */
