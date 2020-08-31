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

#ifndef __BSP__VIRTUAL_EEPROM__H__
#define __BSP__VIRTUAL_EEPROM__H__

#include "pmsis.h"
#include "bsp/eeprom_slave.h"

/**
 * @addtogroup EEPROM_SLAVE
 * @{
 */

/**
 * @defgroup VIRTUAL_EEPROM VIRTUAL_EEPROM
 */

/**
 * @addtogroup VIRTUAL_EEPROM
 * @{
 */

/** \struct pi_virtual_eeprom_conf
 * \brief virtual eeprom configuration structure.
 *
 * This structure is used to pass the desired virtual eeprom configuration to the
 * runtime when opening the device.
 */
struct pi_virtual_eeprom_conf
{
    struct pi_eeprom_slave_conf eeprom_slave;   /*!< Generic EEPROM configuration. */
    uint16_t i2c_addr;
    int i2c_itf;
};

/** \brief Initialize a virtual eeprom configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * virtual eeprom device is opened.
 *
 * \param conf A pointer to the virtual eeprom configuration.
 */
void pi_virtual_eeprom_conf_init(struct pi_virtual_eeprom_conf *conf);

//!@}

/**
 * @} end of VIRTUAL_EEPROM
 */

/**
 * @} end of EEPROM
 */



#endif
