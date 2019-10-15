/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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
#include "maestro.h"

// The IRQ handler is isolated in this file, otherwise it disturbs other code
// of the same file due to a compiler bug
void __attribute__((interrupt)) __rt_pmu_scu_handler()
{
  // This handler gets called when a sequence is finished. It should then
  // acknoledge it and continue with the next pending sequence.

  // Clear end of sequence interrupt
  PMU_WRITE(MAESTRO_DLC_IFR_OFFSET, MAESTRO_EVENT_SCU_OK);

  // Notify that nothing is pending
  __rt_pmu_pending_sequence = 0;

  // In case someone is waiting for the end of sequence, enqueue the event
  if (__rt_pmu_scu_event)
  {
    __rt_push_event(rt_event_internal_sched(), __rt_pmu_scu_event);
    __rt_pmu_scu_event = NULL;
  }

  // And handle the next pending sequence
  rt_event_t *event = __rt_pmu_pending_requests;
  if (event)
  {
    __rt_pmu_pending_requests = event->implem.next;

    __rt_pmu_apply_state(event->implem.data[0], event->implem.data[1], event->implem.data[2]);

    // Push the event now or when the sequence is done, depending on what is indicated
    // in the evnt
    if (event->implem.data[3])
      __rt_pmu_scu_event = event;
    else
      __rt_push_event(rt_event_internal_sched(), event);
  }
}
