/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


        .file "asm_util.S"

#include "pmsis/implem/drivers/pmsis_it.h"

/*******************************************************************************
        This file contains functions to save/restore regs.
*******************************************************************************/

/*******************************************************************************
        EXTERNAL VARIABLES & FUNCTIONS
*******************************************************************************/


/*******************************************************************************
        MACRO DEFINITION
*******************************************************************************/

        .macro DECLARE Routine
        .global \Routine
        .func \Routine
        .type \Routine, %function
        \Routine:
        .endm

        .macro SAVE_MINIMAL_CONTEXT
        sw      a0,  1*PI_WORD_SIZE(sp)
        sw      a1,  2*PI_WORD_SIZE(sp)
        sw      a2,  3*PI_WORD_SIZE(sp)
        sw      a3,  4*PI_WORD_SIZE(sp)
        sw      a4,  5*PI_WORD_SIZE(sp)
        sw      a5,  6*PI_WORD_SIZE(sp)
        sw      a6,  7*PI_WORD_SIZE(sp)
        sw      a7,  8*PI_WORD_SIZE(sp)
        sw      t0,  9*PI_WORD_SIZE(sp)
        sw      t1, 10*PI_WORD_SIZE(sp)
        sw      t2, 11*PI_WORD_SIZE(sp)
        sw      t3, 12*PI_WORD_SIZE(sp)
        sw      t4, 13*PI_WORD_SIZE(sp)
        sw      t5, 14*PI_WORD_SIZE(sp)
        sw      t6, 15*PI_WORD_SIZE(sp)
        .endm

        .macro SAVE_SAVE_CONTEXT
        sw      s0, 16*PI_WORD_SIZE(sp)
        sw      s1, 17*PI_WORD_SIZE(sp)
        sw      s2, 18*PI_WORD_SIZE(sp)
        sw      s3, 19*PI_WORD_SIZE(sp)
        sw      s4, 20*PI_WORD_SIZE(sp)
        sw      s5, 21*PI_WORD_SIZE(sp)
        sw      s6, 22*PI_WORD_SIZE(sp)
        sw      s7, 23*PI_WORD_SIZE(sp)
        sw      s8, 24*PI_WORD_SIZE(sp)
        sw      s9, 25*PI_WORD_SIZE(sp)
        sw      s10, 26*PI_WORD_SIZE(sp)
        sw      s11, 27*PI_WORD_SIZE(sp)
        .endm

        .macro SAVE_CSR_CONTEXT
        /* MSTATUS */
        csrr    t0, mstatus
        sw      t0, 28*PI_WORD_SIZE(sp)
        /* MEPC */
        csrr    t0, mepc
        sw      t0, 29*PI_WORD_SIZE(sp)
        .endm

        .macro SAVE_ADDITIONAL_CONTEXT
        /* HW   Loop regs. */
        csrr    t0, LP_START_0
        sw      t0, 30*PI_WORD_SIZE(sp)
        csrr    t1, LP_END_0
        sw      t1, 31*PI_WORD_SIZE(sp)
        csrr    t2, LP_COUNT_0
        sw      t2, 32*PI_WORD_SIZE(sp)
        csrr    t0, LP_START_1
        sw      t0, 33*PI_WORD_SIZE(sp)
        csrr    t1, LP_END_1
        sw      t1, 34*PI_WORD_SIZE(sp)
        csrr    t2, LP_COUNT_1
        sw      t2, 35*PI_WORD_SIZE(sp)
        .endm

        .macro RESTORE_ADDITIONAL_CONTEXT
        /* HW   Loop regs. */
        lw      t2, 35*PI_WORD_SIZE(sp)
        csrw    LP_COUNT_1, t2
        lw      t1, 34*PI_WORD_SIZE(sp)
        csrw    LP_END_1, t1
        lw      t0, 33*PI_WORD_SIZE(sp)
        csrw    LP_START_1, t0
        lw      t2, 32*PI_WORD_SIZE(sp)
        csrw    LP_COUNT_0, t2
        lw      t1, 31*PI_WORD_SIZE(sp)
        csrw    LP_END_0, t1
        lw      t0, 30*PI_WORD_SIZE(sp)
        csrw    LP_START_0, t0
        .endm

        .macro RESTORE_CSR_CONTEXT
        /* MEPC */
        lw      t0, 29*PI_WORD_SIZE(sp)
        csrw    mepc, t0
        /* MSTATUS */
        lw      t0, 28*PI_WORD_SIZE(sp)
        csrw    mstatus, t0
        .endm

        .macro RESTORE_SAVE_CONTEXT
        lw      s11, 27*PI_WORD_SIZE(sp)
        lw      s10, 26*PI_WORD_SIZE(sp)
        lw      s9, 25*PI_WORD_SIZE(sp)
        lw      s8, 24*PI_WORD_SIZE(sp)
        lw      s7, 23*PI_WORD_SIZE(sp)
        lw      s6, 22*PI_WORD_SIZE(sp)
        lw      s5, 21*PI_WORD_SIZE(sp)
        lw      s4, 20*PI_WORD_SIZE(sp)
        lw      s3, 19*PI_WORD_SIZE(sp)
        lw      s2, 18*PI_WORD_SIZE(sp)
        lw      s1, 17*PI_WORD_SIZE(sp)
        lw      s0, 16*PI_WORD_SIZE(sp)
        .endm

        .macro RESTORE_MINIMAL_CONTEXT
        lw      t6, 15*PI_WORD_SIZE(sp)
        lw      t5, 14*PI_WORD_SIZE(sp)
        lw      t4, 13*PI_WORD_SIZE(sp)
        lw      t3, 12*PI_WORD_SIZE(sp)
        lw      t2, 11*PI_WORD_SIZE(sp)
        lw      t1, 10*PI_WORD_SIZE(sp)
        lw      t0,  9*PI_WORD_SIZE(sp)
        lw      a7,  8*PI_WORD_SIZE(sp)
        lw      a6,  7*PI_WORD_SIZE(sp)
        lw      a5,  6*PI_WORD_SIZE(sp)
        lw      a4,  5*PI_WORD_SIZE(sp)
        lw      a3,  4*PI_WORD_SIZE(sp)
        lw      a2,  3*PI_WORD_SIZE(sp)
        lw      a1,  2*PI_WORD_SIZE(sp)
        lw      a0,  1*PI_WORD_SIZE(sp)
        .endm


