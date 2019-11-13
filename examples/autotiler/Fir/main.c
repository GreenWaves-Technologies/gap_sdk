/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap.h"
#include "FirKernels.h"




//short int * __restrict__ Samples;
RT_CL_DATA short int * __restrict__ L1_Coeffs40;
RT_CL_DATA short int * __restrict__ L1_Coeffs10;
RT_CL_DATA short int *  __restrict__ L1_Coeffs20;
//short int * __restrict__ Out;

int finished = 0;

#define NSAMPLES	32768
#define NCOEFFS		40
int ERR=0 ;

// include coeffs, input, output from numpy ref
L2_MEM short int Samples[NSAMPLES] = {
#include "fir_in32768.h"
};

L2_MEM short int Out_ref40[NSAMPLES] = {
#include "fir_out40.h"
};

L2_MEM short int Out_ref20[NSAMPLES] = {
#include "fir_out20.h"
};

L2_MEM short int Out_ref10[NSAMPLES] = {
#include "fir_out10.h"
};


short int *Out;
//L2_MEM short int Out[NSAMPLES];

L2_MEM short int Coeffs40[NCOEFFS] = {
#include "fir_coeff40.h"
};

L2_MEM short int Coeffs20[NCOEFFS] = {
#include "fir_coeff20.h"
};

L2_MEM short int Coeffs10[NCOEFFS] = {
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

static inline __attribute__((always_inline)) void DumpPerf(rt_perf_t * p_perf, int taps, const char * typ)
{
	rt_perf_stop(p_perf);
	rt_perf_save(p_perf);
	printf("Core %1d %d Taps %5d Samples %s Cycles %8d PerOut %2d\n", gap_ncore(), taps,
		NSAMPLES, typ, rt_perf_read(RT_PERF_CYCLES), rt_perf_read(RT_PERF_CYCLES)/NSAMPLES);
	check_output(taps);
	rt_perf_init(p_perf);
	rt_perf_reset(p_perf);
	rt_perf_start(p_perf);
}




static void cluster_main()
{
	printf ("cluster master start\n");

	rt_dma_copy_t DmaR_Evt1;
	
	// load coeffs from L2 to L1
	rt_dma_memcpy((rt_pointerT) Coeffs40, (rt_pointerT) L1_Coeffs40,  40*sizeof(short int),  RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt1);
	rt_dma_memcpy((rt_pointerT) Coeffs20, (rt_pointerT) L1_Coeffs20,  20*sizeof(short int),  RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt1);
	rt_dma_memcpy((rt_pointerT) Coeffs10, (rt_pointerT) L1_Coeffs10,  10*sizeof(short int),  RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt1);
	
	unsigned int Ti, Ti1, Ti2;
	rt_perf_t perf;
	rt_perf_init(&perf);
	rt_perf_reset(&perf);
	rt_perf_conf(&perf, RT_PERF_CYCLES);
	rt_perf_start(&perf);
	
	TiledFir10GenericScalar(Samples, L1_Coeffs10, Out, 16);
	DumpPerf(&perf, 10, "GenericScalar");

	TiledFir10Generic(Samples, L1_Coeffs10, Out, 16);
	DumpPerf(&perf, 10, "GenericVect  ");

	TiledFir10Opt(Samples, L1_Coeffs10, Out, 16);
	DumpPerf(&perf, 10, "OptVect      ");

	TiledFir20GenericScalar(Samples, L1_Coeffs20, Out, 16);
	DumpPerf(&perf, 20, "GenericScalar");

	TiledFir20Generic(Samples, L1_Coeffs20, Out, 16);
	DumpPerf(&perf, 20, "GenericVect  ");

	TiledFir20Opt(Samples, (short int*)L1_Coeffs20, Out, 16);
	DumpPerf(&perf, 20, "OptVect      ");	

	TiledFir40GenericScalar(Samples,  (short int*)L1_Coeffs40, Out, 16);
	DumpPerf(&perf, 40, "GenericScalar");

	TiledFir40Generic(Samples,  (short int*)L1_Coeffs40, Out, 16);
	DumpPerf(&perf, 40, "GenericVect  ");
	rt_perf_stop(&perf);
}

static void end_of_app(){
	finished = 1;
	printf ("End of application\n");
}

int main()
{
	printf ("Start of application\n");

	// Allocate a scheduler and initialize for some events
	rt_event_sched_t sched;
	rt_event_sched_init(&sched);
	if (rt_event_alloc(&sched, 4)) return -1;

	// Switch on the cluster
	rt_cluster_mount(1, 0, 0, NULL);

	printf ("Allocate memory\n");
	// Allocate the necessary memory areas in shared L1
	FIR_L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _FIR_L1_Memory_SIZE);

	Out = (short int*) rt_alloc(RT_ALLOC_L2_CL_DATA, NSAMPLES*sizeof(short int) );
	
	L1_Coeffs40        = (short int *) rt_alloc(RT_ALLOC_CL_DATA, (40)*sizeof(short int));
	L1_Coeffs20        = (short int *) rt_alloc(RT_ALLOC_CL_DATA, (20)*sizeof(short int));
	L1_Coeffs10        = (short int *) rt_alloc(RT_ALLOC_CL_DATA, (10)*sizeof(short int));
	// Allocate areas in L2 for input and output
	//Samples       = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));
	//Out           = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));
	
	if (!(FIR_L1_Memory && L1_Coeffs10 && L1_Coeffs20 && L1_Coeffs40)) {
		printf("Memory allocation failed, L1 Tiler %p, Samples: %p, Coeffs: %p, Out: %p\n", FIR_L1_Memory, Samples, L1_Coeffs10, Out);
		return 1;
	}

	printf ("Call cluster\n");
	rt_cluster_call(NULL, 0, cluster_main, NULL, NULL, 0, 0, rt_nb_pe(), rt_event_get(&sched, end_of_app, 0));

	while(!finished)
		rt_event_execute(&sched, 1);

	rt_cluster_mount(0, 0, 0, NULL);

	if (ERR==0) {
	  printf("Test success\n");
	  return 0;
	}
	else {
	  printf("Test failed on accuracy wrt ref\n");
	  return 1;
	}
}
