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

#define NB_TASKS 32

#if TEST_DURATION >= 10
#define NB_CALL 1024
#else
#define NB_CALL 128
#endif

static int nb_call;
static struct pi_cluster_task task[NB_TASKS];
static struct pi_task events[NB_TASKS];

static void cluster_entry(void *arg)
{
  nb_call++;
}

static int test_task_sync()
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task task;
  int errors = 0;

  nb_call = 0;

  pi_cluster_conf_init(&conf);
  conf.id = 0;

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&task, cluster_entry, NULL);

  for (int i=0; i<NB_CALL; i++)
  {
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
  }

  pi_cluster_close(&cluster_dev);

  if (nb_call != NB_CALL)
    errors++;

  return errors;
}

static int test_task_async()
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  int errors = 0;

  nb_call = 0;

  pi_cluster_conf_init(&conf);
  conf.id = 0;   

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  for (int i=0; i<NB_TASKS; i++)
  {
    pi_cluster_task(&task[i], cluster_entry, (void *)&errors);
  }

  for (int i=0; i<NB_CALL; i += NB_TASKS)
  {
    for (int j=0; j<NB_TASKS; j++)
    {
      pi_cluster_send_task_to_cl_async(&cluster_dev, &task[j], pi_task_block(&events[j]));
    }
    for (int j=0; j<NB_TASKS; j++)
    {
      pi_task_wait_on(&events[j]);
    }
  }

  if (nb_call != NB_CALL)
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