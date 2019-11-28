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


// This is the entry point executed by all processing elements when the cluster controller
// is doing a fork
static void pe_entry(void *arg)
{
  printf("(%ld, %ld) Entering processing element\n", pi_cluster_id(), pi_core_id());

  // Just execute a barrier and leave
  pi_cl_team_barrier();
}



// This is the entry point executed by the cluster controller
static void cluster_entry(void *arg)
{
  printf("(%ld, %ld) Entering cluster controller\n", pi_cluster_id(), pi_core_id());

  // Just fork the execution on all cores
  pi_cl_team_fork(0, pe_entry, NULL);
}



int test_entry()
{
  printf("(%ld, %ld) Entering main controller\n", pi_cluster_id(), pi_core_id());

  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task;
  struct pi_task task;

  // First open the cluster
  pi_cluster_conf_init(&conf);

  pi_open_from_conf(&cluster_dev, &conf);
    
  if (pi_cluster_open(&cluster_dev))
    return -1;

  // Then offload an entry point, this will get executed on the cluster controller
  pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cluster_task, cluster_entry, NULL));

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
