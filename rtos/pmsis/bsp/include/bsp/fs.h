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

#ifndef __FS__FS__H__
#define __FS__FS__H__

#include "stdbool.h"

#include "pmsis.h"

/// @cond IMPLEM

typedef struct __pi_fs_api_t pi_fs_api_t;

/// @endcond

/**
 * @defgroup FS File-System
 *
 * The file-system driver provides support for accessing files on a flash. The
 * following file-systems are available:
 *  - Read-only file system. This file-system is very basic but quite-convenient
 *    to have access to input data. The open operation does not scale well when
 *    having lots of file so this file-system should only be used with few
 *    files.
 *
 */

/**
 * @addtogroup FS
 * @{
 */

/**@{*/



/** \enum pi_fs_type_e
 * \brief File-system type.
 *
 * This can be used to select the type of file-system to mount.
 */
typedef enum {
  PI_FS_READ_ONLY     = 0,    /*!< Read-only file system. */
  PI_FS_HOST          = 1,     /*!< Host file system. */
  PI_FS_LFS           = 2,     /*!< LittleFS Filesystem. */
} pi_fs_type_e;



/** \enum pi_fs_flags_e
 * \brief File-system open flags.
 *
 * This can be used to select the type of file-system to mount.
 */
typedef enum {
  PI_FS_FLAGS_READ     = 0,    /*!< File is opened for reading. */
  PI_FS_FLAGS_WRITE    = 1,     /*!< File is opened for writing. */
  PI_FS_FLAGS_APPEND   = 2    /*!< File is opened for appending (writing at
    end of file). */
} pi_fs_flags_e;



/** \struct pi_fs_conf
 * \brief File-system configuration structure.
 *
 * This structure is used to pass the desired file-system configuration to the
 * runtime when mounting the file-system.
 */
struct pi_fs_conf {
  pi_fs_type_e type;        /*!< File-system type. */
  struct pi_device *flash;  /*!< Flash device. The flash device must be first
    opened and its device structure passed here. */
  char *partition_name; /*!< useful if there are several partitions of this FS type.
    By default this field is set to null, which allows to find the first partition compatible with this type of FS. */
  bool auto_format;     /*!< Defined the behavior of the mount operation in case the file system could not be found in the partition.
    if auto_format is set to false, An error is returned .
    In the opposite case, if auto_format is set to true, the partition will be formated and ready to use.
    Not available in ReadFS.  */
  pi_fs_api_t *api;    /*!< Pointer to specific FS methods. Reserved for
    internal runtime usage. */
};

/** \brief FS file structure.
 *
 * This structure is used by the runtime to store information about a file.
 */
typedef struct pi_fs_file_s pi_fs_file_t;

/** \brief FS cluster file request structure.
 *
 * This structure is used by the runtime to manage a cluster remote operation
 * with the FS. It must be instantiated once for each operation and must be kept
 * alive until the operation is finished. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_fs_req_s pi_cl_fs_req_t;

/** \brief Initialize a file-system configuration with default values.
 *
 * The structure containing the configuration must be kept allocated until
 * the file-system is mounted.
 *
 * \param conf A pointer to the file-system configuration.
 */
void pi_fs_conf_init(struct pi_fs_conf *conf);

/** \brief Mount a file-system.
 *
 * This function must be called before the file-system device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int32_t pi_fs_mount(struct pi_device *device);

/** \brief Unmount a mounted file-system.
 *
 * This function can be called to close a mounted file-system once it is not
 * needed anymore, in order to free all allocated resources. Once this function
 * is called, the file-system is not accessible anymore and must be mounted
 * again before being used.
 *
 * \param device    The device structure of the FS to unmount.
 */
void pi_fs_unmount(struct pi_device *device);

/** \brief Open a file.
 *
 * This function can be called to open a file on a file-system in order to read
 * or write data to it.
 *
 * \param device    The device structure of the FS where to open the file.
 * \param file      The path to the file to be opened.
 * \param flags     Optional flags to configure how the file is opened.
 * \return          NULL if the file is not found, or a handle identifying the
 *   file which can be used with other functions.
 */
pi_fs_file_t *pi_fs_open(struct pi_device *device, const char *file, int flags);

/** \brief Close a file.
 *
 * This function can be called to close an opened file once it is not needed
 * anymore in order to free the allocated resources.
 *
 * \param file      The handle of the file to be closed.
 */
void pi_fs_close(pi_fs_file_t *file);

