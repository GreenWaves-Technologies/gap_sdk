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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "rt/rt_data.h"
#include "archi/pulp.h"


#if PULP_CHIP == CHIP_GAP8_REVC

  // x9: channel, x10: event, x8,x11,x12:temp
  .global __rt_hyper_handle_copy
__rt_hyper_handle_copy:
  sw  x8, -4(sp)
  add sp, sp, -128

  lw    x12, %tiny(__rt_hyper_pending_repeat)(x0)
  beqz      x12, __rt_hyper_handle_copy_end


// Registers content
//   x8  : current copy
//   x9  : pointer to channel
//   x12 : number of bytes to repeat
__rt_hyper_repeat_copy:

  lw        x11, %tiny(__rt_hyper_pending_base)(x0)
  
#ifdef RV_ISA_RV32
  li        x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and       x9, x11, x10
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
#else
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
  p.bclr    x9, x11, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  add       x10, x10, x12
  sw        x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
  sw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)

  lw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  lw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  add       x10, x10, x12
  sub       x9, x9, x12
  blt       x12, x9, __rt_hyper_repeat_copy_not_last
  mv        x12, x9
  sw        x0, %tiny(__rt_hyper_pending_repeat)(x0)
  beq       x12, x0, udma_event_handler_end_hyper

__rt_hyper_repeat_copy_not_last:
  sw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  sw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  sw        x10, UDMA_CHANNEL_SADDR_OFFSET(x11)
  sw        x12, UDMA_CHANNEL_SIZE_OFFSET(x11)

  li        x10, UDMA_CHANNEL_CFG_EN
  sw        x10, UDMA_CHANNEL_CFG_OFFSET(x11)

  j         udma_event_handler_end_hyper





__rt_hyper_handle_copy_end:
  lw        x11, %tiny(__rt_hyper_end_task)(x0)
  sw        x0, %tiny(__rt_hyper_end_task)(x0)
  beqz      x11, __rt_hyper_handle_emu_task
  sw        x0, %tiny(__rt_hyper_current_task)(x0)
  jal       x9, __rt_event_enqueue
  
__rt_hyper_handle_emu_task:
  lw        x10, %tiny(__rt_hyper_pending_emu_task)(x0)
  beqz      x10, __rt_hyper_handle_pending_tasks

  la      x12, __rt_hyper_resume_emu_task
  la        x9, udma_event_handler_end_hyper
  j         __rt_call_external_c_function


__rt_hyper_handle_pending_tasks:
  lw        x10, %tiny(__rt_hyper_pending_tasks)(x0)
  beqz      x10, udma_event_handler_end_hyper

  la      x12, __rt_hyper_resume_copy
  la        x9, udma_event_handler_end_hyper
  j         __rt_call_external_c_function

  .global udma_event_handler_end_hyper
udma_event_handler_end_hyper:
  add sp, sp, 128
  lw  x8, -4(sp)
  lw  x9, -8(sp)
  lw  x10, -12(sp)
  lw  x11, -16(sp)
  lw  x12, -20(sp)
  mret




  .global __rt_hyper_handler
__rt_hyper_handler:
  sw  x9, -8(sp)
  sw  x10, -12(sp)
  sw  x12, -20(sp)
  lw        x12, %tiny(__rt_hyper_udma_handle)(x0)
  sw  x11, -16(sp)
  jr        x12



  .global __rt_hyper_handle_burst
__rt_hyper_handle_burst:

// Registers content
//   x8  : current copy
//   x9  : pointer to channel
//   x12 : number of bytes to repeat

  lw        x12, %tiny(__rt_hyper_pending_repeat)(x0)

  lw        x11, %tiny(__rt_hyper_pending_base)(x0)
  
#ifdef RV_ISA_RV32
  li        x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and       x9, x11, x10
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
#else
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
  p.bclr    x9, x11, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  add       x10, x10, x12
  sw        x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
  sw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)

  lw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  lw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  add       x10, x10, x12
  sub       x9, x9, x12
  bge       x12, x9, __rt_hyper_repeat_copy_last3

__rt_hyper_repeat_copy_not_last3:
  sw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  sw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  sw        x10, UDMA_CHANNEL_SADDR_OFFSET(x11)
  sw        x12, UDMA_CHANNEL_SIZE_OFFSET(x11)

  li        x10, UDMA_CHANNEL_CFG_EN
  sw        x10, UDMA_CHANNEL_CFG_OFFSET(x11)

  lw  x9, -8(sp)
  lw  x10, -12(sp)
  lw  x11, -16(sp)
  lw  x12, -20(sp)
  mret

