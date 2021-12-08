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

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "stdio.h"

void __rt_task_exit();

#ifdef ARCHI_HAS_CLUSTER

/*
 * Cluster tiny data
 * They are in tiny area for fast access from cluster side. As they local
 * they will be instantiated once for each cluster and are thus
 * naturally supporting multi cluster.
 */

// First task to be handled which was pushed from cluster side
RT_L1_TINY_DATA rt_task_t *__rt_task_first_cl;

// Last task to be handled which was pushed from cluster side
RT_L1_TINY_DATA rt_task_t *__rt_task_last_cl;

void __rt_task_master_entry(void *arg);



void rt_task_conf_init(rt_task_conf_t *conf)
{
  conf->stacks = NULL;
  conf->stack_size = 512;
  conf->nb_cores = rt_nb_pe();
  conf->cid = 0;
}



int rt_task_cluster_init(rt_task_cluster_t *cluster, rt_task_conf_t *conf, rt_event_t *event)
{
  int irq = rt_irq_disable();
  rt_task_cluster_loc_t *cluster_loc = NULL;

  // Get default conf if noone is specified
  rt_task_conf_t default_conf;
  if (conf == NULL)
  {
    conf = &default_conf;
    rt_task_conf_init(conf);
  }

  int cid = conf->cid;


  // Handle all allocations for early error checking

  // Allocate one event to handle end of cluster call
  if (rt_event_alloc(NULL, 1))
    goto error;

  // Cluster local structure
  cluster_loc = rt_alloc(RT_ALLOC_CL_DATA, sizeof(rt_task_cluster_loc_t));
  if (cluster_loc == NULL)
    goto error_dealloc;

  // Allocate stacks only if they are not specified and a stack size is
  // specified, which is the default behavior.
  if (conf->stacks == NULL && conf->stack_size != 0)
  {
    cluster->free_stacks = 1;
    conf->stacks = rt_alloc(RT_ALLOC_CL_DATA, conf->stack_size*conf->nb_cores);
    if (conf->stacks == NULL)
      goto error_dealloc;
  }
  else
  {
    cluster->free_stacks = 0;
  }


  // Init cluster structure information which are needed during other calls
  cluster->stacks = conf->stacks;
  cluster->stack_size = conf->stack_size;
  cluster->cid = conf->cid;
  cluster->loc = cluster_loc;
  cluster->nb_cores = conf->nb_cores;

  cluster_loc->__rt_task_first_fc = NULL;
  cluster_loc->__rt_task_last_fc = NULL;
  *(int *)rt_cluster_tiny_addr(cid, &__rt_task_first_cl) = 0;
  cluster_loc->__rt_task_first_fc_for_cl = NULL;

  cluster->end_event = rt_event_get_blocking(NULL);


  // Start the cluster with the task loop entry. We give here -1 as a stack so
  // that no stack is allocated as the stacks comes from the task framework.
  // The event will be used afterwards, once we notify the cores to leave the
  // task framework, to wait until they fully left it.
  rt_cluster_call(NULL, cid, __rt_task_master_entry, cluster, (void *)-1, 0, 0, conf->nb_cores, cluster->end_event);


  // Not really used as this call is synchronous, just enqueue the event if
  // one is specified.
  if (event)
  __rt_event_enqueue(event);


  rt_irq_restore(irq);

  return 0;

error_dealloc:
  rt_event_free(NULL, 1);
  if (cluster_loc)
    rt_free(RT_ALLOC_CL_DATA, cluster_loc, sizeof(rt_task_cluster_loc_t));

error:
  rt_irq_restore(irq);
  return -1;
}



