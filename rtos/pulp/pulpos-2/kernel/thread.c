/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"



PI_FC_L1_TINY pi_thread_queue_t __pi_thread_ready_queues[PI_THREAD_MAX_PRIORITIES];
PI_FC_L1_TINY uint32_t __pi_thread_ready;
static PI_FC_L1_TINY pi_thread_t __pi_thread_main;
PI_FC_L1_TINY pi_thread_t *__pi_thread_current;


extern void __pi_thread_start();
void pi_thread_exit(void *status);


static inline void __attribute__((always_inline)) __pi_thread_enqueue(pi_thread_queue_t *queue, pi_thread_t *thread)
{
    if (queue->first == NULL)
    {
        queue->first = thread;
    }
    else
    {
        queue->last->next = thread;
    }

    thread->next = NULL;
    queue->last = thread;
}


static inline __attribute__((always_inline)) void __pi_thread_enqueue_ready(pi_thread_t *thread)
{
    int priority = thread->priority;
  __pi_thread_enqueue(&__pi_thread_ready_queues[priority], thread);
  __pi_thread_ready = __BITSET_R(__pi_thread_ready, 1, priority);
}


static void __pi_thread_queue_init(pi_thread_queue_t *queue)
{
    queue->first = NULL;
}


static void __pi_thread_init(pi_thread_t *thread, void *(*entry)(void *), void *arg,
    void *stack, unsigned int stack_size)
{
    thread->regs.sp = (uint32_t)stack + stack_size;
    thread->regs.ra = (int)__pi_thread_start;
    thread->regs.s0 = (int)entry;
    thread->regs.s1 = (int)arg;
    thread->regs.s2 = (int)pi_thread_exit;
}



int pi_thread_create(pi_thread_t *thread, void *(*entry)(void *), void *arg, int priority,
    void *stack, unsigned int stack_size)
{
    int irq = pi_irq_disable();
    thread->waiting = NULL;
    thread->finished = 0;
    thread->priority = priority;
    __pi_thread_init(thread, entry, arg, stack, stack_size);
    __pi_thread_enqueue_ready(thread);
    pi_irq_restore(irq);
    return 0;
}


void pi_thread_exit(void *status)
{
    pi_irq_disable();
    pi_thread_t *thread = __pi_thread_current;
    thread->finished = 1;
    thread->status = status;
    if (thread->waiting)
    {
        __pi_thread_enqueue_ready(thread->waiting);
    }

    //__rt_thread_sleep();

}


void *pi_thread_join(pi_thread_t *thread)
{
    int irq = pi_irq_disable();

    if (!thread->finished)
    {
        __pi_thread_current->next = thread->waiting;
        thread->waiting = __pi_thread_current;
        //__rt_thread_sleep();
    }

    void *status = thread->status;

    pi_irq_restore(irq);

    return status;
}




void __pi_thread_switch_to_next()
{
    pi_thread_t *current = __pi_thread_current;
    __pi_thread_current = __pi_thread_dequeue_ready();
    __pi_thread_switch(current, __pi_thread_current);
}


void __pi_thread_sched_init()
{
    for (int i=0; i<PI_THREAD_MAX_PRIORITIES; i++)
    {
        __pi_thread_queue_init(&__pi_thread_ready_queues[i]);
    }
  __pi_thread_current = &__pi_thread_main;
  __pi_thread_ready = 0;
}


void __pi_thread_enqueue_list(pi_thread_t *thread)
{
    do
    {
        pi_thread_t *next = thread->next;
        __pi_thread_enqueue_ready(thread);
        thread = next;
    } while (thread);
}