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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 11/28/2019.
 */

#include "string.h"

#include "pmsis.h"
#include "pmsis/errno.h"
#include "bsp/partition.h"
#include "bsp/fs/pi_lfs.h"
#include "bsp/flash.h"

typedef struct pi_lfs_t {
    lfs_t lfs;
    struct lfs_config config;
    pi_device_t *flash;
    uint32_t partition_offset;
    size_t partition_size;
    pi_fs_data_t fs_data;
} pi_lfs_t;

pi_fs_api_t pi_lfs_api;

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    pi_lfs_t *pi_lfs = (pi_lfs_t *) c->context;
    
    if(block * c->block_size + off + size > pi_lfs->partition_offset + pi_lfs->partition_size)
        return LFS_ERR_IO;
    
    pi_flash_read(pi_lfs->flash,
                  pi_lfs->partition_offset + block * c->block_size + off,
                  buffer, size);
    return 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    pi_lfs_t *pi_lfs = (pi_lfs_t *) c->context;
    
    if(block * c->block_size + off + size > pi_lfs->partition_offset + pi_lfs->partition_size)
        return LFS_ERR_IO;
    
    pi_flash_program(pi_lfs->flash,
                     pi_lfs->partition_offset + block * c->block_size + off,
                     buffer, size);
    return 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    pi_lfs_t *pi_lfs = (pi_lfs_t *) c->context;
    
    if(block * c->block_size + c->block_size > pi_lfs->partition_offset + pi_lfs->partition_size)
        return LFS_ERR_IO;
    
    pi_flash_erase_sector(pi_lfs->flash,
                          pi_lfs->partition_offset + block * c->block_size);
    return 0;
}

static int lfs_sync(const struct lfs_config *c)
{
    return 0;
}

static void init_lfs_config(struct lfs_config *lfs_config, pi_lfs_t *pi_lfs, const size_t sector_size)
{
    memset(lfs_config, 0, sizeof(struct lfs_config));
    
    /**
     *  Store reference to pi_lfs instance in order to fetch flash device
     *  and offset partition.
     */
    lfs_config->context = pi_lfs;

/*
 * Call-back functions
 */
    lfs_config->read = lfs_read;
    lfs_config->prog = lfs_prog;
    lfs_config->erase = lfs_erase;
    lfs_config->sync = lfs_sync;
    
    /*
     * LittleFS default configuration
     * They can be override by LFS header
     */
    lfs_config->read_size = 4;
    lfs_config->prog_size = 4;
    lfs_config->block_cycles = 100;
    lfs_config->block_size = sector_size;
    lfs_config->block_count = pi_lfs->partition_size / sector_size;
    
    /*
     * Buffers configurations
     */
    lfs_config->cache_size = 1024;
    lfs_config->lookahead_size = 16; // 16 * 8 blocks
}

