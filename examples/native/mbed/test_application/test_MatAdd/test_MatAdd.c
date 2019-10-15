/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Matrix add example main file. This code actually runs on GAP */
// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#define CORE_NUMBER         8

#define MAT_W              100
#define MAT_H              100
#define MAT_SIZE           (MAT_W * MAT_H)

#define L1_BUFFER_SIZE     43200  // 40K
#define CYCLE_BUFFER_SIZE  7200

GAP_L2_DATA int *Mat1;
GAP_L2_DATA int *Mat2;
GAP_L2_DATA int *MatOut;

typedef struct matrix_addition_arg_s{
    int *In1;
    int *In2;
    int *Out;
    uint32_t W;
    uint32_t H;
} matrix_addition_arg_t;

inline static uint32_t __attribute__((always_inline)) ChunkSize(uint32_t X)
{
    uint32_t Log2Core = __FL1(CORE_NUMBER);
    uint32_t Chunk = (X >> Log2Core) + ((X & (CORE_NUMBER - 1)) != 0);
    return Chunk;
}

static void matrix_addition(matrix_addition_arg_t *KerArg0)
{
    // Work out which vertical strip of the tile we need to do
    uint32_t CoreId = __core_ID();
    uint32_t Chunk = ChunkSize(KerArg0->W);
    uint32_t First = CoreId*Chunk;
    uint32_t Last  = (First+Chunk > KerArg0->W) ? (KerArg0->W) : (First+Chunk);

    for (uint32_t i = 0; i<KerArg0->H; ++i)
    {
        for (uint32_t j=First; j<Last; j++) {
            KerArg0->Out[(i*KerArg0->W)+j] = KerArg0->In1[(i*KerArg0->W)+j] + KerArg0->In2[(i*KerArg0->W)+j];
        }
    }
    // Wait for the other cores to complete the tile
    CLUSTER_SynchBarrier();
}

// MatADD(Mat1, Mat2, MatOut, NULL);
static void matrix_addition_test(char *L1_BUFFER)
{
    dma_req_t DmaR_Evt1;
    dma_req_t DmaR_Evt2;
    dma_req_t DmaW_Evt1;
    int Last, NextLast;
    int N_Ti = 0;

    matrix_addition_arg_t KerArg0;

    printf ("cluster master start!\n");

    KerArg0.W = (uint32_t) (100);

    // copy from L2 to shared L1
    DMAMCHAN_Memcpy_1D((uint32_t)Mat1, (uint32_t)L1_BUFFER, CYCLE_BUFFER_SIZE, GAP_DMA_L22TCDM, &DmaR_Evt1);
    DMAMCHAN_Memcpy_1D((uint32_t)Mat2, (uint32_t)(L1_BUFFER + (CYCLE_BUFFER_SIZE * 2)), CYCLE_BUFFER_SIZE, GAP_DMA_L22TCDM, &DmaR_Evt2);

    for (int Iter = 0; Iter < 6; Iter++) {
        Last = ((Iter+1) == 6); NextLast = ((Iter+2) == 6);

        DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
        DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);

        if (!Last) {
            DMAMCHAN_Memcpy_1D((uint32_t)Mat1 + (Iter + 1) * CYCLE_BUFFER_SIZE,
                               (uint32_t)L1_BUFFER + ((N_Ti + 1) % 2) * CYCLE_BUFFER_SIZE,
                               NextLast?4000 : CYCLE_BUFFER_SIZE, GAP_DMA_L22TCDM, &DmaR_Evt1);
            DMAMCHAN_Memcpy_1D((uint32_t)Mat2 + (Iter + 1) * CYCLE_BUFFER_SIZE,
                               (uint32_t)L1_BUFFER + (CYCLE_BUFFER_SIZE * 2) + ((N_Ti + 1) % 2)*CYCLE_BUFFER_SIZE,
                               NextLast?4000 : CYCLE_BUFFER_SIZE, GAP_DMA_L22TCDM, &DmaR_Evt2);
        }

        KerArg0.In1 = (int * __restrict__) ((uint32_t) (L1_BUFFER + 0) + 7200*((N_Ti) % 2));
        KerArg0.In2 = (int * __restrict__) ((uint32_t) (L1_BUFFER + 14400) + 7200*((N_Ti) % 2));
        KerArg0.Out = (int * __restrict__) ((uint32_t) (L1_BUFFER + 28800) + 7200*((N_Ti) % 2));
        KerArg0.H = (uint32_t) (Last?10:18);

        CLUSTER_CoresFork((void *)matrix_addition, (void *) &KerArg0);

        if (Iter) {
            DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
        }

        DMAMCHAN_Memcpy_1D((uint32_t)MatOut + Iter * CYCLE_BUFFER_SIZE,
                           (uint32_t)L1_BUFFER + (CYCLE_BUFFER_SIZE * 4) + (N_Ti % 2)*CYCLE_BUFFER_SIZE,
                           Last?4000 : CYCLE_BUFFER_SIZE, GAP_DMA_TCDM2L2, &DmaW_Evt1);
        N_Ti++;
    }

    DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
}

static void matrix_addition_test_init(){
    for(int i = 0;i < MAT_SIZE; i++){
        Mat1[i] = 1;
        Mat2[i] = 2;
        MatOut[i] = 0;
    }
}

char *L1_BUFFER;

int main()
{
    printf("Fabric controller code execution for mbed_os matrix addition test\n");

    Mat1 = (int *) malloc(MAT_SIZE * sizeof(int));
    Mat2 = (int *) malloc(MAT_SIZE * sizeof(int));
    MatOut = (int *) malloc(MAT_SIZE * sizeof(int));

    /* Init test data */
    matrix_addition_test_init();

    /* Cluster Start - power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* Allocate a buffer in the shared L1 memory */
    char *L1_BUFFER = L1_Malloc(L1_BUFFER_SIZE);

    /* Allocation check */
    if(L1_BUFFER == 0 ){
        printf("Memory Allocation Error! Quit...\n");
        return 1;
    }

    /* FC send a task to cluster */
    CLUSTER_SendTask(0, matrix_addition_test, L1_BUFFER, 0);

    /* Cluster Stop - power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    for(int i = 0; i < MAT_H; i++){
        for(int j = 0; j < MAT_W; j++){
            if (MatOut[(i * MAT_W) + j] != 3)
                error++;
        }
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
