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
#ifdef STACK_SIZE
#undef STACK_SIZE
#endif
#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

PI_L2 unsigned char *ImageIn;
PI_L2 int *Out;
PI_L2 unsigned char done = 0;

void Process()

{
    printf("FFT Process start on Cluster\n");
    // initialize the performance clock
    pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
    // Configure performance counters for counting the cycles
    pi_perf_reset();
    pi_perf_start();

    FFT2D_128(ImageIn,
      (int32_t *) Out,
      R2_Twiddles_128,
      R2_SwapTable_128);

    pi_perf_stop();
  
    printf("FFT2D 128, Cycles = %d\n", pi_perf_read(PI_PERF_ACTIVE_CYCLES));

    pi_perf_reset();
    

    FFT2D_256(ImageIn,
      (int32_t *) Out,
      R4_Twiddles_256,
      R4_SwapTable_256);

    pi_perf_stop();

    printf("FFT2D 256, Cycles = %d\n", pi_perf_read(PI_PERF_ACTIVE_CYCLES));

}

static void end_of_app(){

    printf("End of processing, exit!!\n");
    done = 1;

}

void fft_2d(void)

{
    printf("FC Launched\n");

    // Configure and open cluster

    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    ImageIn = pi_l2_malloc(DIM*DIM*sizeof(char));
    if(ImageIn == NULL) {
        printf("ImageIn buffer alloc failed\n");
        pmsis_exit(-1);
    }

    Out = pi_l2_malloc(DIM*DIM*sizeof(int));
    if(Out == NULL) {
        printf("Out buffer alloc failed\n");
        pmsis_exit(-1);
    }

    L1_Memory = pi_l1_malloc(0,_L1_Memory_SIZE);
    if(L1_Memory == NULL) {
        printf("L1_Memory buffer alloc failed\n");
        pmsis_exit(-1);
    }

    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = (void *)Process;
    task->arg = (void *) NULL;
    task->stack_size = (uint32_t) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

    // Close the cluster
    pi_cluster_close(&cluster_dev);
    printf("Test success\n");

    pmsis_exit(0);

}



int main()
{
    printf("\n\n\t *** PMSIS FFT 2D Autotiler Test ***\n\n");
    return pmsis_kickoff((void *) fft_2d);
}
