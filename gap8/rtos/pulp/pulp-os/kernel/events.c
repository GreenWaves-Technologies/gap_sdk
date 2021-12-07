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
#include "stdio.h"

RT_FC_TINY_DATA rt_event_sched_t   __rt_sched;
RT_FC_TINY_DATA rt_event_t        *__rt_first_free;

void rt_event_sched_init(rt_event_sched_t *sched)
{
  sched->first = NULL;
}

void __rt_event_init(rt_event_t *event, rt_event_sched_t *sched)
{
  __rt_event_min_init(event);
#if PULP_CHIP_FAMILY == CHIP_GAP || !defined(ARCHI_HAS_FC)
  event->implem.copy.periph_data = (char *)rt_alloc(RT_ALLOC_PERIPH, RT_PERIPH_COPY_PERIPH_DATA_SIZE);
#endif
  event->implem.saved_pending = 0;
  event->arg[0] = 0;
}

rt_event_t *__rt_wait_event_prepare_blocking()
{
  rt_event_t *event = __rt_first_free;
  __rt_first_free = event->next;
  __rt_event_min_init(event);
  event->implem.pending = 1;
  event->arg[0] = 0;
  return event;
}

int rt_event_alloc(rt_event_sched_t *sched, int nb_events)
{
  int irq = rt_irq_disable();

  sched = __rt_event_get_current_sched();

  int flags;
  if (rt_is_fc()) flags = RT_ALLOC_FC_DATA;
  else flags = RT_ALLOC_CL_DATA + rt_cluster_id();

  rt_event_t *event = (rt_event_t *)rt_alloc(flags, sizeof(rt_event_t)*nb_events);
  if (event == NULL) return -1;

  for (int i=0; i<nb_events; i++) {
    __rt_event_init(event, sched);
    event->next = __rt_first_free;
    __rt_first_free = event;
    event++;
  }

  rt_irq_restore(irq);
  return 0;
}

void __rt_event_free(rt_event_t *event)
{
#if PULP_CHIP_FAMILY == CHIP_GAP
  rt_free(RT_ALLOC_PERIPH, (void *)event->implem.copy.periph_data, RT_PERIPH_COPY_PERIPH_DATA_SIZE);
#endif  
  rt_free(RT_ALLOC_FC_DATA, (void *)event, sizeof(rt_event_t));
}

void rt_event_free(rt_event_sched_t *sched, int nb_events)
{
  for (int i=0; i<nb_events; i++)
  {
    rt_event_t *event = __rt_first_free;
    __rt_first_free = event->next;
    __rt_event_free(event);
  }
}

static inline __attribute__((always_inline)) rt_event_t *__rt_get_event(rt_event_sched_t *sched, void (*callback)(void *), void *arg)
{
  // Get event from scheduler and initialize it
  rt_event_t *event = __rt_first_free;
  if (event == NULL) return NULL;
  __rt_first_free = event->next;
  event->arg[0] = (intptr_t)callback;
  event->arg[1] = (intptr_t)arg;
  return event;
}

rt_event_t *rt_event_get(rt_event_sched_t *sched, void (*callback)(void *), void *arg)
{
  int irq = rt_irq_disable();
  sched = __rt_event_get_current_sched();
  rt_event_t *event = __rt_get_event(sched, callback, arg);
  rt_irq_restore(irq);
  return event;
}

rt_event_t *rt_event_get_permanent(rt_event_sched_t *sched, void (*callback)(void *), void *arg)
{
  int irq = rt_irq_disable();
  sched = __rt_event_get_current_sched();
  rt_event_t *event = __rt_get_event(sched, callback, arg);
  if (event)
  {
    __rt_event_set_keep(event);
  }
  rt_irq_restore(irq);
  return event;
}

rt_event_t *rt_event_get_blocking(rt_event_sched_t *sched)
{
  int irq = rt_irq_disable();
  sched = __rt_event_get_current_sched();
  rt_event_t *event = __rt_get_event(sched, NULL, NULL);
  if (event) {
    event->implem.pending = 1;
  }
  rt_irq_restore(irq);
  return event;
}

void rt_event_push(rt_event_t *event)
{
  int irq = rt_irq_disable();
  __rt_push_event(rt_event_internal_sched(), event);
  rt_irq_restore(irq);
}

int rt_event_push_callback(rt_event_sched_t *sched, void (*callback)(void *), void *arg)
{
  int irq = rt_irq_disable();
  sched = __rt_event_get_current_sched();
  rt_event_t *event = __rt_get_event(sched, callback, arg);
  if (event == NULL) return -1;
  __rt_push_event(sched, event);
  rt_irq_restore(irq);
  return 0;
}

void __rt_event_unblock(rt_event_t *event)
{
  event->implem.pending = 0;
}

void __rt_sched_event_cancel(rt_event_t *event)
{
  rt_event_sched_t *sched = rt_event_internal_sched();
  rt_event_t *current = sched->first, *prev = NULL;
  while (current && current != event)
  {
    prev = current;
    current = current->next;
  }

  if (current)
  {
    if (prev)
      prev->next = current->next;
    else
      sched->first = current->next;
  }
}

void __rt_event_yield(rt_event_sched_t *sched)
{
  __rt_event_execute(sched, 0);
  rt_wait_for_interrupt();
  rt_irq_enable();
  rt_irq_disable();
}

void __rt_event_execute(rt_event_sched_t *sched, int wait)
{
  sched = __rt_event_get_current_sched();
  rt_event_t *event = sched->first;

  if (event == NULL) {
    if (wait) {
      // Pop first event from the queue. Loop until we pop a null event
      // We must always read again the queue head, as the executed
      // callback can modify the queue 
      rt_wait_for_interrupt();
      rt_irq_enable();
#if defined(ARCHI_CORE_RISCV_ITC)
      // TODO temporary work-around until HW bug is fixed
      asm volatile ("nop");
#endif
      rt_irq_disable();
      event = *((rt_event_t * volatile *)&sched->first);
      if (event == NULL)
      {
        return;
      }
    } else {
      return;
    }
  }

  do {
    sched->first = event->next;

    // Read event information and put it back in the scheduler

    void (*callback)(void *) = (void (*)(void *))event->arg[0];
    void *arg = (void *)event->arg[1];

    event->done = 1;

    // Free the event now so that it can be used directly from the callback
    if (!event->implem.keep && !event->implem.pending)
    {
      __rt_event_release(event);
    }

    __rt_event_unblock(event);

    // Finally execute the event with interrupts enabled
    if (callback) {
      rt_irq_enable();
      callback(arg);
      rt_irq_disable();
    }

    event = sched->first;

  } while(event);

}

void __rt_wait_event(rt_event_t *event)
{
  while (event->implem.pending || event->implem.saved_pending) {
    __rt_event_execute(NULL, 1);
  }

  event->next = __rt_first_free;
  __rt_first_free = event;
}

void rt_event_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();
__rt_wait_event(event);
  rt_irq_restore(irq);
}

void __rt_event_sched_init()
{
  __rt_first_free = NULL;
  rt_event_sched_init(&__rt_sched);
  // Push one event ot the runtime scheduler as some runtime services need
  // one event.
  rt_event_alloc(&__rt_sched, 1);
}

void pi_task_push_delayed_us(pi_task_t *task, uint32_t delay)
{
  rt_event_push_delayed(task, delay);
}

#ifdef ARCHI_HAS_CLUSTER

void pi_cl_send_task_to_fc(pi_task_t *task)
{
  __rt_task_init_from_cluster(task);
  __rt_cluster_push_fc_event(task);
}

#endif