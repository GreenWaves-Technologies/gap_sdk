 /* test I2S interface */
 /* 2 channels using the traffic generator in test bench configured by I2C */
 /* PDM mode chan1 and chan0, using vip i2s */


#include <stdio.h>
#include <stdlib.h>
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_performance.h"

#include "./AUDIO/AUDIO.h"
#include "./MFCC/MFCC_Processing.h"

/* Kernel function */
#include "KernelLibStdTypes.h"

/* produced by read_data_test.c */
#ifdef DOMFCC
#include "DataTest.h"
#else
#include "DataFEAT.h"
#endif

#ifdef RT_HAS_HWCE
#include "CnnKernels_hwce.h"
#define POWERCNN1_2   (6670.f)
#define POWERCNN1     (5600.f)
#else
#include "CnnKernels.h"
#define POWERCNN1_2   (28000.f)
#define POWERCNN1     (16800.f)
#endif
#include "CnnKernels_tileddense.h"

#ifdef RT_HAS_HWCE
#include "coeff_hwce.h"
#else

#ifdef FULL_PREC
#include "coeff_ref.h"
#endif

#ifdef W_HALFCHAR
#include "coeff.h"
#endif

#ifdef W_CHAR
#include "coeff.h"
#endif
#endif

unsigned char Debug=0;
/*******************************************************************************
 * ClUSTER Definitions
 ******************************************************************************/
#define CORE_NUMBER      8
#define L1_Memory_SIZE 50000

performance_t cluster_perf;

/*******************************************************************************
 * Frame Variables
 ******************************************************************************/
v2s  W_Frame[N_FFT+4];
short int Frame[FRAME];
short int FEAT_LIST[N_FRAME * NUMCEP];
short int *pfeat_list;
uint8_t* i2s_buffer;

/*******************************************************************************
 * IMAGE Variables
 ******************************************************************************/
#define C0_NFEAT 32
#define C1_NFEAT 32
#define CLast_NFEAT 12

#define IMAGE_SIZE 98
#define BUF0_SIZE  20000
#define BUF1_SIZE  13000


short int l2_big0[BUF0_SIZE];
short int l2_big1[BUF1_SIZE];

/*******************************************************************************
 * CNN Variables
 ******************************************************************************/
#define CHECKSUM 52259
char *word_list[12] = {"silence", "none", "yes", "no", "up", "down", "left", "right", "on", "off", "stop",  "go"};

void CNN_Process() {

    #ifdef RT_HAS_HWCE
    Conv8x20MaxPool2x2_HWCE_0(pfeat_list,L2_W_0,l2_big0,8,L2_B_0,AllKernels + 0);
    #else
    ConvLayer1(pfeat_list, L2_W_0,l2_big0,8,L2_B_0,8,20,AllKernels + 0);
    #endif

    #ifdef RT_HAS_HWCE
    Conv6x10_HWCE_1(l2_big0,L2_W_1,l2_big1,8,L2_B_1,AllKernels + 1);
    #else

    #ifdef W_HALFCHAR
    ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #ifdef FULL_PREC
    ConvLayer2(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #ifdef W_CHAR
    ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #endif

    // in,filter,normfilter,bias,normbias,out,outsize
    #ifdef W_HALFCHAR
    Dense_halfchar(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2);
    #endif
    #ifdef FULL_PREC
    Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2);
    #endif
    #ifdef W_CHAR
    Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2);
    #endif
}

void Master_Entry(void *arg) {
    performance_t perf;
    #ifdef RT_HAS_HWCE
    /* Make HWCE event active */
    if (Debug) printf("CNN launched on the HWCE\n");
    EU_EVT_MaskSet(1 << EU_HWCE_EVENT);
    #else
    if (Debug) printf("CNN launches on cluster with (%d cores)\n", CORE_NUMBER);
    #endif

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    CNN_Process();
    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE (include core sleep cycles): %d\n" , (int)PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT));
}

int main() {
    int error = 0;

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    // Allocate a buffer in the shared L1 memory
    L1_Memory = L1_Malloc(L1_Memory_SIZE);

    if(L1_Memory == NULL) {
        if (Debug) printf("L1 Memory allocation error\n");
        return -1;
    }

    i2s_buffer = malloc((BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE);

    #ifdef USE_AUDIO
    Audio_Init();
    #endif

    while(1) {
#ifdef USE_AUDIO
        Audio_Enable();
        Audio_Capture();
        Audio_Disable();
#endif

#ifdef DOMFCC
#ifdef USE_AUDIO
        short int *InSignal = (short int *) (i2s_buffer + WAV_HEADER_SIZE);
#else
        short int *InSignal = DataIn;
#endif

        MFCC_Processing(InSignal, W_Frame, Frame, FEAT_LIST);
        pfeat_list = (short int*) FEAT_LIST;
#else
        pfeat_list = (short int*) DataIn;
#endif

        /* FC send a task to Cluster */
        CLUSTER_SendTask(0, Master_Entry, 0, 0);

        /* Wait cluster finish task */
        CLUSTER_Wait(0);

        //  ******************************** Softmax on FC ****************************
        int i, j, sum=0;
        char *s;
        uint8_t idx_max=0;

        printf("\n");
        int max=0x80000000;

        for(i=0; i < CLast_NFEAT; i++) {
            printf(" feat %d: %d  \n", i, l2_big0[i]);
            sum += l2_big0[i];

#ifndef TESTNONE
            if (l2_big0[i]>max && i!=1) {max=l2_big0[i];idx_max=i;}
#else
            if (l2_big0[i]>max) {max=l2_big0[i];idx_max=i;}
#endif
        }

        printf("found max %d\n", idx_max);
        printf("found word %s\n", word_list[idx_max]);

#ifdef USE_AUDIO
        Audio_SynchroPython();
#endif
    }

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    return error;
}
