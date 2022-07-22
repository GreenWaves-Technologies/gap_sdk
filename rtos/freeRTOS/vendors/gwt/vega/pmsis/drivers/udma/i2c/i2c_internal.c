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

#define I2C_WRITE_READ 0x0
#define I2C_WRITE_DUAL 0x1
#define I2C_WRITE 0x2
#define I2C_READ 0x3

/* Max length of a i2c request/data buffer. */
#define MAX_SIZE               (0xFF)

// I2C command IDS definition
#define NEW_I2C_CMD_START() ((0x0UL << 0x1cUL))
#define NEW_I2C_CMD_STOP() ((0x2UL << 0x1cUL))
#define NEW_I2C_CMD_RD_ACK() ((0x4UL << 0x1cUL))
#define NEW_I2C_CMD_RD_NACK() ((0x6UL << 0x1cUL))
#define NEW_I2C_CMD_WR() ((0x8UL << 0x1cUL))
#define NEW_I2C_CMD_WAIT(dummy_cycles) ((0xaUL << 0x1cUL)|(((uint32_t)dummy_cycles) << 0x0))
#define NEW_I2C_CMD_RPT(rpt_nb) ((0xcUL << 0x1cUL)|(((uint32_t)rpt_nb) << 0x0))
#define NEW_I2C_CMD_CFG(cfg) ((0xeUL << 0x1cUL)|(((uint32_t)cfg) << 0x0))
#define NEW_I2C_CMD_WAIT_EV(event) ((0x1UL << 0x1cUL) | ((event&0x3)<<24))
#define NEW_I2C_CMD_WRB(data_byte) ((0x7UL << 0x1cUL)|(((uint32_t)data_byte) << 0x0))
#define NEW_I2C_CMD_EOT() ((0x9UL << 0x1cUL))
#define NEW_I2C_CMD_SETUP_UCA(txrxn,saddr) ((0x3UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)saddr) << 0x0))
#define NEW_I2C_CMD_SETUP_UCS(txrxn,size) ((0x5UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)size) << 0x0))

#define NEW_I2C_CMD_SETUP_WRA(addr,rx) (NEW_I2C_CMD_WRB((addr&0xFE) | (rx)))

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

static void __pi_i2c_enqueue_next_task(pi_task_t *task)
{
    switch(task->data[5])
    {
        case I2C_WRITE_DUAL:
            __pi_i2c_write_dual_async(task->data[4],task->data[0],task->data[1],
                    task->data[2],task->data[3],task);
            break;
        case I2C_WRITE_READ:
            __pi_i2c_write_read_async(task->data[4],task->data[0],task->data[1],
                    task->data[2],task->data[3],task);
            break;
        case I2C_WRITE:
            __pi_i2c_write_async(task->data[4],task->data[0],task->data[1],
                    task->data[2],task);
            break;
        case I2C_READ:
            __pi_i2c_read_async(task->data[4],task->data[0],task->data[1],
                    task->data[2],task);
            break;
        default:
            exit(-1);
    }
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
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2C_ID(0);

    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[periph_id];

    /* Pending transfer. */
    struct pi_task *task = driver_data->pending;
    if (task != NULL)
    {
        __pi_irq_handle_end_of_task(task);
    }
    driver_data->pending = NULL;
    task = __pi_i2c_task_fifo_pop(driver_data);
    if (task != NULL)
    {
        /* Enqueue transfer in HW fifo. */
        __pi_i2c_enqueue_next_task(task);
    }
    driver_data->nb_events = 0;
}

