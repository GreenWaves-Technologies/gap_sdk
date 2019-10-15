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

#ifndef __PMSIS_L1_MALLOC_H__
#define __PMSIS_L1_MALLOC_H__

#include "pmsis/rtos/malloc/pmsis_malloc_internal.h"

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

/** \brief Allocate L1 memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 * This can be called only when the speficied cluster is opened.
 *
 * \param device  The cluster device where to allocate memory.
 * \param size   The size in bytes of the memory to be allocated.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l1_malloc(struct pi_device *device, uint32_t size);

/** \brief Free L1 memory.
*
 * This can be called only when the speficied cluster is opened.
 *
 * \param device  The cluster device where to free memory.
 * \param chunk  The chunk to be freed.
 * \param size   The size in bytes of the memory to be freed.
 */
void pi_l1_free(struct pi_device *device, void *chunk, int size);

/** \brief Allocate L1 memory.
 *
 * The allocated memory is aligned on the specified number of bytes.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 * This can be called only when the speficied cluster is opened.
 *
 * \param device  The cluster device where to allocate memory.
 * \param size   The size in bytes of the memory to be allocated.
 * \param align  The alignment in bytes.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l1_malloc_align(struct pi_device *device, int size, int align);

//!@}

/**        
 * @} 
 */

/// @cond IMPLEM

void pmsis_l1_malloc_init(void *heapstart, uint32_t size);

void pmsis_l1_malloc_set_malloc_struct(malloc_t malloc_struct);

malloc_t pmsis_l1_malloc_get_malloc_struct(void);

#define pmsis_l1_malloc(x...) pi_l1_malloc((void *)0, x)
#define pmsis_l1_malloc_align(x...) pi_l1_malloc_align((void *)0, x)
#define pmsis_l1_malloc_free(x...) pi_l1_free((void *)0, x)

/// @endcond

#endif  /* __PMSIS_L1_MALLOC_H__ */
