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

static int alloc_size = 256;

static void hello(void *arg)
{
  printf("[clusterID: 0x%2x] Hello from core %d\n", rt_cluster_id(), rt_core_id());
}


static void free_mem(void* p,int size,rt_free_req_t* req)
{
  rt_free_cluster(RT_ALLOC_L2_CL_DATA, p, alloc_size, req);
  rt_free_cluster_wait(req);
}



static void cluster_entry(void *arg)
{
  rt_alloc_req_t req0;
  rt_free_req_t  req1;
  printf("Entering cluster on core %d\n", rt_core_id());
  printf("There are %d cores available here.\n", rt_nb_pe());
  char * a,*b;
  

  rt_alloc_cluster(RT_ALLOC_L2_CL_DATA, alloc_size, &req0);
  a = (char *) rt_alloc_cluster_wait(&req0);
  rt_alloc_cluster(RT_ALLOC_L2_CL_DATA, alloc_size, &req0);
  b = (char *) rt_alloc_cluster_wait(&req0);
  
  if((a)==NULL){ printf("L2 Allocation Error...\n"); return; }
  if((b)==NULL){ printf("L2 Allocation Error...\n"); return; }

  rt_team_fork(8, hello, (void *)0x0);

  free_mem(a,alloc_size,&req1);
  free_mem(b,alloc_size,&req1);
  
  printf("Leaving cluster on core %d\n", rt_core_id());

}

int main()
{
  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 8)) return -1;

  rt_cluster_mount(MOUNT, CID, 0, NULL);

  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  for(int i=0;i<10;i++) {
    rt_cluster_call(NULL, CID, cluster_entry, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);
    alloc_size = alloc_size*2;
  }
  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  printf("Test success: Leaving main controller\n");
  return 0;
}
