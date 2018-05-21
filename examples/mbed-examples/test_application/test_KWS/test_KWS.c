// test I2S interface
// 2 channels using the traffic generator in test bench configured by I2C
// PDM mode chan1 and chan0, using vip i2s

#include <stdio.h>
#include <string.h>
#include "gap_cluster.h"
#include "gap_dmamchan.h"

#include "./AUDIO/AUDIO.h"
#include "./MFCC/MFCC_Processing.h"

#define CORE_NUMBER      8
#define L1_Memory_SIZE 50000
GAP_L1_GLOBAL_DATA char * L1_Memory;

v2s  W_Frame[N_FFT+4];
short int Frame[FRAME];
short int FEAT_LIST[N_FRAME*NUMCEP];

int main() {
    #define Max(a, b)     (((a)>=(b))?(a):(b))

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER);

    // Allocate a buffer in the shared L1 memory
    L1_Memory = L1_Malloc(L1_Memory_SIZE);

    if(L1_Memory == NULL) {
        printf("L1 Memory allocation error\n");
        return -1;
    }

    while(1) {
        Audio_Capture();

        MFCC_Processing(W_Frame, Frame, FEAT_LIST);
    }




    return 0;
}