/** \brief Read data from a file.
 *
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * The caller is blocked until the transfer is finished.
 * Compared to pi_fs_direct_read, this functions can use a cache
 * to optimize small transfers.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \return          The number of bytes actually read from the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size);

/** \brief Write data to a file.
 *
 * This function can be called to write data to an opened file. The data is
 * written to the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes written by the call to this function.
 * This functionmay not be supported by each file-system.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to write data.
 * \param buffer    The memory location where the data to be written must be
 *                  read.
 * \param size      The size in bytes to write to the file.
 * \return          The number of bytes actually written to the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size);

/** \brief Read data from a file with no intermediate cache.
 *
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * The caller is blocked until the transfer is finished.
 * Compared to pi_fs_read, this function does direct read transfers from the
 * flash without any cache.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \return          The number of bytes actually read from the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size);

/** \brief Reposition the current file position.
 *
 * This function can be called to change the current position of a file.
 * Note that this does not affect pending copies, but only the ones which will
 * be enqueued after this call.
 *
 * \param file      The handle of the file for which the current position is
 *   changed.
 * \param offset    The offset where to set the current position. The offset
 *   can be between 0 for the beginning of the file and the file size.
 * \return          0 if the operation was successful, -1 otherwise.
 */
int32_t pi_fs_seek(pi_fs_file_t *file, unsigned int offset);

/** \brief Copy data between a FS file and a chip memory.
 *
 * This function can be called to transfer data between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \return          0 if the operation was successful, -1 otherwise.
 */
int32_t pi_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, int32_t ext2loc);

/** \brief Enqueue a 2D copy (rectangle area) between a FS file and a chip
 * memory.
 *
 * This function can be called to transfer data between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \return          0 if the operation was successful, -1 otherwise.
 */
int32_t pi_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc);

/** \brief Read data from a file asynchronously.
 *
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Compared to pi_fs_direct_read, this function can use a cache
 * to optimize small transfers.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 * \return          The number of bytes actually read from the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_read_async(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_task_t *task);

/** \brief Write data to a file asynchronously.
 *
 * This function can be called to write data to an opened file. The data is
 * written to the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes written by the call to this function.
 * This functionmay not be supported by each file-system.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to write data.
 * \param buffer    The memory location where the data to be written must be
 *                  written.
 * \param size      The size in bytes to write to the file.
 * \param task      The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 * \return          The number of bytes actually written to the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_write_async(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_task_t *task);

/** \brief Read data from a file with no intermediate cache asynchronously.
 *
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Compared to pi_fs_read, this function does direct read transfers from the
 * flash without any cache.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param task        The task used to notify the end of transfer.
 * \return          The number of bytes actually read from the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
int32_t pi_fs_direct_read_async(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_task_t *task);

/** \brief Copy data between a FS file and a chip memory asynchronously.
 *
 * This function can be called to transfer data between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \param task        The task used to notify the end of transfer.
 * \return          0 if the operation was successful, -1 otherwise.
 */
int32_t pi_fs_copy_async(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, int32_t ext2loc, pi_task_t *task);

/** \brief Enqueue a 2D copy (rectangle area) between a FS file and a chip
 * memory asynchronously.
 *
 * This function can be called to transfer data between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \param task        The task used to notify the end of transfer.
 * \return          0 if the operation was successful, -1 otherwise.
 */
int32_t pi_fs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc,
  pi_task_t *task);

/** \brief Read data from a file from cluster side.
 *
 * This function implements the same feature as pi_fs_read but can be called
 * from cluster side in order to expose the feature on the cluster.
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination is managed through the
 * request structure.
 * Compared to pi_fs_direct_read, this function can use a cache
 * to optimize small transfers.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 * The only difference compared to pi_cl_fs_read is that the file position is
 * automatically set to 0 for the next transfer if the current transfer reaches
 * the end of the file.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param req       The request structure used for termination.
 */
void pi_cl_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_cl_fs_req_t *req);

/** \brief Write data to a file from cluster side.
 *
 * This function implements the same feature as pi_fs_write but can be called
 * from cluster side in order to expose the feature on the cluster.
 * This function can be called to write data to an opened file. The data is
 * written to the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes written by the call to this function.
 * This functionmay not be supported by each file-system.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to write data.
 * \param buffer    The memory location where the data to be written must be
 *                  read.
 * \param size      The size in bytes to write to the file.
 * \param req       The request structure used for termination.
 * \return          The number of bytes actually written to the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
void pi_cl_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_cl_fs_req_t *req);

/** \brief Read data from a file with no intermediate cache from cluster side.
 *
 * This function implements the same feature as pi_fs_direct_read but can be
 * called from cluster side in order to expose the feature on the cluster.
 * This function can be called to read data from an opened file. The data is
 * read from the current position which is the beginning of the file when the
 * file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination is managed through the
 * request structure.
 * Compared to pi_cl_fs_read, this function does direct read transfers from the
 * flash without any cache.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param req       The request structure used for termination.
 */
