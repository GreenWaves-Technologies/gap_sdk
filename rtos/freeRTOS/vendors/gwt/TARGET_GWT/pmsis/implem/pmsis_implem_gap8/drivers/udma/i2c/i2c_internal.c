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
#include "pmsis/implem/drivers/udma/i2c/i2c_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Defines for read & write adress access. */
#define ADDRESS_WRITE 0x0
#define ADDRESS_READ  0x1

/* Max length of a i2c request/data buffer. */
#define MAX_SIZE               (0xFF)

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static struct i2c_itf_data_s *g_i2c_itf_data[UDMA_NB_I2C] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* IRQ handler. */
static void __pi_i2c_handler(void *arg);

/* Clock divider. */
static uint32_t __pi_i2c_clk_div_get(uint32_t baudrate);

/* Add a cs_data to list of opened devices. */
static void __pi_i2c_cs_data_add(struct i2c_itf_data_s *driver_data,
                                 struct i2c_cs_data_s *cs_data);

/* Remove a cs_data from list of opened devices. */
static void __pi_i2c_cs_data_remove(struct i2c_itf_data_s *driver_data,
                                    struct i2c_cs_data_s *cs_data);

/* Handle a pending transfer after end of previous part of transfer. */
static void __pi_i2c_handle_pending_transfer(struct i2c_itf_data_s *driver_data);

/* Send a stop command sequence. */
static void __pi_i2c_send_stop_cmd(struct i2c_itf_data_s *driver_data);


/*
 * Check if both RX & TX channels are empty, needed to make sure there are
 * no collision between a write req and a read req(which uses TX).
 */
static int32_t __pi_i2c_hw_fifo_both_empty(struct i2c_itf_data_s *driver_data);

/* Check if a HW UDMA slot is free. */
static int32_t __pi_i2c_hw_fifo_empty(struct i2c_itf_data_s *driver_data,
                                      udma_channel_e channel);

/* Enqueue a new task in HW fifo for callback. */
static void __pi_i2c_hw_fifo_enqueue(struct i2c_itf_data_s *driver_data,
                                     struct pi_task *task,
                                     udma_channel_e channel);

/* Pop a task from HW fifo. */
static struct pi_task *__pi_i2c_hw_fifo_pop(struct i2c_itf_data_s *driver_data,
                                            udma_channel_e channel);

/* Create a new callabck struct with transfer info then enqueue it in SW fifo. */
static void __pi_i2c_task_fifo_enqueue(struct i2c_itf_data_s *driver_data,
                                       struct pi_task *task);

/* Pop a callback struct containing a new transfer from SW fifo. */
static struct pi_task *__pi_i2c_task_fifo_pop(struct i2c_itf_data_s *driver_data);

/* Initiate and enqueue a read command sequence. */
static void __pi_i2c_copy_exec_read(struct i2c_itf_data_s *driver_data,
                                    struct pi_task *task);

/* Initiate and enqueue a write command sequence. */
static void __pi_i2c_copy_exec_write(struct i2c_itf_data_s *driver_data,
                                     struct pi_task *task);

