/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#define CORE_NUMBER     8
/* Matrix add example main file. This code actually runs on GAP */
#include <stdlib.h>
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_autotiler.h"

//include "Gap8.h"
//include "ModelKernels.h"
//include "ModelKernelsInit.h"
#include "MatAddKernels.h"
#include "MatAddKernelsInit.h"


#include "model-size.h"


#define MAT_SIZE (MAT_W*MAT_H)

GAP_L2_DATA int Mat1[MAT_SIZE];
GAP_L2_DATA int Mat2[MAT_SIZE];
GAP_L2_DATA int MatOut[MAT_SIZE];

int finished = 0;

static void cluster_main()
{
    printf ("cluster master start\n");

    MatADD(Mat1, Mat2, MatOut, NULL);
}

static void end_of_app(){
    finished = 1;
    printf ("End of application\n");
}

static void init_test(){
    for(int i=0;i<MAT_SIZE;i++){
        Mat1[i] = 1;
        Mat2[i] = 2;
        MatOut[i] = 0;
    }
}

int main(int argc, char *argv[])

{
    // Load matrices to add
    init_test();

    printf ("Start of application\n");

    // Switch on the cluster
    CLUSTER_Start(0, CORE_NUMBER, 0);

    // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
    L1_Memory = L1_Malloc(L1_MEMORY_SIZE);
    if(L1_Memory == 0 ){
        printf("Memory Allocation Error! Quit...");
        return 1;
    }

    CLUSTER_SendTask(0, cluster_main, 0, 0); //end_of_app);


    CLUSTER_Stop(0);

    int H=100;
    int W=100;

    for(int i=0;i<H;i++){
        for(int j=0;j<W;j++){
            if (MatOut[(i*W)+j] != 3) {
                printf("Error in test");
                return 1;
            }
        }
    }
    printf("Test completed successfully\n");
    return 0;
}
