/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __RT_RT_HYPERRAM_H__
#define __RT_RT_HYPERRAM_H__




/**
* @ingroup groupDrivers
*/



/**
 * @defgroup HyperRAM HyperRAM
 *
 * The HyperRAM driver provides support for transferring data between an external HyperRAM chip
 * and the processor running this driver.
 *
 * The driver provides a memory allocator for allocating and freeing memory in the HyperRAM chip
 * and an asynchronous API for transferring data.
 *
 */

/**
 * @addtogroup HyperRAM
 * @{
 */

/**@{*/

/** \struct rt_hyperram_conf_t
 * \brief HyperRAM configuration structure.
 *
 * This structure is used to pass the desired HyperRAM configuration to the runtime when opening the device.
 */
typedef struct {
  signed char id;         /*!< If it is different from -1, this specifies on which hyperbus interface the device is connected. */
  signed int ram_size;   /*!< Size of the ram. */
  unsigned int padding [16];
} rt_hyperram_conf_t;

/** \brief HyperRAM copy structure.
 *
 * This structure is used by the runtime to manage a copy with the HyperRAM.
 * It must be instantiated once for each copy and must be kept alive until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef rt_periph_copy_t rt_hyperram_copy_t;



/** \brief HyperRAM request structure.
 *
 * This structure is used by the runtime to manage a cluster remote copy with the HyperRAM.
 * It must be instantiated once for each copy and must be kept alive until the copy is finished.
 * It can be instantiated as a normal variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_hyper_req_s rt_hyperram_req_t;



/** \brief Initialize an HyperRAM configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the camera is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the HyperRAM configuration.
 */
void rt_hyperram_conf_init(rt_hyperram_conf_t *conf);



/** \brief Open an HyperRAM device.
 *
 * This function must be called before the HyperRAM device can be used. It will do all the needed configuration to make it
 * usable and also return a handle used to refer to this opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 * Can only be called from fabric-controller side.
 *
 * \param dev_name  The device name. This name should correspond to the one used to configure the devices managed by the runtime.
 * \param conf      A pointer to the HyperRAM configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device and which can be used with other functions
 */
rt_hyperram_t *rt_hyperram_open(char *dev_name, rt_hyperram_conf_t *conf, rt_event_t *event);



/** \brief Close an opened HyperRAM device.
 *
 * This function can be called to close an opened HyperRAM device once it is not needed anymore, in order to free
 * all allocated resources. Once this function is called, the device is not accessible anymore and must be opened
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 * Can only be called from fabric-controller side.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
void rt_hyperram_close(rt_hyperram_t *handle, rt_event_t *event);





/** \brief Enqueue a read copy to the HyperRAM (from HyperRAM to processor).
 *
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * An event can be specified in order to be notified when the transfer is finished.
 * Can only be called from fabric-controller side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param event       The event used to notify the end of transfer. See the documentation of rt_event_t for more details.
 */
static inline void rt_hyperram_read(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_event_t *event);



/** \brief Enqueue a write copy to the HyperRAM (from processor to HyperRAM).
 *
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * An event can be specified in order to be notified when the transfer is finished.
 * Can only be called from fabric-controller side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param event       The event used to notify the end of transfer. See the documentation of rt_event_t for more details.
 */
static inline void rt_hyperram_write(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_event_t *event);



/** \brief Enqueue a 2D read copy (rectangle area) to the HyperRAM (from HyperRAM to processor).
 *
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * An event can be specified in order to be notified when the transfer is finished.
 * Can only be called from fabric-controller side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit in 16 bits, i.e. must be less than 65536.
 * \param length      2D length, which is the number of transfered bytes after which the driver will switch to the next line. Must fit in 16 bits, i.e. must be less than 65536.
 * \param event       The event used to notify the end of transfer. See the documentation of rt_event_t for more details.
 */
static inline void rt_hyperram_read_2d(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event);



/** \brief Enqueue a 2D write copy (rectangle area) to the HyperRAM (from processor to HyperRAM).
 *
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * An event can be specified in order to be notified when the transfer is finished.
 * Can only be called from fabric-controller side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit in 16 bits, i.e. must be less than 65536.
 * \param length      2D length, which is the number of transfered bytes after which the driver will switch to the next line. Must fit in 16 bits, i.e. must be less than 65536.
 * \param event       The event used to notify the end of transfer. See the documentation of rt_event_t for more details.
 */
