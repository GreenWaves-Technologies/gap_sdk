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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"


void rt_iclock_conf_init(rt_iclock_conf_t *conf)
{
  conf->cluster_mask = (1<<rt_nb_cluster()) - 1;
}

static inline rt_iclock_cl_t *__rt_cl_lock_get(rt_iclock_t *lock, int id)
{
  return ((rt_iclock_cl_t **)(lock + 1))[id];
}

rt_iclock_cl_t *rt_iclock_cl(rt_iclock_t *lock)
{
  unsigned int mask = lock->cluster_mask;
  int my_cid = rt_cluster_id();
  int id;

  for (id=0; ; id++)
  {
    int cid = __FL1(mask);
    if (cid == my_cid) break;
    mask &= ~(1<<cid);
  }

  return __rt_cl_lock_get(lock, id);
}

static inline void __rt_cl_lock_set(rt_iclock_t *lock, int id, rt_iclock_cl_t *cl_lock)
{
  ((rt_iclock_cl_t **)(lock + 1))[id] = cl_lock;
}

rt_iclock_t *rt_iclock_alloc(rt_iclock_conf_t *conf)
{
  int id = 0;
  unsigned int mask = conf->cluster_mask;
  unsigned int nb_cluster = __builtin_clb(mask);

  // Allocate the global FC structure pointing to all per-cluster structures
  // The cluster structures pointers are put at the end of the global structure
  int lock_size = sizeof(rt_iclock_t) + nb_cluster * sizeof(void *);
  rt_iclock_t *lock = (rt_iclock_t *)rt_alloc(RT_ALLOC_FC_DATA, lock_size);
  if (lock == NULL) return NULL;

  // Set it to zero to be able to properly exit from error
  memset((void *)lock, 0, lock_size);

  lock->cluster_mask = mask;

  // Now allocate one cluster structure per active cluster in the cluster memory as each cluster will
  // use it for locking and unlocking
  unsigned int lock_addr = 0;
  while (mask)
  {
    int cid = __FL1(mask);
    mask &= ~(1<<cid);

    rt_iclock_cl_t *cl_lock = (rt_iclock_cl_t *)rt_alloc(RT_ALLOC_CL_DATA+cid, sizeof(rt_iclock_cl_t));
    if (cl_lock == NULL) goto error;

    // The first cluster is providing the global lock and each cluster
    // including first cluster is pointing to it
    if (lock_addr == 0) {
      cl_lock->tas = 0;
      lock_addr = (unsigned int)&cl_lock->tas;
    }
    cl_lock->addr = lock_addr;
    cl_lock->cluster_mask = conf->cluster_mask;

    __rt_cl_lock_set(lock, id, cl_lock);

    id++;
  }

  return lock;

error:
  // In case of error, just roll back everything, in particular free all per-cluster descriptors
  mask = conf->cluster_mask;
  for (int i=0; i<id; i++)
  {
    int cid = __FL1(mask);
    mask &= ~(1<<cid);
    rt_free(RT_ALLOC_CL_DATA+cid, __rt_cl_lock_get(lock, i), sizeof(rt_iclock_cl_t));
  }
  rt_free(RT_ALLOC_FC_DATA, (void *)lock, lock_size);
  return NULL;
}

void rt_iclock_lock(rt_iclock_cl_t *lock)
{
  // First lock the cluster so that only one core tries to acquire the soc lock
  // to avoid too many accesses outside
  eu_mutex_lock(eu_mutex_addr(0));

  // The multi-clock resides inside one of the cluster. Just wait until we can get it
  // The owner of the lock will broadcast the event to all clusters when unlocking
  while (rt_tas_lock_32(lock->addr) == -1)
  {
    eu_evt_maskWaitAndClr(1<<RT_CL_SYNC_EVENT);
  }

}

void rt_iclock_unlock(rt_iclock_cl_t *lock)
{
  // First unlock the global lock
  rt_tas_unlock_32(lock->addr, 0);

  // Broadcast the event to wake-up waiting clusters
  unsigned int mask = lock->cluster_mask;
  while (mask)
  {
    int cid = __FL1(mask);
    mask &= ~(1<<cid);
    eu_evt_trig(eu_evt_trig_cluster_addr(cid, RT_CL_SYNC_EVENT), 0);
  }

  // And unlock the local cores
  eu_mutex_unlock(eu_mutex_addr(0));
}

void rt_iclock_free(rt_iclock_t *lock)
{

}
