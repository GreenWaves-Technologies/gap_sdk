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
  .global _entry
_entry:
#ifndef PLP_NO_PERF_COUNTERS
  // Performance counters are active after reset, deactivate them to let the
  // user control when they should start counting
  csrw    0x7A1, x0
#endif

  # Cluster PEs will also starts here to avoid aligning another entry point
  # Just re-route them to the right entry
#if defined(ARCHI_HAS_CLUSTER)
#if RISCV_VERSION >= 4
  csrr    a0, 0xF14
#else
  csrr    a0, 0xF10
#endif
  andi    a1, a0, 0x1f
  srli    a0, a0, 5
#if defined(ARCHI_HAS_FC)
  li      a2, ARCHI_FC_CID
  bne     a0, a2, __rt_pe_start
#else
  // In case we don't have FC, just let cluster 0 pe 0 be the FC and other jump
  // to the slave entry point
  bne     a1, x0, __rt_pe_start
  bne     a0, x0, __rt_pe_start
#endif
#endif

#if defined(__ariane__)

  csrr    a0, 0xF14
  srli    a0, a0, 5
  li      a2, 0x1f
  bne     a0, a2, is_ariane

  la      t1, _start
  li      t2, 0x1a104000
  li      t3, 0x8
  li      t4, 0xC

  sw      t1, 0x80(t2)
  sw      x0, 0x84(t2)
  sw      t3, 0x70(t2)
  sw      x0, 0x70(t2)
  sw      t3, 0x70(t2)
  sw      t4, 0x70(t2)

  lui     t5,0x1a10a
  addi    t5,t5,-2040
  addi    t6, x0, -1
  sw t6, 0(t5)

loop:
  wfi
  j loop

is_ariane:

#endif


  # clear the bss segment
  la      t0, _bss_start
  la      t1, _bss_end
1:
#ifdef __riscv64
  sd      zero,0(t0)
  addi    t0, t0, 8
#else
  sw      zero,0(t0)
  addi    t0, t0, 4
#endif
  bltu    t0, t1, 1b



  /* Stack initialization */
  la   a0, __rt_fc_stack_size
  lw   a0, 0(a0)
  la   x2, __rt_fc_stack
  add  x2, x2, a0


#if defined(__RT_USE_LIBC)
  la      a0, __libc_fini_array   # Register global termination functions
  call    atexit                  #  to be called upon exit
  call    __libc_init_array       # Run global initialization functions
#endif


  /* Do all other initializations from C code */
  jal  x1, __rt_init





.section .text

  // On all other chips we simply pass 0.
  addi  a0, x0, 0
  addi  a1, x0, 0

  // Jump to main program entry point (argc = a0, argv = a1).
  la    t2, main
  jalr  x1, t2
  mv    s0, a0

  /* Do all other initializations from C code */
  jal  x1, __rt_deinit

#ifdef __RT_MODE_BARE

__rt_exit_loop:
  j __rt_exit_loop

#else

  /* If program returns from main, call exit routine */
  mv   a0, s0
  jal  x1, exit

#endif



  .global _init
  .global _fini
_init:
_fini:
  # These don't have to do anything since we use init_array/fini_array.
  ret


#ifndef __ariane__
.section .vectors, "ax"
.option norvc;


#ifdef ARCHI_CORE_HAS_1_10
  j __rt_illegal_instr
#else
  j __rt_no_irq_handler
#endif

  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler
  j __rt_no_irq_handler


  .org 0x80
  .global _start
_start:
  jal x0, _entry


#ifndef __RT_MODE_BARE

  jal x0, __rt_illegal_instr

  // This variable is pointing to the structure containing all information exchanged with
  // the platform loader. It is using a fixed address so that the loader can also find it
  // and then knows the address of the debug structure
  .org 0x90
  .global __rt_debug_struct_ptr
__rt_debug_struct_ptr:
  .word __hal_debug_struct

#endif

__rt_no_irq_handler:
  j __rt_no_irq_handler

#endif

  .global __rt_semihosting_call
__rt_semihosting_call:
  ebreak
  jr          ra
