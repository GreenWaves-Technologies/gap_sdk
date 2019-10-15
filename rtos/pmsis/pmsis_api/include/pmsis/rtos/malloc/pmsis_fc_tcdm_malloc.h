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

#ifndef __PMSIS_FC_TCDM_MALLOC_H__
#define __PMSIS_FC_TCDM_MALLOC_H__

#include "pmsis/rtos/malloc/pmsis_malloc_internal.h"

#if (__FC_MALLOC_NATIVE__ == 0)

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

/** \brief Allocate FC TCDM memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size   The size in bytes of the memory to be allocated.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_fc_tcdm_malloc(uint32_t size);

/** \brief Free FC TCDM memory.
 *
 * \param chunk  The chunk to be freed.
 * \param size   The size in bytes of the memory to be freed.
 */
void pi_fc_tcdm_free(void *chunk, int size);

/** \brief Allocate FC TCDM memory.
 *
 * The allocated memory is aligned on the specified number of bytes.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size   The size in bytes of the memory to be allocated.
 * \param align  The alignment in bytes.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_fc_tcdm_malloc_align(int size, int align);

//!@}

/**        
 * @} 
 */

/// @cond IMPLEM

void pmsis_fc_tcdm_malloc_init(void *heapstart, uint32_t size);

void pmsis_fc_tcdm_malloc_set_malloc_struct(malloc_t malloc_struct);

malloc_t pmsis_fc_tcdm_malloc_get_malloc_struct(void);

#define pmsis_fc_tcdm_malloc pi_fc_tcdm_malloc
#define pmsis_fc_tcdm_malloc_align pi_fc_tcdm_malloc_align
#define pmsis_fc_tcdm_malloc_free pi_fc_tcdm_free

/// @endcond

#endif

#endif  /* __PMSIS_FC_TCDM_MALLOC_H__ */
