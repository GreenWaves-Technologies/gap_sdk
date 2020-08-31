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
    pi_sem_init(&(callback_task->wait_on));
    // lock the mutex so that task may be descheduled while waiting on it
    callback_task->destroy = 1;
    callback_task->core_id = -1;
    return callback_task;
}

pi_task_t *__pi_task_callback(pi_task_t *callback_task,
                              void (*func)(void *), void *arg)
{
    callback_task->id = PI_TASK_CALLBACK_ID;
    callback_task->arg[0] = (uintptr_t)func;
    callback_task->arg[1] = (uintptr_t)arg;
    callback_task->done = 0;
    callback_task->wait_on.sem_object = (void*)NULL;
    callback_task->destroy = 0;
    callback_task->core_id = -1;
    return callback_task;
}

void __pi_task_destroy(pi_task_t *task)
{
    if (task->destroy)
    {
        task->destroy = 0;
        // if the mutex is only virtual (e.g. wait on soc event)
        hal_compiler_barrier();
        // if the sched support semaphore/mutexes
        if (task->wait_on.sem_object)
        {
            pi_sem_deinit(&task->wait_on);
            task->wait_on.sem_object = (void*)NULL;
        }
        hal_compiler_barrier();
    }
}

void __pi_task_wait_on(pi_task_t *task)
{
    // FIXME: workaround for gcc bug
    hal_compiler_barrier();
    while (!task->done)
    {
        // if the underlying scheduler support it, deschedule the task
        if (task->wait_on.sem_object != NULL)
        {
            pi_sem_take(&task->wait_on);
        }
        DEBUG_PRINTF("[%s] waited on sem %p\n", __func__, &(task->wait_on));
    }
    hal_compiler_barrier();
    __pi_task_destroy(task);
}

void __pi_task_push(pi_task_t *task)
{
    uint32_t irq = disable_irq();
    pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    restore_irq(irq);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

pi_task_t *pi_task_callback_no_mutex(pi_task_t *callback_task,
                                     void (*func)(void *), void *arg)
{
    return pi_task_callback(callback_task, func, arg);
}

pi_task_t *pi_task_block_no_mutex(pi_task_t *callback_task)
{
    callback_task->id = PI_TASK_NONE_ID;
    callback_task->done = 0;
    callback_task->wait_on.sem_object = (void*)NULL;
    callback_task->destroy = 0;
    callback_task->core_id = -1;
    // lock the mutex so that task may be descheduled while waiting on it
    return callback_task;
}

void pi_task_release(pi_task_t *task)
{
    DEBUG_PRINTF("[%s] releasing task %p\n",__func__,task);
    // if the mutex is only virtual (e.g. wait on soc event)
    // if the sched support semaphore/mutexes
    if(task->wait_on.sem_object)
    {
        DEBUG_PRINTF("[%s] sem give %p\n",__func__,task->wait_on);
        pi_sem_give(&(task->wait_on));
    }
    hal_compiler_barrier();
    task->done = 1;
    hal_compiler_barrier();
    if(task->core_id != -1)
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
