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

#include "hal/tryx/tryx_v1.h"

#include "pulp.h"             // pulp_read32(), eu_evt_mask()
#include "stdio.h"            // printf()

void pulp_tryx_slowpath()
{
  int coreid = get_core_id();
  unsigned int mask;

  // save event mask
  mask = pulp_read32(ARCHI_EU_DEMUX_ADDR + EU_CORE_MASK);

  // only listen to wake-up event
  eu_evt_mask(EVTMASK_RAB_WAKEUP);

  // go to sleep, wait and clear
  evt_read32(ARCHI_EU_DEMUX_ADDR, EU_CORE_EVENT_WAIT_CLEAR);

  // restore the event mask
  eu_evt_mask(mask);

  return;
}
