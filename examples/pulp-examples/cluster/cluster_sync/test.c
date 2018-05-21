/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define STACK_SIZE 2048

static int finished;
static int starting = 0;

static void pe_entry(void *arg)
{
  printf("(%d, %d) Start first task\n", rt_cluster_id(), rt_core_id());
  // Hardware mutex - this will be exposed properly in a subsequent release
  eu_mutex_lock(eu_mutex_addr(0));
  starting++;
  eu_mutex_unlock(eu_mutex_addr(0));
  // Do something with a time varying by core
  for (int i=0; i<(1000 + (rt_core_id() * 10)); i++)
  {
    int j = 0;
  }
  printf("(%d, %d) Finished first task\n", rt_cluster_id(), rt_core_id());
  // Re synchronize with the other cores
  rt_team_barrier();
  eu_mutex_lock(eu_mutex_addr(0));
  starting++;
  eu_mutex_unlock(eu_mutex_addr(0));
  // Do something with a time varying by core
  printf("(%d, %d) Start second task\n", rt_cluster_id(), rt_core_id());
  for (int i=0; i<(1000 + ((8 - rt_core_id()) * 10)); i++)
  {
    int j = 0;
  }
  printf("(%d, %d) Finished second task\n", rt_cluster_id(), rt_core_id());
}

static void cluster_entry(void *arg)
{
  printf("(%d, %d) Entered cluster with arg %x\n", rt_cluster_id(), rt_core_id(), (int)arg);

  rt_team_fork(8, pe_entry, (void *)0x12345678);

}

static void end_of_call(void *arg)
{
  printf("(%d, %d) Received end of call from cluster %d\n", rt_cluster_id(), rt_core_id(), (int)arg);
  finished--;
}

int main()
{
  printf("Entering main controller\n");

  rt_event_sched_t sched;
  rt_thread_t thread;

  rt_event_sched_init(&sched);
  if (rt_event_alloc(&sched, 4)) return -1;

  rt_cluster_mount(1, 0, 0, NULL);

  finished = 1;

  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  rt_cluster_call(NULL, 0, cluster_entry, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(&sched, end_of_call, (void *)0));

  while(finished) {
    rt_event_execute(&sched, 1);
  }

  rt_cluster_mount(0, 0, 0, NULL);
  printf("Test success: Exiting main controller - started=%d\n", starting);

  return 0;
}
