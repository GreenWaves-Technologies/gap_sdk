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
#include "pmsis_driver/cpi/cpi_internal.h"
#include DEFAULT_MALLOC_INC
#include "pmsis_driver/pmsis_fc_event.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static uint8_t _cpi_init = 0;

extern struct cpi_driver_fifo_s *__global_cpi_driver_fifo[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_cpi_conf_init(struct pi_cpi_conf *conf)
{
    conf->device = PI_DEVICE_CPI_TYPE;
    conf->itf = 0;
}

int pi_cpi_open(struct pi_device *device)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    /* Init CPI periph. */
    if (!_cpi_init)
    {
        /* Make a copy of config just in case. */
        struct pi_cpi_conf *config = (struct pi_cpi_conf *) pi_default_malloc(sizeof(struct pi_cpi_conf));
        if (config == NULL)
        {
            return -1;
        }
        config->device = conf->device;
        config->itf = conf->itf;
        device->config = config;
        /* Init fifo struct for CPI. */
        struct cpi_driver_fifo_s *fifo = (struct cpi_driver_fifo_s *) pi_default_malloc(sizeof(struct cpi_driver_fifo_s));
        if (fifo == NULL)
        {
            pi_default_free(conf, sizeof(struct pi_cpi_conf));
            return -2;
        }
        fifo->hw_buffer[RX_CHANNEL] = NULL;
        #if 0
        fifo->hw_fifo_head = NULL;
        fifo->hw_fifo_tail = NULL;
        #endif
        fifo->fifo_head = NULL;
        fifo->fifo_tail = NULL;
        device->data = (void *) fifo;
        __global_cpi_driver_fifo[conf->itf] = fifo;

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_CPI_RX(conf->itf), cpi_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_CPI_RX(conf->itf));

        /* Disable UDMA CG. */
        udma_init_device(UDMA_CPI_ID(conf->itf));

        _cpi_init = 1;
        return 0;
    }
    return 1;
}

void pi_cpi_close(struct pi_device *device)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    /* Deinit CPI periph. */
    if (_cpi_init)
    {
        /* Clear allocated fifo. */
        struct cpi_driver_fifo_s *fifo = (struct cpi_driver_fifo_s *) device->data;
        pi_default_free(fifo, sizeof(struct cpi_driver_fifo_s));

        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_CPI_RX(conf->itf));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_CPI_RX(conf->itf));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_CPI_ID(conf->itf));

        /* Free allocated config. */
        pi_default_free(conf, sizeof(struct pi_cpi_conf));

        _cpi_init = 0;
    }
}

#define MUTEX 1
void pi_cpi_capture(struct pi_device *device, void *buffer, int32_t bufferlen)
{
    pi_task_t task_block;
    #if MUTEX
    pi_task_block(&task_block);
    pi_cpi_capture_async(device, buffer, bufferlen, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    #else
    pi_task_block_no_mutex(&task_block);
    pi_cpi_capture_async(device, buffer, bufferlen, &task_block);
    pi_task_wait_on_no_mutex(&task_block);
    #endif
}

void pi_cpi_capture_async(struct pi_device *device, void *buffer,
                          int32_t bufferlen, struct pi_task *task)
{
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    struct cpi_transfer_s transfer = {0};
    transfer.buffer = buffer;
    transfer.size = bufferlen;
    transfer.device_id = conf->itf;
    DEBUG_PRINTF("CPI(%d) RX Transfer : %x %d\n",
                 transfer.device_id, transfer.buffer, transfer.size);
    __pi_cpi_copy((struct cpi_driver_fifo_s *) device->data, &transfer, task);
}
