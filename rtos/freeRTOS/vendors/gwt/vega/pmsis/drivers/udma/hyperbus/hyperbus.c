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

#include "pmsis/implem/drivers/udma/hyperbus/hyperbus_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_hyper_conf_init(struct pi_hyper_conf *conf)
{
    __pi_hyper_conf_init(conf);
}

int32_t pi_hyper_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    HYPER_TRACE("Open device id=%d\n", conf->id);
    status = __pi_hyper_open(conf, (struct hyper_itf_conf_s **) &device->data);
    HYPER_TRACE("Open status : %d, driver conf: %p\n",
                status, (struct hyper_itf_conf_s *) device->data);
    return status;
}

void pi_hyper_close(struct pi_device *device)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("Close device id=%d\n", conf->device_id);
    if (conf != NULL)
    {
        __pi_hyper_close(conf);
        device->data = NULL;
    }
}

void pi_hyper_read(struct pi_device *device, uint32_t hyper_addr,
                   void *addr, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_hyper_read_async(device, hyper_addr, addr, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_hyper_read_async(struct pi_device *device, uint32_t hyper_addr,
                         void *addr, uint32_t size, struct pi_task *task)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("Read async device id=%d\n", conf->device_id);
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->cs == 0) ? REG_MBR0 : REG_MBR1;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = (uint32_t) addr;
    transfer.size = size;
    transfer.stride = size;
    transfer.length = size;
    transfer.channel = RX_CHANNEL;
    HYPER_TRACE("HYPER(%d) RX Transfer : %lx %lx %ld %s\n",
                conf->device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                ((conf->hyper_type == PI_HYPER_TYPE_FLASH) ? "flash" : "ram"));
    __pi_hyper_copy(conf, &transfer, task);
}

void pi_hyper_write(struct pi_device *device, uint32_t hyper_addr,
                    void *addr, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_hyper_write_async(device, hyper_addr, addr, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_hyper_write_async(struct pi_device *device, uint32_t hyper_addr,
                          void *addr, uint32_t size, struct pi_task *task)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("Write async device id=%d\n", conf->device_id);
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->cs == 0) ? REG_MBR0 : REG_MBR1;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = (uint32_t) addr;
    transfer.size = size;
    transfer.stride = size;
    transfer.length = size;
    transfer.channel = TX_CHANNEL;
    HYPER_TRACE("HYPER(%d) TX Transfer : %lx %lx %ld %s\n",
                conf->device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                ((conf->hyper_type == PI_HYPER_TYPE_FLASH) ? "flash" : "ram"));
    __pi_hyper_copy(conf, &transfer, task);
}

void pi_hyper_read_2d(struct pi_device *device, uint32_t hyper_addr, void *addr,
                      uint32_t size, uint32_t stride, uint32_t length)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_hyper_read_2d_async(device, hyper_addr, addr, size, stride, length, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_hyper_read_2d_async(struct pi_device *device, uint32_t hyper_addr,
                            void *addr, uint32_t size, uint32_t stride,
                            uint32_t length, struct pi_task *task)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("Read 2D async device id=%d\n", conf->device_id);
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->cs == 0) ? REG_MBR0 : REG_MBR1;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = (uint32_t) addr;
    transfer.size = size;
    transfer.stride = stride;
    transfer.length = length;
    transfer.channel = RX_CHANNEL;
    HYPER_TRACE("HYPER(%d) RX Transfer : %lx %lx %ld %ld %ld %s\n",
                conf->device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                transfer.stride, transfer.length,
                ((conf->hyper_type == PI_HYPER_TYPE_FLASH) ? "flash" : "ram"));
    __pi_hyper_copy_2d(conf, &transfer, task);
}

void pi_hyper_write_2d(struct pi_device *device, uint32_t hyper_addr, void *addr,
                       uint32_t size, uint32_t stride, uint32_t length)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_hyper_write_2d_async(device, hyper_addr, addr, size, stride, length, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_hyper_write_2d_async(struct pi_device *device, uint32_t hyper_addr,
                             void *addr, uint32_t size, uint32_t stride,
                             uint32_t length, struct pi_task *task)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("Write 2D async device id=%d\n", conf->device_id);
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->cs == 0) ? REG_MBR0 : REG_MBR1;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = (uint32_t) addr;
    transfer.size = size;
    transfer.stride = stride;
    transfer.length = length;
    transfer.channel = TX_CHANNEL;
    HYPER_TRACE("HYPER(%d) TX Transfer : %lx %lx %ld %ld %ld %s\n",
                conf->device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                transfer.stride, transfer.length,
                ((conf->hyper_type == PI_HYPER_TYPE_FLASH) ? "flash" : "ram"));
    __pi_hyper_copy_2d(conf, &transfer, task);
}

int pi_hyper_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct hyper_itf_conf_s *conf = (struct hyper_itf_conf_s *) device->data;
    HYPER_TRACE("HYPER(%ld) ioctl cmd=%ld, arg=%lx\n", conf->device_id, cmd, arg);
    return __pi_hyper_ioctl(conf, cmd, arg);
}
