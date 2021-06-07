/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#if !defined(ASSEMBLY_LANGUAGE)

/* Control and Status Registers reset values. */
/* Machine mode with IRQ disabled, after MRET stay in MM. */
#define portINITIAL_MSTATUS ( 0x1880 )
/* Retrieve MTVEC address from linker script. */
extern uint8_t __irq_vector_base_m__;
#define portINITIAL_MEPC    ( &__irq_vector_base_m__ )
#define portINITIAL_MCAUSE  ( 0x00000000 )

#endif  /* ASSEMBLY_LANGUAGE */

/* Size of a word, in bytes. */
#define portWORD_SIZE                    ( 4 )
/* Number of chip specific additional extensions. */
#define portVEGA_ADDITIONAL_EXTENSIONS   ( 6 )

/* a0 - a7 */
#define portVEGA_ARGS_REGS               ( 8 )
/* t0 - t6 */
#define portVEGA_TEMP_REGS               ( 7 )
/* s0 - s11 */
#define portVEGA_SAVE_REGS               ( 12 )
/* Minimal context size to save for irq handler. */
/* a0-a7 + t0-t6 + ra */
#define portVEGA_MINIMAL_CONTEXT_SIZE    ( portVEGA_ARGS_REGS + portVEGA_TEMP_REGS + 1 )
/* General context size. */
/* ra + a0-a7 + t0-t6 + s0-s11 + mstatus + mepc */
#define portVEGA_CONTEXT_SIZE            ( portVEGA_MINIMAL_CONTEXT_SIZE + portVEGA_SAVE_REGS + 2 )
/* Chip's additional extensions to save. */
#define portVEGA_ADDITIONAL_CONTEXT_SIZE ( portVEGA_ADDITIONAL_EXTENSIONS )
/* GAP8 core total context size. */
#define portVEGA_FULL_CONTEXT_SIZE       ( portVEGA_CONTEXT_SIZE + portVEGA_ADDITIONAL_CONTEXT_SIZE )

/* Additionnal extensions. */
/* Hardware loops. */
#define LP_START_0  ( 0x7C0 )
#define LP_END_0    ( 0x7C1 )
#define LP_COUNT_0  ( 0x7C2 )
#define LP_START_1  ( 0x7C4 )
#define LP_END_1    ( 0x7C5 )
#define LP_COUNT_1  ( 0x7C6 )

/* Registers position on stack. */
#define portVEGA_REG_RA_POS      ( 0 )
#define portVEGA_REG_A0_POS      ( 1 )
#define portVEGA_REG_MSTATUS_POS ( 28 )
#define portVEGA_REG_MEPC_POS    ( 29 )

/* Stack alignment. */
#define portVEGA_STACKALIGN_BYTE ( 16 )
#define portVEGA_STACKALIGN_MASK ( portVEGA_STACKALIGN_BYTE - 1 )
