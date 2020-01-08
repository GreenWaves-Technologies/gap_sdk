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

#include "pmsis_driver/i2c/i2c_internal.h"
#include DEFAULT_MALLOC_INC
#include "pmsis_driver/pmsis_fc_event.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static uint8_t _i2c_init[UDMA_NB_I2C] = {0};

extern struct i2c_driver_fifo_s *__global_i2c_driver_fifo[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_i2c_conf_init(pi_i2c_conf_t *conf)
{
    conf->device = PI_DEVICE_I2C_TYPE;
    conf->cs = 0;
    conf->max_baudrate = 200000;
    conf->itf = 0;
}

int pi_i2c_open(struct pi_device *device)
{
    struct pi_i2c_conf *conf = (struct pi_i2c_conf *) device->config;
    /* Init I2C periph. */
    _i2c_init[conf->itf]++;
    if (_i2c_init[conf->itf] == 1)
    {
        DEBUG_PRINTF("[%s]\n",__func__);
        /* Make a copy of config just in case. */
        struct pi_i2c_conf *config = (struct pi_i2c_conf *) pi_default_malloc(sizeof(struct pi_i2c_conf));
        if (config == NULL)
        {
            DEBUG_PRINTF("[%s] config malloc failed\n", __func__);
            return -1;
        }
        config->device = conf->device;
        config->cs = conf->cs;
        config->max_baudrate = conf->max_baudrate;
        config->itf = conf->itf;
        device->config = config;
        /* Init fifo struct for I2C. */
        struct i2c_driver_fifo_s *fifo = (struct i2c_driver_fifo_s *) pi_default_malloc(sizeof(struct i2c_driver_fifo_s));
        if (fifo == NULL)
        {
            DEBUG_PRINTF("[%s] fifo malloc failed\n", __func__);
            pi_default_free(config, sizeof(struct pi_i2c_conf));
            return -2;
        }
        /* Pending transfer info. */
        fifo->pending = (struct i2c_pending_transfer_s *) pi_default_malloc(sizeof(struct i2c_pending_transfer_s));
        if (fifo->pending == NULL)
        {
            DEBUG_PRINTF("[%s] fifo->pending malloc failed\n", __func__);
            pi_default_free(config, sizeof(struct pi_i2c_conf));
            pmsis_l2_malloc_free(fifo, sizeof(struct i2c_driver_fifo_s));
            return -3;
        }
        /* Best to use only one queue since both RX & TX can be used at the same time. */
        fifo->hw_buffer[RX_CHANNEL] = NULL;
        fifo->hw_buffer[TX_CHANNEL] = NULL;
        fifo->fifo_head = NULL;
        fifo->fifo_tail = NULL;
        fifo->cs = conf->cs;
        fifo->max_baudrate = conf->max_baudrate;
        fifo->div = __pi_i2c_get_clk_div(conf->max_baudrate);
        fifo->i2c_cmd_index = 0;
        for (uint32_t i=0; i<(uint32_t) __PI_I2C_CMD_BUFF_SIZE; i++)
        {
            fifo->i2c_cmd_seq[i] = 0;
        }
        fifo->i2c_stop_send = 0;
        /* Set up i2c cmd stop sequence. */
        fifo->i2c_stop_seq[0] = I2C_CMD_STOP;
        fifo->i2c_stop_seq[1] = I2C_CMD_WAIT;
        fifo->i2c_stop_seq[2] = 0xFF;
        device->data = (void *) fifo;
        __global_i2c_driver_fifo[conf->itf] = fifo;

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_RX(conf->itf), i2c_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_TX(conf->itf), i2c_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_TX(conf->itf));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_I2C_ID(conf->itf));
    }
    return 0;
}

void pi_i2c_close(struct pi_device *device)
{
    struct pi_i2c_conf *conf = (struct pi_i2c_conf *) device->config;
    /* Deinit I2C periph. */
    _i2c_init[conf->itf]--;
    if (_i2c_init[conf->itf] == 0)
    {
        /* Clear allocated fifo. */
        struct i2c_driver_fifo_s *fifo = (struct i2c_driver_fifo_s *) device->data;
        pi_default_free(fifo->pending, sizeof(struct i2c_pending_transfer_s));
        pi_default_free(fifo, sizeof(struct i2c_driver_fifo_s));

        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2C_TX(conf->itf));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_RX(conf->itf));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2C_TX(conf->itf));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_I2C_ID(conf->itf));

        /* Free allocated config. */
        pi_default_free(conf, sizeof(struct pi_i2c_conf));
    }
}

void pi_i2c_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    /* TODO. */
}

#define MUTEX 1
void pi_i2c_read(struct pi_device *device, uint8_t *rx_buff, int length,
                 pi_i2c_xfer_flags_e flags)
{
    pi_task_t task_block;
    #if MUTEX
    pi_task_block(&task_block);
    pi_i2c_read_async(device, rx_buff, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    #else
    pi_task_block_no_mutex(&task_block);
    pi_i2c_read_async(device, rx_buff, length, flags, &task_block);
    pi_task_wait_on_no_mutex(&task_block);
    #endif
}

void pi_i2c_read_async(struct pi_device *device, uint8_t *rx_buff, int length,
                       pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    struct pi_i2c_conf *conf = (struct pi_i2c_conf *) device->config;
    struct i2c_transfer_s transfer = {0};
    transfer.buffer = rx_buff;
    transfer.size = length;
    transfer.flags = flags;
    transfer.device_id = conf->itf;
    transfer.channel = RX_CHANNEL;
    DEBUG_PRINTF("[%s] I2C(%d) RX Transfer : %x %d %x\n",
                 __func__, transfer.device_id, transfer.buffer, transfer.size, transfer.flags);
    __pi_i2c_copy((struct i2c_driver_fifo_s *) device->data, &transfer, task);
}

void pi_i2c_write(struct pi_device *device, uint8_t *tx_data, int length,
                  pi_i2c_xfer_flags_e flags)
{
    pi_task_t task_block;
    #if MUTEX
    pi_task_block(&task_block);
    pi_i2c_write_async(device, tx_data, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    #else
    pi_task_block_no_mutex(&task_block);
    pi_i2c_write_async(device, tx_data, length, flags, &task_block);
    pi_task_wait_on_no_mutex(&task_block);
    #endif
}

void pi_i2c_write_async(struct pi_device *device, uint8_t *tx_data, int length,
                        pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    struct pi_i2c_conf *conf = (struct pi_i2c_conf *) device->config;
    struct i2c_transfer_s transfer = {0};
    transfer.buffer = tx_data;
    transfer.size = length;
    transfer.flags = flags;
    transfer.device_id = conf->itf;
    transfer.channel = TX_CHANNEL;
    DEBUG_PRINTF("[%s] I2C(%d) TX Transfer : %x %d %x\n",
                 __func__, transfer.device_id, transfer.buffer, transfer.size, transfer.flags);
    __pi_i2c_copy((struct i2c_driver_fifo_s *) device->data, &transfer, task);
}
