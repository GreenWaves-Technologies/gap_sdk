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

#ifndef __BSP_RAM_H__
#define __BSP_RAM_H__

#include "pmsis.h"

/// @cond IMPLEM

typedef struct __pi_ram_api_t pi_ram_api_t;

/// @endcond

/**
 * @defgroup Ram Ram
 *
 * The RAM driver provides support for transferring data between an
 * external RAM chip (e.g. Hyperram or SPI ram) and the processor running this
 * driver.
 *
 */

/**
 * @addtogroup Ram
 * @{
 */

/**@{*/

/** \struct pi_ram_conf
 * \brief RAM configuration structure.
 *
 * This structure is used to pass the desired RAM configuration to the
 * runtime when opening the device. This configuration should be not be used
 * directly as it is meant to be encapsulated into a specific device
 * configuration.
 */
struct pi_ram_conf {
    pi_ram_api_t *api;   /*!< Pointer to specific RAM methods. Reserved for 
    internal runtime usage. */
};

/** \brief RAM cluster copy request structure.
 *
 * This structure is used by the runtime to manage a cluster remote copy with
 * the RAM. It must be instantiated once for each copy and must be kept
 * alive until the copy is finished. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_ram_req_s pi_cl_ram_req_t;

/** \brief RAM cluster alloc request structure.
 *
 * This structure is used by the runtime to manage a cluster remote allocation
 * in the RAM. It must be instantiated once for each allocation and must be kept
 * alive until the allocation is done. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_ram_alloc_req_s pi_cl_ram_alloc_req_t;

/** \brief RAM cluster free request structure.
 *
 * This structure is used by the runtime to manage a cluster remote free
 * in the RAM. It must be instantiated once for each free and must be kept
 * alive until the free is done. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_ram_free_req_s pi_cl_ram_free_req_t;

/** \brief Open a RAM device.
 *
 * This function must be called before the RAM device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * The configuration associated to the device must specify the exact model of
 * RAM which must be opened.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int32_t pi_ram_open(struct pi_device *device);

/** \brief Close an opened RAM device.
 *
 * This function can be called to close an opened RAM device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
static inline void pi_ram_close(struct pi_device *device);

/** \brief Allocate RAM memory
 *
 * The allocated memory is 4-bytes aligned. The allocator uses some meta-data
 * stored in the chip memory for every allocation so it is advisable to do as
 * few allocations as possible to lower the memory overhead.
 *
 * \param device    The device structure of the device where to allocate the
 *   memory.
 * \param addr     A pointer to the variable where the allocated address
 *   must be returned.
 * \param size      The size in bytes of the memory to allocate
 * \return          0 if the allocation succeeded, -1 if not enough memory was
 *   available.
 */
static inline int pi_ram_alloc(struct pi_device *device, uint32_t *addr,
  uint32_t size);

/** \brief Free RAM memory
 *
 * The allocator does not store any information about the allocated chunks,
 * thus the size of the allocated chunk to to be freed must be provided by the
 * caller.
 *
 * \param device The device structure of the device where to free the
 *   memory.
 * \param addr  The allocated chunk to free
 * \param size   The size in bytes of the memory chunk which was allocated
 * \return       0 if the operation was successful, -1 otherwise
 */
static inline int pi_ram_free(struct pi_device *device, uint32_t addr,
  uint32_t size);

/** \brief Enqueue a read copy to the RAM (from RAM to processor).
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_ram_read(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size);

/** \brief Enqueue a write copy to the RAM (from processor to RAM).
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 */
static inline void pi_ram_write(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size);

/** \brief Enqueue a copy with the RAM.
 *
 * The copy will make a transfer between the RAM and one of
 * the processor memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 */
static inline void pi_ram_copy(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, int ext2loc);

/** \brief Enqueue a 2D read copy (rectangle area) to the RAM (from
 * RAM to processor).
 *
 * The copy will make a transfer between the RAM and one of
 * the processor memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 */
static inline void pi_ram_read_2d(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, uint32_t stride, uint32_t length);

/** \brief Enqueue a 2D write copy (rectangle area) to the RAM (from
 * processor to RAM).
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 * the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 */
static inline void pi_ram_write_2d(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, uint32_t stride, uint32_t length);

/** \brief Enqueue a 2D copy (rectangle area) with the RAM.
 *
 * The copy will make a transfer between the RAM and one of the processor
 * memory areas.
 * The calller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 * the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 */
static inline void pi_ram_copy_2d(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc);

/** \brief Enqueue an asynchronous read copy to the RAM (from RAM
 * to processor).
 *
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_ram_read_async(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, pi_task_t *task);

/** \brief Enqueue an asynchronous write copy to the RAM (from processor
 * to RAM).
 *
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_ram_write_async(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, pi_task_t *task);

/** \brief Enqueue an asynchronous copy with the RAM.
 *
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_ram_copy_async(struct pi_device *device, uint32_t pi_ram_addr,
  void *data, uint32_t size, int ext2loc, pi_task_t *task);

/** \brief Enqueue an asynchronous 2D read copy (rectangle area) to the
 * RAM (from RAM to processor).
 *
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 * the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param task        The task used to notify the end of transfer. See the
 * documentation of pi_task_t for more details.
 */