/* Callback to execute when frequency changes. */
static void __pi_i2c_freq_cb(void *args);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_i2c_handle_pending_transfer(struct i2c_itf_data_s *driver_data)
{
    struct i2c_pending_transfer_s *pending = driver_data->pending;
    pending->pending_buffer += pending->pending_repeat;
    pending->pending_repeat_size -= pending->pending_repeat;
    pending->pending_size = pending->pending_repeat;

    if (pending->pending_repeat_size <= pending->pending_repeat)
    {
        pending->pending_repeat = 0;
        pending->pending_size = pending->pending_repeat_size;
        /* Stop bit at the end? */
        driver_data->i2c_stop_send = (pending->flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
    }
    /* Initiate next part of command sequence. */
    {
        /* Data. */
        uint32_t index = 0;
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_RPT;
        driver_data->i2c_cmd_seq[index++] = pending->pending_size;
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_WR;
    }
    //hal_i2c_enqueue(device_id, driver_data->channel);
    /* TODO: Renqueue next cmd! */
}

static void __pi_i2c_send_stop_cmd(struct i2c_itf_data_s *driver_data)
{
    /* Send stop command. */
    driver_data->i2c_stop_send = 0;
    hal_i2c_enqueue(driver_data->device_id, TX_CHANNEL, (uint32_t) driver_data->i2c_stop_seq,
                    (uint32_t) __PI_I2C_STOP_CMD_SIZE, UDMA_CORE_TX_CFG_EN(1));
}

static void __pi_i2c_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2C_ID(0);

    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[periph_id];
    /*
     * In case of a read command sequence, TX ends first then wait on RX.
     * Until then, no other transaction should occur.
     * Check if driver_data->hw_buffer[channel] != NULL.
     * If NULL -> read command, so wait until RX event.
     * Else -> write command, pop associated task.
     * When receiving the first event, it should not be handled.
     * Need to check again : WHY ?????
     */
    driver_data->nb_events++;
    if (driver_data->nb_events == 1)
    {
        return;
    }
    if (driver_data->hw_buffer[channel] != NULL)
    {
        /* Pending transfer. */
        if (driver_data->pending->pending_repeat)
        {
            __pi_i2c_handle_pending_transfer(driver_data);
        }
        else
        {
            if (driver_data->i2c_stop_send)
            {
                if (channel == RX_CHANNEL)
                {
                    driver_data->hw_buffer[TX_CHANNEL] = driver_data->hw_buffer[RX_CHANNEL];
                    driver_data->hw_buffer[RX_CHANNEL] = NULL;
                }
                __pi_i2c_send_stop_cmd(driver_data);
            }
            else
            {
                struct pi_task *task = __pi_i2c_hw_fifo_pop(driver_data, channel);
                if (task != NULL)
                {
                    __pi_irq_handle_end_of_task(task);
                }
                task = __pi_i2c_task_fifo_pop(driver_data);
                if (task != NULL)
                {
                    /* Enqueue transfer in HW fifo. */
                    if (task->data[3] == RX_CHANNEL)
                    {
                        __pi_i2c_copy_exec_read(driver_data, task);
                    }
                    else
                    {
                        __pi_i2c_copy_exec_write(driver_data, task);
                    }
                }
                driver_data->nb_events = 0;
            }
        }
    }
}

static int32_t __pi_i2c_hw_fifo_both_empty(struct i2c_itf_data_s *driver_data)
{
    int32_t rx_empty = __pi_i2c_hw_fifo_empty(driver_data, RX_CHANNEL);
    int32_t tx_empty = __pi_i2c_hw_fifo_empty(driver_data, TX_CHANNEL);
    return  ((rx_empty == 1) && (tx_empty == 1));
}

static int32_t __pi_i2c_hw_fifo_empty(struct i2c_itf_data_s *driver_data,
                                      udma_channel_e channel)
{
    return ((driver_data->hw_buffer[channel] == NULL) ? 1 : 0);
}

static void __pi_i2c_hw_fifo_enqueue(struct i2c_itf_data_s *driver_data,
                                     struct pi_task *task,
                                     udma_channel_e channel)
{
    uint32_t irq = __disable_irq();
    /* Enqueue task in hw_buffer[channel] to signal the slot is used. */
    driver_data->hw_buffer[channel] = task;
    __restore_irq(irq);
}

static struct pi_task *__pi_i2c_hw_fifo_pop(struct i2c_itf_data_s *driver_data,
                                            udma_channel_e channel)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    task_to_return = driver_data->hw_buffer[channel];
    /* Free the slot for another transfer. */
    driver_data->hw_buffer[channel] = NULL;
    __restore_irq(irq);
    return task_to_return;
}

static void __pi_i2c_task_fifo_enqueue(struct i2c_itf_data_s *driver_data,
                                       struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    /* Enqueue transfer in SW fifo. */
    if (driver_data->fifo_head == NULL)
    {
        driver_data->fifo_head = task;
    }
    else
    {
        driver_data->fifo_tail->next = task;
    }
    driver_data->fifo_tail = task;
    __restore_irq(irq);
}

