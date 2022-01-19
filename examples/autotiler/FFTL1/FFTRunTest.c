#ifndef __EMUL__
	#include "pmsis.h"
#else
	#define pmsis_exit(a)	exit(a)		
#endif

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s
#include <stdlib.h>
#include <stdio.h>
#include "Gap.h"
#include "DSP_Lib.h"
#include "TwiddlesDef.h"
#include "SwapTablesDef.h"
#include "In_Data.h"

#ifdef __gap9__
#define  MAXDIM 2048
#else
#define  MAXDIM 2048
#endif
#define  STACK_SIZE 			   2048
typedef void (*FFTFun_T )(void *Data, void *Twiddles, signed char *shift, unsigned int Nfft, unsigned int Inverse);


short int *InBuff_q16;
float     *InBuff_f32, *InBuff_f32R4, *OutBuff_f32;

char      *shift_buff;
void *L1_Twiddles, *L1_RTwiddles, *L1_SwapLUT, *OutBuff;
#ifdef __gap9__
f16     *InDataf16, *InBuff_f16;
float MSE_f16(float* real, f16* calc, int Size){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) calc[i])) * (real[i] - ((float) calc[i])));
	return mse / Size;
}
#endif

float MSE_16(float* real, short int* calc, int Size, int Q){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) FIX2FP(calc[i], Q))) * (real[i] - ((float) FIX2FP(calc[i], Q))));
	return mse / Size;
}

float MSE(float* real, int* calc, int Size, int Q){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += (real[i] - (float) FIX2FP(calc[i], Q)) * (real[i] - (float) FIX2FP(calc[i], Q));
	return mse / Size;
}

float MSE_f32(float* real, float* calc, int Size){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) calc[i])) * (real[i] - ((float) calc[i])));
	return mse / Size;
}

void CallFFT(int Nfft, int Type){

	// FFT: reset buffers, run and check mse
	int start, elapsed, elapsedFFT, Q;
	FFT_InstallArg_T ArgIns;
	FFT_Arg_T FFTArg;
	AT_L2_EVENT DmaR_Evt1;
	void (*FFTFun)(FFT_Arg_T*);
	void (*SwapFun)(SwapSamples_Arg_T*);
	char *FFTDataType = 0;
	void *InBuff;

  	ArgIns.Nfft = Nfft; 
  	ArgIns.Radix = ((Nfft)==64 || (Nfft)==256 || (Nfft)==1024)?4:2;
  	ArgIns.L1_Twiddles  = L1_Twiddles;
	ArgIns.L1_SwapLUT   = L1_SwapLUT;
	switch (Type) {
		case 0 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf32), ((AT_L2_INT_ADDR_TYPE) InBuff_f32), 8*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f32;
			FFTDataType = "F32";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R4_Twiddles_float_64;   ArgIns.SwapLUT = R4_SwapTable_fix_64;   break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_float_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_float_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_float_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_float_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_float_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_f32;
			else 				   FFTFun = &Radix4FFT_DIF_Par_f32;
			SwapFun = &SwapSamples_Par_f32;
			break;
		case 1 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataQ16), ((AT_L2_INT_ADDR_TYPE) InBuff_q16), 4*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_q16;
			FFTDataType = "Q16";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R4_Twiddles_fix_64;   ArgIns.SwapLUT = R4_SwapTable_fix_64;   Q = 10; break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_fix_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  Q = 8; break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_fix_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  Q = 8; break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_fix_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  Q = 6; break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_fix_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; Q = 6; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_fix_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; Q = 4; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_Fix16;
			else 				   FFTFun = &Radix4FFT_DIF_Par_Fix16;
			SwapFun = &SwapSamples_Par;
			break;
		case 2 :
			#ifdef __gap9__
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf16), ((AT_L2_INT_ADDR_TYPE) InBuff_f16), 4*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f16;
			FFTDataType = "F16";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R4_Twiddles_f16_64;   ArgIns.SwapLUT = R4_SwapTable_fix_64;   break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_f16_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_f16_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_f16_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_f16_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_f16_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_f16;
			else 				   FFTFun = &Radix4FFT_DIF_Par_f16;
			SwapFun = &SwapSamples_Par_f16;
			#endif
			break;
	}

  	start = gap_cl_readhwtimer(); FFT_InstallTwiddlesAndSwapLUT(&ArgIns, Type==0); elapsed = gap_cl_readhwtimer() - start; //printf("\tInstall cyc: %d\n", elapsed);
  	FFTArg.Data = InBuff;
  	FFTArg.Twiddles = L1_Twiddles;
  	FFTArg.N_fft = Nfft; FFTArg.Inverse = 0;
    SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){InBuff, L1_SwapLUT, Nfft};
  	start = gap_cl_readhwtimer();
  	AT_FORK(gap_ncore(), (void *) (*FFTFun), (void *) &FFTArg);
  	elapsedFFT = gap_cl_readhwtimer() - start;
  	start = gap_cl_readhwtimer();
  	__CALL((*FFTFun), &FFTArg);
  	AT_FORK(gap_ncore(), (void *) (*SwapFun), (void *) &SwapArg);
  	__CALL((*SwapFun), &SwapArg);
  	elapsed = gap_cl_readhwtimer() - start; printf("|     %4d | %3s %6s | %6d | %5d | %6d", Nfft, FFTDataType, ArgIns.Radix==2?"Radix2":"Radix4", elapsedFFT, elapsed, elapsed+elapsedFFT);
	#if !defined(__EMUL__) && defined(PERF_ALL)
  	printf(" | %7d | %7d | %7d | %8d | %7d |", pi_perf_read(PI_PERF_INSTR), pi_perf_read(PI_PERF_ACTIVE_CYCLES), pi_perf_read(PI_PERF_TCDM_CONT), pi_perf_read(PI_PERF_LD_STALL), pi_perf_read(PI_PERF_IMISS));
  	#else
  	printf(" |         |         |         |          |         |");
  	#endif
  	if (Type == 0) {
  		printf("          |\n");
  		// printf("\nOutFFT%d_f32 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) printf("%f%+fj, ", InBuff_f32[2*i], InBuff_f32[2*i+1]); printf("])\n");
  	} else if (Type == 1) {
  		// printf("\nOutFFT%d_q16 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) printf("%d%+dj, ", ((short int*)InBuff_q16)[2*i], ((short int*)InBuff_q16)[2*i+1]); printf("])\n");
  		printf(" %f |\n", MSE_16(InBuff_f32, (short int*) InBuff_q16, Nfft, Q));
  	} else if (Type == 2) {
  		#ifdef __gap9__
  		// printf("\nOutFFT%d_f16 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) printf("%f%+fj, ", ((f16*)OutBuff)[2*i], ((f16*)OutBuff)[2*i+1]); printf("])\n");
  		printf(" %f |\n", MSE_f16(InBuff_f32, (f16 *) InBuff_f16, Nfft));
  		#else
  		printf("\n");
  		#endif
  	}
}

