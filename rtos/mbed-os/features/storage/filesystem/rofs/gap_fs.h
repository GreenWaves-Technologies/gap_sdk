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

#ifndef _GAP_RO_FS_H_
#define _GAP_RO_FS_H_

/* GAP specific includes. */
#include "hyperbus_io.h"

/*!
 * @addtogroup filesystem_read_only
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FS_READ_THRESHOLD            16
#define FS_READ_THRESHOLD_BLOCK      128
#define FS_READ_THRESHOLD_BLOCK_FULL (FS_READ_THRESHOLD_BLOCK + 8)

/* Error report. */
typedef enum
{
    GAP_FS_OK = 0,
    GAP_FS_MOUNT_ERROR = 1
} fs_error_e;

/* Type of filesystem. Read only for now. */
typedef enum
{
    GAP_FS_READ_ONLY = 0
} fs_type_e;

/* Filesytem config struct. */
typedef struct _fs_config_s
{
    fs_type_e fs_type;
} fs_config_t;

typedef enum
{
    FS_SPI   = 0,
    FS_HYPER = 1
} fs_device_e;

/* File descriptor structure. */
typedef struct _fs_desc_s
{
    uint32_t addr;
    uint32_t size;
    uint32_t path_size;
    char name[];
} fs_desc_t;

typedef struct _fs_l2_s {
    uint32_t fs_offset;
    uint32_t reserved0;
    uint32_t fs_size;
    uint32_t reserved1;
} fs_l2_t;

/* Filesystem handler type. */
typedef struct _fs_handle_s {
    int fs_size;
    fs_l2_t *fs_l2;
    uint32_t *fs_info;
    uint32_t nb_comps;
    uint8_t *fs_cache;
    uint32_t fs_cache_addr;
    fs_device_e fs_device;
    int error;
} fs_handle_t;

/* File type structure. */
typedef struct _fs_file_s
{
    uint32_t offset;
    uint32_t size;
    uint32_t addr;
    uint32_t pending_addr;
    fs_handle_t *fs;
    uint32_t pending_buffer;
    uint32_t pending_size;
    char name[];
} fs_file_t;

/* IO Errors. */
typedef enum
{
    IO_OK = 0,
    IO_FILE_ERROR = -1,
    IO_READ_ERROR = -2,
    IO_SEEK_ERROR = -3
} fs_IOerror_e;

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {}
#endif /* __cplusplus */

/*!
 * @brief Initialise filesystem config struct with default values.
 *
 * @param Pointer to config struct.
 */
void fs_config_default( fs_config_t *fs_config );

/*!
 * @brief Mount a filesystem.
 * Before any I/O operations on files, this function must be called first.
 *
 * @param device     Type of device used.
 * @param fs_config  Pointer to filesystem configuration.
 *
 * @return A pointer to the mounted filesystem.
 */
fs_handle_t* fs_mount( uint8_t device, fs_config_t *fs_config );

/*!
 * @brief Unmount a filesystem.
 *
 * @param fs Pointer to a handler of a mounted filesystem.
 */
void fs_unmount( fs_handle_t *fs );

/*!
 * @brief Open a file.
 *
 * @param fs        Pointer to a handler of a mounted filesystem.
 * @param file      Path to the file.
 * @param mode      File opening mode.
 */
fs_file_t *fs_open( fs_handle_t *fs, const char *file, uint8_t mode );

/*!
 * @brief Close a file.
 *
 * @param file Pointer to an opened file.
 */
void fs_close( fs_file_t *file );

/*!
 * @brief Read file. The offset is incremented by the nimber of bytes read.
 *
 * @param file    Pointer to an opened file.
 * @param size    Number of bytes to read from file.
 * @param buffer  Buffer to copy data from file.
 *
 * @return Number of bytes read, or error code.
 */
int fs_read( fs_file_t *file, uint32_t size, void *buffer );

/*!
 * @brief Reposition the offset of a file.
 *
 * @param file    Pointer to an opened file.
 * @param offset  Replace the current position of the offset by the new offset.
 *                Between 0 and EOF.
 *
 * @return IO_OK if success else IO_SEEK_OFFSET.
 */
int fs_seek( fs_file_t *file, int offset );

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /*_GAP_RO_FS_H_*/
