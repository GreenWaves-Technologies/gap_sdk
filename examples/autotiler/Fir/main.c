/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* PMSIS includes. */
#include "pmsis.h"

#include <stdio.h>
#include "Gap.h"
#include "FirKernels.h"


//short int * __restrict__ Samples;

PI_L1 short int * __restrict__ L1_Coeffs40;
PI_L1 short int * __restrict__ L1_Coeffs10;
PI_L1 short int *  __restrict__ L1_Coeffs20;
//short int * __restrict__ Out;

int finished = 0;

#define STACK_SIZE  1024
#define NSAMPLES	32768
#define NCOEFFS		40
int ERR=0 ;

// include coeffs, input, output from numpy ref
PI_L2 short int Samples[NSAMPLES] = {
#include "fir_in32768.h"
};

PI_L2 short int Out_ref40[NSAMPLES] = {
#include "fir_out40.h"
};

PI_L2 short int Out_ref20[NSAMPLES] = {
#include "fir_out20.h"
};

PI_L2 short int Out_ref10[NSAMPLES] = {
#include "fir_out10.h"
};


PI_L2 short int *Out;
//L2_MEM short int Out[NSAMPLES];

PI_L2 short int Coeffs40[NCOEFFS] = {
#include "fir_coeff40.h"
};

PI_L2 short int Coeffs20[NCOEFFS] = {
#include "fir_coeff20.h"
};

PI_L2 short int Coeffs10[NCOEFFS] = {
#include "fir_coeff10.h"
};


int absi(int a) {
  return ((a>0)?a:-a);

}

void check_output(int ncoeff) {

  int diff=0, totsum=0;
  short int *Out_ref;
  float ratio;
  
  if (ncoeff==40) Out_ref = Out_ref40;
  if (ncoeff==20) Out_ref = Out_ref20;
  if (ncoeff==10) Out_ref = Out_ref10;
  
  for (int i=NCOEFFS;i<NSAMPLES-ncoeff;i++){
    totsum += absi(Out[i]);
    diff += absi(Out[i+ncoeff]-Out_ref[i]);
#if 0
    printf("%d %d %d\n", i,Out[i+ncoeff],Out_ref[i]);
#endif    
  }
  ratio = 1000.0*(float)diff/(float)absi(totsum);
  if ((int) ratio > 5) ERR=1;
  printf("%s total  %d diff %d ratio %d\n",(int)ratio>5?"********":"", totsum,diff,(int)ratio);
  
}

static inline __attribute__((always_inline)) void DumpPerf(int taps, const char * typ)
{
	pi_perf_stop();

	printf("Core %1d %d Taps %5d Samples %s Cycles %8d PerOut %2d\n", gap_ncore(), taps,
		NSAMPLES, typ, pi_perf_read(PI_PERF_ACTIVE_CYCLES), pi_perf_read(PI_PERF_ACTIVE_CYCLES)/NSAMPLES);
	check_output(taps);
	
	pi_perf_reset();
	pi_perf_start();
}




static void cluster_main()
{
	printf ("cluster master start\n");

	pi_cl_dma_cmd_t DmaR_Evt1;
	
	// load coeffs from L2 to L1
	pi_cl_dma_cmd((int) Coeffs40, (int) L1_Coeffs40,  40*sizeof(short int),  PI_CL_DMA_DIR_EXT2LOC, &DmaR_Evt1);
	pi_cl_dma_cmd_wait(&DmaR_Evt1);
	pi_cl_dma_cmd((int) Coeffs20, (int) L1_Coeffs20,  20*sizeof(short int),  PI_CL_DMA_DIR_EXT2LOC, &DmaR_Evt1);
	pi_cl_dma_cmd_wait(&DmaR_Evt1);
	pi_cl_dma_cmd((int) Coeffs10, (int) L1_Coeffs10,  10*sizeof(short int),  PI_CL_DMA_DIR_EXT2LOC, &DmaR_Evt1);
	pi_cl_dma_cmd_wait(&DmaR_Evt1);
	
	unsigned int Ti, Ti1, Ti2;
	
	pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
 	pi_perf_start();


	TiledFir10GenericScalar(Samples, L1_Coeffs10, Out, 16);
	DumpPerf( 10, "GenericScalar");

	TiledFir10Generic(Samples, L1_Coeffs10, Out, 16);
	DumpPerf( 10, "GenericVect  ");

	TiledFir10Opt(Samples, L1_Coeffs10, Out, 16);
	DumpPerf( 10, "OptVect      ");

	TiledFir20GenericScalar(Samples, L1_Coeffs20, Out, 16);
	DumpPerf( 20, "GenericScalar");

	TiledFir20Generic(Samples, L1_Coeffs20, Out, 16);
	DumpPerf( 20, "GenericVect  ");

	TiledFir20Opt(Samples, (short int*)L1_Coeffs20, Out, 16);
	DumpPerf( 20, "OptVect      ");	

	TiledFir40GenericScalar(Samples,  (short int*)L1_Coeffs40, Out, 16);
	DumpPerf( 40, "GenericScalar");

	TiledFir40Generic(Samples,  (short int*)L1_Coeffs40, Out, 16);
	DumpPerf( 40, "GenericVect  ");
	pi_perf_stop();
}

static void end_of_app(){
	finished = 1;
	printf ("End of application\n");
}

void test_fir()
{
	printf ("Start of application\n");

	// Switch on the cluster
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

	printf ("Allocate memory\n");
	// Allocate the necessary memory areas in shared L1
	FIR_L1_Memory = pmsis_l1_malloc(_FIR_L1_Memory_SIZE);

	Out = (short int*) pmsis_l2_malloc(NSAMPLES*sizeof(short int) );
	
	L1_Coeffs40  = (short int *) pmsis_l1_malloc((40)*sizeof(short int));
	L1_Coeffs20  = (short int *) pmsis_l1_malloc((20)*sizeof(short int));
	L1_Coeffs10  = (short int *) pmsis_l1_malloc((10)*sizeof(short int));
	
	// Allocate areas in L2 for input and output
	//Samples       = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));
	//Out           = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));
	
	if (!(FIR_L1_Memory && L1_Coeffs10 && L1_Coeffs20 && L1_Coeffs40)) {
		printf("Memory allocation failed, L1 Tiler %p, Samples: %p, Coeffs: %p, Out: %p\n", FIR_L1_Memory, Samples, L1_Coeffs10, Out);
		pmsis_exit(-1);
	}

	printf ("Call cluster\n");
	struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = cluster_main;
    task->arg = (void *) NULL;
    task->stack_size = (uint32_t) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

	pi_cluster_close(&cluster_dev);
	
	if (ERR==0) {
	  printf("Test success\n");
	  pmsis_exit(0);
	}
	else {
	  printf("Test failed on accuracy wrt ref\n");
	  pmsis_exit(-1);
	}
}

int main()
{
    printf("\n\n\t *** PMSIS Fir Filter Test ***\n\n");
    return pmsis_kickoff((void *) test_fir);
}
