/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __BSP_CAMERA_THERMEYE_H__
#define __BSP_CAMERA_THERMEYE_H__

#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup Thermeye Thermeye
 */

/**
 * @addtogroup Thermeye
 * @{
 */

/**
 * \struct pi_thermeye_conf
 *
 * \brief Thermeye configuration structure.
 *
 * This structure is used to pass the desired thermeye configuration to the
 * runtime when opening the device.
 */
struct pi_thermeye_conf
{
    struct pi_camera_conf camera; /*!< Generic camera configuration. */
    uint8_t cpi_id;  /*!< CPI ID used to transfer data from camera. */
    uint8_t i2c_id;  /*!< I2C ID used to control camera. */
    uint8_t pwm_id;  /*!< PWM ID used to generate Master Clock. */
    uint8_t pwm_channel; /*<! PWM channel used to generate Master Clock. */
    pi_gpio_e gpio_power;  /*!< GPIO pin used to power camera. */
    pi_gpio_e gpio_reset;  /*!< GPIO pin used to reset camera. */
    uint8_t skip_pads_config; /*!< Skip pads configuration if set to 1. */
    pi_cpi_format_e cpi_format;
};

/**
 * \brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the camera configuration.
 */
void pi_thermeye_conf_init(struct pi_thermeye_conf *conf);

/**
 * @} end of Thermeye
 */

/**
 * @} end of Camera
 */

#endif  /* __BSP_CAMERA_THERMEYE_H__ */
