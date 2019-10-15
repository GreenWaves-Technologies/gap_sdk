/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_autotiler.h"
#include <stdlib.h>

#include "MatMultKernels.h"

#define CORE_NUMBER         8

L2_MEM short int *M1;
L2_MEM short int *M2;
L2_MEM short int *Out;

static void cluster_main()
{
    printf ("cluster master start\n");
    unsigned int ElapsedTime[2];
    performance_t perf;

    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk);

    ParMatMult(M1, M2, Out, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[0] = PERFORMANCE_Get(&perf, PCER_CYCLE_Pos);
    printf("ParMatMult: %d cycles\n", ElapsedTime[0]);

    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk);
    ParVectMatMult(M1, M2, Out, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[1] = PERFORMANCE_Get(&perf, PCER_CYCLE_Pos);
    printf("ParVectMatMult: %d cycles\n", ElapsedTime[1]);

}

int main()

{

    printf ("Matrix Mult start\n");
    int W_M1 = 70;
    int H_M1 = 200;
    int W_M2 = 150;
    int H_M2 = 70;
    int W_Out = 200;
    int H_Out = 150;

    M1  = (short int *) malloc(W_M1*H_M1*sizeof(short int));
    M2  = (short int *) malloc(W_M2*H_M2*sizeof(short int));
    Out = (short int *) malloc(W_Out*H_Out*sizeof(short int));
    if ((M1==0)||(M2==0)||(Out==0)) {
        printf("Failed to allocate mem.\n"); return 0;
    }

    memset (M1, 0x1, W_M1*H_M1*sizeof(short int));
    memset (M2, 0x2, W_M2*H_M2*sizeof(short int));

    // Switch on the cluster
    CLUSTER_Start(0, CORE_NUMBER, 0);

    // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
    L1_Memory = L1_Malloc(_L1_Memory_SIZE);
    if(L1_Memory == 0 ){
        printf("Memory Allocation Error! Quit...");
        return 1;
    }

    CLUSTER_SendTask(0, cluster_main, 0, 0);

    CLUSTER_Stop(0);

    return 0;
}

