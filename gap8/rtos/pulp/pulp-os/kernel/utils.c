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

typedef struct rt_cbsys_s {
  int (*callback)(void *);
  void *arg;
  struct rt_cbsys_s *next;
} rt_cbsys_t;


RT_FC_DATA static rt_cbsys_t *cbsys_first[RT_CBSYS_NB];

void __rt_cbsys_del(__rt_cbsys_e cbsys_id, int (*cb)(void *), void *cb_arg)
{
  rt_cbsys_t *cbsys = cbsys_first[cbsys_id];
  rt_cbsys_t *prev = NULL;
  while(cbsys)
  {
    if (cbsys->callback == cb && cbsys->arg == cb_arg)
    {
      if (prev)
      {
        prev->next = cbsys->next;
      }
      else
      {
        cbsys_first[cbsys_id] = cbsys->next;
      }
      rt_free(RT_ALLOC_FC_RET_DATA, (void *)cbsys, sizeof(rt_cbsys_t));     
      return;
    }

    prev = cbsys;
    cbsys = cbsys->next;
  }
}

int __rt_cbsys_add(__rt_cbsys_e cbsys_id, int (*cb)(void *), void *cb_arg)
{
  rt_cbsys_t *cbsys = (rt_cbsys_t *)rt_alloc(RT_ALLOC_FC_RET_DATA, sizeof(rt_cbsys_t));
  if (cbsys == NULL) return -1;

  cbsys->callback = cb;
  cbsys->arg = cb_arg;
  cbsys->next = cbsys_first[cbsys_id];
  cbsys_first[cbsys_id] = cbsys;

  return 0;
}


int __rt_cbsys_exec(__rt_cbsys_e cbsys_id)
{
  rt_cbsys_t *cbsys = cbsys_first[cbsys_id];
  while (cbsys)
  {
    if (cbsys->callback(cbsys->arg)) return -1;
    cbsys = cbsys->next;
  }

  return 0;
}


RT_FC_BOOT_CODE void __rt_utils_init()
{
  for (int i=0; i<RT_CBSYS_NB; i++)
  {
    cbsys_first[i] = NULL;
  }
}

void __rt_fc_lock(rt_fc_lock_t *lock)
{
#if defined(ARCHI_HAS_FC)
  int irq = rt_irq_disable();
  while(lock->locked)
  {
    //lock->fc_wait = __rt_thread_current;
    __rt_event_execute(rt_event_internal_sched(), 1);
  }
  lock->locked = 1;
  rt_irq_restore(irq);
#else
  while (rt_tas_lock_32((uint32_t)&lock->lock) == -1)
  {

  }
#endif
}

static int __rt_fc_unlock_to_cluster(rt_fc_lock_t *lock)
{
#if defined(ARCHI_HAS_CLUSTER)
  if (lock->waiting) {
    rt_fc_lock_req_t *req = lock->waiting;
    lock->waiting = req->next;
    rt_compiler_barrier();
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);
    return 1;
  }
#else
#endif
  return 0;
}

void __rt_fc_unlock(rt_fc_lock_t *lock)
{
#if defined(ARCHI_HAS_FC)
  int irq = rt_irq_disable();
  if (!__rt_fc_unlock_to_cluster(lock))
  {
    lock->locked = 0;    
  }
  rt_irq_restore(irq);
#else
  rt_tas_unlock_32((uint32_t)&lock->lock, 0);
#endif
}

#if defined(ARCHI_HAS_CLUSTER)
static void __rt_fc_cluster_lock_req(void *_req)
{
  int irq = rt_irq_disable();
  rt_fc_lock_req_t *req = (rt_fc_lock_req_t *)_req;
  rt_fc_lock_t *lock = req->lock;
  if (req->req_lock)
  {
    // Request is a lock
    if (lock->locked)
    {
      // The lock is already taken, just register ourself in the wait list
      // and return
      if (lock->waiting) lock->last_waiting->next = req;
      else lock->waiting = req;
      lock->last_waiting = req;
      req->next = NULL;
    }
    else
    {
      rt_compiler_barrier();
      // The lock is not taken, take it and reply to the cluster
      req->done = 1;
      lock->locked = 1;
    __rt_cluster_notif_req_done(req->cid);
    }
  }
  else
  {
    // Request is an unlock, first check if we can give it to a cluster
    if (__rt_fc_unlock_to_cluster(lock))
    {
    }
    else
    {
      lock->locked = 0;
    } 

    rt_compiler_barrier();
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);

  }
  rt_irq_restore(irq);
}

#if defined(ARCHI_HAS_FC)

void __rt_fc_cluster_lock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req)
{
  req->lock = lock;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->req_lock = 1;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_fc_cluster_lock_req, (void *)req);
  __rt_cluster_push_fc_irq_event(&req->event);
}

void __rt_fc_cluster_unlock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req)
{
  req->lock = lock;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->req_lock = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_fc_cluster_lock_req, (void *)req);
  __rt_cluster_push_fc_irq_event(&req->event);
}

#else

void __rt_fc_cluster_lock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req)
{
  while (rt_tas_lock_32((uint32_t)&lock->lock) == -1)
  {

  }
}

void __rt_fc_cluster_unlock(rt_fc_lock_t *lock, rt_fc_lock_req_t *req)
{
  rt_tas_unlock_32((uint32_t)&lock->lock, 0);
}

#endif

#endif

