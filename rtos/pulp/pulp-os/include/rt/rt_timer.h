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

#ifndef __RT_RT_TIMER_H__
#define __RT_RT_TIMER_H__

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
 * @defgroup VirtTimer Virtual timer
 *
 */

/**@{*/


/** \enum rt_timer_flags_e
 * \brief Configuration flags for virtual timers.
 *
 * This is used to describe hos to configure a virtual timer.
 */
typedef enum {
  RT_TIMER_PERIODIC = 1,  /*!< Periodic mode. The timer will trigger a function call periodically. */
  RT_TIMER_ONE_SHOT = 2   /*!< One-shot mode. The timer will trigger a function call only once and need to be re-armed. */
} rt_timer_flags_e;



/** \brief Create a virtual timer.
 *
 * A virtual timer can be created in order to trigger function callbacks
 * based on time. These callbacks can be triggered either periodically or
 * once.
 *
 * \param timer    A pointer to the timer descriptor, which must be allocated by the caller. The descriptor must remain valid until the timer is destroyed.
 * \param flags    The flags for configuring the timer.
 * \param event    The event. This must be used to describe the function callback called by the timer. This event cannot be NULL and cannot be a blocking event.
 * \return         0 if the operation is successfull, -1 otherwise.
 */
int rt_timer_create(rt_timer_t *timer, rt_timer_flags_e flags, rt_event_t *event);



/** \brief Start a timer.
 *
 * This will start triggering function callback calls. If the timer is configured
 * periodically, the call will be triggered everytime the specified amount of 
 * time has passed. If it is configured in one-shot mode, a call is triggered
 * only once and the timer must be re-armed using this function.
 *
 * \param timer    A pointer to the timer descriptor (the one provided when the timer was created).
 * \param time_us  The time in microseconds after which the function callback is called.
 */
void rt_timer_start(rt_timer_t *timer, int time_us);



/** \brief Stop a timer.
 *
 * This will stop triggering function callback calls.
 *
 * \param timer    A pointer to the timer descriptor (the one provided when the timer was created).
 */
void rt_timer_stop(rt_timer_t *timer);



/** \brief Destroy a timer.
 *
 * This frees all allocated resources. The timer must not be used anymore after
 * this call.
 *
 * \param timer    A pointer to the timer descriptor (the one provided when the timer was created).
 */
void rt_timer_destroy(rt_timer_t *timer);



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
