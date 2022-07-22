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

struct dmacpy_itf_data_s *g_dmacpy_itf_data[UDMA_NB_DMACPY] = {NULL};

static uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* IRQ handler. */
static void __pi_dmacpy_handler(void *arg);

/* Enqueue a task in fifo. */
static uint8_t __pi_dmacpy_task_fifo_enqueue(struct dmacpy_itf_data_s *fifo,
                                             struct pi_task *task);

/* Pop a task from fifo. */
static struct pi_task *__pi_dmacpy_task_fifo_pop(struct dmacpy_itf_data_s *fifo);

/* Execute transfer. */
static void __pi_dmacpy_copy_exec(struct dmacpy_itf_data_s *fifo,
                                  struct pi_task *task);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/
/* IRQ handler. */
static void __pi_dmacpy_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_DMACPY_ID(0);
    DMACPY_TRACE("DMACPY(%d) IRQ %d.\n", periph_id, event);

    struct dmacpy_itf_data_s *driver_data = g_dmacpy_itf_data[periph_id];
    struct pi_task *task = driver_data->fifo_head;
    /* Pending data on current transfer. */
    if ((task->data[5]) && (channel == TX_CHANNEL))
    {
        DMACPY_TRACE("Current transfer is L2 to L2, received TX event, do nothing, continue.\n");
        return;
    }
    if (task->data[4] != 0)
    {
        DMACPY_TRACE("Reenqueue pending data on current transfer.\n");
        __pi_dmacpy_copy_exec(driver_data, task);
    }
    else
    {
        DMACPY_TRACE("No pending data on current transfer.\n Handle end of "
                     "current transfer and pop and start a new transfer if there is.\n");
        task = __pi_dmacpy_task_fifo_pop(driver_data);
        __pi_irq_handle_end_of_task(task);
        task = driver_data->fifo_head;
        if (task != NULL)
        {
            __pi_dmacpy_copy_exec(driver_data, task);
        }
    }
}

/* Enqueue a task in fifo. */
static uint8_t __pi_dmacpy_task_fifo_enqueue(struct dmacpy_itf_data_s *driver_data,
                                             struct pi_task *task)
{
    uint8_t head = 0;
    uint32_t irq = __disable_irq();
    if (driver_data->fifo_head == NULL)
    {
        /* Transfer on going, enqueue this one to the list. */
        /* Empty fifo. */
        driver_data->fifo_head = task;
        driver_data->fifo_tail = driver_data->fifo_head;
        head = 0;
        DMACPY_TRACE("Enqueue task to head.\n");
    }
    else
    {
        /* Enqueue the transfer to list of pending transfers. */
        driver_data->fifo_tail->next = task;
        driver_data->fifo_tail = driver_data->fifo_tail->next;
        head = 1;
        DMACPY_TRACE("Enqueue task to tail.\n");
    }
    __restore_irq(irq);
    return head;
}

