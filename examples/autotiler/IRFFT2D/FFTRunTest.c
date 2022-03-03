#ifndef __EMUL__
	#include "pmsis.h"
#else
	#include <stdlib.h>
	#include <stdio.h>
	#define pmsis_exit(a)	exit(a)		
#endif
#include "RFFTKernels.h"
#include "WinLUT.def"
#include "gaplib/wavIO.h"

#define  BUF_SIZE 	16500
#define  STACK_SIZE     2048
#define  NORM           6
#define  N_FRAME        49

#if (DATA_TYPE==2)
typedef F16_DSP IN_TYPE;
typedef F16_DSP OUT_TYPE;
#elif (DATA_TYPE==3)
typedef float IN_TYPE;
typedef float OUT_TYPE;
#else
short int PreempShift[N_FRAME];
typedef short int OUT_TYPE;	
typedef short int IN_TYPE;
#endif

OUT_TYPE *Out;
IN_TYPE *FFTInSig;
short int *inWav;
int num_samples;
char *FileName;

static void RunFFT()
{
	int OutFrameBytes = (N_FFT / 2 + 1);
	gap_cl_starttimer();
	gap_cl_resethwtimer();
	int start, elapsed, timef32;
	short int PreempShift[N_FRAME];
	int Q_Out_FFT;
	int Log2Nfft = gap_fl1(N_FFT >> 1);
	Q_Out_FFT = 15 - (Log2Nfft - 1);

	start = gap_cl_readhwtimer();
	#if (DATA_TYPE==0)
	RFFT(FFTInSig, Out, TwiddlesLUT, RFFTTwiddlesLUT, SwapTable, WindowLUT, PreempShift);
	#else
	RFFT(FFTInSig, Out, TwiddlesLUT, RFFTTwiddlesLUT, SwapTable, WindowLUT);
	#endif
	elapsed = gap_cl_readhwtimer() - start;
	printf("RFFT:      %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);
	start = gap_cl_readhwtimer();
	IRFFT(Out, FFTInSig, TwiddlesLUT, RFFTTwiddlesLUT, SwapTable, (short int *)InvWindowLUT);
	elapsed = gap_cl_readhwtimer() - start;
	printf("IRFFT:     %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);

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

	FFTInSig   = (IN_TYPE *) AT_L2_ALLOC(0, N_FRAME*2*N_FFT * sizeof(IN_TYPE));
	Out        = (OUT_TYPE *) AT_L2_ALLOC(0, N_FRAME * 2 * (N_FFT/2+1) * sizeof(OUT_TYPE));
	if (FFTInSig==NULL || Out==NULL){
		printf("Error allocating\n");
		pmsis_exit(-1);
	}

	header_struct header_info;
        if (ReadWavFromFile(FileName, (short int *)FFTInSig, BUF_SIZE*sizeof(short), &header_info)){
        	printf("Error reading wav file\n");
        	pmsis_exit(-1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

        #if (DATA_TYPE==2) || (DATA_TYPE==3)
 	printf("FLOAT\n");
       	for (int i=num_samples-1; i>=0; i--) FFTInSig[i] = ((IN_TYPE) (((short int *) FFTInSig)[i]) / (1<<15));
       	for (int i=0; i<num_samples; i++) printf("%f, ", FFTInSig[i]); printf("\n");
       	#endif

	#ifdef __EMUL__
		RunFFT();
	#else
		struct pi_cluster_task task;
		pi_cluster_task(&task, RunFFT, NULL);
		task.stack_size = (unsigned int) STACK_SIZE;
		pi_cluster_send_task(&cluster_dev, &task);
	#endif


    pmsis_exit(0);
}

int main(int argc, char *argv[])
{
	printf("\n\n\t *** FFT Test ***\n\n");

	#ifdef __EMUL__
        if (argc < 2) {
                printf("Usage: mnist [image_file]\n");
                exit(-1);
        }
        FileName = argv[1];
        test_kickoff(NULL);
        #else
	#define __XSTR(__s) __STR(__s)
	#define __STR(__s) #__s
        FileName = __XSTR(AT_WAV);
        return pmsis_kickoff((void *) test_kickoff);
        #endif
}
