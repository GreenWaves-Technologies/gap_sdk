
/** \addtogroup rtos */
/** @{*/
/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef RTOS_IDLE_H
#define RTOS_IDLE_H

#include "mbed_toolchain.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup rtos_Idle Idle hook function
 * @{
 */
/**
 @note
 Sets the hook function called by idle task
 @param fptr Hook function pointer.
 */
void rtos_attach_idle_hook(void (*fptr)(void));

/** @private */
MBED_NORETURN void rtos_idle_loop(void);

/** @}*/

#ifdef __cplusplus
}
#endif

#endif

/** @}*/

