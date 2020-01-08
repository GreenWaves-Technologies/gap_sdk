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

#include "stddef.h"
#include "stdarg.h"
#include "pmsis_driver/cpi/cpi_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include DEFAULT_MALLOC_INC

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct cpi_driver_fifo_s *__global_cpi_driver_fifo[UDMA_NB_CPI];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* End of task handler, release/execute user callback. */
static void __pi_cpi_handle_end_of_task(struct pi_task *task);

/* Check if a HW UDMA slot is free. */
static int32_t __pi_cpi_hw_fifo_empty(struct cpi_driver_fifo_s *fifo);

/* Enqueue a new task in HW fifo for callback. */
static void __pi_cpi_hw_fifo_enqueue(struct cpi_driver_fifo_s *fifo,
                                     struct pi_task *task);

/* Pop a task from HW fifo. */
static struct pi_task *__pi_cpi_hw_fifo_pop(struct cpi_driver_fifo_s *fifo);

/* Enqueue a new transfer in SW fifo. */
static int32_t __pi_cpi_task_fifo_enqueue(struct cpi_driver_fifo_s *fifo,
                                          struct cpi_transfer_s *transfer,
                                          struct pi_task *task);

/* Pop a transfer from SW fifo. */
static struct cpi_cb_args_s *__pi_cpi_task_fifo_pop(struct cpi_driver_fifo_s *fifo);

/* Enqueue a task in HW fifo and enqueue the transfer in UDMA. */
static void __pi_cpi_copy_exec(struct cpi_driver_fifo_s *fifo,
                               struct cpi_transfer_s *transfer,
                               struct pi_task *task);

/*******************************************************************************
 * Inner functions
 ******************************************************************************/

static void __pi_cpi_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    }
}

void cpi_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_CPI_ID(0);

    struct cpi_driver_fifo_s *fifo = __global_cpi_driver_fifo[periph_id];
    struct pi_task *task = __pi_cpi_hw_fifo_pop(fifo);
    if (task != NULL)
    {
        __pi_cpi_handle_end_of_task(task);
    }
    struct cpi_cb_args_s *cb_args = __pi_cpi_task_fifo_pop(fifo);
    if (cb_args != NULL)
    {
        //int32_t hw_buffer_slot = __pi_cpi_hw_fifo_empty(fifo);
        __pi_cpi_copy_exec(fifo, &(cb_args->transfer), cb_args->cb);
        /* Free allocated cpi_cb_args_s struct. */
        pi_default_free(cb_args, sizeof(struct cpi_cb_args_s));
    }
}

static int32_t __pi_cpi_hw_fifo_empty(struct cpi_driver_fifo_s *fifo)
{
    return ((fifo->hw_buffer[RX_CHANNEL] == NULL) ? 1 : 0);
}

static void __pi_cpi_hw_fifo_enqueue(struct cpi_driver_fifo_s *fifo,
                                     struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    /* Enqueue task in hw_buffer to signal the slot is used. */
    fifo->hw_buffer[RX_CHANNEL] = task;
    __restore_irq(irq);
}

static struct pi_task *__pi_cpi_hw_fifo_pop(struct cpi_driver_fifo_s *fifo)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    task_to_return = fifo->hw_buffer[RX_CHANNEL];
    /* Free the slot for another transfer. */
    fifo->hw_buffer[RX_CHANNEL] = NULL;
    __restore_irq(irq);
    return task_to_return;
}

static int32_t __pi_cpi_task_fifo_enqueue(struct cpi_driver_fifo_s *fifo,
                                          struct cpi_transfer_s *transfer,
                                          struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct cpi_cb_args_s *cb_args = (struct cpi_cb_args_s *) pmsis_l2_malloc(sizeof(struct cpi_cb_args_s));
    if (cb_args == NULL)
    {
        __restore_irq(irq);
        return -1;
    }
    /* Callback args. */
    cb_args->cb = task;
    cb_args->transfer.buffer = transfer->buffer;
    cb_args->transfer.size = transfer->size;
    cb_args->transfer.device_id = transfer->device_id;
    cb_args->next = NULL;
    /* Enqueue transfer in SW fifo. */
    if (fifo->fifo_head == NULL)
    {
        /* Empty fifo. */
        fifo->fifo_head = cb_args;
        fifo->fifo_tail = fifo->fifo_head;
    }
    else
    {
        /* Enqueue to tail. */
        fifo->fifo_tail->next = cb_args;
        fifo->fifo_tail = fifo->fifo_tail->next;
    }
    __restore_irq(irq);
    return 0;
}

