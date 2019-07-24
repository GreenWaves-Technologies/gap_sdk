/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define NB_FC_TASKS 64

static int result;
static int nb_tasks_done;
static rt_task_t tasks[NB_FC_TASKS];
static rt_task_cluster_t cluster;

static void task_entry(rt_task_t *task, int id)
{
  rt_team_critical_enter();
  result += (int)task + id;
  rt_team_critical_exit();

  volatile int i;
  for (i=0; i<id*10; i++);

  //printf("[%d, %d] Entered task %p with ID %d\n", rt_cluster_id(), rt_core_id(), task, id);
}

static void end_of_fc_task(void *arg)
{
  int task_id = (int)arg;
  nb_tasks_done++;
}

int main()
{
  rt_task_conf_t conf;
  int expected;

  printf("Starting test\n");

  if (rt_event_alloc(NULL, NB_FC_TASKS)) return -1;

  rt_cluster_mount(1, 0, 0, NULL);

  rt_task_conf_init(&conf);
  rt_event_t *event = rt_event_get_blocking(NULL);

  rt_task_cluster_init(&cluster, &conf, event);

  expected = 0;
  result = 0;
  nb_tasks_done = 0;

  for (int i=0; i<NB_FC_TASKS; i++)
  {
    rt_task_t *task = &tasks[i];
    //printf("ENQUEUE TASK %p\n", task);
    rt_task_init(task, task_entry);
    int nb_cores = (i & (rt_nb_pe()-1)) + 1;
    expected += ((int)task)*nb_cores + (nb_cores-1)*nb_cores/2;
    rt_task_cores(task, nb_cores);
    rt_task_fc_push(&cluster, task, rt_event_get(NULL, end_of_fc_task, (void *)i));
  }

  while(nb_tasks_done != NB_FC_TASKS)
  {
    rt_event_yield(NULL);
  }

  rt_task_cluster_deinit(&cluster, NULL);

  if (expected != result)
  {
    printf("ERROR, wrong checksum (expected: 0x%x, got: 0x%x)\n", expected, result);
    printf("Test FAILURE\n");
    return -1;
  }

  printf("Test SUCCESS\n");

  return 0;
}
