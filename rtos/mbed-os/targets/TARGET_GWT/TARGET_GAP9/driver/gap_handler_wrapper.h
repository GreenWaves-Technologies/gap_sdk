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

#ifndef _GAP_HANDLER_HELPER_H_
#define _GAP_HANDLER_HELPER_H_

#include <assert.h>
#include "cmsis.h"
#include "gap_util.h"

/*!
 * @addtogroup handler_wrapper
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Machine mode IRQ handler wrapper */
#define Handler_Wrapper(f)                                                     \
__attribute__((section(".text")))                                              \
static inline void __handler_wrapper_##f (void) {                            \
    asm volatile( "addi sp, sp, -18*4\n\t"                                     \
                  "sw   ra, 15*4(sp)\n\t"                                      \
                  "jal  Exception_handler\n\t");                               \
    asm volatile("jalr  %0" :: "r" (f));                                       \
    asm volatile("j    SVC_Context");                                          \
}

/* Machine mode IRQ handler wrapper light version */
#define Handler_Wrapper_Light(f)                                    \
__attribute__((section(".text")))                                   \
static inline void __handler_wrapper_light_##f (void) {           \
    asm volatile( "addi sp, sp, -11*4\n\t"                          \
                  "sw    ra,  32(sp)\n\t"                           \
                  "sw    a0,  28(sp)\n\t"                           \
                  "sw    a1,  24(sp)\n\t"                           \
                  "sw    a2,  20(sp)\n\t"                           \
                  "sw    a3,  16(sp)\n\t"                           \
                  "sw    a4,  12(sp)\n\t"                           \
                  "sw    a5,  8(sp)\n\t"                            \
                  "sw    a6,  4(sp)\n\t"                            \
                  "sw    a7,  0(sp)\n\t");                          \
    asm volatile("jalr  %0" :: "r" (f));                            \
    asm volatile("lw     ra,  32(sp)\n\t"                           \
                 "lw     a0,  28(sp)\n\t"                           \
                 "lw     a1,  24(sp)\n\t"                           \
                 "lw     a2,  20(sp)\n\t"                           \
                 "lw     a3,  16(sp)\n\t"                           \
                 "lw     a4,  12(sp)\n\t"                           \
                 "lw     a5,  8(sp)\n\t"                            \
                 "lw     a6,  4(sp)\n\t"                            \
                 "lw     a7,  0(sp)\n\t"                            \
                 "addi   sp, sp, 11*4\n\t"                          \
                 "mret\n\t");                                       \
}
/* @} */

#endif /*_GAP_HANDLER_HELPER_H_*/
