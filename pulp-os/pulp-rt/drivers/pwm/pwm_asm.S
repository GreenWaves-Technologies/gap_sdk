
/*
 * Copyright (C) 2018 GreenWaves Technologies
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
 *Author: Yao ZHANG, GreenWaves Technologies. yao.zhang@greenwaves-technologies.com
 */

#include "rt/rt_data.h"
#include "archi/pulp.h"
#include "archi/eu/eu_v3.h"

  .global pwm_event_handler
pwm_event_handler:
  addi  x8, x8, -ARCHI_SOC_EVENT_ADV_TIMER_FIRST_EVT
  andi  x8, x8, 255
  la   x9, pwmEventsStatus
  addi  x11, x0, 1
  lw   x10, 0(x9)
  p.insertr x10, x11, x8
  sw   x10, 0(x9)

  // continue with sw event raising
  li   x9, ARCHI_EU_DEMUX_ADDR + EU_SW_EVENTS_DEMUX_OFFSET + EU_CORE_TRIGG_SW_EVENT + (PLP_RT_NOTIF_EVENT << 2)
  li   x10, -1
  sw   x10, 0(x9)

  j udma_event_handler_end
