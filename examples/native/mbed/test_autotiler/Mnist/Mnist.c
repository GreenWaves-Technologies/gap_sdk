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

#include "MnistKernels.h"
#include "MnistCoeffs.def"
#if RT_HAS_HWCE
#include "MnistCoeffs_HWCE.def"
#endif
#include "MnistData.def"

#define CORE_NUMBER     8

L2_MEM short int *Out_Layer0;
L2_MEM short int *Out_Layer1;
L2_MEM short int *Out_Layer2;

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)

{
    unsigned int i, j;
    int Acc = 0;
    unsigned int K = 5;

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
    printf("L2_MEM short int %s[] = {\n", Name);
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


static void RunMnist(void)

{
    unsigned int ElapsedTime[3];
    int CheckResults = 0;
    performance_t perf;

#if RT_HAS_HWCE
    /* Make HWCE event active */
    EU_EVT_MaskSet(1 << EU_HWCE_EVENT);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5ReLUMaxPool2x2_HWCE_0((short int*) ImageIn, Filter_Layer0_HWCE, Out_Layer0, 14, Bias_Layer0, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[0] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("HWCE Layer0", Out_Layer0, 8, 14, 14);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5ReLUMaxPool2x2_HWCE_1(Out_Layer0, Filter_Layer1_HWCE, Out_Layer1, 14, Bias_Layer1, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[1] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("HWCE Layer1", Out_Layer1, 12, 5, 5);
#else

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5ReLUMaxPool2x2_0((short int*) ImageIn, Filter_Layer0, Bias_Layer0, Out_Layer0, 14, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[0] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer0", Out_Layer0, 8, 14, 14);

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    Conv5x5ReLUMaxPool2x2_1(Out_Layer0, Filter_Layer1, Bias_Layer1, Out_Layer1, 14, 0);
    PERFORMANCE_Stop(&perf);
    ElapsedTime[1] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer1", Out_Layer1, 12, 5, 5);
#endif

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    LinearLayerReLU_1(Out_Layer1, Filter_Layer2, Bias_Layer2, Out_Layer2, 16, 13, 0);

    PERFORMANCE_Stop(&perf);
    ElapsedTime[2] = PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT);
    if (CheckResults) Check("SW   Layer2", Out_Layer1, 10, 1, 1);

    //Chenking Results
    int rec_digit=0;
    int highest=Out_Layer2[0];
    for(int i=1;i<10;i++){
        if(highest<Out_Layer2[i]){
            highest=Out_Layer2[i];
            rec_digit=i;
        }
    }

    printf("Recognized: %d\n",rec_digit);

    printf("Layer0: %7d Cycles\n", ElapsedTime[0]);
    printf("Layer1: %7d Cycles\n", ElapsedTime[1]);
    printf("Layer2: %7d Cycles\n", ElapsedTime[2]);
}

int main()

{

    printf("FC Launched\n");

    Out_Layer0 = (short int *) malloc( 24*24*sizeof(short int)*32);
    printf("Allocating %d: %s\n", 24*24*sizeof(short int)*32, Out_Layer0?"Ok":"Failed");
    Out_Layer1 = (short int *) malloc( 4*4*sizeof(short int)*64);
    printf("Allocating %d: %s\n", 4*4*sizeof(short int)*64, Out_Layer1?"Ok":"Failed");
    Out_Layer2 = (short int *) malloc( 10*sizeof(short int));
    printf("Allocating %d: %s\n", 10*sizeof(short int), Out_Layer2?"Ok":"Failed");
    if (!(Out_Layer0 && Out_Layer1 && Out_Layer2)) {
        printf("Failed to allocated memory, giving up.\n");
        return 0;
    }

#ifdef DMP_PAD_COEFF
    DumpPaddedCoeff("Filter_Layer0_HWCE", Filter_Layer0, 25, 1*32);
    DumpPaddedCoeff("Filter_Layer1_HWCE", Filter_Layer1, 25, 32*64);
#endif

    CLUSTER_Start(0, CORE_NUMBER, 0);

    Mnist_L1_Memory = L1_Malloc(_Mnist_L1_Memory_SIZE);
    if(Mnist_L1_Memory == NULL) {
        printf("Mnist_L1_Memory alloc failed\n");
        return -1;
    }

    // Execute the function "RunCifar10" on the cluster.
    CLUSTER_SendTask(0, RunMnist, 0, 0);

    // Close the cluster
    CLUSTER_Stop(0);
    printf ("Test END\n");

    return 0;
}

