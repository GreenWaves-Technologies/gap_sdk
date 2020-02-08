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

#ifndef __PMSIS_RTOS_MALLOC_L2_MALLOC_H__
#define __PMSIS_RTOS_MALLOC_L2_MALLOC_H__

#include "pmsis/rtos/malloc/malloc_internal.h"

/**
 * @addtogroup MemAlloc
 * @{
 */

/**
 * @defgroup L2_Malloc L2 Memory Allocation
 *
 * \brief Memory allocation in L2 memory from FC.
 *
 * @addtogroup L2_Malloc
 * @{
 */

#if !defined(__L2_MALLOC_NATIVE__) || (__L2_MALLOC_NATIVE__ == 0)

/**
 * \brief Allocate in L2 memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size           Size in bytes of the memory to be allocated.
 *
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l2_malloc(int size);

/**
 * \brief Free L2 memory.
 *
 * \param chunk          Chunk to be freed.
 * \param size           Size in bytes of the memory to be freed.
 */
void pi_l2_free(void *chunk, int size);

/**
 * \brief Allocate in L2 memory.
 *
 * The allocated memory is aligned on the specified number of bytes.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 *
 * \param size           Size in bytes of the memory to be allocated.
 * \param align          Alignment in bytes.
 *
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_l2_malloc_align(int size, int align);

/**
 * @cond IMPLEM
 */

#define pmsis_l2_malloc       pi_l2_malloc
#define pmsis_l2_malloc_align pi_l2_malloc_align
#define pmsis_l2_malloc_free  pi_l2_free
#define pmsis_l2_malloc_init  pi_l2_malloc_init
#define pmsis_l2_malloc_dump  pi_l2_malloc_dump

/**
 * \brief malloc init: initialize mutex and heap
 * Warning: thread unsafe by nature
 * \param heapstart heap start address
 * \param heap_size size of the heap
 */
void pi_l2_malloc_init(void *heapstart, uint32_t size);

void pi_l2_malloc_dump();

/**
 * @endcond IMPLEM
 */

/**
 * @} L2_Malloc
 */

/**
 * @} MemAlloc
 */

#endif  /* __L2_MALLOC_NATIVE__ */

#endif  /* __PMSIS_RTOS_MALLOC_L2_MALLOC_H__ */
