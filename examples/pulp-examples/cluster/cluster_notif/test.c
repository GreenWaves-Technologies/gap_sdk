/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "rt/rt_api.h"
#include "stdio.h"

static rt_notif_t notif;

static volatile int finished = 0;

static void pe_entry(void *arg)
{
  int event = rt_cluster_notif_event(&notif);

  // wait for the notification from FC to continue
  rt_cluster_notif_wait(event);
  printf("(%d, %d) Received event\n", rt_cluster_id(), rt_core_id());
  rt_team_barrier();
}

static void cluster_entry(void *arg)
{
  rt_team_fork(0, pe_entry, (void *)NULL);
}

static void delay()
{
  volatile int i;
  for (i=0; i<1000; i++);
}

int main()
{
  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 1)) return -1;

  rt_cluster_mount(1, 0, 0, NULL);

  rt_cluster_notif_init(&notif, 0);

  rt_event_t *event = rt_event_get_blocking(NULL);

  rt_cluster_call(NULL, 0, cluster_entry, NULL, NULL, 0, 0, rt_nb_pe(), event);

  rt_cluster_notif_trigger(&notif, 0x03);

  // trigger the notif event for all the cores.
  rt_cluster_notif_trigger(&notif, RT_TRIGGER_ALL_CORE);

  rt_event_wait(event);

  rt_cluster_mount(0, 0, 0, NULL);

  printf("Test success\n");

  return 0;
}
