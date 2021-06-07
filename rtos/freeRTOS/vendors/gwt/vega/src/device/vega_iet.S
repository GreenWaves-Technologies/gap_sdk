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


        .file "vega_iet.S"

#include "pmsis/implem/drivers/pmsis_it.h"
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
        .extern ASM_FUNC_SAVE_MINIMAL_CONTEXT
        .extern ASM_FUNC_SAVE_SAVE_CONTEXT
        .extern ASM_FUNC_SAVE_CSR_CONTEXT
        .extern ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        .extern ASM_FUNC_RESTORE_SAVE_CONTEXT
        .extern ASM_FUNC_RESTORE_CSR_CONTEXT


/*******************************************************************************
	MACRO DEFINITION
*******************************************************************************/

	.macro DECLARE Routine
	.global \Routine
	.func \Routine
	.type \Routine, %function
	\Routine:
	.endm



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
        addi sp, sp, -(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        sw ra, 0*PI_WORD_SIZE(sp)
        jal ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, cl_notify_fc_event_handler

        /* Restore current context. */
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
        .endfunc


        /* PendSV_Handler. */
	DECLARE pendSV_handler
        addi sp, sp, -(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        sw ra, 0*PI_WORD_SIZE(sp)
        jal ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch_pendsv
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_SAVE_SAVE_CONTEXT
        jal ra, ASM_FUNC_SAVE_CSR_CONTEXT
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_CSR_CONTEXT
        jal ra, ASM_FUNC_RESTORE_SAVE_CONTEXT
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
_no_switch_pendsv:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
	mret
	.endfunc


        /* SysTick_Handler. */
        DECLARE systick_handler
        addi sp, sp, -(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        sw ra, 0*PI_WORD_SIZE(sp)
        jal ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, xTaskIncrementTick
        jal ra, pi_task_delayed_increment_push
#ifdef PREEMPTION
        //jal ra, uTaskCheckQuantum
        // if 0 is returned, semaphore yielded already
        beq a0, zero, _no_switch
        jal ra, vTaskSwitchContext
        lw a0, pxCurrentTCB
        beq tp, a0, _no_switch
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_SAVE_SAVE_CONTEXT
        jal ra, ASM_FUNC_SAVE_CSR_CONTEXT
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_CSR_CONTEXT
        jal ra, ASM_FUNC_RESTORE_SAVE_CONTEXT
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
#endif
_no_switch:
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
        .endfunc


        /* FC SOC event Handler. */
        DECLARE fc_event_handler
        /* Save current context. */
        addi sp, sp, -(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        sw ra, 0*PI_WORD_SIZE(sp)
        jal ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT
        lw tp, pxCurrentTCB
        sw sp, 0*0(tp)

        /* ISR Stack. */
        la sp, __stack_irq_start__
        jal ra, fc_soc_event_handler

        /* Restore current context. */
        lw tp, pxCurrentTCB
        lw sp, 0*0(tp)
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
        .endfunc


        /* DMA IRQ Handler. */
        DECLARE dma_irq_handler
        .equ CLUSTER_ID_FC, 32
        addi sp, sp, -(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        sw ra, 0*PI_WORD_SIZE(sp)
        jal ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT

        /* Check cluster ID. */
        csrr t0, mhartid
        srli t1, t0, 5
        li t0, CLUSTER_ID_FC
        beq t1, t0, __dma_irq_is_fc

        /* DMA IRQ for cluster core. */
        jal ra, pi_cl_dma_2d_handler

__dma_irq_is_fc:
        jal ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT
        lw ra, 0*PI_WORD_SIZE(sp)
        addi sp, sp, +(PI_VEGA_FULL_CONTEXT_SIZE * PI_WORD_SIZE)
        mret
        .endfunc



/*******************************************************************************
	ECALL HANDLER
*******************************************************************************/

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



/*******************************************************************************
	EXCEPTION HANDLER
*******************************************************************************/

        .extern _ill_insn_print
	DECLARE ill_ins_handler
        /* MEPC */
        csrr a0, mepc
	j _ill_insn_print
	.endfunc
