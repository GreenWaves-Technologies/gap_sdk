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

#ifndef __BSP__EEPROM__m24c02_H__
#define __BSP__EEPROM__m24c02_H__

#include "pmsis.h"
#include "bsp/eeprom.h"

/**
 * @addtogroup EEPROM
 * @{
 */

/**
 * @defgroup m24c02 m24c02
 */

/**
 * @addtogroup m24c02
 * @{
 */

/** \struct pi_m24c02_conf
 * \brief m24c02 configuration structure.
 *
 * This structure is used to pass the desired m24c02 configuration to the
 * runtime when opening the device.
 */
struct pi_m24c02_conf
{
    struct pi_eeprom_conf eeprom;   /*!< Generic EEPROM configuration. */
    uint16_t i2c_addr;
    int i2c_itf;
};

/** \brief Initialize an m24c02 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * m24c02 device is opened.
 *
 * \param conf A pointer to the m24c02 configuration.
 */
void pi_m24c02_conf_init(struct pi_m24c02_conf *conf);

//!@}

/**
 * @} end of m24c02
 */

/**
 * @} end of EEPROM
 */



#endif
