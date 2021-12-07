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

#ifndef __KERNEL_VEGA_MAESTRO_H__
#define __KERNEL_VEGA_MAESTRO_H__

#include "rt/rt_api.h"
#include "stdio.h"

#define COLD_BOOT       0     /* SoC cold boot, from Flash usually */
#define DEEP_SLEEP_BOOT 1     /* Reboot from deep sleep state, state has been lost, somehow equivalent to COLD_BOOT */
#define RETENTIVE_BOOT  2     /* Reboot from Retentive state, state has been retained, should bypass flash reload */


#define RT_PMU_SWU_ID     0
#define RT_PMU_MRAM_ID    1
#define RT_PMU_CSI_ID     2
#define RT_PMU_CLUSTER_ID 3
#define RT_PMU_CHIP_ID    4

#define RT_PMU_STATE_OFF 0
#define RT_PMU_STATE_ON  1

#define RT_PMU_FLAGS_NO_RET     0
#define RT_PMU_FLAGS_RET        1
#define RT_PMU_FLAGS_PADS_OFF   0
#define RT_PMU_FLAGS_PADS_ON    2

/* Maestro internal events */
#define MAESTRO_EVENT_ICU_OK            (1<<0)
#define MAESTRO_EVENT_ICU_DELAYED       (1<<1)
#define MAESTRO_EVENT_MODE_CHANGED      (1<<2)
#define MAESTRO_EVENT_PICL_OK           (1<<3)
#define MAESTRO_EVENT_SCU_OK            (1<<4)


extern void __rt_pmu_scu_handler();


extern RT_FC_DATA uint32_t __rt_pmu_pending_sequence;

extern RT_FC_TINY_DATA rt_event_t *__rt_pmu_scu_event;

extern RT_FC_DATA rt_event_t *__rt_pmu_pending_requests;

extern RT_FC_DATA uint32_t __rt_pmu_domains_on;


static inline __attribute__((always_inline)) void __rt_pmu_apply_state(int domain, int state, int flags)
{
  int sequence;

  // Update local domain status
  __rt_pmu_domains_on = (__rt_pmu_domains_on & ~(1 << domain)) | (state << domain);

  // Remember that a sequence is pending as no other one should be enqueued until
  // this one is finished.
  __rt_pmu_pending_sequence = 1;

  // Compute the right sequence
  if (domain == RT_PMU_CHIP_ID)
  {
    int ret  = (flags & RT_PMU_FLAGS_RET) != 0;

    if (flags & RT_PMU_FLAGS_PADS_ON)
    {
      // For soc, 14 is deep sleep with pads on, 15 retentive deep sleep with pads on
      sequence = 14 + ret;
    }
    else
    {
      // For soc, 4 is deep sleep with pads off, 5 retentive deep sleep with pads off
      sequence = 4 + ret;
    }
  }
  else
  {
    // For other domains, first sequence if OFF, second is ON
    sequence = domain*2 + 6 + state;
  }

  // Finally ask Maestro to trigger the sequence
  maestro_trigger_sequence(sequence);
}



#endif