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
#include "pmsis_driver/i2c/i2c_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include DEFAULT_MALLOC_INC

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Defines for read & write adress access. */
#define ADDRESS_WRITE 0x0
#define ADDRESS_READ  0x1

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct i2c_driver_fifo_s *__global_i2c_driver_fifo[UDMA_NB_I2C];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Handle a pending transfer after end of previous part of transfer. */
static void __pi_i2c_handle_pending_transfer(struct i2c_driver_fifo_s *fifo);

/* End of task handler, release task/execute user callback. */
static void __pi_i2c_handle_end_of_task(struct pi_task *task);

/*
 * Check if both RX & TX channels are empty, needed to make sure there are
 * no collision between a write req and a read req(which uses TX).
 */
static int32_t __pi_i2c_hw_fifo_both_empty(struct i2c_driver_fifo_s *fifo);

/* Check if a HW UDMA slot is free. */
static int32_t __pi_i2c_hw_fifo_empty(struct i2c_driver_fifo_s *fifo,
                                      udma_channel_e channel);

/* Enqueue a new task in HW fifo for callback. */
static void __pi_i2c_hw_fifo_enqueue(struct i2c_driver_fifo_s *fifo,
                                     struct pi_task *task,
                                     udma_channel_e channel);

/* Pop a task from HW fifo. */
static struct pi_task *__pi_i2c_hw_fifo_pop(struct i2c_driver_fifo_s *fifo,
                                            udma_channel_e channel);

/* Create a new callabck struct with transfer info then enqueue it in SW fifo. */
static int32_t __pi_i2c_task_fifo_enqueue(struct i2c_driver_fifo_s *fifo,
                                          struct i2c_transfer_s *transfer,
                                          struct pi_task *task);

/* Pop a callback struct containing a new transfer from SW fifo. */
static struct i2c_cb_args_s *__pi_i2c_task_fifo_pop(struct i2c_driver_fifo_s *fifo);

/*
 * Enqueue a read command sequence. This function is called by __pi_i2c_read and
 * the IRQ handle to enqueue pending transfer.
 */
static void __pi_i2c_copy_exec_read(struct i2c_driver_fifo_s *fifo,
                                    struct i2c_transfer_s *transfer,
                                    struct pi_task *task);

/* Initiate a read command sequence. */
static void __pi_i2c_read(struct i2c_driver_fifo_s *fifo,
                          struct i2c_transfer_s *transfer,
                          struct pi_task *task);

/*
 * Enqueue a read command sequence. This function is called by __pi_i2c_write and
 * the IRQ handle to enqueue pending transfer.
 */
static void __pi_i2c_copy_exec_write(struct i2c_driver_fifo_s *fifo,
                                     struct i2c_transfer_s *transfer,
                                     struct pi_task *task);

/* Initiate a write command sequence. */
static void __pi_i2c_write(struct i2c_driver_fifo_s *fifo,
                           struct i2c_transfer_s *transfer,
                           struct pi_task *task);

/* DEPRECATED. */
/* Enqueue a task in HW fifo and enqueue the transfer in UDMA. */
static void __pi_i2c_copy_exec(struct i2c_driver_fifo_s *fifo,
                               struct i2c_transfer_s *transfer,
                               struct pi_task *task,
                               uint32_t hw_buffer_slot);

/* This variable is used to count number of events received to handle EoT sequence. */
static uint8_t nb_events = 0;

/*******************************************************************************
 * Inner functions
 ******************************************************************************/

