/*
 * Copyright (c) 2018 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "kv_config.h"
#include "KVStore.h"
#include "KVMap.h"
#include "BlockDevice.h"
#include "FileSystem.h"
#include "FileSystemStore.h"
#include "SlicingBlockDevice.h"
#include "FATFileSystem.h"
#include "LittleFileSystem.h"
#include "TDBStore.h"
#include "mbed_error.h"
#include "FlashIAP.h"
#include "FlashSimBlockDevice.h"
#include "mbed_trace.h"
#include "SecureStore.h"
#define TRACE_GROUP "KVCFG"

#if COMPONENT_FLASHIAP
#include "FlashIAPBlockDevice.h"
#endif

#if COMPONENT_QSPIF
#include "QSPIFBlockDevice.h"
#endif

#if COMPONENT_SPIF
#include "SPIFBlockDevice.h"
#endif

#if COMPONENT_DATAFLASH
#include "DataFlashBlockDevice.h"
#endif

#if COMPONENT_SD
#include "SDBlockDevice.h"
#endif

/**
 * @brief This function initializes internal memory secure storage
 *        This includes a TDBStore instance with a FlashIAPBlockdevice
 *        as the supported storage.
 *        The following is a list of configuration parameter
 *        MBED_CONF_STORAGE_TDB_INTERNAL_SIZE - The size of the underlying FlashIAPBlockdevice
 *        MBED_CONF_STORAGE_TDB_INTERNAL_BASE_ADDRESS - The start address of the underlying FlashIAPBlockdevice
 * @returns 0 on success or negative value on failure.
 */
int _storage_config_TDB_INTERNAL();

/**
 * @brief This function initialize external memory secure storage
 *        This includes a SecureStore class with TDBStore over FlashIAPBlockdevice
 *        and an external TDBStore over a default blockdevice unless configured differently.
 *        The following is a list of configuration parameter:
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_RBP_INTERNAL_SIZE - Size of the internal FlashIAPBlockDevice and by
 *                                                           default is set to from start address to the end of the flash.
 *                                                           If start address is 0 the start address will be set to end of
 *                                                           flash - rbp_internal_size.
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_INTERNAL_BASE_ADDRESS - The satrt address of the internal FlashIAPBlockDevice.
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_SIZE - Size of the external blockdevice in bytes or NULL for
 *                                                       max possible size.
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_BASE_ADDRESS - The block device start address.
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_BLOCK_DEVICE - Alowed vlaues are: default, SPIF, DATAFASH, QSPIF or SD
 * @returns 0 on success or negative value on failure.
 */
int _storage_config_TDB_EXTERNAL();

/**
 * @brief This function initialize a external memory secure storage
 *        This includes a SecureStore class with external TDBStore over a blockdevice or,
 *        if no blockdevice was set the default blockdevice will be used.
 *        The following is a list of configuration parameter:
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_SIZE - Size of the external blockdevice in bytes
 *                                                              or NULL for max possible size.
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_BASE_ADDRESS - The block device start address
 *        MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_BLOCK_DEVICE - Alowed vlaues are: default, SPIF, DATAFASH, QSPIF or SD
 * @returns 0 on success or negative value on failure.
 */
int _storage_config_TDB_EXTERNAL_NO_RBP();

/**
 * @brief This function initialize a FILESYSTEM memory secure storage
 *        This includes a SecureStore class with TDBStore over FlashIAPBlockdevice
 *        in the internal memory and an external FileSysteStore. If blockdevice and filesystem not set,
 *        the system will use the default block device and default filesystem
 *        The following is a list of configuration parameter:
 *        MBED_CONF_STORAGE_FILESYSTEM_RBP_INTERNAL_SIZE - Size of the internal FlashIAPBlockDevice and by
 *                                                         default is set to from start address to the end of the flash.
 *                                                         If start address is 0 the start address will be set to end of
 *                                                         flash - rbp_internal_size.
 *        MBED_CONF_STORAGE_FILESYSTEM_INTERNAL_BASE_ADDRESS - The satrt address of the internal FlashIAPBlockDevice.
 *        MBED_CONF_STORAGE_FILESYSTEM_FILESYSTEM - Allowed values are: default, FAT or LITTLE
 *        MBED_CONF_STORAGE_FILESYSTEM_BLOCKDEVICE - Allowed values are: default, SPIF, DATAFASH, QSPIF or SD
 *        MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_SIZE - External Blockdevice size in bytes or NULL for max possible size.
 *        MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_BASE_ADDRESS - The block device start address.
 *        MBED_CONF_STORAGE_FILESYSTEM_MOUNT_POINT - Where to mount the filesystem
 *        MBED_CONF_STORAGE_FILESYSTEM_FOLDER_PATH - The working folder paths
 *
 * @returns 0 on success or negative value on failure.
 */
int _storage_config_FILESYSTEM();

