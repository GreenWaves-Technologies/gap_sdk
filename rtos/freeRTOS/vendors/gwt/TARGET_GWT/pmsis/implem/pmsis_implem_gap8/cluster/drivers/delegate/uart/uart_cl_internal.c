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

#if defined(FEATURE_CLUSTER)

#include "pmsis.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"
#include "pmsis/implem/cluster/delegate/uart/uart_cl_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Cluster uart delegate copy.
 *
 * This function is executed by FC.
 */
static void __pi_cl_uart_delegate_copy(void *arg);

/**
 * \brief Cluster uart copy.
 *
 * The cluster core prepares a delegate transfer and send it to FC.
 * This function is executed by cluster cores.
 */
static inline int32_t __pi_cl_uart_copy(pi_device_t *device, void *buffer,
                                        uint32_t size, udma_channel_e channel,
                                        pi_cl_uart_req_t *req);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_cl_uart_delegate_copy(void *arg)
{
    pi_cl_uart_req_t *req = (pi_cl_uart_req_t *) arg;
    __pi_uart_copy((struct uart_itf_data_s *) req->task_done.data[4],
                   req->task_done.data[0], req->task_done.data[1],
                   req->task_done.data[2], &(req->task_done));
}

static inline int32_t __pi_cl_uart_copy(pi_device_t *device, void *buffer,
                                        uint32_t size, udma_channel_e channel,
                                        pi_cl_uart_req_t *req)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    req->task_done.data[0] = l2_buf;
    req->task_done.data[1] = size;
    req->task_done.data[2] = channel;
    req->task_done.data[4] = (uint32_t) data;
    /* FC Callback. */
    pi_task_callback_no_mutex(&(req->req_cb), __pi_cl_uart_delegate_copy, req);
    /* Send delegate task to FC. */
    pi_cl_send_task_to_fc(&(req->req_cb));
    return 0;
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block(&(req->task_done));
    __pi_cl_uart_copy(device, buffer, size, TX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_write_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, byte, 1, TX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_read(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, buffer, size, RX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_read_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, byte, 1, RX_CHANNEL, req);
    return 0;
}

#endif  /* FEATURE_CLUSTER */