static int32_t pi_lfs_mount(struct pi_device *device)
{
    pi_err_t rc;
    enum lfs_error lfs_rc;
    struct pi_flash_info flash_info;
    struct pi_fs_conf *fs_conf = (struct pi_fs_conf *) device->config;
    pi_partition_table_t partitionTable = NULL;
    const pi_partition_t *lfs_partition = NULL;
    pi_lfs_t *pi_lfs = NULL;
    
    /*
     * Open LFS partition
     */
    rc = pi_partition_table_load(fs_conf->flash, &partitionTable);
    if(rc != PI_OK)
    {
        goto mount_error;
    }
    
    lfs_partition = pi_partition_find_first(partitionTable, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_LFS,
                                            fs_conf->partition_name);
    if(lfs_partition == NULL)
    {
        rc = PI_ERR_NOT_FOUND;
        goto mount_error;
    }
    
    /*
     * PI LFS allocation and configuration
     */
    pi_lfs = pi_l2_malloc(sizeof(*pi_lfs));
    if(pi_lfs == NULL)
    {
        rc = PI_ERR_L2_NO_MEM;
        goto mount_error;
    }
    
    device->data = pi_lfs;
    
    pi_lfs->flash = fs_conf->flash;
    
    /*
     * To optimize flash access from Little FS,
     * pi_lfs will bypass the partition layer.
     */
    pi_lfs->partition_offset = pi_partition_get_flash_offset(lfs_partition);
    pi_lfs->partition_size = pi_partition_get_size(lfs_partition);
//    printf("%s: LFS flash offset 0x%lx\n", __func__, pi_lfs->partition_offset);
    
    pi_partition_close(lfs_partition);
    lfs_partition = NULL;
    pi_partition_table_free(partitionTable);
    partitionTable = NULL;
    
    // Fetch default sector size from flash
    pi_flash_ioctl(pi_lfs->flash, PI_FLASH_IOCTL_INFO, &flash_info);
//    printf("%s: Flash block size %lx\n", __func__, flash_info.sector_size);
    
    init_lfs_config(&pi_lfs->config, pi_lfs, flash_info.sector_size);
    
    pi_lfs->fs_data.cluster_reqs_first = NULL;

    // Little FS buffers allocation
    pi_lfs->config.read_buffer = pi_l2_malloc(pi_lfs->config.cache_size);
    pi_lfs->config.prog_buffer = pi_l2_malloc(pi_lfs->config.cache_size);
    pi_lfs->config.lookahead_buffer = pi_l2_malloc(pi_lfs->config.lookahead_size);
    if(!(pi_lfs->config.read_buffer
         && pi_lfs->config.prog_buffer
         && pi_lfs->config.lookahead_buffer))
    {
        rc = PI_ERR_L2_NO_MEM;
        goto mount_error;
    }
    
    /*
     * Test magic number
     */
//    uint8_t *data = pi_l2_malloc(16);
//    pi_flash_read(pi_lfs->flash, pi_lfs->partition_offset + 8, data, 8);
//    data[8] = '\0';
//    printf("%s: Magic number `%s'\n", __func__, data);
//    pi_l2_free(data, 16);
    
    // Try to mount Little FS
    lfs_rc = lfs_mount(&pi_lfs->lfs, &pi_lfs->config);
    if(lfs_rc != LFS_ERR_OK)
    {
        if(!fs_conf->auto_format)
        {
            rc = PI_ERR_NOT_FOUND;
            goto mount_error;
        } else
        {
            lfs_rc = lfs_format(&pi_lfs->lfs, &pi_lfs->config);
            if(lfs_rc != LFS_ERR_OK)
            {
                rc = PI_ERR_INVALID_STATE;
                goto mount_error;
            }
        }
    }
    
    return PI_OK;
    
    mount_error:
    if(lfs_partition)
    {
        pi_partition_close(lfs_partition);
    }
    if(partitionTable)
    {
        pi_partition_table_free(partitionTable);
    }
    if(pi_lfs->config.read_buffer)
    {
        pi_l2_free(pi_lfs->config.read_buffer, pi_lfs->config.cache_size);
    }
    if(pi_lfs->config.prog_buffer)
    {
        pi_l2_free(pi_lfs->config.prog_buffer, pi_lfs->config.cache_size);
    }
    if(pi_lfs->config.lookahead_buffer)
    {
        pi_l2_free(pi_lfs->config.lookahead_buffer, pi_lfs->config.lookahead_size);
    }
    if(pi_lfs)
    {
        pi_l2_free(pi_lfs, sizeof(pi_lfs_t));
    }
    return rc;
}

static void pi_lfs_unmount(struct pi_device *device)
{
    pi_lfs_t *pi_lfs = (pi_lfs_t *) device->data;
    
    if(!pi_lfs)
        return;
    
    lfs_unmount(&pi_lfs->lfs);
    
    if(pi_lfs->config.read_buffer)
        pi_l2_free(pi_lfs->config.read_buffer, pi_lfs->config.cache_size);
    if(pi_lfs->config.prog_buffer)
        pi_l2_free(pi_lfs->config.prog_buffer, pi_lfs->config.cache_size);
    if(pi_lfs->config.lookahead_buffer)
        pi_l2_free(pi_lfs->config.lookahead_buffer, pi_lfs->config.lookahead_size);
    pi_l2_free(pi_lfs, sizeof(pi_lfs_t));
    device->data = NULL;
}

static pi_fs_file_t *pi_lfs_open(struct pi_device *device, const char *file, int flags)
{
    enum lfs_error rc = LFS_ERR_OK;
    pi_fs_file_t *pi_file = NULL;
    lfs_file_t *lfs_file = NULL;
    pi_lfs_t *pi_lfs = device->data;
    lfs_t *lfs = &pi_lfs->lfs;
    int lfs_flags = LFS_O_RDONLY;
    
    if (flags == PI_FS_FLAGS_WRITE)
    {
        lfs_flags = LFS_O_CREAT | LFS_O_RDWR;
    }
    
    lfs_file = pi_fc_l1_malloc(sizeof(lfs_file_t));
    if(lfs_file == NULL) return NULL;
    
    rc = lfs_file_open(lfs, lfs_file, file, lfs_flags);
    if(rc != LFS_ERR_OK) goto error;
    
    pi_file = pi_fc_l1_malloc(sizeof(*pi_file));
    if(pi_file == NULL) goto error;
    
    pi_file->data = lfs_file;
    pi_file->fs = device;
    pi_file->api = &pi_lfs_api;
    pi_file->size = lfs_file_size(lfs, lfs_file);
    pi_file->fs_data = &pi_lfs->fs_data;
    
    return pi_file;
    
    error:
    if(pi_file) pi_fc_l1_free(pi_file, sizeof(*pi_file));
    if(lfs_file) pi_fc_l1_free(lfs_file, sizeof(lfs_file_t));
    return NULL;
}

