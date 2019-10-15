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

static uint32_t timer_count;
rt_event_t *first_delayed;



static int __rt_time_poweroff(void *arg)
{
  // Remember the current timer count so that we can restore it
  // when the system is powered-on
  timer_count = timer_count_get(timer_base_fc(0, 1));

  return 0;
}

static int __rt_time_poweron(void *arg)
{
  // Restore the timer count we saved before shutdown
  timer_count_set(timer_base_fc(0, 1), timer_count);

  return 0;
}

unsigned int rt_time_get_us()
{
  // Get 64 bit timer counter value and convert it to microseconds
  // as the timer input is connected to the ref clock.
  unsigned int count = timer_count_get(timer_base_fc(0, 1));
  return ((unsigned long long)count) * 1000000 / ARCHI_REF_CLOCK;
}

void rt_event_push_delayed(rt_event_t *event, int us)
{
  int irq = rt_irq_disable();

  int set_irq = 0;
  rt_event_t *current = first_delayed, *prev=NULL;
  unsigned int ticks, ticks_from_now;
  uint32_t current_time = timer_count_get(timer_base_fc(0, 1));
  
  if (us < 0)
    us = 0;

  // First compute the corresponding number of ticks.
  // The specified time is the minimum we must, so we have to round-up
  // the number of ticks.
#if PULP_CHIP_FAMILY == CHIP_USOC_V1
  ticks = us * ARCHI_REF_CLOCK / 1000000 + 1;
#else
  ticks = us / ( 1000000 / ARCHI_REF_CLOCK) + 1;
#endif

  // In order to simplify time comparison, we sacrify the MSB to avoid overflow
  // as the given amount of time must be short
  uint32_t time = (current_time & 0x7fffffff) + ticks;
  event->implem.time = current_time + ticks;


  // Enqueue the event in the wait list.
  while (current && (current->implem.time & 0x7fffffff) < time)
  {
    prev = current;
    current = current->implem.next;
  }

  if (prev)
  {
    prev->implem.next = event;
  }
  else
  {
    set_irq = 1;
    first_delayed = event;
  }
  event->implem.next = current;

  // And finally update the timer trigger time in case we enqueued the event
  // at the head of the wait list.
  if (set_irq)
  {
    // This is important to reload the current time, in case the previous code
    // took too much time so that the interrupt is not missed
    uint32_t timer = timer_count_get(timer_base_fc(0, 1)) + ticks;
    timer_cmp_set(timer_base_fc(0, 1), timer);

    timer_conf_set(timer_base_fc(0, 1),
      TIMER_CFG_LO_ENABLE(1) |
      TIMER_CFG_LO_IRQEN(1)  |
      TIMER_CFG_LO_CCFG(1)
    );
  }

  rt_irq_restore(irq);
}


void rt_time_wait_us(int time_us)
{
  rt_event_t *event = rt_event_get_blocking(NULL);
  rt_event_push_delayed(event, time_us);
  rt_event_wait(event);
}

void pi_time_wait_us(int time_us)
{
  rt_time_wait_us(time_us);
}

RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_time_init()
{
  int err = 0;

  first_delayed = NULL;
 
  // Configure the FC timer in 64 bits mode as it will be used as a common
  // timer for all virtual timers.
  // We also use the ref clock to make the frequency stable.
  timer_conf_set(timer_base_fc(0, 1),
    TIMER_CFG_LO_ENABLE(1) |
    TIMER_CFG_LO_RESET(1)  |
    TIMER_CFG_LO_CCFG(1)
  );

#if defined(ARCHI_HAS_FC)
  rt_irq_set_handler(ARCHI_FC_EVT_TIMER0_HI, __rt_timer_handler);
  rt_irq_mask_set(1<<ARCHI_FC_EVT_TIMER0_HI);
#else
  rt_irq_set_handler(ARCHI_EVT_TIMER0_HI, __rt_timer_handler);
  rt_irq_mask_set(1<<ARCHI_EVT_TIMER0_HI);
#endif

  err |= __rt_cbsys_add(RT_CBSYS_POWEROFF, __rt_time_poweroff, NULL);
  err |= __rt_cbsys_add(RT_CBSYS_POWERON, __rt_time_poweron, NULL);

  if (err) rt_fatal("Unable to initialize time driver\n");
}

static void __rt_timer_handle(void *arg)
{
  rt_timer_t *timer = (rt_timer_t *)arg;
  rt_event_t *event = timer->user_event;

  void (*callback)(void *) = (void (*)(void *))event->arg[0];
  void *cb_arg = (void *)event->arg[1];

  if (callback) {
    callback(cb_arg);
  }

  __rt_event_unblock(event);

  if (timer->flags == RT_TIMER_PERIODIC)
  {
    timer->current_time += timer->period;
    __rt_event_set_pending(timer->event);
    rt_event_push_delayed(timer->event, timer->current_time - rt_time_get_us());
  }
}

int rt_timer_create(rt_timer_t *timer, rt_timer_flags_e flags, rt_event_t *event)
{
  if (rt_event_alloc(rt_event_internal_sched(), 1))
    return -1;

  timer->event = rt_event_get(rt_event_internal_sched(), __rt_timer_handle, (void *)timer);
  timer->user_event = event;
  timer->flags = flags;

  return 0;
}

void rt_timer_start(rt_timer_t *timer, int us)
{
  timer->period = us;
  timer->current_time = rt_time_get_us() + us;
  __rt_event_set_pending(timer->event);
  rt_event_push_delayed(timer->event, us);
}

void rt_timer_stop(rt_timer_t *timer)
{
  int irq = rt_irq_disable();

  // When the time is stopped, we have to remove the event from any
  // list to avoid spurious events

  // First look inside the wait list
  rt_event_t *event = timer->event;
  rt_event_t *current = first_delayed, *prev=NULL;
  while (current && current != event)
  {
    prev = current;
    current = current->implem.next;
  }

  if (current)
  {
    if (prev)
      prev->implem.next = current->implem.next;
    else
      first_delayed = current->implem.next;
  }
  else
  {
    // Otherwise look inside the scheduler event list
    __rt_sched_event_cancel(event);
  }

  rt_irq_restore(irq);
}

void rt_timer_destroy(rt_timer_t *timer)
{
  __rt_event_release(timer->user_event);
  __rt_event_free(timer->event);
}
