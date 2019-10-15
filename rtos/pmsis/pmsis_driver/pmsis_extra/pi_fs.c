#if 0
/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#include "stddef.h"
#include "string.h"
#include "pmsis.h"
#include "pmsis/device.h"
#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"
#include "pmsis/extra/pi_fs.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis_driver/hyper/hyperbus_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Data
 *****************************************************************************/

static uint8_t cache[FS_READ_THRESHOLD_BLOCK_FULL];

static struct pi_device flash_dev;
static struct pi_hyper_conf conf_flash;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static int32_t __pi_fs_read_from_cache(fs_file_t *file, uint32_t addr,
                                    uint32_t size, uint32_t buffer);
static void __pi_fs_read_block(fs_file_t *file, uint32_t addr,
                               uint32_t size, uint32_t buffer);
static int32_t __pi_fs_read_cached(fs_file_t *file, uint32_t addr,
                                uint32_t size, uint32_t buffer);
static int32_t __pi_fs_read(fs_file_t *file, uint32_t addr,
                         uint32_t size, uint32_t buffer);
static void __pi_fs_try_read(fs_file_t *file);

/*******************************************************************************
 * APIs definition
 ******************************************************************************/

void pi_fs_config_default (fs_config_t *fs_config)
{
    fs_config->fs_type = PI_FS_READ_ONLY;
}

int32_t pi_fs_mount(fs_handle_t *fs, uint8_t device, fs_config_t *fs_config)
{
    if ((fs_config == NULL) || (fs == NULL))
    {
        return PI_FS_MOUNT_ERROR;
    }

    fs->fs_size = 0;
    fs->fs_l2_offset = 0;
    fs->fs_l2_size = 0;
    fs->fs_info = NULL;
    fs->fs_cache_addr = 0;
    fs->fs_cache = cache;

    fs->fs_device = device;

    //uint32_t fs_size = 0;
    uint32_t fs_offset = 0;
    switch (fs->fs_device)
    {
    case fs_HYPER :
        /* Initialize device. */
        pi_hyper_conf_init(&conf_flash);
        conf_flash.id = 0;
        conf_flash.type = PI_HYPER_TYPE_FLASH;
        /* Hyperflash conf init. */
        pi_open_from_conf(&flash_dev, &conf_flash);
        if (pi_hyper_open(&flash_dev))
        {
            DEBUG_PRINTF("Hyperflash open failed !\n");
            return PI_FS_FLASH_ERROR;
        }

        /* Get offset of filesystem header */
        pi_hyper_flash_read(&flash_dev, 0x00, &(fs->fs_l2_offset), 8);

        /* Get header size. */
        pi_hyper_flash_read(&flash_dev, fs->fs_l2_offset, &(fs->fs_l2_size), 8);
        fs->fs_size = ((fs->fs_l2_size + 7) & ~0x07);
        fs_offset = fs->fs_l2_offset;
        fs->fs_info = (uint32_t *) pmsis_l2_malloc(fs->fs_size);
        if (fs->fs_info == NULL)
        {
            return PI_FS_MOUNT_ERROR;
        }
        pi_hyper_flash_read(&flash_dev, fs_offset + 8, fs->fs_info, fs->fs_size);

        return PI_FS_OK;
        break;
    default:
        return PI_FS_MOUNT_ERROR;
    }
    return PI_FS_MOUNT_ERROR;
}

void pi_fs_unmount(fs_handle_t *fs)
{
    if (fs != NULL)
    {
        switch (fs->fs_device)
        {
        case fs_HYPER :
            pmsis_l2_malloc_free(fs->fs_info, fs->fs_size);
            pi_hyper_close(&flash_dev);
        default :
            break;
        }
    }
}

fs_file_t *pi_fs_open(fs_handle_t *fs_handle, const char *file_name, uint8_t mode)
{
    uint32_t *fs_info = fs_handle->fs_info;
    uint32_t nb = *fs_info++;

    fs_desc_t *desc = NULL;
    uint32_t i = 0;

    for (i=0; i<nb; i++)
    {
        desc = (fs_desc_t *) fs_info;

        if (strcmp(desc->name, file_name) == 0)
        {
            fs_file_t *file = (fs_file_t *) pmsis_l2_malloc(sizeof(fs_file_t));
            if (file == NULL)
            {
                return NULL;
            }
            file->offset = 0;
            file->size = desc->size;
            file->addr = desc->addr;
            file->fs = fs_handle;
            return file;
        }
        fs_info = (uint32_t *)((uint32_t) fs_info + sizeof(fs_desc_t) + desc->path_size);
    }
    /* No file found. */
    return NULL;
}

