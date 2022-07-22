/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "stdio.h"

#include "pmsis/cluster/cluster_team/cl_team.h"

#define STACK_SIZE 2048

static int nb_fork;
static int nb_callback_exec;

static void pe_entry(void *arg)
{
  int *var = (int *)arg;
  pi_cl_team_critical_enter();
  *var |= 1 << (pi_core_id());
  pi_cl_team_critical_exit();
}

static int check_fork(int nb_cores)
{
  int var = 0;
  nb_fork++;
  pi_cl_team_fork(nb_cores, pe_entry, (void *)&var);
  return (1<<pi_cl_team_nb_cores()) - 1 != var;
}

static int check_preset_fork(int nb_cores)
{
  int var;
  int errors = 0;
  nb_fork++;
  pi_cl_team_prepare_fork(nb_cores);
  var = 0;
  pi_cl_team_preset_fork(pe_entry, (void *)&var);
  errors += (1<<pi_cl_team_nb_cores()) - 1 != var;
  var = 0;
  pi_cl_team_preset_fork(pe_entry, (void *)&var);
  errors += (1<<pi_cl_team_nb_cores()) - 1 != var;
  var = 0;
  pi_cl_team_preset_fork(pe_entry, (void *)&var);
  errors += (1<<pi_cl_team_nb_cores()) - 1 != var;

  return errors;
}

static void cluster_entry(void *arg)
{
  int *errors = (int *)arg;

  if (pi_cl_cluster_nb_pe_cores() != NB_PE)
    *errors += 1;

  for (unsigned int i=0; i<=pi_cl_cluster_nb_pe_cores(); i++)
  {
    *errors += check_fork(i);
    if (i != 0)
      *errors += check_fork(0);
  }

  for (unsigned int i=1; i<=pi_cl_cluster_nb_pe_cores(); i++)
  {
    *errors += check_preset_fork(i);
  }
}

static int test_task_sync()
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task task;
  int errors = 0;

  nb_fork = 0;

  pi_cluster_conf_init(&conf);
  conf.id = 0;

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&task, &cluster_entry, (void *)&errors);
  pi_cluster_send_task_to_cl(&cluster_dev, &task);
  pi_cluster_close(&cluster_dev);

  if (nb_fork == 0)
    errors++;

  return errors;
}

static void cluster_task_callback(void *arg)
{
  nb_callback_exec++;
}

static int test_task_async()
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task;
  struct pi_task task;
  int errors = 0;

  nb_fork = 0;
  nb_callback_exec = 0;

  pi_cluster_conf_init(&conf);
  conf.id = 0;   

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&cluster_task, cluster_entry, (void *)&errors);

  pi_task_callback(&task, cluster_task_callback, (void *)&task);

  pi_cluster_send_task_to_cl_async(&cluster_dev, &cluster_task, &task);

  while (nb_callback_exec == 0)
    pi_yield();

  if (nb_fork == 0)
    errors++;

  if (nb_callback_exec != 1)
    errors++;

  pi_cluster_close(&cluster_dev);

  return errors;
  
}

int test_entry()
{
  int errors = 0;

  printf("Starting test\n");

  errors += test_task_sync();

  errors += test_task_async();

  if (errors)
    printf("Test failure\n");
  else
    printf("Test success\n");

  return errors;
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
