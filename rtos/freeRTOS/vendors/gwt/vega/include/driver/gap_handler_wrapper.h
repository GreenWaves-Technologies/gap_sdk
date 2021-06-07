/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis/targets/target.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * Machine mode IRQ handler wrapper
 * Macro to use to setup an IRQ. The registers will be saved before call to the
 * function.
 */
#define HANDLER_WRAPPER_LIGHT(f)                                    \
__attribute__((section(".text")))                                   \
static inline void __handler_wrapper_light_##f (void) {             \
    asm volatile("addi sp, sp, -(36*4)\n\t");                       \
    asm volatile("sw   ra, 0*4(sp)\n\t"                             \
                 "jal  ra, ASM_FUNC_SAVE_MINIMAL_CONTEXT\n\t"       \
                 "lw   tp, pxCurrentTCB\n\t"                        \
                 "sw   sp, 0*0(tp)\n\t");                           \
    asm volatile("jalr %0" :: "r" (f));                             \
    asm volatile("lw   tp, pxCurrentTCB\n\t");                      \
    asm volatile("lw   sp,  0*0(tp)\n\t"                            \
                 "jal  ra, ASM_FUNC_RESTORE_MINIMAL_CONTEXT\n\t"    \
                 "lw   ra, 0*4(sp)\n\t"                             \
                 "addi sp, sp, +(36*4)\n\t"                         \
                 "mret\n\t");                                       \
}
