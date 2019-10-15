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

#include "archi/pulp.h"

  .section .text
  .global _start
_start:

#define REDZONE 128
#define RT_EXCEPTION_FRAME (REDZONE + 0x44)

#include "rt/rt_data.h"
#include "archi/pulp.h"

#define LOAD_SYMBOL_2_GPR(gpr,symbol)  \
  .global symbol ;               \
  l.movhi gpr, hi(symbol) ;      \
  l.ori   gpr, gpr, lo(symbol)

#define LOAD_CONST_2_GPR(gpr,symbol)  \
    l.movhi gpr, hi(symbol) ;      \
    l.ori   gpr, gpr, lo(symbol)

  .section .vectors, "ax"
  .org 0x00
  l.j   reset_handler
  l.nop

  .org 0x38
  l.j   __rt_trap_handler
  l.nop

  // This variable is pointed to the structure containing all information exchanged with
  // the platform loader. It is using a fixed address so that the loader can also find it
  // and then knows the address of the debug structure
  .org 0x90
  .global __rt_debug_struct_ptr
__rt_debug_struct_ptr:
  .word __hal_debug_struct




  .section .text

reset_handler:

  # clear the bss segment
  LOAD_SYMBOL_2_GPR(r5, _bss_start)
  LOAD_SYMBOL_2_GPR(r6, _bss_end)

  l.sfleu r6, r5
  l.bf    zero_loop_end
  l.nop   0
zero_loop:
  l.sw    -4(r6), r0
  l.addi  r6, r6, -4
  l.sfgtu r6, r5
  l.bf    zero_loop
  l.nop   0
zero_loop_end:


  /* Early Stack initilization */
  LOAD_SYMBOL_2_GPR(r1, stack)

  /* Do all other initializations from C code */
  l.jal    __rt_init
  l.nop

  /* Jump to main program entry point (argc = argv = 0) */
  l.addi   r3, r0, 0
  l.addi   r4, r0, 0
  l.jal    main
  l.nop


  /* If program returns from main, call exit routine */
  l.addi   r10, r11, 0
  l.jal    __rt_deinit
  l.nop

  l.addi   r3, r10, 0
  l.jal    exit
  l.nop


__rt_trap_handler:
  l.addi  r1, r1, -RT_EXCEPTION_FRAME;

  l.sw    0x18(r1),r9;

  LOAD_SYMBOL_2_GPR(r9, __rt_irq_handle)
  l.j      __rt_call_external_c_function;
  l.nop;



// r9 must be saved before calling store_regs, as otherwise its content is already overwritten by l.jal
__rt_call_external_c_function:
  l.sw    0x00(r1),  r3
  l.sw    0x04(r1),  r4
  l.sw    0x08(r1),  r5
  l.sw    0x0c(r1),  r6
  l.sw    0x10(r1),  r7
  l.sw    0x14(r1),  r8
  l.sw    0x1c(r1), r11
  l.sw    0x20(r1), r12
  l.sw    0x24(r1), r13
  l.sw    0x28(r1), r15
  l.sw    0x2c(r1), r17
  l.sw    0x30(r1), r19
  l.sw    0x34(r1), r21
  l.sw    0x38(r1), r23
  l.sw    0x3c(r1), r25
  l.sw    0x40(r1), r27
  l.jalr  r9
  l.nop

__rt_trap_end_except:   // load back registers from stack
  l.lwz   r3,  0x00(r1)
  l.lwz   r4,  0x04(r1)
  l.lwz   r5,  0x08(r1)
  l.lwz   r6,  0x0c(r1)
  l.lwz   r7,  0x10(r1)
  l.lwz   r8,  0x14(r1)
  l.lwz   r9,  0x18(r1)
  l.lwz   r11, 0x1c(r1)
  l.lwz   r12, 0x20(r1)
  l.lwz   r13, 0x24(r1)
  l.lwz   r15, 0x28(r1)
  l.lwz   r17, 0x2c(r1)
  l.lwz   r19, 0x30(r1)
  l.lwz   r21, 0x34(r1)
  l.lwz   r23, 0x38(r1)
  l.lwz   r25, 0x3c(r1)
  l.lwz   r27, 0x40(r1)
  l.rfe                                // recover SR register and prior PC (jumps back to program)
  l.addi  r1, r1, RT_EXCEPTION_FRAME // free stack places



  // TODO try to work-around the interrupt board

#ifdef SR_BUG_WORKAROUND
  // Work-around for HW bug
  // We save by hands the SR to restore it when leaving as the HW
  // saved the wrong one due to pipeline sync issue
  l.mfspr  r20, r0, SPR_SR
  l.sw     8(r1), r20
#endif




#ifdef SR_BUG_WORKAROUND

  // The only way to work-around the HW bug is to emulate in SW the rfe instruction
  // Read status register saved by hands when entering the handler
  l.lw        r30, 8(r1)

  // Get the address where you to jump back when we emulate rfe
  l.mfspr     r31, r0, SPR_EPCR_BASE

  l.addi      r1, r1, REDZONE+12

  // And finally emulate return to interrupted pc
  // Use delay slot to restore interrupt enable and condition flag
  l.jr        r31
  l.mtspr     r0, r30, SPR_SR

#endif