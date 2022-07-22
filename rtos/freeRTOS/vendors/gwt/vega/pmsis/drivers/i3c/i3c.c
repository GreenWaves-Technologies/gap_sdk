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

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_i3c_i2c_conf_init(struct pi_i3c_i2c_conf_s *conf)
{
    __pi_i3c_i2c_conf_init(conf);
}

int pi_i3c_i2c_open(pi_device_t *device)
{
    int32_t status = -1;
    struct pi_i3c_i2c_conf_s *conf = (struct pi_i3c_i2c_conf_s *) device->config;
    I3C_TRACE("Open device id=%d\n", conf->itf);
    status = __pi_i3c_i2c_open(conf, (struct i3c_itf_data_s **) &(device->data));
    I3C_TRACE("Open status : %ld, driver data: %p\n",
              status, (struct i3c_itf_data_s *) device->data);
    return status;
}

void pi_i3c_i2c_close(pi_device_t *device)
{
    struct i3c_itf_data_s *itf_data = (struct i3c_itf_data_s *) device->data;
    I3C_TRACE("Close device id=%d\n", itf_data->device_id);
    if (itf_data != NULL)
    {
        __pi_i3c_i2c_close(itf_data);
        device->data = NULL;
    }
}

int pi_i3c_i2c_read(pi_device_t *device, uint8_t *rx_buff, uint32_t length,
                    uint8_t flags)
{
    int32_t status = -1;
    pi_task_t task_block;
    pi_task_block(&task_block);
    status = pi_i3c_i2c_read_async(device, rx_buff, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    return status;
}

int pi_i3c_i2c_read_async(pi_device_t *device, uint8_t *rx_buff, uint32_t length,
                          uint8_t flags, struct pi_task *task)
{
    int32_t status = 0;
    struct i3c_itf_data_s *itf_data = (struct i3c_itf_data_s *) device->data;
    I3C_TRACE("Read device=%d: l2_buff=%lx, length=%ld, flags=%x, task=%lx\n",
              itf_data->device_id, rx_buff, length, flags, task);
    __pi_i3c_i2c_copy(itf_data, (uint32_t) rx_buff, length, flags, RX_CHANNEL, task);
    return status;
}

int pi_i3c_i2c_write(pi_device_t *device, uint8_t *tx_buff, uint32_t length,
                     uint8_t flags)
{
    int32_t status = -1;
    pi_task_t task_block;
    pi_task_block(&task_block);
    status = pi_i3c_i2c_write_async(device, tx_buff, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    return status;
}

int pi_i3c_i2c_write_async(pi_device_t *device, uint8_t *tx_buff, uint32_t length,
                           uint8_t flags, struct pi_task *task)
{
    int32_t status = 0;
    struct i3c_itf_data_s *itf_data = (struct i3c_itf_data_s *) device->data;
    I3C_TRACE("Write device=%d: l2_buff=%lx, length=%ld, flags=%x, task=%lx\n",
              itf_data->device_id, tx_buff, length, flags, task);
    __pi_i3c_i2c_copy(itf_data, (uint32_t) tx_buff, length, flags, TX_CHANNEL, task);
    return status;
}