static int32_t __pi_i2c_hw_fifo_both_empty(struct i2c_itf_data_s *driver_data)
{
    return driver_data->pending == NULL;
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

void __pi_i2c_write_read_async(struct i2c_cs_data_s *cs_data, void *tx_buffer,
        void *rx_buffer, uint32_t tx_size, uint32_t rx_size, pi_task_t *task)
{
    uint32_t irq = disable_irq();

    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];

    //pi_i2c_t *i2c = (pi_i2c_t *)device->data;

    if (driver_data->pending)
    {
        task->data[0] = tx_buffer;
        task->data[1] = rx_buffer;
        task->data[2] = tx_size;
        task->data[3] = rx_size;
        task->data[4] = (uint32_t) cs_data;
        task->data[5] = I2C_WRITE_READ;

        __pi_i2c_task_fifo_enqueue(driver_data,task);
        __restore_irq(irq);
        return;
    }
    driver_data->pending = task;

    int seq_index = 0;

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_START();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_WRA(cs_data->cs, 0);

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(tx_size);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WR();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)tx_buffer) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)tx_size) & 0x1FFFFF);

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_START();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_WRA(cs_data->cs, 1);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(rx_size-1);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(0, ((uint32_t)rx_buffer) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(0, ((uint32_t)rx_size) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RD_ACK();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RD_NACK();

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_STOP();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WAIT(0xFFUL);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_EOT();

    /* Send everything in command channel */
    hal_i2c_enqueue(driver_data->device_id, COMMAND_CHANNEL,
            driver_data->i2c_cmd_seq, seq_index*4, UDMA_CORE_RX_CFG_EN(1));
    for(volatile int i = 0; i < 50000; i++);
    restore_irq(irq);
}

void __pi_i2c_write_dual_async(struct i2c_cs_data_s *cs_data, void *buffer0,
        void *buffer1, uint32_t size0, uint32_t size1, pi_task_t *task)
{
    uint32_t irq = disable_irq();
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];

    if (driver_data->pending)
    {
        task->data[0] = buffer0;
        task->data[1] = buffer1;
        task->data[2] = size0;
        task->data[3] = size1;
        task->data[4] = (uint32_t)cs_data;
        task->data[5] = I2C_WRITE_DUAL;

        __pi_i2c_task_fifo_enqueue(driver_data,task);
        restore_irq(irq);
        return;
    }

    driver_data->pending = task;
    int seq_index = 0;

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_START();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_WRA(cs_data->cs, 0);

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(size0);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WR();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)buffer0) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)size0) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(size1);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WR();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)buffer1) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)size1) & 0x1FFFFF);

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_STOP();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WAIT(0xFFUL);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_EOT();
    /* Send everything in command channel */
    hal_i2c_enqueue(driver_data->device_id, COMMAND_CHANNEL,
            driver_data->i2c_cmd_seq, seq_index*4, UDMA_CORE_RX_CFG_EN(1));
    for(volatile int i = 0; i < 50000; i++);
    restore_irq(irq);
}

void __pi_i2c_write_async(struct i2c_cs_data_s *cs_data, uint8_t *tx_data,
        int length, pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    uint32_t irq = disable_irq();
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];

    if (driver_data->pending)
    {
        task->data[0] = tx_data;
        task->data[1] = length;
        task->data[2] = flags;
        task->data[4] = (uint32_t)cs_data;
        task->data[5] = I2C_WRITE;

        __pi_i2c_task_fifo_enqueue(driver_data,task);
        restore_irq(irq);
        return;
    }

    driver_data->pending = task;
    int seq_index = 0;

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_START();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_WRA(cs_data->cs, 0);

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(length);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WR();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)tx_data) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)length) & 0x1FFFFF);

    if(!(flags & PI_I2C_XFER_NO_STOP))
    {
        driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_STOP();
    }
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_EOT();
    /* Send everything in command channel */
    hal_i2c_enqueue(driver_data->device_id, COMMAND_CHANNEL,
            driver_data->i2c_cmd_seq, seq_index*4, UDMA_CORE_RX_CFG_EN(1));
    restore_irq(irq);
}

