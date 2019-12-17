/*
 * Copyright (C) 2018 GreenWaves Technologies
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

/*
 * Authors: Mathieu Barbe, GreenWaves Technologies (mathieu.barbe@greenwaves-technologies.com)
 */

#ifndef __BSP_PARTITION_H__
#define __BSP_PARTITION_H__

#include "pmsis.h"
#include "bsp/flash.h"

/**
 * @defgroup Partition Partition
 *
 * The partition driver provides support for handling partition
 * contained into a device storage.
 * Currently, PMSIS and GAP SDK provide both partitions, binary firmware and filesystem.
 *
 * - 0 -> Binary firmware;
 * - 1 -> Filesystem or Free space.
 *
 * The beginning  of the filesystem partition is aligned on a flash sector.
 * Thus, an erase partition operation at address zero does not affect the data of the previous partition.
 */

/**
 * @addtogroup Partition
 * @{
 */

/**@{*/

/** @struct pi_partition_conf
 * @brief Partition configuration structure.
 *
 */
struct pi_partition_conf {
    uint8_t id;
    /*!< The partition number: 0 - Firmware binary; 1 - Filesystem or free space. */
    pi_device_t *flash; /*!<
 * The flash device where the partition is stored. */
};

/** @brief Open a partition device.
 *
 * This function must be called before the partition can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * @param device
 * A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * @return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_partition_open(struct pi_device *device);

/** @brief Close an opened partition device.
 *
 * This function can be called to close an opened partition device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * @param device
 * The device structure of the device to close.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_close(struct pi_device *device);

/** @brief Enqueue an asynchronous read copy to the flash partition
 * (from flash partition to processor).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_read_async(struct pi_device *device, const uint32_t partition_addr,
                        void *data, const size_t size, pi_task_t *task);

/** @brief Enqueue a read copy to the flash partition (from flash to processor).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_read(struct pi_device *device, const uint32_t partition_addr,
                  void *data, const size_t size);

/** @brief Enqueue an asynchronous read copy to the flash partition
 * (from processor to flash partition).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 *
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_write_async(struct pi_device *device, const uint32_t partition_addr, const void *data,
                         const size_t size, pi_task_t *task);

/** @brief Enqueue a write copy to the flash partition (from processor to flash).
 *
 * The copy will make a write transfer from one of the processor memory areas
 * to the partition.
 * The locations in the flash being written should have first been erased.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_write(struct pi_device *device, const uint32_t partition_addr, const void *data, const size_t size);

/** @brief Erase an area in the flash partition asynchronously.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
  * @return 0 if the operation is successfull, -1 if there was an error.
*/
static inline int pi_partition_erase_async(struct pi_device *device, uint32_t partition_addr, int size, pi_task_t *task);

/** @brief Erase an area in the flash partition.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * The caller is blocked until the operation is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_erase(struct pi_device *device, uint32_t partition_addr, int size);

/** @brief Erase the whole flash partition asynchronously.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_format_async(struct pi_device *device, pi_task_t *task);

/** @brief Erase the whole flash partition.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * The caller is blocked until the operation is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_format(struct pi_device *device);

/** @brief Get the size in byte of the partition
 *
 * @param device
 * The partition where the size will be fetched.
 * @return The size in byte of the partition
 */
size_t pi_partition_get_size(pi_device_t *device);

/** @brief Get flash partition start offset
 *
 * @param device
 * The partition where the offset will be fetched.
 * @return The flash offset in byte where the partition starts.
 */
uint32_t pi_partition_get_flash_offset(pi_device_t *device);

//!@}

/**
 * @} end of Partition
 */

/// @cond IMPLEM

typedef struct partition {
    struct pi_device *flash;
    uint32_t offset;
    uint32_t size;
} pi_partition_t;

static inline int pi_partition_close(struct pi_device *device)
{
    pi_l2_free(device->data, sizeof(pi_partition_t));
    return 0;
}

#define CHECK_ADDR() if (partition_addr + size > partition->size) return -1

static inline int pi_partition_read_async(struct pi_device *device, const uint32_t partition_addr,
                                          void *data, const size_t size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_read_async(conf->flash, partition_addr + partition->offset, data, size, task);
    return 0;
}

static inline int pi_partition_read(struct pi_device *device, const uint32_t partition_addr,
                                    void *data, const size_t size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_read_async(device, partition_addr, data, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_write_async(struct pi_device *device, const uint32_t partition_addr, const void *data,
                                           const size_t size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_program_async(conf->flash, partition_addr + partition->offset, data, size, task);
    return 0;
}

static inline int pi_partition_write(struct pi_device *device, const uint32_t partition_addr, const void *data, const size_t size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_write_async(device, partition_addr, data, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_erase_async(struct pi_device *device, uint32_t partition_addr, int size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_erase_async(conf->flash, partition_addr + partition->offset, size, task);
    return 0;
}

static inline int pi_partition_erase(struct pi_device *device, uint32_t partition_addr, int size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_erase_async(device, partition_addr, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_erase_partition_async(struct pi_device *device, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;

    return pi_partition_erase_async(device, 0, partition->size, task);
}

static inline int pi_partition_erase_partition(struct pi_device *device)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_erase_partition_async(device, &task);
    if (rc < 0)
        return 0;
    pi_task_wait_on(&task);
    return 0;
}

size_t pi_partition_get_size(pi_device_t *device);

uint32_t pi_partition_get_flash_offset(pi_device_t *device);

/// @endcond

#endif
