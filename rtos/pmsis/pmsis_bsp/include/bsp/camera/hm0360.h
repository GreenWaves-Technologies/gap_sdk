/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __BSP_CAMERA_HM0360_H__
#define __BSP_CAMERA_HM0360_H__

#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup HM0360 HM0360
 */

/**
 * @addtogroup HM0360
 * @{
 */

/** \struct pi_hm0360_conf
 * \brief HM0360 configuration structure.
 *
 * This structure is used to pass the desired HM0360 configuration to the
 * runtime when opening the device.
 */
struct pi_hm0360_conf
{
  struct pi_camera_conf camera; /*!< Generic camera configuration. */
  int cpi_itf;                  /*!< CPI interface where the camera is
    connected. */
  int i2c_itf;                  /*!< I2C interface where the camera control
    interface is connected. */
  char skip_pads_config;        /*!< Skip pads configuration if set to 1. */
  int gpio_xshdown;
  int gpio_xsleep;
  int gpio_cxt_sel;
  int gpio_fae;
  pi_camera_format_e format;    /*!< Camera image resolution. */
};

/** \brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the camera configuration.
 */
void pi_hm0360_conf_init(struct pi_hm0360_conf *conf);

//!@}

/**
 * @} end of HM0360
 */

/**
 * @} end of Camera
 */




#endif
