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
#include "pmsis/implem/drivers/udma/cpi/cpi_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define UDMA_DATA_MAX_SIZE 0x1FFFF

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct cpi_itf_data_s *g_cpi_itf_data[UDMA_NB_CPI] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* End of task handler, release/execute user callback. */
static void __pi_cpi_handle_end_of_task(struct pi_task *task);

/* IRQ handler. */
static void __pi_cpi_handler(void *arg);

/* Enqueue a new transfer in SW fifo. */
static uint32_t __pi_cpi_task_fifo_enqueue(struct cpi_itf_data_s *driver_data,
                                           struct pi_task *task);

/* Pop a transfer from SW fifo. */
static struct pi_task *__pi_cpi_task_fifo_pop(struct cpi_itf_data_s *driver_data);

/* Enqueue a transfer in UDMA. */
static void __pi_cpi_copy_exec(struct cpi_itf_data_s *driver_data,
                               struct pi_task *task);

/*******************************************************************************
 * Internal function implementation
 ******************************************************************************/

static void __pi_cpi_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        pi_task_push(task);
    }
}

static void __pi_cpi_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_CPI_ID(0);
    CPI_TRACE("CPI IRQ %ld %ld\n", periph_id, event);

    struct cpi_itf_data_s *driver_data = g_cpi_itf_data[periph_id];
    struct pi_task *task = NULL;
    task = driver_data->fifo_head;
    /* Pending data on current transfer ? */
    if (task->data[2] != 0)
    {
        CPI_TRACE("Reenqueue pending transfer.\n");
        task->data[0] += task->data[1]; /* Buffer + repeat_size. */
        task->data[2] -= task->data[1]; /* Size - repeat_size. */
        uint32_t iter_size = task->data[1];
        if (task->data[2] <= task->data[1])
        {
            iter_size = task->data[2];
            task->data[2] = 0;
        }
        uint32_t cfg = (UDMA_CORE_RX_CFG_DATASIZE(16 >> 4) | UDMA_CORE_RX_CFG_EN(1));
        hal_cpi_enqueue(driver_data->device_id, RX_CHANNEL, task->data[0],
                        iter_size, cfg);
    }
    else
    {
        if (task->data[3])
        {
            task->data[3] = 0;
            return;
        }
        CPI_TRACE("No pending transfer, pop current task and handle it.\n");
        task = __pi_cpi_task_fifo_pop(driver_data);
        __pi_cpi_handle_end_of_task(task);
        task = driver_data->fifo_head;
        if (task != NULL)
        {
            __pi_cpi_copy_exec(driver_data, task);
        }
    }
}

/* Enqueue a task in fifo. */
static uint32_t __pi_cpi_task_fifo_enqueue(struct cpi_itf_data_s *driver_data,
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
    }
    else
    {
        /* Execute the transfer. */
        driver_data->fifo_tail->next = task;
        driver_data->fifo_tail = driver_data->fifo_tail->next;
        head = 1;
    }
    __restore_irq(irq);
    return head;
}

/* Pop a task from fifo. */
static struct pi_task *__pi_cpi_task_fifo_pop(struct cpi_itf_data_s *driver_data)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
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

/* Enqueue a transfer in UDMA. */
static void __pi_cpi_copy_exec(struct cpi_itf_data_s *driver_data,
                               struct pi_task *task)
{
    uint32_t device_id = driver_data->device_id;
    uint32_t buffer = task->data[0];
    uint32_t iter_size = task->data[1];
    uint32_t cfg = (UDMA_CORE_RX_CFG_DATASIZE(16 >> 4) | UDMA_CORE_RX_CFG_EN(1));
    uint32_t max_size = (uint32_t) UDMA_DATA_MAX_SIZE;

