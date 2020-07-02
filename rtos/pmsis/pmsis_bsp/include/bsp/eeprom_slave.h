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

#ifndef __BSP_EEPROM_SLAVE_H__
#define __BSP_EEPROM_SLAVE_H__

#include "pmsis.h"

/// @cond IMPLEM

typedef struct __pi_eeprom_slave_api_t pi_eeprom_slave_api_t;

/// @endcond

/**
 * @defgroup EEPROM_SLAVE EEPROM_SLAVE
 *
 * The EEPROM Slave API provides simple means to simulate an Electrically Erasable
 * Programmable Read-Only Memory (EEPROM) device.
 */

/**
 * @addtogroup EEPROM_SLAVE
 * @{
 */

/**@{*/

/**
 * \struct pi_eeprom_slave_conf
 * \brief EEPROM slave configuration structure.
 */
struct pi_eeprom_slave_conf {
    pi_eeprom_slave_api_t *api;   /*!< Pointer to specific RAM methods. Reserved for
    internal runtime usage. */
};

/** \brief Open an eeprom slave
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_eeprom_slave_open(struct pi_device *device);

/** \brief Close an opened eeprom slave
 *
 * \param device    The device structure of the device to close.
 */
static inline void pi_eeprom_slave_close(struct pi_device *device);

//!@}

/**
 * @} end of EEPROM_SLAVE
 */


/// @cond IMPLEM

typedef struct __pi_eeprom_slave_api_t
{
    int (*open)(struct pi_device *device);
    void (*close)(struct pi_device *device);
} pi_eeprom_slave_api_t;


static inline int pi_eeprom_slave_open(struct pi_device *device)
{
    struct pi_eeprom_slave_conf *conf = (struct pi_eeprom_slave_conf *)device->config;
    pi_eeprom_slave_api_t *api = conf->api;
    return api->open(device);
}

static inline void pi_eeprom_slave_close(struct pi_device *device)
{
    pi_eeprom_slave_api_t *api = (pi_eeprom_slave_api_t *)device->api;
    api->close(device);
}

/// @endcond

#endif  /* __BSP_EEPROM_SLAVE_H__ */
