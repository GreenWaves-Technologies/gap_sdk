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

#ifndef __RT_RT_READ_FS_H__
#define __RT_RT_READ_FS_H__




/**
* @ingroup groupDrivers
*/



/**
 * @defgroup FS File-System
 *
 * The file-system driver provides support for accessing files on a flash. The following file-systems are available:
 *  - Read-only file system. This file-system is very basic but quite-convenient to have access to input data. The open operation does not scale well when having lots of file so this file-system should only be used with few files.
 *
 */

/**
 * @addtogroup FS
 * @{
 */

/**@{*/

/** \enum rt_fs_error_e
 * \brief File-system errors.
 *
 * This is used to report errors related to file-system operations.
 */
typedef enum {
  RT_FS_MOUNT_FLASH_ERROR     = 1,     /*!< There was an error mounting the flash filesystem. */
  RT_FS_MOUNT_MEM_ERROR       = 2      /*!< There was an error allocating memory when mounting the file-system. */
} rt_fs_error_e;



/** \enum rt_fs_type_e
 * \brief File-system type.
 *
 * This can be used to select the type of file-system to mount.
 */
typedef enum {
  RT_FS_READ_ONLY     = 0     /*!< Read-only file system. */
} rt_fs_type_e;



/** \struct rt_fs_conf_t
 * \brief File-system configuration structure.
 *
 * This structure is used to pass the desired file-system configuration to the runtime when mounting the file-system.
 */
typedef struct {
  rt_fs_type_e type;     /*!< File-system type. */
  rt_flash_conf_t flash_conf;        /*!< Flash configuration. */
} rt_fs_conf_t;



/** \brief Initialize a file-system configuration with default values.
 *
 * The structure containing the configuration must be kept allocated until the file-system is mounted.
 * This can only be called on the fabric-controller.
 *
 * \param conf A pointer to the file-system configuration.
 */
void rt_fs_conf_init(rt_fs_conf_t *conf);



/** \brief Mount a file-system.
 *
 * This function must be called before the file-system is used. It will do all necessary configuration
 * and return a handle used to refer to this file-system when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 *
 * \param dev_name  The device name where the file-system must be mounted. This name should correspond to the one used to configure the devices managed by the runtime. The data pointed by this pointer variable must be kept valid until the file-system is mounted.
 * \param conf      A pointer to the file-system configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the file-system and which can be used with other functions.
 */
rt_fs_t *rt_fs_mount(const char *dev_name, rt_fs_conf_t *conf, rt_event_t *event);



/** \brief Unmount a mounted file-system.
 *
 * This function can be called to close a mounted file-system once it is not needed anymore, in order to free
 * all allocated resources. Once this function is called, the file-system is not accessible anymore and must be mounted
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 *
 * \param handle    The handle of the file-system which was returned when the file-system was mounted.
 * \param event     The event used for managing termination.
 */
void rt_fs_unmount(rt_fs_t *handle, rt_event_t *event);



/** \brief Open a file.
 *
 * This function can be called to open a file on a file-system in order to read or write data to it.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 *
 * \param fs        The handle of the file-system which was returned when the file-system was mounted.
 * \param file      The path to the file to be opened.
 * \param flags     Optional flags to configure how the file is opened.
 * \param event     The event used for managing termination.
 * \return          NULL if the file is not found, or a handle identifying the file which can be used with other functions.
 */
rt_file_t *rt_fs_open(rt_fs_t *fs, const char *file, int flags, rt_event_t *event);



/** \brief Close a file.
 *
 * This function can be called to close an opened file once it is not needed anymore in order to free the allocated resources.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 *
 * \param file      The handle of the file to be closed.
 * \param event     The event used for managing termination.
 */
void rt_fs_close(rt_file_t *file, rt_event_t *event);



/** \brief Read data from a file.
 *
 * This function can be called to read data from an opened file. The data is read from the current position which
 * is the beginning of the file when the file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 * Compared to rt_fs_direct_read, this functions can use intermediate transfers to support any alignment constraint
 * from the flash. So it can be slower in case part of the transfer has to be emulated.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param event     The event used for managing termination.
 * \return          The number of bytes actually read from the file. This can be smaller than the requested size if the end of file is reached.
 */
int rt_fs_read(rt_file_t *file, void *buffer, size_t size, rt_event_t *event);



