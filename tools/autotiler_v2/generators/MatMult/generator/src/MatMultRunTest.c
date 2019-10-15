/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap8.h"
#include "MatMultKernels.h"

#define MOUNT           1
#define UNMOUNT         0
#define CID             0
#define STACK_SIZE      2048

L2_MEM short int *M1;
L2_MEM short int *M2;
L2_MEM short int *Out1;
L2_MEM short int *Out2;
#ifndef __EMUL__
L2_MEM rt_perf_t *cluster_perf;
#endif
int finished = 0;

int num_op=0;

static void cluster_main()
{
    printf ("cluster master start\n");
#ifndef __EMUL__
    unsigned int ElapsedTime[2];
    unsigned int StartTime;
    rt_perf_t *perf = cluster_perf;
    // initialize the performance clock
    rt_perf_init(perf);
    // Configure performance counters for counting the cycles
    rt_perf_conf(perf, (1<<RT_PERF_CYCLES));
    rt_perf_reset(perf);
    rt_perf_start(perf);

    StartTime = rt_perf_read(RT_PERF_CYCLES);
#endif
    ParMatMult(M1, M2, Out1, 0);
#ifndef __EMUL__
    ElapsedTime[0] = rt_perf_read(RT_PERF_CYCLES)-StartTime;

    StartTime = rt_perf_read(RT_PERF_CYCLES);
#endif
    ParVectMatMult(M1, M2, Out2, 0);
#ifndef __EMUL__
    ElapsedTime[1] = rt_perf_read(RT_PERF_CYCLES)-StartTime;

    printf("ParMatMult: %d cycles %d ops/cycle\n", ElapsedTime[0], (num_op/ElapsedTime[0]));
    printf("ParVectMatMult: %d cycles, %d ops/cycle\n", ElapsedTime[1], (num_op/ElapsedTime[1]));

    rt_perf_stop(perf);
    rt_perf_reset(perf);
#endif
}

static void end_of_app(){
  finished = 1;
  printf ("End of application\n");
}

int main()
{

  printf ("Matrix Mult start\n");
  int H_M1 = 200;
  int W_M1 = 70;
  int H_M2 = 70;
  int W_M2 = 150;

  num_op = H_M1 * W_M2 * ( 70 + 70 -1);

  printf("==================================\n");
  printf("Matrix 1: %d x %d\n",W_M1,H_M1);
  printf("Matrix 2: %d x %d\n",W_M2,H_M2);
  printf("Matrix Multiplication number of operations: %d\n",num_op);
  printf("==================================\n");
  int W_Out = H_M1;
  int H_Out = W_M2;

  M1  = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_M1*H_M1*sizeof(short int));
  M2  = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_M2*H_M2*sizeof(short int));
  Out1 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_Out*H_Out*sizeof(short int));
  Out2 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, W_Out*H_Out*sizeof(short int));
  if ((M1==0)||(M2==0)||(Out1==0) ||(Out2==0)) {
    printf("Failed to allocate mem.\n"); return 0;
  }

  for(int i=0;i< W_M1*H_M1;i++) M1[i]=i;
  for(int i=0;i< W_M2*H_M2;i++) M2[i]=i+1;

  // Allocate some events
  if (rt_event_alloc(NULL, 8)){printf("Error in events allocation...\n"); return -1;}

  // Switch on the cluster
  rt_cluster_mount(MOUNT, CID, 0, NULL);

#ifndef __EMUL__
  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) return -1;
#endif

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE);

  if(L1_Memory == NULL ){
    printf("Memory Allocation Error! Quit...\n");
    return 1;
  }

  rt_cluster_call(NULL, CID, cluster_main, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(NULL, end_of_app, 0));

#ifndef __EMUL__
  while(!finished)
    rt_event_execute(NULL, 1);
#endif

  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  for(int i=0;i<H_Out;i++){
    for(int j=0;j<W_Out;j++){
      if(Out1[i*W_Out+j] != Out2[i*W_Out+j]){
        printf("Error, result of different methods does not correspond!\n");
        return -1;
      }
    }
  }

  printf("Test success\n");

  return 0;
}

