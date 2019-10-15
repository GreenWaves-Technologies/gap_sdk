/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __RT_RT_TIME_H__
#define __RT_RT_TIME_H__

#include "rt/rt_data.h"

/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Time Time management
 * 
 */



/**        
 * @addtogroup Time
 * @{        
 */

/**        
 * @defgroup TimeClock System clock
 *
 */

/**@{*/

/** \brief Give the current time in microseconds.
 *
 * This returns the total amount of time elapsed since the runtime was started. Periods where the system
 * was powered-down are not counted.
 * Note that the time returned may start again from 0 after a while as the timer
 * used is limited, thus this functions is only suitable when computing
 * a difference of time.
 *
 * \return           The time in microseconds.
 */
unsigned int rt_time_get_us();




/** \brief Wait for a specific amount of time.
 *
 * Calling this function puts the calling core to sleep mode until the specified
 * amount of time has passed.
 * The time is specified in microseconds and is a minimum amount of time that
 * the core will sleep. The actual time may be bigger due to the timer
 * resolution.
 * Each call to this function is consuming an event, which is freed when the function returns.
 * It is up to the caller to make sure enough events have been allocated.
 *
 * \param time_us  The time to wait in microseconds.
 */
void rt_time_wait_us(int time_us);


/** \brief Wait for a specific number of clock cycles.
 *
 * Calling this function lets the calling core spin for the specified number of
 * clock cycles.  This function guarantees that at least the specified number of
 * clock cycles is executed.  The actual number of clock cycles spent in the
 * function might be slightly higher (especially for low cycle numbers), but
 * this is designed to be the most precise way to wait a specific number of
 * clock cycles.
 *
 * In order to be as generally applicable as possible, this function does not
 * interfere with any timers and does not depend on or use hardware loops.
 *
 * \param   cycles  The number of clock cycles to wait.
 */
static inline void rt_time_wait_cycles(const unsigned cycles);

//!@}

/**        
 * @}
 */


/// @cond IMPLEM

extern rt_event_t *first_delayed;

void rt_time_wait_cycles(const unsigned cycles)
{
    /**
     * Each iteration of the loop below will take four cycles on RI5CY (one for
     * `addi` and three for the taken `bnez`; if the instructions hit in the
     * I$).  Thus, we let `i` count the number of remaining loop iterations and
     * initialize it to a fourth of the number of clock cyles.  With this
     * initialization, we must not enter the loop if the number of clock cycles
     * is less than four, because this will cause an underflow on the first
     * subtraction.
     */
    register unsigned i = cycles >> 2;
    register unsigned threshold;
    asm volatile("li %[threshold], 4" : [threshold] "=r" (threshold));
    asm volatile goto("ble %[cycles], %[threshold], %l2"
            : /* no output */
            : [cycles] "r" (cycles), [threshold] "r" (threshold)
            : /* no clobbers */
            : __wait_cycles_end);
__wait_cycles_start:
    // Decrement `i` and loop if it is not yet zero.
    asm volatile("addi %0, %0, -1" : "+r" (i));
    asm volatile goto("bnez %0, %l1"
            : /* no output */
            : "r" (i)
            : /* no clobbers */
            : __wait_cycles_start);
__wait_cycles_end:
    return;
}

/// @endcond

#endif
