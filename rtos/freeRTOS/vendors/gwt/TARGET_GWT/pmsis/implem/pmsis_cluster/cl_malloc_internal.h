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

#ifndef __PI_CL_L2_MALLOC_H__
#define __PI_CL_L2_MALLOC_H__

#if defined(FEATURE_CLUSTER)

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_alloc_req_s
{
    void *chunk;
    int32_t size;
    uint8_t done;
    pi_task_t cb;
    uint8_t cid;
};

struct pi_cl_free_req_s
{
    void *chunk;
    int32_t size;
    uint8_t done;
    pi_task_t cb;
    uint8_t cid;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void *pi_cl_l2_malloc_wait(pi_cl_alloc_req_t *req)
{
    cl_wait_task(&(req->done));
    return req->chunk;
}

static inline void pi_cl_l2_free_wait(pi_cl_free_req_t *req)
{
    cl_wait_task(&(req->done));
}

#endif  /* FEATURE_CLUSTER */

#endif  /* __PI_CL_L2_MALLOC_H__ */
