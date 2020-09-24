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

#ifndef __SYSTEM_GAP8_H__
#define __SYSTEM_GAP8_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Define clock source values */
#define DEFAULT_SYSTEM_CLOCK           50000000u           /* Default System clock value */


/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern volatile uint32_t SystemCoreClock;

/**
 * @brief Setup the microcontroller system.
 *
 * This function configures the oscillator (FLL), setup caches and IRQ.
 * It also initializes malloc functions.
 * this function should be called from startup_device file.
 */
void system_init(void);

/**
 * @brief Setup timer used for SysTick.
 *
 * This function sets up low timer for a given tick rate.
 *
 * @param tick_rate_hz   Tick rate of SysTick timer.
 */
void system_setup_systick(uint32_t tick_rate_hz);

/**
 * @brief Update the SystemCoreClock variable.
 *
 * It must be called whenever the CPU frequency is changed during execution.
 * This function updates SystemCoreClock variable according to current CPU frequency.
 */
void system_core_clock_update(void);

/**
 * @brief Get current SystemCoreClock value.
 *
 * @returns Current core frequency.
 *
 */
uint32_t system_core_clock_get(void);

/**
 * @brief Stop execution.
 *
 * This function is called to stop an execution and stop the microcontroller.
 */
void system_exit(int32_t code);


#ifdef __cplusplus
}
#endif

#endif  /* __SYSTEM_GAP8_H__ */
