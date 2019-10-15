/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __RT_RT_ALLOC_H__
#define __RT_RT_ALLOC_H__

#include "rt/rt_data.h"

/// @cond IMPLEM

#ifdef __RT_ALLOC_L2_MULTI

#define __RT_ALLOC_FC_DATA 0
#define __RT_ALLOC_FC_RET_DATA 0
#define __RT_ALLOC_FC_CODE 1
#define __RT_ALLOC_CL_CODE 2
#define __RT_ALLOC_L2_CL_DATA 2
#define __RT_ALLOC_PERIPH  2
#define __RT_ALLOC_CL_DATA(cid)  (3 + (cid))

#else

#if defined(ARCHI_HAS_FC)

#define __RT_ALLOC_FC_DATA 1
#define __RT_ALLOC_FC_RET_DATA 0
#define __RT_ALLOC_FC_CODE 0
#define __RT_ALLOC_CL_CODE 0
#define __RT_ALLOC_L2_CL_DATA 0
#define __RT_ALLOC_PERIPH  0
#define __RT_ALLOC_CL_DATA(cid)  (2 + (cid))

#else

#define __RT_ALLOC_FC_DATA 1
#define __RT_ALLOC_FC_RET_DATA 1
#define __RT_ALLOC_FC_CODE 0
#define __RT_ALLOC_CL_CODE 0
#define __RT_ALLOC_L2_CL_DATA 0
#define __RT_ALLOC_PERIPH  0
#define __RT_ALLOC_CL_DATA(cid)  (1 + (cid))

#endif

#endif

/// @endcond


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup MemAlloc Memory allocators
 *
 * This group of APIs provides:
 *   - an API for creating a user memory allocator, fed with a memory chunk given by the user.
 *   - a set of memory allocators for all the chip memories.
 *   - a portable memory allocator which provides a single entry point for all memory allocators. This one can also be called form cluster side.
 *
 */

/**        
 * @addtogroup MemAlloc
 * @{        
 */

/**        
 * @defgroup UserMemAlloc User memory allocators
 *
 * These allocators are not the same as the usual malloc allocator. They require the caller to provide 
 * the size of the previously allocated chunk when it is freed, in order to decrease the amount of metadata that is stored.
 * The allocator is using free memory to store metadata and thus the memory must be
 * directly accessible to the runtime (e.g. this should not be flash addresses).
 */

/**@{*/

/** \brief Create a user memory allocator.
 *
 * The memory allocator is created from the given chunk, and can then be immediately used to allocate memory.
 * \param alloc   A pointer to the memory allocator structure, which must be allocated by the caller.
 * \param chunk   The start address of the memory chunk to be managed by this memory allocator.
 * \param size    The size of the memory chunk to be managed by this memory allocator.
 */
void rt_user_alloc_init(rt_alloc_t *alloc, void *chunk, int size);



/** \brief Allocate memory.
 *
 * Allocate the specified amount of bytes. Note that the allocated memory is at least aligned on 4 bytes.
 * \param alloc   A pointer to the memory allocator structure, which was also given when creating the allocator.
 * \param size    The size in bytes to be allocated.
 * \return        The allocated chunk or NULL if there was not enough memory.
 */
void *rt_user_alloc(rt_alloc_t *alloc, int size);



/** \brief Free memory.
 *
 * Free the previously allocated chunk.
 * \param alloc   A pointer to the memory allocator structure, which was also given when creating the allocator.
 * \param chunk   The memory chunk to be freed.
 * \param size    The size of the memory chunk to be freed.
 */
void rt_user_free(rt_alloc_t *alloc, void *chunk, int size);



/** \brief Allocate aligned memory.
 *
 * Allocate the specified amount of bytes with the specified alignment.
 * \param alloc   A pointer to the memory allocator structure, which was also given when creating the allocator.
 * \param size    The size in bytes to be allocated.
 * \param align   The needed alignment in bytes.
 * \return        The allocated chunk or NULL if there was not enough memory.
 */
void *rt_user_alloc_align(rt_alloc_t *alloc, int size, int align);



/** \brief Return information about the allocator state.
 *
 * This can be useful in order to take over an existing allocator if it has only 1 free chunk.
 * \param alloc   A pointer to the memory allocator structure, which was also given when creating the allocator.
 * \param size    Returns the remaining amount of memory in bytes.
 * \param first_chunk    Returns the start address of the first free chunk.
 * \param nb_chunks   Returns the number of free chunks.
 */
void rt_user_alloc_info(rt_alloc_t *alloc, int *size, void **first_chunk, int *nb_chunks);