__rt_hyper_repeat_copy_last3:
  la        x12, __rt_hyper_handle_copy
  sw        x12, %tiny(__rt_hyper_udma_handle)(x0)

  mv        x12, x9
  sw        x0, %tiny(__rt_hyper_pending_repeat)(x0)
  beq       x12, x0, udma_event_handler_end

  j         __rt_hyper_repeat_copy_not_last3

#else

  // x9: channel, x10: event, x8,x11,x12:temp
  .global __rt_hyper_handle_copy
__rt_hyper_handle_copy:

  lw    x12, %tiny(__rt_hyper_pending_repeat)(x0)
  beqz      x12, __rt_hyper_handle_copy_end


// Registers content
//   x8  : current copy
//   x9  : pointer to channel
//   x12 : number of bytes to repeat
__rt_hyper_repeat_copy:

  lw        x11, %tiny(__rt_hyper_pending_base)(x0)
  
#ifdef RV_ISA_RV32
  li        x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and       x9, x11, x10
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
#else
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
  p.bclr    x9, x11, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  add       x10, x10, x12
  sw        x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
  sw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)

  lw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  lw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  add       x10, x10, x12
  sub       x9, x9, x12
  blt       x12, x9, __rt_hyper_repeat_copy_not_last
  mv        x12, x9
  sw        x0, %tiny(__rt_hyper_pending_repeat)(x0)
  beq       x12, x0, udma_event_handler_end

__rt_hyper_repeat_copy_not_last:
  sw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  sw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  sw        x10, UDMA_CHANNEL_SADDR_OFFSET(x11)
  sw        x12, UDMA_CHANNEL_SIZE_OFFSET(x11)

  li        x10, UDMA_CHANNEL_CFG_EN
  sw        x10, UDMA_CHANNEL_CFG_OFFSET(x11)

  j         udma_event_handler_end





__rt_hyper_handle_copy_end:
  lw        x11, %tiny(__rt_hyper_end_task)(x0)
  sw        x0, %tiny(__rt_hyper_end_task)(x0)
  beqz      x11, __rt_hyper_handle_emu_task
  sw        x0, %tiny(__rt_hyper_current_task)(x0)
  jal       x9, __rt_event_enqueue
  
__rt_hyper_handle_emu_task:
  lw        x10, %tiny(__rt_hyper_pending_emu_task)(x0)
  beqz      x10, __rt_hyper_handle_pending_tasks

  la      x12, __rt_hyper_resume_emu_task
  la        x9, udma_event_handler_end
  j         __rt_call_external_c_function


__rt_hyper_handle_pending_tasks:
  lw        x10, %tiny(__rt_hyper_pending_tasks)(x0)
  beqz      x10, udma_event_handler_end

  la      x12, __rt_hyper_resume_copy
  la        x9, udma_event_handler_end
  j         __rt_call_external_c_function


// Registers content
//   x8  : current copy
//   x9  : pointer to channel
//   x12 : number of bytes to repeat
  .global __rt_hyper_handle_burst
__rt_hyper_handle_burst:

  lw        x12, %tiny(__rt_hyper_pending_repeat)(x0)

  lw        x11, %tiny(__rt_hyper_pending_base)(x0)
  
#ifdef RV_ISA_RV32
  li        x10, ~(1<<UDMA_CHANNEL_SIZE_LOG2)
  and       x9, x11, x10
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
#else
  lw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)
  p.bclr    x9, x11, 0, UDMA_CHANNEL_SIZE_LOG2
#endif
  add       x10, x10, x12
  sw        x10, HYPER_EXT_ADDR_CHANNEL_CUSTOM_OFFSET(x9)
  sw        x10, %tiny(__rt_hyper_pending_hyper_addr)(x0)

  lw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  lw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  add       x10, x10, x12
  sub       x9, x9, x12
  blt       x12, x9, __rt_hyper_repeat_copy_not_last2

  la        x12, __rt_hyper_handle_copy
  sw        x12, %tiny(__rt_hyper_udma_handle)(x0)

  mv        x12, x9
  sw        x0, %tiny(__rt_hyper_pending_repeat)(x0)
  beq       x12, x0, udma_event_handler_end

__rt_hyper_repeat_copy_not_last2:
  sw        x10, %tiny(__rt_hyper_pending_addr)(x0)
  sw        x9, %tiny(__rt_hyper_pending_repeat_size)(x0)
  sw        x10, UDMA_CHANNEL_SADDR_OFFSET(x11)
  sw        x12, UDMA_CHANNEL_SIZE_OFFSET(x11)

  li        x10, UDMA_CHANNEL_CFG_EN
  sw        x10, UDMA_CHANNEL_CFG_OFFSET(x11)

  j         udma_event_handler_end

#endif
