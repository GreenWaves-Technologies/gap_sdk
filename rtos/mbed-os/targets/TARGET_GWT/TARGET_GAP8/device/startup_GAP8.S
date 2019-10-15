/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_GAP8.s                                                              */
/*  @purpose: CMSIS RV32IMC Core Device Startup File                                      */
/*            GAP8                                                                        */
/*  @version: 1.0                                                                         */
/*  @date:    2018-3-8                                                                    */
/*  @build:                                                                               */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright (c) 2018 , GreenWaves Technologies, Inc.                                     */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* Redistribution and use in source and binary forms, with or without modification,       */
/* are permitted provided that the following conditions are met:                          */
/*                                                                                        */
/* o Redistributions of source code must retain the above copyright notice, this list     */
/*   of conditions and the following disclaimer.                                          */
/*                                                                                        */
/* o Redistributions in binary form must reproduce the above copyright notice, this       */
/*   list of conditions and the following disclaimer in the documentation and/or          */
/*   other materials provided with the distribution.                                      */
/*                                                                                        */
/* o Neither the name of GreenWaves Technologies, Inc. nor the names of its               */
/*   contributors may be used to endorse or promote products derived from this            */
/*   software without specific prior written permission.                                  */
/*                                                                                        */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND        */
/* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED          */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                 */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR       */
/* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;           */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON         */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS          */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/******************************************************************************************/

//#define EXCEPTION_STACK_SIZE 72
    .file    "startup_GAP8.S"

    .equ _start, _mainCRTStartup
    .section .text.reset
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
  /* Just FC (if it is there) executes this code, other cores jump to the PE loop */
  csrr a0, 0xF14
  srli a0, a0, 5

#ifdef FEATURE_CLUSTER
  .equ FC_CLUSTER_ID, 32
  li   a2, FC_CLUSTER_ID
  bne  a2, a0, CLUSTER_SleepWait
#endif

  /* FC core needs to set MTVEC */
  /* U mode and M mode have differnt vector table */
  li    a0, 0x1c000100
  csrw  0x305, a0
  li    a0, 0x001c0000
  csrw  0x005, a0

  /* Stack initialization */
  // FC should take its own stack
  li   sp, 0x1B003FFF

  /* Initial mstatus and current stack = master stack */
  csrw 0x300, 0
  csrw 0x000, 0
  la   a0, currentStack
  sw   x0, (a0)

#define __STARTUP_CLEAR_BSS 1

CRT0_Resume:
#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
  la    a0, __bss_start__
  la    a1, __bss_end__
.LC1:
  sw    x0, (a0)
  addi  a0, a0, 4
  bltu  a0, a1, .LC1

  la    a0, __fc_bss_start__
  la    a1, __fc_bss_end__
.LC2:
  sw    x0, (a0)
  addi  a0, a0, 4
  bltu  a0, a1, .LC2
#endif

System_Init:
#ifndef __NO_SYSTEM_INIT
  /* Do all other initializations from C code */
  jal  ra, SystemInit
#endif

  /* jump to main program entry point (argc = argv = 0) */
  mv    a0, x0
  mv    a1, x0
#ifndef __START
#define __START _start
#endif
   j    _start

  /* This section has to be down here, since we have to disable rvc for it  */
  .section .vectors_U, "ax"
  .option norvc;
  .org 0x80
  jal x0, Reset_Handler

  .org 0x100
  .section .vectors_M, "ax"
  .option norvc;
  /* reset vector */
  .org 0x80
  jal x0, Reset_Handler

  .org 0x84
  jal x0, Ill_Ins_Handler

  .org 0x88
  jal x0, Ecall_Handler

  /* This variable is pointed to the structure containing all information exchanged with
     the platform loader. It is using a fixed address so that the loader can also find it
     and then knows the address of the debug structure */
  .org 0x90
  .global __rt_debug_struct_ptr
__rt_debug_struct_ptr:
  .word Debug_Struct

  .global Plp_Semihosting_Call
Plp_Semihosting_Call:
  scallimm    16
  jr          ra

Default_Handler:
  mret

Ill_Ins_Handler:
  j Ill_Ins_Handler