static inline void rt_hyperram_write_2d(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event);



/** \brief Allocate HyperRAM memory
 *
 * The allocated memory is 4-bytes aligned. The allocator uses some meta-data stored in the fabric controller memory
 * for every allocation so it is advisable to do as few allocations as possible to lower the memory overhead.
 * Can only be called from fabric-controller side.
 *
 * \param dev    The device descriptor of the HyperRAM chip for which the memory must be allocated
 * \param size   The size in bytes of the memory to allocate
 * \return NULL if not enough memory was available, otherwise the address of the allocated chunk
 */
static inline void *rt_hyperram_alloc(rt_hyperram_t *dev, int size);

/** \brief Free HyperRAM memory
 *
 * The allocator does not store any information about the allocated chunks, thus the size of the allocated
 * chunk to to be freed must be provided by the caller.
 * Can only be called from fabric-controller side.
 *
 * \param dev    The device descriptor of the HyperRAM chip for which the memory must be freed
 * \param chunk  The allocated chunk to free
 * \param size   The size in bytes of the memory chunk which was allocated
 * \return 0 if the operation was successful, -1 otherwise
 */
static inline int rt_hyperram_free(rt_hyperram_t *dev, void *chunk, int size);



/** \brief Enqueue a read copy to the HyperRAM from cluster side (from HyperRAM to processor).
 *
 * This function is a remote call that the cluster can do to the fabric-controller in order to ask
 * for an HyperRam read copy.
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can properly do the remote call.
 * Can only be called from cluster side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param req         A pointer to the HyperRam request structure. It must be allocated by the caller and kept alive until the copy is finished.
 */
static inline void rt_hyperram_cluster_read(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_hyperram_req_t *req);



/** \brief Enqueue a write copy to the HyperRAM from cluster side (from HyperRAM to processor).
 *
 * This function is a remote call that the cluster can do to the fabric-controller in order to ask
 * for an HyperRam write copy.
 * The copy will make an asynchronous transfer between the HyperRAM and one of the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can properly do the remote call.
 * Can only be called from cluster side.
 *
 * \param dev         The device descriptor of the HyperRAM chip on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param hyper_addr  The address of the copy in the HyperRAM.
 * \param size        The size in bytes of the copy
 * \param req         A pointer to the HyperRam request structure. It must be allocated by the caller and kept alive until the copy is finished.
 */
static inline void rt_hyperram_cluster_write(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_hyperram_req_t *req);



/** \brief Wait until the specified hyperram request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void rt_hyperram_cluster_wait(rt_hyperram_req_t *req);

/** \brief Allocate HyperRAM memory from cluster
 *
 * The allocated memory is 4-bytes aligned. The allocator uses some meta-data stored in the fabric controller memory
 * for every allocation so it is advisable to do as few allocations as possible to lower the memory overhead.
 *
 * \param dev    The device descriptor of the HyperRAM chip for which the memory must be allocated
 * \param size   The size in bytes of the memory to allocate
 * \param req    The request structure used for termination.
 */
static inline void rt_hyperram_alloc_cluster(rt_hyperram_t *dev, int size, rt_hyperram_alloc_req_t *req);

/** \brief Free HyperRAM memory from cluster
 *
 * The allocator does not store any information about the allocated chunks, thus the size of the allocated
 * chunk to to be freed must be provided by the caller.
 * Can only be called from fabric-controller side.
 *
 * \param dev    The device descriptor of the HyperRAM chip for which the memory must be freed
 * \param chunk  The allocated chunk to free
 * \param size   The size in bytes of the memory chunk which was allocated
 * \param req    The request structure used for termination.
 */
static inline void rt_hyperram_free_cluster(rt_hyperram_t *dev, void *chunk, int size, rt_hyperram_free_req_t *req);

/** \brief Wait until the specified hyperram alloc request has finished.
 *
 * This blocks the calling core until the specified cluster hyperram allocation is finished.
 *
 * \param req       The request structure used for termination.
 * \return NULL     if not enough memory was available, otherwise the address of the allocated chunk
 */
static inline void *rt_hyperram_alloc_cluster_wait(rt_hyperram_alloc_req_t *req);

/** \brief Wait until the specified hyperram free request has finished.
 *
 * This blocks the calling core until the specified cluster hyperram free is finished.
 *
 * \param req       The request structure used for termination.
 * \return 0        if the operation was successful, -1 otherwise
 */