static inline void pi_ram_read_2d_async(struct pi_device *device,
  uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_task_t *task);

/** \brief Enqueue an asynchronous 2D write copy (rectangle area) to the
 * RAM (from processor to RAM).
 *
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_ram_write_2d_async(struct pi_device *device,
  uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_task_t *task);

/** \brief Enqueue an asynchronous 2D copy (rectangle area) with the RAM.
 *
 * The copy will make an asynchronous transfer between the RAM and one of the
 * processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 * the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param data        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
static inline void pi_ram_copy_2d_async(struct pi_device *device,
  uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, int ext2loc, pi_task_t *task);

/** \brief Allocate RAM memory from cluster side.
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM allocation.
 * The allocated memory is 4-bytes aligned. The allocator uses some meta-data
 * stored in the fabric controller memory for every allocation so it is
 * advisable to do as few allocations as possible to lower the memory overhead.
 *
 * \param device  The device descriptor of the RAM chip for which the memory
 *   must be allocated.
 * \param size   The size in bytes of the memory to allocate.
 * \param req    The request structure used for termination.
 */
void pi_cl_ram_alloc(struct pi_device *device, uint32_t size,
  pi_cl_ram_alloc_req_t *req);

/** \brief Free RAM memory from cluster side.
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM allocation.
 * The allocator does not store any information about the allocated chunks,
 * thus the size of the allocated
 * chunk to to be freed must be provided by the caller.
 *
 * \param device   The device descriptor of the RAM chip for which the memory
 *   must be freed.
 * \param chunk  The allocated chunk to free.
 * \param size   The size in bytes of the memory chunk which was allocated.
 * \param req    The request structure used for termination.
 */
void pi_cl_ram_free(struct pi_device *device, uint32_t chunk, uint32_t size,
  pi_cl_ram_free_req_t *req);

/** \brief Wait until the specified RAM alloc request has finished.
 *
 * This blocks the calling core until the specified cluster RAM allocation is
 * finished.
 *
 * \param req       The request structure used for termination.
 * \param addr     A pointer to the variable where the allocated address
 *   must be returned.
 * \return          0 if the allocation succeeded, -1 if not enough memory was
 *   available.
 */
static inline int32_t pi_cl_ram_alloc_wait(pi_cl_ram_alloc_req_t *req,
  uint32_t *addr);

/** \brief Wait until the specified RAM free request has finished.
 *
 * This blocks the calling core until the specified cluster RAM free is
 * finished.
 *
 * \param req       The request structure used for termination.
 * \return 0        if the operation was successful, -1 otherwise
 */
static inline int32_t pi_cl_ram_free_wait(pi_cl_ram_free_req_t *req);

/** \brief Enqueue a read copy to the RAM from cluster side (from RAM
 * to processor).
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM read copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_ram_read(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, pi_cl_ram_req_t *req);

/** \brief Enqueue a write copy to the RAM from cluster side (from
 * RAM to processor).
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM write copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_ram_write(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, pi_cl_ram_req_t *req);

/** \brief Enqueue a copy with the RAM from cluster side.
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
void pi_cl_ram_copy(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, int ext2loc, pi_cl_ram_req_t *req);

/** \brief Enqueue a 2D read copy (rectangle area) to the RAM from cluste
 * side (from RAM to processor).
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM read copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy.
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_ram_read_2d(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_cl_ram_req_t *req);

/** \brief Enqueue a 2D write copy (rectangle area) to the RAM from
 * cluster side (from RAM to processor).
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM write copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_ram_write_2d(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_cl_ram_req_t *req);

/** \brief Enqueue a 2D copy (rectangle area) with the RAM from cluster
 * side.
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for a RAM copy.
 * The copy will make an asynchronous transfer between the RAM and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the RAM chip on which to do
 *   the copy.
 * \param pi_ram_addr  The address of the copy in the RAM.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc     1 if the copy is from RAM to the chip or 0 for the
 *   contrary.
 * \param req         A pointer to the RAM request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
void pi_cl_ram_copy_2d(struct pi_device *device,
  uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, int ext2loc, pi_cl_ram_req_t *req);

/** \brief Wait until the specified RAM request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_ram_read_wait(pi_cl_ram_req_t *req);

/** \brief Wait until the specified RAM request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_ram_write_wait(pi_cl_ram_req_t *req);

/** \brief Wait until the specified RAM request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_ram_copy_wait(pi_cl_ram_req_t *req);

//!@}

/**
 * @} end of Ram
 */


/// @cond IMPLEM

