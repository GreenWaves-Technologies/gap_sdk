/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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


        .file "gap8_iet.S"

/*******************************************************************************
	This file contains the Ecall handler and the Interrupt handler.
*******************************************************************************/

/*******************************************************************************
	EXTERNAL VARIABLES & FUNCTIONS
*******************************************************************************/
	.extern _ecall_vector
        .extern pxCurrentTCB
        .extern xISRStack
/******************************************************************************/


/*******************************************************************************
	MACRO DEFINITION
*******************************************************************************/

/******************************************************************************/
	.macro DECLARE Routine
	.global \Routine
	.func \Routine
	.type \Routine, %function
	\Routine:
	.endm
/******************************************************************************/

/******************************************************************************/
	.macro SAVE_CONTEXT

        lw sp, 0*0(tp)

        /* General purpose registers. */
	sw a6, 7*4(sp)
	sw a7, 8*4(sp)
        sw t0, 9*4(sp)
	sw t1, 10*4(sp)
	sw t2, 11*4(sp)
	sw t3, 12*4(sp)
	sw t4, 13*4(sp)
	sw t5, 14*4(sp)
	sw t6, 15*4(sp)
	sw s0, 16*4(sp)
	sw s1, 17*4(sp)
	sw s2, 18*4(sp)
	sw s3, 19*4(sp)
	sw s4, 20*4(sp)
	sw s5, 21*4(sp)
	sw s6, 22*4(sp)
	sw s7, 23*4(sp)
	sw s8, 24*4(sp)
	sw s9, 25*4(sp)
	sw s10, 26*4(sp)
	sw s11, 27*4(sp)

        /* MSTATUS */
        csrr t0, mstatus
        sw t0, 28*4(sp)

        /* MEPC */
        csrr t0, mepc
        sw t0, 29*4(sp)

        /* HW Loop regs. */
        csrr t0, 0x7B0
        sw t0, 30*4(sp)
        csrr t1, 0x7B1
        sw t1, 31*4(sp)
        csrr t2, 0x7B2
        sw t2, 32*4(sp)
        csrr t0, 0x7B4
        sw t0, 33*4(sp)
        csrr t1, 0x7B5
        sw t1, 34*4(sp)
        csrr t2, 0x7B6
        sw t2, 35*4(sp)
	.endm
/******************************************************************************/

/******************************************************************************/
	.macro RESTORE_CONTEXT

        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)

        /* HW Loop regs. */
        lw t2, 35*4(sp)
        csrw 0x7B6, t2
        lw t1, 34*4(sp)
        csrw 0x7B5, t1
        lw t0, 33*4(sp)
        csrw 0x7B4, t0
        lw t2, 32*4(sp)
        csrw 0x7B2, t2
        lw t1, 31*4(sp)
        csrw 0x7B1, t1
        lw t0, 30*4(sp)
        csrw 0x7B0, t0

        /* MEPC */
	lw t0, 29*4(sp)
	csrw mepc, t0

        /* MSTATUS */
	lw t0, 28*4(sp)
	csrw mstatus, t0

        /* General purpose registers. */
	lw s11, 27*4(sp)
	lw s10, 26*4(sp)
	lw s9, 25*4(sp)
	lw s8, 24*4(sp)
	lw s7, 23*4(sp)
	lw s6, 22*4(sp)
	lw s5, 21*4(sp)
	lw s4, 20*4(sp)
	lw s3, 19*4(sp)
	lw s2, 18*4(sp)
	lw s1, 17*4(sp)
	lw s0, 16*4(sp)
	lw t6, 15*4(sp)
	lw t5, 14*4(sp)
	lw t4, 13*4(sp)
	lw t3, 12*4(sp)
	lw t2, 11*4(sp)
	lw t1, 10*4(sp)
	lw t0, 9*4(sp)
	lw a7, 8*4(sp)
	lw a6, 7*4(sp)
	lw a5, 6*4(sp)
	lw a4, 5*4(sp)
	lw a3, 4*4(sp)
	lw a2, 3*4(sp)
	lw a1, 2*4(sp)
	lw a0, 1*4(sp)
	lw ra, 0*4(sp)

        addi sp, sp, (+36*4)
        mret
	.endm
/******************************************************************************/


