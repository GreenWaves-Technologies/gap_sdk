/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


/* Generic Includes*/
#include <stdio.h>
#include "gaplib/ImgIO.h"

/* PMSIS includes. */
#include "pmsis.h"

/* Autotiler includes. */
#include "IntegralImgKernels.h"
#include "Gap.h"

#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#ifdef NO_BRIDGE
#include "golden.h"
#else
L2_MEM unsigned char *ImageIn;
#endif
L2_MEM unsigned int *IntegralImg;

#define DEBUG 0

typedef struct ArgCluster {
	unsigned char *ImageIn;
	unsigned int W;
	unsigned int H;
	unsigned int *IntegralImg;
} ArgCluster_T;

static void cluster_main(ArgCluster_T *ArgC)
{
	printf ("cluster master start\n");

	int W=ArgC->W, H=ArgC->H;
	unsigned int i, MaxCore = gap_ncore();

    pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
    
	//Allocating L1 memory for cluster
	II_L1_Memory = (char *) pmsis_l1_malloc(_II_L1_Memory_SIZE);
	if (II_L1_Memory == 0) {
		printf("Failed to allocate %d bytes for L1_memory\n", _II_L1_Memory_SIZE); return ;
	}
	
	pi_perf_start();

	BodyIntegralImage(ArgC->ImageIn,ArgC->IntegralImg);

   	pi_perf_stop();
   	unsigned int Ti  = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
	printf("[W:%d, H:%d] Integral Image extraction done in %d cycles at %f cycles per pixel ....\n", W, H, Ti, (float)Ti/(W*H));

}


void integral_image(int argc, char *argv[])
{

	ArgCluster_T ClusterCall;
	int FromImage = 1;
	char *ImageName = "../../../Pedestrian.ppm";
	unsigned int Wi, Hi;

	//Input image size
	unsigned int W = 161, H = 121;

	printf ("Starting Integral Image sample application\n");

	#ifndef NO_BRIDGE
	//Allocating input and output image buffers in L2 memory
	ImageIn = (unsigned char *) pmsis_l2_malloc( W*H*sizeof(unsigned char));
	#endif

	IntegralImg = (unsigned int *) pmsis_l2_malloc( W*H*sizeof(unsigned int));

	if (ImageIn==0) {
		printf("Failed to allocate memory for Image (%d bytes)\n", W*H*sizeof(unsigned char));
		pmsis_exit(-4);
	}
	if (IntegralImg==0) {
		printf("Failed to allocate memory for Integral Image (%d bytes)\n", W*H*sizeof(unsigned int));
		pmsis_exit(-5);
	}

	#ifndef NO_BRIDGE
	//Reading Image from Hyperflash
	if (ReadImageFromFile(ImageName, W, H, 1, ImageIn, W*H*sizeof(unsigned char), 0, 0)) {
		printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
		pmsis_exit(-6);
	}
	#endif

	/* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-7);
    }

	//Assinging all input variables to Cluster structure
	ClusterCall.ImageIn     = ImageIn;
	ClusterCall.W           = W;
	ClusterCall.H           = H;
	ClusterCall.IntegralImg = IntegralImg;


    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = (void *)cluster_main;
    task->arg = (void *) &ClusterCall;
    task->stack_size = (uint32_t) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

	//Enable Debug to print to stdout the result image
	if(DEBUG){
		for(unsigned int a=0;a<H;a++){
			for(unsigned int b=0;b<W;b++)
				printf("%d ",IntegralImg[a*W + b]);
			printf("\n");
		}
	}

	// Close the cluster
    pi_cluster_close(&cluster_dev);

	#ifdef NO_BRIDGE
	for(unsigned int i=0;i<W*H;i++) if(IntegralImg[i]!=ImageOut_golden[i]) return pmsis_exit(-1);
	#endif
    printf ("Test success\n");
	
	pmsis_exit(0);
}

int main()
{
    printf("\n\n\t *** PMSIS Integral Image Autotiler Test ***\n\n");
    return pmsis_kickoff((void *) integral_image);
}

