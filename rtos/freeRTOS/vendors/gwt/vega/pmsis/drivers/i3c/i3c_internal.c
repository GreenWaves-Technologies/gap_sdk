/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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
#include "pmsis/implem/drivers/i3c/i3c_internal.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SLAVE_TARGET ( 0 )
#define TAG_ID       ( 1 )

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static struct i3c_itf_data_s *g_i3c_itf_data[ARCHI_NB_I3C] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_i3c_i2c_handler(void *arg);

static uint32_t __pi_i3c_i2c_clk_div_get(uint32_t i2c_freq);

static void __pi_i3c_i2c_copy_exec(struct i3c_itf_data_s *itf_data,
                                   struct pi_task *task, uint8_t hw_buff_idx);

static int32_t __pi_i3c_i2c_rx_data_read(struct i3c_itf_data_s *itf_data,
                                         struct pi_task *task);

static struct pi_task rx_data = {0}; /* TODO: use in non IRQ mode with a push. */

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_i3c_i2c_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint8_t periph_id = event - SOC_EVENT_I3C(0);
    uint32_t status = hal_i3c_irq_status_get(periph_id);
    I3C_TRACE("I3C(%d, %ld) IRQ received, status=%lx!\n", periph_id, event, status);

    struct i3c_itf_data_s *itf_data = g_i3c_itf_data[periph_id];

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    struct pi_task *task = pi_udma_fifo_hw_fifo_task_pop(udma_chan);
    I3C_TRACE("I3C(%d): pop task=%lx from HW buffer.\n", periph_id, task);

    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    if (task->data[2] == RX_CHANNEL)
    {
        uint32_t resp = hal_i3c_resp_get(periph_id);
        uint32_t *rx_buff = (uint32_t *) task->data[0];
        uint32_t size = task->data[1];
        uint32_t iter_size = (size >> 2) + ((size & 0x3) ? 1 : 0);
        for (uint32_t i=0; i<iter_size; i++)
        {
            rx_buff[i] = hal_i3c_data_rx_fifo_get(periph_id);
        }
    }
    I3C_TRACE("Handle end of current transfer, pop and start a new transfer if there is.\n");
    __pi_irq_handle_end_of_task(task);
    task = pi_udma_fifo_sw_fifo_task_pop(udma_chan);
    I3C_TRACE("I3C(%d) : pop new task=%lx from SW fifo and handle it.\n",
               itf_data->device_id, task);
    if (task != NULL)
    {
        __pi_i3c_i2c_copy_exec(itf_data, task, hw_buff_idx);
    }
}

static void __pi_i3c_i2c_copy_read(struct i3c_itf_data_s *itf_data, uint32_t buff,
                                   uint32_t size, uint8_t stop, uint8_t slave_target)
{
    uint8_t is_read = 1;
    uint8_t device_id = itf_data->device_id;
    if (size <= 4)
    {
        /* Immediate command transfer. */
        hal_i3c_cmd_transfer_immediate_set(device_id,
                                           0,
                                           size,
                                           stop,
                                           1, /* ROC */
                                           is_read,
                                           slave_target);
    }
    else
    {
        /* Regular command transfer. */
        hal_i3c_cmd_transfer_regular_set(device_id,
                                         size,
                                         stop,
                                         1, /* ROC */
                                         is_read,
                                         slave_target,
                                         TAG_ID);
    }
    /* Data available after IRQ is received. */
}

static void __pi_i3c_i2c_copy_write(struct i3c_itf_data_s *itf_data, uint32_t buff,
                                    uint32_t size, uint8_t stop, uint8_t slave_target)
{
    uint8_t is_read = 0;
    uint8_t device_id = itf_data->device_id;
    uint32_t *tx_buff = (uint32_t *) buff;
    if (size <= 4)
    {
        /* Immediate command transfer. */
        hal_i3c_cmd_transfer_immediate_set(device_id,
                                           tx_buff[0],
                                           size,
                                           stop,
                                           1, /* ROC */
                                           is_read,
                                           slave_target);
    }
    else
    {
        /* Regular command transfer. */
        hal_i3c_cmd_transfer_regular_set(device_id,
                                         size,
                                         stop,
                                         1, /* ROC */
                                         is_read,
                                         slave_target,
                                         TAG_ID);
        /* Send data. */
        uint32_t iter_size = (size >> 2) + ((size & 0x3) ? 1 : 0);
        for (uint32_t i=0; i<iter_size; i++)
        {
            hal_i3c_data_tx_fifo_set(device_id, tx_buff[i]);
        }
    }
    /* Data transfer completion signaled with IRQ. */
}

static void __pi_i3c_i2c_copy_exec(struct i3c_itf_data_s *itf_data,
                                   struct pi_task *task, uint8_t hw_buff_idx)
{
    uint32_t buff = task->data[0];
    uint32_t size = task->data[1];
    uint8_t channel = task->data[2];
    uint8_t stop = (task->data[3] & PI_I3C_I2C_XFER_STOP);
    uint8_t device_id = itf_data->device_id;

