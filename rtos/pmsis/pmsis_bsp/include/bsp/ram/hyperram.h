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

#ifndef __BSP__RAM__HYPERRAM_H__
#define __BSP__RAM__HYPERRAM_H__

#include "pmsis.h"
#include "bsp/ram.h"

/**
 * @addtogroup Ram
 * @{
 */

/**
 * @defgroup Hyperram Hyperram
 */

/**
 * @addtogroup Hyperram
 * @{
 */

/** \struct pi_hyperram_conf
 * \brief Hyperram configuration structure.
 *
 * This structure is used to pass the desired Hyperram configuration to the
 * runtime when opening the device.
 */
struct pi_hyperram_conf
{
    struct pi_ram_conf ram;   /*!< Generic RAM configuration. */
    int hyper_itf;         /*!< Hyperbus interface where the RAM is
      connected. */
    int hyper_cs;          /*!< Chip select where the RAM is connected. */
    char skip_pads_config; /*!< Skip pads configuration if set to 1. */
    int xip_en;            /*!< enable xip mode if set to 1. */
    int ram_start;         /*!< Hyperram start address. */
    int ram_size;          /*!< Hyperram size. */
    uint32_t baudrate;     /*!< Baudrate (in bytes/second). */
    int reserve_addr_0;    /*!< Reserve address 0 and never return a chunk with
      address 0. */
};

/** \brief Initialize an Hyperram configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * hyperram device is opened.
 *
 * \param conf A pointer to the hyperram configuration.
 */
void pi_hyperram_conf_init(struct pi_hyperram_conf *conf);

//!@}

/**
 * @} end of Hyperram
 */

/**
 * @} end of Ram
 */

#endif 
