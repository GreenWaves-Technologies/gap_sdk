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
#include <math.h>
#include "Gap.h"
#include "FirKernels.h"

int finished = 0;

#define STACK_SIZE  2048
#define NSAMPLES	   400
#define NCOEFFS		    74
int ERR=0 ;



#if FIR_FLOAT32
typedef float FP;
#else
typedef float16 FP;
#endif

// include coeffs, input, output from numpy ref

#if FIR_FLOAT32

PI_L2 float Samples[NSAMPLES] = {
#include "fir_golden_input.h"
};

PI_L2 float Out_ref[NSAMPLES] = {
#include "fir_golden_output.h"
};

PI_L2 float *Out;
//L2_MEM short int Out[NSAMPLES];

PI_L2 float Coeffs[NCOEFFS] = {
#include "fir_golden_coeffs.h"
};

#else

PI_L2 F16_DSP Samples[NSAMPLES] = {
#include "fir_golden_input.h"
};

PI_L2 F16_DSP Out_ref[NSAMPLES] = {
#include "fir_golden_output.h"
};

PI_L2 F16_DSP *Out;
//L2_MEM short int Out[NSAMPLES];

PI_L2 F16_DSP Coeffs[NCOEFFS] = {
#include "fir_golden_coeffs.h"
};
#endif


void check_output(int ncoeff) {

  FP diff=0, totsum=0;
  float ratio;

  for (int i=0;i<NSAMPLES;i++){
    totsum += fabs(Out[i]);
    diff += fabs(Out[i]-Out_ref[i]);
#ifdef DEBUG
    printf("Sample #: %d \t  Out: %f \t Ref: %f\n", i,Out[i],Out_ref[i]);
#endif    
  }
  ratio = 1000.0*diff/totsum;
  if ((int) ratio > 5) ERR=1;
  printf("Num Check: Gathered diff %f ratio %f\n", diff, ratio);
  
}

static inline __attribute__((always_inline)) void DumpPerf(int taps, const char * typ)
{
	pi_perf_stop();

	printf("Core: %1d - %d Taps - %5d Samples - %s Cycles %8d - Per Output %2d\n", gap_ncore(), taps,
		NSAMPLES, typ, pi_perf_read(PI_PERF_ACTIVE_CYCLES), pi_perf_read(PI_PERF_ACTIVE_CYCLES)/NSAMPLES);
	check_output(taps);
	
	pi_perf_reset();
	pi_perf_start();
}




static void cluster_main()
{
	
	printf ("cluster master start\n");

	pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
 	pi_perf_start();

	TiledFir(Samples, Coeffs, Out);
	DumpPerf( NCOEFFS, "TiledFir");

	pi_perf_stop();
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

	Out = (FP*) pmsis_l2_malloc(NSAMPLES*sizeof(FP) );
	
	
	if (!(FIR_L1_Memory && Out)) {
		printf("Memory allocation failed, L1 Tiler %pOut: %p\n", FIR_L1_Memory, Out);
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