/** \brief Dump the state of the allocator.
 *
 * Useful for debugging memory corruptions which affects the memory allocator.
 * \param alloc   A pointer to the memory allocator structure, which was also given when creating the allocator.
 */
void rt_user_alloc_dump(rt_alloc_t *alloc);

//!@}

/**        
 * @}  
 */



/**        
 * @addtogroup MemAlloc
 * @{        
 */

/**        
 * @defgroup ChipMemAlloc Chip memory allocators
 *
 * This set of functions give access to the memory allocators instantiated by the runtime in order
 * to manage the chip memories.
 * Each function is supposed to return the allocator of one memory. However, depending on the architecture,
 * this can return NULL if the memory does not exist.
 */

/**@{*/


/** \brief Return the allocator for L1 cluster memory.
 *
 * \param  cid  The cluster identifier for which the information must be returned.
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_l1(int cid);



/** \brief Return the allocator for the fabric controller tightly coupled memory.
 *
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_fc_tcdm();



/** \brief Return the allocator for the L2 first private bank.
 *
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_l2_priv0();



/** \brief Return the allocator for the L2 second private bank.
 *
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_l2_priv1();



/** \brief Return the allocator for the L2 shared banks.
 *
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_l2_shared();



/** \brief Return the allocator for the L2 memory. Note that this one exists only if the only L2 allocators do not exist.
 *
 * \return The allocator if the memory exists or NULL.
 */
static inline rt_alloc_t *rt_alloc_l2();

//!@}

/**        
 * @} 
 */




/**        
 * @addtogroup MemAlloc
 * @{        
 */

/**        
 * @defgroup PortableMemAlloc Portable chip memory allocator
 *
 * Instead of focusing on which memory area to allocate chunks, this allocator focuses on the usage
 * of the memory to be allocated, so that the runtime can map it to the most appropriate memory area.
 * The goal is to increase the portability of the code, in particular between chips with slightly
 * different memory hierarchies.
 */

/**@{*/

/** \enum rt_alloc_e
 * \brief Allocator identifier.
 *
 * This can be used to describe the intended usage of the allocated memory.
 */
typedef enum {
  RT_ALLOC_FC_CODE     = __RT_ALLOC_FC_CODE,     /*!< Memory for fabric controller code. */
  RT_ALLOC_FC_DATA     = __RT_ALLOC_FC_DATA,     /*!< Memory for fabric controller data. */
  RT_ALLOC_FC_RET_DATA = __RT_ALLOC_FC_RET_DATA, /*!< Memory for fabric controller retentive data. */
  RT_ALLOC_CL_CODE     = __RT_ALLOC_CL_CODE,     /*!< Memory for cluster code. */
  RT_ALLOC_CL_DATA     = __RT_ALLOC_CL_DATA(0),  /*!< Memory for cluster data. */
  RT_ALLOC_L2_CL_DATA  = __RT_ALLOC_L2_CL_DATA,  /*!< Memory for L2 cluster data. */
  RT_ALLOC_PERIPH      = __RT_ALLOC_PERIPH,      /*!< Memory for peripherals data. */
} rt_alloc_e;


/** \brief Allocate memory for the specified usage.
 *
 * \param flags  Specify how the memory is supposed to be used, to determine which memory allocator must be used.
 * \param size   The size in bytes of the memory to be allocated.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *rt_alloc(rt_alloc_e flags, int size);



/** \brief Free memory for the specified usage.
 *
 * \param flags  Specify how the memory is supposed to be used, to determine which memory allocator must be used.
 * \param chunk  The chunk to be freed.
 * \param size   The size in bytes of the memory to be freed.
 */
void rt_free(rt_alloc_e flags, void *chunk, int size);

/** \brief Allocate aligned memory for the specified usage.
 *
 * \param flags  Specify how the memory is supposed to be used, to determine which memory allocator must be used.
 * \param size   The size in bytes of the memory to be allocated.
 * \param align  The alignment in bytes of the memory to be allocated.
 * \return The allocated chunk or NULL if there was not enough memory available.
 */
void *rt_alloc_align(rt_alloc_e flags, int size, int align);



//!@}

/**        
 * @} 
 */




/**        
 * @addtogroup MemAlloc
 * @{        
 */



/**        
 * @defgroup ClusterMemAlloc Cluster memory allocator
 *
 * This API gives access to the memory allocators from the cluster side.
 */



/**@{*/


/** \brief Alloc request structure.
 *
 * This structure is used by the runtime to manage a cluster remote allocation.
 * It must be instantiated once for each allocation and must be kept alive until the allocation is finished.
 * It can be instantiated as a normal variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_alloc_req_s rt_alloc_req_t ;


/** \brief Free request structure.
 *
 * This structure is used by the runtime to manage a cluster remote free.
 * It must be instantiated once for each free and must be kept alive until the free is finished.
 * It can be instantiated as a normal variable, for example as a global variable, a local one on the stack,
 * or through a memory allocator.
 */