/* Pop a task from fifo. */
static struct pi_task *__pi_dmacpy_task_fifo_pop(struct dmacpy_itf_data_s *driver_data)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    /* Pop the first task and return the next transfer if there are any. */
    if (driver_data->fifo_head != NULL)
    {
        task_to_return = driver_data->fifo_head;
        driver_data->fifo_head = driver_data->fifo_head->next;
        if (driver_data->fifo_head == NULL)
        {
            driver_data->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return task_to_return;
}

/* Execute transfer. */
static void __pi_dmacpy_copy_exec(struct dmacpy_itf_data_s *driver_data,
                                  struct pi_task *task)
{
    uint32_t l2_buf_0 = task->data[0]; /* RX_SADDR for FC->L2 or L2->L2. */
    uint32_t l2_buf_1 = task->data[1]; /* TX_SADDR for L2->FC or L2->L2. */
    uint32_t cpy_src  = task->data[2]; /* SRC for FC->L2. */
    uint32_t cpy_dst  = task->data[3]; /* DST for L2->FC. */
    uint32_t cpy_size = task->data[4]; /* Size. */
    uint32_t dir      = task->data[5]; /* Copy from/to TCDM? */

    if (task->data[4] > max_size)
    {
        cpy_size = max_size;
    }
    task->data[4] -= cpy_size;
    DMACPY_TRACE("DMACPY(%ld): remaining size: %ld\n", driver_data->device_id, task->data[4]);
    if (dir)
    {
        task->data[0] += cpy_size;
        task->data[1] += cpy_size;
        DMACPY_TRACE("DMACPY(%ld): enqueue FC to L2 l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     driver_data->device_id, l2_buf_0, cpy_src, cpy_dst, cpy_size, dir, RX_CHANNEL);
        hal_dmacpy_enqueue(driver_data->device_id, l2_buf_0, cpy_size, 0,
                           cpy_src, cpy_dst, dir, RX_CHANNEL);
        DMACPY_TRACE("DMACPY(%ld): enqueue L2 to FC l2_buf=%lx src=%lx dst=%lx "
                     "size=%ld mem_sel=%ld channel=%ld\n",
                     driver_data->device_id, l2_buf_1, cpy_src, cpy_dst, cpy_size, dir, TX_CHANNEL);
        hal_dmacpy_enqueue(driver_data->device_id, l2_buf_1, cpy_size, 0,
                           cpy_src, cpy_dst, dir, TX_CHANNEL);
    }
    else
    {
        if (cpy_src)
        {
            task->data[0] += cpy_size;
            task->data[2] += cpy_size;
            DMACPY_TRACE("DMACPY(%ld): enqueue FC to L2 l2_buf=%lx src=%lx dst=%lx "
                         "size=%ld mem_sel=%ld channel=%ld\n",
                         driver_data->device_id, l2_buf_0, cpy_src, cpy_dst, cpy_size,
                         dir, RX_CHANNEL);
            hal_dmacpy_enqueue(driver_data->device_id, l2_buf_0, cpy_size, 0,
                               cpy_src, cpy_dst, dir, RX_CHANNEL);
        }
        else
        {
            task->data[1] += cpy_size;
            task->data[3] += cpy_size;
            DMACPY_TRACE("DMACPY(%ld): enqueue L2 to FC l2_buf=%lx src=%lx dst=%lx "
                         "size=%ld mem_sel=%ld channel=%ld\n",
                         driver_data->device_id, l2_buf_1, cpy_src, cpy_dst, cpy_size,
                         dir, TX_CHANNEL);
            hal_dmacpy_enqueue(driver_data->device_id, l2_buf_1, cpy_size, 0,
                               cpy_src, cpy_dst, dir, TX_CHANNEL);
        }
    }
    DMACPY_TRACE("Enqueue DMACPY(%d): %x %x %x %x %x %x %x %x %x\n",
                 driver_data->device_id,
                 dmacpy(driver_data->device_id)->udma.rx_saddr,
                 dmacpy(driver_data->device_id)->udma.rx_size,
                 dmacpy(driver_data->device_id)->udma.rx_cfg,
                 dmacpy(driver_data->device_id)->udma.tx_saddr,
                 dmacpy(driver_data->device_id)->udma.tx_size,
                 dmacpy(driver_data->device_id)->udma.tx_cfg,
                 dmacpy(driver_data->device_id)->dst_addr,
                 dmacpy(driver_data->device_id)->src_addr,
                 dmacpy(driver_data->device_id)->mem_sel);
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf)
{
    conf->id = 0;
}

int32_t __pi_dmacpy_open(struct pi_dmacpy_conf *conf,
                         struct dmacpy_itf_data_s **device_data)
{
    struct dmacpy_itf_data_s *driver_data = g_dmacpy_itf_data[conf->id];
    if (driver_data == NULL)
    {
        driver_data = (struct dmacpy_itf_data_s *) pi_l2_malloc(sizeof(struct dmacpy_itf_data_s));
        if (driver_data == NULL)
        {
            DMACPY_TRACE_ERR("Driver fifo alloc failed !\n");
            return -11;
        }
        driver_data->fifo_head = NULL;
        driver_data->fifo_tail = NULL;
        driver_data->device_id = conf->id;
        driver_data->nb_open   = 1;

        /* Open device for first time. */
        DMACPY_TRACE("Device id=%ld opened for first time\n", driver_data->device_id);
        DMACPY_TRACE("dmacpy(%ld) %p\n", driver_data->device_id, dmacpy(driver_data->device_id));

        DMACPY_TRACE("Enable SoC events and set handlers : RX : %d -> %p | TX: %d -> %p\n",
                     SOC_EVENT_UDMA_DMACPY_RX(driver_data->device_id), __pi_dmacpy_handler,
                     SOC_EVENT_UDMA_DMACPY_TX(driver_data->device_id), __pi_dmacpy_handler);
        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_DMACPY_RX(driver_data->device_id), __pi_dmacpy_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_DMACPY_TX(driver_data->device_id), __pi_dmacpy_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_DMACPY_RX(driver_data->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_DMACPY_TX(driver_data->device_id));

        /* Disable UDMA CG and reset periph. */
        udma_ctrl_cg_disable(UDMA_DMACPY_ID(driver_data->device_id));
        DMACPY_TRACE("Disable CG for dmacpy(%ld): %ld %p %lx\n",
                     driver_data->device_id, UDMA_DMACPY_ID(driver_data->device_id),
                     &UDMA_GC->CG, UDMA_GC->CG);

        /* Attach device data info to data. */
        g_dmacpy_itf_data[driver_data->device_id] = driver_data;
        *device_data = g_dmacpy_itf_data[driver_data->device_id];
    }
    else
    {
        driver_data->nb_open++;
        DMACPY_TRACE("Device id=%x already opened, now open=%d\n",
                     driver_data->device_id, driver_data->nb_open);
    }
    return 0;
}

void __pi_dmacpy_close(uint32_t device_id)
{
    struct dmacpy_itf_data_s *driver_data = g_dmacpy_itf_data[device_id];
    if (driver_data != NULL)
    {
        /* Decrement number of devices opened. */
        driver_data->nb_open--;
        /* Free device and structure opened. */
        if (driver_data->nb_open == 0)
        {
            /* Clear handlers. */
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_DMACPY_RX(driver_data->device_id));
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_DMACPY_TX(driver_data->device_id));
            /* Disable SOC events propagation to FC. */
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_DMACPY_RX(driver_data->device_id));
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_DMACPY_TX(driver_data->device_id));

            /* Free allocated fifo. */
            pi_l2_free(g_dmacpy_itf_data[device_id],
                       sizeof(struct dmacpy_itf_data_s));
            g_dmacpy_itf_data[device_id] = NULL;
        }
    }
}

