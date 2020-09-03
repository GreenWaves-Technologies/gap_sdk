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

#include "pmsis/implem/drivers/udma/i2c/i2c_internal.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_i2c_conf_init(pi_i2c_conf_t *conf)
{
    __pi_i2c_conf_init(conf);
}

int pi_i2c_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_i2c_conf *conf = (struct pi_i2c_conf *) device->config;
    I2C_TRACE("Open device id=%d\n", conf->itf);
    status = __pi_i2c_open(conf, (struct i2c_cs_data_s **) &(device->data));
    I2C_TRACE("Open status : %ld, driver data: %lx\n",
              status, (struct i2c_cs_data_s *) device->data);
    return status;
}

void pi_i2c_close(struct pi_device *device)
{
    struct i2c_cs_data_s *device_data = (struct i2c_cs_data_s *) device->data;
    if (device_data != NULL)
    {
        I2C_TRACE("Close device id=%d\n", device_data->device_id);
        __pi_i2c_close(device_data);
        device->data = NULL;
    }
}

void pi_i2c_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct i2c_cs_data_s *device_data = (struct i2c_cs_data_s *) device->data;
    if (device_data != NULL)
    {
        I2C_TRACE("Ioctl command : %lx, arg %lx\n", cmd, arg);
        __pi_i2c_ioctl(device_data, cmd, arg);
    }
}

#define MUTEX 1
int pi_i2c_read(struct pi_device *device, uint8_t *rx_buff, int length,
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
    return PI_OK;
}

void pi_i2c_read_async(struct pi_device *device, uint8_t *rx_buff, int length,
                       pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    struct i2c_cs_data_s *device_data = (struct i2c_cs_data_s *) device->data;
    udma_channel_e channel = RX_CHANNEL;
    I2C_TRACE("I2C(%d) : transfer %d %lx %ld %lx, task %lx\n",
              device_data->device_id, channel, (uint32_t) rx_buff, length,
              flags, task);
    __pi_i2c_copy(device_data, (uint32_t) rx_buff, length, flags, channel, task);
}

int pi_i2c_write(struct pi_device *device, uint8_t *tx_data, int length,
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
    return PI_OK;
}

void pi_i2c_write_async(struct pi_device *device, uint8_t *tx_data, int length,
                        pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    struct i2c_cs_data_s *device_data = (struct i2c_cs_data_s *) device->data;
    udma_channel_e channel = TX_CHANNEL;
    I2C_TRACE("I2C(%d) : transfer %d %lx %ld %lx, task %lx\n",
              device_data->device_id, channel, (uint32_t) tx_data, length,
              flags, task);
    __pi_i2c_copy(device_data, (uint32_t) tx_data, length, flags, channel, task);
}

int pi_i2c_get_request_status(pi_task_t* task)
{
    (void) task;
    return PI_OK;
}

int pi_i2c_detect(struct pi_device *device, struct pi_i2c_conf *conf, uint8_t *rx_data)
{
    int32_t status = -1;
    struct i2c_cs_data_s *cs_data = (struct i2c_cs_data_s *) device->data;
    pi_task_t task_block;
    pi_task_block(&task_block);
    I2C_TRACE("Search device at cs=%x\n", conf->cs);
    __pi_i2c_detect(cs_data, conf, rx_data, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    status = (*rx_data == 0x00) ? 0 : -1;
    I2C_TRACE("Search device at cs=%x result=%x\n", conf->cs, status);
    return status;
}