typedef struct pi_cl_free_req_s rt_free_req_t ;



/** \brief Allocate memory for the specified usage from cluster side.
 *
 * \param flags  Specify how the memory is supposed to be used, to determine which memory allocator must be used.
 * \param size   The size in bytes of the memory to be allocated.
 * \param req       The request structure used for termination.
 */
void rt_alloc_cluster(rt_alloc_e flags, int size, rt_alloc_req_t *req);



/** \brief Free memory for the specified usage from cluster side.
 *
 * \param flags  Specify how the memory is supposed to be used, to determine which memory allocator must be used.
 * \param chunk  The chunk to be freed.
 * \param size   The size in bytes of the memory to be freed.
 * \param req    The request structure used for termination.
 */
void rt_free_cluster(rt_alloc_e flags, void *chunk, int size, rt_free_req_t *req);



/** \brief Wait until the specified allocation request has finished.
 *
 * This blocks the calling core until the specified cluster remote allocation is finished.
 *
 * \param req       The request structure used for termination.
 * \return          The allocated chunk or NULL if there was not enough memory available.
 */
static inline void *rt_alloc_cluster_wait(rt_alloc_req_t *req);



/** \brief Wait until the specified free request has finished.
 *
 * This blocks the calling core until the specified cluster remote free is finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void rt_free_cluster_wait(rt_free_req_t *req);

//!@}

/**        
 * @} 
 */



/// @cond IMPLEM

// TODO experimental feature, integrate it into the visible API once it is well tested
typedef enum {
  RT_ALLOC_CONF_POWER_DOWN = 1,
  RT_ALLOC_CONF_POWER_UP = 2,
  RT_ALLOC_CONF_POWER_RET = 3,
  RT_ALLOC_CONF_POWER_NON_RET = 4
} rt_alloc_conf_e;

void rt_alloc_conf(rt_alloc_e flags, void *chunk, int size, rt_alloc_conf_e conf);

#if defined(ARCHI_HAS_L2)
#ifdef __RT_ALLOC_L2_MULTI
#ifdef ARCHI_HAS_L2_SCM
#define __RT_NB_ALLOC_L2 4
#else
#define __RT_NB_ALLOC_L2 3
#endif
#else
#define __RT_NB_ALLOC_L2 1
#endif
#endif


#if defined(ARCHI_HAS_L1)

extern rt_alloc_t *__rt_alloc_l1;

static inline rt_alloc_t *rt_alloc_l1(int cid) { return &__rt_alloc_l1[cid]; }

void __rt_alloc_init_l1(int cid);

#else

static inline rt_alloc_t *rt_alloc_l1(int cid) { return NULL; }

#endif

#if defined(ARCHI_HAS_FC_TCDM)

extern rt_alloc_t __rt_alloc_fc_tcdm;

static inline rt_alloc_t *rt_alloc_fc_tcdm() { return &__rt_alloc_fc_tcdm; }

#else

static inline rt_alloc_t *rt_alloc_fc_tcdm() { return NULL; }

#endif



#if defined(__RT_ALLOC_L2_MULTI) && defined(ARCHI_HAS_L2)

extern rt_alloc_t __rt_alloc_l2[];

static inline rt_alloc_t *rt_alloc_l2_priv0() { return &__rt_alloc_l2[0]; }

static inline rt_alloc_t *rt_alloc_l2_priv1() { return &__rt_alloc_l2[1]; }

static inline rt_alloc_t *rt_alloc_l2_shared() { return &__rt_alloc_l2[2]; }

#else

static inline rt_alloc_t *rt_alloc_l2_priv0() { return NULL; }

static inline rt_alloc_t *rt_alloc_l2_priv1() { return NULL; }

static inline rt_alloc_t *rt_alloc_l2_shared() { return NULL; }

#endif

#if !defined(__RT_ALLOC_L2_MULTI) && defined(ARCHI_HAS_L2)

extern rt_alloc_t __rt_alloc_l2[];

static inline rt_alloc_t *rt_alloc_l2() { return &__rt_alloc_l2[0]; }

#else

static inline rt_alloc_t *rt_alloc_l2() { return NULL; }

#endif

void __rt_allocs_init();


#if defined(ARCHI_HAS_CLUSTER)

#include "hal/pulp.h"

void __rt_alloc_cluster_req(void *req);

#endif


/// @endcond



#endif
