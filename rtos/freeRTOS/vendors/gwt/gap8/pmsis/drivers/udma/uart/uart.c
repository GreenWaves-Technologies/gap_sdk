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

#include "pmsis/implem/drivers/udma/uart/uart_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_uart_conf_init(struct pi_uart_conf *conf)
{
    __pi_uart_conf_init(conf);
}

int pi_uart_open(struct pi_device *device)
{
    int32_t status = 1;
    struct pi_uart_conf *conf = (struct pi_uart_conf *) device->config;
    UART_TRACE("Open device id=%ld\n", conf->uart_id);
    status = __pi_uart_open((struct uart_itf_data_s **) &(device->data), conf);
    UART_TRACE("Open status : %ld\n", status);
    return status;
}

void pi_uart_close(struct pi_device *device)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    if (data != NULL)
    {
        UART_TRACE("Close device id=%d\n", data->device_id);
        __pi_uart_close(data);
        device->data = NULL;
    }
}

int pi_uart_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    UART_TRACE("Ioctl device id=%ld cmd=%ld arg=%lx\n",
               data->device_id, cmd, arg);
    return __pi_uart_ioctl(data, cmd, arg);
}

int pi_uart_write(struct pi_device *device, void *buffer, uint32_t size)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_write_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_write_byte(struct pi_device *device, uint8_t *byte)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_write_async(device, byte, 1, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_write_byte_async(struct pi_device *device, uint8_t *byte,
                             pi_task_t* callback)
{
    return pi_uart_write_async(device, byte, 1, callback);
}

int pi_uart_write_async(struct pi_device *device, void *buffer, uint32_t size,
                        pi_task_t* callback)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    UART_TRACE("UART(%ld): Write l2_buf=%lx size=%ld\n",
               data->device_id, l2_buf, size);
    return __pi_uart_copy(data, l2_buf, size, TX_CHANNEL, callback);
}

int pi_uart_read(struct pi_device *device, void *buffer, uint32_t size)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_read_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_read_byte(struct pi_device *device, uint8_t *byte)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_read_async(device, byte, 1, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_read_async(struct pi_device *device, void *buffer, uint32_t size,
                       pi_task_t* callback)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    UART_TRACE("UART(%ld): Read l2_buf=%lx size=%ld\n",
               data->device_id, l2_buf, size);
    return __pi_uart_copy(data, l2_buf, size, RX_CHANNEL, callback);
}
