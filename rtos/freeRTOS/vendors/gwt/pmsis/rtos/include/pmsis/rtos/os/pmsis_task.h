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

#ifndef __PI_RTOS_PMSIS_TASK_H__
#define __PI_RTOS_PMSIS_TASK_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/rtos/rtos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Prepare a blocking event task.
 *
 * This function initializes an instance of event task.
 * A semaphore will be initialized.
 *
 * \param task           Pointer to event task.
 *
 * \return task This function returns the event task initialized.
 */
pi_task_t *__pi_task_block(pi_task_t *task);

/**
 * \brief Wait on an event task.
 *
 * This function allows to wait on an event task until the event occurs.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_wait_on(pi_task_t *task);

/**
 * \brief Push an event task.
 *
 * This function is used to push an event task to the event kernel.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_push(pi_task_t *task);

/**
 * \brief Delete an event task.
 *
 * This function removes an event task.
 * It will delete the semaphore if it has been allocated.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_destroy(pi_task_t *task);

/**
 * \brief Enqueue a delayed task.
 *
 * This function is used to pushed a delayed event task to event kernel.
 *
 * \param task           Pointer to event task.
 * \param delay_us       Delay to wait for execution of the event task.
 *
 * \note This function is OS dependant.
 */
void pi_task_delayed_fifo_enqueue(struct pi_task *task, uint32_t delay_us);


static inline void __pi_task_push_no_irq(pi_task_t *task)
{
    pmsis_event_push(pmsis_event_get_default_scheduler(), task);
}

static inline void __pi_task_push_exec_irq_safe(pi_task_t *task)
{
    pi_callback_func_t func = (pi_callback_func_t) task->arg[0];
    void *arg = (void *) task->arg[1];
    func(arg);
}

void __pi_task_push_locked(pi_task_t * task);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline pi_task_t *pi_task_block(pi_task_t *task)
{
    return __pi_task_block(task);
}

static inline pi_task_t *pi_task_callback(pi_task_t *task,
                                          void (*callback)(void*), void *arg)
{
    task->id = PI_TASK_CALLBACK_ID;
    task->arg[0] = (uintptr_t) callback;
    task->arg[1] = (uintptr_t) arg;
    task->done = 0;
    task->sync_obj = NULL;
    //task->destroy = 0;
    task->core_id = -1;
    task->timeout = 0;
    task->next = NULL;
    return task;
}

static inline pi_task_t *pi_task_irq_callback(pi_task_t *task,
                                              void (*callback)(void*), void *arg)
{
    task->id = PI_TASK_IRQ_ID;
    task->arg[0] = (uintptr_t) callback;
    task->arg[1] = (uintptr_t) arg;
    return task;
}

static inline void pi_task_wait_on(pi_task_t *task)
{
    __pi_task_wait_on(task);
}

static inline void pi_task_push_irq_safe(pi_task_t *task)
{
    __pi_task_push_locked(task);
}

static inline void pi_task_push(pi_task_t *task)
{
    uint32_t irq = pi_irq_disable();
    __pi_task_push_locked(task);
    pi_irq_restore(irq);
}

static inline void pi_task_destroy(pi_task_t *task)
{
    __pi_task_destroy(task);
}

static inline void pi_task_timeout_set(pi_task_t *task, uint32_t timeout_us)
{
    task->timeout = timeout_us;
}

static inline int32_t pi_task_transfer_end_result_get(pi_task_t *task)
{
    return task->arg[3];
}

static inline int32_t pi_task_status_get(pi_task_t *task)
{
    return task->arg[3];
}

static inline void pi_task_status_set(pi_task_t *task, int32_t status)
{
    task->arg[3] = status;
}

static inline void pi_task_timeout_callback_set(pi_task_t *task, pi_callback_func_t func,
                                                void *arg)
{
    task->arg[2] = (uintptr_t) func;
    task->arg[3] = (uintptr_t) arg;
}

#endif  /* __PI_RTOS_PMSIS_TASK_H__ */
