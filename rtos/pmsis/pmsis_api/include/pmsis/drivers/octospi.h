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

#ifndef __PI_PMSIS_DRIVERS_OCTOSPI_H__
#define __PI_PMSIS_DRIVERS_OCTOSPI_H__

#include "pmsis/pmsis_types.h"

#ifndef PI_INLINE_OCTOSPI_LVL_0
#define PI_INLINE_OCTOSPI_LVL_0
#endif

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup Octospi Octospi
 *
 * The Octospi driver provides support for transferring data between an
 * external Octospi chip and the processor running this driver.
 *
 * This is a driver for the Octospi interface. Higher-level drivers can be
 * built on top of this one to target specific devices such as Octospiflash or
 * Octospiram. Please refer to the PMSIS BSP documentation for such drivers.
 *
 */

/**
 * @addtogroup Octospi
 * @{
 */

/**@{*/

/** \enum pi_octospi_type_e
 * \brief Type of the device connected to the octospi interface.
 *
 * This is used to know if the device is a flash or a RAM.
 */
typedef enum
{
    PI_OCTOSPI_TYPE_FLASH,  /*!< Device is an Octospiflash. */
    PI_OCTOSPI_TYPE_RAM     /*!< Device is an Octospiram. */
} pi_octospi_type_e;

/** \enum pi_octospi_flags_e
 * \brief SPI operation flags.
 *
 * This is used to describe some SPI operation flags like command size,
 * address size and so on.
 */
typedef enum
{
    PI_OCTOSPI_FLAG_CMD_SIZE_0=0<<0,    /*!< No command. */
    PI_OCTOSPI_FLAG_CMD_SIZE_1=1<<0,    /*!< Command is 1 byte. */
    PI_OCTOSPI_FLAG_CMD_SIZE_2=2<<0,    /*!< Command is 2 bytes. */
    PI_OCTOSPI_FLAG_ADDR_SIZE_0=0<<4,   /*!< No address. */
    PI_OCTOSPI_FLAG_ADDR_SIZE_1=1<<4,   /*!< Address is 1 byte. */
    PI_OCTOSPI_FLAG_ADDR_SIZE_2=2<<4,   /*!< Address is 2 bytes. */
    PI_OCTOSPI_FLAG_ADDR_SIZE_3=3<<4,   /*!< Address is 3 bytes. */
    PI_OCTOSPI_FLAG_ADDR_SIZE_4=4<<4,   /*!< Address is 4 bytes. */
    PI_OCTOSPI_FLAG_LINE_SINGLE=2<<8,   /*!< Use 1 SPI line. */
    PI_OCTOSPI_FLAG_LINE_QUAD=1<<8,     /*!< Use 4 SPI lines. */
    PI_OCTOSPI_FLAG_LINE_OCTO=0<<8,     /*!< Use 8 SPI lines. */
    PI_OCTOSPI_FLAG_CMD_DTR=0,          /*!< Use DTR mode for command. */
    PI_OCTOSPI_FLAG_CMD_STR=1<<12,      /*!< Use STR mode for command. */
    PI_OCTOSPI_FLAG_ADDR_DTR=0,         /*!< Use DTR mode for address. */
    PI_OCTOSPI_FLAG_ADDR_STR=1<<13,     /*!< Use STR mode for address. */
    PI_OCTOSPI_FLAG_DATA_DTR=0,         /*!< Use DTR mode for data. */
    PI_OCTOSPI_FLAG_DATA_STR=1<<14,     /*!< Use STR mode for data. */
    PI_OCTOSPI_FLAG_DATA_DTR_LSB=0,     /*!< Use LSB in DTR mode for data. */
    PI_OCTOSPI_FLAG_DATA_DTR_MSB=1<<15, /*!< Use MSB in DTR mode for data. */
} pi_octospi_flags_e;

/** \enum pi_octospi_cmd_e
 * \brief SPI operation command flags.
 *
 * This is used to describe some SPI operation flags for the command.
 */
