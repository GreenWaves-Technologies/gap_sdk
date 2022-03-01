/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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
#include "pmsis/task.h"
#include "pmsis/rtos/rtos.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "pmsis/rtos/os/pmsis_task.h"

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

pi_task_t *__pi_task_block(pi_task_t *callback_task)
{
    callback_task->id = PI_TASK_NONE_ID;
    callback_task->done = 0;
    pi_sync_obj_init((void *) &(callback_task->sync_obj));
    //callback_task->destroy = 1;
    callback_task->core_id = -1;
    callback_task->timeout = 0;
    callback_task->next = NULL;
    return callback_task;
}

void __pi_task_destroy(pi_task_t *task)
{
    //if (task->destroy)
    {
        //task->destroy = 0;
        // if the mutex is only virtual (e.g. wait on soc event)
        //hal_compiler_barrier();
        if (task->sync_obj != NULL)
        {
            pi_sync_obj_deinit((void *) &(task->sync_obj));
        }
        //hal_compiler_barrier();
    }
}

void __pi_task_wait_on(pi_task_t *task)
{
    // FIXME: workaround for gcc bug
    hal_compiler_barrier();
    while (!task->done)
    {
        if (task->sync_obj != NULL)
        {
            pi_sync_obj_take(task->sync_obj);
        }
        DEBUG_PRINTF("[%s] waited on sync_obj=%lx\n", __func__, task->sync_obj);
    }
    hal_compiler_barrier();
    __pi_task_destroy(task);
}

#if 0
void __pi_task_push(pi_task_t *task)
{
    uint32_t irq = disable_irq();
    pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    restore_irq(irq);
}
#endif

/*******************************************************************************
 * API implementation
 ******************************************************************************/

pi_task_t *pi_task_callback_no_mutex(pi_task_t *callback_task,
                                     pi_callback_func_t func, void *arg)
{
    return pi_task_callback(callback_task, func, arg);
}

pi_task_t *pi_task_block_no_mutex(pi_task_t *callback_task)
{
    callback_task->id = PI_TASK_NONE_ID;
    callback_task->done = 0;
    callback_task->sync_obj = NULL;
    //callback_task->destroy = 0;
    callback_task->core_id = -1;
    callback_task->timeout = 0;
    callback_task->next = NULL;
    return callback_task;
}

void __pi_task_push_locked(pi_task_t * task)
{
    switch (task->id)
    {
    case PI_TASK_NONE_ID :
        pi_task_release(task);
        break;

    case PI_TASK_CALLBACK_ID :
        __pi_task_push_no_irq(task);
        break;

    case PI_TASK_IRQ_ID :
        __pi_task_push_exec_irq_safe(task);
        break;

    default :
        return;
    }
}

void pi_task_release(pi_task_t *task)
{
    DEBUG_PRINTF("[%s] releasing task %p\n",__func__,task);
    // if the mutex is only virtual (e.g. wait on soc event)
    if (task->sync_obj != NULL)
    {
        pi_sync_obj_release(task->sync_obj);
    }

    hal_compiler_barrier();
    task->done = 1;
    hal_compiler_barrier();
    if (task->core_id != -1)
    {
        pi_cl_pi_task_notify_done(task);
    }
}

void pi_cl_pi_task_wait(pi_task_t *task)
{
    while((*(volatile uint8_t *)&task->done) == 0)
    {
        hal_eu_evt_mask_wait_and_clr(1 << FC_NOTIFY_CLUSTER_EVENT);
    }
}

void pi_cl_pi_task_notify_done(pi_task_t *task)
{
    (*(volatile uint8_t *)(&task->done)) = 1;
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0);
}

void pi_task_wait_on_no_mutex(pi_task_t *task)
{
    // if the mutex is only virtual (e.g. wait on soc event)
    while(!task->done)
    {
        hal_compiler_barrier();
        asm volatile ("nop");
        // FIXME: workaround for gcc bug
        hal_compiler_barrier();
    }
}

void pi_task_push_delayed_us(pi_task_t *task, uint32_t delay)
{
    uint32_t irq = disable_irq();
    pi_task_delayed_fifo_enqueue(task, delay);
    restore_irq(irq);
}

extern void pi_event_kernel_task_remove(struct pmsis_event_kernel_wrap *ek_wrap,
                                        pi_task_t *task);
void pi_task_abort(pi_task_t *task)
{
    uint32_t irq = __disable_irq();
    struct pmsis_event_kernel_wrap *ek_wrap = pmsis_event_get_default_scheduler();
    pi_event_kernel_task_remove(ek_wrap, task);
    __restore_irq(irq);
}