    if (iter_size > max_size)
    {
        iter_size = max_size;
        uint32_t sec_iter_size = task->data[1] - max_size;
        if (sec_iter_size > max_size)
        {
            sec_iter_size = max_size;
        }
        uint32_t sec_iter_buffer = task->data[0] + max_size;
        task->data[0] += (max_size /* + sec_iter_size */); /* Buffer + max_size for second iter. */
        task->data[2] = task->data[1] - (max_size + sec_iter_size); /* Remaining size. */
        if ((int32_t)task->data[2] < 0 )
        {
            task->data[2] = 0;
        }
        task->data[1] = max_size; /* Size of each slice. */
        task->data[3] = 1;

        CPI_TRACE("CPI(%ld) enqueue double transfer 1) %lx %ld %lx  1) %lx %ld %lx\n",
                  device_id, buffer, iter_size, cfg,
                  sec_iter_buffer , sec_iter_size, cfg);
        hal_cpi_enqueue(device_id, RX_CHANNEL, buffer, iter_size, cfg);
        hal_cpi_enqueue(device_id, RX_CHANNEL, sec_iter_buffer, sec_iter_size, cfg);
    }
    else
    {
        CPI_TRACE("CPI(%ld) enqueue one transfer %lx %ld %lx\n",
                  device_id, buffer, iter_size, cfg);
        hal_cpi_enqueue(device_id, RX_CHANNEL, buffer, iter_size, cfg);
    }
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_cpi_conf_init(struct pi_cpi_conf *conf)
{
    conf->device = PI_DEVICE_CPI_TYPE;
    conf->itf = 0;
}

int32_t __pi_cpi_open(struct pi_cpi_conf *conf, struct cpi_itf_data_s **device_data)
{
    struct cpi_itf_data_s *driver_data = g_cpi_itf_data[conf->itf];
    /* Allocate driver data. */
    if (driver_data == NULL)
    {
        /* Open device for first time. */
        driver_data = (struct cpi_itf_data_s *) pi_default_malloc(sizeof(struct cpi_itf_data_s));
        if (driver_data == NULL)
        {
            CPI_TRACE_ERR("Driver data alloc failed !\n");
            return -11;
        }
        driver_data->fifo_head = NULL;
        driver_data->fifo_tail = NULL;
        driver_data->device_id = conf->itf;
        driver_data->nb_open = 1;

        CPI_TRACE("Device id=%ld opened for first time\n", driver_data->device_id);

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_CPI_RX(conf->itf), __pi_cpi_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_CPI_RX(conf->itf));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_CPI_ID(conf->itf));

        /* Attach device data info to data. */
        g_cpi_itf_data[driver_data->device_id] = driver_data;
        *device_data = g_cpi_itf_data[driver_data->device_id];
    }
    else
    {
        driver_data->nb_open++;
        *device_data = driver_data;
        CPI_TRACE("Device id=%ld already opened, now open=%ld\n",
                  driver_data->device_id, driver_data->nb_open);
    }
    return 0;
}

void __pi_cpi_close(struct cpi_itf_data_s *device_data)
{
    struct cpi_itf_data_s *driver_data = g_cpi_itf_data[device_data->device_id];
    /* Decrement number of devices opened. */
    device_data->nb_open--;
    /* Free device and structure opened. */
    if (device_data->nb_open == 0)
    {
        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_CPI_RX(device_data->device_id));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_CPI_RX(device_data->device_id));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_CPI_ID(device_data->device_id));

        /* Free allocated driver data. */
        g_cpi_itf_data[device_data->device_id] = NULL;
        pi_default_free(device_data, sizeof(struct cpi_itf_data_s));
    }
}

void __pi_cpi_copy(struct cpi_itf_data_s *device_data, void *l2_buf,
                   uint32_t size, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->data[0] = (uint32_t) l2_buf; /* l2 buffer. */
    task->data[1] = size;              /* size. */
    task->data[2] = 0;                 /* pending ? */
    task->data[3] = 0;                 /* resume ? */
    task->next    = NULL;
    uint32_t head = __pi_cpi_task_fifo_enqueue(device_data, task);
    if (head == 0)
    {
        /* Execute the transfer. */
        CPI_TRACE("CPI(%ld) execute transfer now\n", device_data->device_id);
        __pi_cpi_copy_exec(device_data, task);
    }
    __restore_irq(irq);
}
