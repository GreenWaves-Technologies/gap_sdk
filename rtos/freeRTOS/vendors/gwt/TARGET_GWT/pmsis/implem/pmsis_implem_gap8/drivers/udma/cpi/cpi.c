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
#include "pmsis/implem/drivers/udma/cpi/cpi_internal.h"
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

void pi_cpi_conf_init(struct pi_cpi_conf *conf)
{
    __pi_cpi_conf_init(conf);
}

int pi_cpi_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    CPI_TRACE("Open device id=%ld\n", conf->itf);
    status = __pi_cpi_open(conf, (struct cpi_itf_data_s **) &(device->data));
    CPI_TRACE("Open status : %ld, driver data : %lx\n",
              status, (struct cpi_itf_data_s *) device->data);
    return status;
}

void pi_cpi_close(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    CPI_TRACE("Close device id=%ld\n", device_data->device_id);
    if (device_data != NULL)
    {
        __pi_cpi_close(device_data);
        device->data = NULL;
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
    struct cpi_itf_data_s *device_data = (struct cpi_itf_data_s *) device->data;
    CPI_TRACE("CPI(%d) RX Transfer : %lx %ld\n",
              device_data->device_id, buffer, bufferlen);
    __pi_cpi_copy(device_data, buffer, bufferlen, task);
}
