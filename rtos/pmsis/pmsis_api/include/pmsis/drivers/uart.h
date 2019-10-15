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


#ifndef _PMSIS_UART_H_
#define _PMSIS_UART_H_

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup UART UART
 *
 * This API provides support for transferring data between an external UART
 * device and the processor running this driver.
 *
 */

/**
 * @addtogroup UART
 * @{
 */

/**@{*/

/** \struct pi_uart_conf
 * \brief UART device configuration structure.
 *
 * This structure is used to pass the desired UART configuration to the runtime
 * when opening the device.
 */
struct pi_uart_conf
{
    uint32_t src_clock_Hz;
    uint32_t baudrate_bps; /*!< Required baudrate, in baud per second. */
    uint32_t stop_bit_count; /*!< Number of stop bits, 1 stop bit (default) or
      2 stop bits  */
    uint8_t parity_mode; /*!< 1 to activate it, 0 to deactivate it. */
    uint8_t uart_id; /*!< Uart interface number where the device is
      connected. */
    uint8_t enable_rx; /*!< 1 to activate reception, 0 to deactivate it. */
    uint8_t enable_tx; /*!< 1 to activate transmission, 0 to deactivate it. */
};

/** \brief UART cluster request structure.
 *
 * This structure is used by the runtime to manage a cluster remote copy with
 * the UART. It must be instantiated once for each copy and must be kept
 * alive until the copy is finished. It can be instantiated as a normal
 * variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_uart_req_s pi_cl_uart_req_t;

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/** \brief Initialize a UART configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the uart
 * device is opened.
 *
 * \param conf A pointer to the uart configuration.
 */
void pi_uart_conf_init(struct pi_uart_conf *conf);

/** \brief Open a UART device.
 *
 * This function must be called before the UART device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_open(struct pi_device *device);

/** \brief Close an opened UART device.
 *
 * This function can be called to close an opened UART device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_uart_close(struct pi_device *device);

/** \brief Write data to an UART.
 *
 * This writes data to the specified UART.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param buffer      The address of the data in the processor.
 * \param size        The size in bytes of the copy.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_write(struct pi_device *device, void *buffer, uint32_t size);

/** \brief Read data from an UART.
 *
 * This reads data from the specified UART.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param buffer      The address of the data in the processor.
 * \param size        The size in bytes of the copy.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_read(struct pi_device *device, void *buffer, uint32_t size);

/** \brief Write a byte to an UART.
 *
 * This writes a byte to the specified UART.
 * The caller is blocked until the transfer is finished.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param byte       A pointer to the byte which should be written.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_write_byte(struct pi_device *device, uint8_t *byte);

/** \brief Read a byte from an UART.
 *
 * This reads a byte from the specified UART.
 * The caller is blocked until the transfer is finished.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param byte       A pointer to the byte where the data should be written.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_read_byte(struct pi_device *device, uint8_t *byte);

/** \brief Write data to an UART asynchronously.
 *
 * This writes data to the specified UART asynchronously.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param buffer      The address of the data in the processor.
 * \param size        The size in bytes of the copy.
 * \param callback        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_write_async(struct pi_device *device, void *buffer, uint32_t size, pi_task_t* callback);

/** \brief Read data from an UART asynchronously.
 *
 * This reads data from the specified UART asynchronously.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param buffer      The address of the data in the processor.
 * \param size        The size in bytes of the copy.
 * \param callback        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_read_async(struct pi_device *device, void *buffer, uint32_t size, pi_task_t* callback);

/** \brief Write a byte to an UART asynchronously.
 *
 * This writes a byte to the specified UART asynchronously.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param byte       A pointer to the byte which should be written.
 * \param callback        The task used to notify the end of transfer. See the
 *   documentation of pi_task_t for more details.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_uart_write_byte_async(struct pi_device *device, uint8_t *byte, pi_task_t* callback);


/** \brief Write data to an UART from cluster side.
 *
 * This function implements the same feature as pi_uart_write but can be called
 * from cluster side in order to expose the feature on the cluster.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 *
 * \param device      The device descriptor of the UART on which to do
 *   the copy.
 * \param buffer    The memory buffer from where the data to write to the UART
 *   must be read.
 * \param size      The size in bytes of the memory buffer.
 * \param req       The request structure used for termination.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req);

/** \brief Write a byte to an UART from cluster side.
 *
 * This function implements the same feature as pi_uart_write_byte but can be
 * called from cluster side in order to expose the feature on the cluster.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param byte       A pointer to the byte which should be written.
 * \param req       The request structure used for termination.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_cl_uart_write_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req);

/** \brief Wait until the specified UART cluster write request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_uart_write_wait(pi_cl_uart_req_t *req);

/** \brief Read a byte from an UART from cluster side.
 *
 * This function implements the same feature as pi_uart_read_byte but can be
 * called from cluster side in order to expose the feature on the cluster.
 * A pointer to a request structure must be provided so that the runtime can
 * properly do the remote call.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param addr      The address of the data in the processor.
 * \param size        The size in bytes of the copy.
 * \param req       The request structure used for termination.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_cl_uart_read(pi_device_t *device, void *addr, uint32_t size, pi_cl_uart_req_t *req);

/** \brief Read a byte from an UART.
 *
 * This reads a byte from the specified UART.
 * The caller is blocked until the transfer is finished.
 *
 * \param device      The device descriptor of the UART device on which to do
 *   the copy.
 * \param byte       A pointer to the byte where the data should be written.
 * \param req       The request structure used for termination.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_cl_uart_read_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req);

/** \brief Wait until the specified UART cluster read request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is
 * finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void pi_cl_uart_read_wait(pi_cl_uart_req_t *req);

#if defined(__cplusplus)
}
#endif

//!@}

/**
 * @}
 */

#endif /* _GAP_UART_H_ */
