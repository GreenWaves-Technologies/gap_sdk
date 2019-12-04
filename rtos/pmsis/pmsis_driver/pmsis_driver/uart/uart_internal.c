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
#include "pmsis.h"
#include DEFAULT_MALLOC_INC
#include "pmsis_driver/uart/uart_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/rtos/os_frontend_api/pmsis_task.h"
#include "pmsis_driver/pmsis_fc_event.h"
#include "pmsis_hal/pmsis_hal.h"

#define PRINTF(...)

struct uart_driver_data *__global_uart_drv_data[NB_UART];

static pi_task_t *__pi_uart_end_task[2] = {NULL, NULL};

/*******************************************************************************
 * Prototypes & inlines
 ******************************************************************************/

static inline void __irq_exec_transfer(void *arg, udma_channel_e channel)
{
    struct uart_callback_args *cb_args = (struct uart_callback_args *)arg;
    __pi_uart_end_task[channel] = cb_args->callback;
    // Enqueue transfer directly since no one is using uart
    udma_uart_enqueue_transfer(cb_args->data->uart_id, cb_args->buffer, cb_args->size, channel);
    // this really should be some kind of slab
    pi_default_free(cb_args, sizeof(struct uart_callback_args));
}


__attribute__((section(".text"))) __noinline
void uart_handler(void *arg)
{
    #ifdef __GAP8__
    uint32_t uart_id = 0;
    #else
    uint32_t uart_id = (uint32_t) arg;
    #endif

    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;

    if (__pi_uart_end_task[channel] != NULL)
    {
        // if it's a mutex, release on the spot
        if (__pi_uart_end_task[channel]->id == PI_TASK_NONE_ID)
        {
            pi_task_release(__pi_uart_end_task[channel]);
        }
        else
        {
            // we really should never come here --> you should never wait on a callback
            PRINTF("WARNING: You are using an end of transfer callback\n This is deprecated and will be removed shortly, please update your code\n");
            pmsis_event_push(pmsis_event_get_default_scheduler(), __pi_uart_end_task[channel]);
        }
        __pi_uart_end_task[channel] = NULL;
    }

    pi_task_t *task = __uart_drv_fifo_pop(__global_uart_drv_data[uart_id], channel);
    if(task == NULL)
    {
        return;
    }
    else
    {
        // enqueue in place to reduce latency (but longer irq)
        __irq_exec_transfer((void*)task->arg[1], channel);
    }
}

/***********
 * Inner functions
 ***********/

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the UART configuration structure to a default value. The default
 * values are as follows.
 *   pi_uart_conf->baudRate_Bps = 115200U;
 *   pi_uart_conf->parity_mode = UART_PARITY_DISABLED;
 *   pi_uart_conf->stop_bit_count = UART_ONE_STOP_BIT;
 *   pi_uart_conf->enable_tx = false;
 *   pi_uart_conf->enable_rx = false;
 *
 * @param config Pointer to configuration structure.
 */
void pi_uart_conf_init(struct pi_uart_conf *conf)
{
    memset(conf, 0, sizeof(struct pi_uart_conf));
    conf->uart_id = 0;
    conf->baudrate_bps = 115200U;
    conf->parity_mode  = UART_PARITY_DISABLED;

    conf->stop_bit_count = UART_ONE_STOP_BIT;
    conf->src_clock_Hz = SystemCoreClock;

    conf->enable_tx = 0;
    conf->enable_rx = 0;
}

void  __pi_uart_write_callback(void *arg)
{
    struct uart_callback_args *cb_args = (struct uart_callback_args *)arg;
    int irq = __disable_irq();
    // have to block irq for a short time here as driver is irq driven
    //__uart_drv_fifo_enqueue(cb_args->data, cb_args->callback);
    __pi_uart_end_task[TX_CHANNEL] = cb_args->callback;
    // Enqueue transfer directly since no one is using uart
    udma_uart_enqueue_transfer(cb_args->data->uart_id, cb_args->buffer, cb_args->size, TX_CHANNEL);
    __restore_irq(irq);

    pi_default_free(cb_args, sizeof(struct uart_callback_args));
}

void  __pi_uart_read_callback(void *arg)
{
    struct uart_callback_args *cb_args = (struct uart_callback_args *)arg;

    int irq = __disable_irq();
    // have to block irq for a short time here as driver is irq driven
    //__uart_drv_fifo_enqueue(cb_args->data, cb_args->callback);
    __pi_uart_end_task[RX_CHANNEL] = cb_args->callback;
    // Enqueue transfer directly since no one is using uart
    udma_uart_enqueue_transfer(cb_args->data->uart_id, cb_args->buffer, cb_args->size, RX_CHANNEL);
    __restore_irq(irq);

    pi_default_free(cb_args, sizeof(struct uart_callback_args));
}

// Inner function with actual write logic
int __pi_uart_write(struct uart_driver_data *data, void *buffer, uint32_t size, pi_task_t *callback)
{
    // Due to udma restriction, we need to use an L2 address,
    // Since the stack is probably in FC tcdm, we have to either ensure users gave
    // us an L2 pointer or alloc ourselves
    if(((uintptr_t)buffer & 0xFFF00000) != 0x1C000000)
    {
        return -1;
    }

    // have to block irq for a short time here as driver is irq driven
    int irq = __disable_irq();
    if (__pi_uart_end_task[TX_CHANNEL] != NULL)
    {
        //PRINTF("Write enqueue\n");
        __uart_drv_fifo_enqueue(data, __uart_prepare_write_callback(data, buffer, size, callback),
                                TX_CHANNEL);
    }
    else
    {
        //__uart_drv_fifo_enqueue(data, callback);
        __pi_uart_end_task[TX_CHANNEL] = callback;
        // Enqueue transfer directly since no one is using uart
        udma_uart_enqueue_transfer(data->uart_id, buffer, size, TX_CHANNEL);
    }
    __restore_irq(irq);
    return 0;
}

int __pi_uart_read(struct uart_driver_data *data, void *buffer, uint32_t size, pi_task_t *callback)
{
    // Due to udma restriction, we need to use an L2 address,
    // Since the stack is probably in FC tcdm, we have to either ensure users gave
    // us an L2 pointer or alloc ourselves
    if(((uintptr_t)buffer & 0xFFF00000) != 0x1C000000)
    {
        return -1;
    }

    // have to block irq for a short time here as driver is irq driven
    int irq = __disable_irq();
    if (__pi_uart_end_task[RX_CHANNEL] != NULL)
    {
        __uart_drv_fifo_enqueue(data, __uart_prepare_read_callback(data, buffer, size, callback),
                                RX_CHANNEL);
    }
    else
    {
        __pi_uart_end_task[RX_CHANNEL] = callback;
        udma_uart_enqueue_transfer(data->uart_id, buffer, size, RX_CHANNEL);
    }
    __restore_irq(irq);
    return 0;
}
