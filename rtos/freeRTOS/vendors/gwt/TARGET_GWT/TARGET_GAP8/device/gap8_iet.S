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

#include "chip_specific_extensions/gap8/freertos_risc_v_chip_specific_extensions.h"
/*******************************************************************************
	This file contains the Ecall handler and the Interrupt handler.
*******************************************************************************/

/*******************************************************************************
	EXTERNAL VARIABLES & FUNCTIONS
*******************************************************************************/
	.extern _ecall_vector
        .extern pxCurrentTCB
        .extern __stack_irq_start__

        .extern cl_notify_fc_event_handler
        .extern fc_soc_event_handler
        .extern pi_cl_dma_2d_handler
        .extern uTaskCheckQuantum
        .extern pi_task_delayed_increment_push
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
        .macro SAVE_MINIMAL_CONTEXT
        sw a0, 1*portWORD_SIZE(sp)
        sw a1, 2*portWORD_SIZE(sp)
        sw a2, 3*portWORD_SIZE(sp)
        sw a3, 4*portWORD_SIZE(sp)
        sw a4, 5*portWORD_SIZE(sp)
        sw a5, 6*portWORD_SIZE(sp)
        sw a6, 7*portWORD_SIZE(sp)
        sw a7, 8*portWORD_SIZE(sp)
        sw t0, 9*portWORD_SIZE(sp)
        sw t1, 10*portWORD_SIZE(sp)
        sw t2, 11*portWORD_SIZE(sp)
        sw t3, 12*portWORD_SIZE(sp)
        sw t4, 13*portWORD_SIZE(sp)
        sw t5, 14*portWORD_SIZE(sp)
        sw t6, 15*portWORD_SIZE(sp)
	.endm
/******************************************************************************/

/******************************************************************************/
        .macro RESTORE_MINIMAL_CONTEXT
        lw t6, 15*portWORD_SIZE(sp)
        lw t5, 14*portWORD_SIZE(sp)
        lw t4, 13*portWORD_SIZE(sp)
        lw t3, 12*portWORD_SIZE(sp)
        lw t2, 11*portWORD_SIZE(sp)
        lw t1, 10*portWORD_SIZE(sp)
        lw t0, 9*portWORD_SIZE(sp)
        lw a7, 8*portWORD_SIZE(sp)
        lw a6, 7*portWORD_SIZE(sp)
        lw a5, 6*portWORD_SIZE(sp)
        lw a4, 5*portWORD_SIZE(sp)
        lw a3, 4*portWORD_SIZE(sp)
        lw a2, 3*portWORD_SIZE(sp)
        lw a1, 2*portWORD_SIZE(sp)
        lw a0, 1*portWORD_SIZE(sp)
        .endm
/******************************************************************************/

/******************************************************************************/
        .macro SAVE_SAVE_CONTEXT
        sw s0, 16*portWORD_SIZE(sp)
        sw s1, 17*portWORD_SIZE(sp)
        sw s2, 18*portWORD_SIZE(sp)
        sw s3, 19*portWORD_SIZE(sp)
        sw s4, 20*portWORD_SIZE(sp)
        sw s5, 21*portWORD_SIZE(sp)
        sw s6, 22*portWORD_SIZE(sp)
        sw s7, 23*portWORD_SIZE(sp)
        sw s8, 24*portWORD_SIZE(sp)
        sw s9, 25*portWORD_SIZE(sp)
        sw s10, 26*portWORD_SIZE(sp)
        sw s11, 27*portWORD_SIZE(sp)
	.endm
/******************************************************************************/

/******************************************************************************/
        .macro RESTORE_SAVE_CONTEXT
        lw s11, 27*portWORD_SIZE(sp)
        lw s10, 26*portWORD_SIZE(sp)
        lw s9, 25*portWORD_SIZE(sp)
        lw s8, 24*portWORD_SIZE(sp)
        lw s7, 23*portWORD_SIZE(sp)
        lw s6, 22*portWORD_SIZE(sp)
        lw s5, 21*portWORD_SIZE(sp)
        lw s4, 20*portWORD_SIZE(sp)
        lw s3, 19*portWORD_SIZE(sp)
        lw s2, 18*portWORD_SIZE(sp)
        lw s1, 17*portWORD_SIZE(sp)
        lw s0, 16*portWORD_SIZE(sp)
        .endm
/******************************************************************************/

/******************************************************************************/
        .macro SAVE_CSR_CONTEXT
        /* MSTATUS */
        csrr t0, mstatus
        sw t0, 28*portWORD_SIZE(sp)
        /* MEPC */
        csrr t0, mepc
        sw t0, 29*portWORD_SIZE(sp)
	.endm
/******************************************************************************/

/******************************************************************************/
        .macro RESTORE_CSR_CONTEXT
        /* MEPC */
        lw t0, 29*portWORD_SIZE(sp)
        csrw mepc, t0
        /* MSTATUS */
        lw t0, 28*portWORD_SIZE(sp)
        csrw mstatus, t0
        .endm
/******************************************************************************/

