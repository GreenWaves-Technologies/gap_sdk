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

#include "pmsis.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/implem/drivers/pmsis_it.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

extern struct cluster_driver_data *__per_cluster_data[];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void cl_notify_fc_event_handler(void)
{
    struct cluster_driver_data *data = __per_cluster_data[0];
    struct pi_task *task = data->task_to_fc;
    pi_callback_func_t callback_func = NULL;
    /* Light callback executed now. */
    if ((uint32_t) task & 0x1)
    {
        task = (pi_task_t *) ((uint32_t) task & ~0x1);
        callback_func = (pi_callback_func_t) task->arg[0];
        callback_func((void*) task->arg[1]);
    }
    /* Push pi_task callback to event kernel. */
    else
    {
        pi_task_push(task);
    }
    hal_compiler_barrier();
    data->task_to_fc = NULL;
    hal_cl_eu_glob_sw_event_trigger(0, FC_TO_CLUSTER_NOTIFY_EVENT, 0);
}

void cl_wait_task(uint8_t *done)
{
    while ((*(volatile uint8_t *) done) == 0)
    {
        hal_cl_eu_event_mask_wait_and_clear(1 << FC_NOTIFY_CLUSTER_EVENT);
    }
}

void cl_notify_task_done(uint8_t *done, uint8_t cluster_id)
{
    (*(volatile uint8_t *) done) = 1;
    hal_cl_eu_glob_sw_event_trigger(0, FC_NOTIFY_CLUSTER_EVENT, 0);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_cl_send_task_to_fc(pi_task_t *task)
{
    hal_cl_eu_mutex_lock(0);
    struct cluster_driver_data *data = __per_cluster_data[0];
    while (data->task_to_fc != NULL)
    {
        hal_compiler_barrier();
        hal_cl_eu_event_mask_wait_and_clear(1 << FC_NOTIFY_CLUSTER_EVENT);
        hal_compiler_barrier();
    }
    data->task_to_fc = task;
    hal_itc_irq_set(CLUSTER_TO_FC_NOTIFY_IRQN);
    hal_cl_eu_mutex_unlock(0);
}

void mc_fc_delegate_init(void *arg)
{
    /* Activate interrupt handler for FC when cluster want to push a task to FC */
    hal_itc_mask_set(CLUSTER_TO_FC_NOTIFY_IRQN);
    /* Set DMA handler. */
    extern void cl_dma_handler(void);
    extern uint8_t __irq_cluster_vector_base_m__;
    uint32_t cl_dma_irq = (uint32_t) CL_IRQ_DMA1;
    uint32_t *cl_vector = (uint32_t *) &__irq_cluster_vector_base_m__;
    cl_vector[cl_dma_irq] = pi_irq_get_itvec((uint32_t) cl_vector,
                                             cl_dma_irq,
                                             (uint32_t) cl_dma_handler);
    return;
}
