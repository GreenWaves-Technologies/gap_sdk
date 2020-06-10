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

#ifndef __BSP_CAMERA_PIXART_H__
#define __BSP_CAMERA_PIXART_H__

#include "pmsis.h"
#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup pixart
 */

/**
 * @addtogroup
 * @{
 */

/** \struct pi_pixart_conf
 * \brief pixart configuration structure.
 *
 * This structure is used to pass the desired pixart configuration to the
 * runtime when opening the device.
 */

typedef struct
{
  pi_gpio_e gpio_power_4V; /*!< GPIO number where the power pad of the camera is connected. */
  pi_gpio_e gpio_power_2V5; /*!< GPIO number where the power pad of the module is connected. */
  pi_gpio_e gpio_reset; /*!< GPIO number where reset the camera. */
} gpio_control_t;

struct pi_pixart_conf
{
  struct pi_camera_conf camera; /*!< Generic camera configuration. */
  uint8_t cpi_itf;/*!< CPI interface where the camera is
    connected. */
  uint8_t spi_itf; /* spi interface to control the camera */
  uint8_t spi_cs; /* spi as master select which pad to select slave */
  uint8_t pwm_id; /* output pwm as camera MCLK. */
  uint8_t pwm_channel; /* select pwm channeal. */
  gpio_control_t gpio_ctl;
  uint8_t skip_pads_config;/*!< Skip pads configuration if set to 1. */
  pi_camera_format_e format;/*!< Camera image resolution. */
};

/** \brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the camera configuration.
 */
void pi_pixart_conf_init(struct pi_pixart_conf *conf);

//!@}

/**
 * @} end of pixart
 */

/**
 * @} end of Camera
 */




#endif
