/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
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

#include "cmsis.h"
#include "platform/mbed_toolchain.h"
#include "platform/mbed_wait_api.h"

// This implementation of the wait functions will be compiled only
// if the RTOS is not present.
#ifndef MBED_CONF_RTOS_PRESENT

#include "hal/us_ticker_api.h"

void wait(float s)
{
    wait_us(s * 1000000.0f);
}

void wait_ms(int ms)
{
    wait_us(ms * 1000);
}

void wait_us(int us)
{
#if DEVICE_USTICKER
    const ticker_data_t *const ticker = get_us_ticker_data();
    uint32_t start = ticker_read(ticker);
    while ((ticker_read(ticker) - start) < (uint32_t)us);
#else // fallback to wait_ns for targets without usticker
    while (us > 1000) {
        us -= 1000;
        wait_ns(1000000);
    }
    if (us > 0) {
        wait_ns(us * 1000);
    }
#endif // DEVICE_USTICKER
}

#endif // #ifndef MBED_CONF_RTOS_PRESENT

// This wait_ns is used by both RTOS and non-RTOS builds

#ifdef __CORTEX_M
#if (__CORTEX_M == 0 && !defined __CM0PLUS_REV) || __CORTEX_M == 1
// Cortex-M0 and Cortex-M1 take 6 cycles per iteration - SUBS = 1, 2xNOP = 2, BCS = 3
#define LOOP_SCALER 6000
#elif (__CORTEX_M == 0 && defined __CM0PLUS_REV) || __CORTEX_M == 3 || __CORTEX_M == 4 || \
      __CORTEX_M == 23
// Cortex-M0+, M3, M4 and M23 take 5 cycles per iteration - SUBS = 1, 2xNOP = 2, BCS = 2
#define LOOP_SCALER 5000
#elif __CORTEX_M == 33
// Cortex-M33 can dual issue for 3 cycles per iteration (SUB,NOP) = 1, (NOP,BCS) = 2
#define LOOP_SCALER 3000
#elif __CORTEX_M == 7
// Cortex-M7 manages to dual-issue for 2 cycles per iteration (SUB,NOP) = 1, (NOP,BCS) = 1
// (The NOPs were added to stabilise this - with just the SUB and BCS, it seems that the
// M7 sometimes takes 1 cycle, sometimes 2, possibly depending on alignment)
#define LOOP_SCALER 2000
#endif
#elif defined __CORTEX_A
#if __CORTEX_A == 9
// Cortex-A9 can dual issue for 3 cycles per iteration (SUB,NOP) = 1, (NOP,BCS) = 2
#define LOOP_SCALER 3000
#endif
#endif

/* We only define the function if we've identified the CPU. If we haven't,
 * rather than a compile-time error, leave it undefined, rather than faulting
 * with an immediate #error. This leaves the door open to non-ARM
 * builds with or people providing substitutes for other CPUs, and only if
 * needed.
 */
#ifdef LOOP_SCALER

/* Timing seems to depend on alignment, and toolchains do not support aligning
 * functions well. So sidestep that by hand-assembling the code. Also avoids
 * the hassle of handling multiple toolchains with different assembler
 * syntax.
 */
MBED_ALIGN(8)
static const uint16_t delay_loop_code[] = {
    0x1E40, // SUBS R0,R0,#1
    0xBF00, // NOP
    0xBF00, // NOP
    0xD2FB, // BCS .-3        (0x00 would be .+2, so 0xFB = -5 = .-3)
    0x4770  // BX LR
};

/* Take the address of the code, set LSB to indicate Thumb, and cast to void() function pointer */
#define delay_loop ((void(*)()) ((uintptr_t) delay_loop_code | 1))

void wait_ns(unsigned int ns)
{
    uint32_t cycles_per_us = SystemCoreClock / 1000000;
    // Note that this very calculation, plus call overhead, will take multiple
    // cycles. Could well be 100ns on its own... So round down here, startup is
    // worth at least one loop iteration.
    uint32_t count = (cycles_per_us * ns) / LOOP_SCALER;

    delay_loop(count);
}
#endif // LOOP_SCALER
