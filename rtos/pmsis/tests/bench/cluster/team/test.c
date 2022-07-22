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


#define NB_FORK 64
#define NB_SYNC 64

static int cluster_insn;
static int master_cluster_insn;
static int master_cluster_cycles;
static int slave_cluster_insn;
static int cluster_nb_cores;
static int master_id;


static void pe_entry_perf_start(void *arg)
{
  pi_perf_conf(1<<PI_PERF_INSTR);
  pi_perf_reset();
  pi_perf_start();
}

static void pe_entry_perf_stop(void *arg)
{
  pi_perf_stop();
  if (pi_core_id() != master_id)
  {
    slave_cluster_insn = pi_perf_read(PI_PERF_INSTR);
  }
}

static void pe_entry_empty(void *arg)
{
}

static void pe_entry_barrier(void *arg)
{
  for (int i=0; i<NB_SYNC; i++)
    pi_cl_team_barrier();

  cluster_insn = pi_perf_read(PI_PERF_INSTR) / NB_SYNC;
}

static void pe_entry_critical(void *arg)
{
  for (int i=0; i<NB_SYNC; i++)
  {
    pi_cl_team_critical_enter();
    pi_cl_team_critical_exit();
  }

  cluster_insn = pi_perf_read(PI_PERF_INSTR) / NB_SYNC;
}



static void cluster_entry_bench(void *arg)
{
  int nb_cores = cluster_nb_cores;

  pi_perf_conf((1<<PI_PERF_INSTR) | (1<<PI_PERF_CYCLES));
  pi_perf_reset();
  pi_cl_team_fork(0, pe_entry_perf_start, NULL);
  pi_perf_start();


  for (int i=0; i<NB_FORK; i++)
  {
    pi_cl_team_fork(nb_cores, arg, NULL);
  }

  pi_perf_stop();
  master_id = pi_core_id();
  pi_cl_team_fork(0, pe_entry_perf_stop, NULL);

  master_cluster_insn = pi_perf_read(PI_PERF_INSTR) / NB_FORK;
  master_cluster_cycles = pi_perf_read(PI_PERF_CYCLES) / NB_FORK;
  slave_cluster_insn = slave_cluster_insn / NB_FORK;
}



static void cluster_entry_bench_single(void *arg)
{
  int nb_cores = cluster_nb_cores;

  pi_perf_conf((1<<PI_PERF_INSTR) | (1<<PI_PERF_CYCLES));
  pi_perf_reset();
  pi_cl_team_fork(0, pe_entry_perf_start, NULL);
  pi_perf_start();

  pi_cl_team_fork(nb_cores, arg, NULL);

  pi_perf_stop();
  master_id = pi_core_id();
  pi_cl_team_fork(0, pe_entry_perf_stop, NULL);

  master_cluster_insn = pi_perf_read(PI_PERF_INSTR) / NB_SYNC;
  master_cluster_cycles = pi_perf_read(PI_PERF_CYCLES) / NB_SYNC;
  slave_cluster_insn = slave_cluster_insn / NB_SYNC;
}



static void do_bench(struct pi_device *cluster_dev, char *name, int nb_cores, char *op, void (*entry)(void *arg), void (*pe_entry)(void *arg))
{
  struct pi_cluster_task task;

  cluster_nb_cores = nb_cores;

  pi_cluster_send_task_to_cl(cluster_dev, pi_cluster_task(&task, entry, pe_entry));

  if (strcmp(op, "fork") == 0)
  {
    printf("%s, cluster master instructions      , %d\n", name, master_cluster_insn);
    printf("                               , cluster master cycles            , %d\n", master_cluster_cycles);
    printf("                               , cluster slave instructions       , %d\n", slave_cluster_insn);

    printf("@BENCH@%s.%d_master_insn=%d@DESC@Cluster %s instructions on master (nb_cores: %d)@\n", op, nb_cores, master_cluster_insn, op, nb_cores);
    printf("@BENCH@%s.%d_master_cycles=%d@DESC@Cluster %s cycles on master (nb_cores: %d)@\n", op, nb_cores, master_cluster_cycles, op, nb_cores);
    printf("@BENCH@%s.%d_slave_insn=%d@DESC@Cluster %s instructions on slave (nb_cores: %d)@\n", op, nb_cores, slave_cluster_insn, op, nb_cores);
  }
  else
  {
    printf("%s, cluster instructions      , %d\n", name, cluster_insn);
    printf("                               , cluster cycles            , %d\n", master_cluster_cycles);

    printf("@BENCH@%s.%d_insn=%d@DESC@Cluster %s instructions (nb_cores: %d)@\n", op, nb_cores, cluster_insn, op, nb_cores);
    printf("@BENCH@%s.%d_cycles=%d@DESC@Cluster %s cycles (nb_cores: %d)@\n", op, nb_cores, master_cluster_cycles, op, nb_cores);
  }
}


static void bench_sync(struct pi_device *cluster_dev)
{
  // do_bench(cluster_dev, "Cluster fork (nb_cores: 4)     ", 4, "fork", cluster_entry_bench, pe_entry_empty);
  // do_bench(cluster_dev, "Cluster fork (nb_cores: 8)     ", 0, "fork", cluster_entry_bench, pe_entry_empty);
  do_bench(cluster_dev, "Cluster barrier                ", 0, "barrier", cluster_entry_bench_single, pe_entry_barrier);
  // do_bench(cluster_dev, "Cluster critical (nb_cores: 1) ", 1, "critical", cluster_entry_bench_single, pe_entry_critical);
  // do_bench(cluster_dev, "Cluster critical (nb_cores: 2) ", 2, "critical", cluster_entry_bench_single, pe_entry_critical);
  // do_bench(cluster_dev, "Cluster critical (nb_cores: 4) ", 4, "critical", cluster_entry_bench_single, pe_entry_critical);
  // do_bench(cluster_dev, "Cluster critical (nb_cores: 8) ", 0, "critical", cluster_entry_bench_single, pe_entry_critical);
}



int test_entry()
{
  printf("Benchmark start\n");

  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;

  pi_cluster_conf_init(&conf);

  conf.id = 0;
    
  pi_open_from_conf(&cluster_dev, &conf);
  pi_cluster_open(&cluster_dev);

  bench_sync(&cluster_dev);

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