static struct pi_task *__pi_i2c_task_fifo_pop(struct i2c_itf_data_s *driver_data)
{
    struct pi_task *task_to_return = NULL;
    uint32_t irq = __disable_irq();
    if (driver_data->fifo_head != NULL)
    {
        task_to_return = driver_data->fifo_head;
        driver_data->fifo_head = driver_data->fifo_head->next;
    }
    __restore_irq(irq);
    return task_to_return;
}

static uint32_t __pi_i2c_clk_div_get(uint32_t i2c_freq)
{
    /* Clock divided by 4 in HW. */
    uint32_t freq = i2c_freq << 2;
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    uint32_t div = (periph_freq + freq - 1) / freq;
    /* Clock divider counts from 0 to clk_div value included. */
    if (div <= 1)
    {
        div = 0;
    }
    else
    {
        div -= 1;
    }
    if (div > 0xFFFF)
    {
        I2C_TRACE_ERR("Error computing clock divier : Fsoc=%ld, Fi2c=%ld\n",
                      periph_freq, i2c_freq);
        return 0xFFFFFFFF;
    }
    return div;
}

static void __pi_i2c_copy_exec_read(struct i2c_itf_data_s *driver_data,
                                    struct pi_task *task)
{
    uint32_t index = 0, start_bit = 0, stop_bit = 0;
    uint32_t buffer = task->data[0];
    uint32_t size = task->data[1];
    uint32_t flags = task->data[2];
    uint32_t channel = task->data[3];
    struct i2c_cs_data_s *cs_data = (struct i2c_cs_data_s *) task->data[4];
    if (size > 0)
    {
        /* Header. */
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_CFG;
        driver_data->i2c_cmd_seq[index++] = ((cs_data->clk_div >> 8) & 0xFF);
        driver_data->i2c_cmd_seq[index++] = (cs_data->clk_div & 0xFF);
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_START;
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_WR;
        driver_data->i2c_cmd_seq[index++] = (cs_data->cs | ADDRESS_READ);

        struct i2c_pending_transfer_s *pending = driver_data->pending;
        if (size > (uint32_t) MAX_SIZE)
        {
            pending->pending_buffer = buffer;
            pending->pending_repeat = (uint32_t) MAX_SIZE;
            pending->pending_repeat_size = size;
            //pending->device_id = driver_data->device_id;
            pending->flags = flags;
            pending->channel = channel;
            size = (uint32_t) MAX_SIZE;
        }
        else
        {
            pending->pending_repeat = 0;
            /* Stop bit at then end? */
            driver_data->i2c_stop_send = (flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
        }
        /* Data. */
        if (size > 1)
        {
            driver_data->i2c_cmd_seq[index++] = I2C_CMD_RPT;
            driver_data->i2c_cmd_seq[index++] = size - 1;
            driver_data->i2c_cmd_seq[index++] = I2C_CMD_RD_ACK;
        }
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_RD_NACK;

        /* Enqueue in HW fifo. */
        __pi_i2c_hw_fifo_enqueue(driver_data, task, RX_CHANNEL);

        /* Open RX channel to receive data. */
        hal_i2c_enqueue(driver_data->device_id, RX_CHANNEL, buffer, size,
                        UDMA_CORE_RX_CFG_EN(1));
        /* Transfer command. */
        hal_i2c_enqueue(driver_data->device_id, TX_CHANNEL, (uint32_t) driver_data->i2c_cmd_seq,
                        index, UDMA_CORE_TX_CFG_EN(1));
    }
}

static void __pi_i2c_copy_exec_write(struct i2c_itf_data_s *driver_data,
                                     struct pi_task *task)
{
    uint32_t index = 0, start_bit = 0, stop_bit = 0;
    uint32_t buffer = task->data[0];
    uint32_t size = task->data[1];
    uint32_t flags = task->data[2];
    uint32_t channel = task->data[3];
    struct i2c_cs_data_s *cs_data = (struct i2c_cs_data_s *) task->data[4];
    if (size > 0)
    {
        start_bit = flags & PI_I2C_XFER_NO_START;

        /* Header. */
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_CFG;
        driver_data->i2c_cmd_seq[index++] = ((cs_data->clk_div >> 8) & 0xFF);
        driver_data->i2c_cmd_seq[index++] = (cs_data->clk_div & 0xFF);
        if (!start_bit)
        {
            driver_data->i2c_cmd_seq[index++] = I2C_CMD_START;
            driver_data->i2c_cmd_seq[index++] = I2C_CMD_WR;
            driver_data->i2c_cmd_seq[index++] = (cs_data->cs | ADDRESS_WRITE);
        }
        struct i2c_pending_transfer_s *pending = driver_data->pending;
        if (size > (uint32_t) MAX_SIZE)
        {
            pending->pending_buffer = buffer;
            pending->pending_repeat = (uint32_t) MAX_SIZE;
            pending->pending_repeat_size = size;
            //pending->device_id = driver_data->device_id;
            pending->flags = flags;
            pending->channel = channel;
            size = (uint32_t) MAX_SIZE;
        }
        else
        {
            pending->pending_repeat = 0;
            /* Stop bit at the end? */
            driver_data->i2c_stop_send = (flags & PI_I2C_XFER_NO_STOP) ? 0 : 1;
        }
        /* Data. */
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_RPT;
        driver_data->i2c_cmd_seq[index++] = size;
        driver_data->i2c_cmd_seq[index++] = I2C_CMD_WR;

        /* Enqueue in HW fifo. */
        __pi_i2c_hw_fifo_enqueue(driver_data, task, TX_CHANNEL);

        /* Transfer header. */
        hal_i2c_enqueue(driver_data->device_id, TX_CHANNEL, (uint32_t) driver_data->i2c_cmd_seq,
                        index, UDMA_CORE_TX_CFG_EN(1));
        /* Transfer data. */
        hal_i2c_enqueue(driver_data->device_id, TX_CHANNEL, buffer, size,
                        UDMA_CORE_TX_CFG_EN(1));
    }
}

static void __pi_i2c_cs_data_add(struct i2c_itf_data_s *driver_data,
                                 struct i2c_cs_data_s *cs_data)
{
    struct i2c_cs_data_s *head = driver_data->cs_list;
    while (head != NULL)
    {
        head = head->next;
    }
    head->next = cs_data;
}

static void __pi_i2c_cs_data_remove(struct i2c_itf_data_s *driver_data,
                                    struct i2c_cs_data_s *cs_data)
{
    struct i2c_cs_data_s *head = driver_data->cs_list;
    struct i2c_cs_data_s *prev = driver_data->cs_list;
    while ((head != NULL) && (head != cs_data))
    {
        prev = head;
        hal_compiler_barrier();
        head = head->next;
    }
    if (head != NULL)
    {
        prev->next = head->next;
    }
}

static void __pi_i2c_freq_cb(void *args)
{
    uint32_t irq = __disable_irq();
    struct i2c_itf_data_s *driver_data = (struct i2c_itf_data_s *) args;
    uint32_t device_id = driver_data->device_id;
    struct i2c_cs_data_s *cs_data = driver_data->cs_list;

    /* Wait until current transfer is done. */
    while (hal_i2c_busy_get(device_id));

    /* Update all clock div. */
    while (cs_data != NULL)
    {
        cs_data->clk_div = __pi_i2c_clk_div_get(cs_data->max_baudrate);
        cs_data = cs_data->next;
    }
    __restore_irq(irq);
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_i2c_conf_init(pi_i2c_conf_t *conf)
{
    conf->device = PI_DEVICE_I2C_TYPE;
    conf->cs = 0;
    conf->max_baudrate = 200000;
    conf->itf = 0;
}

int32_t __pi_i2c_open(struct pi_i2c_conf *conf, struct i2c_cs_data_s **device_data)
{
    if ((uint8_t) UDMA_NB_I2C < conf->itf)
    {
        I2C_TRACE_ERR("Error : wrong interface ID, itf=%d !\n", conf->itf);
        return -11;
    }

    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[conf->itf];
    if (driver_data == NULL)
    {
        /* Allocate driver data. */
        driver_data = (struct i2c_itf_data_s *) pi_l2_malloc(sizeof(struct i2c_itf_data_s));
        if (driver_data == NULL)
        {
            I2C_TRACE_ERR("Driver data alloc failed !\n");
            return -12;
        }
        driver_data->hw_buffer[0] = NULL;
        driver_data->hw_buffer[1] = NULL;
        driver_data->fifo_head = NULL;
        driver_data->fifo_tail = NULL;
        driver_data->pending = NULL;
        driver_data->nb_open = 0;
        driver_data->i2c_cmd_index = 0;
        driver_data->cs_list = NULL;
        for (uint32_t i=0; i<(uint32_t) __PI_I2C_CMD_BUFF_SIZE; i++)
        {
            driver_data->i2c_cmd_seq[i] = 0;
        }
        driver_data->i2c_stop_send = 0;
        /* Set up i2c cmd stop sequence. */
        driver_data->i2c_stop_seq[0] = I2C_CMD_STOP;
        driver_data->i2c_stop_seq[1] = I2C_CMD_WAIT;
        driver_data->i2c_stop_seq[2] = 0xFF;
        driver_data->nb_events = 0;
        driver_data->device_id = conf->itf;
        /* Attach freq callback. */
        pi_freq_callback_init(&(driver_data->i2c_freq_cb), __pi_i2c_freq_cb, driver_data);
        pi_freq_callback_add(&(driver_data->i2c_freq_cb));
        g_i2c_itf_data[conf->itf] = driver_data;

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_RX(conf->itf), __pi_i2c_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_TX(conf->itf), __pi_i2c_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_TX(conf->itf));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_I2C_ID(conf->itf));


        I2C_TRACE("I2C(%d) : driver data init done.\n", driver_data->device_id);
    }

