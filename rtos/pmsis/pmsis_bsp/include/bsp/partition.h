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

#include "stdint.h"
#include "stdbool.h"

#include "pmsis/errno.h"
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

/**
 * @brief Partition type
 * @note Keep this enum in sync with PartitionDefinition class in gapy tool
 */
typedef enum {
    PI_PARTITION_TYPE_APP = 0x00,       //!< Application partition type
    PI_PARTITION_TYPE_DATA = 0x01,      //!< Data partition type
} pi_partition_type_t;

/**
 * @brief Partition subtype
 * @note Keep this enum in sync with PartitionDefinition class in gapy tool.
 */
typedef enum {
    PI_PARTITION_SUBTYPE_APP_FACTORY = 0x00,                                 //!< Factory application partition
    PI_PARTITION_SUBTYPE_APP_OTA_0 = 0x10,  //!< OTA partition 0
    PI_PARTITION_SUBTYPE_APP_OTA_1 = 0x11,  //!< OTA partition 1
    PI_PARTITION_SUBTYPE_APP_TEST = 0x20,                                    //!< Test application partition
    PI_PARTITION_SUBTYPE_APP_UPDATER_0 = 0x30,  //!< Updater partition 0
    PI_PARTITION_SUBTYPE_APP_UPDATER_1 = 0x31,  //!< Updater partition 1
    
    PI_PARTITION_SUBTYPE_APP_0 = 0x40,  //!< App partition 0 to run once
    PI_PARTITION_SUBTYPE_APP_1 = 0x41,  //!< App partition 1 to run once
    PI_PARTITION_SUBTYPE_APP_2 = 0x42,  //!< App partition 2 to run once
    PI_PARTITION_SUBTYPE_APP_3 = 0x43,  //!< App partition 3 to run once
    PI_PARTITION_SUBTYPE_APP_4 = 0x44,  //!< App partition 4 to run once
    PI_PARTITION_SUBTYPE_APP_5 = 0x45,  //!< App partition 5 to run once
    PI_PARTITION_SUBTYPE_APP_6 = 0x46,  //!< App partition 6 to run once
    PI_PARTITION_SUBTYPE_APP_7 = 0x47,  //!< App partition 7 to run once
    PI_PARTITION_SUBTYPE_APP_8 = 0x48,  //!< App partition 8 to run once
    PI_PARTITION_SUBTYPE_APP_9 = 0x49,  //!< App partition 9 to run once
    
    PI_PARTITION_SUBTYPE_DATA_OTA = 0x00,                                    //!< OTA selection partition
    PI_PARTITION_SUBTYPE_DATA_PHY = 0x01,                                    //!< PHY init data partition
    
    PI_PARTITION_SUBTYPE_DATA_RAW = 0x80,                                    //!< RAW space partition
    PI_PARTITION_SUBTYPE_DATA_READFS = 0x81,                               //!< Readonly filesystem partition
    PI_PARTITION_SUBTYPE_DATA_LFS = 0x82,                                    //!< LittleFS filesystem partition
    
    pi_PARTITION_SUBTYPE_ANY = 0xfe,                                         //!< Used to search for partitions with any subtype
    PI_PARTITION_SUBTYPE_UNKNOWN = 0xff,                                    //!< LittleFS filesystem partition
} pi_partition_subtype_t;

/**
 * @brief partition information structure
 */
typedef struct partition {
    struct pi_device *flash;         /*!< Flash device on which the partition resides */
    pi_partition_type_t type;            /*!< partition type (app/data) */
    pi_partition_subtype_t subtype;  /*!< partition subtype */
    uint32_t offset;                 /*!< starting address of the partition in flash */
    uint32_t size;                   /*!< size of the partition, in bytes */
    char label[17];                  /*!< partition label, zero-terminated ASCII string */
    bool encrypted;                  /*!< flag is set to true if partition is encrypted */
    bool read_only;                  /*!< flag is set to true if partition is read only */
} pi_partition_t;

