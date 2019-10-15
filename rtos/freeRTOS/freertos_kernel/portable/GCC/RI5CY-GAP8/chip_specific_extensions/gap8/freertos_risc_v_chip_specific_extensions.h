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

#define portWORD_SIZE                    ( 4 )
#define portGAP8_ADDITIONAL_EXTENSIONS   ( 6 )

/* ra + a0-a7 + t0-t6 + s0-s11 + mstatus + mepc */
#define portGAP8_CONTEXT_SIZE            ( 30 )
#define portGAP8_ADDITIONAL_CONTEXT_SIZE ( portGAP8_ADDITIONAL_EXTENSIONS )

/* Additionnal extensions. */
/* Hardware loops. */
#define LP_START_0  ( 0x7B0 )
#define LP_END_0    ( 0x7B1 )
#define LP_COUNT_0  ( 0x7B2 )
#define LP_START_1  ( 0x7B4 )
#define LP_END_1    ( 0x7B5 )
#define LP_COUNT_1  ( 0x7B6 )