void pi_cl_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_cl_fs_req_t *req);

/** \brief Reposition the current file position from cluster side.
 *
 * This function can be called from cluster side to change the current position
 * of a file.
 * Note that this does not affect pending copies, but only the ones which will
 * be enqueued after this call.
 * This operation is asynchronous and its termination is managed through the
 * request structure.
 *
 * \param file      The handle of the file for which the current position is
 *   changed.
 * \param offset    The offset where to set the current position. The offset
 *   can be between 0 for the beginning of the file and the file size.
 * \param req       The request structure used for termination.
 */
void pi_cl_fs_seek(pi_fs_file_t *file, uint32_t offset,
  pi_cl_fs_req_t *req);

/** \brief Copy data between a FS file and a chip memory from cluster side.
 *
 * This function is a remote call that the cluster can do to transfer data
 * between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * This operation is asynchronous and its termination is managed through the
 * request structure.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \param req       The request structure used for termination.
 */
void pi_cl_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, int32_t ext2loc, pi_cl_fs_req_t *req);

/** \brief Enqueue a 2D copy (rectangle area) between a FS file and a chip
 * memory from cluster side.
 *
 * This function is a remote call that the cluster can do to transfer data
 * between an opened file and a
 * chip memory using a specified offset instead of a current position.
 * This operation is asynchronous and its termination is managed through the
 * request structure.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param file      The handle of the file where to read data.
 * \param index     The offset in the file where to start accessing data.
 * \param buffer    The memory location in the chip memory where the data is
 *   accessed.
 * \param size      The size in bytes to transfer.
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc   1 if the copy is from file to the chip or 0 for the
 *   contrary.
 * \param req       The request structure used for termination.
 */
void pi_cl_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer,
  uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc,
  pi_cl_fs_req_t *req);

/** \brief Wait until the specified fs request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 * As the remote copy is asynchronous, this also gives the number of bytes
 * which was read.
 *
 * \param req       The request structure used for termination.
 * \return          The number of bytes actually read from the file. This can
 *   be smaller than the requested size if the end of file is reached.
 */
static inline int32_t pi_cl_fs_wait(pi_cl_fs_req_t *req);

//!@}

/**
 * @} end of FS
 */


/// @cond IMPLEM

struct __pi_fs_api_t {
    int32_t (*mount)(struct pi_device *device);
    void (*unmount)(struct pi_device *device);
    pi_fs_file_t *(*open)(struct pi_device *device, const char *file, int flags);
    void (*close)(pi_fs_file_t *file);
    int32_t (*read)(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task);
    int32_t (*direct_read)(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task);
    int32_t (*write)(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task);
    int32_t (*seek)(pi_fs_file_t *file, unsigned int offset);
    int32_t (*copy)(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task);
    int32_t (*copy_2d)(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_task_t *task);
};

extern pi_fs_api_t __pi_read_fs_api;
extern pi_fs_api_t __pi_host_fs_api;
extern pi_fs_api_t pi_lfs_api;

typedef struct {
  pi_cl_fs_req_t *cluster_reqs_first;
  pi_cl_fs_req_t *cluster_reqs_last;
  pi_task_t cl_req_task;
} pi_fs_data_t;

typedef struct pi_fs_file_s {
  struct pi_device *fs;
  pi_fs_api_t *api;
  void *data;
  unsigned int size;
  pi_fs_data_t *fs_data;
} pi_fs_file_t;

typedef enum {
  FS_MOUNT_FLASH_ERROR     = 1,     /*!< There was an error mounting the flash filesystem. */
  FS_MOUNT_MEM_ERROR       = 2      /*!< There was an error allocating memory when mounting the file-system. */
} pi_fs_error_e;



typedef struct pi_cl_fs_req_s
{
  pi_fs_file_t *file;
  pi_callback_t callback;
  union {
    struct {
      uint8_t done;
      char result;
      unsigned char cid;
      unsigned char direct;
      unsigned char write;
      void *buffer;
      uint32_t size;
      uint32_t offset;
    } rw;
    struct {
      uint8_t done;
      char result;
      unsigned char cid;
      unsigned char ext2loc;
      void *buffer;
      uint32_t size;
      uint32_t index;
      uint32_t stride;
      uint32_t length;
    } copy;
  };
} pi_cl_fs_req_t;

static inline __attribute__((always_inline)) int32_t pi_cl_fs_wait(pi_cl_fs_req_t *req)
{
    cl_wait_task(&(req->rw.done));
    return req->rw.result;
}

/// @endcond

#endif