/** \brief Read data from a file with no intermediate cache.
 *
 * This function can be called to read data from an opened file. The data is read from the current position which
 * is the beginning of the file when the file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination can be managed through an event.
 * This can only be called on the fabric-controller.
 * Compared to rt_fs_read, this function does direct read transfers from the flash. So the flash address and the size
 * of the transfer can have some constraints depending on the flash.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param event     The event used for managing termination.
 * \return          The number of bytes actually read from the file. This can be smaller than the requested size if the end of file is reached.
 */
int rt_fs_direct_read(rt_file_t *file, void *buffer, size_t size, rt_event_t *event);



/** \brief Reposition the current file position.
 *
 * This function can be called to change the current position of a file.
 * Note that this does not affect pending copies, but only the ones which will be enqueued after this call.
 * This can only be called on the fabric-controller.
 *
 * \param file      The handle of the file for which the current position is changed.
 * \param offset    The offset where to set the current position. The offset can be between 0 for the beginning of the file and the file size.
 * \return          RT_STATUS_OK if the operation was successful, RT_STATUS_ERR otherwise.
 */
int rt_fs_seek(rt_file_t *file, unsigned int offset);



/** \brief Read data from a file from cluster side.
 *
 * This function implements the same feature as rt_fs_read but can be called from cluster side in order to expose
 * the feature on the cluster.
 * This function can be called to read data from an opened file. The data is read from the current position which
 * is the beginning of the file when the file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination is managed through the request structure.
 * This can only be called on the cluster.
 * Compared to rt_fs_direct_read, this functions can use intermediate transfers to support any alignment constraint
 * from the flash. So it can be slower in case part of the transfer has to be emulated.
 * The only difference compared to rt_fs_read is that the file position is automatically set to 0 for the next
 * transfer if the current transfer reaches the end of the file.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param req       The request structure used for termination.
 */
static inline void rt_fs_cluster_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req);



/** \brief Read data from a file with no intermediate cache from cluster side.
 *
 * This function implements the same feature as rt_fs_direct_read but can be called from cluster side in order to expose
 * the feature on the cluster.
 * This function can be called to read data from an opened file. The data is read from the current position which
 * is the beginning of the file when the file is opened. The current position is incremented by the number of
 * bytes read by the call to this function.
 * This operation is asynchronous and its termination can be managed through an event.
 * Can only be called from cluster side.
 * Compared to rt_fs_direct_read, this function does direct read transfers from the flash. So the flash address and the size
 * of the transfer can have some constraints depending on the flash.
 *
 * \param file      The handle of the file where to read data.
 * \param buffer    The memory location where the read data must be copied.
 * \param size      The size in bytes to read from the file.
 * \param req       The request structure used for termination.
 */
static inline void rt_fs_cluster_direct_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req);



/** \brief Reposition the current file position from cluster side.
 *
 * This function can be called from cluster side to change the current position of a file.
 * Note that this does not affect pending copies, but only the ones which will be enqueued after this call.
 *
 * \param file      The handle of the file for which the current position is changed.
 * \param offset    The offset where to set the current position. The offset can be between 0 for the beginning of the file and the file size.
 * \param req       The request structure used for termination.
 */
static inline void rt_fs_cluster_seek(rt_file_t *file, unsigned int offset, rt_fs_req_t *req);


/** \brief Wait until the specified fs request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is finished.
 * As the remote copy is asynchronous, this also gives the number of bytes which was read.
 * Can only be called from cluster side.
 *
 * \param req       The request structure used for termination.
 * \return          The number of bytes actually read from the file. This can be smaller than the requested size if the end of file is reached.
 *                  Could be also RT_STATUS_OK if the rt_fs_cluster_seek was successful, RT_STATUS_ERR otherwise.
 */
static inline int rt_fs_cluster_wait(rt_fs_req_t *req);


//!@}

/**
 * @} end of FS
 */



/// @cond IMPLEM

char *__rt_fs_error_str(int error);

#if defined(ARCHI_HAS_CLUSTER)

static inline __attribute__((always_inline)) int rt_fs_cluster_wait(rt_fs_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
  return req->result;
}

void __rt_fs_cluster_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req, int direct);
void __rt_fs_cluster_seek(rt_file_t *file, unsigned int offset, rt_fs_req_t *req);

static inline void rt_fs_cluster_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req)
{
  __rt_fs_cluster_read(file, buffer, size, req, 0);
}

static inline void rt_fs_cluster_direct_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req)
{
  __rt_fs_cluster_read(file, buffer, size, req, 1);
}

static inline void rt_fs_cluster_seek(rt_file_t *file, unsigned int offset, rt_fs_req_t *req)
{
  __rt_fs_cluster_seek(file, offset, req);
}

#endif

/// @endcond

#endif
