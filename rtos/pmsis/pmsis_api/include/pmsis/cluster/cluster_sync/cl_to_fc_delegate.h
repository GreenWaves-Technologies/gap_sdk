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

#ifndef __CL_TO_FC_DELEGATE_H__
#define __CL_TO_FC_DELEGATE_H__

#include "pmsis/cluster/cl_pmsis_types.h"
#include "pmsis/rtos/malloc/malloc_internal.h"

/**
 * @addtogroup FcClusterSync
 * @{
 */

/**@{*/

/**
 * \brief Enqueue a task to fabric-controller side.
 *
 * This enqueues the specified task into the fabric-controller task scheduler for
 * execution. The task must have been initialized from fabric-controller
 * side.
 *
 * \param task           Pointer to the fabric-controller task to be enqueued.
 */
void pi_cl_send_task_to_fc(pi_task_t *task);

/**
 * \brief Send a callback to Fabric Controller.
 *
 * This function is used to send a simple callback to FC.
 *
 * \param callback       Pointer to callback(with function and arg).
 *
 * \note This is an alternative to pi_cl_send_task_to_fc().
 */
static inline void pi_cl_send_callback_to_fc(pi_callback_t *callback);

//!@}

/**
 * @}
 */


/// @cond IMPLEM


/** \brief Create an opaque task structure for FC
 * create a task ready to launch on the fc (os dependant implementation)
 *
 * \param task pi_task_t generic form argument (optional)
 */
void *cl_create_pi_task(pi_task_t *task);

/** \brief Create an opaque synchronisation object for cl to wait on
 *
 * \return      return an opaque synchronisation object to wait on (used for async)
 */
// TODO allocated by cluster (on stack)
void cl_create_fc_cl_sync_object(void *sync_object);

/** \brief send an opaque task structure for FC with async call
 *
 * FC will execute task according to the opaque structure
 * (callback, driver access...)
 * \param        opaque argument for the fc
 * \param sync_object opaque sync object to wait on or get signaled by
 */
void cl_send_task_to_fc_async(void *task, void *sync_object);

/** \brief wait on opaque sync object
 *
 * \param sync_object used in async call with FC
 */
void cl_wait_on_sync_object(void *sync_object);

/** \brief deported read pointer access to FC
 * Have FC read a pointer
 * \param pointer address
 */
void *cl_fc_synchronized_pointer_get(uint32_t *pointer);

/** \brief deported write pointer access to FC
 * Have FC write a pointer with value "value"
 * \param pointer address
 * \param value to be assigned
 */
void cl_fc_synchronized_pointer_assign(void *pointer, void *value);

/**
 * \brief obtain device object pointer from FC to execute deported read/write
 * \param device device object pointer pointer
 */
int cl_delegate_device_get(struct pi_device **device);

/**
 * \brief  deported read to a device (read itself is executed by FC)
 * \param device device object pointer
 * \param size size (in bytes) of the access
 * \param loc_address local l1 tcdm address
 * \param device_address address in device if significant, NULL otherwise
 */
int cl_delegate_device_read(struct pi_device *device, uint32_t size, void *loc_address, uint32_t device_address);

/**
 * \brief  deported write to a device (write itself is executed by FC)
 * \param device device object pointer
 * \param size size (in bytes) of the access
 * \param loc_address local l1 tcdm address
 * \param device_address address in device if significant, NULL otherwise
 */
int cl_delegate_device_write(struct pi_device *device, uint32_t size, void *loc_address, uint32_t device_address);

/**
 * \brief deported malloc
 * Allocates a chunk of memory with desired allocator (L1 cluster/ FC tcdm/ L2)
 * \param allocator chosen allocator
 * \param size size to be allocated
 */
void *cl_delegate_malloc(malloc_t *allocator, uint32_t size);

/**
 * \brief deported free
 * Free a chunk of memory allocated with desired allocator (L1 cluster/ FC tcdm/ L2)
 * \param allocator chosen allocator
 * \param ptr pointer to zone to be freed
 * \param size size to be allocated
 */
void cl_delegate_free(malloc_t *allocator, void *ptr, uint32_t size);

/**
 * \brief  deported read to a device (read itself is executed by FC)
 * \param device device object pointer
 * \param size size (in bytes) of the access
 * \param loc_address local l1 tcdm address
 * \param device_address address in device if significant, NULL otherwise
 * \param req opaque sync object to wait on for request completion
 */
int cl_delegate_device_read_async(struct pi_device *device, uint32_t size, void *loc_address, uint32_t device_address, void *req);

/**
 * \brief  deported write to a device (write itself is executed by FC)
 * \param device device object pointer
 * \param size size (in bytes) of the access
 * \param loc_address local l1 tcdm address
 * \param device_address address in device if significant, NULL otherwise
 * \param req opaque sync object to wait on for request completion
 */
int cl_delegate_device_write_async(struct pi_device *device, uint32_t size, void *loc_address, uint32_t device_address, void *req);

/**
 * \brief deported malloc
 * Allocates a chunk of memory with desired allocator (L1 cluster/ FC tcdm/ L2)
 * \param allocator chosen allocator
 * \param size size to be allocated
 * \param req opaque sync object to wait on for request completion
 */
void *cl_delegate_malloc_async(malloc_t *allocator, uint32_t size, void *req);

/**
 * \brief deported free
 * Free a chunk of memory allocated with desired allocator (L1 cluster/ FC tcdm/ L2)
 * \param allocator chosen allocator
 * \param ptr pointer to zone to be freed
 * \param size size to be allocated
 * \param req opaque sync object to wait on for request completion
 */
void cl_delegate_free_async(malloc_t *allocator, void *ptr, uint32_t size, void *req);

/**
 * \brief delegate init
 * Initialize delegate facilities
 * \param arg config arg (os dependant)
 */
void mc_fc_delegate_init(void *arg);

/// @endcond

#endif
