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

#ifndef __BSP__RAM__APS25XXXN_H__
#define __BSP__RAM__APS25XXXN_H__

#include "pmsis.h"
#include "bsp/ram.h"

/**
 * @addtogroup Ram
 * @{
 */

/**
 * @defgroup Aps25xxxn Aps25xxxn
 */

/**
 * @addtogroup Aps25xxxn
 * @{
 */

/** \struct pi_aps25xxxn_conf
 * \brief Aps25xxxn configuration structure.
 *
 * This structure is used to pass the desired Aps25xxxn configuration to the
 * runtime when opening the device.
 */
struct pi_aps25xxxn_conf
{
    struct pi_ram_conf ram;   /*!< Generic RAM configuration. */
    int spi_itf;           /*!< SPI interface where the RAM is
      connected. */
    int spi_cs;            /*!< Chip select where the RAM is connected. */
    int xip_en;            /*!< enable xip mode if set to 1. */
    int ram_start;         /*!< Aps25xxxn start address. */
    int ram_size;          /*!< Aps25xxxn size. */
    uint32_t baudrate;     /*!< Baudrate (in bytes/second). */
    int reserve_addr_0;    /*!< Reserve address 0 and never return a chunk with
      address 0. */
};

/** \brief Initialize an Aps25xxxn configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * aps25xxxn device is opened.
 *
 * \param conf A pointer to the aps25xxxn configuration.
 */
void pi_aps25xxxn_conf_init(struct pi_aps25xxxn_conf *conf);

//!@}

/**
 * @} end of Aps25xxxn
 */

/**
 * @} end of Ram
 */

#endif 
