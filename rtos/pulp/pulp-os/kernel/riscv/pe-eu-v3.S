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

#ifdef __RT_USE_PROFILE
#include "archi/gvsoc/gvsoc.h"
#endif

#include "data.h"
#include "archi/pulp.h"
#if defined(ITC_VERSION)
#include "archi/itc/itc_v1.h"
#endif
#include "archi/eu/eu_v3.h"
#include "archi/cluster_ctrl/cluster_ctrl_v2.h"

    .section .cluster.text , "ax"
#ifdef RV_ISA_RV32
  .global __rt_pe_start
__rt_pe_start:


  .global __rt_set_slave_stack
__rt_set_slave_stack:

#else

  .global __rt_pe_start
__rt_pe_start:

#endif

    csrr    a0, 0xF14
    andi    a1, a0, 0x1f
    srli    a0, a0, 5
    
    // Activate a few events
    li      t0, (1<<PULP_DISPATCH_EVENT) | (1<<PULP_HW_BAR_EVENT) | (1<<PULP_MUTEX_EVENT)
    li      t1, ARCHI_EU_DEMUX_ADDR
    sw      t0, EU_CORE_MASK(t1)

#ifndef ARCHI_NO_L1_TINY
    sw      x0, %tiny(__rt_dma_first_pending)(x0)
#endif

#ifdef ARCHI_HAS_CC
    li      t2, ARCHI_CC_CORE_ID
    bne     a1, t2, __rt_slave_start
#else
    bne     a1, x0, __rt_slave_start
#endif

    li      t0, (1<<ARCHI_CL_EVT_DMA1)
    li      t1, ARCHI_EU_DEMUX_ADDR
    sw      t0, EU_CORE_MASK_IRQ_OR(t1)



    // Prepare few values that will be kept in saved registers to optimize the loop
    la      s0, __rt_cluster_pool
    li      s3, ARCHI_EU_DEMUX_ADDR
    li      s4, 1<<RT_CLUSTER_CALL_EVT
    la      s5, __rt_master_event
    la      s7, __rt_fc_cluster_data
    li      t2, RT_FC_CLUSTER_DATA_T_SIZEOF
    mul     t2, t2, a0
    add     s7, s7, t2
    addi    s7, s7, RT_FC_CLUSTER_DATA_T_EVENTS
#if defined(ARCHI_HAS_FC)
#if defined(ITC_VERSION)
    li      s9, ARCHI_FC_ITC_ADDR + ITC_STATUS_SET_OFFSET
    li      s8, 1<<RT_FC_ENQUEUE_EVENT
#else
    li      s9, ARCHI_FC_GLOBAL_ADDR + ARCHI_FC_PERIPHERALS_OFFSET + ARCHI_FC_EU_OFFSET + EU_SW_EVENTS_AREA_BASE + EU_CORE_TRIGG_SW_EVENT + (RT_FC_ENQUEUE_EVENT << 2)
    li      s8, 1
#endif
#else
    // In case there is no FC, the event must be sent to cluster 0 event unit
    li      s9, ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(0) + ARCHI_EU_OFFSET + EU_SW_EVENTS_AREA_BASE + EU_CORE_TRIGG_SW_EVENT + (RT_FC_ENQUEUE_EVENT << 2)
    li      s8, 1    
#endif
#ifndef ARCHI_HAS_NO_DISPATCH
    la      s10, __rt_set_slave_stack
    ori     s10, s10, 1
#endif

    csrwi   0x300, 0x8

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
    // Check if a task is ready in the pool
    lw      t3, 0(s0)
    // Check if a call is ready, e.g. if nb_pe is not zero
    // otherwise go to sleep
    beq     t3, x0, __rt_master_sleep

__rt_master_loop_update_next:
    lw      t4, RT_CLUSTER_TASK_NEXT(t3)
    sw      x0, RT_CLUSTER_TASK_PENDING(t3)
    sw      t4, 0(s0)

    // Check again next pointer in case it was updated by the FC.
    // If so, do it it again as this will ensure that either we see the new
    // value or the FC sees our write
    lw      t5, RT_CLUSTER_TASK_NEXT(t3)
    bne     t4, t5, __rt_master_loop_update_next

#ifdef __RT_USE_PROFILE
    li      a0, GV_SEMIHOSTING_VCD_DUMP_TRACE
    lw      a1, %tiny(__rt_pe_trace)(x0)
    li      a2, 0
    li      a4, 0
    ebreak
#endif

