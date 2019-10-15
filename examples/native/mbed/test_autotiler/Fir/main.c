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

#include "FirKernels.h"

short int * __restrict__ Samples;
GAP_L1_GLOBAL_DATA short int * __restrict__ Coeffs;
short int * __restrict__ Out;

int finished = 0;

#define CORE_NUMBER         8
#define NSAMPLES            49149
#define NCOEFFS             40

static void cluster_main()
{
    printf ("cluster master start\n");

    unsigned int Ti, Ti1, Ti2;
    TiledFir10GenericScalar(Samples, Coeffs, Out, 10, 0);
    TiledFir10Generic(Samples, Coeffs, Out, 10, 0);
    TiledFir10Opt(Samples, Coeffs, Out, 10, 0);
    TiledFir20GenericScalar(Samples, Coeffs, Out, 10, 0);
    TiledFir20Generic(Samples, Coeffs, Out, 10, 0);
    TiledFir20Opt(Samples, Coeffs, Out, 10, 0);
    TiledFir40GenericScalar(Samples, Coeffs, Out, 10, 0);
    TiledFir40Generic(Samples, Coeffs, Out, 10, 0);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Fir test\n");

    // Switch on the cluster
    CLUSTER_Start(0, CORE_NUMBER, 0);

    // Allocate the necessary memory areas in shared L1
    FIR_L1_Memory = L1_Malloc(_FIR_L1_Memory_SIZE);
    Coeffs        = (short int *) L1_Malloc(NCOEFFS*sizeof(short int));
    // Allocate areas in L2 for input and output
    Samples       = (short int *) malloc(NSAMPLES*sizeof(short int));
    Out           = (short int *) malloc(NSAMPLES*sizeof(short int));

    if (!(Samples && Out && FIR_L1_Memory && Coeffs)) {
        printf("Memory allocation failed, L1 Tiler %p, Samples: %p, Coeffs: %p, Out: %p\n", FIR_L1_Memory, Samples, Coeffs, Out);
        return 1;
    }

    CLUSTER_SendTask(0, cluster_main, 0, 0);

    // Close the cluster
    CLUSTER_Stop(0);
    printf("Example completed successfully\n");

    return 0;
}
