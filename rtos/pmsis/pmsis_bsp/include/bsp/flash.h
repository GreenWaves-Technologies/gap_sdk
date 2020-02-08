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

#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "pmsis.h"

/// @cond IMPLEM

typedef struct __pi_flash_api_t pi_flash_api_t;

/// @endcond

/**
 * @defgroup Flash Flash
 *
 * The flash driver provides support for transferring data between an
 * external flash chip (e.g. Hyperflash or SPI flash) and the processor running
 * this driver.
 *
 */

/**
 * @addtogroup Flash
 * @{
 */

/**@{*/

/** \struct pi_flash_conf
 * \brief Flash configuration structure.
 *
 * This structure is used to pass the desired flash configuration to the
 * runtime when opening the device. This configuration should be not be used
 * directly as it is meant to be encapsulated into a specific device
 * configuration.
 */
struct pi_flash_conf {
  pi_flash_api_t *api;    /*!< Pointer to specific flash methods. Reserved for 
    internal runtime usage. */
};

/** \enum pi_flash_ioctl_e
 * \brief Command ID for pi_flash_ioctl.
 *
 */
typedef enum {
  PI_FLASH_IOCTL_INFO   /*!< Command for getting flash information. The argument
    must be a pointer to a variable of type struct pi_flash_info so that the
    call is returning information there. */
} pi_flash_ioctl_e;

/** \struct pi_flash_info
 * \brief Parameter for FLASH_IOCTL_INFO command.
 *
 * This structure is used to return flash information when executing the
 * FLASH_IOCTL_INFO ioctl command.
 */
struct pi_flash_info {
  uint32_t sector_size; /*!< Size in bytes of a sector. */
  uint32_t flash_start; /*!< Start address in the flash. What is before is
    reserved for runtime usage and should not be accessed. */
};

/** \brief Open a flash device.
 *
 * This function must be called before the flash device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * The configuration associated to the device must specify the exact model of
 * flash which must be opened.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_flash_open(struct pi_device *device);

/** \brief Close an opened flash device.
 *
 * This function can be called to close an opened flash device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
static inline void pi_flash_close(struct pi_device *device);

/** \brief Control device.
 *
 * This can be called to configure and control the device after it has been
 * opened.
 *
 * \param device    The device structure of the device to control.
 * \param cmd       The command to execute on the device.
 * \param arg       The argument to the command. The size and meaning of this
 *  parameter depends on the command which is passed.
 */
static inline int32_t pi_flash_ioctl(struct pi_device *device, uint32_t cmd,
  void *arg);

/** \brief Set flash register.
 *
 * This can be called to set a flash register. This must be used carefully
 * as this can disturb the behavior of the other calls. The list of registers
 * is specific to each flash, see the documentation of the flash for more
 * information.
 *
 * \param device    The device structure of the flash.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value to be set. The size of this variable
 *   depends on the register being accessed.
 */
static inline void pi_flash_reg_set(struct pi_device *device, uint32_t reg_addr,
  uint8_t *value);

/** \brief Get flash register.
 *
 * This can be called to get a flash register. The list of registers
 * is specific to each flash, see the documentation of the flash for more
 * information.
 *
 * \param device    The device structure of the flash.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value where the read value will be stored.
 *   The size of this variable depends on the register being accessed.
 */
static inline void pi_flash_reg_get(struct pi_device *device, uint32_t reg_addr,
  uint8_t *value);

/** \brief Set flash register asynchronously.
 *
 * This can be called to set a flash register. This must be used carefully
 * as this can disturb the behavior of the other calls. The list of registers
 * is specific to each flash, see the documentation of the flash for more
 * information.
 * A task must be specified in order to specify how the caller should be
 * notified when the operation is finished.
 *
 * \param device    The device structure of the flash.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value to be set. The size of this variable
 *   depends on the register being accessed.
 * \param task      The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_flash_reg_set_async(struct pi_device *device,
  uint32_t reg_addr, uint8_t *value, pi_task_t *task);

/** \brief Get flash register asynchronously.
 *
 * This can be called to get a flash register. The list of registers
 * is specific to each flash, see the documentation of the flash for more
 * information.
 * A task must be specified in order to specify how the caller should be
 * notified when the operation is finished.
 *
 * \param device    The device structure of the flash.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value where the read value will be stored.
 *   The size of this variable depends on the register being accessed.
 * \param task      The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_flash_reg_get_async(struct pi_device *device,
  uint32_t reg_addr, uint8_t *value, pi_task_t *task);

/** \brief Enqueue a read copy to the flash (from flash to processor).
 *
 * The copy will make a transfer between the flash and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the copy.
 * \param pi_flash_addr  The address of the copy in the flash.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_flash_read(struct pi_device *device, uint32_t pi_flash_addr,
  void *data, uint32_t size);

/** \brief Enqueue a write copy to the flash (from processor to flash).
 *
 * The copy will make a write transfer from one of the processor memory areas
 * to the flash.
 * The locations in the flash being written should have first been erased.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the copy.
 * \param pi_flash_addr    The address of the copy in the flash.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_flash_program(struct pi_device *device, uint32_t pi_flash_addr,
  const void *data, uint32_t size);

/** \brief Erase the whole flash.
 *
 * This will erase the entire flash. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * The caller is blocked until the operation is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 */
static inline void pi_flash_erase_chip(struct pi_device *device);

/** \brief Erase a sector.
 *
 * This will erase one sector. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * The caller is blocked until the operation is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 * \param pi_flash_addr  The address of the sector to be erased.
 */
static inline void pi_flash_erase_sector(struct pi_device *device,
  uint32_t pi_flash_addr);

/** \brief Erase an area in the flash.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * The caller is blocked until the operation is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 * \param pi_flash_addr  The address of the area to be erased.
 * \param size  The size of the area to be erased.
 */
