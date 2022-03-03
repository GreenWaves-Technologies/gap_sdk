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

#ifndef __BSP__FLASH__MX25U51245G_H__
#define __BSP__FLASH__MX25U51245G_H__

#include "bsp/flash.h"

/**
 * @addtogroup Flash
 * @{
 */

/**
 * @defgroup Mx25u51245g Mx25u51245g
 *
 */

/**
 * @addtogroup Mx25u51245g
 * @{
 */

/**@{*/

/** \struct pi_mx25u51245g_conf
 * \brief Mx25u51245g configuration structure.
 *
 * This structure is used to pass the desired Mx25u51245g configuration to the
 * runtime when opening the device.
 */
struct pi_mx25u51245g_conf
{
  struct pi_flash_conf flash;  /*!< Generic flash configuration. */
    int spi_itf;           /*!< SPI interface where the RAM is
      connected. */
    int spi_cs;            /*!< Chip select where the RAM is connected. */
    int xip_en;
    uint32_t baudrate;     /*!< Baudrate (in bytes/second). */
};

/** \brief Initialize an Mx25u51245g configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * mx25u51245g device is opened.
 *
 * \param conf A pointer to the mx25u51245g configuration.
 */
void pi_mx25u51245g_conf_init(struct pi_mx25u51245g_conf *conf);


//!@}

/**
 * @} end of Mx25u51245g
 */

/**
 * @} end of Flash
 */

#endif 
