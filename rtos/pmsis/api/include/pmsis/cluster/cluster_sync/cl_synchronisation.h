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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup clusterDriver
 *
 * \defgroup ClusterSync Cluster synchronization
 *
 * \addtogroup ClusterSync
 * \{
 */


/**
 * \brief Return the number of available bitfields.
 *
 * This will return the number of bitfields which can be allocated. This is
 * equal to the number of physical bitfields minus the ones already allocated,
 * including the ones allocated for runtime usage.
 *
 * \return Number of bitfields.
 */
static inline int pi_cl_bitfield_nb_available();


/**
 * \brief Allocate a bitfield.
 *
 * This will allocate a bitfield and return a descriptor to it which can be
 * passed to other functions.
 *
 * \return The bitfield address  or 0 if no bitfield was available.
 */
static inline uint32_t pi_cl_bitfield_alloc();


/**
 * \brief Free a bitfield.
 *
 * This will free a bitfield and make it available for allocation.
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 */
static inline void pi_cl_bitfield_free(uint32_t bitfield);


/**
 * \brief Return bitfield identifier.
 *
 * This will return the bitfield identifier which is an integer from 0 to the
 * number of physical bitfields minus 1.
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 *
 * \return The bitfield identifier.
 */
static inline int pi_cl_bitfield_id(uint32_t bitfield);


/**
 * \brief Set the value of a bitfield.
 *
 * Set the whole value of the bitfield.
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 * \param value       Value to set in the bitfield.
 */
static inline void pi_cl_bitfield_value_set(uint32_t bitfield, uint32_t value);


/**
 * \brief Get the value of a bitfield.
 *
 * Get the whole value of the bitfield.
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 *
 * \return The bitfield value.
 */
static inline uint32_t pi_cl_bitfield_value_get(uint32_t bitfield);


/**
 * \brief Sets some bits of the bitfield to 1.
 *
 * All the bits at 1 in the specified mask are set to 1 (do an OR between the
 * bitfield value and the specified value).
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 * \param mask        Mask used to set the bits.
 */
static inline void pi_cl_bitfield_bits_set(uint32_t bitfield, uint32_t mask);


/**
 * \brief Clears some bits of the bitfield to 0.
 *
 * All the bits at 1 in the specified mask are set to 0 (do an AND between the
 * bitfield value and the negated value of the specified mask).
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 * \param mask        Mask used to clear the bits.
 */
static inline void pi_cl_bitfield_bits_clr(uint32_t bitfield, uint32_t mask);


/**
 * \brief Allocated a bit.
 *
 * Returns the position (from 0 to 31) of the least significant bit at 1,
 * and sets it to 0
 *
 * \param bitfield    Bitfield descriptor returned when it was allocated.
 *
 * \return The allocated bit.
 */
static inline uint32_t pi_cl_bitfield_bit_alloc(uint32_t bitfield);


/**
 * \brief Return the number of available semaphores.
 *
 * This will return the number of semaphores which can be allocated. This is
 * equal to the number of physical semaphores minus the ones already allocated,
 * including the ones allocated for runtime usage.
 *
 * \return Number of semaphores.
 */
static inline int pi_cl_sem_nb_available();


/**
 * \brief Allocate a semaphore.
 *
 * This will allocate a semaphore and return a descriptor to it which can be
 * passed to other functions.
 *
 * \return The semaphore address  or 0 if no semaphore was available.
 */
static inline uint32_t pi_cl_sem_alloc();


/**
 * \brief Free a semaphore.
 *
 * This will free a semaphore and make it available for allocation.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 */
static inline void pi_cl_sem_free(uint32_t sem);


/**
 * \brief Return semaphore identifier.
 *
 * This will return the semaphore identifier which is an integer from 0 to the
 * number of physical semaphore minus 1.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 *
 * \return The semaphore identifier.
 */
static inline int pi_cl_sem_id(uint32_t sem);


/**
 * \brief Set the value of a semaphore.
 *
 * Set the whole value of the semaphore.
 *
 * \param semaphore   Semaphore descriptor returned when it was allocated.
 * \param value       Value to set in the semaphore.
 */
static inline void pi_cl_sem_set(uint32_t sem, uint32_t value);


/**
 * \brief Get the value of a semaphore.
 *
 * Get the whole value of the semaphore.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 *
 * \return The semaphore value.
 */
static inline uint32_t pi_cl_sem_get(uint32_t sem);


/**
 * \brief Increment a semaphore.
 *
 * Increment the value of the semaphore by the specified value.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 * \param inc          Value to be added to the semaphore.
 *
 * \return The semaphore value.
 */
static inline void pi_cl_sem_inc(uint32_t sem, uint32_t inc);


/**
 * \brief Decrement a semaphore by 1.
 *
 * Decrement the value of the semaphore by the specified value.
 * This operation is a load which can block the caller if the semaphore value
 * is 0. In this case, the calling core is blocked until another core is setting
 * the semaphore to a value greater than 0, which will decrement the semaphore
 * value by 1 and unlock the core.
 * Note that this function is using a fast access but can only be called from the
 * worker cores (all the cores except the cluster controller).
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 *
 * \return The semaphore value after it has been decremented.
 */
static inline uint32_t pi_cl_sem_dec(uint32_t sem);


/**
 * \brief Decrement a semaphore by 1.
 *
 * Decrement the value of the semaphore by the specified value.
 * This operation is a load which can block the caller if the semaphore value
 * is 0. In this case, the calling core is blocked until another core is setting
 * the semaphore to a value greater than 0, which will decrement the semaphore
 * value by 1 and unlock the core.
 * Note that this function is using a slower access but can be called from any
 * core.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 *
 * \return The semaphore value after it has been decremented.
 */
static inline uint32_t pi_cl_sem_dec_cc(uint32_t sem);


/**
 * \brief Load a semaphore value and increment it by 1.
 *
 * This returns the current value of the semaphore and increment it by 1.
 *
 * \param semaphore    Semaphore descriptor returned when it was allocated.
 *
 * \return The semaphore value before it is incremented.
 */
static inline uint32_t pi_cl_sem_load_inc(uint32_t sem);


/**
 * \}
 */




/// @cond IMPLEM

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

/// @endcond

#ifdef __cplusplus
}
#endif
#endif
