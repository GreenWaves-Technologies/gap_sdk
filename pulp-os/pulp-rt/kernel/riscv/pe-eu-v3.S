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

#include "rt/rt_data.h"
#include "archi/pulp.h"
#if defined(ITC_VERSION)
#include "archi/itc/itc_v1.h"
#endif
#include "archi/eu/eu_v3.h"
#include "archi/cluster_ctrl/cluster_ctrl_v2.h"

    .section .cluster.text


#ifdef RV_ISA_RV32
  .global __rt_pe_start
__rt_pe_start:


  .global __rt_set_slave_stack
__rt_set_slave_stack:

#else

  .global __rt_pe_start
__rt_pe_start:

    // Activate a few events
    li      t0, (1<<PULP_DISPATCH_EVENT) | (1<<PULP_HW_BAR_EVENT) | (1<<PULP_MUTEX_EVENT)
    li      t1, ARCHI_EU_DEMUX_ADDR
    sw      t0, EU_CORE_MASK(t1)

#ifdef ARCHI_HAS_CC
    li      t2, ARCHI_CC_CORE_ID
    bne     a1, t2, __rt_slave_start
#else
    bne     a1, x0, __rt_slave_start
#endif

    // Prepare few values that will be kept in saved registers to optimize the loop
    la      s0, __rt_cluster_call
    addi    s1, s0, RT_CLUSTER_CALL_T_SIZEOF * 2
    mv      s2, s0
    li      s3, ARCHI_EU_DEMUX_ADDR
    li      s4, 1<<RT_CLUSTER_CALL_EVT
    la      s5, __rt_master_event
    la      s7, __rt_fc_cluster_data
    lw      s7, 0(s7)
    li      t2, RT_FC_CLUSTER_DATA_T_SIZEOF
    mul     t2, t2, a0
    add     s7, s7, t2
    addi    s7, s7, RT_FC_CLUSTER_DATA_T_EVENTS
#if defined(ARCHI_HAS_FC)
#if defined(ITC_VERSION)
    li      s9, ARCHI_FC_ITC_ADDR + ARCHI_ITC_STATUS_SET
    li      s8, 1<<RT_FC_ENQUEUE_EVENT
#else
    li      s9, ARCHI_FC_GLOBAL_ADDR + ARCHI_FC_PERIPHERALS_OFFSET + ARCHI_FC_EU_OFFSET + EU_SW_EVENTS_AREA_BASE + EU_CORE_TRIGG_SW_EVENT + (RT_FC_ENQUEUE_EVENT << 2)
    li      s8, 1
#endif
#endif
    la      s10, __rt_set_slave_stack
    ori     s10, s10, 1
    j       __rt_master_loop

__rt_master_event:
    beq     s6, x0, __rt_master_loop

__rt_push_event_to_fc_retry:
    // Now we have to push the termination event to FC side
    // First wait until the slot for posting events is free
    lw      t0, 0(s7)
    bne     t0, x0, __rt_push_event_to_fc_wait

    // Push it
    sw      s6, 0(s7)

    // And notify the FC side with a HW event in case it is sleeping
    sw      s8, 0(s9)


__rt_master_loop:
    // Check if a call is ready, e.g. if nb_pe is not zero
    // otherwise go to sleep
    lw      t3, RT_CLUSTER_CALL_T_NB_PE(s0)
    beq     t3, x0, __rt_master_sleep

#ifdef ARCHI_HAS_CLUSTER_CLK_GATE
    // Deactivate automatic cluster clock-gating
    // Due to a HW bug, it can not work when the cluster is doing something
    li      a0, ARCHI_CLUSTER_PERIPHERALS_ADDR
    sw      x0, ARCHI_CLUSTER_CTRL_CLUSTER_CLK_GATE(a0)
#endif

    // Reads entry point information
    lw      a0, RT_CLUSTER_CALL_T_ARG(s0)
    lw      t0, RT_CLUSTER_CALL_T_ENTRY(s0)
    lw      sp, RT_CLUSTER_CALL_T_STACKS(s0)
    lw      t1, RT_CLUSTER_CALL_T_M_STACK_SIZE(s0)
    lw      t2, RT_CLUSTER_CALL_T_S_STACK_SIZE(s0)
    lw      s6, RT_CLUSTER_CALL_T_EVENT(s0)
    lw      t4, RT_CLUSTER_CALL_T_SCHED(s0)
    sw      x0, RT_CLUSTER_CALL_T_NB_PE(s0)