/******************************************************************************/
	.macro SAVE_CONTEXT_YIELD
        /* General purpose registers. */
        addi sp, sp, (-36*4)
        sw ra, 0*4(sp)
        sw a0, 1*4(sp)
        sw a1, 2*4(sp)
        sw a2, 3*4(sp)
        sw a3, 4*4(sp)
        sw a4, 5*4(sp)
        sw a5, 6*4(sp)
	sw a6, 7*4(sp)
	sw a7, 8*4(sp)
        sw t0, 9*4(sp)
	sw t1, 10*4(sp)
	sw t2, 11*4(sp)
	sw t3, 12*4(sp)
	sw t4, 13*4(sp)
	sw t5, 14*4(sp)
	sw t6, 15*4(sp)
	sw s0, 16*4(sp)
	sw s1, 17*4(sp)
	sw s2, 18*4(sp)
	sw s3, 19*4(sp)
	sw s4, 20*4(sp)
	sw s5, 21*4(sp)
	sw s6, 22*4(sp)
	sw s7, 23*4(sp)
	sw s8, 24*4(sp)
	sw s9, 25*4(sp)
	sw s10, 26*4(sp)
	sw s11, 27*4(sp)

        /* MSTATUS */
        csrr t0, mstatus
        sw t0, 28*4(sp)

        /* MEPC */
        csrr t0, mepc
        sw t0, 29*4(sp)

        /* HW Loop regs. */
        csrr t0, 0x7B0
        sw t0, 30*4(sp)
        csrr t1, 0x7B1
        sw t1, 31*4(sp)
        csrr t2, 0x7B2
        sw t2, 32*4(sp)
        csrr t0, 0x7B4
        sw t0, 33*4(sp)
        csrr t1, 0x7B5
        sw t1, 34*4(sp)
        csrr t2, 0x7B6
        sw t2, 35*4(sp)
	.endm
/******************************************************************************/

/******************************************************************************/
	.macro RESTORE_CONTEXT_YIELD

        /* HW Loop regs. */
        lw t2, 35*4(sp)
        csrw 0x7B6, t2
        lw t1, 34*4(sp)
        csrw 0x7B5, t1
        lw t0, 33*4(sp)
        csrw 0x7B4, t0
        lw t2, 32*4(sp)
        csrw 0x7B2, t2
        lw t1, 31*4(sp)
        csrw 0x7B1, t1
        lw t0, 30*4(sp)
        csrw 0x7B0, t0

        /* MEPC */
	lw t0, 29*4(sp)
	csrw mepc, t0

        /* MSTATUS */
	lw t0, 28*4(sp)
	csrw mstatus, t0

        /* General purpose registers. */
	lw s11, 27*4(sp)
	lw s10, 26*4(sp)
	lw s9, 25*4(sp)
	lw s8, 24*4(sp)
	lw s7, 23*4(sp)
	lw s6, 22*4(sp)
	lw s5, 21*4(sp)
	lw s4, 20*4(sp)
	lw s3, 19*4(sp)
	lw s2, 18*4(sp)
	lw s1, 17*4(sp)
	lw s0, 16*4(sp)
	lw t6, 15*4(sp)
	lw t5, 14*4(sp)
	lw t4, 13*4(sp)
	lw t3, 12*4(sp)
	lw t2, 11*4(sp)
	lw t1, 10*4(sp)
	lw t0, 9*4(sp)
	lw a7, 8*4(sp)
	lw a6, 7*4(sp)
	lw a5, 6*4(sp)
	lw a4, 5*4(sp)
	lw a3, 4*4(sp)
	lw a2, 3*4(sp)
	lw a1, 2*4(sp)
	lw a0, 1*4(sp)
	lw ra, 0*4(sp)
        addi sp, sp, (+36*4)
	.endm
/******************************************************************************/

/*******************************************************************************
	ECALL HANDLER
*******************************************************************************/
	.section .text, "ax"
	DECLARE ecall_handler
	/* Save on the stack used registers. */
        addi sp, sp, (-8*4)
        sw ra, 0*4(sp)
        sw t0, 1*4(sp)
        sw t1, 2*4(sp)
	csrr t0, mepc
        addi t0, t0, 4
	sw t0, 3*4(sp)
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)

	/* Search ecall :  a7 contains ecall number. */
	andi t0, a7, 0xf
	la t1, _ecall_vector
	sll t0, t0, 0x2
	add t1, t1, t0
	lw t0, 0*4(t1)

	/* Execute demanded service. */
	jalr ra, t0

	/* Restore registers used. */
        lw sp, 0*0(tp)
        lw t0, 3*4(sp)
	csrw mepc, t0
        lw t1, 2*4(sp)
	lw t0, 1*4(sp)
        lw ra, 0*4(sp)

	addi sp, sp, (+8*4)
	mret
	.endfunc
/******************************************************************************/


/*******************************************************************************
	IRQ HANDLER
*******************************************************************************/
	/* Default handler, weak func, should be redefined to be overwritten. */
	.weak default_handler
	DECLARE default_handler
	j default_handler
	.endfunc

	/* SysTick_Handler. */
        DECLARE systick_handler
        addi sp, sp, (-36*4)
        sw ra, 0*4(sp)
        sw a0, 1*4(sp)
        sw a1, 2*4(sp)
        sw a2, 3*4(sp)
        sw a3, 4*4(sp)
        sw a4, 5*4(sp)
        sw a5, 6*4(sp)
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)
        jal ra, xTaskIncrementTick
#ifdef PREEMPTION
        beq a0, zero, _no_switch
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch
        SAVE_CONTEXT
        RESTORE_CONTEXT