#ifdef __RT_USE_ASSERT
    csrwi   0x7D0, 0
#endif

    // Reads entry point information
    lw      a0, RT_CLUSTER_TASK_ARG(t3)
    lw      t0, RT_CLUSTER_TASK_ENTRY(t3)
    lw      sp, RT_CLUSTER_TASK_STACKS(t3)
    lw      t1, RT_CLUSTER_TASK_STACK_SIZE(t3)
    lw      t2, RT_CLUSTER_TASK_SLAVE_STACK_SIZE(t3)
    lw      t5, RT_CLUSTER_TASK_CORE_MASK(t3)
    lw      s6, RT_CLUSTER_TASK_COMPLETION_CALLBACK(t3)
    lw      t6, RT_CLUSTER_TASK_NB_CORES(t3)
    mv      ra, s5

    add     sp, sp, t1

#ifdef ARCHI_NO_L1_TINY
    la      t4, __rt_cluster_nb_active_pe
    sw      t6, 0(t4)
#else
    sw      t6, %tiny(__rt_cluster_nb_active_pe)(x0)
#endif

#ifdef __RT_USE_ASSERT
    // Update stack checking information
    beqz    t1, __rt_no_stack_check
    sub     t4, sp, t1
    csrw    0x7D1, t4
    csrw    0x7D2, sp
    csrwi   0x7D0, 1
#endif

__rt_no_stack_check:
    // Whatever the number of cores, we need to setup the barrier as the master code is compiled to use it
    sw      t5, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_TEAM_CONFIG(s3)
#ifdef ARCHI_HAS_CC
    // When we have a cluster controller, don't configure the slave barrier
    // if we don't have have any slave
    beqz    t5, __rt_master_no_slave_barrier
#endif
    sw      t5, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_MASK(s3)
    sw      t5, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TARGET_MASK(s3)
__rt_master_no_slave_barrier:
#ifdef ARCHI_HAS_CC
    ori     t6, t5, 1<<ARCHI_CC_CORE_ID
    sw      t6, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_MASK + EU_BARRIER_SIZE(s3)
    sw      t6, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TARGET_MASK + EU_BARRIER_SIZE(s3)
#endif

    // Set stack on slaves
    // For that we push first the function for setting stack, then the stack size and the base
    p.beqimm t5, 0, __rt_master_loop_no_slave
    sw      s10, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)
    sw      t2, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)
    sw      sp, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s3)

__rt_master_loop_no_slave:

    // Call the entry point, this will directly come back to the master loop
    jr      t0


__rt_master_sleep:
#ifdef __RT_USE_PROFILE
    li      a0, GV_SEMIHOSTING_VCD_DUMP_TRACE
    lw      a1, %tiny(__rt_pe_trace)(x0)
    li      a2, 1
    ebreak
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


#ifndef ARCHI_HAS_NO_DISPATCH

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
#ifndef ARCHI_HAS_NO_BARRIER
    p.elw   t0, EU_BARRIER_DEMUX_OFFSET + EU_HW_BARR_TRIGGER_WAIT_CLEAR(s2)
#else
    jal     ra, __rt_team_barrier
#endif
#endif
    

__rt_wait_for_dispatch:

#ifdef __RT_USE_PROFILE
    li      a0, GV_SEMIHOSTING_VCD_DUMP_TRACE
    slli    t2, s3, 2
    lw      a1, %tiny(__rt_pe_trace)(t2)
    li      a2, 1
    ebreak
#endif

    // Wait for PC + arg information from dispatcher
    p.elw   t0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)
    p.elw   a0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)

#ifdef __RT_USE_PROFILE
    mv      t1, a0
    li      a0, GV_SEMIHOSTING_VCD_DUMP_TRACE
    slli    t2, s3, 2
    lw      a1, %tiny(__rt_pe_trace)(t2)
    li      a2, 0
    li      a4, 1
    ebreak
    mv      a0, t1
#endif

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

#ifdef __RT_USE_ASSERT
    csrwi   0x7D0, 0
#endif

    // Multiply the stack size by the core ID and add the stack base to get our stack
    p.elw   t0, EU_DISPATCH_DEMUX_OFFSET + EU_DISPATCH_FIFO_ACCESS(s2)
#ifdef ARCHI_HAS_CC
    // If the cluster has a cluster controller, the first slave has core ID 0
    // and thus we need to take the next stack
    addi     t5, s3, 1
    p.mul   t4, t5, a0
#else
    p.mul   t4, s3, a0
