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

#if PULP_CHIP == CHIP_OPRECOMPKW
  // Store the work element descriptor which is passed as a0/a1 in the opc_wed
  // section.
  csrr  t0, 0xF14
  bne   t0, x0, _start_wed_stored
  la    t0, opc_wed
  sw    a0, 0(t0)
  sw    a1, 4(t0)
_start_wed_stored:
#endif

  # Cluster PEs will also starts here to avoid aligning another entry point
  # Just re-route them to the right entry
#if defined(ARCHI_HAS_CLUSTER)
  csrr    a0, 0xF14
  andi    a1, a0, 0x1f
  srli    a0, a0, 5
#if defined(ARCHI_HAS_FC)
  la      a2, ARCHI_FC_CID
  bne     a0, a2, __rt_pe_start
#else
  // In case we don't have FC, just let cluster 0 pe 0 be the FC and other jump
  // to the slave entry point
  bne     a1, x0, __rt_pe_start
  bne     a0, x0, __rt_pe_start
#endif
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
  la   x2, stack

#if PULP_CHIP == CHIP_OPRECOMPKW
  /* Store the bootloader return address */
  addi sp, sp, -4
  sw ra, 0(sp)
#endif


#if defined(__RT_USE_LIBC)
  la      a0, __libc_fini_array   # Register global termination functions
  call    atexit                  #  to be called upon exit
  call    __libc_init_array       # Run global initialization functions
#endif


  /* Do all other initializations from C code */
  jal  x1, __rt_init





.section .text

  // Prepare the arguments for the jump to the main program entry point.
#if PULP_CHIP == CHIP_OPRECOMPKW
  // OPRECOMP passes the opc_wed as a uint64_t to main.
  la    t0, opc_wed
  lw    a0, 0(t0)
  lw    a1, 4(t0)
#else
  // On all other chips we simply pass 0.
  addi  a0, x0, 0
  addi  a1, x0, 0
#endif

  // Jump to main program entry point (argc = a0, argv = a1).
  la    t2, main
  jalr  x1, t2

  /* Do all other initializations from C code */
  jal  x1, __rt_deinit


#if PULP_CHIP == CHIP_OPRECOMPKW
  /* Restore the bootloader return address and jump there */
  lw ra, 0(sp)
  addi sp, sp, 4
  ret
#endif

  /* If program returns from main, call exit routine */
  jal  x1, exit



  .global _init
  .global _fini
_init:
_fini:
  # These don't have to do anything since we use init_array/fini_array.
  ret


// Declare a symbol where the bootloader can store the work element descriptor.
#if PULP_CHIP == CHIP_OPRECOMPKW
  .section .data
  .global opc_wed
opc_wed:
  .quad 0
#endif


#ifndef __ariane__
.section .vectors, "ax"
.option norvc;

  .org 0x80
  jal x0, _start
  jal x0, __rt_illegal_instr
#endif

  .global __rt_semihosting_call
__rt_semihosting_call:
  ebreak
  jr          ra
