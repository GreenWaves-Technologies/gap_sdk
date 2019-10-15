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
#include "pmsis_driver/pmsis_fc_event.h"
#include "pmsis_driver/spi/spi_internal.h"
#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int pi_spi_open(struct pi_device *device)
{
    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;
    int irq = __disable_irq();
    int status = __pi_spi_open((struct spim_cs_data**)(&device->data), conf);
    restore_irq(irq);
    return status;
}

void pi_spi_close(struct pi_device *device)
{
    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;
    int irq = __disable_irq();
    __pi_spi_close(device->data);
    restore_irq(irq);
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    // TODO
}

void pi_spi_send(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_send_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_send_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_send_async(device->data, data, len, flag, task);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_receive_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_receive_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_receive_async(device->data, data, len, flag, task);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flag)
{
    // TODO
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_transfer_async(device, tx_data, rx_data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_transfer_async(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    // TODO
    __pi_spi_xfer_async(device->data, tx_data, rx_data, len, flag, task);
}
