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


#ifndef __BSP__FLASH__SPIFLASH_H__
#define __BSP__FLASH__SPIFLASH_H__

#include "bsp/flash.h"

/**
 * @addtogroup Flash
 * @{
 */

/**
 * @defgroup spiflash spiflash
 *
 */

/**
 * @addtogroup spiflash
 * @{
 */

/**@{*/

/** \struct spiflash_conf
 * \brief spiflash configuration structure.
 *
 * This structure is used to pass the desired spiflash configuration to the
 * runtime when opening the device.
 */
struct pi_spiflash_conf
{
  struct pi_flash_conf flash;   /*!< Generic flash configuration. */
  int spi_cs;                   /*!< Chip select where the flash is connected.*/
  int spi_itf;                  /*!< Chip select where the flash is connected.*/
  size_t size;                  /*!< Size of the connected flash.*/
  size_t sector_size;           /*!< Sector size of the connected flash.*/
  uint32_t baudrate;            /*!< baudrate of the underlying interface. */
};

/** \brief Initialize an spiflash configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * spiflash device is opened.
 *
 * \param conf A pointer to the spiflash configuration.
 */
void pi_spiflash_conf_init(struct pi_spiflash_conf *conf);

//!@}

/**
 * @} end of spiflash
 */

/**
 * @} end of Flash
 */

#endif 
