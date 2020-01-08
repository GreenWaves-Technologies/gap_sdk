/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PMSIS_TASK_H__
#define __PMSIS_TASK_H__

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupRTOS
*/

/**
 * @defgroup Task Task management
 *
 * The asynchronous interactions between the fabric controller, the cluster and
 * the peripherals are managed with events on the fabric controller side.
 *
 * A task is either a function callback which can be pushed for a deferred
 * execution when it is triggered, or notification event, which can used to
 * block the caller execution until it is triggered.
 */

/**
 * @addtogroup Task
 * @{
 */

/**@{*/

/** \brief Prepare a notification event.
 *
 * This initializes a notification event so that it is ready to be
 * triggered.
 * A notification event can be used to block the execution of the caller (using
 * pi_task_wait_on) until a certain action occurs, e.g. an end of transfer. 
 * If the same notification is re-used several times, it must be reinitialized
 * everytime by calling this function or another variant.
 *
 * \param task A pointer to the structure describing the notification
 *   event.
 *   This structure is allocated by the caller and must be kept alive until the
 *   pi_task_wait_on returns.
 */
static inline pi_task_t *pi_task_block(pi_task_t *task);

/** \brief Prepare a notification callback.
 *
 * This initializes a notification callback so that it is ready to be
 * triggered.
 * A notification callback can be used to trigger a function execution when
 * a certain action occurs, e.g. an end of transfer. 
 * If the same notification is re-used several times, it must be reinitialized
 * everytime by calling this function or another variant.
 * A notification callback can not be used to block the caller execution
 * with pi_task_wait_on.
 *
 * \param task  A pointer to the structure describing the notification callback.
 *   This structure is allocated by the caller and must be kept alive until the
 *   notification callback has been entered.
 * \param callback  The callback which will get executed when the
 *   notification is triggered.
 * \param arg The argument to the callback.
 */
pi_task_t *pi_task_callback(pi_task_t *task, void (*callback)(void*),
	void *arg);

/** \brief Wait until a notification event is triggered.
 *
 * This can be called to block the caller until the specified notification
 * event (created with pi_task_block) has been triggered.
 * The notification event is released just before returning from this call
 * and must be reinitialized before it can be re-used.
 *
 * \param task  A pointer to the notification event.
 */
void pi_task_wait_on(pi_task_t *task);

/** \brief Trigger a notification.
 *
 * This can be used to trigger the specified notification. If the notification
 * is a callback, this will schedule the callback execution. If the notification
 * is an event, this will trigger the event.
 *
 * \param task  A pointer to the notification event.
 */
void pi_task_push(pi_task_t *task);

/** \brief Trigger a notification.
 *
 * This can be used to trigger the specified notification after the specified
 * delay, given in micro-seconds. If the notification
 * is a callback, this will schedule the callback execution. If the notification
 * is an event, this will trigger the event.
 *
 * \param task  A pointer to the notification event.
 * \param delay  The number of micro-seconds after which the notification
 *   must be triggered.
 */
void pi_task_push_delayed_us(pi_task_t *task, uint32_t delay);

//!@}

/**
 * @}
 */

/**
 * @}
 */

/// @cond IMPLEM

static inline void pi_task_destroy(pi_task_t *task);

pi_task_t *pi_task_callback_no_mutex(pi_task_t *callback_task, void (*func)(void *), void *arg);

void pi_task_wait_on_no_mutex(pi_task_t *task);

pi_task_t *pi_task_block_no_mutex(pi_task_t *callback_task);

void pi_task_release(pi_task_t *task);

/** \brief Wait on a pi_task from cluster side
 *
 * At the difference of pi_task_wait_on, can't use os facilities directly and 
 * uses a software event instead (os defined)
 *
 * \param task  A pointer to the structure describing the notification callback.
 *   This structure is allocated by the caller and must be kept alive until the
 *   notification callback has been executed/released.
 */
void pi_cl_pi_task_wait(pi_task_t *task);

/** \brief Notify a cluster that task callback has been executed/released
 *
 * Use info contained in task to notify a cluster that pi_task "task" is done.
 * Wake up the cluster cores waiting on said task if need be.
 * Use a software event (os defined)
 *
 * \param task  A pointer to the structure describing the notification callback,
 * contains info necessary for cluster notification.
 *
 */
void pi_cl_pi_task_notify_done(pi_task_t *task);

#ifdef PMSIS_DRIVERS

//#include "pmsis_hal/pmsis_hal.h"
#include "pmsis_backend/pmsis_backend_native_task_api.h"
pi_task_t *__pi_task_block(pi_task_t *callback_task);

static inline struct pi_task *pi_task_block(struct pi_task *callback_task)
{
    __pi_task_block(callback_task);
    return callback_task;
}

void __pi_task_destroy(pi_task_t *task);

static inline void pi_task_destroy(pi_task_t *task)
{
    __pi_task_destroy(task);
}

#endif

/// @endcond

#endif  /* __PMSIS_TASK_H__ */