    struct i2c_cs_data_s *cs_data = (struct i2c_cs_data_s *) pi_l2_malloc(sizeof(struct i2c_cs_data_s));
    if (cs_data == NULL)
    {
        I2C_TRACE_ERR("I2C(%ld) : cs=%d, cs_data alloc failed !\n",
                      driver_data->device_id, conf->cs);
        return -13;
    }
    cs_data->device_id = conf->itf;
    cs_data->cs = conf->cs;
    cs_data->max_baudrate = conf->max_baudrate;
    uint32_t clk_div = __pi_i2c_clk_div_get(cs_data->max_baudrate);
    if (clk_div == 0xFFFFFFFF)
    {
        pi_l2_free(cs_data, sizeof(struct i2c_cs_data_s));
        I2C_TRACE_ERR("I2C(%d) : error computing clock divider !\n", conf->itf);
        return -14;
    }
    cs_data->clk_div = clk_div;
    cs_data->next = NULL;
    __pi_i2c_cs_data_add(driver_data, cs_data);
    driver_data->nb_open++;
    I2C_TRACE("I2C(%d) : opened %ld time(s).\n",
              driver_data->device_id, driver_data->nb_open);
    *device_data = cs_data;
    return 0;
}

void __pi_i2c_close(struct i2c_cs_data_s *device_data)
{
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[device_data->device_id];
    __pi_i2c_cs_data_remove(driver_data, device_data);
    driver_data->nb_open--;
    I2C_TRACE("I2C(%d) : number of opened devices %ld.\n",
              driver_data->device_id, driver_data->nb_open);
    if (driver_data->nb_open == 0)
    {
        I2C_TRACE("I2C(%d) : closing interface.\n", driver_data->device_id);

        /* Remove freq callback. */
        pi_freq_callback_remove(&(driver_data->i2c_freq_cb));

        /* Clear allocated fifo. */
        pi_l2_free(driver_data->pending, sizeof(struct i2c_pending_transfer_s));
        pi_l2_free(driver_data, sizeof(struct i2c_itf_data_s));

        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_RX(driver_data->device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_TX(driver_data->device_id));

        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_RX(driver_data->device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_TX(driver_data->device_id));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_I2C_ID(driver_data->device_id));

        /* Free allocated struct. */
        pi_l2_free(driver_data, sizeof(struct i2c_itf_data_s));
        g_i2c_itf_data[device_data->device_id] = NULL;
    }
    pi_l2_free(device_data, sizeof(struct i2c_cs_data_s));
}