void rt_task_cluster_deinit(rt_task_cluster_t *cluster, rt_event_t *event)
{
  int irq = rt_irq_disable();

  // Init now the counter to check that all cores has exited the loop, before
  // they start accessing it
  cluster->loc->nb_cores_done = cluster->nb_cores;
  rt_compiler_barrier();

  // Notify all the cores to leave the task framework with a special value
  cluster->loc->__rt_task_first_fc_for_cl = (rt_task_t *)-1;
  rt_compiler_barrier();
  eu_evt_trig(eu_evt_trig_cluster_addr(cluster->cid, RT_CLUSTER_CALL_EVT), 0);

  // Wait until they all leave the task code before freeing cluster struct as
  // it may still be used
  rt_irq_enable();
  while(*(volatile int *)&cluster->loc->nb_cores_done != 0)
  {
    __rt_wait_for_event(1<<RT_FC_SYNC);
  }
  rt_irq_disable();

  // Now free all allocated resources
  if (cluster->free_stacks)
  {
    rt_free(RT_ALLOC_CL_DATA, cluster->stacks, cluster->stack_size*cluster->nb_cores);
  }

  rt_free(RT_ALLOC_CL_DATA, cluster->loc, sizeof(rt_task_cluster_loc_t));

  // The event we used for the cluster call to enter the task framework has
  // never been released as the cluster cores just started from scratch.
  // We need to release it now to properly free it
  __rt_event_release(cluster->end_event);
  rt_event_free(NULL, 1);

  // Not really used as this call is synchronous, just enqueue the event if
  // one is specified.
  if (event)
  __rt_event_enqueue(event);


  rt_irq_restore(irq);
}



static void __rt_task_handle_end_of_task(void *_task)
{
  int irq = rt_irq_disable();
  rt_task_t *task = (rt_task_t *)_task;
  rt_task_cluster_loc_t *cluster_loc = task->cluster;
  rt_event_t *event = task->event;


  // Everytime a task is finished, first check if we can update the queue head
  // as it is not updated by cluster side to avoid race conditions.
  // At least this task won't be there anymore after we update, and maybe even
  // more tasks, which is not an issue, as we compare against the head.
  rt_task_t *current = cluster_loc->__rt_task_first_fc;
  while (current && current->pending == 0)
  {
    current = current->next;
  }

  if (current == NULL)
  {
    cluster_loc->__rt_task_first_fc = NULL;
    cluster_loc->__rt_task_last_fc = NULL;
  }
  else
  {
    cluster_loc->__rt_task_first_fc = current->next;
  }

  // Now handle the user event.
  // First restore it, as we used user event for our own purpose.
  __rt_event_restore(event);
  // And then enqueue it.
  __rt_event_enqueue(event);

  rt_irq_restore(irq);
}



void rt_task_fc_push(rt_task_cluster_t *cluster, rt_task_t *task, rt_event_t *event)
{
  int cid = cluster->cid;
  rt_task_cluster_loc_t *cluster_loc = cluster->loc;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  // We need to execute some code after the task is done to update the queues.
  // To simplify allocation, we reuse user event.
  rt_event_sched_t *sched = rt_event_internal_sched();
  __rt_event_save(call_event);
  __rt_init_event(call_event, sched, __rt_task_handle_end_of_task, (void *)task);
  __rt_event_set_pending(call_event);

  task->cluster = cluster_loc;
  task->event = call_event;
  task->next = NULL;
  rt_compiler_barrier();

  if (cluster_loc->__rt_task_last_fc)
  {
    cluster_loc->__rt_task_last_fc->next = task;
  }
  else
  {
    cluster_loc->__rt_task_first_fc = task;
  }

  cluster_loc->__rt_task_last_fc = task;

  rt_compiler_barrier();
  
  if (cluster_loc->__rt_task_first_fc_for_cl == NULL)
    cluster_loc->__rt_task_first_fc_for_cl = task;

  rt_compiler_barrier();
  eu_evt_trig(eu_evt_trig_cluster_addr(cid, RT_CLUSTER_CALL_EVT), 0);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_task_cl_push(rt_task_t *task)
{
  task->event = NULL;
  task->next = NULL;

  eu_mutex_lock_from_id(0);

  if (__rt_task_first_cl)
  {
    __rt_task_last_cl->next = task;
  }
  else
  {
    __rt_task_first_cl = task;
  }

  __rt_task_last_cl = task;

  eu_mutex_unlock_from_id(0);

  eu_evt_trig_from_id(RT_CLUSTER_CALL_EVT, 0);
}

void rt_task_cl_wait(rt_task_t *task)
{
  eu_evt_maskSet(1<<RT_CLUSTER_CALL_EVT);
  while(*(volatile rt_event_t **)&task->event == NULL)
  {
    eu_evt_waitAndClr();
  }
  eu_evt_maskClr(1<<RT_CLUSTER_CALL_EVT);
}

#endif