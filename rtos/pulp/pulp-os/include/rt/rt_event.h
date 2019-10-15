/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __RT_RT_EVENT_H__
#define __RT_RT_EVENT_H__

/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Event Event scheduler
 *
 * The asynchronous interactions between the fabric controller, the cluster and the peripherals are managed
 * with events on the fabric controller side.
 *
 * An event is a function callback which can be pushed to an event scheduler, for a deferred execution. 
 * All events in the same scheduler are executed in-order, in a FIFO manner.
 *
 * In order to manage several levels of event priorities, there may be several event schedulers at the same
 * time. The application is entered with one event scheduler already created in the runtime, which is the
 * one which can be used as the default scheduler. However the application can then create multiple
 * event schedulers in order to build a more complex multi-priority scenario.
 *
 * Event schedulers are only executed when specific actions are executed by the application. They can
 * be either invoked explicitly by calling rt_event_execute or implicitly when for example no thread
 * is active and the application is blocked waiting for something.
 *
 * The pool of events which can be pushed to schedulers must be explicitly managed by the application.
 * There is by default no event allocated. They must be first allocated before they can be pushed to
 * schedulers. This allows the application to decide the optimal number of events that it needs
 * depending on the various actions which can occur.
 *
 * When a scheduler is invoked, it executes all its pending events, unless the current thread is preempted
 * to schedule another thread. To execute an event, it removes it from the queue and executes the associated
 * callback with its argument, until the callback returns. This scheduler can not execute any other event
 * until the callback has returned.
 *
 * Each thread is assigned an event scheduler. By default, when a thread is created, it is assigned
 * the internal event scheduler. Another scheduler can then be assigned by the application.
 * This scheduler is the one implicitly invoked when the thread is calling a blocking operation such
 * as waiting for an event.
 */

/**        
 * @addtogroup Event
 * @{        
 */

/**@{*/

/** \brief Creates an event scheduler.
 *
 * This initialize the scheduler and makes it usable. As soon as this function is executed, events can be enqueued to the scheduler.
 *
 * \param sched     Pointer to the scheduler structure. This structure must be allocated by the caller and must be kept allocated until the scheduler is not used anymore.
 */
void rt_event_sched_init(rt_event_sched_t *sched);

/** \brief Allocate new events.
 *
 * Allocates the specified number of events and put them in the free list. These events can then be reserved through other functions and pushed to the scheduler.
 * Note that this function can be called several times in order to add more events to the scheduler.
 *
 * \param sched     The scheduler for which the events are intended to be used. If NULL the default scheduler for the current thread is used.
 * \param nb_events The number of events to allocate.
 * \return 0 if successfull, 1 otherwise.
 */
int rt_event_alloc(rt_event_sched_t *sched, int nb_events);

/** \brief Free events.
 *
 * Removes the specified number of events from the free list and frees them.
 *
 * \param sched     The scheduler for which the events must be freed
 * \param nb_events The number of events to free.
 */
void rt_event_free(rt_event_sched_t *sched, int nb_events);

/** \brief Reserve an event and set its callback and argument.
 *
 * This gets an event from the free list and initializes it with the specified callback.
 * The event is then ready to be pushed to the scheduler.
 *
 * \param sched    The scheduler for which to get the event for.
 * \param callback The function which will be called when the event is executed.
 * \param arg      The argument of the function callback.
 * \return         The assigned event if there was at least one available, or NULL if not.
 */
rt_event_t *rt_event_get(rt_event_sched_t *sched, void (*callback)(void *), void *arg);


/** \brief Reserve a permanent event and set its callback and argument.
 *
 * This gets an event from the free list and initializes it with the specified callback.
 * The event is then ready to be pushed to the scheduler.
 * Compared to normal events, this one is never released and can be
 * reused without being re-allocated, for example for regular actions.
 *
 * \param sched    The scheduler for which to get the event for.
 * \param callback The function which will be called when the event is executed.
 * \param arg      The argument of the function callback.
 * \return         The assigned event if there was at least one available, or NULL if not.
 */
rt_event_t *rt_event_get_permanent(rt_event_sched_t *sched, void (*callback)(void *), void *arg);



/** \brief Reserve an IRQ event and set its callback and argument.
 *
 * This gets an IRQ event from the free list and initializes it with the specified callback.
 * Contrary to classic events, IRQ events are not meant to be pushed
 * to a scheduler, they should instead be used in specific cases
 * where an interrupt handler should execute directly the callback
 * instead of enqueueing the event to a scheduler.
 * These events should be used in very specific cases where it
 * is explicitely mentionned they can be used.
 *
 * \param callback The function which will be called when the event is executed.
 * \param arg      The argument of the function callback.
 * \return         The assigned event if there was at least one available, or NULL if not.
 */