/******************************************************************************/
        .macro SAVE_ADDITIONAL_CONTEXT
        /* HW Loop regs. */
        csrr t0, LP_START_0
        sw t0, 30*portWORD_SIZE(sp)
        csrr t1, LP_END_0
        sw t1, 31*portWORD_SIZE(sp)
        csrr t2, LP_COUNT_0
        sw t2, 32*portWORD_SIZE(sp)
        csrr t0, LP_START_1
        sw t0, 33*portWORD_SIZE(sp)
        csrr t1, LP_END_1
        sw t1, 34*portWORD_SIZE(sp)
        csrr t2, LP_COUNT_1
        sw t2, 35*portWORD_SIZE(sp)
        .endm
/******************************************************************************/

/******************************************************************************/
        .macro RESTORE_ADDITIONAL_CONTEXT
        /* HW Loop regs. */
        lw t2, 35*portWORD_SIZE(sp)
        csrw LP_COUNT_1, t2
        lw t1, 34*portWORD_SIZE(sp)
        csrw LP_END_1, t1
        lw t0, 33*portWORD_SIZE(sp)
        csrw LP_START_1, t0
        lw t2, 32*portWORD_SIZE(sp)
        csrw LP_COUNT_0, t2
        lw t1, 31*portWORD_SIZE(sp)
        csrw LP_END_0, t1
        lw t0, 30*portWORD_SIZE(sp)
        csrw LP_START_0, t0
        .endm
/******************************************************************************/


/*******************************************************************************
        Functions to save and restore context.
*******************************************************************************/

/******************************************************************************/
        DECLARE gap8_SAVE_MINIMAL_CONTEXT
        /*
        Save temp and args registers. These can be used in IRQ handlers.
        Saving these allow calls to C functions, save registers will be saved
        by the callee if necessary.
        */
        SAVE_MINIMAL_CONTEXT
        jr ra
	.endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_RESTORE_MINIMAL_CONTEXT
        /* Restore temp and args registers. */
        RESTORE_MINIMAL_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_SAVE_SAVE_CONTEXT
        SAVE_SAVE_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_RESTORE_SAVE_CONTEXT
        RESTORE_SAVE_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_SAVE_CSR_CONTEXT
        SAVE_CSR_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_RESTORE_CSR_CONTEXT
        RESTORE_CSR_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_SAVE_ADDITIONAL_CONTEXT
        SAVE_ADDITIONAL_CONTEXT
        jr ra
        .endfunc
/******************************************************************************/

/******************************************************************************/
        DECLARE gap8_RESTORE_ADDITIONAL_CONTEXT
        RESTORE_ADDITIONAL_CONTEXT
        jr ra
        .endfunc
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
        la sp, __stack_irq_start__

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


        /* Cluster Notify FC Handler. */
        DECLARE cluster_notify_fc_handler
        /* Save current context. */
        addi sp, sp, -(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
        sw ra, 0*portWORD_SIZE(sp)
        jal ra, gap8_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, cl_notify_fc_event_handler

        /* Restore current context. */
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
        mret
        .endfunc


        /* PendSV_Handler. */
	DECLARE pendSV_handler
        addi sp, sp, -(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
        sw ra, 0*portWORD_SIZE(sp)
        jal ra, gap8_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch_pendsv
        lw sp, 0*0(tp)
        jal ra, gap8_SAVE_SAVE_CONTEXT
        jal ra, gap8_SAVE_CSR_CONTEXT
        jal ra, gap8_SAVE_ADDITIONAL_CONTEXT
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_ADDITIONAL_CONTEXT
        jal ra, gap8_RESTORE_CSR_CONTEXT
        jal ra, gap8_RESTORE_SAVE_CONTEXT
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
        mret
_no_switch_pendsv:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
	mret
	.endfunc


        /* DMA 2D Handler. */
        DECLARE cluster_dma_2d_handler
        /* Save current context. */
        addi sp, sp, -(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
        sw ra, 0*portWORD_SIZE(sp)
        jal ra, gap8_SAVE_MINIMAL_CONTEXT

        jal ra, pi_cl_dma_2d_handler

        /* Restore current context. */
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
	lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
        mret
        .endfunc


        /* SysTick_Handler. */
        DECLARE systick_handler
        addi sp, sp, -(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
        sw ra, 0*portWORD_SIZE(sp)
        jal ra, gap8_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, xTaskIncrementTick
        jal ra, pi_task_delayed_increment_push
#ifdef PREEMPTION
        //jal ra, uTaskCheckQuantum
        beq a0, zero, _no_switch
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch
        lw sp, 0*0(tp)
        jal ra, gap8_SAVE_SAVE_CONTEXT
        jal ra, gap8_SAVE_CSR_CONTEXT
        jal ra, gap8_SAVE_ADDITIONAL_CONTEXT
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_ADDITIONAL_CONTEXT
        jal ra, gap8_RESTORE_CSR_CONTEXT
        jal ra, gap8_RESTORE_SAVE_CONTEXT
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
        mret
#endif
_no_switch:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_FULL_CONTEXT_SIZE * portWORD_SIZE)
        mret
        .endfunc


        /* FC SOC event Handler. */
        DECLARE fc_event_handler
        /* Save current context. */
        addi sp, sp, -(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
        sw ra, 0*portWORD_SIZE(sp)
        jal ra, gap8_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, fc_soc_event_handler

        /* Restore current context. */
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, gap8_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*portWORD_SIZE(sp)
        addi sp, sp, +(portGAP8_MINIMAL_CONTEXT_SIZE * portWORD_SIZE)
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
