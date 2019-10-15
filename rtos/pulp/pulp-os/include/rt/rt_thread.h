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

#ifndef __RT_RT_THREAD_H__
#define __RT_RT_THREAD_H__

#include "rt/rt_data.h"

/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Threading Thread scheduler
 *
 * The runtime on the fabric controller is always running a thread scheduler. This scheduler is priority-based and preemptive.
 * Only the ready threads of the highest priority are scheduled following a round-robin policy.
 * Threads are preempted at a fixed frequency in order to let other threads of the same priority run.
 *
 * The main function of the application is entered from the initial thread.
 * If the application does not need multiple threads, the following API is not needed as the whole
 * application will execute inside the initial thread.
 * 
 */

/**        
 * @addtogroup Threading
 * @{        
 */

/**@{*/

/** \brief Create a thread.
 *
 * Creates a thread. The thread is created active and put in the list of ready threads and can be scheduled immediately.
 * \param thread     Pointer to the thread structure. This structure must be allocated by the caller and must be kept allocated until the thread has been joined.
 * \param entry      The thread entry point. As soon as the thread is scheduled for the first time, it will execute this function.
 * \param arg        The thread entry point argument. The entry point is called with this argument.
 * \param stack      Thread stack. It must be allocated by the caller and be allocated anywhere. If it is NULL, the runtime will allocate the stack.
 * \param stack_size Thread stack size. If a stack is given, this indicates its size. If no stack is given, this specifies the required stack size. If it is 0, the runtime will take a default stack size.
 * \return           0 if successful, -1 otherwise
 */
int rt_thread_create(rt_thread_t *thread, void *(*entry)(void *), void *arg, unsigned int stack, unsigned int stack_size);

/** \brief Yield the processor.
 *
 * This asks the scheduler to scheduler the next thread in the ready list. If no other thread is ready, the calling thread just continues execution.
 */
void rt_thread_yield();

/** \brief Join a thread.
 *
 * This blocks the execution of the calling thread until the specified thread has finished execution.
 * \param thread   The thread which the calling thread will wait for.
 * \return         The exit status returned by the thread.
 */
void *rt_thread_join(rt_thread_t *thread);

/** \brief Exit the calling thread.
 *
 * This immediately stops the execution of the calling thread which will not be scheduled anymore.
 * \param status   The exit status of the thread, which can be retrieved through thread_join/
 */
void rt_thread_exit(void *status);

//!@}

/**        
 * @} end of Threading group        
 */



/// @cond IMPLEM

extern rt_thread_queue_t __rt_ready_queue;

extern rt_thread_t *__rt_thread_current;

extern RT_FC_TINY_DATA rt_event_sched_t __rt_sched;

static inline void __rt_thread_enqueue(rt_thread_queue_t *queue, rt_thread_t *thread)
{
  thread->next = NULL;
  if (queue->first == NULL) {
    queue->first = thread;
  } else {
    queue->last->next = thread;
  }
  queue->last = thread;
}

void __rt_thread_sleep();

void __rt_thread_set_sched(rt_thread_t *thread, rt_event_sched_t *sched);

static inline void __rt_thread_enqueue_ready(rt_thread_t *thread)
{
  __rt_thread_enqueue(&__rt_ready_queue, thread);
  thread->state = RT_THREAD_STATE_READY;
}

static inline rt_thread_t *__rt_thread_dequeue_first(rt_thread_queue_t *queue)
{
  rt_thread_t *result = queue->first;
  if (result) {
    queue->first = result->next;
  }
  return result;
}

static inline rt_thread_t *__rt_thread_dequeue_ready()
{ 
  rt_thread_t *thread = __rt_thread_dequeue_first(&__rt_ready_queue);
  if (thread) thread->state = RT_THREAD_STATE_OTHER;
  return thread;
}

static inline __attribute__((always_inline)) void __rt_thread_enqueue_ready_check(rt_thread_t *thread)
{
  if (thread->state != RT_THREAD_STATE_READY) {
    __rt_thread_enqueue_ready(thread);
  }
}

void __rt_thread_wakeup(rt_thread_t *thread);

void __rt_thread_sched_init();

/// @endcond

#endif