    uint8_t dynamic_addr = 0;
    uint8_t static_addr = itf_data->cs;
    uint8_t slave_target = SLAVE_TARGET;

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    //pi_udma_fifo_buffer_set(task, &buff, &size, max_size);
    pi_udma_fifo_hw_fifo_task_enqueue(udma_chan, task, hw_buff_idx);

    /* Enable i3c master control. */
    hal_i3c_master_control_enable_set(device_id, 1);
    /* Set up i3c as i2c, set slave address. */
    hal_i3c_device_addr_table_reg_set(device_id,
                                      slave_target,
                                      I3C_IS_I2C,
                                      dynamic_addr,
                                      static_addr);
    /* Start data transfer. */
    if (channel == RX_CHANNEL)
    {
        __pi_i3c_i2c_copy_read(itf_data, buff, size, stop, slave_target);
    }
    else
    {
        __pi_i3c_i2c_copy_write(itf_data, buff, size, stop, slave_target);
    }
}

static uint32_t __pi_i3c_i2c_clk_div_get(uint32_t i2c_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    uint32_t div = (periph_freq + i2c_freq - 1) / i2c_freq;
    if (div > 0xFF)
    {
        I3C_TRACE_ERR("Error computing clock divier : Fsoc=%ld, Fi2c=%ld, clk_div=%ld\n",
                      periph_freq, i2c_freq, div);
        return 0xFFFFFFFF;
    }
    return div;
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_i3c_i2c_conf_init(struct pi_i3c_i2c_conf_s *conf)
{
    conf->itf = 0;
    conf->cs = 0x0;
    conf->baudrate = 0;
}

int32_t __pi_i3c_i2c_open(struct pi_i3c_i2c_conf_s *conf,
                          struct i3c_itf_data_s **device_data)
{
    uint32_t irq = __disable_irq();

    struct i3c_itf_data_s *itf_data = g_i3c_itf_data[conf->itf];
    if (itf_data == NULL)
    {
        itf_data = pi_fc_l1_malloc(sizeof(struct i3c_itf_data_s));
        if (itf_data == NULL)
        {
            I3C_TRACE_ERR("Driver itf alloc failed !\n");
            __restore_irq(irq);
            return -11;
        }
        g_i3c_itf_data[conf->itf] = itf_data;
        /* itf_data->fifo_head = NULL; */
        /* itf_data->fifo_tail = NULL; */
        pi_udma_fifo_init(&(itf_data->udma_chan));
        itf_data->device_id = conf->itf;
        itf_data->cs = conf->cs;
        itf_data->nb_open = 0;

        uint8_t clk_div = __pi_i3c_i2c_clk_div_get(conf->baudrate);

        /* Enable CLK_DIV. */
        hal_soc_ctrl_i3c_clk_div_set(itf_data->device_id, clk_div);

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_I3C(itf_data->device_id), __pi_i3c_i2c_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_I3C(itf_data->device_id));
    }
    *device_data = g_i3c_itf_data[conf->itf];
    itf_data->nb_open++;
    I3C_TRACE("Device id=%d opened=%ld\n", itf_data->device_id, itf_data->nb_open);

    __restore_irq(irq);
    return 0;
}

void __pi_i3c_i2c_close(struct i3c_itf_data_s *itf_data)
{
    uint32_t irq = __disable_irq();
    /* Decrement number of devices opened. */
    itf_data->nb_open--;
    /* Free device and structure opened. */
    if (itf_data->nb_open == 0)
    {
        /* Clear handler. */
        pi_fc_event_handler_clear(SOC_EVENT_I3C(itf_data->device_id));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_I3C(itf_data->device_id));

        /* Disable CLK_DIV. */
        hal_soc_ctrl_i3c_clk_div_disable_set(itf_data->device_id);

        /* Free allocated driver data. */
        g_i3c_itf_data[itf_data->device_id] = NULL;
        pi_fc_l1_free(itf_data, sizeof(struct i3c_itf_data_s));
    }
    __restore_irq(irq);
}

void __pi_i3c_i2c_copy(struct i3c_itf_data_s *itf_data, uint32_t l2_buff,
                       uint32_t length, uint8_t flags, udma_channel_e channel,
                       struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->data[0] = l2_buff;
    task->data[1] = length;
    task->data[2] = channel;
    task->data[3] = flags;
    task->next = NULL;

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Only one transfer at a time. */
    if (hw_buff_idx == 0)
    {
        I3C_TRACE("I3C(%d) : enqueue transfer in HW buffer. "
                  "Task=%lx, l2_buf=%lx, size=%ld, channel=%d, flags=%x\n",
                  itf_data->device_id, task, task->data[0], task->data[1],
                  task->data[2], task->data[3]);
        __pi_i3c_i2c_copy_exec(itf_data, task, hw_buff_idx);
    }
    else
    {
        I3C_TRACE("I3C(%d) : in use, enqueue transfer in SW buffer. "
                  "Task=%lx, l2_buf=%lx, size=%ld, channel=%d, flags=%x\n",
                  itf_data->device_id, task, task->data[0], task->data[1],
                  task->data[2], task->data[3]);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
    }
    __restore_irq(irq);
}
