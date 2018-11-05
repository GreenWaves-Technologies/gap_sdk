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
#include "FFT2DKernels.h"
#include "FFTBasicKernels.h"
#include "TwiddlesDef.h"
#include "SwapTablesDef.h"

#define DIM 256
#ifdef STACK_SIZE
#undef STACK_SIZE
#endif
#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

L2_MEM unsigned char *ImageIn;
L2_MEM int *Out;
L2_MEM rt_perf_t *cluster_perf;
L2_MEM unsigned char done = 0;

void Process()

{
  printf("FFT Process start\n");
  rt_perf_t *perf = cluster_perf;
  unsigned int instr[2], cycles[2];
  // initialize the performance clock
  rt_perf_init(perf);
  // Configure performance counters for counting the cycles
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  rt_perf_reset(perf);
  rt_perf_start(perf);

  FFT2D_128(ImageIn,
      (int32_t *) Out,
      R2_Twiddles_128,
      R2_SwapTable_128,
      0);

  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[0] = rt_perf_get(perf, RT_PERF_INSTR);
  cycles[0] = rt_perf_get(perf, RT_PERF_CYCLES);

  printf("FFT2D 128, Cycles = %d, Instructions: %d\n", cycles[0], instr[0]);

  rt_perf_reset(perf);
  rt_perf_start(perf);

  FFT2D_256(ImageIn,
      (int32_t *) Out,
      R4_Twiddles_256,
      R4_SwapTable_256,
      0);

  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[1] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0];
  cycles[1] = rt_perf_get(perf, RT_PERF_CYCLES) - cycles[0];

  printf("FFT2D 256, Cycles = %d, Instructions: %d\n", cycles[1], instr[1]);

}

static void end_of_app(){

    printf("End of processing, exit!!\n");
    done = 1;

}

int main()

{
  printf("FC Launched\n");
  if (rt_event_alloc(NULL, 4)) return -1;

  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate the memory of L2 for the performance structure
  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL){
      printf("cluster perf alloc failed\n");
      return -1;
  }

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL){
      printf("cluster stacks alloc failed\n");
      return -1;
  }

  ImageIn = rt_alloc(RT_ALLOC_L2_CL_DATA, DIM*DIM*sizeof(char));
  if(ImageIn == NULL) {
    printf("ImageIn buffer alloc failed\n");
    return -1;
  }

  Out = rt_alloc(RT_ALLOC_L2_CL_DATA, DIM*DIM*sizeof(int));
  if(Out == NULL) {
    printf("Out buffer alloc failed\n");
    return -1;
  }

  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE);
  if(L1_Memory == NULL) {
    printf("L1_Memory buffer alloc failed\n");
    return -1;
  }

  // Execute the function "Process" on the cluster.
  rt_cluster_call(NULL, CID, (void *) Process, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), rt_event_get(NULL, end_of_app, 0));

  while(!done)
      rt_event_execute(NULL, 1);

  // Close the cluster
  rt_cluster_mount(UNMOUNT, CID, 0, NULL);
  printf("Test success\n");

  return 0;

}
