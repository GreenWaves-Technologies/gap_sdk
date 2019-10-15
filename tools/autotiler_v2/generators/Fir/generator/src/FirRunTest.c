/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap8.h"
#include "FirKernels.h"

short int * __restrict__ Samples;
RT_L1_DATA short int * __restrict__ Coeffs;
short int * __restrict__ Out;

int finished = 0;

#define NSAMPLES	49149
#define NCOEFFS		40

#ifdef __EMUL__
typedef struct{

}rt_perf_t;
#endif

static inline __attribute__((always_inline)) void DumpPerf(rt_perf_t * p_perf, int taps, const char * typ)
{
#ifndef __EMUL__
	rt_perf_stop(p_perf);
	rt_perf_save(p_perf);
	printf("Core %1d %d Taps %5d Samples %s Cycles %8d PerOut %2d\n", gap8_ncore(), taps,
		NSAMPLES, typ, rt_perf_read(RT_PERF_CYCLES), rt_perf_read(RT_PERF_CYCLES)/NSAMPLES);
	rt_perf_init(p_perf);
	rt_perf_reset(p_perf);
	rt_perf_start(p_perf);
#endif
}

static void cluster_main()
{
	printf ("cluster master start\n");

	unsigned int Ti, Ti1, Ti2;
	rt_perf_t perf;
#ifndef __EMUL__
	rt_perf_init(&perf);
	rt_perf_reset(&perf);
	rt_perf_conf(&perf, RT_PERF_CYCLES);
	rt_perf_start(&perf);
#endif
	TiledFir10GenericScalar(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 10, "GenericScalar");
	TiledFir10Generic(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 10, "GenericVect  ");
	TiledFir10Opt(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 10, "OptVect      ");
	TiledFir20GenericScalar(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 20, "GenericScalar");
	TiledFir20Generic(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 20, "GenericVect  ");
	TiledFir20Opt(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 20, "OptVect      ");
	TiledFir40GenericScalar(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 40, "GenericScalar");
	TiledFir40Generic(Samples, Coeffs, Out, 10, 0);
	DumpPerf(&perf, 40, "GenericVect  ");
#ifndef __EMUL__
	rt_perf_stop(&perf);
#endif
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
	Coeffs        = (short int *) rt_alloc(RT_ALLOC_CL_DATA, NCOEFFS*sizeof(short int));
	// Allocate areas in L2 for input and output
	Samples       = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));
	Out           = (short int *) rt_alloc(RT_ALLOC_FC_RET_DATA, NSAMPLES*sizeof(short int));

	if (!(Samples && Out && FIR_L1_Memory && Coeffs)) {
		printf("Memory allocation failed, L1 Tiler %p, Samples: %p, Coeffs: %p, Out: %p\n", FIR_L1_Memory, Samples, Coeffs, Out);
		return 1;
	}

	printf ("Call cluster\n");
	rt_cluster_call(NULL, 0, cluster_main, NULL, NULL, 0, 0, rt_nb_pe(), rt_event_get(&sched, end_of_app, 0));

#ifndef __EMUL__
	while(!finished)
		rt_event_execute(&sched, 1);
#endif

	rt_cluster_mount(0, 0, 0, NULL);

	printf("Test success\n");
	return 0;
}
