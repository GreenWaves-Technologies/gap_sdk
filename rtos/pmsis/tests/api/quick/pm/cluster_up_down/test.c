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

#define BUFF_SIZE 64
#define NB_ITER_RTL     10
#define NB_ITER_OTHER   200

static int finished;
struct pi_device cluster_dev;

static int get_nb_iter()
{
#ifdef __PLATFORM_RTL__
    return NB_ITER_RTL;
#else
    return NB_ITER_OTHER;
#endif
}

static void pe_entry(void *arg)
{
  int *var = (int *)arg;
  pi_cl_team_critical_enter();
  *var |= 1 << (pi_core_id());
  pi_cl_team_critical_exit();
}

static int check_fork(int nb_cores)
{
  unsigned int var = 0;
  pi_cl_team_fork(nb_cores, pe_entry, (void *)&var);
  return ((unsigned int )(1<<pi_cl_team_nb_cores()) - 1) != var;
}

static void cluster_entry(void *arg)
{
  int *errors = (int *)arg;
  for (int i=0; i<=pi_cl_cluster_nb_pe_cores(); i++)
  {
    *errors += check_fork(i);
    if (i != 0)
      *errors += check_fork(0);
  }
}

static int check_cluster_accesses(int cid)
{
  volatile unsigned char *buffer = pi_l1_malloc(&cluster_dev, BUFF_SIZE);

  for (int i=0; i<BUFF_SIZE; i++)
  {
    buffer[i] = i;
  }

  for (int i=0; i<BUFF_SIZE; i++)
  {
    if (buffer[i] != (i & 0xff))
    {
      //printf("Error while accessing L1 buffer\n");
      return -1;
    }
  }

  pi_l1_free(&cluster_dev, (void *)buffer, BUFF_SIZE);

  return 0;
}

static int check_cluster_up(int cid)
{
  struct pi_cluster_conf conf;
  struct pi_cluster_task task;

  int errors = 0;

  pi_cluster_conf_init(&conf);
  conf.id = 0;

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  // First check L1 accesses
  if (check_cluster_accesses(cid))
    return -1;

  // Then code execution
  pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&task, cluster_entry, (void *)&errors));

  // And again L1 accesses
  if (check_cluster_accesses(cid))
    return -1;

  pi_cluster_close(&cluster_dev);

  return errors;
}

static void end_of_call(void *arg)
{
  finished--;
}

int test_entry()
{
  printf("Starting test\n");

  for (int i=0; i<get_nb_iter(); i++)
  {
    if (check_cluster_up(0))
      goto failed;
  }

  printf("Test success\n");

  return 0;

failed:
  //printf("Test failure\n");

  return -1;
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
