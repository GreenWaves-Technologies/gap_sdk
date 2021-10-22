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
#pragma once
#define PI_INLINE_HYPER_LVL_0 static inline
#include "pmsis.h"
#include "pmsis/implem/drivers/udma/hyperbus/hyperbus_internal.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/
/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Inner functions.
 ******************************************************************************/

/*******************************************************************************
 * PMSIS FC functions implem
 ******************************************************************************/


PI_INLINE_HYPER_LVL_0
void pi_hyper_read(struct pi_device *device, uint32_t hyper_addr,
                   void *buffer, uint32_t size)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_read_async(device, hyper_addr, buffer, size, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_read_async(struct pi_device *device, uint32_t hyper_addr,
                         void *buffer, uint32_t size, struct pi_task *callback)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = buffer;
    transfer.size = size;
    transfer.stride = 0;
    transfer.channel = RX_CHANNEL;
    transfer.device_id = conf->id;
    HYPER_TRACE("HYPER(%d) RX Transfer : %x %x %d %x\n",
                transfer.device_id, transfer.hyper_addr, transfer.buffer, transfer.size, conf->type);
    __pi_hyper_copy(transfer.device_id, &transfer, callback);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_write(struct pi_device *device, uint32_t hyper_addr,
                    void *buffer, uint32_t size)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_write_async(device, hyper_addr, buffer, size, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_write_async(struct pi_device *device, uint32_t hyper_addr,
                          void *buffer, uint32_t size, struct pi_task *callback)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *)device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = buffer;
    transfer.size = size;
    transfer.stride = 0;
    transfer.channel = TX_CHANNEL;
    transfer.device_id = conf->id;
    HYPER_TRACE("HYPER(%d) TX Transfer : %x %x %d %x\n",
                transfer.device_id, transfer.hyper_addr, transfer.buffer, transfer.size, conf->type);
    __pi_hyper_copy(transfer.device_id, &transfer, callback);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_read_2d(struct pi_device *device, uint32_t hyper_addr,
                      void *buffer, uint32_t size,
                      uint32_t stride, uint32_t length)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_read_2d_async(device, hyper_addr, buffer, size, stride, length, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_read_2d_async(struct pi_device *device, uint32_t hyper_addr,
                            void *buffer, uint32_t size,
                            uint32_t stride, uint32_t length, struct pi_task *callback)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = buffer;
    transfer.size = size;
    transfer.stride = stride;
    transfer.length = length;
    transfer.channel = RX_CHANNEL;
    transfer.device_id = conf->id;
    HYPER_TRACE("HYPER(%d) RX Transfer : %x %x %d %d %d %x\n",
                transfer.device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                transfer.stride, transfer.length, conf->type);
    __pi_hyper_copy_2d(conf->id, &transfer, callback);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_write_2d(struct pi_device *device, uint32_t hyper_addr,
                       void *buffer, uint32_t size,
                       uint32_t stride, uint32_t length)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_write_2d_async(device, hyper_addr, buffer, size, stride, length, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

PI_INLINE_HYPER_LVL_0
void pi_hyper_write_2d_async(struct pi_device *device, uint32_t hyper_addr,
                             void *buffer, uint32_t size,
                             uint32_t stride, uint32_t length, struct pi_task *callback)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    struct hyper_transfer_s transfer = {0};
    transfer.hyper_addr = ext_addr;
    transfer.buffer = buffer;
    transfer.size = size;
    transfer.stride = stride;
    transfer.length = length;
    transfer.channel = TX_CHANNEL;
    transfer.device_id = conf->id;
    HYPER_TRACE("HYPER(%d) TX Transfer : %x %x %d %d %d %x\n",
                transfer.device_id, transfer.hyper_addr, transfer.buffer, transfer.size,
                transfer.stride, transfer.length, conf->type);
    __pi_hyper_copy_2d(conf->id, &transfer, callback);
}
