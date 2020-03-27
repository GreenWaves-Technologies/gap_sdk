/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* One I2S periph with two distinct devices inside. */
struct dmacpy_driver_fifo_s *g_dmacpy_driver_fifo[UDMA_NB_DMACPY] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_dmacpy_handle_end_of_task(struct pi_task *task);

static void __pi_dmacpy_handler(void *arg);

/* Enqueue a task in fifo. */
static uint8_t __pi_dmacpy_task_fifo_enqueue(struct dmacpy_driver_fifo_s *fifo,
                                             struct pi_task *task);

/* Pop a task from fifo. */
static struct pi_task *__pi_dmacpy_task_fifo_pop(struct dmacpy_driver_fifo_s *fifo);

static void __pi_dmacpy_copy_exec(struct dmacpy_driver_fifo_s *fifo,
                                  struct pi_task *task);

/*******************************************************************************

 * Internal functions
 ******************************************************************************/

static void __pi_dmacpy_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        if (task->id == PI_TASK_CALLBACK_ID)
        {
            pi_task_push(task);
        }
    }
}

static void __pi_dmacpy_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_DMACPY_ID(0);
    DMACPY_TRACE("DMACPY(%d) IRQ %d.\n", periph_id, event);

    struct dmacpy_driver_fifo_s *fifo = g_dmacpy_driver_fifo[periph_id];
    struct pi_task *task = fifo->fifo_head;
    /* Pending data on current transfer. */
    if ((task->data[3] == PI_DMACPY_L2_L2) && (channel == TX_CHANNEL))
    {
        DMACPY_TRACE("Current transfer is L2 to L2, received TX event, do nothing, continue.\n");
        return;
    }
    if (task->data[4] != 0)
    {
        DMACPY_TRACE("Reenqueue pending data on current transfer.\n");
        uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;
        task->data[0] += max_size;
        task->data[1] += max_size;
        task->data[2] -= max_size;
        //task->data[3]
        __pi_dmacpy_copy_exec(fifo, task);
    }
    else
    {
        DMACPY_TRACE("No pending data on current transfer.\n Handle end of "
                     "current transfer and pop and start a new transfer if there is.\n");
        task = __pi_dmacpy_task_fifo_pop(fifo);
        __pi_dmacpy_handle_end_of_task(task);
        task = fifo->fifo_head;
        if (task != NULL)
        {
            __pi_dmacpy_copy_exec(fifo, task);
        }
    }
}

/* Enqueue a task in fifo. */
static uint8_t __pi_dmacpy_task_fifo_enqueue(struct dmacpy_driver_fifo_s *fifo,
                                             struct pi_task *task)
{
    uint8_t head = 0;
    uint32_t irq = __disable_irq();
    if (fifo->fifo_head == NULL)
    {
        /* Transfer on going, enqueue this one to the list. */
        /* Empty fifo. */
        fifo->fifo_head = task;
        fifo->fifo_tail = fifo->fifo_head;
        head = 0;
        DMACPY_TRACE("Enqueue task to head.\n");
    }
    else
    {
        /* Enqueue the transfer to list of pending transfers. */
        fifo->fifo_tail->next = task;
        fifo->fifo_tail = fifo->fifo_tail->next;
        head = 1;
        DMACPY_TRACE("Enqueue task to tail.\n");
    }
    __restore_irq(irq);
    return head;
}

/* Pop a task from fifo. */
static struct pi_task *__pi_dmacpy_task_fifo_pop(struct dmacpy_driver_fifo_s *fifo)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    /* Pop the first task and return the next transfer if there are any. */
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

