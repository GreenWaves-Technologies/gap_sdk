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

#ifndef __PMSIS_RTOS_MALLOC_FC_L1_MALLOC_H__
#define __PMSIS_RTOS_MALLOC_FC_L1_MALLOC_H__

#include "pmsis/rtos/malloc/malloc_internal.h"

#if !defined(__FC_MALLOC_NATIVE__) || (__FC_MALLOC_NATIVE__ == 0)

/**
 * @ingroup groupRTOS
 *
 * @defgroup MemAlloc Memory allocation
 *
 * \brief Memory management
 *
 * This provides support for memory allocation in internal memories of GAP8/GAP9.
 *
 * There are 3 types of memory :
 * * FC TCDM memory    : L1 memory close to Fabric Controller.
 * * Cluster L1 memory : L1 memory used by the cluster cores.
 * * L2 memory         : L2 memory used by both FC and cluster cores(using delegation).
 *
 * @addtogroup MemAlloc
 * @{
 */

/**
 * @defgroup FC_L1_Malloc FC_L1 Memory allocation
 *
 * \brief Memory allocation in FC TCDM memory(GAP8) or L2(GAP9).
 *
 * @addtogroup FC_L1_Malloc
 * @{
 */

/**
 * \brief Allocate in FC L1 memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 * This will allocate in the closest memory for the FC, which can vary
 * depending on the chip. Check the chip-specific section for more information.
 *
 * \param size           Size in bytes of the memory to be allocated.
 *
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_fc_l1_malloc(int size);

/**
 * \brief Free FC L1 memory.
 *
 * \param chunk          Chunk to be freed.
 * \param size           Size in bytes of the memory to be freed.
 */
void pi_fc_l1_free(void *chunk, int size);

/**
 * \brief Allocate in FC L1 memory.
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
void *pi_fc_l1_malloc_align(int size, int align);

/**
 * @cond IMPLEM
 */

/**
 * \brief FC_TCDM malloc.
 *
 * Allocate memory in FC TCDM.
 */
#define pi_fc_tcdm_malloc                      pi_fc_l1_malloc
/**
 * \brief FC_TCDM free.
 *
 * Free allocated memory in FC TCDM.
 */
#define pi_fc_tcdm_free                        pi_fc_l1_free
/**
 * \brief FC_TCDM aligned malloc.
 *
 * Allocate memory aligned in FC TCDM.
 */
#define pi_fc_tcdm_malloc_align                pi_fc_l1_malloc_align

#define pmsis_fc_tcdm_malloc                   pi_fc_l1_malloc
#define pmsis_fc_tcdm_malloc_init              pi_fc_l1_malloc_init
#define pmsis_fc_tcdm_malloc_set_malloc_struct pi_fc_l1_malloc_struct_set
#define pmsis_fc_tcdm_malloc_get_malloc_struct pi_fc_l1_malloc_struct_get

/**
 * \brief malloc init: initialize mutex and heap
 * Warning: thread unsafe by nature
 * \param heapstart heap start address
 * \param heap_size size of the heap
 */
void pi_fc_l1_malloc_init(void *heapstart, uint32_t size);

void pi_fc_l1_malloc_dump();

void pi_fc_l1_malloc_struct_set(malloc_t malloc_struct);

malloc_t pi_fc_l1_malloc_struct_get(void);

/**
 * @endcond IMPLEM
 */

/**
 * @} FC_L1_Malloc
 */

/**
 * @} MemAlloc
 */

#endif  /* (__FC_MALLOC_NATIVE__) || (__FC_MALLOC_NATIVE__ == 0) */

#endif  /* __PMSIS_RTOS_MALLOC_FC_L1_MALLOC_H__ */
