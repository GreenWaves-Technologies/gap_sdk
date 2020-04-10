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

#if defined(FEATURE_CLUSTER)

#ifndef __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__
#define __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_uart_req_s
{
    pi_task_t req_cb;    /*!< Event task used to send the delegate transfer. */
    pi_task_t task_done; /*!< Event task used to check EOT. */
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

static inline void pi_cl_uart_write_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&(req->task_done));
    hal_compiler_barrier();
}

static inline void pi_cl_uart_read_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&(req->task_done));
    hal_compiler_barrier();
}


#endif  /* __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__ */

#endif  /* FEATURE_CLUSTER */
