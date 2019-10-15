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

// TODO this code should be in time.c but due to a bug in the compiler which
// generates wrong code in other functions if the attribute interrupt is 
// present in the same file, this handler must be in a different file

#if defined(__LLVM__)
void __rt_timer_handler()
#else
void __attribute__((interrupt)) __rt_timer_handler()
#endif
{
  rt_event_t *event = first_delayed;

  uint32_t current_time = timer_count_get(timer_base_fc(0, 1));

  // First dequeue and push to their scheduler all events with the same number of
  // ticks as they were waiting for the same time.
  while (event && (current_time - event->implem.time) < 0x7fffffff)
  {
    rt_event_t *next = event->implem.next;
    __rt_push_event(rt_event_internal_sched(), event);
    event = next;
  }

  // Update the wait list with the next waiting event which has a different number
  // of ticks
  first_delayed = event;

  // Now re-arm the timer in case there are still some events
  if (first_delayed)
  {
    // Be carefull to set the new comparator from the current time plus a number of ticks
    // in order to set a value which is not before the actual count.
    // This may just delay a bit the events which is fine as the specified
    // duration is a minimum.
    timer_cmp_set(timer_base_fc(0, 1),
      timer_count_get(timer_base_fc(0, 1)) + 
      first_delayed->implem.time - current_time
    );

    timer_conf_set(timer_base_fc(0, 1),
      TIMER_CFG_LO_ENABLE(1) |
      TIMER_CFG_LO_IRQEN(1)  |
      TIMER_CFG_LO_CCFG(1)
    );
  }
  else
  {
    // Set back default state where timer is only counting with
    // no interrupt
    timer_conf_set(timer_base_fc(0, 1),
      TIMER_CFG_LO_ENABLE(1) |
      TIMER_CFG_LO_CCFG(1)
    );

    // Also clear timer interrupt as we might have a spurious one after
    // we entered the handler
#ifdef ARCHI_HAS_FC
    rt_irq_clr(1 << ARCHI_FC_EVT_TIMER0_HI);
#else
    rt_irq_clr(1 << ARCHI_EVT_TIMER0_HI);
#endif
  }
}
