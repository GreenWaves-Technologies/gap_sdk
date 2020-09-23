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
 * \brief Prepare an event task with callback.
 *
 * This function initializes an instance of event task.
 * This event task executes the callback given in argument.
 *
 * \param callback_task  Pointer to event task.
 * \param func           Callback function.
 * \param arg            Callback function argument.
 *
 * \return task This function returns the event task initialized.
 */
pi_task_t *__pi_task_callback(pi_task_t *callback_task,
                              void (*func)(void *), void *arg);

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
    return __pi_task_callback(task, callback, arg);
}

static inline void pi_task_wait_on(pi_task_t *task)
{
    __pi_task_wait_on(task);
}

static inline void pi_task_push(pi_task_t *task)
{
    __pi_task_push(task);
}

static inline void pi_task_destroy(pi_task_t *task)
{
    __pi_task_destroy(task);
}

#endif  /* __PI_RTOS_PMSIS_TASK_H__ */