static void RunFFT()
{
	#if !defined(__EMUL__) && defined(PERF_ALL)
  	pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR) | (1<<PI_PERF_ACTIVE_CYCLES) | (1<<PI_PERF_LD_EXT) | (1<<PI_PERF_TCDM_CONT) | (1<<PI_PERF_LD_STALL) | (1<<PI_PERF_IMISS));
  	pi_perf_cl_start();
  	#else
    gap_cl_starttimer();
    #endif
    gap_cl_resethwtimer();
    int	start, elapsed, timef32;
    printf("Initializing inputs....\n");
	//InitData4      (InDataQ16, MAXDIM, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	//InitData4_float(InDataf32, MAXDIM, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	#ifdef __gap9__
	for (int i=0; i<MAXDIM; i++) InDataf16[i] = (f16) InDataf32[i];
	#endif
	printf("Done!\n");

  	gap_cl_resethwtimer();

  	int FFTBins = 64;
  	printf("|----------+------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
  	printf("| FFT BINS | Type       | FFT    | Swap  | Tot    | Instr   | Act Cyc | TCDM Co | LD Stall | Imiss   | MSE Err  |\n");
  	printf("|----------+------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
	while (FFTBins < MAXDIM){
		//printf("FFT: %4d\n", FFTBins);
	  	CallFFT(FFTBins, 0);
	  	CallFFT(FFTBins, 1);
	  	#ifdef __gap9__
	  	CallFFT(FFTBins, 2);
	  	#endif
	  	FFTBins *= 2;
  		printf("|----------+------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
  	}
  	printf("Finished\n");
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
		printf("FC Frequency as %d MHz, CL Frequency = %d MHz, PERIIPH Frequency = %d\n", pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));
	#endif

	// InDataQ16  = (short int *)  AT_L2_ALLOC(0, MAXDIM*sizeof(short int));
	InBuff_q16 = (short int *)  AT_L1_ALLOC(0, 2*MAXDIM*sizeof(short int));
	// InDataf32  = (float *)      AT_L2_ALLOC(0, MAXDIM*sizeof(float));
	InBuff_f32 = (float *)      AT_L1_ALLOC(0, 2*MAXDIM*sizeof(float));
	OutBuff_f32 = (float *)     AT_L1_ALLOC(0, 2*MAXDIM*sizeof(float));
	OutBuff     = (void *) 	    AT_L1_ALLOC(0, 2*MAXDIM*sizeof(signed short));
	#ifdef __gap9__
	InDataf16  = (f16 *)        AT_L2_ALLOC(0, 2*MAXDIM*sizeof(f16));
	InBuff_f16 = (f16 *)        AT_L1_ALLOC(0, 2*MAXDIM*sizeof(f16));
	#endif
	L1_Twiddles	= (short int *) AT_L1_ALLOC(0, 2*MAXDIM * sizeof(short));
	L1_SwapLUT	= (short int *) AT_L1_ALLOC(0, 2*MAXDIM * sizeof(short));
	if (InDataQ16==0 || InBuff_q16==0 || InDataf32==0 || InBuff_f32==0) printf("Failed to allocated InData\n");

	#ifdef __EMUL__
		RunFFT();
	#else
		struct pi_cluster_task task;
		pi_cluster_task(&task, RunFFT, NULL);
		task.stack_size = (unsigned int) STACK_SIZE;
		task.slave_stack_size = (unsigned int) 1048;
		pi_cluster_send_task(&cluster_dev, &task);
	#endif
	printf("Exiting\n");
    pmsis_exit(0);
}

#ifndef __EMUL__
int main()
{
	printf("\n\n\t *** FFT Test ***\n\n");

    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    return pmsis_kickoff((void *) test_kickoff);
}
#else
int main(int argc, char *argv[])
{
    printf("\n\t *** FFT Test ***\n\n");
    test_kickoff(NULL);
    return 0;
}
#endif