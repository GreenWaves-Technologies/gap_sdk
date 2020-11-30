#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

#include <pos/data/data.h>

    .section .text_l2

    .global pos_task_push_asm
pos_task_push_asm:

    // Can be called with following registers:
    //   x9/s1:  return address
    //   x10/a0: temporary register
    //   x11/a1: the task
    //   x12/a2: temporary register

    lw      x12, %tiny(pos_sched_first)(x0)
    sw      x0, PI_TASK_T_NEXT(x11)
    beqz    x12, __rt_no_first
    lw      x12, %tiny(pos_sched_last)(x0)
    sw      x11, PI_TASK_T_NEXT(x12)
    j       __rt_common

    __rt_no_first:
    sw      x11, %tiny(pos_sched_first)(x0)

    __rt_common:
    sw      x11, %tiny(pos_sched_last)(x0)

    enqueue_end:
    jr          x9



    // This interrupt handler is triggered by cluster for pushing
    // remotly an event
    // The event is temporarly put into a single per-cluster entry
    // The FC must get it and push it to the scheduler

    .global pos_task_remote_enqueue
pos_task_remote_enqueue:

    sw  x8, -4(sp)
    sw  x9, -8(sp)
    sw  a0, -12(sp)
    sw  a1, -16(sp)
    sw  a2, -20(sp)

    li   x8, ARCHI_NB_CLUSTER
    la   x9, pos_cluster

    // Loop over the clusters to see if there is an event to push
pos_task_remote_enqueue_event_loop_cluster:
    lw   a1, POS_CLUSTER_T_CL_TASKS(x9)
    lw   a2, POS_CLUSTER_T_POOL(x9)

    beq  a1, x0, pos_task_remote_enqueue_event_loop_cluster_continue

    lw   a0, POS_CLUSTER_CALL_POOL_T_FIRST_CALL_FC_FOR_CL(a2)

    bnez a0, pos_task_remote_enqueue_no_queue_update
    sw   x0, POS_CLUSTER_T_LAST_CALL_FC(x9)

pos_task_remote_enqueue_no_queue_update:
    lw   a2, POS_CLUSTER_T_TRIG_ADDR(x9)
    sw   x0, POS_CLUSTER_T_CL_TASKS(x9)

    sw   x0, 0(a2)

    la   x9, pos_task_remote_enqueue_event_loop_cluster_continue

    // Check if it is a normal event
    andi    a0, a1, 0x3
    bne     a0, x0, pos_task_callback

    j    pos_task_push_asm

pos_task_callback:
    li      x10, ~0x3
    and     x11, x11, x10
    lw      x12, PI_CALLBACK_T_ENTRY(x11)
    lw      x10, PI_CALLBACK_T_ARG(x11)
    j       pos_irq_call_external_c_function


pos_task_remote_enqueue_event_loop_cluster_continue:
    addi x8, x8, -1
    bgt  x8, x0, pos_task_remote_enqueue_event_loop_next_cluster



    lw  x8, -4(sp)
    lw  x9, -8(sp)
    lw  a0, -12(sp)
    lw  a1, -16(sp)
    lw  a2, -20(sp)

    mret


pos_task_remote_enqueue_event_loop_next_cluster:
    la   x9, pos_cluster
    li   a1, POS_CLUSTER_T_SIZEOF
    mul  a1, x8, a1
    add  x9, x9, a1
    j pos_task_remote_enqueue_event_loop_cluster
