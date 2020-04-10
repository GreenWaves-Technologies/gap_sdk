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

#if (FEATURE_CLUSTER == 1)

#include "pmsis.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/implem/cluster/delegate/hyperbus/hyperbus_cl_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static pi_task_t task_delegate_exec;   /*!< pi_task used to keep info of current delegate. */
static pi_cl_hyper_req_t *fifo = NULL; /*!< FIFO of hyper req. */

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Signal a delegate is done, it wakes up cluster. */
void __pi_cl_hyper_copy_task_done(void *arg);

/*******************************************************************************
 * Function implementation.
 ******************************************************************************/

static void __pi_cl_delegate_hyper_copy_exec(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    struct pi_hyper_conf *conf = req->device->config;
    pi_task_callback_no_mutex(&task_delegate_exec, __pi_cl_hyper_copy_task_done, (void *) req);
    if (req->is_2d)
    {
        __pi_hyper_copy_2d(conf->id, &(req->transfer), &task_delegate_exec);
    }
    else
    {
        __pi_hyper_copy(conf->id, &(req->transfer), &task_delegate_exec);
    }
}

static void __pi_cl_delegate_hyper_copy(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    if (fifo == NULL)
    {
        fifo = req;
        fifo->next = NULL;
        __pi_cl_delegate_hyper_copy_exec(fifo);
    }
    else
    {
        fifo->next = req;
        fifo->next->next = NULL;
    }
}

void __pi_cl_hyper_copy_task_done(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    fifo = fifo->next;
    cl_notify_task_done(&(req->done), req->cid);
    if (fifo != NULL)
    {
        __pi_cl_delegate_hyper_copy_exec(fifo);
    }
}

void __pi_cl_hyper_copy_task_wait(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    cl_wait_task(&(req->done));
}

void __pi_cl_hyper_copy(struct pi_device *device, uint32_t hyper_addr, void *buffer,
                        uint32_t size, udma_channel_e read, pi_cl_hyper_req_t *req)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    /* Transfer struct. */
    req->transfer.hyper_addr = ext_addr;
    req->transfer.buffer = buffer;
    req->transfer.size = size;
    req->transfer.stride = 0;
    req->transfer.length = 0;
    req->transfer.channel = read;
    req->transfer.device_id = conf->id;
    req->device = device;
    req->done = 0;
    req->cid = pi_cluster_id();
    req->is_2d = 0;
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_copy, req);
    pi_cl_send_task_to_fc(&(req->cb));
}

void __pi_cl_hyper_copy_2d(struct pi_device *device, uint32_t hyper_addr,
                           void *buffer, uint32_t size, uint32_t stride,
                           uint32_t length, udma_channel_e read, pi_cl_hyper_req_t *req)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    /* Transfer struct. */
    req->transfer.hyper_addr = ext_addr;
    req->transfer.buffer = buffer;
    req->transfer.size = size;
    req->transfer.stride = stride;
    req->transfer.length = length;
    req->transfer.channel = read;
    req->transfer.device_id = conf->id;
    req->device = device;
    req->done = 0;
    req->cid = pi_cluster_id();
    req->is_2d = 1;
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_copy, req);
    pi_cl_send_task_to_fc(&(req->cb));
}

static void __pi_cl_hyper_alloc_task_done(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_hyper_alloc_task_wait(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    cl_wait_task(&(req->done));
}

static void __pi_cl_delegate_hyper_alloc(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    req->chunk = pi_hyperram_alloc(req->device, req->size);
    __pi_cl_hyper_alloc_task_done(arg);
}

void pi_cl_hyperram_alloc(struct pi_device *device, uint32_t size,
                          pi_cl_hyperram_alloc_req_t *req)
{
    req->device = device;
    req->chunk = 0;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_alloc, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

static void __pi_cl_hyper_free_task_done(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_hyper_free_task_wait(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    cl_wait_task(&(req->done));
}

static void __pi_cl_delegate_hyper_free(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    req->chunk = pi_hyperram_free(req->device, req->chunk, req->size);
    __pi_cl_hyper_free_task_done(arg);
}

void pi_cl_hyperram_free(struct pi_device *device, uint32_t chunk, uint32_t size,
                         pi_cl_hyperram_free_req_t *req)
{
    req->device = device;
    req->chunk = chunk;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_free, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

#endif  /* (FEATURE_CLUSTER == 1) */
