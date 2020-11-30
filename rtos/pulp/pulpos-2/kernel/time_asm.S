/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <pos/data/data.h>

    .section .text_l2

    .global pos_time_timer_handler_asm
pos_time_timer_handler_asm:
    add sp, sp, -8
    sw  x12, 0(sp)
    sw  x9, 4(sp)

    la   x12, pos_time_timer_handler
    la   x9, pos_time_timer_handler_asm_ret
    j    pos_irq_call_external_c_function_full

pos_time_timer_handler_asm_ret:
    lw  x9, 4(sp)
    lw  x12, 0(sp)
    add sp, sp, 8
    mret

