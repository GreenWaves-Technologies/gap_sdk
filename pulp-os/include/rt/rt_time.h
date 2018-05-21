/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Copyright (C) 2018 GreenWaves Technologies
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
unsigned long long rt_time_get_us();




/** \brief Wait for a specific amount of time.
 *
 * Calling this function puts the calling core to sleep mode until the specified
 * amount of time has passed.
 * The time is specified in microseconds and is a minimum amount of time that
 * the core will sleep. The actual time may be bigger due to the timer
 * resolution.
 *
 * \param time_us  The time to wait in microseconds.
 */
void rt_time_wait_us(int time_us);



//!@}

/**        
 * @}
 */


/// @cond IMPLEM

extern rt_event_t *first_delayed;

#if !defined(__LLVM__)
void __attribute__((interrupt)) __rt_timer_handler();
#else
void __rt_timer_handler();
#endif

/// @endcond

#endif
