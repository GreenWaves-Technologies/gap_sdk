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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

RT_FC_TINY_DATA rt_thread_queue_t __rt_ready_queue;
RT_FC_GLOBAL_DATA static rt_thread_t __rt_thread_main;
RT_FC_TINY_DATA rt_thread_t *__rt_thread_current;

extern void __rt_thread_switch(rt_thread_t *current, rt_thread_t *new);
extern void __rt_thread_start();


static void __rt_thread_queue_init(rt_thread_queue_t *queue)
{
  queue->first = NULL;
}

static void __rt_thread_init(rt_thread_t *thread, void *(*entry)(void *), void *arg,
  unsigned int stack, unsigned int stack_size)
{
  thread->u.regs.sp = stack + stack_size;
  thread->u.regs.ra = (int)__rt_thread_start;
  thread->u.regs.s0 = (int)entry;
  thread->u.regs.s1 = (int)arg;
  thread->u.regs.s2 = (int)rt_thread_exit;
  thread->state = RT_THREAD_STATE_OTHER;
  __rt_event_init(&thread->event, &__rt_sched);
  thread->event.next = __rt_first_free;
  __rt_first_free = &thread->event;
}

void rt_thread_yield()
{
  int irq = rt_irq_disable();
  rt_thread_t *current = __rt_thread_current;
  __rt_thread_enqueue_ready(current);
  rt_thread_t *new = __rt_thread_dequeue_ready();
  if (new != current) {    
    __rt_thread_current = new;
    __rt_thread_switch(current, new);
  }
  rt_irq_restore(irq);
}

void __rt_thread_sleep()
{
  rt_thread_t *current = __rt_thread_current;
  do {
    rt_thread_t *new = __rt_thread_dequeue_ready();
    if (new) {
      if (new != current) {
        __rt_thread_current = new;
        __rt_thread_switch(current, new);
      }
      break;
    }
    rt_wait_for_interrupt();
    rt_irq_enable();
    rt_irq_disable();
  } while (1);
}

void __rt_thread_wakeup(rt_thread_t *thread)
{
  if (thread->next == NULL && thread != __rt_thread_current) {
    __rt_thread_enqueue_ready_check(thread);
  }
}

int rt_thread_create(rt_thread_t *thread, void *(*entry)(void *), void *arg, unsigned int stack, unsigned int stack_size)
{
  int irq = rt_irq_disable();
  thread->waiting = NULL;
  thread->finished = 0;
  __rt_thread_init(thread, entry, arg, stack, stack_size);
  __rt_thread_enqueue_ready(thread);
  rt_irq_restore(irq);
  return 0;
}

void rt_thread_exit(void *status)
{
  rt_irq_disable();
  rt_thread_t *thread = __rt_thread_current;
  thread->finished = 1;
  thread->status = status;
  if (thread->waiting) {
    __rt_thread_enqueue_ready_check(thread->waiting);
  }

  __rt_thread_sleep();

}

void *rt_thread_join(rt_thread_t *thread)
{
  int irq = rt_irq_disable();

  if (!thread->finished) {
    thread->waiting = __rt_thread_current;
    __rt_thread_sleep();
  }

  void *status = thread->status;

  rt_irq_restore(irq);

  return status;
}

void __rt_thread_sched_init()
{
  __rt_thread_queue_init(&__rt_ready_queue);
  __rt_thread_init(&__rt_thread_main, NULL, NULL, 0, 0);
  __rt_thread_current = &__rt_thread_main;
}