void __pi_i2c_ioctl(struct i2c_cs_data_s *device_data, uint32_t cmd, void *arg)
{
    return;
}

void __pi_i2c_copy(struct i2c_cs_data_s *cs_data, uint32_t l2_buff, uint32_t length,
                   pi_i2c_xfer_flags_e flags, udma_channel_e channel,
                   struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->data[0] = l2_buff;
    task->data[1] = length;
    task->data[2] = flags;
    task->data[3] = channel;
    task->data[4] = (uint32_t) cs_data;
    task->next    = NULL;
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];
    int32_t slot_rxtx = __pi_i2c_hw_fifo_both_empty(driver_data);
    /* Both slots should be empty to start a new read transfer. When enqueueing
     * a new read transfer, RX should be opened first then TX. So if RX is already
     * in use, then wait for it to finish. */
    if (slot_rxtx == 0)
    {
        /* Enqueue transfer in SW fifo. */
        I2C_TRACE("I2C(%d) : enqueue transfer in SW fifo : channel %d task %lx.\n",
                  driver_data->device_id, task->data[3], task);
        __pi_i2c_task_fifo_enqueue(driver_data, task);
    }
    else
    {
        /* Enqueue transfer in HW fifo. */
        I2C_TRACE("I2C(%d) : enqueue transfer in HW fifo : channel %d task %lx.\n",
                  driver_data->device_id, task->data[3], task);
        if (task->data[3] == RX_CHANNEL)
        {
            __pi_i2c_copy_exec_read(driver_data, task);
        }
        else
        {
            __pi_i2c_copy_exec_write(driver_data, task);
        }
    }
    __restore_irq(irq);
}

