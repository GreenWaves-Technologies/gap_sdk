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

#ifndef __PMSIS_RTOS_MALLOC_CL_L1_MALLOC_H__
#define __PMSIS_RTOS_MALLOC_CL_L1_MALLOC_H__

#include "pmsis/rtos/malloc/malloc_internal.h"

/**
 * @addtogroup MemAlloc
 * @{
 */

/**
 * @defgroup CL_L1_Malloc CL_L1 Memory allocation
 *
 * \brief Memory allocation in Cluster L1 memory.
 *
 * @addtogroup CL_L1_Malloc
 * @{
 */

/**
 * \brief Allocate in Cluster L1 memory.
 *
 * The allocated memory is 4-bytes aligned.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 * This can be called only when the speficied cluster is opened.
 *
 * \param device         Cluster device where to allocate memory.
 * \param size           Size in bytes of the memory to be allocated.
 *
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_cl_l1_malloc(struct pi_device *device, uint32_t size);

/**
 * \brief Free Cluster L1 memory.
 *
 * This can be called only when the speficied cluster is opened.
 *
 * \param device         Cluster device where to free memory.
 * \param chunk          Chunk to be freed.
 * \param size           Size in bytes of the memory to be freed.
 */
void pi_cl_l1_free(struct pi_device *device, void *chunk, int size);

/**
 * \brief Allocate in Cluster L1 memory.
 *
 * The allocated memory is aligned on the specified number of bytes.
 * The caller has to provide back the size of the allocated chunk when freeing
 * it.
 * This can be called only when the speficied cluster is opened.
 *
 * \param device         Cluster device where to allocate memory.
 * \param size           Size in bytes of the memory to be allocated.
 * \param align          Alignment in bytes.
 *
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *pi_cl_l1_malloc_align(struct pi_device *device, int size, int align);

/**
 * @cond IMPLEM
 */

#define pmsis_l1_malloc(x...)             pi_cl_l1_malloc((void *) 0, x)
#define pmsis_l1_malloc_align(x...)       pi_cl_l1_malloc_align((void *) 0, x)
#define pmsis_l1_malloc_free(x...)        pi_cl_l1_free((void *) 0, x)
#define pmsis_l1_malloc_init              pi_cl_l1_malloc_init
#define pmsis_l1_malloc_set_malloc_struct pi_cl_l1_malloc_struct_set
#define pmsis_l1_malloc_get_malloc_struct pi_cl_l1_malloc_struct_get

#define pi_l1_malloc(x, y)          pi_cl_l1_malloc(x, y)
#define pi_l1_malloc_align(x, y, z) pi_cl_l1_malloc_align(x, y, z)
#define pi_l1_malloc_free(x, y)     pi_cl_l1_free(x, y)
#define pi_l1_free(x, y, z)     pi_cl_l1_free(x, y, z)

/**
 * \brief malloc init: initialize mutex and heap
 * Warning: thread unsafe by nature
 * \param heapstart heap start address
 * \param heap_size size of the heap
 */
void pi_cl_l1_malloc_init(void *heapstart, uint32_t size);

void pi_cl_l1_malloc_dump();

void pi_cl_l1_malloc_struct_set(malloc_t malloc_struct);

malloc_t pi_cl_l1_malloc_struct_get(void);

/**
 * @endcond IMPLEM
 */

/**
 * @} CL_L1_Malloc
 */

/**
 * @} MemAlloc
 */

#endif  /* __PMSIS_RTOS_MALLOC_CL_L1_MALLOC_H__ */
