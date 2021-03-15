#ifndef __EMUL__
	#include "pmsis.h"
#else
	#include <stdlib.h>
	#include <stdio.h>
	#define pmsis_exit(a)	exit(a)		
#endif
#include "wavIO.h"
#include "MFCC_params.h"
#include "MFCCKernels.h"
#include "LUT.def"
#include "MFCC_FB.def"

#define  BUF_SIZE 				   16500
#define  STACK_SIZE 			   2048

short int *out_feat;
short int *inSig;
int num_samples;
char *FileName;

static void RunMFCC()
{
  PRINTF("run mfcc\n");
  #ifdef PERF
    gap_cl_starttimer();
    gap_cl_resethwtimer();
	int	start = gap_cl_readhwtimer();
  #endif
  MFCC(inSig, out_feat, 0, TwiddlesLUT, SwapLUT, WindowLUT, MFCC_FilterBank, MFCC_Coeffs, 5, DCT_Coeff);
  #ifdef PERF
  	int elapsed = gap_cl_readhwtimer() - start;
  	printf("Total Cycles: %d over %d Frames %d Cyc/Frame\n", elapsed, N_FRAME, elapsed / N_FRAME);
  #endif
}

void test_kickoff(void *arg)
{
	#ifndef __EMUL__
		struct pi_device cluster_dev;
		struct pi_cluster_conf cl_conf;
		cl_conf.id = 0;

		pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
		if (pi_cluster_open(&cluster_dev))
		{
			printf("Cluster open failed !\n");
			pmsis_exit(-4);
		}
	#endif
	
	L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, _L1_Memory_SIZE);
	if (L1_Memory==NULL){
		printf("Error allocating L1\n");
		pmsis_exit(-1);
	}

	out_feat = (short int *) AT_L2_ALLOC(0, N_FRAME * N_DCT * sizeof(short int));
	inSig    = (short int *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(short));
	if (inSig==NULL || out_feat==NULL){
		printf("Error allocating\n");
		pmsis_exit(1);
	}

	header_struct header_info;
    if (ReadWavFromFile(FileName, inSig, BUF_SIZE*sizeof(short), &header_info)){
    	printf("Error reading wav file\n");
    	pmsis_exit(1);
    }
    num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

	#ifdef __EMUL__
		RunMFCC();
	#else
		struct pi_cluster_task task = {0};
		task.entry = RunMFCC;
		task.arg = NULL;
		task.stack_size = (unsigned int) STACK_SIZE;
		pi_cluster_send_task_to_cl(&cluster_dev, &task);
	#endif

	#ifdef PRINT_INOUT
	printf("out = \n");
	for (int i=0; i<N_FRAME; i++) {
		for (int j=0; j<N_DCT; j++) {
			printf("%d, ", out_feat[i*N_DCT+j]);
		}
		printf("\n");
	}
	#endif

    pmsis_exit(0);
}

#ifndef __EMUL__
int main()
{
	PRINTF("\n\n\t *** MFCC Test ***\n\n");

    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    FileName = __XSTR(AT_WAV);
    return pmsis_kickoff((void *) test_kickoff);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: mnist [image_file]\n");
        exit(-1);
    }
    FileName = argv[1];
    PRINTF("\n\n\t *** MFCC Test ***\n\n");
    test_kickoff(NULL);
    return 0;
}
#endif