int32_t __pi_i2c_detect(struct i2c_cs_data_s *cs_data, struct pi_i2c_conf *conf,
                        uint8_t *rx_data, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    if (cs_data->device_id != conf->itf)
    {
        I2C_TRACE_ERR("I2C(%d) : error wrong interfaces %d - %d !\n",
                      cs_data->device_id, conf->itf);
        __restore_irq(irq);
        return -11;
    }
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];
    uint32_t clk_div = __pi_i2c_clk_div_get(conf->max_baudrate);
    if (clk_div == 0xFFFFFFFF)
    {
        I2C_TRACE_ERR("I2C(%d) : error computing clock divider !\n", conf->itf);
        __restore_irq(irq);
        return -12;
    }
    uint16_t clkdiv = clk_div;

    task->next    = NULL;

    uint32_t index = 0;
    uint32_t buffer = (uint32_t) rx_data;
    uint32_t size = 1;

    /* Header. */
    driver_data->i2c_cmd_seq[index++] = I2C_CMD_CFG;
    driver_data->i2c_cmd_seq[index++] = ((clkdiv >> 8) & 0xFF);
    driver_data->i2c_cmd_seq[index++] = (clkdiv & 0xFF);
    driver_data->i2c_cmd_seq[index++] = I2C_CMD_START;
    driver_data->i2c_cmd_seq[index++] = I2C_CMD_WR;
    driver_data->i2c_cmd_seq[index++] = (conf->cs | ADDRESS_READ);

    struct i2c_pending_transfer_s *pending = driver_data->pending;
    pending->pending_repeat = 0;
    /* Stop bit at then end? */
    driver_data->i2c_stop_send = 1;

    driver_data->i2c_cmd_seq[index++] = I2C_CMD_RD_NACK;

    /* Enqueue in HW fifo. */
    __pi_i2c_hw_fifo_enqueue(driver_data, task, RX_CHANNEL);

    /* Open RX channel to receive data. */
    hal_i2c_enqueue(driver_data->device_id, RX_CHANNEL, buffer, size,
                        UDMA_CORE_RX_CFG_EN(1));
    /* Transfer command. */
    hal_i2c_enqueue(driver_data->device_id, TX_CHANNEL, (uint32_t) driver_data->i2c_cmd_seq,
                    index, UDMA_CORE_TX_CFG_EN(1));
    __restore_irq(irq);
    return 0;
}
