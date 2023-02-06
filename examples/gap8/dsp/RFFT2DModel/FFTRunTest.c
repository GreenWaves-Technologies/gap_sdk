#ifndef __EMUL__
	#include "pmsis.h"
#else
	#define pmsis_exit(a)	exit(a)		
#endif
#include <stdlib.h>
#include <stdio.h>
#include "RFFTKernels.h"
#include "WinLUT.def"
#include "WinLUT_f32.def"
#ifdef __gap9__
#include "WinLUT_f16.def"
#endif
#include "gaplib/wavIO.h"

#define  MAXDIM 	2048
#define  STACK_SIZE 	2048
#define  BUF_SIZE 	16500
#define  N_FRAME	49

short int *FFTInSigQ16, *OutQ16, *Twiddles_Q16, *SwapTable_Q16;
signed char *shift_fft;
float     *FFTInSigF32, *OutF32, *Twiddles_f32;
float MSE_buff[N_FRAME], MSE_Max[3];
int num_samples;
#ifdef __gap9__
f16       *FFTInSigF16, *OutF16, *Twiddles_f16;
float MSE_f16(float* real, f16* calc, int Size){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) calc[i])) * (real[i] - ((float) calc[i])));
	return mse / Size;
}
#endif
char *FileName;

float MSE_16(float* real, short int* calc, int Size, int Q){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) FIX2FP(calc[i], Q))) * (real[i] - ((float) FIX2FP(calc[i], Q))));
	return mse / Size;
}

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
	Twiddles_Q16 = TwiddlesLUT_Fix; SwapTable_Q16 = SwapTable_Fix;
	Twiddles_f32 = TwiddlesLUT_f32;
	#ifdef __gap9__
	Twiddles_f16 = TwiddlesLUT_f16;
	#endif


	start = gap_cl_readhwtimer();
	RFFT_f32(FFTInSigF32, OutF32, Twiddles_f32, RFFTTwiddlesLUT_f32, SwapTable_f32, WindowLUT_f32);
	elapsed = gap_cl_readhwtimer() - start;
	printf("FLOAT 32:     %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);

	start = gap_cl_readhwtimer();
	RFFT_q16(FFTInSigQ16, OutQ16, Twiddles_Q16, RFFTTwiddlesLUT_Fix, SwapTable_Q16, WindowLUT_Fix, PreempShift);
	elapsed = gap_cl_readhwtimer() - start;
	printf("FIX 16:       %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);
	MSE_Max[0] = 0; MSE_Max[1] = 0; MSE_Max[2] = 0;
	for (int i=0; i<N_FRAME; i++) {
		MSE_buff[i] = MSE_16(OutF32+2*OutFrameBytes*i, OutQ16+2*OutFrameBytes*i, OutFrameBytes, Q_Out_FFT+PreempShift[i]);
		MSE_Max[0] = (MSE_buff[i]>MSE_Max[0])?MSE_buff[i]:MSE_Max[0];
	}

	start = gap_cl_readhwtimer();
	RFFT_q16_nopreemp(FFTInSigQ16, OutQ16, Twiddles_Q16, RFFTTwiddlesLUT_Fix, SwapTable_Q16, WindowLUT_Fix);
	elapsed = gap_cl_readhwtimer() - start;
	printf("FIX 16 NOPRE: %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);

	#ifdef __gap9__
	start = gap_cl_readhwtimer();
	RFFT_f16(FFTInSigF16, OutF16, Twiddles_f16, RFFTTwiddlesLUT_f16, SwapTable_f16, WindowLUT_f16);
	elapsed = gap_cl_readhwtimer() - start;
	printf("FLOAT 16:     %10d Cycles %10d Cycles/Frame\n", elapsed, elapsed/N_FRAME);
	printf("|------------------+--------------+--------------+--------------|\n");
	printf("| MeanSquaredError | FIX16        | Fix16 NOPRE  | Float16      |\n");
	printf("|------------------+--------------+--------------+--------------|\n");
	for (int i=0; i<N_FRAME; i++) {
		float MSE_q16_nopreemp = MSE_16(OutF32+2*OutFrameBytes*i, OutQ16+2*OutFrameBytes*i, OutFrameBytes, Q_Out_FFT+PreempShift[i]);
		float MSE_f16_calc = MSE_f16(OutF32+2*OutFrameBytes*i, OutF16+2*OutFrameBytes*i, OutFrameBytes);
		printf("| MSE FRAME %2d:    | %e | %e | %e |\n", i, MSE_buff[i], MSE_q16_nopreemp, MSE_f16_calc);
		MSE_Max[1] = (MSE_q16_nopreemp>MSE_Max[1])?MSE_q16_nopreemp:MSE_Max[1];
		MSE_Max[2] = (MSE_f16_calc>MSE_Max[2])?MSE_f16_calc:MSE_Max[2];
	}
	printf("|------------------+--------------+--------------+--------------|\n");
	printf("| Max MSE          | %e | %e | %e |\n", MSE_Max[0], MSE_Max[1], MSE_Max[2]);
	printf("|------------------+--------------+--------------+--------------|\n");
	#else
	printf("|------------------+--------------+--------------|\n");
	printf("| MeanSquaredError | FIX16        | Fix16 NOPRE  |\n");
	printf("|------------------+--------------+--------------|\n");
	for (int i=0; i<N_FRAME; i++) {
		float MSE_q16_nopreemp = MSE_16(OutF32+2*OutFrameBytes*i, OutQ16+2*OutFrameBytes*i, OutFrameBytes, Q_Out_FFT+PreempShift[i]);
		MSE_Max[1] = (MSE_q16_nopreemp>MSE_Max[1])?MSE_q16_nopreemp:MSE_Max[1];
		printf("| MSE FRAME %2d:    | %e | %e |\n", i, MSE_buff[i], MSE_q16_nopreemp);
	}
	printf("|------------------+--------------+--------------|\n");
	printf("| Max MSE          | %e | %e |\n", MSE_Max[0], MSE_Max[1]);
	printf("|------------------+--------------+--------------|\n");
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

	L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, _L1_Memory_SIZE);
	if (L1_Memory==NULL){
		printf("Error allocating L1\n");
		pmsis_exit(-1);
	}

	FFTInSigQ16   = (short int *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(short));
	OutQ16        = (short int *) AT_L2_ALLOC(0, N_FRAME * 2 * (N_FFT/2+1) * sizeof(short));
	FFTInSigF32   = (float *)     AT_L2_ALLOC(0, BUF_SIZE * sizeof(float));
	OutF32        = (float *)     AT_L2_ALLOC(0, N_FRAME * 2 * (N_FFT/2+1) * sizeof(float));
	shift_fft 	  = (signed char *) AT_L2_ALLOC(0, N_FRAME * 2 * (N_FFT/2+1) * sizeof(signed char));
	#ifdef __gap9__
	FFTInSigF16   = (f16 *)       AT_L2_ALLOC(0, BUF_SIZE * sizeof(f16));
	OutF16        = (f16 *)       AT_L2_ALLOC(0, N_FRAME * N_FFT * 2 * sizeof(f16));
	#endif
	if (FFTInSigQ16==NULL || FFTInSigQ16==NULL || FFTInSigF32==NULL){
		printf("Error allocating\n");
		pmsis_exit(-1);
	}

	header_struct header_info;
        if (ReadWavFromFile(FileName, FFTInSigQ16, BUF_SIZE*sizeof(short), &header_info)){
        	printf("Error reading wav file\n");
        	pmsis_exit(-1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

 	printf("FLOAT\n");
       	for (int i=0; i<num_samples; i++) FFTInSigF32[i] = ((float) FFTInSigQ16[i]) / (1<<15);
        #ifdef __gap9__
       	for (int i=0; i<num_samples; i++) FFTInSigF16[i] = ((f16) FFTInSigQ16[i]) / (1<<15);
       	#endif

	#ifdef __EMUL__
		RunFFT();
	#else
		struct pi_cluster_task task;
		pi_cluster_task(&task, RunFFT, NULL);
		pi_cluster_send_task(&cluster_dev, &task);
	#endif

	#ifdef __gap9__
		if (MSE_Max[0] > 1.4e-4 || MSE_Max[1] > 9.44e-1 || MSE_Max[2] > 5.54e-7) {
			printf("The error is more than expected: %e (expected < 1.4e-4) %e (expected < 9.44e-1) %e (expected < 5.54e-7)\n", MSE_Max[0], MSE_Max[1], MSE_Max[2]);
			printf("Test FAILED\n");
			pmsis_exit(-1);
		}
	#else
		if (MSE_Max[0] > 1.4e-4 || MSE_Max[1] > 9.44e-1) {
			printf("The error is more than expected: %e (expected < 1.4e-4) %e (expected < 9.44e-1)\n", MSE_Max[0], MSE_Max[1]);
			printf("Test FAILED\n");
			pmsis_exit(-1);
		}
	#endif
	printf("Test PASSED\n");

    pmsis_exit(0);
}

#ifndef __EMUL__
int main()
{
	printf("\n\n\t *** FFT Test ***\n\n");

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