static void __pi_dmacpy_copy_exec(struct dmacpy_driver_fifo_s *fifo,
                                  struct pi_task *task)
{
    uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;
    uint32_t l2_buf_1 = 0, l2_buf_2 = 0;
    uint32_t src = task->data[0];
    uint32_t dst = task->data[1];
    uint32_t size = task->data[2];
    uint32_t dir = task->data[3];
    udma_channel_e channel = RX_CHANNEL;
    if (task->data[2] > max_size)
    {
        task->data[4] = task->data[2] - max_size;
        size = max_size;
    }
    switch (task->data[3])
    {
    case PI_DMACPY_FC_L1_L2 :
        l2_buf_1 = dst;
        channel = RX_CHANNEL;
        dir = 0;
        dst = 0;
        DMACPY_TRACE("DMACPY(%ld): enqueue FC to L2 l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     fifo->device_id, l2_buf_1, src, dst, size, dir, channel);
        hal_dmacpy_enqueue(fifo->device_id, l2_buf_1, size, 0, src, dst, dir, channel);
        break;

    case PI_DMACPY_L2_FC_L1 :
        l2_buf_1 = src;
        channel = TX_CHANNEL;
        src = 0;
        dir = 0;
        DMACPY_TRACE("DMACPY(%ld): enqueue L2 to FC l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     fifo->device_id, l2_buf_1, src, dst, size, dir, channel);
        hal_dmacpy_enqueue(fifo->device_id, l2_buf_1, size, 0, src, dst, dir, channel);
        break;

    case PI_DMACPY_L2_L2 :
        l2_buf_1 = dst;
        l2_buf_2 = src;
        channel = RX_CHANNEL;
        src = 0;
        dst = 0;
        dir = 1;
        DMACPY_TRACE("DMACPY(%ld): enqueue FC to L2 l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     fifo->device_id, l2_buf_1, src, dst, size, dir, channel);
        hal_dmacpy_enqueue(fifo->device_id, l2_buf_1, size, 0, src, dst, dir, channel);
        //l2_buf_2 = src;
        channel = TX_CHANNEL;
        src = 0;
        dst = 0;
        dir = 1;
        DMACPY_TRACE("DMACPY(%ld): enqueue L2 to FC l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     fifo->device_id, l2_buf_2, src, dst, size, dir, channel);
        hal_dmacpy_enqueue(fifo->device_id, l2_buf_2, size, 0, src, dst, dir, channel);
        break;

    default :
        DMACPY_TRACE_ERR("Wrong param for direction !\n");
        return;
    }
    DMACPY_TRACE("Enqueue DMACPY(%d): %x %x %x %x %x %x %x %x %x\n",
                 fifo->device_id,
                 dmacpy(fifo->device_id)->udma.rx_saddr,
                 dmacpy(fifo->device_id)->udma.rx_size,
                 dmacpy(fifo->device_id)->udma.rx_cfg,
                 dmacpy(fifo->device_id)->udma.tx_saddr,
                 dmacpy(fifo->device_id)->udma.tx_size,
                 dmacpy(fifo->device_id)->udma.tx_cfg,
                 dmacpy(fifo->device_id)->dst_addr,
                 dmacpy(fifo->device_id)->src_addr,
                 dmacpy(fifo->device_id)->mem_sel);
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf)
{
    conf->id = 0;
}

int32_t __pi_dmacpy_open(struct pi_dmacpy_conf *conf,
                         struct dmacpy_driver_fifo_s **device_data)
{
    struct dmacpy_driver_fifo_s *fifo = g_dmacpy_driver_fifo[conf->id];
    if (fifo == NULL)
    {
        fifo = (struct dmacpy_driver_fifo_s *) pi_l2_malloc(sizeof(struct dmacpy_driver_fifo_s));
        if (fifo == NULL)
        {
            DMACPY_TRACE_ERR("Driver fifo alloc failed !\n");
            return -11;
        }
        memset((void *) fifo, 0, sizeof(struct dmacpy_driver_fifo_s));
        fifo->fifo_head = NULL;
        fifo->fifo_tail = NULL;
        fifo->device_id = conf->id;
        fifo->nb_open   = 1;
        g_dmacpy_driver_fifo[fifo->device_id] = fifo;

        /* Open device for first time. */
        DMACPY_TRACE("Device id=%ld opened for first time\n", fifo->device_id);
        DMACPY_TRACE("dmacpy(%ld) %p\n", fifo->device_id, dmacpy(fifo->device_id));

        DMACPY_TRACE("Enable SoC events and set handlers : RX : %d -> %p | TX: %d -> %p\n",
                     SOC_EVENT_UDMA_DMACPY_RX(fifo->device_id), __pi_dmacpy_handler,
                     SOC_EVENT_UDMA_DMACPY_TX(fifo->device_id), __pi_dmacpy_handler);
        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_DMACPY_RX(fifo->device_id), __pi_dmacpy_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_DMACPY_TX(fifo->device_id), __pi_dmacpy_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_DMACPY_RX(fifo->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_DMACPY_TX(fifo->device_id));

        /* Disable UDMA CG and reset periph. */
        udma_ctrl_cg_disable(UDMA_DMACPY_ID(fifo->device_id));
        DMACPY_TRACE("Disable CG for dmacpy(%ld): %ld %p %lx\n",
                     fifo->device_id, UDMA_DMACPY_ID(fifo->device_id),
                     &UDMA_GC->CG, UDMA_GC->CG);
    }
    else
    {
        fifo->nb_open++;
        DMACPY_TRACE("Device id=%x already opened, now open=%d\n",
                     fifo->device_id, fifo->nb_open);
    }
    return 0;
}

void __pi_dmacpy_close(uint32_t device_id)
{
    struct dmacpy_driver_fifo_s *fifo = g_dmacpy_driver_fifo[device_id];
    if (fifo != NULL)
    {
        /* Decrement number of devices opened. */
        fifo->nb_open--;
        /* Free device and structure opened. */
        if (fifo->nb_open == 0)
        {
            /* Clear handlers. */
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_DMACPY_RX(fifo->device_id));
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_DMACPY_TX(fifo->device_id));
            /* Disable SOC events propagation to FC. */
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_DMACPY_RX(fifo->device_id));
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_DMACPY_TX(fifo->device_id));

            /* Free allocated fifo. */
            pi_l2_free(g_dmacpy_driver_fifo[device_id],
                       sizeof(struct dmacpy_driver_fifo_s));
            g_dmacpy_driver_fifo[device_id] = NULL;
        }
    }
}

int32_t __pi_dmacpy_copy(uint32_t device_id, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task)
{
    /* Src address not aligned on 4 bytes. */
    if (((uint32_t) src % 4) != 0)
    {
        return -11;
    }
    /* Dst address not aligned on 4 bytes. */
    if (((uint32_t) dst % 4) != 0)
    {
        return -12;
    }
    /* Size not a multiple of 4 bytes. */
    if (((uint32_t) size % 4) != 0)
    {
        return -13;
    }
    task->data[0] = (uint32_t) src;
    task->data[1] = (uint32_t) dst;
    task->data[2] = size;
    task->data[3] = dir;
    task->data[4] = 0;
    uint32_t irq = disable_irq();
    struct dmacpy_driver_fifo_s *fifo = g_dmacpy_driver_fifo[device_id];
    uint8_t head = __pi_dmacpy_task_fifo_enqueue(fifo, task);
    if (head == 0)
    {
        /* Execute the transfer. */
        DMACPY_TRACE("Execute transfer now.\n");
        __pi_dmacpy_copy_exec(fifo, task);
    }
    else
    {
        DMACPY_TRACE("Task enqueued in list.\n");
    }
    restore_irq(irq);
    return 0;
}