void __pi_i2c_read_async(struct i2c_cs_data_s *cs_data, uint8_t *rx_buff,
        int length, pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    uint32_t irq = disable_irq();
    struct i2c_itf_data_s *driver_data = g_i2c_itf_data[cs_data->device_id];

    if (driver_data->pending)
    {
        task->data[0] = rx_buff;
        task->data[1] = length;
        task->data[2] = flags;
        task->data[4] = (uint32_t)cs_data;
        task->data[5] = I2C_READ;

        __pi_i2c_task_fifo_enqueue(driver_data,task);
        restore_irq(irq);
        return;
    }

    driver_data->pending = task;
    int seq_index = 0;

    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_START();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_WRA(cs_data->cs, 1);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RPT(length-1);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCA(0, ((uint32_t)rx_buff) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_SETUP_UCS(0, ((uint32_t)length) & 0x1FFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RD_ACK();
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_RD_NACK();

    if(!(flags & PI_I2C_XFER_NO_STOP))
    {
        driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_STOP();
    }
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_EOT();
    /* Send everything in command channel */
    hal_i2c_enqueue(driver_data->device_id, COMMAND_CHANNEL,
            driver_data->i2c_cmd_seq, seq_index*4, UDMA_CORE_RX_CFG_EN(1));
    restore_irq(irq);
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

static int32_t __pi_i2c_baudrate_set(struct i2c_cs_data_s *cs_data,
                                     uint32_t new_baudrate)
{
    cs_data->max_baudrate = new_baudrate;
    uint32_t clk_div = __pi_i2c_clk_div_get(cs_data->max_baudrate);
    if (clk_div == 0xFFFFFFFF)
    {
        I2C_TRACE_ERR("I2C(%d) : error computing clock divider !\n", cs_data->device_id);
        return -14;
    }
    cs_data->clk_div = clk_div;
    return 0;
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
    int irq = disable_irq();
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
        driver_data->nb_events = 0;
        driver_data->device_id = conf->itf;
        /* Attach freq callback. */
        pi_freq_callback_init(&(driver_data->i2c_freq_cb), __pi_i2c_freq_cb, driver_data);
        pi_freq_callback_add(&(driver_data->i2c_freq_cb));
        g_i2c_itf_data[conf->itf] = driver_data;

        /* Set handlers. */
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_RX(conf->itf), __pi_i2c_handler);
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_CMD(conf->itf), __pi_i2c_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_EOT(conf->itf), __pi_i2c_handler);
        /* Enable SOC events propagation to FC. */
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_TX(conf->itf));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_EOT(conf->itf));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_I2C_ID(conf->itf));
        //hal_read32(&UDMA_GC->CG);

//,1 << udma_device_id
        //printf("device_id= %x CG=%x\n",UDMA_I2C_ID(conf->itf),hal_read32(&UDMA_GC->CG));


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

    
    uint32_t seq_index = 0;
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_CFG(clk_div & 0xFFFF);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_WAIT(0xFFUL);
    driver_data->i2c_cmd_seq[seq_index++] = NEW_I2C_CMD_EOT();

    pi_task_t init_task;
    pi_task_block(&init_task);
    driver_data->pending = &init_task;
    hal_i2c_enqueue(cs_data->device_id, COMMAND_CHANNEL,
            driver_data->i2c_cmd_seq, seq_index*4, UDMA_CORE_RX_CFG_EN(1));

    restore_irq(irq);
    pi_task_wait_on(&init_task);
    pi_task_destroy(&init_task);
    return 0;
}

void __pi_i2c_close(struct i2c_cs_data_s *device_data)
{
    int irq = __disable_irq();
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

        /* Clear handlers. */
        //pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_RX(driver_data->device_id));
        //pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_TX(driver_data->device_id));

        /* Set handlers. */
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_RX(conf->itf), __pi_i2c_handler);
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_CMD(conf->itf), __pi_i2c_handler);
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_CMD(driver_data->device_id));
        /* Enable SOC events propagation to FC. */
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_TX(conf->itf));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_CMD(driver_data->device_id));

        /* Disable SOC events propagation to FC. */
        //hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_RX(driver_data->device_id));
        //hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_TX(driver_data->device_id));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_I2C_ID(driver_data->device_id));

        /* Free allocated struct. */
        pi_l2_free(driver_data, sizeof(struct i2c_itf_data_s));
        g_i2c_itf_data[device_data->device_id] = NULL;
    }
    pi_l2_free(device_data, sizeof(struct i2c_cs_data_s));
    __restore_irq(irq);
}

void __pi_i2c_ioctl(struct i2c_cs_data_s *device_data, uint32_t cmd, void *arg)
{
    switch (cmd)
    {
    case PI_I2C_CTRL_SET_MAX_BAUDRATE :
        __pi_i2c_baudrate_set(device_data, (uint32_t) arg);
        break;

    default :
        break;
    }
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