/**
 * @brief This function initialize a FILESYSTEM_NO_RBP memory secure storage with no
 *        rollback protection. This includes a SecureStore class an external FileSysteStore over a default
 *        filesystem with default blockdevice unless differently configured.
 *        The following is a list of configuration parameter:
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FILESYSTEM - Allowed values are: default, FAT or LITTLE
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_BLOCKDEVICE - Allowed values are: default, SPIF, DATAFASH, QSPIF or SD
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_SIZE - Blockdevice size in bytes. or NULL for max possible size.
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_BASE_ADDRESS - The block device start address.
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_MOUNT_POINT - Where to mount the filesystem
 *        MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FOLDER_PATH - The working folder paths
 *
 * @returns 0 on success or negative value on failure.
 */
int _storage_config_FILESYSTEM_NO_RBP();

int _storage_config_tdb_external_common();
int _storage_config_filesystem_common();

static const char *filesystemstore_folder_path = NULL;

using namespace mbed;


static SingletonPtr<PlatformMutex> mutex;
static bool is_kv_config_initialize = false;
static kvstore_config_t kvstore_config;

#define INTERNAL_BLOCKDEVICE_NAME FLASHIAP

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define _GET_FILESYSTEM_concat(dev, ...) _get_filesystem_##dev(__VA_ARGS__)
#define GET_FILESYSTEM(dev, ...) _GET_FILESYSTEM_concat(dev, __VA_ARGS__)

#define _GET_BLOCKDEVICE_concat(dev, ...) _get_blockdevice_##dev(__VA_ARGS__)
#define GET_BLOCKDEVICE(dev, ...) _GET_BLOCKDEVICE_concat(dev, __VA_ARGS__)

static inline uint32_t align_up(uint64_t val, uint64_t size)
{
    return (((val - 1) / size) + 1) * size;
}

static inline uint32_t align_down(uint64_t val, uint64_t size)
{
    return (((val) / size)) * size;
}

int _calculate_blocksize_match_tdbstore(BlockDevice *bd)
{
    bd_size_t size = bd->size();
    bd_size_t erase_size = bd->get_erase_size();
    bd_size_t number_of_sector = size / erase_size;

    if (number_of_sector < 2) {
        tr_warning("KV Config: There are less than two sectors - TDBStore will not work.");
        return -1;
    }


    if (number_of_sector % 2 != 0) {
        tr_warning("KV Config: Number of sectors is not an even number. Consider changing the BlockDevice size");
    }

    return MBED_SUCCESS;
}

int _get_addresses(BlockDevice *bd, bd_addr_t start_address, bd_size_t size, bd_addr_t *out_start_addr,
                   bd_addr_t *out_end_addr)
{
    bd_addr_t aligned_end_address;
    bd_addr_t end_address;
    bd_addr_t aligned_start_address;

    aligned_start_address = align_down(start_address, bd->get_erase_size(start_address));
    if (aligned_start_address != start_address) {
        tr_error("KV Config: Start address is not aligned. Better use %02llx", aligned_start_address);
        return -1;
    }

    if (size == 0) {
        (*out_start_addr) = aligned_start_address;
        (*out_end_addr) = bd->size();
        return 0;
    }

    end_address = start_address + size;
    aligned_end_address = align_up(end_address, bd->get_erase_size(end_address));
    if (aligned_end_address != end_address) {
        tr_error("KV Config: End address is not aligned. Consider changing the size parameter.");
        return -1;
    }

    if (aligned_end_address > bd->size()) {
        tr_error("KV Config: End address is out of boundaries");
        return -1;
    }

    (*out_start_addr) = aligned_start_address;
    (*out_end_addr) = aligned_end_address;
    return 0;
}

FileSystem *_get_filesystem_FAT(const char *mount)
{
    static FATFileSystem sdcard(mount);
    return &sdcard;

}

FileSystem *_get_filesystem_LITTLE(const char *mount)
{
    static LittleFileSystem flash(mount);
    return &flash;
}

FileSystemStore *_get_file_system_store(FileSystem *fs)
{
    static FileSystemStore fss(fs);
    return &fss;
}

FileSystem *_get_filesystem_default(const char *mount)
{
#if COMPONENT_QSPIF || COMPONENT_SPIF || COMPONENT_DATAFLASH
    return _get_filesystem_LITTLE(mount);
#elif COMPONENT_SD
    return _get_filesystem_FAT(mount);
#else
    return NULL;
#endif
}