#ifndef ARCHI_NO_L1_TINY
    sw      t4, %tiny(__rt_cluster_sched_current)(x0)
#else
    la      t5, __rt_cluster_sched_current
    sw      t4, 0(t5)
#endif
    mv      ra, s5

    // Whatever the number of cores, we need to setup the barrier as the master code is compiled to use it
    li      t4, 1
#ifdef ARCHI_HAS_CC
    addi    t3, t3, -1
#endif
    sll     t3, t4, t3
    addi    t3, t3, -1
    sw      t3, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_MASK(s3)
    sw      t3, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TARGET_MASK(s3)
    sw      t3, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_TEAM_CONFIG(s3)
#ifdef ARCHI_HAS_CC
    ori     t3, t3, 1<<ARCHI_CC_CORE_ID
    sw      t3, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_MASK + EU_BARRIER_SIZE(s3)
    sw      t3, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TARGET_MASK + EU_BARRIER_SIZE(s3)
#endif

    // Set stack on slaves
    // For that we push first the function for setting stack, then the stack size and the base
    p.beqimm t3, 1, __rt_master_loop_no_slave
    sw      s10, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)
    sw      t2, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)
    sw      sp, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)

__rt_master_loop_no_slave:

    // Update cluster call pointer
    addi    s0, s0, RT_CLUSTER_CALL_T_SIZEOF
    bne     s0, s1, __rt_master_no_reset
    mv      s0, s2

__rt_master_no_reset:
    // Call the entry point, this will directly come back to the master loop
    jr      t0


__rt_master_sleep:
#ifdef ARCHI_HAS_CLUSTER_CLK_GATE
    // Reactivate automatic cluster clock-gating
    // Due to a HW bug, it can not work when the cluster is doing something
    li      a0, ARCHI_CLUSTER_PERIPHERALS_ADDR
    li      a1, 1
    //sw      a1, ARCHI_CLUSTER_CTRL_CLUSTER_CLK_GATE(a0)
#endif

    sw      s4, EU_CORE_MASK_OR(s3)
    p.elw   x0, EU_CORE_EVENT_WAIT_CLEAR(s3)
    sw      s4, EU_CORE_MASK_AND(s3)
    j       __rt_master_loop




__rt_push_event_to_fc_wait:
    sw      s4, EU_CORE_MASK_OR(s3)
    p.elw   x0, EU_CORE_EVENT_WAIT_CLEAR(s3)
    sw      s4, EU_CORE_MASK_AND(s3)
    j       __rt_push_event_to_fc_retry







__rt_slave_start:

    li      s2, ARCHI_EU_DEMUX_ADDR
    csrr    s3, 0xF14
    and     s3, s3, 0x1f
    la      s4, __rt_fork_return
    la      s5, __rt_wait_for_dispatch
    j       __rt_wait_for_dispatch


__rt_fork_return:

#ifdef ARCHI_HAS_CC
    // When the cluster has a controller barrier 0 is used for normal team barrier
    // and barrier 1 is used for end of offload
    p.elw   t0, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_WAIT_CLEAR + EU_BARRIER_SIZE(s2)
#else
    p.elw   t0, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_WAIT_CLEAR(s2)
#endif
    

__rt_wait_for_dispatch:

    // Wait for PC + arg information from dispatcher
    p.elw   t0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)
    p.elw   a0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)

    // Check if this is an entry with a barrier at the end (fork entry)
    andi    t1, t0, 1
    bne     t1, zero, __rt_other_entry

__rt_fork_entry:

    // Jump to the handler and prepare r9 to jump back just before the main loop
    add     ra, s4, x0
    jr      t0

__rt_other_entry:

  // Jump to the handler and prepare r9 to jump back directly to the main loop
    add     ra, s5, x0
    jr      t0



  .global __rt_set_slave_stack
__rt_set_slave_stack:

    // Multiply the stack size by the core ID and add the stack base to get our stack
    p.elw   t0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)
#ifdef ARCHI_HAS_CC
    // If the cluster has a cluster controller, the first slave has core ID 0
    // and thus we need to take the next stack
    addi     s3, s3, 1
#endif
    p.mul   a0, s3, a0
    add     sp, a0, t0
    ret

#endif