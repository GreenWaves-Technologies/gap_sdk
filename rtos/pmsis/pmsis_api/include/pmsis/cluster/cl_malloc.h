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

#ifndef __PMSIS_CLUSTER_CL_MALLOC_H__
#define __PMSIS_CLUSTER_CL_MALLOC_H__

/**
 * @addtogroup MemAlloc
 * @{
 */

/**
 * @defgroup CL_L2_Malloc CL_L2 Memory allocation
 *
 * \brief Memory allocation in L2 from cluster cores.
 *
 * This API uses cluster delegation to FC to allocate memory in L2.
 *
 * @addtogroup CL_L2_Malloc
 * @{
 */

/**
 * \brief Cluster memory allocation request structure.
 *
 * This structure is used by the runtime to manage a cluster remote allocation.
 * It must be instantiated once for each allocation and must be kept alive until
 * the allocation is finished.
 * It can be instantiated as a normal variable, for example as a global
 * variable, a local one on the stack, or through a memory allocator.
 */
typedef struct pi_cl_alloc_req_s pi_cl_alloc_req_t ;


/**
 * \brief Cluster memory free request structure.
 *
 * This structure is used by the runtime to manage a cluster remote free.
 * It must be instantiated once for each free and must be kept alive until the
 * free is finished.
 * It can be instantiated as a normal variable, for example as a global
 * variable, a local one on the stack, or through a memory allocator.
 */
typedef struct pi_cl_free_req_s pi_cl_free_req_t ;

/**
 * \brief Allocate L2 memory from cluster side.
 *
 * \param size           Size in bytes of the memory to be allocated.
 * \param req            Request structure used for termination.
 */
void pi_cl_l2_malloc(int size, pi_cl_alloc_req_t *req);

/**
 * \brief Free L2 memory from cluster side.
 *
 * \param chunk          Chunk to be freed.
 * \param size           Size in bytes of the memory to be freed.
 * \param req            Request structure used for termination.
 */
void pi_cl_l2_free(void *chunk, int size, pi_cl_free_req_t *req);

/**
 * \brief Wait until the specified allocation request has finished.
 *
 * This blocks the calling core until the specified cluster remote allocation
 * is finished.
 *
 * \param req            Request structure used for termination.
 *
 * \return               Allocated chunk or NULL if there was not enough
 *                       memory available.
 */
static inline void *pi_cl_l2_malloc_wait(pi_cl_alloc_req_t *req);

/**
 * \brief Wait until the specified free request has finished.
 *
 * This blocks the calling core until the specified cluster remote free is
 * finished.
 *
 * \param req            Request structure used for termination.
 */
static inline void pi_cl_l2_free_wait(pi_cl_free_req_t *req);

/**
 * @} CL_L2_Malloc
 */

/**
 * @} MemAlloc
 */

#endif  /* __PMSIS_CLUSTER_CL_MALLOC_H__ */
