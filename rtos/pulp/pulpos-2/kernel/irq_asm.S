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


  .section .text_l2

  .global pos_irq_call_external_c_function
pos_irq_call_external_c_function:

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


  .global pos_irq_call_external_c_function_full
pos_irq_call_external_c_function_full:

    add  sp, sp, -128

    sw   ra, 0x00(sp)
    sw   gp, 0x04(sp)
    sw   tp, 0x08(sp)
    sw   t0, 0x0C(sp)
    sw   t1, 0x10(sp)
    sw   t2, 0x14(sp)
    sw   a0, 0x18(sp)
    sw   a1, 0x1C(sp)
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
    lw   a0, 0x18(sp)
    lw   a1, 0x1C(sp)
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


    .global pos_irq_c_handler_stub
pos_irq_c_handler_stub:
    add  sp, sp, -128

    sw   ra, 0x00(sp)
    sw   gp, 0x04(sp)
    sw   tp, 0x08(sp)
    sw   t0, 0x0C(sp)
    sw   t1, 0x10(sp)
    sw   t2, 0x14(sp)
    sw   a0, 0x18(sp)
    sw   a1, 0x1C(sp)
    sw   a2, 0x20(sp)
    sw   a3, 0x24(sp)
    sw   a4, 0x28(sp)
    sw   a5, 0x2C(sp)
    sw   a6, 0x30(sp)
    sw   a7, 0x34(sp)
    sw   t3, 0x38(sp)
    sw   t4, 0x3C(sp)
    sw   t5, 0x40(sp)
    sw   t6, 0x4C(sp)

    csrr   t0, 0x342
    slli   t0, t0, 2
    la     t1, pos_irq_c_handlers
#if !defined(ARCHI_CORE_HAS_PULPV2)
    add    t1, t0, t1
    lw     t1, 0x00(t1)
#else
    p.lw   t1, t0(t1)
#endif

    beqz   t1, pos_irq_no_handler

    jalr   ra, t1

pos_irq_no_handler:
    lw   ra, 0x00(sp)
    lw   gp, 0x04(sp)
    lw   tp, 0x08(sp)
    lw   t0, 0x0C(sp)
    lw   t1, 0x10(sp)
    lw   t2, 0x14(sp)
    lw   a0, 0x18(sp)
    lw   a1, 0x1C(sp)
    lw   a2, 0x20(sp)
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

    mret
