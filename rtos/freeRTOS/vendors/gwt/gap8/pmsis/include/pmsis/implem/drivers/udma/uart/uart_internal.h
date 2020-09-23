/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PMSIS_IMPLEM_DRIVERS_UDMA_UART_UART_INTERNAL_H__
#define __PMSIS_IMPLEM_DRIVERS_UDMA_UART_UART_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/drivers/uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define UART_TX_BUFFER_SIZE     16
#define UART_DEFAULT_PRE_ALLOC_EVT 5

#define NB_UART 1

#ifndef UART_DRIVER_DATA_IMPLEM_SPECIFC
    #define UART_DRIVER_DATA_IMPLEM_SPECIFC
#endif


/*
 * pi_task:
 * data[0] = l2_buf
 * data[1] = size
 * data[2] = channel
 * data[3] = repeat_size
 * data[4] = device_id (used for delegation)
 */
struct uart_itf_data_s
{
    struct pi_task *fifo_head[2]; /*!< 0 = RX | 1 = TX. */
    struct pi_task *fifo_tail[2]; /*!< 0 = RX | 1 = TX. */
    uint32_t nb_open;             /*!< Number of times device has been opened. */
    uint32_t device_id;           /*!< Device ID. */
    uint32_t baudrate_bps;        /*!< Current baudrate. */
    pi_freq_cb_t uart_freq_cb;    /*!< Callback associated to frequency changes. */
    uint8_t flow_ctrl_ena;        /*!< Flow control enable? */
    #if defined(UART_FLOW_CONTROL_EMU)
    struct pi_device pwm;         /*!< PWM device used to emulate flow control. */
    #endif  /* UART_FLOW_CONTROL_EMU */
    UART_DRIVER_DATA_IMPLEM_SPECIFC
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Initialize conf struct.
 *
 * This function initializes a config struct with default values.
 *
 * \param conf           Pointer to struct pi_uart_conf.
 */
void __pi_uart_conf_init(struct pi_uart_conf *conf);

/**
 * \brief Open a device.
 *
 * This function opens a device.
 * A conf struct and a pointer to store UART driver info should be given.
 *
 * \para driver_data     Pointer to store driver info.
 * \param conf           Pointer to struct pi_uart_conf.
 *
 * \retval 0            If operation is successfull.
 * \retval ERRNO        An error code otherwise.
 */
int32_t __pi_uart_open(struct uart_itf_data_s **driver_data,
                       struct pi_uart_conf *conf);

/**
 * \brief Close a device.
 *
 * This function closes an opened device.
 *
 * \param driver_data    Pointer to driver info.
 */
void __pi_uart_close(struct uart_itf_data_s *driver_data);

/**
 * \brief Ioctl commands.
 *
 * This function allows to configure a device using ioctl commands.
 *
 * \param driver_data    Pointer to driver info.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl command arg.
 *
 * \retval -1            If wrong ioctl command.
 * \retval Value         Value depending on ioctl command.
 */
int32_t __pi_uart_ioctl(struct uart_itf_data_s *driver_data, uint32_t cmd,
                        void *arg);

/**
 * \brief Transfer data.
 *
 * This function allows to send/receive data using the periph.
 * The transfer is executed immediately if there is no current transfer or no
 * pending transfer. Otherwise, the transfer is enqueued in a fifo.
 *
 * \param driver_data    Pointer to driver info.
 * \param l2_buf         Address of data buffer.
 * \param size           Size of data buffer.
 * \param channel        Direction of transfer.
 * \param task           Event task used to notify end of transfer.
 *
 * \retval 0             If operation is successfull.
 * \retval -1            Otherwise.
 */
int32_t __pi_uart_copy(struct uart_itf_data_s *driver_data, uint32_t l2_buf,
                       uint32_t size, udma_channel_e channel,
                       struct pi_task *task);

/**
 * \brief Transfer data.
 *
 * This function allows to send data using the periph.
 * The transfer is executed immediately if there is no current transfer or no
 * pending transfer. Otherwise, the transfer is enqueued in a fifo.
 *
 * \param driver_data    Pointer to driver info.
 * \param buffer         Pointer to data buffer.
 * \param size           Size of data buffer.
 * \param task           Event task used to notify end of transfer.
 *
 * \retval 0             If operation is successfull.
 * \retval -1            Otherwise.
 */
static inline int __pi_uart_write(struct uart_itf_data_s *data, void *buffer,
                                  uint32_t size, pi_task_t *callback)
{
    return __pi_uart_copy(data, (uint32_t) buffer, size, TX_CHANNEL, callback);
}

/**
 * \brief Transfer data.
 *
 * This function allows to receive data using the periph.
 * The transfer is executed immediately if there is no current transfer or no
 * pending transfer. Otherwise, the transfer is enqueued in a fifo.
 *
 * \param driver_data    Pointer to driver info.
 * \param buffer         Pointer to data buffer.
 * \param size           Size of data buffer.
 * \param task           Event task used to notify end of transfer.
 *
 * \retval 0             If operation is successfull.
 * \retval -1            Otherwise.
 */
static inline int __pi_uart_read(struct uart_itf_data_s *data, void *buffer,
                                 uint32_t size, pi_task_t *callback)
{
   return  __pi_uart_copy(data, (uint32_t) buffer, size, RX_CHANNEL, callback);
}

#endif  /* __PMSIS_IMPLEM_DRIVERS_UDMA_UART_UART_INTERNAL_H__ */
