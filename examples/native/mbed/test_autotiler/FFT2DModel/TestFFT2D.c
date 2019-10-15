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

#include "FFT2DKernels.h"
#include "SwapTablesDef.h"
#include "TwiddlesDef.h"

#define CORE_NUMBER         8
#define DIM                256
#define L1_BUFFER_SIZE     _L1_Memory_SIZE  // 40K

GAP_L2_DATA uint8_t *ImageIn;
GAP_L2_DATA int32_t *Out;
GAP_L2_DATA performance_t cluster_perf;

void Process()

{
    performance_t perf;
    unsigned int instr[2], cycles[2];

    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
    FFT2D_128(ImageIn,
              Out,
              R2_Twiddles_128,
              R2_SwapTable_128,
              0);
    PERFORMANCE_Stop(&perf);
    instr[0] =  PERFORMANCE_Get(&perf, PCER_INSTR_Pos);
    cycles[0] = PERFORMANCE_Get(&perf, PCER_CYCLE_Pos);
    printf("FFT2D 128, Cycles = %d, Instructions: %d\n", cycles[0], instr[0]);

    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
    FFT2D_256(ImageIn,
              Out,
              R4_Twiddles_256,
              R4_SwapTable_256,
              0);
    PERFORMANCE_Stop(&perf);
    instr[1] =  PERFORMANCE_Get(&perf, PCER_INSTR_Pos);
    cycles[1] = PERFORMANCE_Get(&perf, PCER_CYCLE_Pos);
    printf("FFT2D 256, Cycles = %d, Instructions: %d\n", cycles[1], instr[1]);

}

int main()
{
    printf("Fabric controller code execution for mbed_os matrix addition test\n");

    ImageIn = malloc(DIM * DIM * sizeof(char));
    Out = malloc(DIM * DIM * sizeof(int));
    if (!ImageIn || !Out) printf ("malloc failed\n");

    /* Cluster Start - power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* Allocate a buffer in the shared L1 memory */
    L1_Memory = L1_Malloc(L1_BUFFER_SIZE);
    if (!L1_Memory) printf ("L1 Memory alloc failed\n");

    /* Allocation check */
    if(L1_Memory == NULL ){
        printf("Memory Allocation Error! Quit...\n");
        return 1;
    }

    /* FC send a task to cluster */
    CLUSTER_SendTask(0, Process, 0, 0);

    /* Cluster Stop - power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
