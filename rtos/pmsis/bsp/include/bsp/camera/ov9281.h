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

#ifndef __BSP_CAMERA_OV9281_H__
#define __BSP_CAMERA_OV9281_H__

#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup Ov9281 Ov9281
 */

/**
 * @addtogroup Ov9281
 * @{
 */

/** \struct pi_ov9281_conf
 * \brief Ov9281 configuration structure.
 *
 * This structure is used to pass the desired Ov9281 configuration to the
 * runtime when opening the device.
 */
struct pi_ov9281_conf
{
  struct pi_camera_conf camera; /*!< Generic camera configuration. */
  int csi2_itf;                  /*!< CPI interface where the camera is
    connected. */
  int i2c_itf;                  /*!< I2C interface where the camera control
    interface is connected. */
  char vc;                           /*!< support virtual channel, default is 0, can up to 2 virtual channel . */
  pi_camera_slicing_conf_t roi;      /*!< ROI (region of interest) of an image. */
};

/** \brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the camera configuration.
 */
void pi_ov9281_conf_init(struct pi_ov9281_conf *conf);

//!@}

/**
 * @} end of Ov9281
 */

/**
 * @} end of Camera
 */




#endif
