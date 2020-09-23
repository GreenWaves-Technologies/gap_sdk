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

#include "pmsis/implem/cluster/malloc/cl_malloc_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_cl_delegate_l2_alloc(void *arg);

void __pi_cl_delegate_l2_free(void *arg);

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_cl_delegate_l2_alloc(void *arg)
{
    pi_cl_alloc_req_t *req = (pi_cl_alloc_req_t *) arg;
    req->chunk = pmsis_l2_malloc(req->size);
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_delegate_l2_free(void *arg)
{
    pi_cl_free_req_t *req = (pi_cl_free_req_t *) arg;
    pmsis_l2_malloc_free(req->chunk, req->size);
    req->chunk = NULL;
    cl_notify_task_done(&(req->done), req->cid);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_cl_l2_malloc(int size, pi_cl_alloc_req_t *req)
{
    req->chunk = NULL;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_l2_alloc, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

void pi_cl_l2_free(void *chunk, int size, pi_cl_free_req_t *req)
{
    req->chunk = chunk;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_l2_free, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

#endif  /* (FEATURE_CLUSTER == 1) */
