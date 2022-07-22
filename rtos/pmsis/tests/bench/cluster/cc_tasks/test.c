/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#include "pmsis.h"
#include "stdio.h"


#define NB_CALL 1024
#define NB_TASKS 32

static struct pi_cluster_task task[NB_TASKS];
static struct pi_task events[NB_TASKS];
static int cluster_insn;


static void cluster_perf_start(void *arg)
{
  pi_perf_conf(1<<PI_PERF_INSTR);
  pi_perf_reset();
  pi_perf_start();
}

static void cluster_perf_stop(void *arg)
{
  pi_perf_stop();
  cluster_insn = pi_perf_read(PI_PERF_INSTR);
}

static void cluster_entry(void *arg)
{
}



static void bench_send_task(struct pi_device *cluster_dev)
{
  struct pi_cluster_task task;

  pi_cluster_task(&task, cluster_entry, NULL);

  struct pi_cluster_task perf_task;
  pi_cluster_send_task_to_cl(cluster_dev, pi_cluster_task(&perf_task, cluster_perf_start, NULL));

  pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));
  pi_perf_reset();
  pi_perf_start();


  for (int i=0; i<NB_CALL; i++)
  {
    pi_cluster_send_task_to_cl(cluster_dev, &task);
  }

  pi_perf_stop();

  pi_cluster_send_task_to_cl(cluster_dev, pi_cluster_task(&perf_task, cluster_perf_stop, NULL));

  printf("Cluster task send      , FC instructions      , %d\n", pi_perf_read(PI_PERF_INSTR) / NB_CALL);
  printf("                       , FC cycles            , %d\n", pi_perf_read(PI_PERF_CYCLES) / NB_CALL);
  printf("                       , Cluster instructions , %d\n", cluster_insn / NB_CALL);
}



static void bench_send_task_async(struct pi_device *cluster_dev)
{
  for (int i=0; i<NB_TASKS; i++)
  {
    task[i].entry = cluster_entry;
  }

  struct pi_cluster_task perf_task;
  pi_cluster_send_task_to_cl(cluster_dev, pi_cluster_task(&perf_task, cluster_perf_start, NULL));


  pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));
  pi_perf_reset();
  pi_perf_start();

  for (int i=0; i<NB_CALL; i += NB_TASKS)
  {
    for (int j=0; j<NB_TASKS; j++)
    {
      pi_cluster_send_task_to_cl_async(cluster_dev, &task[j], pi_task_block(&events[j]));
    }
    for (int j=0; j<NB_TASKS; j++)
    {
      pi_task_wait_on(&events[j]);
    }
  }

  pi_perf_stop();

  pi_cluster_send_task_to_cl(cluster_dev, pi_cluster_task(&perf_task, cluster_perf_stop, NULL));

  printf("Async cluster task send, FC instructions      , %d\n", pi_perf_read(PI_PERF_INSTR) / NB_CALL);
  printf("                       , FC cycles            , %d\n", pi_perf_read(PI_PERF_CYCLES) / NB_CALL);
  printf("                       , Cluster instructions , %d\n", cluster_insn / NB_CALL);
}



int test_entry()
{
  printf("Benchmark start\n");

  struct pi_device cluster_dev;
  struct cluster_driver_conf conf;

  pi_cluster_conf_init(&conf);

  conf.id = 0;
    
  pi_open_from_conf(&cluster_dev, &conf);
  pi_cluster_open(&cluster_dev);

  bench_send_task(&cluster_dev);
  bench_send_task_async(&cluster_dev);

  pi_cluster_close(&cluster_dev);

  printf("Benchmark stop\n");

  return 0;
}

void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}