//Calculates the start address of FLASHIAP block device for TDB_INTERNAL profile.
//If possible, the address will start 2 sectors after the end of code sector allowing
//some space for an application update.
int _get_flashiap_bd_default_addresses_tdb_internal(bd_addr_t *start_address, bd_size_t *size)
{
#if COMPONENT_FLASHIAP

    FlashIAP flash;

    if (*start_address != 0 || *size != 0) {
        return MBED_ERROR_INVALID_ARGUMENT;
    }

    //If default values are set, we should get the maximum available size of internal bd.
    if (flash.init() != 0) {
        return MBED_ERROR_FAILED_OPERATION;
    }

    *start_address = align_up(FLASHIAP_APP_ROM_END_ADDR, flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR));

    // Give the application a couple of spare sectors to grow (if there are such)
    bd_size_t spare_size_for_app = 0;
    bd_addr_t curr_addr = *start_address;
    bd_addr_t flash_end_address = flash.get_flash_start() + flash.get_flash_size();

    int spare_sectors_for_app = 2;
    int min_sectors_for_storage = 2;
    for (int i = 0; i < spare_sectors_for_app + min_sectors_for_storage - 1; i++) {
        bd_size_t sector_size = flash.get_sector_size(curr_addr);
        curr_addr += sector_size;
        if (curr_addr >= flash_end_address) {
            spare_size_for_app = 0;
            break;
        }

        if (i < spare_sectors_for_app) {
            spare_size_for_app += sector_size;
        }
    }
    *start_address += spare_size_for_app;

    flash.deinit();

#endif

    return MBED_SUCCESS;
}

//Calculates address and size for FLASHIAP block device in TDB_EXTERNAL and FILESYSTEM profiles.
//The size of the block device will be 2 sectors at the ends of the internal flash for
//the use of the rbp internal TDBStore.
int _get_flashiap_bd_default_addresses_rbp(bd_addr_t *start_address, bd_size_t *size)
{
#if COMPONENT_FLASHIAP

    bd_addr_t flash_end_address;
    bd_addr_t flash_start_address;
    bd_addr_t aligned_start_address;
    bd_addr_t flash_first_writable_sector_address;
    FlashIAP flash;

    if (*start_address != 0 || *size != 0) {
        return MBED_ERROR_INVALID_ARGUMENT;
    }

    int ret = flash.init();
    if (ret != 0) {
        return MBED_ERROR_INITIALIZATION_FAILED;
    }

    flash_first_writable_sector_address = align_up(FLASHIAP_APP_ROM_END_ADDR, flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR));
    flash_start_address = flash.get_flash_start();
    flash_end_address = flash_start_address + flash.get_flash_size();;
    *start_address = flash_end_address - 1;
    aligned_start_address = align_down(*start_address, flash.get_sector_size(*start_address));
    *size = (flash_end_address - aligned_start_address) * 2;
    *start_address = (flash_end_address - *size);
    aligned_start_address = align_down(*start_address, flash.get_sector_size(*start_address));

    flash.deinit();

    if (aligned_start_address < flash_first_writable_sector_address) {
        tr_error("KV Config: Internal block device start address overlapped ROM address ");
        return MBED_ERROR_INITIALIZATION_FAILED;
    }

#endif

    return MBED_SUCCESS;

}

BlockDevice *_get_blockdevice_FLASHIAP(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_FLASHIAP

    bd_addr_t flash_end_address;
    bd_addr_t flash_start_address;
    bd_addr_t flash_first_writable_sector_address;
    bd_addr_t aligned_start_address;
    bd_addr_t aligned_end_address;
    bd_addr_t end_address;
    FlashIAP flash;

    int ret = flash.init();
    if (ret != 0) {
        return NULL;
    }

    //Get flash parameters before starting
    flash_first_writable_sector_address = align_up(FLASHIAP_APP_ROM_END_ADDR, flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR));
    flash_start_address = flash.get_flash_start();
    flash_end_address = flash_start_address + flash.get_flash_size();;

    //Non default configuration
    if (start_address != 0) {

        aligned_start_address = align_down(start_address, flash.get_sector_size(start_address));
        if (start_address != aligned_start_address) {
            tr_error("KV Config: Internal block device start address is not aligned. Better use %02llx", aligned_start_address);
            flash.deinit();
            return NULL;
        }

        if (size == 0) {
            //The block device will have all space form start address to the end of the flash
            size = (flash_end_address - start_address);

            static FlashIAPBlockDevice bd(start_address, size);
            flash.deinit();
            return &bd;
        }

        if (size != 0) {
            end_address = start_address + size;
            if (end_address > flash_end_address) {
                tr_error("KV Config: Internal block device end address is out of boundaries");
                flash.deinit();
                return NULL;
            }

            aligned_end_address = align_up(end_address, flash.get_sector_size(end_address - 1));
            if (end_address != aligned_end_address) {
                tr_error("KV Config: Internal block device start address is not aligned. Consider changing the size parameter");
                flash.deinit();
                return NULL;
            }

            static FlashIAPBlockDevice bd(start_address, size);
            flash.deinit();
            return &bd;
        }
    }

    //Non default configuration start_address = 0
    start_address = flash_end_address - size;
    aligned_start_address = align_down(start_address, flash.get_sector_size(start_address));
    if (start_address != aligned_start_address) {
        tr_error("KV Config: Internal block device start address is not aligned. Consider changing the size parameter");
        flash.deinit();
        return NULL;
    }

    flash.deinit();

    if (aligned_start_address < flash_first_writable_sector_address) {
        tr_error("KV Config: Internal block device start address overlapped ROM address ");
        return NULL;
    }
    static FlashIAPBlockDevice bd(aligned_start_address, size);
    return &bd;

