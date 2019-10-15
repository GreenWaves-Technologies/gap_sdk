/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Matrix add example main file. This code actually runs on GAP8 */

#include <stdio.h>
#include "Gap8.h"
#include "MatAddKernels.h"
#include "MatAddKernelsInit.h"
#include "MatAddGenerator.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

extern char *L1_Memory;

#define MAT_SIZE (MAT_W*MAT_H)

L2_MEM int Mat1[MAT_SIZE];
L2_MEM int Mat2[MAT_SIZE];
L2_MEM int MatOut[MAT_SIZE];

int finished = 0;

static void cluster_main()
{
  	printf ("cluster master start\n");

  	MatADD(Mat1, Mat2, MatOut, NULL);
}

static void end_of_app(){
  finished = 1;
  printf ("End of application\n");
}

static void init_test(){
  for(int i=0;i<MAT_SIZE;i++){
    Mat1[i] = 1;
    Mat2[i] = 2;
    MatOut[i] = 0;
  }
}

int main(int argc, char *argv[])

{
  // Load matrices to add
  init_test();

  printf ("Start of application\n");

  // Allocate a scheduler and initialize for some events
  rt_event_sched_t sched;
  rt_event_sched_init(&sched);
  if (rt_event_alloc(&sched, 4)) return -1;

  // Switch on the cluster
  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, L1_MEMORY_SIZE);

  if(L1_Memory == 0 ){
    printf("Memory Allocation Error! Quit...");
    return 1;
  }

  rt_cluster_call(NULL, CID, cluster_main, NULL, NULL, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(&sched, end_of_app, 0));

#ifndef __EMUL__
  while(!finished)
    rt_event_execute(&sched, 1);
#endif

  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  int H=100;
  int W=100;

  for(int i=0;i<H;i++){
    for(int j=0;j<W;j++){
      if (MatOut[(i*W)+j] != 3) {
        printf("Error in test");
        return 1;
      }
    }
  }
  printf("Test success\n");
  return 0;
}
