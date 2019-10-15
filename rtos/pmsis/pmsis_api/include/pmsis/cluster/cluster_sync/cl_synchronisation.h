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

#ifndef __CL_SYNCHRONISATION_H__
#define __CL_SYNCHRONISATION_H__

#include "pmsis/cluster/cl_pmsis_types.h"

// ----- FIXME: just for test purpose, should be in OS, not here!
#define L1_TAS_BASE (1<<20)
// -----

/** \brief init a spinlock structure
 * Need an allocated cluster l1 address
 * \param  spinlock ptr (an address in test and set area)
 * \param  lock_addr ptr in l1 cluster area
 */
static inline void cl_sync_init_spinlock(spinlock_t *spinlock, int32_t *lock_addr)
{
    spinlock->release_ptr = lock_addr;
    spinlock->lock_ptr = (int32_t*)((uint32_t)lock_addr | L1_TAS_BASE);
    *lock_addr = 0;
}

/** \brief lock a spinlock structure
 * \param  spinlock spinlock structure to take lock on
 */
static inline int32_t cl_sync_test_and_set_lock(spinlock_t *spinlock)
{
    hal_compiler_barrier();
    int32_t value = *(volatile int32_t*)spinlock->lock_ptr;
    hal_compiler_barrier();
    return value;
}

/** \brief effectuate a spinlock on a boolean in test and set area
 * Wait, using test and set area, until *spinlock != -1
 * May block
 * \param  spinlock spinlock structure to take the lock on
 */
static inline void cl_sync_spinlock_take(spinlock_t *spinlock)
{
    // the test and set is linked directly to the address, nothing special to do
    while(cl_sync_test_and_set_lock(spinlock) == -1){}; // wait while not 0
}


/** \brief Release a spinlock
 * release the spinlock by reseting it
 * \param  spinlock spinlock structure to release
 */
static inline void cl_sync_spinlock_release(spinlock_t *spinlock)
{
    hal_compiler_barrier();
    *(volatile int32_t*)spinlock->release_ptr = 0;
    hal_compiler_barrier();
}
#endif