static struct cpi_cb_args_s *__pi_cpi_task_fifo_pop(struct cpi_driver_fifo_s *fifo)
{
    struct cpi_cb_args_s *cb_args_return = NULL;
    uint32_t irq = __disable_irq();
    if (fifo->fifo_head != NULL)
    {
        cb_args_return = fifo->fifo_head;
        fifo->fifo_head = fifo->fifo_head->next;
        if (fifo->fifo_head == NULL)
        {
            fifo->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return cb_args_return;
}

static void __pi_cpi_copy_exec(struct cpi_driver_fifo_s *fifo,
                               struct cpi_transfer_s *transfer,
                               struct pi_task *task)
{
    uint32_t device_id = transfer->device_id;
    uint32_t buffer = (uint32_t) transfer->buffer;
    uint32_t size = transfer->size;
    uint32_t cfg = (UDMA_CORE_RX_CFG_DATASIZE(16 >> 4) | UDMA_CORE_RX_CFG_EN(1));
    __pi_cpi_hw_fifo_enqueue(fifo, task);
    hal_cpi_enqueue(device_id, RX_CHANNEL, buffer, size, cfg);
}

void __pi_cpi_copy(struct cpi_driver_fifo_s *fifo, struct cpi_transfer_s *transfer,
                   struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    int32_t slot = __pi_cpi_hw_fifo_empty(fifo);
    if (!slot)
    {
        /* Enqueue transfer in SW fifo. */
        __pi_cpi_task_fifo_enqueue(fifo, transfer, task);
    }
    else
    {
        /* Enqueue transfer in HW fifo. */
        __pi_cpi_copy_exec(fifo, transfer, task);
    }
    __restore_irq(irq);
}


#if 0
/* Enqueue SW fifo. */
static void __pi_cpi_fifo_enqueue(struct cpi_driver_fifo_s *fifo, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->next = NULL;
    if (fifo->fifo_head == NULL)
    {
        /* Empty fifo. */
        fifo->fifo_head = task;
        fifo->fifo_tail = fifo->fifo_head;
    }
    else
    {
        fifo->fifo_tail->next = task;
        fifo->fifo_tail = fifo->fifo_tail->next;
    }
    __restore_irq(irq);
}

/* Pop SW fifo. */
static struct pi_task *__pi_cpi_fifo_pop(struct cpi_driver_fifo_s *fifo)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    if (fifo->fifo_head != NULL)
    {
        task_to_return = fifo->fifo_head;
        fifo->fifo_head = fifo->fifo_head->next;
        if (fifo->fifo_head == NULL)
        {
            fifo->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return task_to_return;
}

static void __pi_cpi_callback(void *arg)
{
    uint32_t irq = __disable_irq();
    struct cpi_cb_args_s *cb_args = (struct cpi_cb_args_s *) arg;
    int32_t slot = __pi_cpi_hw_fifo_empty(fifo);
    if (slot == -1)
    {
    }
    __global_cpi_driver_fifo[cb_args->transfer.device_id]->hw_buffer[slot] = cb_args->cb;
    hal_cpi_enqueue(cb_args->transfer.device_id,
                    RX_CHANNEL,
                    (uint32_t) cb_args->transfer.buffer,
                    cb_args->transfer.size,
                    UDMA_CORE_RX_CFG_DATASIZE(16 >> 4) | UDMA_CORE_RX_CFG_EN(1));
    __restore_irq(irq);
}

static pi_task_t *__pi_cpi_prepare_callback(struct cpi_transfer_s *transfer,
                                            struct pi_task *task)
{
    struct pi_task *cb_task = (struct pi_task *) pmsis_l2_malloc(sizeof(struct pi_task));
    struct cpi_cb_args_s *cb_args = (struct cpi_cb_args_s *) pmsis_l2_malloc(sizeof(struct cpi_cb_args_s));
    if ((cb_task == NULL) || (cb_args == NULL))
    {
        return NULL;
    }
    /* Callback args. */
    cb_args->transfer.buffer = transfer->buffer;
    cb_args->transfer.size = transfer->size;
    cb_args->transfer.device_id = transfer->device_id;
    cb_args->cb = task;
    /* Callback task. */
    pi_task_callback_no_mutex(cb_task, __pi_cpi_callback, cb_args);
    cb_task->destroy = 1;

    return cb_task;
}
#endif
