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
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "pmu_driver.h"


void rt_pm_wakeup_clear_all()
{
  apb_soc_sleep_control_get();
}

int rt_pm_state_switch(rt_pm_state_e state, rt_pm_state_flags_e flags)
{
  if (state == RT_PM_STATE_DEEP_SLEEP)
  {
    if ((flags & RT_PM_STATE_FAST) == 0)
      return -1;

    PMU_ShutDown(0, 0);

    return 0;
  }
  else if (state == RT_PM_STATE_SLEEP)
  {
    if ((flags & RT_PM_STATE_FAST) == 0)
      return -1;
    
    PMU_ShutDown(1, 0);

    return 0;
  }

  return -1;
}



rt_pm_wakeup_e rt_pm_wakeup_state()
{
  if (PMURetentionState.Fields.BootType == DEEP_SLEEP_BOOT || PMURetentionState.Fields.BootType == FAST_DEEP_SLEEP_BOOT)
    return RT_PM_WAKEUP_DEEPSLEEP;
  else if (PMURetentionState.Fields.BootType == RETENTIVE_BOOT)
    return RT_PM_WAKEUP_SLEEP;
  else
    return RT_PM_WAKEUP_COLD;
}
