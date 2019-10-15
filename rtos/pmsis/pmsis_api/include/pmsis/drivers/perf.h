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

#ifndef __DRIVERS_PERF_H__
#define __DRIVERS_PERF_H__

/**        
 * @ingroup groupDrivers       
 */

/**        
 * @defgroup Perf Performance counters
 *
 * This API gives access to the core performance counters. Each core has a few
 * performance counters which can be configured 
 * to count one event out of several available. An event is a cycle, an
 * instruction, a cache miss and so on. The number of counters
 * limits the number of events which can be monitored at the same time and
 * depends on the platform. Real chips have only 1
 * counter while other platforms have one per event.
 *
 * In addition, this API uses a few other HW mechanisms useful for monitoring
 * performance such as timers.
 */

/**        
 * @addtogroup Perf
 * @{        
 */


/** \brief Configure performance counters.
 *
 * The set of events which can be activated at the same time depends on the
 * platform. On real chips (rather than with
 * the simulator), there is always only one counter. It is advisable to always
 * use only one to be compatible with simulator and chip.
 * At least PI_PERF_CYCLES and another event can be monitored at the same time
 * as the first one is using the timer.
 * This API can be called both from fabric-controller or cluster side.
 *
 * \param events A mask containing the events to activate. This is a bitfield,
 * so events identifier must be used like 1 << PI_PERF_CYCLES.
 */
static inline void pi_perf_conf(unsigned events);

/** \brief Reset all hardware performance counters.
 *
 * All hardware performance counters are set to 0, except the time, as it is
 * a shared resource within the cluster.
 * This API can be called both from fabric-controller or cluster side.
 *
 */
static inline void pi_perf_reset();

/** \brief Start monitoring configured events.
 *
 * This function is useful for finely controlling the point where performance
 * events start being monitored.
 * The counter retains its value between stop and start so it is possible to
 * easily sum events for several
 * portions of code.
 * This API can be called both from fabric-controller or cluster side.
 *
 */
static inline void pi_perf_start();

/** \brief Stop monitoring configured events.
 *
 * This API can be called both from fabric-controller or cluster side.
 */
static inline void pi_perf_stop();

/** \brief Read a performance counter
 *
 * This does a direct read to the specified performance counter. Calling this
 * function is useful for getting
 * the performance counter with very low overhead (just few instructions).
 * This API can be called both from fabric-controller or cluster side.
 * 
 * \param id    The performance event identifier to read.
 * \return      The performance counter value.
 */
static inline unsigned int pi_perf_read(int id);

//!@}

#endif