#else
    return NULL;
#endif
}

BlockDevice *_get_blockdevice_SPIF(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_SPIF

    bd_addr_t aligned_end_address;
    bd_addr_t aligned_start_address;

    static SPIFBlockDevice bd(
        MBED_CONF_SPIF_DRIVER_SPI_MOSI,
        MBED_CONF_SPIF_DRIVER_SPI_MISO,
        MBED_CONF_SPIF_DRIVER_SPI_CLK,
        MBED_CONF_SPIF_DRIVER_SPI_CS,
        MBED_CONF_SPIF_DRIVER_SPI_FREQ
    );

    if (bd.init() != MBED_SUCCESS) {
        tr_error("KV Config: SPIFBlockDevice init fail");
        return NULL;
    }

    if (start_address == 0 && size == 0) {
        return &bd;
    }

    //If address and size were specified use SlicingBlockDevice to get the correct block device size and start address.
    if (_get_addresses(&bd, start_address, size, &aligned_start_address, &aligned_end_address) != 0) {
        tr_error("KV Config: Fail to get addresses for SlicingBlockDevice.");
        return NULL;
    }

    static SlicingBlockDevice sbd(&bd, aligned_start_address, aligned_end_address);
    return &sbd;

#else
    return NULL;
#endif
}

BlockDevice *_get_blockdevice_QSPIF(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_QSPIF

    bd_addr_t aligned_end_address;
    bd_addr_t aligned_start_address;

    static QSPIFBlockDevice bd(
        QSPI_FLASH1_IO0,
        QSPI_FLASH1_IO1,
        QSPI_FLASH1_IO2,
        QSPI_FLASH1_IO3,
        QSPI_FLASH1_SCK,
        QSPI_FLASH1_CSN,
        QSPIF_POLARITY_MODE_0,
        MBED_CONF_QSPIF_QSPI_FREQ
    );

    if (bd.init() != MBED_SUCCESS) {
        tr_error("KV Config: QSPIFBlockDevice init fail");
        return NULL;
    }

    if (start_address == 0 && size == 0) {
        return &bd;
    }

    //If address and size were specified use SlicingBlockDevice to get the correct block device size and start address.
    if (_get_addresses(&bd, start_address, size, &aligned_start_address, &aligned_end_address) != 0) {
        tr_error("KV Config: Fail to get addresses for SlicingBlockDevice.");
        return NULL;
    }

    static SlicingBlockDevice sbd(&bd, aligned_start_address, aligned_end_address);
    return &sbd;

#else
    return NULL;
#endif
}

BlockDevice *_get_blockdevice_DATAFLASH(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_DATAFLASH

    bd_addr_t aligned_end_address;
    bd_addr_t aligned_start_address;

    static DataFlashBlockDevice bd(
        MBED_CONF_DATAFLASH_SPI_MOSI,
        MBED_CONF_DATAFLASH_SPI_MISO,
        MBED_CONF_DATAFLASH_SPI_CLK,
        MBED_CONF_DATAFLASH_SPI_CS
    );

    if (bd.init() != MBED_SUCCESS) {
        tr_error("KV Config: DataFlashBlockDevice init fail");
        return NULL;
    }

    if (start_address == 0 && size == 0) {
        return &bd;
    }

    //If address and size were specified use SlicingBlockDevice to get the correct block device size and start address.
    if (_get_addresses(&bd, start_address, size, &aligned_start_address, &aligned_end_address) != 0) {
        tr_error("KV Config: Fail to get addresses for SlicingBlockDevice.");
        return NULL;
    }

    static SlicingBlockDevice sbd(&bd, aligned_start_address, aligned_end_address);
    return &sbd;


#else
    return NULL;
#endif
}

