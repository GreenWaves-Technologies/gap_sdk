/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "HoGKernels.h"
#include "ImgIO.h"
#include "Gap8.h"

#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

L2_MEM unsigned char *ImageIn;
L2_MEM unsigned short *HoGFeatures;
L1_CL_MEM unsigned char *CellLinesBuffer;
L2_MEM rt_perf_t *cluster_perf;

typedef struct ArgCluster {
	unsigned char *ImageIn;
	unsigned int W;
	unsigned int H;
	unsigned short *HoGFeatures;
} ArgCluster_T;


unsigned char *CreateCellLinesBuffer(unsigned int CellsPerLine, unsigned int LineCount)

{
	unsigned int CellLineSize = CellsPerLine*HOG_NBINS*sizeof(unsigned short);
	unsigned char *Buffer = (unsigned char *) rt_alloc( RT_ALLOC_CL_DATA, LineCount*sizeof(unsigned short **) + LineCount*CellLineSize);
	unsigned int i;

	if (Buffer==0) return 0;
	for (i=0; i<LineCount; i++)
		((unsigned short **) Buffer)[i] = (unsigned short *) (Buffer + LineCount*sizeof(unsigned short **) + i*CellLineSize);
	return Buffer;
}

void ReportHoGConfiguration(unsigned int W, unsigned H, unsigned int BlockW, unsigned int BlockH)

{
	printf("========================= Configuration ============================================\n");
	printf("HoG Cell  [%2d x %2d] Pixels\n",  HOG_CELL_SIZE, HOG_CELL_SIZE);
	printf("HoG Block [%2d x %2d] Cells\n",   HOG_BLOCK_SIZE, HOG_BLOCK_SIZE);
	printf("HoG Block Overlap [%2d] Cells\n", HOG_BLOCK_OVERLAP);
	printf("HoG Angular Resolution: %d degree, Number of bins: %d\n\n", HOG_ANGLE_STEP, HOG_NBINS);

	printf("Input Image:  [%d x %d], %d bytes\n", W, H, W*H);
	printf("Hog Features: [%d x %d x %d], %d bytes\n", BlockW, BlockH,
		HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS,
		BlockW*BlockH*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS*sizeof(unsigned short));
	printf("====================================================================================\n");
}

static void cluster_main(ArgCluster_T *ArgC)
{
	printf ("cluster master start\n");

	int W=ArgC->W, H=ArgC->H;
	unsigned int i, MaxCore = rt_nb_pe();

	rt_perf_t *perf = cluster_perf;
	// initialize the performance clock
	rt_perf_init(perf);
	// Configure performance counters for counting the cycles
	rt_perf_conf(perf, (1<<RT_PERF_CYCLES));

	printf("Cluster core %d Launched, %d cores configuration\n", rt_core_id(), rt_nb_pe());

	CellLinesBuffer = CreateCellLinesBuffer((W-2)/HOG_CELL_SIZE, HOG_BLOCK_SIZE);
	if (CellLinesBuffer == 0) {
		printf("Failed to allocated cell lines buffer\n"); return ;
	}
	HOG_L1_Memory = (char *) rt_alloc( RT_ALLOC_CL_DATA, _HOG_L1_Memory_SIZE);
	if (HOG_L1_Memory == 0) {
		printf("Failed to allocate %d bytes for L1_memory\n", _HOG_L1_Memory_SIZE); return ;
	}

	//Get cycles count before HOG cluster call
	//Starting hardware timer to get cluster cycles
    rt_perf_reset(perf); rt_perf_start(perf);

	//Launching HoG on cluster
	MyHOG(ArgC->ImageIn, (unsigned short **) CellLinesBuffer, ArgC->HoGFeatures, 0);

	//Stoping hardware timer to get cluster cycles
	rt_perf_stop(perf); rt_perf_save(perf);
    //Get cycles count after HOG cluster call
    unsigned int Ti = rt_perf_get(perf, RT_PERF_CYCLES);

	printf("[W:%d, H:%d] Hog Features extraction done in %d cycles at %d cycles per pixel ....\n", W, H, Ti, Ti/(W*H));

}


int main(int argc, char *argv[])

{
	int FromImage = 1;
	char *ImageName = "../../../Pedestrian.ppm";

	unsigned int W = 322, H = 242;

	if (rt_event_alloc(NULL, 8)) return -1;

	unsigned int BlockW = (((((W-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);	// Number of blocks in a line
	unsigned int BlockH = (((((H-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);	// Number of blocks in a row
	unsigned int BlockSize = HOG_NBINS*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE;								// Size of a single block
	unsigned int AllocSize = Max(W*H*sizeof(unsigned char), BlockW*BlockH*BlockSize*sizeof(unsigned short));

	ArgCluster_T ClusterCall;

	printf ("Start HoG Example application\n");

	rt_bridge_connect(1, NULL);

	ImageIn = (unsigned char *) rt_alloc( RT_ALLOC_L2_CL_DATA, AllocSize);
	HoGFeatures = (unsigned short *) ImageIn;
	if (AllocSize > (W*H+2)) ImageIn += ALIGN((AllocSize-W*H-2), 2);
	if (ImageIn==0) {
		printf("Failed to allocate Memory for Image and HoGFeatures (%d bytes)\n", AllocSize);
		return 1;
	}
	printf("Loading image from bridge...\n");
	unsigned int Wi, Hi;
	if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageIn, AllocSize)==0) || (Wi!=W) || (Hi!=H)) {
		printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
		return 1;
	}
	printf("Image Loaded.\n");

	ReportHoGConfiguration( W, H, BlockW, BlockH);

	// Activate the Cluster
	rt_cluster_mount(MOUNT, CID, 0, NULL);

	// Allocate the memory of L2 for the performance structure
	cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
	if (cluster_perf == NULL) return -1;

	// Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
	void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
	if (stacks == NULL) return -1;

	ClusterCall.ImageIn     = ImageIn;
	ClusterCall.W           = W;
	ClusterCall.H           = H;
	ClusterCall.HoGFeatures = HoGFeatures;

	// Execute the function "cluster_main" on the Core 0 of cluster.
	rt_cluster_call(NULL, CID, (void (*)(void *)) cluster_main, &ClusterCall, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);
	// The FC arrives here when the Cluster finished its job.


	int hash=0;
	for(unsigned int i=0;i<BlockW*BlockH*BlockSize;i++){
		hash = (hash + (324723947 + ClusterCall.HoGFeatures[i])) ^93485734985;
	}
	//printf("Resulting hash: %d \n", hash);

	// Close the cluster
	rt_cluster_mount(UNMOUNT, CID, 0, NULL);

	rt_bridge_disconnect(NULL);

	if(hash == -933402687)
    	printf("Test success\n");
    else
		printf("Test failed\n");


	return 0;
}