#endif
    add     sp, t4, t0

#ifdef __RT_USE_ASSERT
    beqz    a0, __rt_no_stack_check_end
    sub     t4, sp, a0
    csrw    0x7D1, t4
    csrw    0x7D2, sp
    csrwi   0x7D0, 1
#endif
__rt_no_stack_check_end:
    ret

#else

  li      s3, ARCHI_EU_DEMUX_ADDR
  li      s4, 1<<RT_CLUSTER_CALL_EVT
  la      s1, __rt_stacks_slave_base
  la      s2, __rt_stack_slave_size
  csrr    s5, CSR_MHARTID
  andi    s5, s5, 0x1f


__rt_slave_loop:
  lw      sp, 0(s1)
  lw      t2, 0(s2)

  beqz    t2, __rt_slave_wait_call

  mul     t2, t2, s5
  add     sp, sp, t2

  j       __rt_pe_entry

__rt_slave_wait_call:
    sw      s4, EU_CORE_MASK_OR(s3)
    p.elw   x0, EU_CORE_EVENT_WAIT_CLEAR(s3)
    sw      s4, EU_CORE_MASK_AND(s3)
    j       __rt_slave_loop

#endif



#if defined(ARCHI_HAS_CLUSTER)
#ifndef ARCHI_NO_L1_TINY
    .global __rt_dma_2d
__rt_dma_2d:

    sw  x8, -4(sp)
    sw  x9, -8(sp)
    sw  a0, -12(sp)
    sw  a1, -16(sp)
    sw  a2, -20(sp)

    li  a2, ARCHI_MCHAN_DEMUX_ADDR

    lw  x8, %tiny(__rt_dma_first_pending)(x0)

    lw  a1, CL_DMA_CMD_T_ID(x8)
    lw  a0, CL_DMA_CMD_T_SIZE(x8)
    p.bsetr a1, x0, a1
    sw  a1, MCHAN_STATUS_OFFSET(a2)
    lw  a1, CL_DMA_CMD_T_LENGTH(x8)

    beqz a0, __rt_dma_2d_done

__rt_dma_2d_redo:
    lw  x9, CL_DMA_CMD_T_CMD(x8)
    
    bgt a0, a1, __rt_dma_2d_not_last
    mv  a1, a0

__rt_dma_2d_not_last:
    lw  a0, MCHAN_CMD_OFFSET(a2)
    sw  a0, CL_DMA_CMD_T_ID(x8)

    lw  a0, CL_DMA_CMD_T_SIZE(x8)

    p.inserti x9, a1, MCHAN_CMD_CMD_LEN_WIDTH-1, MCHAN_CMD_CMD_LEN_BIT

    sw  x9, MCHAN_CMD_OFFSET(a2)   // cmd

    sub a0, a0, a1
    sw  a0, CL_DMA_CMD_T_SIZE(x8)

    lw  a0, CL_DMA_CMD_T_LOC_ADDR(x8)

    sw  a0, MCHAN_CMD_OFFSET(a2)   // local address
    add a0, a0, a1
    sw  a0, CL_DMA_CMD_T_LOC_ADDR(x8)


    lw  a0, CL_DMA_CMD_T_EXT_ADDR(x8)
    lw  a1, CL_DMA_CMD_T_STRIDE(x8)

    sw  a0, MCHAN_CMD_OFFSET(a2)   // external address
    add a0, a0, a1
    sw  a0, CL_DMA_CMD_T_EXT_ADDR(x8)

__rt_dma_2d_exit:
    lw  x8, -4(sp)
    lw  x9, -8(sp)
    lw  a0, -12(sp)
    lw  a1, -16(sp)
    lw  a2, -20(sp)

    mret

__rt_dma_2d_done:
    sw  x0, CL_DMA_CMD_T_EXT_ADDR(x8)

    lw  x9, CL_DMA_CMD_T_NEXT(x8)
    sw  x9, %tiny(__rt_dma_first_pending)(x0)

    li  x8, ARCHI_EU_DEMUX_ADDR + EU_SW_EVENTS_DEMUX_OFFSET + (RT_DMA_EVENT<<2)
    sw  x0, EU_CORE_TRIGG_SW_EVENT(x8)

    beqz x9, __rt_dma_2d_exit

    mv  x8, x9

    lw  a0, CL_DMA_CMD_T_SIZE(x8)
    lw  a1, CL_DMA_CMD_T_LENGTH(x8)

    j   __rt_dma_2d_redo



#endif
#endif