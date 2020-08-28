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

  .global __rt_gpio_handler
__rt_gpio_handler:

#ifdef ARCHI_FC_EVT_GPIO
  sw  x8, -4(sp)
  sw  x9, -8(sp)
  sw  a0, -12(sp)
  sw  a1, -16(sp)
  sw  a2, -20(sp)
  sw  a3, -24(sp)
#endif


#ifdef GPIO_ASM_HANDLER
  // Read interrupt status from gpio HW reg (this tells which gpio has changed)
  // do an OR with current SW status and update it
  li  x10, ARCHI_GPIO_ADDR + GPIO_INTSTATUS_OFFSET
  lw  x10, 0(x10)
  lw  x12, %tiny(__rt_gpio_status)(x0)
  lw  x8, %tiny(__rt_gpio_active_events)(x0)
  or  x12, x12, x10
  sw  x12, %tiny(__rt_gpio_status)(x0)

  // If there is no event attached to the gpio which have changed, just leave
  and x8, x8, x10
  beqz x8, __rt_gpio_handler_end

#ifdef RV_ISA_RV32
  la       x13, __rt_gpio_events - 4

__rt_gpio_ff1_loop:
  andi     x12, x8, 1
  srli     x8, x8, 1
  addi     x13, x13, 4
  beqz     x12, __rt_gpio_ff1_loop

  lw       x11, 0(x13)
  jal      x9, __rt_event_enqueue

  bnez     x8, __rt_gpio_ff1_loop
#else

__rt_gpio_ff1_loop:
  p.ff1    x9, x8
  p.bclrr  x8, x8, x9
  slli     x9, x9, 2   

  p.lw     x11, %tiny(__rt_gpio_events)(x9)
  jal      x9, __rt_event_enqueue

  bnez     x8, __rt_gpio_ff1_loop
#endif

#else
  .extern __pi_gpio_handler
  sw  ra, 20(sp)
  jal ra, __pi_gpio_handler
  lw  ra, 20(sp)
#endif


__rt_gpio_handler_end:

#ifdef ARCHI_FC_EVT_GPIO
  lw  x8, -4(sp)
  lw  x9, -8(sp)
  lw  a0, -12(sp)
  lw  a1, -16(sp)
  lw  a2, -20(sp)
  lw  a3, -24(sp)

  mret

#else

  j udma_event_handler_end

#endif