static inline rt_event_t *rt_event_irq_get(void (*callback)(void *), void *arg);



/** \brief Reserve an event for blocking wait.
 *
 * This gets an event from the free list, which can then be used for a blocking wait using
 * rt_event_wait.
 *
 * \param sched    The scheduler for which to get the event for.
 * \return         The assigned event if there was at least one available, or NULL if not.
 */
rt_event_t *rt_event_get_blocking(rt_event_sched_t *sched);



/** \brief Enqueue an event to a scheduler.
 *
 * This pushes the event to its scheduler, and makes it ready to be executed.
 * \param event   The event to be pushed.
 */
void rt_event_push(rt_event_t *event);



/** \brief Enqueue a callback to a scheduler.
 *
 * This pushes a function callback to the specified scheduler. An event is reserved from the scheduler,
 * initialized with the callback, and pushed to the scheduler.
 *
 * \param sched    The scheduler to push the callback on.
 * \param callback The function which will be called when the event is executed.
 * \param arg      The argument of the function callback.
 * \return         0 if successful, 1 otherwise.
 */
int rt_event_push_callback(rt_event_sched_t *sched, void (*callback)(void *), void *arg);



/** \brief Execute all pending events.
 *
 * This will invoke the specified scheduler and execute all its pending events. If no
 * event is ready when the scheduler is invoked and the wait argument is 1, the calling thread
 * is blocked until something happens, otherwise the function just returns.
 *
 * \param sched    The scheduler on which the events are executed.
 * \param wait     If 1, this will block the calling thread until something happens.
 */
static inline void rt_event_execute(rt_event_sched_t *sched, int wait);



/** \brief Execute pending events.
 *
 * This will invoke the specified scheduler and execute all its pending events. If no
 * event is ready when the scheduler is invoked, the calling thread is blocked until
 * something appens.
 *
 * \param sched    The scheduler on which the events are executed.
 */
static inline void rt_event_yield(rt_event_sched_t *sched);



/** \brief Return the internal runtime event scheduler.
 *
 * This returns the runtime event scheduler which is created at boot time and which can be used
 * as a default scheduler.
 *
 * \return The runtime scheduler.
 */
static inline rt_event_sched_t *rt_event_internal_sched();



/** \brief Set a thread event scheduler.
 *
 * This sets the event scheduler of the thread, i.e. the event scheduler which is implicitly
 * invoked when the thread is blocked waiting for something. This means only events from this
 * scheduler will be able to wake-up this thread.
 *
 * \param thread   The thread for which the event scheduler must be set.
 * \param sched    The new scheduler of the thread.
 */
static inline void rt_event_thread_sched(rt_thread_t *thread, rt_event_sched_t *sched);



/** \brief Block the execution until the event has been triggered.
 *
 * The event must be a blocking one, allocated through rt_event_get_blocking.
 *
 * \param event   The event to wait for.
 */
void rt_event_wait(rt_event_t *event);



/** \brief Enqueue an event to a scheduler after the specified amount of time.
 *
 * This first waits that the specified amount of time has passed and then
 * pushes the event to its scheduler, and makes it ready to be executed.
 * The time is specified in microseconds and is a minimum amount of time. The
 * actual time may be bigger due to timer resolution.
 *
 * \param event   The event to be pushed.
 * \param time_us The time in microseconds after which the event is pushed to the scheduler.
 */
void rt_event_push_delayed(rt_event_t *event, int time_us);



//!@}

/**        
 * @} end of Event group        
 */



/// @cond IMPLEM

#include "hal/pulp.h"
#include "rt/rt_alloc.h"

extern RT_FC_TINY_DATA rt_event_t        *__rt_first_free;
extern RT_FC_TINY_DATA rt_event_sched_t   __rt_sched;


static inline rt_event_sched_t *__rt_event_get_current_sched()
{
  return rt_event_internal_sched();
}

static inline void __rt_event_release(rt_event_t *event)
{
  event->implem.next = __rt_first_free;
  __rt_first_free = event;  
}

static inline rt_event_t *rt_event_irq_get(void (*callback)(void *), void *arg)
{
  return (rt_event_t *)((((unsigned int)rt_event_get(NULL, callback, arg)) | 1));
}

