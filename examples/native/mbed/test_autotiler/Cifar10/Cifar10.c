/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdlib.h>
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_autotiler.h"

#include "Cifar10Kernels.h"
#include "CifarCoeff.h"
#include "CifarData.h"

#define CORE_NUMBER     8
#define CID             0

GAP_L2_DATA short int *Out_Layer0;
GAP_L2_DATA short int *Out_Layer1;
GAP_L2_DATA short int *Out_Layer2;

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)

{
    int i, j;
    int Acc = 0;
    int K = 5;

    for (i=0; i<K; i++) {
        for (j=0; j<K; j++) {
            Acc += In[(X+i)*W+Y+j]*Filter[K*i+j];
        }
    }
    return (gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
}


void DumpPlane(char *Mess, short int *Plane, unsigned int W, unsigned int H)

{
    unsigned int i, j;

    printf("----------------- %s ------------------------\n", Mess);
    for (i=0; i<H; i++) {
        printf("%2d: ", i);
        for (j=0; j<W; j++) {
            printf("%4x ", (unsigned short) Plane[i*W+j]);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");
}

void DumpPaddedCoeff(char *Name, short int *C, unsigned int NTap, unsigned int NFilter)

{
    unsigned int i, j;
    printf("GAP_L2_DATA short int %s[] = {\n", Name);
    for (i=0; i<NFilter; i++) {
        for (j=0; j<NTap; j++) {
            printf("%d, ", C[i*NTap+j]);
        }
        printf("0,\n");
    }
    printf("};\n");
}

unsigned int CheckSum(short int *In, int Size)

{
    int i;
    unsigned S=0;

    for (i=0; i<Size; i++) S += In[i];
    return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)

{
    int i;

    printf("Check sum for %s\n", Mess);

    for (i=0; i<NPlane; i++) {
        printf("\t%2d: %x\n", i, CheckSum(Planes + i*(W*H), W*H));
    }
}


static void RunCifar10(void)

{
    unsigned int ElapsedTime[3] = {0,0,0};
    int CheckResults = 1;
    performance_t perf;

#if RT_HAS_HWCE
    /* Make HWCE event active */
    EU_EVT_MaskSet(1 << EU_HWCE_EVENT);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5MaxPool2x2_HWCE_0(ImageIn, Filter_Layer0_HWCE, Out_Layer0, 14, Bias_Layer0, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[0] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("HWCE Layer0", Out_Layer0, 8, 14, 14);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5MaxPool2x2_HWCE_1(Out_Layer0, Filter_Layer1_HWCE, Out_Layer1, 14, Bias_Layer1, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[1] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("HWCE Layer1", Out_Layer1, 12, 5, 5);
#else

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5MaxPool2x2_SW_0(ImageIn, Filter_Layer0, Bias_Layer0, Out_Layer0, 14, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[0] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer0", Out_Layer0, 8, 14, 14);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5MaxPool2x2_SW_1(Out_Layer0, Filter_Layer1, Bias_Layer1, Out_Layer1, 14, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[1] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer1", Out_Layer1, 12, 5, 5);
#endif

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    LinearLayerReLU_1(Out_Layer1, Filter_Layer2, Bias_Layer2, Out_Layer2, 16, 10, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[2] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer2", Out_Layer1, 10, 1, 1);

    printf("Layer0: %7d Cycles\n", ElapsedTime[0]);
    printf("Layer1: %7d Cycles\n", ElapsedTime[1]);
    printf("Layer2: %7d Cycles\n", ElapsedTime[2]);
}

int main()
{

    printf("FC Launched\n");

    Out_Layer0 = (short int *) malloc(14*14*sizeof(short int)*8);
    printf("Allocating %d: %s\n", 14*14*sizeof(short int)*8, Out_Layer0?"Ok":"Failed");
    Out_Layer1 = (short int *) malloc(5*5*sizeof(short int)*12);
    printf("Allocating %d: %s\n", 5*5*sizeof(short int)*12, Out_Layer1?"Ok":"Failed");
    Out_Layer2 = (short int *) malloc(10*sizeof(short int));
    printf("Allocating %d: %s\n", 10*sizeof(short int), Out_Layer2?"Ok":"Failed");
    if (!(Out_Layer0 && Out_Layer1 && Out_Layer2)) {
        printf("Failed to allocated memory, giving up.\n");
        return -1;
    }
#ifdef DMP_PAD_COEFF
    DumpPaddedCoeff("Filter_Layer0_HWCE", Filter_Layer0, 25, 1*8);
    DumpPaddedCoeff("Filter_Layer1_HWCE", Filter_Layer1, 25, 8*12);
#endif

    CLUSTER_Start(0, CORE_NUMBER, 0);

    Cifar10_L1_Memory = L1_Malloc(_Cifar10_L1_Memory_SIZE);
    if(Cifar10_L1_Memory == NULL) {
        printf("Cifar10_L1_Memory alloc failed\n");
        return -1;
    }

    // Execute the function "RunCifar10" on the cluster.
    CLUSTER_SendTask(0, RunCifar10, 0, 0);

    // Close the cluster
    CLUSTER_Stop(0);
    printf ("Test END\n");
    return 0;
}

