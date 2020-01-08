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

#ifndef __PMSIS_L2_MALLOC_H__
#define __PMSIS_L2_MALLOC_H__

/**
* @ingroup groupRTOS
*/

/**
 * @defgroup MemAlloc Memory allocation
 *
 * This provides support for memory allocation.
 */

/**        
 * @addtogroup MemAlloc
 * @{        
 */

/**@{*/

#include "pmsis/rtos/malloc/pmsis_malloc_internal.h"

#if !defined(__L2_MALLOC_NATIVE__) || (__L2_MALLOC_NATIVE__ == 0)

/** \brief Allocate L2 memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size   The size in bytes of the memory to be allocated.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l2_malloc(int size);

/** \brief Free L2 memory.
 *
 * \param chunk  The chunk to be freed.
 * \param size   The size in bytes of the memory to be freed.
 */
void pi_l2_free(void *chunk, int size);

/** \brief Allocate L2 memory.
 *
 * The allocated memory is aligned on the specified number of bytes.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size   The size in bytes of the memory to be allocated.
 * \param align  The alignment in bytes.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l2_malloc_align(int size, int align);

//!@}

/**        
 * @} 
 */

#endif  /* __L2_MALLOC_NATIVE__ */

/// @cond IMPLEM

//void pmsis_l2_malloc_set_malloc_struct(malloc_t malloc_struct);

//malloc_t pmsis_l2_malloc_get_malloc_struct(void);

void pmsis_l2_malloc_init(void *heapstart, uint32_t size);

void pmsis_l2_malloc_dump();

#define pmsis_l2_malloc pi_l2_malloc
#define pmsis_l2_malloc_align pi_l2_malloc_align
#define pmsis_l2_malloc_free pi_l2_free

/// @endcond

#endif  /* __PMSIS_L2_MALLOC_H__ */