static inline void pi_flash_erase(struct pi_device *device, uint32_t pi_flash_addr,
  int size);

/** \brief Enqueue an asynchronous read copy to the flash
 * (from flash to processor).
 *
 * The copy will make a transfer between the flash and one of the processor
 * memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the copy.
 * \param pi_flash_addr  The address of the copy in the flash.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_flash_read_async(struct pi_device *device,
  uint32_t pi_flash_addr, void *data, uint32_t size, pi_task_t *task);

/** \brief Enqueue an asynchronous write copy to the flash (from processor
 * to flash).
 *
 * The copy will make a write transfer from one of the processor memory areas
 * to the flash.
 * The locations in the flash being written should have first been erased.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the copy.
 * \param pi_flash_addr    The address of the copy in the flash.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_flash_program_async(struct pi_device *device,
  uint32_t pi_flash_addr, const void *data, uint32_t size, pi_task_t *task);

/** \brief Erase the whole flash asynchronously.
 *
 * This will erase the entire flash. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_flash_erase_chip_async(struct pi_device *device,
  pi_task_t *task);

/** \brief Erase a sector asynchronously.
 *
 * This will erase one sector. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 * \param pi_flash_addr  The address of the sector to be erased.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_flash_erase_sector_async(struct pi_device *device,
  uint32_t pi_flash_addr, pi_task_t *task);

/** \brief Erase an area in the flash asynchronously.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * \param device      The device descriptor of the flash chip on which to do
 *   the operation.
 * \param pi_flash_addr  The address of the area to be erased.
 * \param size  The size of the area to be erased.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_flash_erase_async(struct pi_device *device,
  uint32_t pi_flash_addr, int size, pi_task_t *task);

//!@}

/**
 * @} end of Flash
 */

/// @cond IMPLEM

typedef struct __pi_flash_api_t {
  int (*open)(struct pi_device *device);
  void (*close)(struct pi_device *device);
  int32_t (*ioctl)(struct pi_device *device, uint32_t cmd, void *arg);
  void (*read_async)(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size, pi_task_t *task);
  void (*program_async)(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size, pi_task_t *task);
  void (*erase_chip_async)(struct pi_device *device, pi_task_t *task);
  void (*erase_sector_async)(struct pi_device *device, uint32_t pi_flash_addr, pi_task_t *task);
  void (*erase_async)(struct pi_device *device, uint32_t pi_flash_addr, int size, pi_task_t *task);
  void (*reg_set_async)(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value, pi_task_t *task);
  void (*reg_get_async)(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value, pi_task_t *task);
  int (*copy_async)(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc, pi_task_t *task);
  int (*copy_2d_async)(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task);
  // synchronous implems
  int (*read)(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size);
  int (*program)(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size);
  int (*erase_chip)(struct pi_device *device);
  int (*erase_sector)(struct pi_device *device, uint32_t pi_flash_addr);
  int (*erase)(struct pi_device *device, uint32_t pi_flash_addr, int size);
  int (*reg_set)(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value);
  int (*reg_get)(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value);
  int (*copy)(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc);
  int (*copy_2d)(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc);
} pi_flash_api_t;


static inline void pi_flash_close(struct pi_device *device)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->close(device);
}

static inline int32_t pi_flash_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  return api->ioctl(device, cmd, arg);
}

static inline void pi_flash_reg_set_async(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->reg_set_async(device, pi_flash_addr, value, task);
}

static inline void pi_flash_reg_set(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->reg_set(device, pi_flash_addr, value);
}

static inline void pi_flash_reg_get_async(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->reg_get_async(device, pi_flash_addr, value, task);
}

static inline void pi_flash_reg_get(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->reg_get(device, pi_flash_addr, value);
}

static inline void pi_flash_read_async(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->read_async(device, pi_flash_addr, data, size, task);
}

static inline void pi_flash_read(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->read(device, pi_flash_addr, data, size);
}

static inline void pi_flash_program_async(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->program_async(device, pi_flash_addr, data, size, task);
}

static inline void pi_flash_program(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->program(device, pi_flash_addr, data, size);
}

static inline void pi_flash_erase_chip_async(struct pi_device *device, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase_chip_async(device, task);
}

// this operation is really long and asynchronous or threaded call is strongly advised
static inline void pi_flash_erase_chip(struct pi_device *device)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase_chip(device);
}

static inline void pi_flash_erase_sector_async(struct pi_device *device, uint32_t pi_flash_addr, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase_sector_async(device, pi_flash_addr, task);
}

static inline void pi_flash_erase_sector(struct pi_device *device, uint32_t pi_flash_addr)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase_sector(device, pi_flash_addr);
}

static inline void pi_flash_erase_async(struct pi_device *device, uint32_t pi_flash_addr, int size, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase_async(device, pi_flash_addr, size, task);
}

static inline void pi_flash_erase(struct pi_device *device, uint32_t pi_flash_addr, int size)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  api->erase(device, pi_flash_addr, size);
}

static inline int pi_flash_copy_async(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  return api->copy_async(device, pi_flash_addr, buffer, size, ext2loc, task);
}

static inline int pi_flash_copy_2d_async(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  return api->copy_2d_async(device, pi_flash_addr, buffer, size, stride, length, ext2loc, task);
}

static inline int pi_flash_copy(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  return api->copy(device, pi_flash_addr, buffer, size, ext2loc);
}

static inline int pi_flash_copy_2d(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc)
{
  pi_flash_api_t *api = (pi_flash_api_t *)device->api;
  return api->copy_2d(device, pi_flash_addr, buffer, size, stride, length, ext2loc);
}

void __flash_conf_init(struct pi_flash_conf *conf);

/// @endcond




#endif