/*******************************************************************************
        Functions to save and restore context.
*******************************************************************************/

        /*
        Save temp and args registers. These can be used in IRQ handlers.
        Saving these allow calls to C functions, save registers will be saved
        by the callee if necessary.
        */
        DECLARE ASM_FUNC_SAVE_MINIMAL_CONTEXT
        SAVE_MINIMAL_CONTEXT
        SAVE_ADDITIONAL_CONTEXT
        jr ra
        .endfunc

        /* Restore temp and args registers. */
        DECLARE ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        RESTORE_ADDITIONAL_CONTEXT
        RESTORE_MINIMAL_CONTEXT
        jr ra
        .endfunc

        DECLARE ASM_FUNC_SAVE_SAVE_CONTEXT
        SAVE_SAVE_CONTEXT
        jr ra
        .endfunc

        DECLARE ASM_FUNC_RESTORE_SAVE_CONTEXT
        RESTORE_SAVE_CONTEXT
        jr ra
        .endfunc

        DECLARE ASM_FUNC_SAVE_CSR_CONTEXT
        SAVE_CSR_CONTEXT
        jr ra
        .endfunc

        DECLARE ASM_FUNC_RESTORE_CSR_CONTEXT
        RESTORE_CSR_CONTEXT
        jr ra
        .endfunc

#if 0
        DECLARE ASM_FUNC_SAVE_ADDITIONAL_CONTEXT
        SAVE_ADDITIONAL_CONTEXT
        jr ra
        .endfunc

        DECLARE ASM_FUNC_RESTORE_ADDITIONAL_CONTEXT
        RESTORE_ADDITIONAL_CONTEXT
        jr ra
        .endfunc
#endif
