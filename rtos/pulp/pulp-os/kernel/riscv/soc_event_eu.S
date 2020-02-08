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

#include "rt/rt_data.h"
#include "archi/pulp.h"
#include "archi/eu/eu_v3.h"

  .global __rt_fc_socevents_handler
  .extern pwm_event_handler
__rt_fc_socevents_handler:
// The stack is first adjusted to have stack-based load/store compressed
  add sp, sp, -128
  sw  x8, 0(sp)
  sw  x9, 4(sp)
  sw  x10, 8(sp)
  sw  x11, 12(sp)
  sw  x12, 16(sp)


  // Pop one element from the FIFO
  li  x8, ARCHI_EU_ADDR + EU_SOC_EVENTS_AREA_OFFSET + EU_SOC_EVENTS_CURRENT_EVENT
  lw  x8, 0(x8)

  // Now that we popped the element, we can clear the soc event FIFO event as the FIFO is generating
  // an event as soon as the FIFO is not empty
  li  x9, 1<<PULP_SOC_EVENTS_EVENT
  li  x10, ARCHI_EU_DEMUX_ADDR + EU_CORE_BUFFER_CLEAR
  sw  x9, 0(x10)

#ifdef ARCHI_UDMA_HAS_HYPER
  // Due to a HW bug in the core on Gap, we have to load this value early
  lw    x12, %tiny(__rt_hyper_udma_handle)(x0)
#endif

  // Extract ID part
  p.extractu x10, x8, EU_SOC_EVENTS_EVENT_MASK_BITS-1, EU_SOC_EVENTS_EVENT_MASK_OFFSET

#ifdef ARCHI_UDMA_HAS_HYPER
  p.bneimm x10, UDMA_EVENT_ID(ARCHI_UDMA_HYPER_ID(0)), __rt_fc_socevents_not_hyper_rx
  jr x12

__rt_fc_socevents_not_hyper_rx:
  p.bneimm x10, UDMA_EVENT_ID(ARCHI_UDMA_HYPER_ID(0))+1, __rt_fc_socevents_not_hyper_tx
  jr x12
#endif

__rt_fc_socevents_not_hyper_tx:

  // UDMA CHANNEL EVENTS
  li x9, ARCHI_SOC_EVENT_UDMA_NB_EVT
  bge x10, x9, __rt_soc_evt_no_udma_channel

  // We have the channel ID in x10, get pointer to the corresponding channel

  // TODO once deprecated drivers are removed this should disappear, and we should keep
  // only the callback
#if defined(ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT) && ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT > 2
  andi   x8, x10, 1
  srli   x10, x10, 1
  or     x10, x10, x8
#endif

  la     x8, periph_channels
  slli   x9, x10, RT_PERIPH_CHANNEL_T_SIZEOF_LOG2
  add    x9, x9, x8

  lw   x11, RT_PERIPH_CHANNEL_T_CALLBACK(x9)
  lw   x8, RT_PERIPH_CHANNEL_T_FIRST(x9)

  jr   x11


__rt_soc_evt_no_udma_channel:

  li x9, ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT + ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT
  bge x10, x9, __rt_soc_evt_no_udma

  addi x8, x10, -ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT
  slli x11, x8, 2
  lw   x12, %tiny(__rt_udma_extra_callback)(x11)
  lw   x11, %tiny(__rt_udma_extra_callback_arg)(x11)

  jr   x12



  .global __rt_soc_evt_no_udma
__rt_soc_evt_no_udma:
#ifdef RT_CONFIG_GPIO_ENABLED
  // GPIO EVENT
  li      x9, ARCHI_SOC_EVENT_GPIO
  beq     x10, x9, __rt_gpio_handler
#endif

  li      x9, ARCHI_SOC_EVENT_RTC_IRQ
  beq     x9, x10, rtc_event_handler


// pwm event handler
//x9 : nb of event that will be used by handler
__rt_soc_evt_pwm:
  li x11, ARCHI_SOC_EVENT_ADV_TIMER_NB_EVT-1 // 3
  addi x9, x11, ARCHI_SOC_EVENT_ADV_TIMER_FIRST_EVT //3+38=41
  sub x9, x9, x10 // 41 - 56
  bgt x9, x11, __rt_soc_evt_store // if > 3 not for pwm
  blt x9, x0,  __rt_soc_evt_store // if > 3 not for pwm
  j pwm_event_handler


__rt_soc_evt_store:
  // If the event is not handled, store it in the soc event status mask
  la      x9, __rt_socevents_status
  li      x11, 32
  blt     x10, x11, socevents_set
  addi    x9, x9, 4
  addi    x10, x10, -32

socevents_set:
  lw      x11, 0(x9)
  p.bsetr x12, x11, x10
  sw      x12, 0(x9)
  j       udma_event_handler_end

  .global udma_event_handler_end
udma_event_handler_end:
  lw  x8, 0(sp)
  lw  x9, 4(sp)
  lw  x10, 8(sp)
  lw  x11, 12(sp)
  lw  x12, 16(sp)
  add sp, sp, 128
  mret


rtc_event_handler:
  lw    x11, __rtc_handler
  la    x9, udma_event_handler_end
  j   __rt_event_enqueue
