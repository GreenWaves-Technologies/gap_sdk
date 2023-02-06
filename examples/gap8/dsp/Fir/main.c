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
#include <string.h>
#include "Gap.h"
#include "FirKernels.h"

int finished = 0;

#define STACK_SIZE    1024
#define NSAMPLES	 32768
#define NCOEFFS		    40
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


PI_L2 short int* __restrict__ Out;

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
  
  for (int i=0;i<NSAMPLES;i++){
    totsum += absi(Out[i]);
    diff += absi(Out[i]-Out_ref[i]);
#ifdef DEBUG
    printf("%d %d %d\n", i,Out[i],Out_ref[i]);
#endif    
  }
  ratio = 1000.0*(float)diff/(float)absi(totsum);
  if ((int) ratio > 5) ERR=1;
  printf("%s total  %d diff %d ratio %d\n",(int)ratio>5?"********":"", totsum,diff,(int)ratio);
  
}

static inline __attribute__((always_inline)) void DumpPerf(int taps, const char * typ)
{
	pi_perf_stop();

	printf("Core %1d %d Taps %5d Samples %s Cycles %8d PerOut %2f\n", gap_ncore(), taps,
		NSAMPLES, typ, pi_perf_read(PI_PERF_ACTIVE_CYCLES), pi_perf_read(PI_PERF_ACTIVE_CYCLES)/(float)NSAMPLES);
	check_output(taps);
	
	pi_perf_reset();
	pi_perf_start();
}




static void cluster_main()
{
	printf ("cluster master start\n");
	
	unsigned int Ti, Ti1, Ti2;
	
	pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
 	pi_perf_start();

	TiledFir10GenericScalar(Samples, Coeffs10, Out, 14);
	DumpPerf( 10, "GenericScalar");

	TiledFir10Generic      (Samples, Coeffs10, Out, 14);
	DumpPerf( 10, "GenericVect  ");

	TiledFir10Opt          (Samples, Coeffs10, Out, 14);
	DumpPerf( 10, "OptVect      ");

	TiledFir20GenericScalar(Samples, Coeffs20, Out, 14);
	DumpPerf( 20, "GenericScalar");

	TiledFir20Generic      (Samples, Coeffs20, Out, 14);
	DumpPerf( 20, "GenericVect  ");

	TiledFir20Opt          (Samples, Coeffs20, Out, 14);
	DumpPerf( 20, "OptVect      ");	

	TiledFir40GenericScalar(Samples, Coeffs40, Out, 14);
	DumpPerf( 40, "GenericScalar");

	TiledFir40Generic      (Samples, Coeffs40, Out, 14);
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
	pi_cluster_conf_init(&cl_conf);
	cl_conf.id = 0;
	cl_conf.cc_stack_size = STACK_SIZE;
	pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
	if (pi_cluster_open(&cluster_dev))
	{
		printf("Cluster open failed !\n");
		pmsis_exit(-1);
	}

	printf ("Allocate memory\n");
	// Allocate the necessary memory areas in shared L1
	FIR_L1_Memory = pmsis_l1_malloc(_FIR_L1_Memory_SIZE);

	Out = (short int*) pmsis_l2_malloc(NSAMPLES*sizeof(short int));
	
	if (!(FIR_L1_Memory && Out)) {
		printf("Memory allocation failed, L1 Tiler %p, Out: %p\n", FIR_L1_Memory, Out);
		pmsis_exit(-1);
	}

	printf ("Call cluster\n");
	struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
	pi_cluster_task(task, cluster_main, NULL);
    pi_cluster_send_task(&cluster_dev, task);

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