static void pi_lfs_close(pi_fs_file_t *file)
{
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    lfs_file_close(lfs, lfs_file);
    
    pi_fc_l1_free(file, sizeof(pi_fs_file_t));
    pi_fc_l1_free(lfs_file, sizeof(lfs_file_t));
}

static int32_t pi_lfs_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
    size_t rc;
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    rc = lfs_file_read(lfs, lfs_file, buffer, size);
    #if defined(__PULP_OS__)
    task->implem.data[0] = rc;
    #else
    task->data[0] = rc;
    #endif  /* __PULP_OS__ */

    pi_task_push(task);
    return rc;
}

static int32_t pi_lfs_direct_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
    pi_task_push(task);
    return -1;
}

static int32_t pi_lfs_write_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
    size_t rc;
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    rc = lfs_file_write(lfs, lfs_file, buffer, size);
    file->size = lfs_file_size(lfs, lfs_file);
    
    pi_task_push(task);
    
    return rc;
}

static int32_t pi_lfs_seek(pi_fs_file_t *file, unsigned int offset)
{
    int32_t rc;
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    rc = lfs_file_seek(lfs, lfs_file, offset, LFS_SEEK_SET);
    
    return (rc < 0) ? -1 : 0;
}

static int32_t
pi_lfs_copy_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task)
{
    int32_t rc;
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    rc = lfs_file_seek(lfs, lfs_file, index, LFS_SEEK_SET);
    if(rc < 0) return -1;
    
    if(ext2loc)
    {
        rc = lfs_file_read(lfs, lfs_file, buffer, size);
    } else
    {
        rc = lfs_file_write(lfs, lfs_file, buffer, size);
        file->size = lfs_file_size(lfs, lfs_file);
    }
    
    pi_task_push(task);
    
    return (rc < 0) ? -1 : 0;
}

static int32_t
pi_lfs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t  stride, uint32_t length,
                     int32_t ext2loc, pi_task_t *task)
{
    int32_t rc;
    pi_lfs_t *pi_lfs = file->fs->data;
    lfs_t *lfs = &pi_lfs->lfs;
    lfs_file_t *lfs_file = file->data;
    
    rc = lfs_file_seek(lfs, lfs_file, index, LFS_SEEK_SET);
    if(rc < 0) return -1;
    
    while (size > 0)
    {
        size_t loop_length = (size >= length) ? length : size;
        
        if(ext2loc)
        {
            rc = lfs_file_read(lfs, lfs_file, buffer, loop_length);
        } else
        {
            rc = lfs_file_write(lfs, lfs_file, buffer, loop_length);
        }
        
        if(rc < 0) return -1;
        
        index += stride;
        size -= loop_length;
        buffer += length;
        
        rc = lfs_file_seek(lfs, lfs_file, stride - length, LFS_SEEK_CUR);
        if(rc < 0) return -1;
    }
    
    if(!ext2loc)
    {
        file->size = lfs_file_size(lfs, lfs_file);
    }
    
    pi_task_push(task);
    return 0;
}

pi_fs_api_t pi_lfs_api = {
        .mount = pi_lfs_mount,
        .unmount = pi_lfs_unmount,
        .open = pi_lfs_open,
        .close = pi_lfs_close,
        .read = pi_lfs_read_async,
        .direct_read = pi_lfs_direct_read_async,
        .write = pi_lfs_write_async,
        .seek = pi_lfs_seek,
        .copy = pi_lfs_copy_async,
        .copy_2d = pi_lfs_copy_2d_async
};

lfs_t *pi_lfs_get_native_lfs(pi_device_t *device)
{
    pi_lfs_t *pi_lfs;
    
    pi_lfs = (pi_lfs_t *) device->data;
    return &pi_lfs->lfs;
}


void pi_lfs_conf_init(struct pi_lfs_conf *conf)
{
    pi_fs_conf_init(&conf->fs);
    conf->fs.type = PI_FS_LFS;
    conf->fs.api = &pi_lfs_api;
}