struct pi_cl_ram_req_s
{
    struct pi_device *device;
    void *addr;
    uint32_t pi_ram_addr;
    uint32_t size;
    int32_t stride;
    uint32_t length;
    pi_task_t event;
    struct pi_cl_ram_req_s *next;
    uint8_t done;
    unsigned char cid;
    unsigned char ext2loc;
    unsigned char is_2d;
};

struct pi_cl_ram_alloc_req_s
{
    struct pi_device *device;
    uint32_t result;
    uint32_t  size;
    pi_task_t event;
    uint8_t done;
    char cid;
    char error;
};

struct pi_cl_ram_free_req_s
{
    struct pi_device *device;
    uint32_t result;
    uint32_t size;
    uint32_t chunk;
    pi_task_t event;
    uint8_t done;
    char cid;
    char error;
};

typedef struct __pi_ram_api_t
{
    int (*open)(struct pi_device *device);
    void (*close)(struct pi_device *device);
    void (*copy_async)(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, int ext2loc, pi_task_t *task);
    void (*copy_2d_async)(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task);
    int (*alloc)(struct pi_device *device, uint32_t *addr, uint32_t size);
    int (*free)(struct pi_device *device, uint32_t addr, uint32_t size);
} pi_ram_api_t;


static inline void pi_ram_close(struct pi_device *device)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->close(device);
}

static inline void pi_ram_read_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_async(device, pi_ram_addr, data, size, 1, task);
}

static inline void pi_ram_read_2d_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_2d_async(device, pi_ram_addr, data, size, stride, length, 1, task);
}

static inline void pi_ram_write_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_async(device, pi_ram_addr, data, size, 0, task);
}

static inline void pi_ram_write_2d_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_2d_async(device, pi_ram_addr, data, size, stride, length, 0, task);
}

static inline void pi_ram_copy_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, int ext2loc, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_async(device, pi_ram_addr, data, size, ext2loc, task);
}

static inline void pi_ram_copy_2d_async(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    api->copy_2d_async(device, pi_ram_addr, data, size, stride, length, ext2loc, task);
}

static inline void pi_ram_read(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size)
{
    pi_task_t task;
    pi_ram_read_async(device, pi_ram_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_ram_read_2d(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length)
{
    pi_task_t task;
    pi_ram_read_2d_async(device, pi_ram_addr, data, size, stride, length, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_ram_copy(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, int ext2loc)
{
    pi_task_t task;
    pi_ram_copy_async(device, pi_ram_addr, data, size, ext2loc, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_ram_copy_2d(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc)
{
    pi_task_t task;
    pi_ram_copy_2d_async(device, pi_ram_addr, data, size, stride, length, ext2loc, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_ram_write(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size)
{
    pi_task_t task;
    pi_ram_write_async(device, pi_ram_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline void pi_ram_write_2d(struct pi_device *device, uint32_t pi_ram_addr, void *data, uint32_t size, uint32_t stride, uint32_t length)
{
    pi_task_t task;
    pi_ram_write_2d_async(device, pi_ram_addr, data, size, stride, length, pi_task_block(&task));
    pi_task_wait_on(&task);
}

static inline int pi_ram_alloc(struct pi_device *device, uint32_t *addr, uint32_t size)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    return api->alloc(device, addr, size);
}

static inline int pi_ram_free(struct pi_device *device, uint32_t addr, uint32_t size)
{
    pi_ram_api_t *api = (pi_ram_api_t *)device->api;
    return api->free(device, addr, size);
}

void __pi_ram_conf_init(struct pi_ram_conf *conf);

static inline void pi_cl_ram_read(struct pi_device *device,
                               uint32_t pi_ram_addr, void *addr, uint32_t size, pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy(device, pi_ram_addr, addr, size, 1, req);
}

static inline void pi_cl_ram_read_2d(struct pi_device *device,
                                  uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy_2d(device, pi_ram_addr, addr, size, stride, length, 1, req);
}

static inline void pi_cl_ram_read_wait(pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy_wait(req);
}

static inline void pi_cl_ram_write(struct pi_device *device,
                                uint32_t pi_ram_addr, void *addr, uint32_t size, pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy(device, pi_ram_addr, addr, size, 0, req);
}

static inline void pi_cl_ram_write_2d(struct pi_device *device,
                                   uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy_2d(device, pi_ram_addr, addr, size, stride, length, 0, req);
}

static inline void pi_cl_ram_write_wait(pi_cl_ram_req_t *req)
{
    pi_cl_ram_copy_wait(req);
}

static inline void pi_cl_ram_copy_wait(pi_cl_ram_req_t *req)
{
    cl_wait_task(&(req->done));
}

static inline int32_t pi_cl_ram_alloc_wait(pi_cl_ram_alloc_req_t *req, uint32_t *chunk)
{
    cl_wait_task(&(req->done));

    *chunk = req->result;

    return req->error;
}

static inline int32_t pi_cl_ram_free_wait(pi_cl_ram_free_req_t *req)
{
    cl_wait_task(&(req->done));

    return req->error;
}

/// @endcond




#endif  /* __BSP_RAM_H__ */
