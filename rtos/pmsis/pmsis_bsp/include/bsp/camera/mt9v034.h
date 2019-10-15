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

#ifndef __BSP_CAMERA_MT9V034_H__
#define __BSP_CAMERA_MT9V034_H__

#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup Mt9v Mt9v
 */

/**
 * @addtogroup Mt9v
 * @{
 */

/** \struct pi_mt9v034_conf
 * \brief Mt9v034 configuration structure.
 *
 * This structure is used to pass the desired mt9v034 configuration to the
 * runtime when opening the device.
 */
struct pi_mt9v034_conf
{
  struct pi_camera_conf camera; /*!< Generic camera configuration. */
  char cpi_itf;/*!< CPI interface where the camera is
    connected. */
  char i2c_itf; /*!< I2C interface where the camera control
    interface is connected. */
  char power_gpio; /*!< GPIO number where the power pad of the camera is
    connected. */
  char trigger_gpio; /*!< GPIO number where the trigger pad of the camera is
    connected. */
  char column_flip; /*!< Flip columns if set to 1. */
  char row_flip; /*!< Flip rows if set to 1. */
  char skip_pads_config;/*!< Skip pads configuration if set to 1. */
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
void pi_mt9v034_conf_init(struct pi_mt9v034_conf *conf);

//!@}

/**
 * @} end of Mt9v034
 */

/**
 * @} end of Camera
 */




#endif