typedef enum
{
    PI_OCTOSPI_CMD_AUTO_RW_BIT_EN=1<<17,        /*!< Automatically generate
      additional R/W bit at MSB on top of the specified SPI command. */
    PI_OCTOSPI_CMD_AUTO_RW_BIT_SET_READ=1<<16,  /*!< When using automatic
      R/W bit generation, this will generate 1 if the operation is a read. */
    PI_OCTOSPI_CMD_ADDR_EVEN=1<<18,             /*!< Always send even addresses
      to the device. */
} pi_octospi_cmd_e;

/** \struct pi_octospi_op_conf
 * \brief SPI operation configuration structure.
 *
 * This structure is used to specify the desired SPI operation configuration.
 * It can be passed either with a transfer or through IOCTL to set it 
 * permanently.
 */
typedef struct pi_octospi_op_conf
{
    uint32_t cmd;      /*!< SPI command. */
    uint32_t latency;  /*!< SPI dummy cycles. */
    uint32_t flags;    /*!< SPI flags (command size, etc). */
} pi_octospi_op_conf_t;

/** \struct pi_octospi_conf
 * \brief Octospi configuration structure.
 *
 * This structure is used to pass the desired Octospi configuration to the
 * runtime when opening the device.
 */
typedef struct pi_octospi_conf
{
    pi_device_e device;  /*!< Interface type. */
    signed char id;      /*!< Octospi interface where the device is connected.
     */
    uint8_t xip_en;      /*!< Specify whether xip is on */
    uint32_t cs;         /*!< Chip select where the device is connected. */
    pi_octospi_type_e type;/*!< Type of device connected on the octospi
    interface. */
    uint32_t baudrate;   /*!< Baudrate (in bytes/second). */
    int32_t burst_length; /*< Maximum burst length in ns. */
}pi_octospi_conf_t;

/** \brief Octospi cluster request structure.
 *
 * This structure is used by the runtime to manage a cluster remote copy with
 * the Octospi. It must be instantiated once for each copy and must be kept
 * alive until the copy is finished. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_octospi_req_s pi_cl_octospi_req_t;

/** \brief Initialize an Octospi configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * device is opened.
 *
 * \param conf A pointer to the Octospi configuration.
 */
void pi_octospi_conf_init(struct pi_octospi_conf *conf);

/** \brief Open an Octospi device.
 *
 * This function must be called before the Octospi device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int32_t pi_octospi_open(struct pi_device *device);

/** \brief Close an opened Octospi device.
 *
 * This function can be called to close an opened Octospi device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_octospi_close(struct pi_device *device);

/** IOCTL command */
enum pi_octospi_ioctl_cmd
{
    /** @brief Configure the SPI operation.
     *
     * This command can be used when the interface has been opened to configure
     * the SPI operation used for the next transfer (latency, SPI command, etc).
     * The argument must be a pointer to a variable of type 
     * pi_octospi_op_conf_t.
     */
    PI_OCTOSPI_IOCTL_SET_OP
};

/**
 * \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after
 * it has been opened or to control it.
 *
 * \param device  A pointer to the structure describing the device.
 * \param cmd      The command which specifies which parameters of the driver
 *   to modify and for some of them also their values. The command must be one
 *   of those defined in pi_spi_ioctl_e.
 * \param arg       An additional value which is required for some parameters
 *   when they are set.
 */
