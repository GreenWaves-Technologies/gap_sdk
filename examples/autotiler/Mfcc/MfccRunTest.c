#ifndef __EMUL__
	#include "pmsis.h"
#else
	#include <stdlib.h>
	#include <stdio.h>
	#define pmsis_exit(a)	exit(a)		
#endif
#include "gaplib/wavIO.h"
#include "MFCC_params.h"
#include "MFCCKernels.h"
#include "TwiddlesDef.h"
#include "RFFTTwiddlesDef.h"
#include "SwapTablesDef.h"

#include "LUT.def"
#include "MFCC_FB.def"

#define  BUF_SIZE 	16500
#define  STACK_SIZE     2048
#define  NORM           6
#define  N_FRAME        49

#if (DATA_TYPE==2)
typedef f16 MFCC_IN_TYPE;
typedef f16 OUT_TYPE;
#elif (DATA_TYPE==3)
typedef float MFCC_IN_TYPE;
typedef float OUT_TYPE;
#else
typedef short int OUT_TYPE;	
typedef short int MFCC_IN_TYPE;
#endif

OUT_TYPE *out_feat;
OUT_TYPE *out_fft;
MFCC_IN_TYPE *MfccInSig;
short int *inWav;
int num_samples;
char *FileName;

static void RunMFCC()
{
        PRINTF("run mfcc\n");
        #ifdef PERF
        gap_cl_starttimer();
        gap_cl_resethwtimer();
        int start = gap_cl_readhwtimer();
        #endif

        #if (N_DCT == 0)
                #if (DATA_TYPE==2) || (DATA_TYPE==3)
                Librosa_MFCC(MfccInSig, out_feat, R2_Twiddles_float_512, RFFT_Twiddles_float_1024, R2_SwapTable_float_512, WindowLUT, MFCC_FilterBank, MFCC_Coeffs);
                #else
                Librosa_MFCC(MfccInSig, out_feat, R2_Twiddles_fix_512,   RFFT_Twiddles_fix_1024,   R2_SwapTable_fix_512,   WindowLUT, MFCC_FilterBank, MFCC_Coeffs, NORM);
                #endif
        #else
                #if (DATA_TYPE==2) || (DATA_TYPE==3)
                Librosa_MFCC(MfccInSig, out_feat, R2_Twiddles_float_512, RFFT_Twiddles_float_1024, R2_SwapTable_float_512, WindowLUT, MFCC_FilterBank, MFCC_Coeffs, DCT_Coeff);
                #else
                Librosa_MFCC(MfccInSig, out_feat, R2_Twiddles_fix_512,   RFFT_Twiddles_fix_1024,   R2_SwapTable_fix_512,   WindowLUT, MFCC_FilterBank, MFCC_Coeffs, NORM, DCT_Coeff);
                #endif
        #endif
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

        int frame_size;
        if (N_DCT > 0) frame_size = N_DCT;
        else           frame_size = MFCC_BANK_CNT;
	out_feat = (OUT_TYPE *) AT_L2_ALLOC(0, N_FRAME * frame_size * sizeof(OUT_TYPE));
	out_fft  = (OUT_TYPE *) AT_L2_ALLOC(0, N_FRAME * N_FFT * 2 * sizeof(OUT_TYPE));
	inWav    = (short int *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(short));
	MfccInSig = (MFCC_IN_TYPE *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(MFCC_IN_TYPE));
	if (inWav==NULL || MfccInSig==NULL){
		printf("Error allocating\n");
		pmsis_exit(1);
	}

	header_struct header_info;
        if (ReadWavFromFile(FileName, inWav, BUF_SIZE*sizeof(short), &header_info)){
        	printf("Error reading wav file\n");
        	pmsis_exit(1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

        #if (DATA_TYPE==2) || (DATA_TYPE==3)
        	printf("FLOAT\n");
        	for (int i=0; i<num_samples; i++) {
        		MfccInSig[i] = (MFCC_IN_TYPE) inWav[i] / (1<<15);
        	}
        #else
            	printf("FIXED POINT\n");
                for (int i=0; i<num_samples; i++) {
                        MfccInSig[i] = (MFCC_IN_TYPE) gap_clip(((int) inWav[i]), 15);
                }
        #endif

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
        printf("\n\nout = \n");
        for (int i=0; i<N_FRAME; i++) {
                for (int j=0; j<frame_size; j++) {
                        #if (DATA_TYPE==2) || (DATA_TYPE==3)
                              printf("%f, ", out_feat[i*frame_size+j]);
                        #else
                              //printf("%f, ", ((float) out_feat[i*frame_size+j])/(1<<NORM));
                              printf("%d, ", out_feat[i*frame_size+j]);
                        #endif
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
        if (argc < 2) {
                printf("Usage: mnist [image_file]\n");
                exit(-1);
        }
        FileName = argv[1];
        PRINTF("\n\n\t *** MFCC Test ***\n\n");
        test_kickoff(NULL);
        return 0;
}
#endif