void __rt_event_free(rt_event_t *event);

void __rt_sched_event_cancel(rt_event_t *event);

static inline void __rt_event_min_init(rt_event_t *event)
{
  event->implem.pending = 0;
  event->implem.keep = 0;
}

void __rt_event_init(rt_event_t *event, rt_event_sched_t *sched);

static inline void __rt_event_set_pending(rt_event_t *event)
{
  event->implem.pending = 1;  
}

static inline void __rt_event_set_keep(rt_event_t *event)
{
  event->implem.keep = 1;  
}

void __rt_event_execute(rt_event_sched_t *sched, int wait);


void __rt_event_yield(rt_event_sched_t *sched);

static inline void rt_event_execute(rt_event_sched_t *sched, int wait)
{
  int irq = rt_irq_disable();
  __rt_event_execute(__rt_event_get_current_sched(), wait);
  rt_irq_restore(irq);
}

static inline void rt_event_yield(rt_event_sched_t *sched)
{
  rt_event_execute(NULL, 1);
}

static inline rt_event_sched_t *rt_event_internal_sched()
{
  return &__rt_sched;
}

static inline void rt_event_thread_sched(rt_thread_t *thread, rt_event_sched_t *sched)
{
  //if (thread == NULL) thread = __rt_thread_current;
  //thread->sched = sched;
}

void __rt_wait_event(rt_event_t *event);

static inline void __rt_wait_event_check(rt_event_t *event, rt_event_t *call_event)
{
  if (event == NULL) __rt_wait_event(call_event);
}

static inline void __rt_wait_event_check_irq(rt_event_t *event, rt_event_t *call_event)
{
  int irq = rt_irq_disable();
  if (event == NULL) __rt_wait_event(call_event);
  rt_irq_restore(irq);
}

rt_event_t *__rt_wait_event_prepare_blocking();

static inline rt_event_t *__rt_wait_event_prepare(rt_event_t *event)
{
  if (likely(event != NULL)) return event;
  return __rt_wait_event_prepare_blocking();
}

static inline void __rt_event_save(rt_event_t *event)
{
  event->implem.saved_pending = event->implem.pending;
  // The user can still check the event while it is being
  // reused. It will also check saved_pending to avoid
  // checking the wrong status.
  rt_compiler_barrier();
  event->implem.saved_callback = (void (*)(void *))event->arg[0];
  event->implem.saved_arg = (void *)event->arg[1];
}

static inline void __rt_event_restore(rt_event_t *event)
{
  event->implem.pending = event->implem.saved_pending;
  event->arg[0] = (uintptr_t)event->implem.saved_callback;
  event->arg[1] = (uintptr_t)event->implem.saved_arg;
  rt_compiler_barrier();
  event->implem.saved_pending = 0;
}

static inline rt_event_t *__rt_init_event(rt_event_t *event, rt_event_sched_t *sched, void (*callback)(void *), void *arg)
{
  __rt_event_min_init(event);
  event->arg[0] = (uintptr_t)callback;
  event->arg[1] = (uintptr_t)arg;
  return event;
}

static inline void __rt_event_enqueue(rt_event_t *event)
{
  rt_event_sched_t *sched = rt_event_internal_sched();
  event->implem.next = NULL;
  if (sched->first) {
    sched->last->implem.next = event;
  } else {
    sched->first = event;
  }
  sched->last = event;
}

void __rt_event_unblock(rt_event_t *event);

static inline void rt_event_enqueue(rt_event_t *event) {
  int irq = rt_irq_disable();
  __rt_event_enqueue(event);
  rt_irq_restore(irq);
}

static inline __attribute__((always_inline)) void __rt_enqueue_event_to_sched(rt_event_sched_t *sched, rt_event_t *event)
{
  event->implem.next = NULL;
  if (sched->first == NULL) {
    sched->first = event;
  } else {
    sched->last->implem.next = event;
  }
  sched->last = event;
}

static inline __attribute__((always_inline)) void __rt_push_event(rt_event_sched_t *sched, rt_event_t *event)
{
  // Enqueue the event into the scheduler tail
  __rt_enqueue_event_to_sched(sched, event);
}

void __rt_event_sched_init();

static inline void __rt_task_init(pi_task_t *task)
{
  task->done = 0;
}

static inline void __rt_task_init_from_cluster(pi_task_t *task)
{
  task->implem.keep = 1;
}

static inline void __rt_event_handle_end_of_task(pi_task_t *task)
{
  rt_event_enqueue(task);
}

/// @endcond

#endif
