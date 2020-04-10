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

#if defined(NO_BRIDGE)
#include "golden.h"
#endif

#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

PI_L2 unsigned char *ImageIn;
PI_L2 unsigned short *HoGFeatures;
PI_L1 unsigned char *CellLinesBuffer;
PI_L2 rt_perf_t *cluster_perf;

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

	// Configure performance counters for counting the cycles
	pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
 	
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
    pi_perf_start();

	//Launching HoG on cluster
	MyHOG(ArgC->ImageIn, (unsigned short **) CellLinesBuffer, ArgC->HoGFeatures, 0);

	//Stoping hardware timer to get cluster cycles
	pi_perf_stop();
    //Get cycles count after HOG cluster call
    unsigned int Ti = pi_perf_read(PI_PERF_ACTIVE_CYCLES);

	printf("[W:%d, H:%d] Hog Features extraction done in %d cycles at %f cycles per pixel ....\n", W, H, Ti, (float)Ti/(W*H));

}


void test_hog()
{
	int FromImage = 1;
	char *ImageName = "../../../Pedestrian.ppm";

	unsigned int W = 322, H = 242;

	unsigned int BlockW = (((((W-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);	// Number of blocks in a line
	unsigned int BlockH = (((((H-2)/HOG_CELL_SIZE) - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);	// Number of blocks in a row
	unsigned int BlockSize = HOG_NBINS*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE;								// Size of a single block
	unsigned int AllocSize = Max(W*H*sizeof(unsigned char), BlockW*BlockH*BlockSize*sizeof(unsigned short));

	ArgCluster_T ClusterCall;

	printf ("Start HoG Example application\n");

	ImageIn = (unsigned char *) pi_l2_malloc(AllocSize);
	HoGFeatures = (unsigned short *) ImageIn;
	if (AllocSize > (W*H+2)) ImageIn += ALIGN((AllocSize-W*H-2), 2);
	
	if (ImageIn==0) {
		printf("Failed to allocate Memory for Image and HoGFeatures (%d bytes)\n", AllocSize);
		pmsis_exit(-1);
	}

	#ifdef NO_BRIDGE
	for(uint32_t i =0 ;i<W*H;i++)
		ImageIn[i]=ImageInTest[i];
	#else
	
	printf("Loading image from bridge...\n");
	unsigned int Wi, Hi;
	if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageIn, AllocSize)==0) || (Wi!=W) || (Hi!=H)) {
		printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
		pmsis_exit(-1);
	}
	printf("Image Loaded.\n");
	#endif

	ReportHoGConfiguration( W, H, BlockW, BlockH);

	// Activate the Cluster
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

	ClusterCall.ImageIn     = ImageIn;
	ClusterCall.W           = W;
	ClusterCall.H           = H;
	ClusterCall.HoGFeatures = HoGFeatures;

	struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = (void *)cluster_main;
    task->arg = (void *) &ClusterCall;
    task->stack_size = (uint32_t) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, task);


	
	int hash=0;
	for(unsigned int i=0;i<BlockW*BlockH*BlockSize;i++){
		hash = (hash + (324723947 + ClusterCall.HoGFeatures[i])) ^93485734985;
	}
	#ifdef PRINT_HASH
	printf("Resulting hash: %d \n", hash);
	#endif

	// Close the cluster
	pi_cluster_close(&cluster_dev);
	
	if(hash == -933402687){
    	printf("Test success\n");
    	pmsis_exit(0);
	}
    else{
		printf("Test failed\n");
		pmsis_exit(-1);
    }

}

int main()
{
    printf("\n\n\t *** PMSIS Fir Filter Test ***\n\n");
    return pmsis_kickoff((void *) test_hog);
}