static inline void rt_hyperram_free_cluster_wait(rt_hyperram_free_req_t *req);

//!@}

/**
 * @} end of HyperRAM
 */



/// @cond IMPLEM

#include "pmsis/drivers/hyperbus.h"
#include "rt/implem/hyperram.h"

#if defined(ARCHI_UDMA_HAS_HYPER)


void __rt_hyper_copy(int channel,
  void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr);

void __rt_hyper_copy_2d(int channel,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event, int mbr);


static inline void rt_hyperram_read(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_event_t *event)
{
  if (event)
    pi_hyper_read_async((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, event);
  else
    pi_hyper_read((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size);
}


static inline void rt_hyperram_read_2d(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event)
{
  if (event)
    pi_hyper_read_2d_async((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, stride, length, event);
  else
    pi_hyper_read_2d((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, stride, length);
}


static inline void rt_hyperram_write(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_event_t *event)
{
  if (event)
    pi_hyper_write_async((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, event);
  else
    pi_hyper_write((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size);
}


static inline void rt_hyperram_write_2d(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event)
{
  if (event)
    pi_hyper_write_2d_async((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, stride, length, event);
  else
    pi_hyper_write_2d((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, stride, length);
}

static inline void *rt_hyperram_alloc(rt_hyperram_t *dev, int size)
{
  return (void *)pi_hyperram_alloc((struct pi_device *)dev, size);
}

static inline int rt_hyperram_free(rt_hyperram_t *dev, void *chunk, int size)
{
  return pi_hyperram_free((struct pi_device *)dev, (uint32_t)chunk, size);
}

static inline void rt_hyperflash_copy(rt_hyperflash_t *dev, int channel,
   void *addr, void *_hyper_addr, int size, rt_event_t *event)
{

  uint32_t hyper_addr = REG_MBR1 | (unsigned int)_hyper_addr;

  if (channel & 1)
    if (event)
      pi_hyper_write_async(&dev->device, (uint32_t)hyper_addr, addr, size, event);
    else
      pi_hyper_write(&dev->device, (uint32_t)hyper_addr, addr, size);
  else
    if (event)
      pi_hyper_read_async(&dev->device, (uint32_t)hyper_addr, addr, size, event);
    else
      pi_hyper_read(&dev->device, (uint32_t)hyper_addr, addr, size);
}

#if defined(ARCHI_HAS_CLUSTER)

static inline void rt_hyperram_alloc_cluster(rt_hyperram_t *dev, int size, rt_hyperram_alloc_req_t *req)
{
  pi_cl_hyperram_alloc((struct pi_device *)dev, size, req);;
}

static inline void rt_hyperram_free_cluster(rt_hyperram_t *dev, void *chunk, int size, rt_hyperram_free_req_t *req)
{
  pi_cl_hyperram_free((struct pi_device *)dev, (uint32_t)chunk, size, req);;
}

static inline void *rt_hyperram_alloc_cluster_wait(rt_hyperram_alloc_req_t *req)
{
	while((*(volatile char *)&req->done) == 0)
	{
		eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
	}
	return (void *)req->result;
}

static inline void rt_hyperram_free_cluster_wait(rt_hyperram_free_req_t *req)
{
	while((*(volatile char *)&req->done) == 0)
	{
		eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
	}
}

static inline __attribute__((always_inline)) void rt_hyperram_cluster_wait(rt_hyperram_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

static inline void __rt_hyperram_cluster_copy(rt_hyperram_t *dev,
  void *hyper_addr, void *addr, int size, rt_hyperram_req_t *req, int ext2loc)
{
  __cl_hyper_cluster_copy((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, req, ext2loc);
}

static inline void __rt_hyperram_cluster_copy_2d(rt_hyperram_t *dev,
  void *hyper_addr, void *addr, int size, int stride, int length, rt_hyperram_req_t *req, int ext2loc)
{
  __cl_hyper_cluster_copy_2d((struct pi_device *)dev, (uint32_t)hyper_addr, addr, size, stride, length, req, ext2loc);
}

static inline void rt_hyperram_cluster_read(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_hyperram_req_t *req)
{
  __rt_hyperram_cluster_copy(dev, hyper_addr, addr, size, req, 1);
}

static inline void rt_hyperram_cluster_write(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_hyperram_req_t *req)
{
  __rt_hyperram_cluster_copy(dev, hyper_addr, addr, size, req, 0);
}

#endif

#endif


/// @endcond

#endif
