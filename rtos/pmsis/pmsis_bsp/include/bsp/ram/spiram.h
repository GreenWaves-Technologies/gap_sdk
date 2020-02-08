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

#ifndef __BSP__RAM__SPIRAM_H__
#define __BSP__RAM__SPIRAM_H__

#include "pmsis.h"
#include "bsp/ram.h"

/**
 * @addtogroup Ram
 * @{
 */

/** \struct pi_spiram_conf
 * \brief SPI ram configuration structure.
 *
 * This structure is used to pass the desired SPI ram configuration to the
 * runtime when opening the device.
 */
struct pi_spiram_conf
{
  struct pi_ram_conf ram;   /*!< Generic RAM configuration. */
  int spi_itf;           /*!< SPI interface where the RAM is
      connected. */
  int spi_cs;            /*!< Chip select where the RAM is connected. */
  char skip_pads_config; /*!< Skip pads configuration if set to 1. */
  int ram_start;         /*!< SPI ram start address. */
  int ram_size;          /*!< SPI ram size. */
  uint32_t baudrate;     /*!< Baudrate (in bytes/second). */
};




/** \brief Initialize an SPI ram configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the SPI
 * ram device is opened.
 *
 * \param conf A pointer to the SPI ram configuration.
 */
void pi_spiram_conf_init(struct pi_spiram_conf *conf);

//!@}



#endif 