static void __pi_i2c_handle_pending_transfer(struct i2c_driver_fifo_s *fifo)
{
    struct i2c_pending_transfer_s *pending = fifo->pending;
    pending->pending_buffer += pending->pending_repeat;
    pending->pending_repeat_size -= pending->pending_repeat;
    pending->pending_size = pending->pending_repeat;

    if (pending->pending_repeat_size <= pending->pending_repeat)
    {
        pending->pending_repeat = 0;
        pending->pending_size = pending->pending_repeat_size;
        /* Stop bit at the end? */
        fifo->i2c_stop_send = (pending->flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
    }
    /* Initiate next part of command sequence. */
    {
        /* Data. */
        uint32_t index = 0;
        fifo->i2c_cmd_seq[index++] = I2C_CMD_RPT;
        fifo->i2c_cmd_seq[index++] = pending->pending_size;
        fifo->i2c_cmd_seq[index++] = I2C_CMD_WR;
    }
    //hal_i2c_enqueue(device_id, fifo->channel);
    /* TODO: Renqueue next cmd! */
}

static void __pi_i2c_send_stop_cmd(uint32_t device_id, struct i2c_driver_fifo_s *fifo)
{
    /* Send stop command. */
    fifo->i2c_stop_send = 0;
    hal_i2c_enqueue(device_id, TX_CHANNEL, (uint32_t) fifo->i2c_stop_seq,
                    (uint32_t) __PI_I2C_STOP_CMD_SIZE, UDMA_CORE_TX_CFG_EN(1));
}

static void __pi_i2c_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        DEBUG_PRINTF("[%s] releasing task: %p, with id %i\n",__func__,task, task->id);
        pi_task_release(task);
    }
    else
    {
        DEBUG_PRINTF("[%s] pushing event: %p, with id %i, in sched %p\n",__func__,task, task->id, pmsis_event_get_default_scheduler());
        pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    }
}

void i2c_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2C_ID(0);

    struct i2c_driver_fifo_s *fifo = __global_i2c_driver_fifo[periph_id];
    /*
     * In case of a read command sequence, TX ends first then wait on RX.
     * Until then, no other transaction should occur.
     * Check if fifo->hw_buffer[channel] != NULL.
     * If NULL -> read command, so wait until RX event.
     * Else -> write command, pop associated task.
     * When receiving the first event, it should not be handled.
     * Need to check again : WHY ?????
     */
    nb_events++;
    if (nb_events == 1)
    {
        return;
    }
    if (fifo->hw_buffer[channel] != NULL)
    {
        /* Pending transfer. */
        if (fifo->pending->pending_repeat)
        {
            __pi_i2c_handle_pending_transfer(fifo);
        }
        else
        {
            if (fifo->i2c_stop_send)
            {
                if (channel == RX_CHANNEL)
                {
                    fifo->hw_buffer[TX_CHANNEL] = fifo->hw_buffer[RX_CHANNEL];
                    fifo->hw_buffer[RX_CHANNEL] = NULL;
                }
                __pi_i2c_send_stop_cmd(periph_id, fifo);
            }
            else
            {
                struct pi_task *task = __pi_i2c_hw_fifo_pop(fifo, channel);
                if (task != NULL)
                {
                    __pi_i2c_handle_end_of_task(task);
                }
                struct i2c_cb_args_s *cb_args = __pi_i2c_task_fifo_pop(fifo);
                if (cb_args != NULL)
                {
                    /* Enqueue transfer in HW fifo. */
                    if (cb_args->transfer.channel == RX_CHANNEL)
                    {
                        __pi_i2c_read(fifo, &(cb_args->transfer), cb_args->cb);
                    }
                    else
                    {
                        __pi_i2c_write(fifo, &(cb_args->transfer), cb_args->cb);
                    }
                    pmsis_l2_malloc_free(cb_args, sizeof(struct i2c_cb_args_s));
                }
                nb_events = 0;
            }
        }
    }
}

static int32_t __pi_i2c_hw_fifo_both_empty(struct i2c_driver_fifo_s *fifo)
{
    int32_t rx_empty = __pi_i2c_hw_fifo_empty(fifo, RX_CHANNEL);
    int32_t tx_empty = __pi_i2c_hw_fifo_empty(fifo, TX_CHANNEL);
    return  ((rx_empty == 1) && (tx_empty == 1));
}

static int32_t __pi_i2c_hw_fifo_empty(struct i2c_driver_fifo_s *fifo, udma_channel_e channel)
{
    return ((fifo->hw_buffer[channel] == NULL) ? 1 : 0);
}

static void __pi_i2c_hw_fifo_enqueue(struct i2c_driver_fifo_s *fifo,
                                     struct pi_task *task,
                                     udma_channel_e channel)
{
    uint32_t irq = __disable_irq();
    /* Enqueue task in hw_buffer[channel] to signal the slot is used. */
    fifo->hw_buffer[channel] = task;
    __restore_irq(irq);
}

static struct pi_task *__pi_i2c_hw_fifo_pop(struct i2c_driver_fifo_s *fifo,
                                            udma_channel_e channel)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    task_to_return = fifo->hw_buffer[channel];
    /* Free the slot for another transfer. */
    fifo->hw_buffer[channel] = NULL;
    __restore_irq(irq);
    return task_to_return;
}

