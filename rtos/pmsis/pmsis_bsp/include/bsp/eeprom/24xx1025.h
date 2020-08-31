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

#ifndef __BSP__EEPROM__24XX1025_H__
#define __BSP__EEPROM__24XX1025_H__

#include "pmsis.h"
#include "bsp/eeprom.h"

/**
 * @addtogroup EEPROM
 * @{
 */

/**
 * @defgroup 24XX1025 24XX1025
 */

/**
 * @addtogroup 24XX1025
 * @{
 */

/** \struct pi_24xx1025_conf
 * \brief 24XX1025 configuration structure.
 *
 * This structure is used to pass the desired 24XX1025 configuration to the
 * runtime when opening the device.
 */
struct pi_24xx1025_conf
{
    struct pi_eeprom_conf eeprom;   /*!< Generic EEPROM configuration. */
    uint16_t i2c_addr;
    int i2c_itf;
};

/** \brief Initialize an 24XX1025 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * 24XX1025 device is opened.
 *
 * \param conf A pointer to the 24XX1025 configuration.
 */
void pi_24xx1025_conf_init(struct pi_24xx1025_conf *conf);

//!@}

/**
 * @} end of 24XX1025
 */

/**
 * @} end of EEPROM
 */



#endif
