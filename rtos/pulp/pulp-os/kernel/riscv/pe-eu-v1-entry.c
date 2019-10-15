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

unsigned int __rt_stacks_base;
unsigned int __rt_stacks_size;
static  void (* volatile __rt_cluster_fork_entry)(void *);
static void * volatile __rt_cluster_fork_arg;

static inline void __rt_barrier()
{
  pulp_barrier_notify(0);
  pulp_evt_wait();
  pulp_gpevt_clear(0);
}

void __rt_pe_entry()
{
  while(1)
  {
    while(__rt_cluster_fork_entry == 0)
    {
      eu_evt_maskWaitAndClr(1<<RT_FORK_EVT);
    }

    __rt_cluster_fork_entry(__rt_cluster_fork_arg);

    __rt_barrier();
    __rt_barrier();
  }
}

void __rt_cluster_pe_init(void *stacks, int stacks_size)
{
  __rt_stacks_base = (unsigned int)stacks;
  __rt_stacks_size = stacks_size;
  __rt_cluster_fork_entry = 0;
  pulp_barrier_setup(0, rt_nb_active_pe(), (1 << rt_nb_active_pe()) - 1);

  for (int i=0; i<rt_nb_active_pe(); i++)
  {
    eu_evt_maskSet_base(eu_evt_getClusterBase(i), (1<<PULP_HW_BAR_EVENT));
  }

}

void __rt_team_barrier()
{
  __rt_barrier();
}

void __rt_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
  __rt_cluster_fork_arg = arg;
  __rt_cluster_fork_entry = entry;
  eu_evt_trig(eu_evt_trig_addr(RT_FORK_EVT), -1);

  entry(arg);

  __rt_barrier();
  __rt_cluster_fork_entry = 0;
  __rt_barrier();
}