int pi_octospi_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/** \brief Enqueue a read copy to the Octospi (from Octospi to processor).
 *
 * The copy will make a transfer between the Octospi and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_read(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op);

/** \brief Enqueue an asynchronous read copy to the Octospi (from Octospi
 * to processor).
 *
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_read_async(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op, struct pi_task *task);

/** \brief Enqueue a write copy to the Octospi (from processor to Octospi).
 *
 * The copy will make a transfer between the Octospi and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_write(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op);

/** \brief Enqueue an asynchronous write copy to the Octospi (from processor
 * to Octospi).
 *
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_write_async(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op, struct pi_task *task);

/** \brief Enqueue a 2D read copy (rectangle area) to the Octospi (from
 * Octospi to processor).
 *
 * The copy will make a transfer between the Octospi and one of
 * the processor memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_read_2d(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op);

/** \brief Enqueue an asynchronous 2D read copy (rectangle area) to the
 * Octospi (from Octospi to processor).
 *
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 * the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param task        The task used to notify the end of transfer. See the
 * documentation of pi_task_t for more details.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_read_2d_async(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op, struct pi_task *task);

/** \brief Enqueue a 2D write copy (rectangle area) to the Octospi (from
 * processor to Octospi).
 *
 * The copy will make a transfer between the Octospi and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 * the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_write_2d(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op);

/** \brief Enqueue an asynchronous 2D write copy (rectangle area) to the
 * Octospi (from processor to Octospi).
 *
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 */
PI_INLINE_OCTOSPI_LVL_0 void pi_octospi_write_2d_async(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op, struct pi_task *task);

/** \brief Enqueue a read copy to the Octospi from cluster side (from Octospi
 * to processor).
 *
 * This function is a remote call that the cluster can do to the
 * fabric-controller in order to ask for an Octospi read copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_read(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);

/** \brief Enqueue a 2D read copy (rectangle area) to the Octospi from cluster
 * side (from Octospi to processor).
 *
 * This function is a remote call that the cluster can issue to the
 * fabric-controller in order to ask for an Octospi read copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy.
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_read_2d(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);

/** \brief Wait until the specified octospi request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_octospi_read_wait(pi_cl_octospi_req_t *req);

/** \brief Enqueue a write copy to the Octospi from cluster side (from
 * Octospi to processor).
 *
 * This function is a remote call that the cluster can issue to the
 * fabric-controller in order to ask for an Octospi write copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_write(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);

/** \brief Enqueue a 2D write copy (rectangle area) to the Octospi from
 * cluster side (from Octospi to processor).
 *
 * This function is a remote call that the cluster can issue to the
 * fabric-controller in order to ask for an Octospi write copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_write_2d(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);

/** \brief Wait until the specified octospi request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_octospi_write_wait(pi_cl_octospi_req_t *req);

/** \brief Enqueue a copy with the Octospi from cluster side.
 *
 * This function is a remote call that the cluster can issue to the
 * fabric-controller in order to ask for an Octospi copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param ext2loc     1 if the copy is from Octospi to the chip or 0 for the
 *   contrary.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_copy(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, int ext2loc,
  pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);

/** \brief Enqueue a 2D copy (rectangle area) with the Octospi from cluster
 * side.
 *
 * This function is a remote call that the cluster can issue to the
 * fabric-controller in order to ask for an Octospi copy.
 * The copy will make an asynchronous transfer between the Octospi and one of
 * the processor memory areas.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the Octospi chip on which to do
 *   the copy.
 * \param octospi_addr  The address of the copy in the Octospi.
 * \param addr        The address of the copy in the processor.
 * \param size        The size in bytes of the copy
 * \param stride      2D stride, which is the number of bytes which are added
 *   to the beginning of the current line to switch to the next one.
 * \param length      2D length, which is the number of transferred bytes after
 *   which the driver will switch to the next line.
 * \param ext2loc     1 if the copy is from Octospi to the chip or 0 for the
 *   contrary.
 * \param op          The SPI operation configuration. Can be NULL to keep the
 *   latest one which was set.
 * \param req         A pointer to the Octospi request structure. It must be
 *   allocated by the caller and kept alive until the copy is finished.
 */
static inline void pi_cl_octospi_copy_2d(struct pi_device *device,
  uint32_t octospi_addr, void *addr, uint32_t size, uint32_t stride,
  uint32_t length, int ext2loc, pi_octospi_op_conf_t *op, pi_cl_octospi_req_t *req);


//!@}

/**
 * @} end of Octospi
 */

#endif  /* __PI_PMSIS_DRIVERS_OCTOSPI_H__ */
