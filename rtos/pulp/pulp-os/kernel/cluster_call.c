/*
 * Copyright (c) 2018 ETH Zurich, University of Bologna, GreenWaves Technologies
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


#include "pmsis.h"
#include "pulprt_wrapper.h"

#ifndef __RT_CLUSTER_ASM

void cluster_irq_handler(void *arg)
{
  for (int cid=0; cid<NB_CLUSTERS; cid++)
  {
    pi_task_t *task = __rt_fc_cluster_data[cid];
    if (task != NULL)
    {
      // Everytime a task is finished, first check if we can update the queue head
      // as it is not updated by cluster side to avoid race conditions.
      // At least this task won't be there anymore after we update, and maybe even
      // more tasks, which is not an issue, as we compare against the head.
      rt_fc_cluster_data_t *cluster = &clusters[cid];
      rt_cluster_call_pool_t *cl_cluster = cluster->pool;

      struct pi_cluster_task *current = cl_cluster->first_call_fc;
      while (current && current->implem.pending == 0)
      {
        current = current->next;
      }

      if (current == NULL)
      {
        cl_cluster->first_call_fc = NULL;
        cl_cluster->last_call_fc = NULL;
      }
      else
      {
        cl_cluster->first_call_fc = current->next;
      }

      __rt_fc_cluster_data[cid] = NULL;

      __rt_event_handle_end_of_task(task);
    }
  }
}
#endif

int pi_cluster_send_task_to_cl_async(struct pi_device *device, struct pi_cluster_task *task, pi_task_t *async_task)
{
  rt_fc_cluster_data_t *data = (rt_fc_cluster_data_t *)device->data;

  int lock = __rt_cluster_lock(data);

  __rt_task_init(async_task);
  
  task->implem.pending = 1;

  rt_cluster_call_pool_t *cl_data = data->pool;

  if (task->nb_cores == 0)
    task->nb_cores = pi_cl_cluster_nb_cores();

  if (task->stacks == NULL)
  {
    if (task->stack_size == 0)
    {
      task->stack_size = 0x800;
      task->slave_stack_size = 0x400;
    }

    if (task->slave_stack_size == 0)
      task->slave_stack_size = task->stack_size;

    int stacks_size = task->stack_size + task->slave_stack_size * (task->nb_cores - 1);

    if (data->stacks == NULL || stacks_size != data->stacks_size)
    {
      if (data->stacks)
        rt_user_free(rt_alloc_l1(data->cid), data->stacks, data->stacks_size);

      data->stacks_size = stacks_size;
      data->stacks = rt_user_alloc(rt_alloc_l1(data->cid), stacks_size);

      if (data->stacks == NULL)
        goto error;
    }

    task->stacks = data->stacks;
  }

  task->completion_callback = async_task;
#ifdef ARCHI_HAS_CC
  task->implem.core_mask = (1<<(task->nb_cores-1)) - 1;
#else
  task->implem.core_mask = (1<<task->nb_cores) - 1;
#endif

  task->next = NULL;

  rt_compiler_barrier();

  if (cl_data->last_call_fc)
  {
    cl_data->last_call_fc->next = task;
  }
  else
  {
    cl_data->first_call_fc = task;
  }

  cl_data->last_call_fc = task;

  rt_compiler_barrier();
  
  if (cl_data->first_call_fc_for_cl == NULL)
    cl_data->first_call_fc_for_cl = task;

  rt_compiler_barrier();
  eu_evt_trig(eu_evt_trig_cluster_addr(data->cid, RT_CLUSTER_CALL_EVT), 0);

  __rt_cluster_unlock(data, lock);

  return 0;

error:
  __rt_cluster_unlock(data, lock);
  return -1;
}



int pi_cluster_send_task_to_cl(struct pi_device *device, struct pi_cluster_task *task)
{
  pi_task_t fc_task;

  pi_task_block(&fc_task);

  if (pi_cluster_send_task_to_cl_async(device, task, &fc_task))
  {
    return -1;
  }

  pi_task_wait_on(&fc_task);

  return 0;
}