/**
 * @brief partition table object
 */
typedef void *pi_partition_table_t;


/**
 * @brief Open a partition table from a flash device.
 * @param flash The flash device in which to fetch the partition table.
 * @param table
 * A reference to the user table variable. if the return code is PI_OK, this pointer contains a reference to the new partition table.
 * @return
 * PI_OK on success;
 * PI_ERR_INVALID_ARG if the table pointer is NULL or if the flash device is invalid;
 *PI_ERR_L2_NO_MEM if the allocation of the table in L2 memory fails;
 *PI_ERR_NOT_FOUND if the partition table is not present on the flash device;
 * PI_ERR_INVALID_VERSION if the format version of the partition table missmatch between flash data and BSP program;
 * PI_ERR_INVALID_STATE if partition table MD5 missmatch (Partition table data corrupted).
 */
pi_err_t pi_partition_table_load(pi_device_t *flash, const pi_partition_table_t *table);

/**
 * @brief Close an opened partition table from pi_partition_table_load.
 * @param table A reference of the partition table to free.
 */
void pi_partition_table_free(pi_partition_table_t table);

/**
 * @brief Find first partition based on one or more parameters
 *
 * @param table the partition table descriptor on which to do the operation. This table must be loaded before with pi_partition_table_load(...)
 * @param type Partition type, one of pi_partition_type_t values
 * @param subtype Partition subtype, one of pi_partition_subtype_t values.
 *                To find all partitions of given type, use
 *                pi_PARTITION_SUBTYPE_ANY.
 * @param label (optional) Partition label. Set this value if looking
 *             for partition with a specific name. Pass NULL otherwise.
 *
 * @return pointer to pi_partition_t structure, or NULL if no partition is found.
 *         This pointer is valid for the lifetime of the application.
 */
const pi_partition_t *
pi_partition_find_first(const pi_partition_table_t table, const pi_partition_type_t type,
                        const pi_partition_subtype_t subtype, const char *label);

/** @brief Close an opened partition device.
 *
 * This function can be called to close an opened partition device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * @param partition
 * The partition structure of the device to close.
 * @return PI_OK if the operation is successfull, PI_FAIL if there was an error.
 */
static inline pi_err_t pi_partition_close(const pi_partition_t *partition);

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
 * @param partition
 * The partition descriptor on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
 */
static inline pi_err_t pi_partition_read_async(const pi_partition_t *partition, const uint32_t partition_addr,
                                               void *data, const size_t size, pi_task_t *task);

/** @brief Enqueue a read copy to the flash partition (from flash to processor).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param partition
 * The partition descriptor on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
 */
