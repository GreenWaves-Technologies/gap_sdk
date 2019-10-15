/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "rt/rt_api.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

unsigned int done = 0;
#ifdef PRINTF_UART
unsigned int __rt_iodev=RT_IODEV_UART;
#endif

static void hello(void *arg)
{
  printf("[clusterID: 0x%2x] Hello from core %d\n", rt_cluster_id(), rt_core_id());
}

static void cluster_entry(void *arg)
{
  printf("Entering cluster on core %d\n", rt_core_id());
  printf("There are %d cores available here.\n", rt_nb_pe());
  rt_team_fork(8, hello, (void *)0x0);
  printf("Leaving cluster on core %d\n", rt_core_id());
}

static void end_of_call(void *arg)
{
  printf("[clusterID: 0x%x] Hello from core %d\n", rt_cluster_id(), rt_core_id());
  done = 1;
}

int main()
{
  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 4)) return -1;

  rt_event_t *p_event = rt_event_get(NULL, end_of_call, (void *) CID);

  rt_cluster_mount(MOUNT, CID, 0, NULL);

  rt_cluster_call(NULL, CID, cluster_entry, NULL, NULL, 0, 0, rt_nb_pe(), p_event);

  while(!done)
      rt_event_execute(NULL, 1);

  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  printf("Test success: Leaving main controller\n");
  return 0;
}