int32_t __pi_dmacpy_copy(uint32_t device_id, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task)
{
    /* Src address not aligned on 4 bytes. */
    if (((uint32_t) src % 4) != 0)
    {
        DMACPY_TRACE_ERR("Error : src must be aligned on 4 bytes : %lx !\n", (uint32_t) src);
        return -11;
    }
    /* Dst address not aligned on 4 bytes. */
    if (((uint32_t) dst % 4) != 0)
    {
        DMACPY_TRACE_ERR("Error : dst must be aligned on 4 bytes : %lx !\n", (uint32_t) dst);
        return -12;
    }
    /* Size not a multiple of 4 bytes. */
    if (((uint32_t) size % 4) != 0)
    {
        DMACPY_TRACE_ERR("Error : size must be aligned on 4 bytes : %lx !\n", size);
        return -13;
    }

    task->data[0] = 0;
    task->data[1] = 0;
    task->data[2] = (uint32_t) src;
    task->data[3] = (uint32_t) dst;
    task->data[4] = size;
    task->data[5] = 0;

    switch (dir)
    {
    case PI_DMACPY_FC_L1_L2 :
        task->data[0] = (uint32_t) dst;
        task->data[3] = 0;
        break;

    case PI_DMACPY_L2_FC_L1 :
        task->data[1] = (uint32_t) src;
        task->data[2] = 0;
        break;

    case PI_DMACPY_L2_L2 :
        task->data[0] = (uint32_t) dst;
        task->data[1] = (uint32_t) src;
        task->data[2] = 0;
        task->data[3] = 0;
        task->data[5] = 1;
        break;

    default :
        DMACPY_TRACE_ERR("Wrong param for direction !\n");
        return -14;
    }

    uint32_t irq = disable_irq();
    struct dmacpy_itf_data_s *driver_data = g_dmacpy_itf_data[device_id];
    uint8_t head = __pi_dmacpy_task_fifo_enqueue(driver_data, task);
    if (head == 0)
    {
        /* Execute the transfer. */
        DMACPY_TRACE("Execute transfer now.\n");
        __pi_dmacpy_copy_exec(driver_data, task);
    }
    else
    {
        DMACPY_TRACE("Task enqueued in list.\n");
    }
    restore_irq(irq);
    return 0;
}
