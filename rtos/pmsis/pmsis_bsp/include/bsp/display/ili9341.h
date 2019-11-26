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

#include "pmsis.h"

#ifndef __BSP__DISPLAY__ILI9341_H__
#define __BSP__DISPLAY__ILI9341_H__

#include "bsp/display.h"

/**
 * @addtogroup Display
 * @{
 */

/**
 * @defgroup Ili9341 Ili9341
 *
 * Display Ili9341.
 * This module is interfaced through SPIM1.
 */

/**
 * @addtogroup Ili9341
 * @{
 */

/* @brief Struct holding ili9341 display config. */
struct pi_ili9341_conf
{
  struct pi_display_conf display; /*!< Display conf. */
  int spi_itf;                    /*!< Interface used to communicate with SPI.  */
  int spi_cs;                     /*!< Chip select on the interface.  */
  int gpio;                       /*!< GPIO pin.  */
  char skip_pads_config;          /*!< Buffer used to send  */
};

/* @brief Display orientation. */
typedef enum
{
  PI_ILI_ORIENTATION_0 = 0,     /*!< Orientation at 0 degrees. */
  PI_ILI_ORIENTATION_90 = 1,    /*!< Orientation at 90 degrees. */
  PI_ILI_ORIENTATION_180 = 2,   /*!< Orientation at 180 degrees. */
  PI_ILI_ORIENTATION_270 = 3,   /*!< Orientation at 270 degrees. */
} pi_ili_orientation_e;

/* @brief Ili9341 ioctl commands. */
typedef enum
{
  PI_ILI_IOCTL_ORIENTATION = PI_DISPLAY_IOCTL_CUSTOM /*!< Display orientation
    command. The argument to this command must be a value of type
    pi_ili_orientation_e. */
} pi_ili_ioctl_cmd_e;

/**
 * @brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the camera device is opened.
 * It can only be called from fabric-controller side.
 *
 * @param conf           Pointer to the device configuration.
 */
void pi_ili9341_conf_init(struct pi_ili9341_conf *conf);

/**
 * @}
 */

/**
 * @}
 */

#endif  /* __BSP__DISPLAY__ILI9341_H__ */
