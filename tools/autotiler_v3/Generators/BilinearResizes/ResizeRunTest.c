/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "ResizeKernels.h"
#include "gaplib/ImgIO.h"
#include "Gap.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

L2_MEM unsigned char *ImageIn;
L2_MEM unsigned char *ImageOut;
L2_MEM rt_perf_t *cluster_perf;

typedef struct ArgCluster {
	unsigned char *ImageIn;
	unsigned int Win;
	unsigned int Hin;
	unsigned int Wout;
	unsigned int Hout;
	unsigned char *ImageOut;
} ArgCluster_T;

static void cluster_main(ArgCluster_T *ArgC)
{
	printf ("cluster master start\n");

	int Win=ArgC->Win, Hin=ArgC->Hin;
	int Wout=ArgC->Wout, Hout=ArgC->Hout;

	unsigned int i, MaxCore = rt_nb_pe();

	rt_perf_t *perf = cluster_perf;
	// initialize the performance clock
	rt_perf_init(perf);
	// Configure performance counters for counting the cycles
	rt_perf_conf(perf, (1<<RT_PERF_CYCLES));

	printf("Cluster core %d Launched, %d cores configuration\n", rt_core_id(), rt_nb_pe());

	//Allocating L1 memory for cluster
	Resize_L1_Memory = (char *) rt_alloc( RT_ALLOC_CL_DATA, _Resize_L1_Memory_SIZE);
	if (Resize_L1_Memory == 0) {
		printf("Failed to allocate %d bytes for L1_memory\n", _Resize_L1_Memory_SIZE); return ;
	}

	//Starting hardware timer to get cluster cycles
    rt_perf_reset(perf); rt_perf_start(perf);

	//Launching resize on cluster
	ResizeImage(ArgC->ImageIn,ArgC->ImageOut);

	//Stoping hardware timer to get cluster cycles
	rt_perf_stop(perf); rt_perf_save(perf);
    unsigned int Ti = rt_perf_get(perf, RT_PERF_CYCLES);

	printf("[W:%d, H:%d] Resizing done in %d cycles at %d cycles per pixel ....\n", Win, Hin, Ti, Ti/(Win*Hin));
}


int main(int argc, char *argv[])
{
	ArgCluster_T ClusterCall;
	int FromImage = 1;
	char *ImageName = "../../../Pedestrian.ppm";
	unsigned int Wi, Hi;

	//Input image size
	unsigned int W = 322, H = 242;
	//Output image size (after rsezing)
	unsigned int Wout= W/2, Hout = H/2;

	if (rt_event_alloc(NULL, 8)) return -1;

	//To use file IO system
	//rt_bridge_connect(NULL);

	printf ("Start Resize example application\n");

	//Allocating input and output image buffers in L2 memory
	ImageIn  = (unsigned char *) rt_alloc( RT_ALLOC_L2_CL_DATA, W*H*sizeof(unsigned char));
	ImageOut = (unsigned char *) rt_alloc( RT_ALLOC_L2_CL_DATA, Wout*Hout*sizeof(unsigned char));

	if (ImageIn==0) {
		printf("Failed to allocate Memory for Image (%d bytes)\n", W*H*sizeof(unsigned char));
		return 1;
	}
	if (ImageOut==0) {
		printf("Failed to allocate Memory for Image Out  (%d bytes)\n", W*H*sizeof(unsigned int));
		return 1;
	}


	//Reading Image from Hyperflash
	if (ReadImageFromFile(ImageName, W, H, 1, ImageIn, W*H*sizeof(unsigned char), IMGIO_OUTPUT_CHAR, 0)) {
		printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
		return 1;
	}

	// Activate the Cluster
	rt_cluster_mount(MOUNT, CID, 0, NULL);

	// Allocate the memory of L2 for the performance structure
	cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	if (cluster_perf == NULL) return -1;

	// Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
	void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
	if (stacks == NULL) return -1;

	//Assinging all input variables to Cluster structure
	ClusterCall.ImageIn     = ImageIn;
	ClusterCall.Win         = W;
	ClusterCall.Hin         = H;
	ClusterCall.Wout        = Wout;
	ClusterCall.Hout        = Hout;
	ClusterCall.ImageOut    = ImageOut;

	// Execute the function "cluster_main" on the Core 0 of cluster.
	rt_cluster_call(NULL, CID, (void (*)(void *)) cluster_main, &ClusterCall, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);
	// The FC arrives here when the Cluster finished its job.

	WriteImageToFile("../../../resized_out.ppm",ClusterCall.Wout,ClusterCall.Hout,1,ImageOut,GRAY_SCALE_IO);

	// Close the cluster
	rt_cluster_mount(UNMOUNT, CID, 0, NULL);
    printf("Test success\n");

	return 0;
}