BlockDevice *_get_blockdevice_SD(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_SD

    bd_addr_t aligned_end_address;
    bd_addr_t aligned_start_address;

    static SDBlockDevice bd(
        MBED_CONF_SD_SPI_MOSI,
        MBED_CONF_SD_SPI_MISO,
        MBED_CONF_SD_SPI_CLK,
        MBED_CONF_SD_SPI_CS
    );

    if (bd.init() != MBED_SUCCESS) {
        tr_error("KV Config: SDBlockDevice init fail");
        return NULL;
    }

    if (strcmp(STR(MBED_CONF_STORAGE_STORAGE_TYPE), "TDB_EXTERNAL_NO_RBP") == 0 ||
            strcmp(STR(MBED_CONF_STORAGE_STORAGE_TYPE), "TDB_EXTERNAL") == 0) {
        //In TDBStore profile, we have a constraint of 4GByte
        if (start_address == 0 && size == 0  && bd.size() < (uint32_t)(-1)) {
            return &bd;
        }

        //If the size of external storage is bigger than 4G we need to slice it.
        size = size != 0 ? size : align_down(bd.size(), bd.get_erase_size(bd.size() - 1));

        if (_get_addresses(&bd, start_address, size, &aligned_start_address, &aligned_end_address) != 0) {
            tr_error("KV Config: Fail to get addresses for SlicingBlockDevice.");
            return NULL;
        }

        if (aligned_end_address - aligned_start_address != (uint32_t)(aligned_end_address - aligned_start_address)) {
            aligned_end_address = aligned_start_address + (uint32_t)(-1);//Support up to 4G only
        }
    } else {
        //For all other KVStore profiles beside TDBStore we take the entire external memory space.
        if (start_address == 0 && size == 0) {
            return &bd;
        }

        if (_get_addresses(&bd, start_address, size, &aligned_start_address, &aligned_end_address) != 0) {
            tr_error("KV Config: Fail to get addresses for SlicingBlockDevice.");
            return NULL;
        }
    }

    aligned_end_address = align_down(aligned_end_address, bd.get_erase_size(aligned_end_address));
    static SlicingBlockDevice sbd(&bd, aligned_start_address, aligned_end_address);
    return &sbd;

#else
    return NULL;
#endif
}

BlockDevice *_get_blockdevice_default(bd_addr_t start_address, bd_size_t size)
{
#if COMPONENT_QSPIF
    return _get_blockdevice_QSPIF(start_address, size);
#elif COMPONENT_SPIF
    return _get_blockdevice_SPIF(start_address, size);
#elif COMPONENT_DATAFLASH
    return _get_blockdevice_DATAFLASH(start_address, size);
#elif COMPONENT_SD
    return _get_blockdevice_SD(start_address, size);
#else
    tr_error("KV Config: No default component define in target.json for this target.");
    return NULL;
#endif
}

int _storage_config_TDB_INTERNAL()
{
#if COMPONENT_FLASHIAP
    bd_size_t internal_size = MBED_CONF_STORAGE_TDB_INTERNAL_INTERNAL_SIZE;
    bd_addr_t internal_start_address = MBED_CONF_STORAGE_TDB_INTERNAL_INTERNAL_BASE_ADDRESS;

    if (internal_size == 0 && internal_start_address == 0) {
        //Calculate the block device size and start address in case default values are used.
        if (_get_flashiap_bd_default_addresses_tdb_internal(&internal_start_address, &internal_size) != MBED_SUCCESS) {
            return MBED_ERROR_FAILED_OPERATION;
        }
    }

    //Get internal memory FLASHIAP block device.
    kvstore_config.internal_bd = GET_BLOCKDEVICE(INTERNAL_BLOCKDEVICE_NAME, internal_start_address, internal_size);
    if (kvstore_config.internal_bd == NULL) {
        tr_error("KV Config: Fail to get internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION;
    }


    int ret = kvstore_config.internal_bd->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION;
    }

    //Check that internal flash has 2 or more sectors
    if (_calculate_blocksize_match_tdbstore(kvstore_config.internal_bd) != MBED_SUCCESS) {
        tr_error("KV Config: Can not create TDBStore with less then 2 sector.");
        return MBED_ERROR_INVALID_ARGUMENT;
    }

    //Deinitialize internal block device and TDB will reinitialize and take control on it.
    ret = kvstore_config.internal_bd->deinit();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to deinit internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION;
    }

    //Create a TDBStore in the internal FLASHIAP block device.
    static TDBStore tdb_internal(kvstore_config.internal_bd);
    kvstore_config.internal_store = &tdb_internal;

    ret = kvstore_config.internal_store->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal TDBStore.");
        return ret;
    }
    kvstore_config.kvstore_main_instance =
        kvstore_config.internal_store;

    //Masking flag - Actually used to remove any KVStore flag which is not supported
    //in the chosen KVStore profile.
    kvstore_config.flags_mask = ~(KVStore::REQUIRE_CONFIDENTIALITY_FLAG |
                                  KVStore::REQUIRE_REPLAY_PROTECTION_FLAG);

    //Initialize kv_map and add the configuration struct to KVStore map.
    KVMap &kv_map = KVMap::get_instance();
    ret = kv_map.init();
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to init KVStore global API.");
        return ret;
    }

    ret = kv_map.attach(STR(MBED_CONF_STORAGE_DEFAULT_KV), &kvstore_config);
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to attach KVStore main instance to KVStore global API.");
        return ret;
    }
    return MBED_SUCCESS;
#else
    return MBED_ERROR_UNSUPPORTED;
#endif

}

