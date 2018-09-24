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

#if defined(ARCHI_HAS_FC)

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

  uint32_t current_time = hal_timer_count_get(hal_timer_fc_addr(0, 1));

  // First dequeue and push to their scheduler all events with the same number of
  // ticks as they were waiting for the same time.
  while (event && (current_time - event->time) < 0x7fffffff)
  {
    rt_event_t *next = event->next;
    __rt_push_event(event->sched, event);
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
    hal_timer_cmp_set(hal_timer_fc_addr(0, 1),
      hal_timer_count_get(hal_timer_fc_addr(0, 1)) + 
      first_delayed->time - current_time
    );

    hal_timer_conf(
      hal_timer_fc_addr(0, 1), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_DISABLED,
      PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_DISABLED,
      PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
      PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
    );
  }
  else
  {
    // Set back default state where timer is only counting with
    // no interrupt
    hal_timer_conf(
      hal_timer_fc_addr(0, 1), PLP_TIMER_ACTIVE, 0,
      PLP_TIMER_IRQ_DISABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_DISABLED,
      PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
      PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
    );

    // Also clear timer interrupt as we might have a spurious one after
    // we entered the handler
    rt_irq_clr(1 << ARCHI_FC_EVT_TIMER0_HI);
  }
}

#endif