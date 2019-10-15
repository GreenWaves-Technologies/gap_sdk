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

  //
  // Context saving
  //

// The stack is first adjusted to have stack-based load/store compressed
  add sp, sp, -128
  sw  x8, 0(sp)
  sw  x9, 4(sp)
  sw  x10, 8(sp)
  sw  x11, 12(sp)
  sw  x12, 16(sp)



  //
  // SOC event fifo handing
  //
  
  // Pop one element from the FIFO
  li   x8, ARCHI_FC_ITC_ADDR
  lw   x10, ITC_FIFO_OFFSET(x8)




  //
  // UDMA events handling
  //

  li x9, ARCHI_SOC_EVENT_UDMA_NB_EVT
  bge x10, x9, __rt_fc_socevents_no_udma

  // We have the event ID in x10, get periph info
  andi x12, x10, 0xfffffffc

  // Load callback and callback data and call it
  lw   x11, %tiny(__rt_udma_callback)(x12)
  lw   x8, %tiny(__rt_udma_callback_data)(x12)

  jr   x11



__rt_fc_socevents_no_udma:

#ifdef RTC_VERSION
  li      x9, ARCHI_SOC_EVENT_RTC_IRQ
  beq     x9, x10, rtc_event_handler
#endif

  //
  // SOC event status register
  //

__rt_fc_socevents_register:
  // If the event is not handled, store it in the soc event status mask
  srli    x11, x10, 5
  slli    x11, x11, 2
  lw      x12, %tiny(__rt_socevents_status)(x11)
  andi    x10, x10, 0x1f

#ifdef RV_ISA_RV32
  li      x8, 1
  sll     x10, x8, x10
  or      x12, x12, x10
#else
  p.bsetr x12, x12, x10
#endif

  sw      x12, %tiny(__rt_socevents_status)(x11)





  //
  // Context restore
  //

  .global __rt_fc_socevents_handler_exit
__rt_fc_socevents_handler_exit:
  lw  x8, 0(sp)
  lw  x9, 4(sp)
  lw  x10, 8(sp)
  lw  x11, 12(sp)
  lw  x12, 16(sp)
  add sp, sp, 128
  mret


#ifdef RTC_VERSION
rtc_event_handler:
  lw    x11, __rtc_handler
  la    x9, __rt_fc_socevents_handler_exit
  j   __rt_event_enqueue
#endif

