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

#define MAX_NB_FC_TASKS 4
#define MAX_NB_CL_TASKS 64

static int nb_failures;
volatile static RT_L1_DATA int result[MAX_NB_FC_TASKS];
static RT_L1_DATA rt_task_t tasks[MAX_NB_FC_TASKS][MAX_NB_CL_TASKS];
static rt_task_t fc_tasks[MAX_NB_FC_TASKS];
static rt_event_t *events[MAX_NB_FC_TASKS];
static rt_task_cluster_t cluster;

static void cl_task_entry(rt_task_t *task, int id)
{
  int task_id = task->args[0];
  rt_team_critical_enter();
  result[task_id] += (int)task + id;
  rt_team_critical_exit();

  volatile int i;
  for (i=0; i<id*10; i++);
}

static void fc_task_entry(rt_task_t *task, int id)
{
  int expected = 0;
  int task_id = task->args[0];
  int nb_tasks = task->args[1];
  result[task_id] = 0;

  for (int i=0; i<nb_tasks; i++)
  {
    rt_task_t *task = &tasks[task_id][i];
    task->args[0] = task_id;
    rt_task_init(task, cl_task_entry);
    int nb_cores = (i & (rt_nb_pe()-1)) + 1;
    expected += ((int)task)*nb_cores + (nb_cores-1)*nb_cores/2;
    rt_task_cores(task, nb_cores);
    rt_task_cl_push(task);
  }

  for (int i=0; i<nb_tasks; i++)
  {
    rt_task_t *task = &tasks[task_id][i];
    rt_task_cl_wait(task);
  }

  if (expected != result[task_id])
  {
    printf("ERROR, wrong checksum (expected: 0x%x, got: 0x%x)\n", expected, result);
    rt_team_critical_enter();
    nb_failures++;
    rt_team_critical_exit();
  }
}

static void check_tasks(int nb_fc_task, int nb_cl_task)
{
  for (int i=0; i<nb_fc_task; i++)
  {
    rt_task_t *task = &fc_tasks[i];
    task->args[0] = i;
    task->args[1] = nb_cl_task;
    rt_task_init(task, fc_task_entry);
    events[i] = rt_event_get_blocking(NULL);
    rt_task_fc_push(&cluster, task, events[i]);
  }

  for (int i=0; i<nb_fc_task; i++)
  {
    rt_event_wait(events[i]);
  }
}

int main()
{
  rt_task_conf_t conf;

  if (rt_event_alloc(NULL, MAX_NB_FC_TASKS)) return -1;

  printf("Starting test\n");

  rt_cluster_mount(1, 0, 0, NULL);

  nb_failures = 0;

  rt_task_conf_init(&conf);

  if (rt_task_cluster_init(&cluster, &conf, NULL))
    return -1;

  check_tasks(1, 64);
  if (rt_nb_pe() > 2)
    check_tasks(2, 64);
  if (rt_nb_pe() > 4)
    check_tasks(4, 64);

  if (nb_failures)
  {
    printf("Test FAILURE\n");
    return -1;
  }

  printf("Test SUCCESS\n");

  return 0;
}
