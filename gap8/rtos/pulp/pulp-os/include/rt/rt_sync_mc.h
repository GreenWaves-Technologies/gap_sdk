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

#ifndef __RT_RT_SYNC_MC_H__
#define __RT_RT_SYNC_MC_H__

#include "rt/rt_data.h"

/**        
 * @ingroup groupCluster
 */

/**        
 * @defgroup McSync Inter-cluster synchronization
 *
 * This API provides a set of features for synchronizing the clusters together (e.g inter-cluster locks and barriers).
 * 
 * Most of the features must be configured from the fabric controller and are only usable on a cluster.
 * 
 */

/**        
 * @addtogroup McSync
 * @{        
 */

/**        
 * @defgroup IcLock Inter-cluster locks
 *
 */

/**        
 * @addtogroup IcLock
 * @{        
 */


/** \brief Configuration structure for inter-cluster locks.
 *
 * This can be initialized to default values by calling rt_iclock_conf_init and then specific fields can be set to the desired values.
 */
typedef struct {
  unsigned int cluster_mask;  /**< Bitmask giving the set of clusters which can access the lock. Bit 0 is for cluster 0, a bit set to 1 means the cluster can access. */
} rt_iclock_conf_t;



/** \brief Get default configuration.
 *
 * This will set all fields of the given configuration to default values so that only specific ones can be set.
 *
 * This must be called by the fabric controller.
 *
 * \param conf A pointer to the configuration. This must be allocated by the caller and must be kept alive until the lock is allocated.
 */
void rt_iclock_conf_init(rt_iclock_conf_t *conf);



/** \brief Allocate inter-cluster lock.
 *
 * This will allocate all needed resources for the set of clusters specified in the configuration, and will initialize the lock, which becomes usable
 * by the cluster after this call returns.
 *
 * This must be called by the fabric controller.
 *
 * \param conf A pointer to the configuration. The configuration can be freed after this call.
 * \return The lock if the operation has succeeded, otherwise NULL.
 */
rt_iclock_t *rt_iclock_alloc(rt_iclock_conf_t *conf);



/** \brief Free the lock.
 *
 * Free all resources allocated for the lock and also the lock itself. After this call, the lock should not be used anymore.
 *
 * This must be called by the fabric controller.
 *
 * \param lock The fabric controller lock.
 */
void rt_iclock_free(rt_iclock_t *lock);



/** \brief Get the lock for the calling cluster which he can use for locking / unlocking.
 *
 * A cluster can not use the global lock allocated on fabric controller side. Instead it must call this function to get the lock
 * that it should use.
 *
 * This must be called from the cluster.
 *
 * \param lock The global lock allocated by the fabric controller.
 * \return The cluster lock.
 */
rt_iclock_cl_t *rt_iclock_cl(rt_iclock_t *lock);



/** \brief Get the lock.
 *
 * This will block the calling core until it is the only owner of the lock. If another core is owning the lock, it will sleep 
 * the calling cluster until the lock is released and is given to it (other cores could also be waiting for it).
 *
 * This must be called from cluster side.
 *
 * \param lock The cluster lock.
 */
void rt_iclock_lock(rt_iclock_cl_t *lock);



/** \brief Release the lock.
 *
 * This will make the lock available. If at least one core is waiting for the lock, one of them will become the owner of the lock and
 * will be woken-up.
 *
 * This must be called from cluster side.
 *
 * \param lock The cluster lock.
 */
void rt_iclock_unlock(rt_iclock_cl_t *lock);


//!@}

/**        
 * @}      
 */


/// @cond IMPLEM

/// @endcond


#endif
