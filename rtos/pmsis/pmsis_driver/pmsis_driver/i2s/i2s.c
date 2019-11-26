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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

extern struct i2c_driver_fifo_s *__global_i2s_driver_fifo[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    __pi_i2s_conf_init(conf);
}

int pi_i2s_open(struct pi_device *device)
{
    struct pi_i2s_conf *conf = (struct pi_i2s_conf *) device->config;
    if (__global_i2s_driver_fifo[conf->itf] == NULL)
    {
        __pi_i2s_open(conf);
        device->data = (void *) __global_i2s_driver_fifo[conf->itf];
    }
    return 0;
}

void pi_i2s_close(struct pi_device *device)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    if (fifo != NULL)
    {
        __pi_i2s_close(fifo->i2s_id);
    }
}

int pi_i2s_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    __pi_i2s_ioctl(fifo->i2s_id, cmd, arg);
    return 0;
}

int pi_i2s_read(struct pi_device *device, void **mem_block, size_t *size)
{
    pi_task_t task;
    pi_task_block(&task);
    pi_i2s_read_async(device, &task);
    pi_task_wait_on(&task);
    return pi_i2s_read_status(&task, mem_block, size);
}

int pi_i2s_read_async(struct pi_device *device, pi_task_t *task)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    __pi_i2s_read_async(fifo->i2s_id, task);
    return 0;
}

int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size)
{
    return __pi_i2s_read_status(task, mem_block, size);
}

int pi_i2s_write(struct pi_device *device, void *mem_block, size_t size)
{
    return -1;
}
