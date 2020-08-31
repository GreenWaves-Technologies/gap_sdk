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

#if RISCV_VERSION >= 4

  .global __rt_event_enqueue
__rt_event_enqueue:
  // Can be called with following registers:
  //   x9/s1:  return address
  //   x10/a0: temporary register
  //   x11/a1: the event
  //   x12/a2: temporary register

  // First check if it is a normal event
  andi    x10, x11, 0x3
  bne     x10, x0, __rt_handle_special_event

  // Enqueue normal event
  la      x10, __rt_sched
  sw      x0, RT_EVENT_T_NEXT(x11)
  lw      x12, RT_SCHED_T_FIRST(x10)
  beqz    x12, __rt_no_first
  lw      x12, RT_SCHED_T_LAST(x10)
  sw      x11, RT_EVENT_T_NEXT(x12)
  j       __rt_common

__rt_no_first:
  sw      x11, RT_SCHED_T_FIRST(x10)

__rt_common:
  sw      x11, RT_SCHED_T_LAST(x10)

enqueue_end:
  jr          x9

__rt_handle_special_event:
  li      x10, ~0x3
  and     x11, x11, x10
  lw      x12, PI_CALLBACK_T_ENTRY(x11)
  lw      x10, PI_CALLBACK_T_ARG(x11)
  j       __rt_call_external_c_function



    // This interrupt handler is triggered by the external bridge when it wants
    // to enqueue an event to the FC scheduler.

    .global __rt_bridge_enqueue_event
__rt_bridge_enqueue_event:

    sw  x8, -4(sp)
    sw  x9, -8(sp)
    sw  a0, -12(sp)
    sw  a1, -16(sp)
    sw  a2, -20(sp)

    // Everything is done from C code
    la      x12, __rt_bridge_handle_notif
    jal     x9, __rt_call_external_c_function

    lw  x8, -4(sp)
    lw  x9, -8(sp)
    lw  a0, -12(sp)
    lw  a1, -16(sp)
    lw  a2, -20(sp)

    mret




#if defined(ARCHI_HAS_CLUSTER)
    // This interrupt handler is triggered by cluster for pushing
    // remotly an event
    // The event is temporarly put into a single per-cluster entry
    // The FC must get it and push it to the scheduler

    .global __rt_remote_enqueue_event
__rt_remote_enqueue_event:

    sw  x8, -4(sp)
    sw  x9, -8(sp)
    sw  a0, -12(sp)
    sw  a1, -16(sp)
    sw  a2, -20(sp)

#ifndef ARCHI_NB_CLUSTER
    li   x8, 1
#else
    li   x8, ARCHI_NB_CLUSTER
#endif
    la   x9, __rt_fc_cluster_data

    // Loop over the clusters to see if there is an event to push
__rt_remote_enqueue_event_loop_cluster:
    lw   a1, RT_FC_CLUSTER_DATA_T_EVENTS(x9)
    beq  a1, x0, __rt_remote_enqueue_event_loop_cluster_continue


    // As there is an event also check if we should update the queue of calls

    // Everytime a task is finished, first check if we can update the queue head
    // as it is not updated by cluster side to avoid race conditions.
    // At least this task won t be there anymore after we update, and maybe even
    // more tasks, which is not an issue, as we compare against the head.
    lw   a1, RT_FC_CLUSTER_DATA_T_CLUSTER_POOL(x9)
    lw   a0, RT_CLUSTER_CALL_POOL_T_FIRST_CALL_FC(a1)

    beq  a0, x0, __rt_cluster_pool_update_end

__rt_cluster_pool_update_loop:
    lw    a2, RT_CLUSTER_TASK_PENDING(a0)
    bnez  a2, __rt_cluster_pool_update_loop_end

    lw   a0, RT_CLUSTER_TASK_NEXT(a0)
    bnez a0, __rt_cluster_pool_update_loop


__rt_cluster_pool_update_loop_end:
    
    beqz a0, __rt_cluster_pool_update_no_current

    lw   a0, RT_CLUSTER_TASK_NEXT(a0)
    sw   a0, RT_CLUSTER_CALL_POOL_T_FIRST_CALL_FC(a1)

    j    __rt_cluster_pool_update_end

__rt_cluster_pool_update_no_current:

    sw   x0, RT_CLUSTER_CALL_POOL_T_FIRST_CALL_FC(a1)
    sw   x0, RT_CLUSTER_CALL_POOL_T_FIRST_LAST_FC(a1)




__rt_cluster_pool_update_end:
    lw   a1, RT_FC_CLUSTER_DATA_T_EVENTS(x9)

    lw   a2, RT_FC_CLUSTER_DATA_T_TRIG_ADDR(x9)
    sw   x0, RT_FC_CLUSTER_DATA_T_EVENTS(x9)

    sw   x0, 0(a2)

    la   x9, __rt_remote_enqueue_event_loop_cluster_continue
    j    __rt_event_enqueue

__rt_remote_enqueue_event_loop_cluster_continue:
    addi x8, x8, -1
    bgt  x8, x0, __rt_remote_enqueue_event_loop_next_cluster



    lw  x8, -4(sp)
    lw  x9, -8(sp)
    lw  a0, -12(sp)
    lw  a1, -16(sp)
    lw  a2, -20(sp)

    mret

__rt_remote_enqueue_event_loop_next_cluster:
    la   x9, __rt_fc_cluster_data
    li   a1, RT_FC_CLUSTER_DATA_T_SIZEOF
    mul  a1, x8, a1
    add  x9, x9, a1
    j __rt_remote_enqueue_event_loop_cluster

#endif

#endif

  .global __rt_call_external_c_function
__rt_call_external_c_function:

    add  sp, sp, -128

    sw   ra, 0x00(sp)
    sw   gp, 0x04(sp)
    sw   tp, 0x08(sp)
    sw   t0, 0x0C(sp)
    sw   t1, 0x10(sp)
    sw   t2, 0x14(sp)
    sw   a3, 0x24(sp)
    sw   a4, 0x28(sp)
    sw   a5, 0x2C(sp)
    sw   a6, 0x30(sp)
    sw   a7, 0x34(sp)
    sw   t3, 0x38(sp)
    sw   t4, 0x3C(sp)
    sw   t5, 0x40(sp)
    sw   t6, 0x4C(sp)

    jalr ra, a2

    lw   ra, 0x00(sp)
    lw   gp, 0x04(sp)
    lw   tp, 0x08(sp)
    lw   t0, 0x0C(sp)
    lw   t1, 0x10(sp)
    lw   t2, 0x14(sp)
    lw   a3, 0x24(sp)
    lw   a4, 0x28(sp)
    lw   a5, 0x2C(sp)
    lw   a6, 0x30(sp)
    lw   a7, 0x34(sp)
    lw   t3, 0x38(sp)
    lw   t4, 0x3C(sp)
    lw   t5, 0x40(sp)
    lw   t6, 0x4C(sp)

    add  sp, sp, 128

    jr   x9
