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

#ifndef __BSP_EEPROM_H__
#define __BSP_EEPROM_H__

#include "pmsis.h"

/// @cond IMPLEM

typedef struct __pi_eeprom_api_t pi_eeprom_api_t;

/// @endcond

/**
 * @defgroup EEPROM EEPROM
 *
 * The EEPROM API provides read and write access to Electrically Erasable
 * Programmable Read-Only Memory (EEPROM) devices.
 * EEPROMs have an erase block size of 1 byte, a long lifetime, and allow
 * overwriting data on byte-by-byte access.
 *
 */

/**
 * @addtogroup EEPROM
 * @{
 */

/**@{*/

/** \struct pi_eeprom_conf
 * \brief RAM configuration structure.
 *
 * This structure is used to pass the desired RAM configuration to the
 * runtime when opening the device. This configuration should be not be used
 * directly as it is meant to be encapsulated into a specific device
 * configuration.
 */
struct pi_eeprom_conf {
    pi_eeprom_api_t *api;   /*!< Pointer to specific RAM methods. Reserved for 
    internal runtime usage. */
};

/** \brief Open a RAM device.
 *
 * This function must be called before the RAM device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * The configuration associated to the device must specify the exact model of
 * RAM which must be opened.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_eeprom_open(struct pi_device *device);

/** \brief Close an opened RAM device.
 *
 * This function can be called to close an opened RAM device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
static inline void pi_eeprom_close(struct pi_device *device);

/** \brief Enqueue a read copy to the RAM (from RAM to processor).
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_eeprom_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_eeprom_read(struct pi_device *device, uint32_t eeprom_addr,
  void *data, uint32_t size);

static inline void pi_eeprom_read_async(struct pi_device *device, uint32_t eeprom_addr,
  void *data, uint32_t size, pi_task_t *task);

/** \brief Enqueue a write copy to the RAM (from processor to RAM).
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_eeprom_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_eeprom_write(struct pi_device *device, uint32_t eeprom_addr,
  void *data, uint32_t size);

static inline void pi_eeprom_write_async(struct pi_device *device, uint32_t eeprom_addr,
  void *data, uint32_t size, pi_task_t *task);

//!@}

/**
 * @} end of EEPROM
 */


/// @cond IMPLEM

typedef struct __pi_eeprom_api_t
{
    int (*open)(struct pi_device *device);
    void (*close)(struct pi_device *device);
    void (*read_async)(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task);
    void (*write_async)(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task);
} pi_eeprom_api_t;


static inline int pi_eeprom_open(struct pi_device *device)
{
    struct pi_eeprom_conf *conf = (struct pi_eeprom_conf *)device->config;
    pi_eeprom_api_t *api = conf->api;
    return api->open(device);
}

static inline void pi_eeprom_close(struct pi_device *device)
{
    pi_eeprom_api_t *api = (pi_eeprom_api_t *)device->api;
    api->close(device);
}

static inline void pi_eeprom_read_async(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task)
{
    pi_eeprom_api_t *api = (pi_eeprom_api_t *)device->api;
    api->read_async(device, eeprom_addr, data, size, task);
}

static inline void pi_eeprom_write_async(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task)
{
    pi_eeprom_api_t *api = (pi_eeprom_api_t *)device->api;
    api->write_async(device, eeprom_addr, data, size, task);
}

static inline void pi_eeprom_read(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size)
{
    pi_task_t task;
    pi_eeprom_read_async(device, eeprom_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_eeprom_write(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size)
{
    pi_task_t task;
    pi_eeprom_write_async(device, eeprom_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
}

/// @endcond




#endif  /* __BSP_RAM_H__ */
