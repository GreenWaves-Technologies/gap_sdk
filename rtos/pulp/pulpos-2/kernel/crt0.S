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

#include "archi/pulp.h"
#include "pos/data/irq.h"

    .section .text_l2
    .global pos_init_entry
pos_init_entry:

#ifdef ARCHI_HAS_CLUSTER

    # Cluster PEs will also starts here to avoid aligning another entry point
    # Just re-route them to the right entry
    csrr    a0, 0xF14
    li      a2, ARCHI_FC_COREID
    bne     a0, a2, pi_cl_entry_stub

#endif



    # Stack initialization
    la   x2, stack



    /* Do all other initializations from C code */
    la   t0, pos_init_start
    jalr x1, t0

    // Should never come back here
pos_init_end:
    j    pos_init_end

pi_cl_entry_stub:
    la   t0, pi_cl_entry
    jr   t0


.section .vectors, "ax"
.option norvc;


#ifdef ARCHI_CORE_HAS_1_10
    j pos_illegal_instr_asm
#else
    j pos_no_irq_handler
#endif

    j POS_IRQ_HANDLER1
    j POS_IRQ_HANDLER2
    j POS_IRQ_HANDLER3
    j POS_IRQ_HANDLER4
    j POS_IRQ_HANDLER5
    j POS_IRQ_HANDLER6
    j POS_IRQ_HANDLER7
    j POS_IRQ_HANDLER8
    j POS_IRQ_HANDLER9
    j POS_IRQ_HANDLER10
    j POS_IRQ_HANDLER11
    j POS_IRQ_HANDLER12
    j POS_IRQ_HANDLER13
    j POS_IRQ_HANDLER14
    j POS_IRQ_HANDLER15
    j POS_IRQ_HANDLER16
    j POS_IRQ_HANDLER17
    j POS_IRQ_HANDLER18
    j POS_IRQ_HANDLER19
    j POS_IRQ_HANDLER20
    j POS_IRQ_HANDLER21
    j POS_IRQ_HANDLER22
    j POS_IRQ_HANDLER23
    j POS_IRQ_HANDLER24
    j POS_IRQ_HANDLER25
    j POS_IRQ_HANDLER26
    j POS_IRQ_HANDLER27
    j POS_IRQ_HANDLER28
    j POS_IRQ_HANDLER29
    j POS_IRQ_HANDLER30
    j POS_IRQ_HANDLER31


.section .cluster_vectors, "ax"
.option norvc;


#ifdef ARCHI_CORE_HAS_1_10
    j pos_illegal_instr_asm
#else
    j pos_no_irq_handler
#endif

    j POS_IRQ_HANDLER1
    j POS_IRQ_HANDLER2
    j POS_IRQ_HANDLER3
    j POS_IRQ_HANDLER4
    j POS_IRQ_HANDLER5
    j POS_IRQ_HANDLER6
    j POS_IRQ_HANDLER7
    j POS_IRQ_HANDLER8
    j POS_IRQ_HANDLER9
    j POS_IRQ_HANDLER10
    j POS_IRQ_HANDLER11
    j POS_IRQ_HANDLER12
    j POS_IRQ_HANDLER13
    j POS_IRQ_HANDLER14
    j POS_IRQ_HANDLER15
    j POS_IRQ_HANDLER16
    j POS_IRQ_HANDLER17
    j POS_IRQ_HANDLER18
    j POS_IRQ_HANDLER19
    j POS_IRQ_HANDLER20
    j POS_IRQ_HANDLER21
    j POS_IRQ_HANDLER22
    j POS_IRQ_HANDLER23
    j POS_IRQ_HANDLER24
    j POS_IRQ_HANDLER25
    j POS_IRQ_HANDLER26
    j POS_IRQ_HANDLER27
    j POS_IRQ_HANDLER28
    j POS_IRQ_HANDLER29
    j POS_IRQ_HANDLER30
    j POS_IRQ_HANDLER31

    .org 0x90
    .global __rt_debug_struct_ptr
    __rt_debug_struct_ptr:
    .word 0xdeadbeef


    .global _start
_start:
    la  a0, pos_init_entry
    jalr x0, a0

pos_illegal_instr_asm:
    add sp, sp, -8
    sw  x12, 0(sp)
    sw  x9, 4(sp)
    la   x12, pos_irq_illegal_instr
    la   x9, pos_illegal_instr_asm_ret
    j    pos_irq_call_external_c_function_full

pos_illegal_instr_asm_ret:
    lw  x9, 4(sp)
    lw  x12, 0(sp)
    add sp, sp, 8
    mret


    j pos_illegal_instr_asm

pos_no_irq_handler:
    mret


    .global pos_semihosting_call
pos_semihosting_call:
    ebreak
    jr          ra

