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

#ifndef __BSP_CAMERA_EPEAS_CIS001_H__
#define __BSP_CAMERA_EPEAS_CIS001_H__

#include "pmsis.h"
#include "bsp/camera.h"

/**
 * @addtogroup Camera
 * @{
 */

/**
 * @defgroup epeas_cis001 epeas_cis001
 */

/**
 * @addtogroup epeas_cis001
 * @{
 */

/** \struct pi_epeas_cis001_conf
 * \brief epeas_cis001 configuration structure.
 *
 * This structure is used to pass the desired epeas_cis001 configuration to the
 * runtime when opening the device.
 */

struct pi_epeas_cis001_conf
{
  struct pi_camera_conf camera; /*!< Generic camera configuration. */
  int cpi_itf;                  /*!< CPI interface where the camera is connected. */
  int spi_itf;                  /*!< SPI interface where the camera control */
  int spi_cs;                  
  int pwm_id;                   
  int pwm_channel;
  pi_gpio_e gpio_reset;         /*!< GPIO number for reset the epeas_cis001 camera */
  pi_gpio_e gpio_capture;                               
  char skip_pads_config;        /*!< Skip pads configuration if set to 1. */
  pi_camera_format_e format;    /*!< Camera image resolution. */
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
void pi_epeas_cis001_conf_init(struct pi_epeas_cis001_conf *conf);

//!@}

/**
 * @} end of epeas_cis001
 */

/**
 * @} end of Camera
 */




#endif
