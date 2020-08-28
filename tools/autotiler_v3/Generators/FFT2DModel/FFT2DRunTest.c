/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap.h"
#include "FFT2DKernels.h"
#include "FFTBasicKernels.h"
#include "TwiddlesDef.h"
#include "SwapTablesDef.h"

#define DIM 256

L2_MEM unsigned char ImageIn[DIM*DIM];
L2_MEM int Out[DIM*DIM];
int finished = 0;

void cluster_main()
{
  FFT_InstallArg_T ArgIns;

  ArgIns.L1_Twiddles = (short int *) rt_alloc(RT_ALLOC_CL_DATA, Max(128*sizeof(short int), 3*256*(sizeof(short)/2)));
  ArgIns.L1_SwapLUT = (short int *) rt_alloc(RT_ALLOC_CL_DATA, Max(128*sizeof(short int), 256*sizeof(short int)));

  if (ArgIns.L1_Twiddles == 0 || ArgIns.L1_SwapLUT == 0) {
    printf("Failed to allocated Twiddles or Swap LUT\n"); exit(0);
  }
  ArgIns.Nfft = 128; ArgIns.Radix = 2; ArgIns.Twiddles = R2_Twiddles_128; ArgIns.SwapLUT = R2_SwapTable_128;
  gap_cl_resethwtimer();
  FFT_InstallTwiddlesAndSwapLUT(&ArgIns);
  FFT2D_128(ImageIn,
    Out,
    ArgIns.L1_Twiddles, // R2_Twiddles_128,
    ArgIns.L1_SwapLUT // R2_SwapTable_128,
    );

  printf("FFT2D Radix2 128, Cycles = %d\n", gap_cl_readhwtimer());

  ArgIns.Nfft = 256; ArgIns.Radix = 4; ArgIns.Twiddles = R4_Twiddles_256; ArgIns.SwapLUT = R4_SwapTable_256;
  gap_cl_resethwtimer();
  FFT_InstallTwiddlesAndSwapLUT(&ArgIns);
  FFT2D_256(ImageIn,
    Out,
    ArgIns.L1_Twiddles, // R4_Twiddles_256,
    ArgIns.L1_SwapLUT // R4_SwapTable_256,
    );

  printf("FFT2D Radix4 256, Cycles = %d\n", gap_cl_readhwtimer());
}

static void end_of_app(){
  finished = 1;
  printf ("End of application\n");
}

int main()
{
  // Allocate a scheduler and initialize for some events
  rt_event_sched_t sched;
  rt_event_sched_init(&sched);
  if (rt_event_alloc(&sched, 4)) return -1;

  // Switch on the cluster
  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
  L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE);
  if(L1_Memory == 0 ){
    printf("Memory Allocation Error! Quit...");
    return 1;
  }

  rt_cluster_call(NULL, CID, cluster_main, NULL, NULL, 0, 0, rt_nb_pe(), rt_event_get(&sched, end_of_app, 0));

#ifndef __EMUL__
  while(!finished)
    rt_event_execute(&sched, 1);
#endif

  rt_cluster_mount(UNMOUNT, CID, 0, NULL);
  printf("Test success\n");

  return 0;
}