#endif
_no_switch:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        lw ra, 0*4(sp)
        lw a0, 1*4(sp)
        lw a1, 2*4(sp)
        lw a2, 3*4(sp)
        lw a3, 4*4(sp)
        lw a4, 5*4(sp)
        lw a5, 6*4(sp)
        addi sp, sp, (+36*4)
        mret
        .endfunc

	/* PendSV_Handler. */
	DECLARE pendSV_handler
        addi sp, sp, (-36*4)
        sw ra, 0*4(sp)
        sw a0, 1*4(sp)
        sw a1, 2*4(sp)
        sw a2, 3*4(sp)
        sw a3, 4*4(sp)
        sw a4, 5*4(sp)
        sw a5, 6*4(sp)
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch_pendsv
        SAVE_CONTEXT
        RESTORE_CONTEXT
_no_switch_pendsv:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        lw ra, 0*4(sp)
        lw a0, 1*4(sp)
        lw a1, 2*4(sp)
        lw a2, 3*4(sp)
        lw a3, 4*4(sp)
        lw a4, 5*4(sp)
        lw a5, 6*4(sp)
        addi sp, sp, (+36*4)
	mret
	.endfunc
/******************************************************************************/


/*******************************************************************************
	TO DO : ILL_INS HANDLER
*******************************************************************************/
	DECLARE ill_ins_handler
	j ill_ins_handler
	.endfunc
/******************************************************************************/


/******************************************************************************/
        DECLARE vPortYield
        /* Disable IRQ, especially because of SysTick. */
        csrci mstatus, 8
        /* Save current context. */
        SAVE_CONTEXT_YIELD
        /*
         * Save sp of current context
         * Look for next context
         * Restore sp of next context.
        */
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)
        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)
        jal ra, vTaskSwitchContext
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        /* Restore next context. */
        RESTORE_CONTEXT_YIELD
        /* Enable IRQ, disabled before entering. */
        csrsi mstatus, 8
        /* Resume execution. */
        jr ra
        .endfunc

/******************************************************************************/


/******************************************************************************/

        /* Fc SOC event Handler. */
        .extern fc_soc_event_handler
        DECLARE fc_event_handler
        /* Save current context. */
        SAVE_CONTEXT_YIELD
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)
        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)
        jal ra, fc_soc_event_handler
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        /* Restore current context. */
        RESTORE_CONTEXT_YIELD
        mret
        .endfunc

/******************************************************************************/


/******************************************************************************/

        /* Cluster Notify FC Handler. */
        .extern cl_notify_fc_event_handler
        DECLARE cluster_notify_fc_handler
        /* Save current context. */
        SAVE_CONTEXT_YIELD
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)
        /* ISR Stack. */
        la sp, xISRStack
        lw sp, 0*0(sp)
        jal ra, cl_notify_fc_event_handler
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        /* Restore current context. */
        RESTORE_CONTEXT_YIELD
        mret
        .endfunc

/******************************************************************************/

/******************************************************************************/

        /* DMA 2D Handler. */
        .extern pi_cl_dma_2d_handler
        DECLARE cluster_dma_2d_handler
        /* Save current context. */
        addi sp, sp, (-18*4)
        sw ra, 0*4(sp)
        sw a0, 1*4(sp)
        sw a1, 2*4(sp)
        sw a2, 3*4(sp)
        sw a3, 4*4(sp)
        sw a4, 5*4(sp)
        sw a5, 6*4(sp)
	sw a6, 7*4(sp)
	sw a7, 8*4(sp)
        sw t0, 9*4(sp)
	sw t1, 10*4(sp)
	sw t2, 11*4(sp)
	sw t3, 12*4(sp)
	sw t4, 13*4(sp)
	sw t5, 14*4(sp)
	sw t6, 15*4(sp)

        /* MSTATUS */
        csrr t0, mstatus
        sw t0, 16*4(sp)

        /* MEPC */
        csrr t0, mepc
        sw t0, 17*4(sp)

        jal ra, pi_cl_dma_2d_handler
        /* Restore current context. */
        /* MEPC */
	lw t0, 17*4(sp)
	csrw mepc, t0

        /* MSTATUS */
	lw t0, 16*4(sp)
	csrw mstatus, t0

        lw t6, 15*4(sp)
	lw t5, 14*4(sp)
	lw t4, 13*4(sp)
	lw t3, 12*4(sp)
	lw t2, 11*4(sp)
	lw t1, 10*4(sp)
	lw t0, 9*4(sp)
	lw a7, 8*4(sp)
	lw a6, 7*4(sp)
	lw a5, 6*4(sp)
	lw a4, 5*4(sp)
	lw a3, 4*4(sp)
	lw a2, 3*4(sp)
	lw a1, 2*4(sp)
	lw a0, 1*4(sp)
	lw ra, 0*4(sp)
        addi sp, sp, (+18*4)
        mret
        .endfunc

/******************************************************************************/