int _storage_config_TDB_EXTERNAL()
{
#if !SECURESTORE_ENABLED
    return MBED_ERROR_UNSUPPORTED;
#endif

    bd_size_t internal_rbp_size = MBED_CONF_STORAGE_TDB_EXTERNAL_RBP_INTERNAL_SIZE;
    bd_addr_t internal_start_address = MBED_CONF_STORAGE_TDB_EXTERNAL_INTERNAL_BASE_ADDRESS;

    //Get the default address and size for internal rbp TDBStore
    if (internal_rbp_size == 0 && internal_start_address == 0) {
        //Calculate the block device size and start address in case default values are used.
        if (_get_flashiap_bd_default_addresses_rbp(&internal_start_address, &internal_rbp_size) != MBED_SUCCESS) {
            return MBED_ERROR_FAILED_OPERATION;
        }
    }

    //Create internal FLASHIAP block device
    kvstore_config.internal_bd = GET_BLOCKDEVICE(INTERNAL_BLOCKDEVICE_NAME, internal_start_address, internal_rbp_size);
    if (kvstore_config.internal_bd == NULL) {
        tr_error("KV Config: Fail to get internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    int ret = kvstore_config.internal_bd->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Check if TDBStore has at least 2 sector.
    if (_calculate_blocksize_match_tdbstore(kvstore_config.internal_bd) != MBED_SUCCESS) {
        tr_error("KV Config: Can not create TDBStore with less then 2 sector.");
        return MBED_ERROR_INVALID_ARGUMENT;
    }

    //Create internal TDBStore
    static TDBStore tdb_internal(kvstore_config.internal_bd);
    kvstore_config.internal_store = &tdb_internal;

    ret = kvstore_config.internal_store->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal TDBStore.");
        return ret;
    }

    bd_size_t size = MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_SIZE;
    bd_addr_t address = MBED_CONF_STORAGE_TDB_EXTERNAL_EXTERNAL_BASE_ADDRESS;

    //Get external BlockDevice for TDBStore
    BlockDevice *bd = GET_BLOCKDEVICE(MBED_CONF_STORAGE_TDB_EXTERNAL_BLOCKDEVICE, address, size);
    if (bd == NULL) {
        tr_error("KV Config: Fail to get external BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //TDBStore needs a block device base on flash. so if this is SD block device or the default block device is SD
    //add FlashSimBlockDevice on top of the SDBlockDevice
#if defined(COMPONENT_SD)
    if (strcmp(STR(MBED_CONF_STORAGE_TDB_EXTERNAL_BLOCKDEVICE), "SD") == 0
#if defined(COMPONENT_SD) &&  !defined(COMPONENT_SPIF) && !defined(COMPONENT_QSPIF) && !defined(COMPONENT_DATAFLASH)
            ||  strcmp(STR(MBED_CONF_STORAGE_TDB_EXTERNAL_BLOCKDEVICE), "default") == 0) {
#else
       ) {

#endif
        //TDBStore need FlashSimBlockDevice when working with SD block device
        if (bd->init() != MBED_SUCCESS) {
            tr_error("KV Config: Fail to init external BlockDevice.");
            return MBED_ERROR_FAILED_OPERATION ;
        }

        static FlashSimBlockDevice flash_bd(bd);
        kvstore_config.external_bd = &flash_bd;
    } else {
        kvstore_config.external_bd = bd;
    }
#else
    kvstore_config.external_bd = bd;
#endif

    kvstore_config.flags_mask = ~(0);

    return _storage_config_tdb_external_common();
}

int _storage_config_TDB_EXTERNAL_NO_RBP()
{
#if !SECURESTORE_ENABLED
    return MBED_ERROR_UNSUPPORTED;
#endif
    bd_size_t size = MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_SIZE;
    bd_addr_t address = MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_EXTERNAL_BASE_ADDRESS;

    //Get external block device
    BlockDevice *bd = GET_BLOCKDEVICE(MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_BLOCKDEVICE, address, size);
    if (bd == NULL) {
        tr_error("KV Config: Fail to get external BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //TDBStore needs a block device base on flash. so if this is SD block device or the default block device is SD
    //add FlashSimBlockDevice on top of the SDBlockDevice
#if defined(COMPONENT_SD)
    if (strcmp(STR(MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_BLOCKDEVICE), "SD") == 0
#if defined(COMPONENT_SD) &&  !defined(COMPONENT_SPIF) && !defined(COMPONENT_QSPIF) && !defined(COMPONENT_DATAFLASH)
            ||  strcmp(STR(MBED_CONF_STORAGE_TDB_EXTERNAL_NO_RBP_BLOCKDEVICE), "default") == 0) {
#else
       ) {

#endif
        //TDBStore need FlashSimBlockDevice when working with SD block device
        if (bd->init() != MBED_SUCCESS) {
            tr_error("KV Config: Fail to init external BlockDevice.");
            return MBED_ERROR_FAILED_OPERATION ;
        }

        static FlashSimBlockDevice flash_bd(bd);
        kvstore_config.external_bd = &flash_bd;
    } else {
        kvstore_config.external_bd = bd;
    }
#else
    kvstore_config.external_bd = bd;
#endif

    //Masking flag - Actually used to remove any KVStore flag which is not supported
    //in the chosen KVStore profile.
    kvstore_config.flags_mask = ~(KVStore::REQUIRE_REPLAY_PROTECTION_FLAG);

    return _storage_config_tdb_external_common();
}

int _storage_config_tdb_external_common()
{
#if SECURESTORE_ENABLED
    //Initialize external block device
    int ret = kvstore_config.external_bd->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init external BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Check that there is at least 2 sector for the external TDBStore
    if (_calculate_blocksize_match_tdbstore(kvstore_config.external_bd) != MBED_SUCCESS) {
        tr_error("KV Config: Can not create TDBStore with less then 2 sector.");
        return MBED_ERROR_INVALID_SIZE;
    }

    //Create external TDBStore
    static TDBStore tdb_external(kvstore_config.external_bd);
    kvstore_config.external_store = &tdb_external;

    //Create SecureStore and initialize it
    static SecureStore secst(kvstore_config.external_store, kvstore_config.internal_store);

    ret = secst.init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init SecureStore.");
        return ret ;
    }

    kvstore_config.kvstore_main_instance = &secst;

    //Init kv_map and add the configuration struct to KVStore map.
    KVMap &kv_map = KVMap::get_instance();
    ret = kv_map.init();
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to init KVStore global API");
        return ret;
    }

    ret = kv_map.attach(STR(MBED_CONF_STORAGE_DEFAULT_KV), &kvstore_config);
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to attach KvStore main instance to KVStore global API");
        return ret;
    }

    return MBED_SUCCESS;
#else
    return MBED_ERROR_UNSUPPORTED;
#endif
}

int _storage_config_FILESYSTEM()
{
#if !SECURESTORE_ENABLED
    return MBED_ERROR_UNSUPPORTED;
#endif

    filesystemstore_folder_path = STR(MBED_CONF_STORAGE_FILESYSTEM_FOLDER_PATH);

    bd_size_t internal_rbp_size = MBED_CONF_STORAGE_FILESYSTEM_RBP_INTERNAL_SIZE;
    bd_addr_t internal_start_address = MBED_CONF_STORAGE_FILESYSTEM_INTERNAL_BASE_ADDRESS;

    //Get the default address and size for internal rbp TDBStore
    if (internal_rbp_size == 0 && internal_start_address == 0) {
        //Calculate the block device size and start address in case default values are used.
        if (_get_flashiap_bd_default_addresses_rbp(&internal_start_address, &internal_rbp_size) != MBED_SUCCESS) {
            return MBED_ERROR_FAILED_OPERATION;
        }
    }

    //Get internal FLASHIAP block device
    kvstore_config.internal_bd = GET_BLOCKDEVICE(INTERNAL_BLOCKDEVICE_NAME, internal_start_address, internal_rbp_size);
    if (kvstore_config.internal_bd == NULL) {
        tr_error("KV Config: Fail to get internal BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    int ret = kvstore_config.internal_bd->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Check that internal flash has 2 or more sectors
    if (_calculate_blocksize_match_tdbstore(kvstore_config.internal_bd) != MBED_SUCCESS) {
        tr_error("KV Config: Can not create TDBStore with less then 2 sector.");
        return MBED_ERROR_INVALID_ARGUMENT;
    }

    //Deinitialize internal block device and TDB will reinitialize and take control on it.
    ret = kvstore_config.internal_bd->deinit();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to deinit internal BlockDevice.");
        return MBED_ERROR_FAILED_OPERATION;
    }

    //Create internal TDBStore for rbp
    static TDBStore tdb_internal(kvstore_config.internal_bd);
    kvstore_config.internal_store = &tdb_internal;

    ret = kvstore_config.internal_store->init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init internal TDBStore");
        return ret;
    }

    bd_size_t size = MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_SIZE;
    bd_addr_t address = MBED_CONF_STORAGE_FILESYSTEM_EXTERNAL_BASE_ADDRESS;
    const char *mount_point = STR(MBED_CONF_STORAGE_FILESYSTEM_MOUNT_POINT);

    //Get external block device for FileSystem.
    kvstore_config.external_bd = GET_BLOCKDEVICE(MBED_CONF_STORAGE_FILESYSTEM_BLOCKDEVICE, address, size);
    if (kvstore_config.external_bd == NULL) {
        tr_error("KV Config: Fail to get external BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    ret = kvstore_config.external_bd->init();
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to init external BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Get FileSystem. Can be FAT, LITTLE or default. in case of default, the type will be decided base on the default
    //component block device configured in the system. The priority is:
    //QSPI -> SPI -> DATAFLASH == LITTLE
    //SD == FAT
    kvstore_config.external_fs = GET_FILESYSTEM(MBED_CONF_STORAGE_FILESYSTEM_FILESYSTEM, mount_point);
    if (kvstore_config.external_fs == NULL) {
        tr_error("KV Config: Fail to get FileSystem");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    kvstore_config.flags_mask = ~(0);

    return _storage_config_filesystem_common();
}

int _storage_config_FILESYSTEM_NO_RBP()
{
#if !SECURESTORE_ENABLED
    return MBED_ERROR_UNSUPPORTED;
#endif

    filesystemstore_folder_path = STR(MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FOLDER_PATH);

    bd_size_t size = MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_SIZE;
    bd_addr_t address = MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_EXTERNAL_BASE_ADDRESS;
    const char *mount_point = STR(MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_MOUNT_POINT);

    //Get external block device for FileSystem.
    kvstore_config.external_bd = GET_BLOCKDEVICE(MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_BLOCKDEVICE, address, size);
    if (kvstore_config.external_bd == NULL) {
        tr_error("KV Config: Fail to get external BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    int ret = kvstore_config.external_bd->init();
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to init external BlockDevice ");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Get FileSystem. Can be FAT, LITTLE or default. in case of default, the type will be decided base on the default
    //component block device configured in the system. The priority is:
    //QSPI -> SPI -> DATAFLASH == LITTLE
    //SD == FAT
    kvstore_config.external_fs = GET_FILESYSTEM(MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_FILESYSTEM, mount_point);
    if (kvstore_config.external_fs == NULL) {
        tr_error("KV Config: Fail to get FileSystem");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Masking flag - Actually used to remove any KVStore flag which is not supported
    //in the chosen KVStore profile.
    kvstore_config.flags_mask = ~(KVStore::REQUIRE_REPLAY_PROTECTION_FLAG);

    return _storage_config_filesystem_common();
}

int _storage_config_filesystem_common()
{
#if SECURESTORE_ENABLED

    //Mount file system. if it fails, try to reformat
    int ret = kvstore_config.external_fs->mount(kvstore_config.external_bd);
    if (ret != MBED_SUCCESS) {
        ret = kvstore_config.external_fs->reformat(kvstore_config.external_bd);
        if (ret != MBED_SUCCESS) {
            tr_error("KV Config: Fail to mount FileSystem to %s",
                     STR(MBED_CONF_STORAGE_FILESYSTEM_NO_RBP_MOUNT_POINT));
            return MBED_ERROR_FAILED_OPERATION ;
        }
    }

    //Create FileSystemStore
    kvstore_config.external_store = _get_file_system_store(kvstore_config.external_fs);
    if (kvstore_config.external_store == NULL) {
        tr_error("KV Config: Fail to get FileSystemStore");
        return MBED_ERROR_FAILED_OPERATION ;
    }

    //Create SecureStore and set it as main KVStore
    static SecureStore secst(kvstore_config.external_store, kvstore_config.internal_store);

    ret = secst.init();
    if (ret != MBED_SUCCESS) {
        tr_error("KV Config: Fail to init SecureStore.");
        return ret; ;
    }

    kvstore_config.kvstore_main_instance = &secst;

    //Init kv_map and add the configuration struct to KVStore map.
    KVMap &kv_map = KVMap::get_instance();
    ret = kv_map.init();
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to init KVStore global API");
        return ret;
    }

    ret = kv_map.attach(STR(MBED_CONF_STORAGE_DEFAULT_KV), &kvstore_config);
    if (MBED_SUCCESS != ret) {
        tr_error("KV Config: Fail to attach KvStore main instance to KVStore global API");
        return ret;
    }

    return MBED_SUCCESS;
#else
    return MBED_ERROR_UNSUPPORTED;
#endif
}

int _storage_config_default()
{
#if COMPONENT_QSPIF || COMPONENT_SPIF || COMPONENT_DATAFLASH
    return _storage_config_TDB_EXTERNAL();
#elif COMPONENT_SD
    return _storage_config_FILESYSTEM();
#elif COMPONENT_FLASHIAP
    return _storage_config_TDB_INTERNAL();
#else
    return MBED_ERROR_UNSUPPORTED;
#endif
}

const char *get_filesystemstore_folder_path()
{
    return filesystemstore_folder_path;
}

MBED_WEAK int kv_init_storage_config()
{

    int ret = MBED_SUCCESS;

    // We currently have no supported configuration without internal storage
#ifndef COMPONENT_FLASHIAP
    return MBED_ERROR_UNSUPPORTED;
#endif

    mutex->lock();

    if (is_kv_config_initialize) {
        goto exit;
    }

    memset(&kvstore_config, 0, sizeof(kvstore_config_t));

    ret = _STORAGE_CONFIG(MBED_CONF_STORAGE_STORAGE_TYPE);

    if (ret == MBED_SUCCESS) {
        is_kv_config_initialize = true;
    }

exit:
    mutex->unlock();
    return ret;
}
