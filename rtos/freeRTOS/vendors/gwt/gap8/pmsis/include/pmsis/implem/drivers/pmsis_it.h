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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Size of a word, in bytes. */
#define PI_WORD_SIZE                    ( 4 )
/* Number of chip specific additional extensions. */
#define PI_GAP8_ADDITIONAL_EXTENSIONS   ( 6 )

/* a0 - a7 */
#define PI_GAP8_ARGS_REGS               ( 8 )
/* t0 - t6 */
#define PI_GAP8_TEMP_REGS               ( 7 )
/* s0 - s11 */
#define PI_GAP8_SAVE_REGS               ( 12 )
/* Minimal context size to save for irq handler. */
/* a0-a7 + t0-t6 + ra */
#define PI_GAP8_MINIMAL_CONTEXT_SIZE    ( PI_GAP8_ARGS_REGS + PI_GAP8_TEMP_REGS + 1 )
/* General context size. */
/* ra + a0-a7 + t0-t6 + s0-s11 + mstatus + mepc */
#define PI_GAP8_CONTEXT_SIZE            ( PI_GAP8_MINIMAL_CONTEXT_SIZE + PI_GAP8_SAVE_REGS + 2 )
/* Chip's additional extensions to save. */
#define PI_GAP8_ADDITIONAL_CONTEXT_SIZE ( PI_GAP8_ADDITIONAL_EXTENSIONS )
/* GAP8 core total context size. */
#define PI_GAP8_FULL_CONTEXT_SIZE       ( PI_GAP8_CONTEXT_SIZE + PI_GAP8_ADDITIONAL_CONTEXT_SIZE )

/* Additionnal extensions. */
/* Hardware loops. */
#define LP_START_0  ( 0x7B0 )
#define LP_END_0    ( 0x7B1 )
#define LP_COUNT_0  ( 0x7B2 )
#define LP_START_1  ( 0x7B4 )
#define LP_END_1    ( 0x7B5 )
#define LP_COUNT_1  ( 0x7B6 )
