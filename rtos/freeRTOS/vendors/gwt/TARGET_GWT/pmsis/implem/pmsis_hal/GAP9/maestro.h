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

#include "pmu_v3.h"

#define COLD_BOOT       0     /* SoC cold boot, from Flash usually */
#define DEEP_SLEEP_BOOT 1     /* Reboot from deep sleep state, state has been lost, somehow equivalent to COLD_BOOT */
#define RETENTIVE_BOOT  2     /* Reboot from Retentive state, state has been retained, should bypass flash reload */


#define RT_PMU_MRAM_ID    0
#define RT_PMU_CSI_ID     1
#define RT_PMU_CLUSTER_ID 2
#define RT_PMU_CHIP_ID    3

#define RT_PMU_STATE_OFF 0
#define RT_PMU_STATE_ON  1

#define RT_PMU_RETENTION_OFF 0
#define RT_PMU_RETENTION_ON  1

/* Maestro internal events */
#define MAESTRO_EVENT_ICU_OK            (1<<0)
#define MAESTRO_EVENT_ICU_DELAYED       (1<<1)
#define MAESTRO_EVENT_MODE_CHANGED      (1<<2)
#define MAESTRO_EVENT_PICL_OK           (1<<3)
#define MAESTRO_EVENT_SCU_OK            (1<<4)

static inline __attribute__((always_inline)) void __rt_pmu_apply_state(int domain, int state, int ret)
{
  int sequence;

  // Update local domain status
  //__rt_pmu_domains_on = (__rt_pmu_domains_on & ~(1 << domain)) | (state << domain);

  // Remember that a sequence is pending as no other one should be enqueued until
  // this one is finished.
  //__rt_pmu_pending_sequence = 1;

  // Compute the right sequence
  if (domain == RT_PMU_CHIP_ID)
  {
    // For soc, 4 is deep sleep, 5 retentive deep sleep, and 6 and 7 the same
    // with smart wakeup on.
    sequence = 4 + ret;
  }
  else
  {
    // For other domains, first sequence if OFF, second is ON
    sequence = domain*2 + 8 + state;
  }

  // Finally ask Maestro to trigger the sequence
  maestro_trigger_sequence(sequence);

  if(domain == RT_PMU_CLUSTER_ID)
  {
    hal_itc_wait_for_event_no_irq(FC_EVENT_PMU_DLC_BRIDGE_PICL);
  }
  else
  {
    hal_itc_wait_for_event_no_irq(FC_EVENT_PMU_DLC_BRIDGE_SCU);
  }

  PMU_WRITE(MAESTRO_DLC_IFR_OFFSET, MAESTRO_EVENT_SCU_OK);
}

// Disable all Maestro interrupts but PICL_OK and SCU_OK
static inline void __rt_pmu_init(void)
{
    PMU_WRITE(MAESTRO_DLC_IMR_OFFSET, 0x7);
}

#endif
