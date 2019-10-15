/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __RT_RT_UTILS_H__
#define __RT_RT_UTILS_H__

#include <string.h>
#include "rt_data.h"
#include "archi/pulp.h"


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Utils Utility functions
 * 
 */



/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsRuntime Runtime information
 *
 */

/**@{*/


//!@}

/**        
 * @}  
 */





/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsArchi Architecture information
 *
 */

/**@{*/

/** \brief Tell if the calling core is the fabric controller.
 *
 * \return           1 if the calling core is the fabric controller, 0 otherwise.
 */
static inline int rt_is_fc();

/** \brief Give the cluster identifier of the calling core.
 *
 * \return           The cluster identifier.
 */
static inline int rt_cluster_id();

/** \brief Give the core identifier of the calling core.
 *
 * \return           The core identifier.
 */
static inline int rt_core_id();

/** \brief Returns the number of cores on the first cluster.
 *
 * \return           The number of cores.
 */
static inline int rt_nb_pe();


//!@}

/**        
 * @}  
 */



/**        
 * @addtogroup Utils
 * @{        
 */

/**        
 * @defgroup UtilsMem  Static data memory placement
 *
 * By default, a global variable is put in the fabric L2 memory, as it is the biggest one.
 * The following macros can be used to change this behavior and specify in which memory it must be placed.
 *
 * This has to be used on the declaration of the variable. A few other behavior can also be specified:
 *   - A variable can sometimes be put in the short section, resulting in better optimized accesses as the address generation can then be done with only 1 instruction instead of 2 or 3.
 *   - A variable is by default using a global address which allows the variable to be accessed from anywhere. For some memories, it is possible to ask for a local address so that the variable can be access as a local one on each cluster.
 * 
 */

/**@{*/


/** Put the data inside cluster 0 memory, using a gobal address. */
#define RT_CL_DATA __attribute__((section(".data_l1")))

/** Put the data inside each cluster memory, using a local address. */
#define RT_CL_ALIAS_DATA __attribute__((section(".data_alias_l1")))

/** Put the data inside each cluster memory, using a short local address. */
#define RT_CL_SHORT_DATA __attribute__((section(".data_tiny_l1")))



/** \brief Gives the start address of the runtime cluster code
 *
 * The runtime code used by the cluster is put into a specific section so that
 * it can be better controlled. This function can be used to get the start
 * address of this section.
 */
static inline void *rt_section_cluster_text_start();



/** \brief Gives the size of the runtime cluster code
 *
 * The runtime code used by the cluster is put into a specific section so that
 * it can be better controlled. This function can be used to get the size
 * of this section.
 */
static inline int rt_section_cluster_text_size();

//!@}

/**        
 * @}      
 */


/// @cond IMPLEM

static inline void rt_compiler_barrier();

static inline void rt_wait_for_interrupt();

static inline int rt_nb_cluster();

static inline signed char rt_tas_lock_8(unsigned int addr);

static inline void rt_tas_unlock_8(unsigned int addr, signed char value);

static inline signed short rt_tas_lock_16(unsigned int addr);

static inline void rt_tas_unlock_16(unsigned int addr, signed short value);

static inline int rt_tas_lock_32(unsigned int addr);

static inline void rt_tas_unlock_32(unsigned int addr, signed int value);

static inline int rt_cluster_id();

static inline uint32_t pi_core_id()
{
  return rt_core_id();
}

static inline uint32_t pi_cluster_id()
{
  return rt_cluster_id();
}

static inline uint32_t pi_nb_cluster_cores()
{
  return rt_nb_pe();
}

static inline uint32_t pi_is_fc()
{
  return rt_is_fc();
}

/// @endcond

#endif
