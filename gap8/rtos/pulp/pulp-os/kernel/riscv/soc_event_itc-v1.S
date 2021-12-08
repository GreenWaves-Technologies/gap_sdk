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

#include "rt/rt_data.h"
#include "archi/pulp.h"
#include "archi/itc/itc_v1.h"

  .global __rt_fc_socevents_handler
__rt_fc_socevents_handler:
// The stack is first adjusted to have stack-based load/store compressed
  add sp, sp, -128
  sw  x8, 0(sp)
  sw  x9, 4(sp)
  sw  x10, 8(sp)
  sw  x11, 12(sp)
  sw  x12, 16(sp)

  // Pop one element from the FIFO
  li   x8, ARCHI_FC_ITC_ADDR
  lw   x10, ITC_FIFO_OFFSET(x8)

  // UDMA CHANNEL EVENTS
  li x9, ARCHI_SOC_EVENT_UDMA_NB_EVT
  bge x10, x9, __rt_soc_evt_no_udma_channel

  // We have the channel ID in x10, get pointer to the corresponding channel

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

  addi x11, x10, -ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT
  slli x11, x11, 2
  lw   x11, %tiny(__rt_udma_extra_callback)(x11)

  jr   x11

  .global __rt_soc_evt_no_udma
__rt_soc_evt_no_udma:
  // If the event is not handled, store it in the soc event status mask
  la      x9, __rt_socevents_status
  li      x11, 32
  blt     x10, x11, socevents_set
  addi    x9, x9, 4
  addi    x10, x10, -32

socevents_set:
  lw      x11, 0(x9)
#ifdef RV_ISA_RV32
  li      x8, 1
  sll     x10, x8, x10
  or      x12, x11, x10
#else
  p.bsetr x12, x11, x10
#endif
  sw      x12, 0(x9)
  

  .global udma_event_handler_end
udma_event_handler_end:
  lw  x8, 0(sp)
  lw  x9, 4(sp)
  lw  x10, 8(sp)
  lw  x11, 12(sp)
  lw  x12, 16(sp)
  add sp, sp, 128
  mret




