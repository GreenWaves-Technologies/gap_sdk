/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Matrix add example main file. This code actually runs on GAP */
/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_autotiler.h"

#include "ModelKernels.h"
#include "ModelKernelsInit.h"
#include "model-size.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTaskMatrixAdd( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define CORE_NUMBER     8
#define CID             0

#define MAT_SIZE (MAT_W*MAT_H)

GAP_L2_DATA int Mat1[MAT_SIZE];
GAP_L2_DATA int Mat2[MAT_SIZE];
GAP_L2_DATA int MatOut[MAT_SIZE];

int finished = 0;

/****************************************************************************/

/* Program Entry. */
int main()
{
    printf("\n\n\t *** MatrixAdd Test ***\n\n");

#if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
	vTaskMatrixAdd,
	"TestMatrixAdd",
	configMINIMAL_STACK_SIZE * 2,
	NULL,
	tskIDLE_PRIORITY + 1,
	&xHandleDynamic
	);
    if(xTask != pdPASS) {
	printf("Task Dynamic is NULL !\n");
	vTaskDelete( xHandleDynamic );
    }
#endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

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

void vTaskMatrixAdd ( void *parameters )

{
    // Load matrices to add
    init_test();

    printf ("Start of application\n");

    // Switch on the cluster
    CLUSTER_Start(0, CORE_NUMBER);

    // Allocate the predetermined memory size in the shared L1 memory that the cluster can act on
    L1_Memory = L1_Malloc(L1_MEMORY_SIZE);
    if(L1_Memory == 0 ){
        printf("Memory Allocation Error! Quit...");
        exit(-1);
    }

    CLUSTER_SendTask(0, cluster_main, 0, 0); //end_of_app);


    CLUSTER_Stop(0);

    int H=100;
    int W=100;

    for(int i=0;i<H;i++){
        for(int j=0;j<W;j++){
            if (MatOut[(i*W)+j] != 3) {
                printf("Error in test");
                exit(-1);
            }
        }
    }
    printf("Test successfully completed !\n");
    vTaskSuspend( NULL );
}