static inline pi_err_t pi_partition_read(const pi_partition_t *partition, const uint32_t partition_addr,
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
 * @param partition
 * The partition descriptor on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
 */
static inline pi_err_t
pi_partition_write_async(const pi_partition_t *partition, const uint32_t partition_addr, const void *data,
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
 * @param partition
 * The partition descriptor on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
 */
static inline pi_err_t
pi_partition_write(const pi_partition_t *partition, const uint32_t partition_addr, const void *data, const size_t size);

/** @brief Erase an area in the flash partition asynchronously.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param partition
 * The partition descriptor on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
*/
static inline pi_err_t
pi_partition_erase_async(const pi_partition_t *partition, uint32_t partition_addr, int size, pi_task_t *task);

/** @brief Erase an area in the flash partition.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * The caller is blocked until the operation is finished.
 *
 * @param partition
 * The partition descriptor on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @return PI_OK if the operation is successfull, PI_ERR_INVALID_ARG if adress is out of range.
 */
static inline pi_err_t pi_partition_erase(const pi_partition_t *partition, uint32_t partition_addr, int size);

/** @brief Erase the whole flash partition asynchronously.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param partition
 * The partition descriptor on which to do the operation.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return PI_OK if the operation is successfull, PI_FAIL if there was an error.
 */
//static inline pi_err_t pi_partition_format_async(const pi_partition_t *partition, pi_task_t *task);

/** @brief Erase the whole flash partition.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * The caller is blocked until the operation is finished.
 *
 * @param partition
 * The partition descriptor on which to do the operation.
 * @return PI_OK if the operation is successfull, PI_FAIL if there was an error.
 */
//static inline pi_err_t pi_partition_format(const pi_partition_t *partition);

/** @brief Get the size in byte of the partition
 *
 * @param partition
 * The partition where the size will be fetched.
 * @return The size in byte of the partition
 */
size_t pi_partition_get_size(const pi_partition_t *partition);

/** @brief Get flash partition start offset
 *
 * @param partition
 * The partition where the offset will be fetched.
 * @return The flash offset in byte where the partition starts.
 */
uint32_t pi_partition_get_flash_offset(const pi_partition_t *partition);

//!@}

/**
 * @} end of Partition
 */

/// @cond IMPLEM

static inline pi_err_t pi_partition_close(const pi_partition_t *partition)
{
    pi_l2_free((pi_partition_t *) partition, sizeof(pi_partition_t));
    return PI_OK;
}

#define CHECK_ADDR() if (partition_addr + size > partition->size) return PI_ERR_INVALID_ARG

static inline pi_err_t pi_partition_read_async(const pi_partition_t *partition, const uint32_t partition_addr,
                                               void *data, const size_t size, pi_task_t *task)
{
    CHECK_ADDR();
    pi_flash_read_async(partition->flash, partition_addr + partition->offset, data, size, task);
    return PI_OK;
}

static inline pi_err_t pi_partition_read(const pi_partition_t *partition, const uint32_t partition_addr,
                                         void *data, const size_t size)
{
    pi_err_t rc;
    pi_task_t task;
    
    pi_task_block(&task);
    rc = pi_partition_read_async(partition, partition_addr, data, size, &task);
    if(rc != PI_OK)
        return rc;
    pi_task_wait_on(&task);
    return PI_OK;
}

static inline pi_err_t
pi_partition_write_async(const pi_partition_t *partition, const uint32_t partition_addr, const void *data,
                         const size_t size, pi_task_t *task)
{
    CHECK_ADDR();
    pi_flash_program_async(partition->flash, partition_addr + partition->offset, data, size, task);
    return PI_OK;
}

static inline pi_err_t
pi_partition_write(const pi_partition_t *partition, const uint32_t partition_addr, const void *data, const size_t size)
{
    pi_err_t rc;
    pi_task_t task;
    
    pi_task_block(&task);
    rc = pi_partition_write_async(partition, partition_addr, data, size, &task);
    if(rc != PI_OK)
        return rc;
    pi_task_wait_on(&task);
    return PI_OK;
}

static inline pi_err_t
pi_partition_erase_async(const pi_partition_t *partition, uint32_t partition_addr, int size, pi_task_t *task)
{
    CHECK_ADDR();
    pi_flash_erase_async(partition->flash, partition_addr + partition->offset, size, task);
    return PI_OK;
}

static inline pi_err_t pi_partition_erase(const pi_partition_t *partition, uint32_t partition_addr, int size)
{
    pi_err_t rc;
    pi_task_t task;
    
    pi_task_block(&task);
    rc = pi_partition_erase_async(partition, partition_addr, size, &task);
    if(rc != PI_OK)
        return rc;
    pi_task_wait_on(&task);
    return PI_OK;
}

static inline pi_err_t pi_partition_format_async(const pi_partition_t *partition, pi_task_t *task)
{
    return pi_partition_erase_async(partition, 0, partition->size, task);
}

static inline pi_err_t pi_partition_format(const pi_partition_t *partition)
{
    pi_err_t rc;
    pi_task_t task;
    
    pi_task_block(&task);
    rc = pi_partition_format_async(partition, &task);
    if(rc != PI_OK)
        return rc;
    pi_task_wait_on(&task);
    return PI_OK;
}

/// @endcond

#endif