static int32_t __pi_i2c_task_fifo_enqueue(struct i2c_driver_fifo_s *fifo,
                                          struct i2c_transfer_s *transfer,
                                          struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct i2c_cb_args_s *cb_args = (struct i2c_cb_args_s *) pmsis_l2_malloc(sizeof(struct i2c_cb_args_s));
    if (cb_args == NULL)
    {
        __restore_irq(irq);
        return -1;
    }
    /* Callback args. */
    cb_args->cb = task;
    cb_args->transfer.buffer = transfer->buffer;
    cb_args->transfer.size = transfer->size;
    cb_args->transfer.flags = transfer->flags;
    cb_args->transfer.device_id = transfer->device_id;
    cb_args->transfer.channel = transfer->channel;
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

static struct i2c_cb_args_s *__pi_i2c_task_fifo_pop(struct i2c_driver_fifo_s *fifo)
{
    struct i2c_cb_args_s *cb_args_return = NULL;
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

/* TODO : Get SOC frequence. */
uint32_t __pi_i2c_get_clk_div(uint32_t freq)
{
    #define __pi_get_freq() 50000000
    uint32_t div = (__pi_get_freq() + freq - 1) / freq;
    if (div & 0x1)
    {
        div += 1;
    }
    div >>= 1;
    return div;
}

static void __pi_i2c_copy_exec_read(struct i2c_driver_fifo_s *fifo,
                                    struct i2c_transfer_s *transfer,
                                    struct pi_task *task)
{
    uint32_t device_id = transfer->device_id;
    uint32_t cmd_seq = (uint32_t) fifo->i2c_cmd_seq;
    uint32_t cmd_index = fifo->i2c_cmd_index;
    uint32_t buffer = (uint32_t) transfer->buffer;
    uint32_t size = transfer->size;
    if (task != NULL)
    {
        __pi_i2c_hw_fifo_enqueue(fifo, task, RX_CHANNEL);
    }
    else
    {
        buffer = (uint32_t) fifo->pending->pending_buffer;
        size = (uint32_t) fifo->pending->pending_size;
    }

    /* Open RX channel to receive data. */
    hal_i2c_enqueue(device_id, RX_CHANNEL, buffer, size, UDMA_CORE_RX_CFG_EN(1));
    /* Transfer command. */
    hal_i2c_enqueue(device_id, TX_CHANNEL, cmd_seq, cmd_index, UDMA_CORE_TX_CFG_EN(1));
}

static void __pi_i2c_read(struct i2c_driver_fifo_s *fifo,
                          struct i2c_transfer_s *transfer,
                          struct pi_task *task)
{
    uint32_t index = 0, start_bit = 0, stop_bit = 0;
    if (transfer->size > 0)
    {
        /* Header. */
        fifo->i2c_cmd_seq[index++] = I2C_CMD_CFG;
        fifo->i2c_cmd_seq[index++] = ((fifo->div >> 8) & 0xFF);
        fifo->i2c_cmd_seq[index++] = (fifo->div & 0xFF);
        fifo->i2c_cmd_seq[index++] = I2C_CMD_START;
        fifo->i2c_cmd_seq[index++] = I2C_CMD_WR;
        fifo->i2c_cmd_seq[index++] = (fifo->cs | ADDRESS_READ);

        struct i2c_pending_transfer_s *pending = fifo->pending;
        if (transfer->size > (uint32_t) MAX_SIZE)
        {
            pending->pending_buffer = (uint32_t) transfer->buffer;
            pending->pending_repeat = (uint32_t) MAX_SIZE;
            pending->pending_repeat_size = transfer->size;
            pending->device_id = transfer->device_id;
            pending->flags = transfer->flags;
            pending->channel = transfer->channel;
            transfer->size = (uint32_t) MAX_SIZE;
        }
        else
        {
            pending->pending_repeat = 0;
            /* Stop bit at then end? */
            fifo->i2c_stop_send = (transfer->flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
        }
        /* Data. */
        if (transfer->size > 1)
        {
            fifo->i2c_cmd_seq[index++] = I2C_CMD_RPT;
            fifo->i2c_cmd_seq[index++] = transfer->size - 1;
            fifo->i2c_cmd_seq[index++] = I2C_CMD_RD_ACK;
        }
        fifo->i2c_cmd_seq[index++] = I2C_CMD_RD_NACK;

        fifo->i2c_cmd_index = index;
        __pi_i2c_copy_exec_read(fifo, transfer, task);
    }
}

static void __pi_i2c_copy_exec_write(struct i2c_driver_fifo_s *fifo,
                                     struct i2c_transfer_s *transfer,
                                     struct pi_task *task)
{
    uint32_t device_id = transfer->device_id;
    uint32_t cmd_seq = (uint32_t) fifo->i2c_cmd_seq;
    uint32_t cmd_index = fifo->i2c_cmd_index;
    uint32_t buffer = (uint32_t) transfer->buffer;
    uint32_t size = transfer->size;
    if (task != NULL)
    {
        __pi_i2c_hw_fifo_enqueue(fifo, task, TX_CHANNEL);
    }
    else
    {
        buffer = (uint32_t) fifo->pending->pending_buffer;
        size = (uint32_t) fifo->pending->pending_size;
    }
    /* Transfer header. */
    hal_i2c_enqueue(device_id, TX_CHANNEL, cmd_seq, cmd_index, UDMA_CORE_TX_CFG_EN(1));
    /* Transfer data. */
    hal_i2c_enqueue(device_id, TX_CHANNEL, buffer, size, UDMA_CORE_TX_CFG_EN(1));
}

static void __pi_i2c_write(struct i2c_driver_fifo_s *fifo,
                           struct i2c_transfer_s *transfer,
                           struct pi_task *task)
{
    uint32_t index = 0, start_bit = 0, stop_bit = 0;
    if (transfer->size > 0)
    {
        start_bit = transfer->flags & PI_I2C_XFER_NO_START;

        /* Header. */
        fifo->i2c_cmd_seq[index++] = I2C_CMD_CFG;
        fifo->i2c_cmd_seq[index++] = ((fifo->div >> 8) & 0xFF);
        fifo->i2c_cmd_seq[index++] = (fifo->div & 0xFF);
        if (!start_bit)
        {
            fifo->i2c_cmd_seq[index++] = I2C_CMD_START;
            fifo->i2c_cmd_seq[index++] = I2C_CMD_WR;
            fifo->i2c_cmd_seq[index++] = (fifo->cs | ADDRESS_WRITE);
        }
        struct i2c_pending_transfer_s *pending = fifo->pending;
        if (transfer->size > (uint32_t) MAX_SIZE)
        {
            pending->pending_buffer = (uint32_t) transfer->buffer;
            pending->pending_repeat = (uint32_t) MAX_SIZE;
            pending->pending_repeat_size = transfer->size;
            pending->device_id = transfer->device_id;
            pending->flags = transfer->flags;
            pending->channel = transfer->channel;
            transfer->size = (uint32_t) MAX_SIZE;
        }
        else
        {
            pending->pending_repeat = 0;
            /* Stop bit at the end? */
            fifo->i2c_stop_send = (transfer->flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
        }
        /* Data. */
        fifo->i2c_cmd_seq[index++] = I2C_CMD_RPT;
        fifo->i2c_cmd_seq[index++] = transfer->size;
        fifo->i2c_cmd_seq[index++] = I2C_CMD_WR;

        fifo->i2c_cmd_index = index;
        __pi_i2c_copy_exec_write(fifo, transfer, task);
    }
}

void __pi_i2c_copy(struct i2c_driver_fifo_s *fifo, struct i2c_transfer_s *transfer,
                   struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    int32_t slot_rxtx = __pi_i2c_hw_fifo_both_empty(fifo);
    /* Both slots should be empty to start a new read transfer. When enqueueing
     * a new read transfer, RX should be opened first then TX. So if RX is already
     * in use, then wait for it to finish. */
    if (!slot_rxtx)
    {
        /* Enqueue transfer in SW fifo. */
        DEBUG_PRINTF("[%s] Enqueueing transfer in SW fifo : task %p id %p channel %d.\n",
                     __func__, task, task->id, transfer->channel);
        __pi_i2c_task_fifo_enqueue(fifo, transfer, task);
    }
    else
    {
        /* Enqueue transfer in HW fifo. */
        DEBUG_PRINTF("[%s] Enqueueing transfer in HW fifo : task %p id %p channel %d.\n",
                     __func__, task, task->id, transfer->channel);
        if (transfer->channel == RX_CHANNEL)
        {
            __pi_i2c_read(fifo, transfer, task);
        }
        else
        {
            __pi_i2c_write(fifo, transfer, task);
        }
    }
    __restore_irq(irq);
}
