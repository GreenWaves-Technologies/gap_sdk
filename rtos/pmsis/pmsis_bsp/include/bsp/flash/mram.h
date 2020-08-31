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


#ifndef __BSP__FLASH__MRAM_H__
#define __BSP__FLASH__MRAM_H__

#include "bsp/flash.h"

/**
 * @addtogroup Flash
 * @{
 */

/**
 * @defgroup Mram Mram
 *
 */

/**
 * @addtogroup Mram
 * @{
 */

/**@{*/

/** \struct mram_conf
 * \brief Mram configuration structure.
 *
 * This structure is used to pass the desired Mram configuration to the
 * runtime when opening the device.
 */
struct pi_mram_conf
{
  struct pi_flash_conf flash;  /*!< Generic flash configuration. */
  int itf;                /*!< Mram interface where the flash is
      connected. */
  int baudrate;                /*!< Baudrate in byte/s. */
};

/** \brief Initialize an Mram configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * mram device is opened.
 *
 * \param conf A pointer to the mram configuration.
 */
void pi_mram_conf_init(struct pi_mram_conf *conf);

//!@}

/**
 * @} end of Mram
 */

/**
 * @} end of Flash
 */

#endif 
