/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

        .file "cluster_core.S"
/*******************************************************************************
        This file contains the code executed by the cluster cores while waiting
        for tasks.
*******************************************************************************/

/*******************************************************************************
        INCLUDES
*******************************************************************************/

#include "pmsis/targets/memory_map.h"
#include "pmsis/targets/events.h"
#include "pmsis/targets/periph/cluster_event_unit/cluster_event_unit_offset.h"
#include "pmsis/implem/cluster/cluster_data.h"

/*******************************************************************************
        EXTERNAL VARIABLES & FUNCTIONS
*******************************************************************************/
        .extern __per_cluster_data
        .extern cl_task_finish
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

/******************************************************************************
        Cluster main exec loop
******************************************************************************/

        DECLARE cluster_exec_loop
        /* Coming from reset_handler/_entry with : a0=cl_id, a1=core_id */
        /*
        * Content of used registers
        * s0=cl_demux_core
        * s1=cl_demux_eu_dispatch
        * s2=cl_demux_eu_barrier(0)
        * s10=core_id
        * * master
        * s3=FC_TO_CL delegate IRQ number
        * s4=cluster_data
        * s5=pointer to cluster task fifo
        * s6=first cluster task of the fifo(tasks sent by FC)
        * s7=cl_demux_eu_barrier(ARCHI_CORE_MASTER)
        * s8=cl_slave_stack_setup
        * s9=cl_master_cluster_task_end(fifo mgt + FC notify)
        * * slave
        * s4=cluster end of task -> barrier trig and wait
        * s5=cluster end of task -> no barrier, start over loop at dispatcher
        */
        or s10, zero, a1
        // IRQ to wake cores : irq from dispatcher, HW mutex or barrier.
        li t0, ((1 << CL_IRQ_DISPATCH_EVT) | (1 << CL_IRQ_HW_MUTEX_EVT) | (1 << CL_IRQ_BARRIER_EVT))
        li t1, ARCHI_CLUSTER_MASTER_CORE                        // Core ID of master core on cluster
        li t3, ARCHI_CLUSTER_SYNC_BARR_ID                       // Master-workers sync Barrier ID
        li s2, CL_DEMUX_EU_BARRIER_ADDR
        li t2, CL_DEMUX_EU_HW_BARRIER_SIZE                      // Size of barrier instance
        li s0, CL_DEMUX_EU_CORE_ADDR
        mul t3, t2, t3                                          // Master-workers sync barrier offset
        li s1, CL_DEMUX_EU_DISPATCH_ADDR
        add s7, s2, t3                                          // Barrier used for dispatch sync

        sw t0, CL_DEMUX_EU_CORE_EVENT_MASK_OR(s0)               // CL_DEMUX_EU_CORE->MASK_OR = IRQ

        bne a1, t1, cl_slave_loop                               // Slave cores


        /*** Cluster master ****/
        li t0, (1 << CL_IRQ_DMA1)                               // Enable DMA IRQ for master core
        la s8, cl_slave_stack_setup
        sw t0, CL_DEMUX_EU_CORE_IRQ_MASK_OR(s0)                 // CL_DEMUX_EU_CORE->IRQ_MASK_OR = IRQ
        li s3, (1 << FC_TO_CLUSTER_NOTIFY_EVENT)
        la s4, __per_cluster_data                               // Cluster data
        ori s8, s8, 1

        csrw mstatus, 0x8                                       // Enable IRQ for master core

cl_master_cluster_ready:
        lw s5, 0(s4)                                            // Check if cluster driver is init
        beqz s5, cl_master_sleep                                // If cluster is not on, sleep