void pi_fs_close(fs_file_t *file)
{
    if (file != NULL)
    {
        pmsis_l2_malloc_free(file, sizeof(fs_file_t));
    }
}

int32_t pi_fs_read(fs_file_t *file, uint32_t size, void *buffer)
{
    if (file == NULL)
    {
        return IO_FILE_ERROR;
    }
    if (buffer == NULL)
    {
        return IO_READ_ERROR;
    }

    uint32_t readBytes = size;
    if (file->offset + size > file->size)
    {
        readBytes = file->size - file->offset;
    }
    file->pending_buffer = (uint32_t) buffer;
    file->pending_size = readBytes;
    file->pending_addr = file->addr + file->offset;
    file->offset += readBytes;
    __pi_fs_try_read(file);
    return readBytes;
}

int32_t pi_fs_seek(fs_file_t *file, int32_t offset)
{
    if (file == NULL)
    {
        return IO_FILE_ERROR;
    }
    if ((offset < 0) || (offset > (int32_t) file->size))
    {
        return IO_SEEK_ERROR;
    }
    file->offset = offset;
    return IO_OK;
}

/*******************************************************************************
 * Inner functions
 ******************************************************************************/

static void __pi_fs_try_read(fs_file_t *file)
{
    while (file->pending_size)
    {
        uint32_t size = __pi_fs_read(file, file->pending_addr,
                                     file->pending_size, file->pending_buffer);
        file->pending_addr += size;
        file->pending_buffer += size;
        file->pending_size -= size;
    }
}

static int32_t __pi_fs_read(fs_file_t *file, uint32_t addr,
                     uint32_t size, uint32_t buffer)
{
    /* Access without cache. */
    if ((size <= FS_READ_THRESHOLD) || ((addr & 0x3) != (buffer & 0x3)))
    {
        return __pi_fs_read_cached(file, addr, size, buffer);
    }

    /* Access to cache because of a HIT. */
    if ((size <= FS_READ_THRESHOLD_BLOCK_FULL) &&
        (addr >= file->fs->fs_cache_addr) &&
        ((addr + size) < (file->fs->fs_cache_addr +  FS_READ_THRESHOLD_BLOCK_FULL)))
    {
        return __pi_fs_read_from_cache(file, addr, size, buffer);
    }

    /* Otherwise, transfer to cache, then read from cache. */
    uint32_t prefix_size = addr & 0x03;
    uint32_t read_size = 0;
    if (prefix_size)
    {
        prefix_size = 4 - prefix_size;
        read_size = __pi_fs_read_cached(file, addr, size, buffer);
        addr += prefix_size;
        buffer += prefix_size;
        size -= prefix_size;
    }

    uint32_t block_size = size & ~0x03;
    __pi_fs_read_block(file, addr, block_size, buffer);

    return block_size;
}

static int32_t __pi_fs_read_cached(fs_file_t *file, uint32_t addr,
                                uint32_t size, uint32_t buffer)
{
    if (size > (FS_READ_THRESHOLD_BLOCK_FULL - (addr & 0x03)))
    {
        size = FS_READ_THRESHOLD_BLOCK_FULL - (addr & 0x03);
    }

    if ((addr < file->fs->fs_cache_addr) ||
        ((addr + size) > (file->fs->fs_cache_addr + FS_READ_THRESHOLD_BLOCK_FULL)))
    {
        file->fs->fs_cache_addr = addr & ~0x03;
        __pi_fs_read_block(file, file->fs->fs_cache_addr, FS_READ_THRESHOLD_BLOCK_FULL,
                           (uint32_t) file->fs->fs_cache);
        return 0;
    }

    return __pi_fs_read_from_cache(file, addr, size, buffer);
}


static void __pi_fs_read_block(fs_file_t *file, uint32_t addr,
                               uint32_t size, uint32_t buffer)
{
    switch (file->fs->fs_device)
    {
    case fs_HYPER :
        pi_hyper_read(&flash_dev, addr, (void *) buffer, size);
        break;
    default:
        return;
    }
}

static int32_t __pi_fs_read_from_cache(fs_file_t *file, uint32_t addr, uint32_t size, uint32_t buffer)
{
    memcpy((void *) buffer, &file->fs->fs_cache[addr - file->fs->fs_cache_addr], size);
    return size;
}
#endif
