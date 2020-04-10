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
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static uint8_t __hyper_periph_open = 0;

extern struct hyper_driver_fifo *__global_hyper_driver_fifo[];

static uint32_t read_val = 0, write_val = 0;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Inner functions.
 ******************************************************************************/

/*******************************************************************************
 * PMSIS FC functions implem
 ******************************************************************************/

void pi_hyper_conf_init(struct pi_hyper_conf *conf)
{
    memset(conf, 0, sizeof(struct pi_hyper_conf));
    conf->device = PI_DEVICE_HYPERBUS_TYPE;
    conf->cs = 0;
    conf->type = PI_HYPER_TYPE_RAM;
    conf->id = 0;
}

int32_t pi_hyper_open(struct pi_device *device)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    /* Make a copy of config just in case. */
    struct pi_hyper_conf *config = (struct pi_hyper_conf *) pi_default_malloc(sizeof(struct pi_hyper_conf));
    if (config == NULL)
    {
        return -1;
    }
    config->device = conf->device;
    config->cs = conf->cs;
    config->type = conf->type;
    config->id = conf->id;
    device->config = config;
    /* Init Hyper periph. */
    if (!__hyper_periph_open)
    {
        /* Init event list. */
        struct hyper_driver_fifo *fifo = (struct hyper_driver_fifo *) pi_default_malloc(sizeof(struct hyper_driver_fifo));
        if (fifo == NULL)
        {
            return -1;
        }
        fifo->fifo_head = NULL;
        fifo->fifo_tail = NULL;
        fifo->device_id = config->id;
        fifo->pending_emu_size_2d = 0;
        __global_hyper_driver_fifo[conf->id] = fifo;

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_RX(conf->id), hyper_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_TX(conf->id), hyper_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_TX(conf->id));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_HYPER_ID(conf->id));

        /* Pin settings. */
        __pi_hyper_pin_settings();

        /* Hyper config. */
        __pi_hyper_settings(0);
    }
    /* Signal devices used. */
    __hyper_periph_open++;
    return 0;
}

void pi_hyper_close(struct pi_device *device)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    /* Devices used. */
    __hyper_periph_open--;
    /* Deinit Hyper periph. */
    if (!__hyper_periph_open)
    {
        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_TX(conf->id));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_TX(conf->id));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_HYPER_ID(conf->id));

        /* Free allocated fifo. */
        pi_default_free(__global_hyper_driver_fifo[conf->id], sizeof(struct hyper_driver_fifo));

        /* Pin settings. */
        __pi_hyper_pin_reset_settings();
    }
    /* Free allocated config. */
    pi_default_free(conf, sizeof(struct pi_hyper_conf));
}

void pi_hyper_read(struct pi_device *device, uint32_t hyper_addr,
                   void *buffer, uint32_t size)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_read_async(device, hyper_addr, buffer, size, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

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

void pi_hyper_write(struct pi_device *device, uint32_t hyper_addr,
                    void *buffer, uint32_t size)
{
    pi_task_t task_block;
    pi_task_block(&task_block);

    pi_hyper_write_async(device, hyper_addr, buffer, size, &task_block);

    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

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