cl_master_loop:
        la ra, cl_master_cluster_task_end                       // After finishing cluster task, take next task and notify FC
        lw s6, 0(s5)                                            // Check if there is cluster task in fifo
        beqz s6, cl_master_sleep                                // Sleep if there are no cluster task in fifo

        // Load cluster task info :
        lw t0, PI_CLUSTER_TASK_FUNCTION_OFFSET(s6)              // t0=func
        lw a0, PI_CLUSTER_TASK_FUNC_ARGS_OFFSET(s6)             // a0=args
        lw t1, PI_CLUSTER_TASK_STACK_PTR_OFFSET(s6)             // t1=stack_ptr
        lw t2, PI_CLUSTER_TASK_STACK_MST_SIZE_OFFSET(s6)        // t2=stack_size
        lw t3, PI_CLUSTER_TASK_STACK_SLV_SIZE_OFFSET(s6)        // t3=stack_slave_size
        lw t4, PI_CLUSTER_TASK_TEAM_CORES_OFFSET(s6)            // t4=nb_cores
        add sp, t1, t2                                          // Master core sp, at the top of stack
        lw t5, PI_CLUSTER_TASK_TEAM_MASK_OFFSET(s6)             // t5=team_mask

        or t1, sp, zero                                         // Stack for slaves

cl_master_dispatch_stack:
        xori t6, t5, (1 << ARCHI_CLUSTER_MASTER_CORE)
        beqz t4, cl_master_core_only                            // Only core master execute task
        sw t6, CL_DEMUX_EU_DISPATCH_TEAM_CONFIG(s1)             // Config current team
        sw s8, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)             // Dispatch cluster setup to slaves
        sw t1, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)             // Arg to cl_slave_stack_setup: sp
        sw t3, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)             // Arg to cl_slave_stack_setup: stack_slave_size

cl_master_core_only:
        jr t0                                                   // Execute cluster function

cl_master_cluster_task_end:
        or a0, zero, s5
        jal cl_task_finish
        j cl_master_loop

        // Wait and sleep until an IRQ is sent by FC to cluster(for a new task)
cl_master_sleep:
        sw s3, CL_DEMUX_EU_CORE_EVENT_MASK_OR(s0)
        p.elw t0, CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR(s0)
        sw s3, CL_DEMUX_EU_CORE_EVENT_MASK_AND(s0)
        j cl_master_loop



        /*** Cluster slaves ***/
        // Wait and sleep until a task is dispatched
cl_slave_loop:
        la s4, cl_slave_barrier_trig_wait
        la s5, cl_slave_wait_for_dispatch
        or ra, zero, s4
        j cl_slave_wait_for_dispatch

cl_slave_barrier_trig_wait:
        p.elw t0, CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR(s7) // Trigger and wait on barrier

cl_slave_wait_for_dispatch:
        p.elw t0, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)          // Pop the function to execute
        p.elw a0, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)          // Pop function's args

        // Should the core trigger and wait at barrier after exec
        andi t1, t0, 0x1
        p.bclr t0, t0, 0, 0                                     // Clear LSB barrier bit before function call
        bne t1, zero, cl_slave_entry_no_barrier

cl_slave_entry_with_barrier:
        or ra, zero, s4                                         // Return to wait on barrier
        jr t0                                                   // Jump to function to execute, on return trig and wait at barrier, and start over slave loop

cl_slave_entry_no_barrier:
        or ra, zero, s5                                         // Return to wait on dispatcher
        jr t0                                                   // Jump to function to execute, and start over slave loop

        .endfunc




/******************************************************************************
        Cluster slave stack setup
******************************************************************************/
        DECLARE cl_slave_stack_setup
        /* Coming cluster_exec_loop with : a0=sp */
        /*
        * Cluster cores set stack pointer.
        * From cluster_exec_loop, s10=core_id
        * Master core is core_id=0, stack is at the top of stack ptr
        * Stack pointer adjusted for slave cores.
        */
        p.elw t0, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS(s1)          // Stack slave size
        mul t0, t0, s10                                         // Slave stack offset
        add sp, a0, t0                                          // Slave sp
        jr ra
        .endfunc
