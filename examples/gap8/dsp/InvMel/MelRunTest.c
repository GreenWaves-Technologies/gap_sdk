#ifndef __EMUL__
	#include "pmsis.h"
#else
	#include <stdlib.h>
	#include <stdio.h>
	#define pmsis_exit(a)	exit(a)		
#endif
#include "gaplib/wavIO.h"
#include "MEL_PARAMS.h"
#include "MelKernels.h"
#include "WindowLUT.def"
#include "SwapTable.def"
#include "FFTTwiddles.def"
#include "RFFTTwiddles.def"
#include "MelFBCoeff.def"
#include "MelFBSparsity.def"
#include "IMelFBCoeff.def"

#define  BUF_SIZE 	16500
#define  STACK_SIZE     2048
#define  N_FRAME        49

#if (DATA_TYPE==2)
typedef F16_DSP IN_TYPE;
typedef F16_DSP OUT_TYPE;
#elif (DATA_TYPE==3)
typedef float IN_TYPE;
typedef float OUT_TYPE;
#else
typedef short int IN_TYPE;
typedef int OUT_TYPE;
#endif

IN_TYPE *MelInSig, *InvMel, *MelOutShorts;
OUT_TYPE *MelOut;
short int *InWav;
int num_samples;
char *FileName;

#ifdef __EMUL__
char *FileName1 = "GapMel.bin";
char *FileName2 = "GapInvMel.bin";
#else
char *FileName1 = "../../../GapMel.bin";
char *FileName2 = "../../../GapInvMel.bin";
#endif

static void RunMFCC()
{
        #ifdef PERF
        gap_cl_starttimer();
        gap_cl_resethwtimer();
        #endif

        int start, elapsed_forward, elapsed_inverse;
        #if (DATA_TYPE==2) || (DATA_TYPE==3)
                start = gap_cl_readhwtimer();
                Librosa_Mel (MelInSig, MelOut, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff);
                elapsed_forward = gap_cl_readhwtimer() - start;

                start = gap_cl_readhwtimer();
                Librosa_IMel(MelOut, IMelFBCoeff, InvMel);
                elapsed_inverse = gap_cl_readhwtimer() - start;
        #elif (DATA_TYPE==0)
                start = gap_cl_readhwtimer();
                Librosa_Mel (MelInSig, MelOut, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff);
                elapsed_forward = gap_cl_readhwtimer() - start;

                /* Convert the Q16 int32 output of the MelSpectrogram into Q15 int16 */
                for (int i=0; i<N_FRAME*N_MELS; i++) MelOutShorts[i] = (short int) gap_clip(MelOut[i] >> 1, 15);

                start = gap_cl_readhwtimer();
                Librosa_IMel(MelOutShorts, IMelFBCoeff, InvMel, 10);
                elapsed_inverse = gap_cl_readhwtimer() - start;
        #endif

        #ifdef PERF
	printf("Forward Total Cycles: %d over %d Frames %d Cyc/Frame\n", elapsed_forward, N_FRAME, elapsed_forward / N_FRAME);
        printf("Inverse Total Cycles: %d over %d Frames %d Cyc/Frame\n", elapsed_inverse, N_FRAME, elapsed_inverse / N_FRAME);
        #endif
}

void test_kickoff(void *arg)
{
	#ifndef __EMUL__
	struct pi_device cluster_dev;
	struct pi_cluster_conf cl_conf;

        pi_cluster_conf_init(&cl_conf);

	cl_conf.id = 0;
        cl_conf.cc_stack_size = STACK_SIZE;

	pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
	if (pi_cluster_open(&cluster_dev))
	{
		printf("Cluster open failed !\n");
		pmsis_exit(-4);
	}
	#endif

        /* Allocate L1 Memory */
	L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, _L1_Memory_SIZE);
	if (L1_Memory==NULL){
		printf("Error allocating L1\n");
		pmsis_exit(-1);
	}

        /* Allocate L2 Memory */
	MelOut   = (OUT_TYPE *)   AT_L2_ALLOC(0, N_FRAME*N_MELS*sizeof(OUT_TYPE));
        InvMel   = (IN_TYPE *)    AT_L2_ALLOC(0, N_FRAME*(N_FFT/2+1)*sizeof(IN_TYPE));
        MelInSig = (IN_TYPE *)    AT_L2_ALLOC(0, BUF_SIZE*sizeof(IN_TYPE));
	InWav    = (short int *)  AT_L2_ALLOC(0, BUF_SIZE*sizeof(short));
	if (InWav==NULL || MelInSig==NULL){
		printf("Error allocating\n");
		pmsis_exit(1);
	}

        /* Read WAV file */
	header_struct header_info;
        if (ReadWavFromFile(FileName, InWav, BUF_SIZE*sizeof(short), &header_info)){
        	printf("Error reading wav file\n");
        	pmsis_exit(1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

        /* Initialize data: scale for float */
        #if (DATA_TYPE==2) || (DATA_TYPE==3)
        	printf("FLOAT%d\n", (DATA_TYPE==2)?16:32);
        	for (int i=0; i<num_samples; i++) {
        		MelInSig[i] = (IN_TYPE) InWav[i] / (1<<15);
        	}
        #else
            	printf("FIXED POINT\n");
                MelOutShorts = (short int *)   AT_L2_ALLOC(0, N_FRAME*N_MELS*sizeof(short int));
                for (int i=0; i<num_samples; i++) {
                        MelInSig[i] = (IN_TYPE) gap_clip(((int) InWav[i]), 15);
                }
        #endif

        /* Run on Cluster */
	#ifdef __EMUL__
		RunMFCC();
	#else
		struct pi_cluster_task task;
                pi_cluster_task(&task, RunMFCC, NULL);
		pi_cluster_send_task(&cluster_dev, &task);
	#endif

        switch_fs_t fs;
        __FS_INIT(fs);
        void *File;
        int ret = 0;

        File = __OPEN_WRITE(fs, FileName1);
        ret = __WRITE(File, MelOut, N_FRAME*N_MELS*sizeof(OUT_TYPE));
        __CLOSE(File);

        File = __OPEN_WRITE(fs, FileName2);
        ret = __WRITE(File, InvMel, N_FRAME*(N_FFT/2+1)*sizeof(IN_TYPE));
        __CLOSE(File);

        __FS_DEINIT(fs);

        pmsis_exit(0);
}

int main(int argc, char *argv[])
{
	printf("\n\n\t *** MFCC Test ***\n\n");
        #ifndef __EMUL__
                #define __XSTR(__s) __STR(__s)
                #define __STR(__s) #__s
                FileName = __XSTR(AT_WAV);
                return pmsis_kickoff((void *) test_kickoff);
        #else
                if (argc < 2) {
                        printf("Usage: mnist [image_file]\n");
                        exit(-1);
                }
                FileName = argv[1];
                test_kickoff(NULL);
                return 0;
        #endif
}
