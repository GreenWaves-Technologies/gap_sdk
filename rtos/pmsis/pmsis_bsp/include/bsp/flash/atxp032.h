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

#ifndef __BSP__FLASH__ATXP032_H__
#define __BSP__FLASH__ATXP032_H__

#include "bsp/flash.h"

/**
 * @addtogroup Flash
 * @{
 */

/**
 * @defgroup Atxp032 Atxp032
 *
 */

/**
 * @addtogroup Atxp032
 * @{
 */

/**@{*/

/** \struct pi_atxp032_conf
 * \brief Atxp032 configuration structure.
 *
 * This structure is used to pass the desired Atxp032 configuration to the
 * runtime when opening the device.
 */
struct pi_atxp032_conf
{
  struct pi_flash_conf flash;  /*!< Generic flash configuration. */
    int spi_itf;           /*!< SPI interface where the RAM is
      connected. */
    int spi_cs;            /*!< Chip select where the RAM is connected. */
    int xip_en;
    uint32_t baudrate;     /*!< Baudrate (in bytes/second). */
};

/** \brief Initialize an Atxp032 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * atxp032 device is opened.
 *
 * \param conf A pointer to the atxp032 configuration.
 */
void pi_atxp032_conf_init(struct pi_atxp032_conf *conf);

//!@}

/**
 * @} end of Atxp032
 */

/**
 * @} end of Flash
 */

#endif 
