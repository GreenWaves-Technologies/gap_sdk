#ifndef __EMUL__
	#include "pmsis.h"
#else
	#define pmsis_exit(a)	exit(a)		
#endif

#ifndef SILENT
    #define PRINTF printf
#else
    #define PRINTF(...) ((void) 0)
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

#ifndef DEBUG
#define DEBUG 0
#endif
#define STACK_SIZE 2048
typedef void (*FFTFun_T )(void *Data, void *Twiddles, signed char *shift, unsigned int Nfft, unsigned int Inverse);
PI_L2 float MSE_ARR[MAXDIM/MINDIM+1][2];

short int *InBuff_q16;
float     *InBuff_f32, *InBuff_f32R4, *OutBuff_f32;

char      *shift_buff;
void *L1_Twiddles, *L1_RTwiddles, *L1_SwapLUT;
F16_DSP     *InDataf16, *InBuff_f16;
float MSE_f16(float* real, F16_DSP* calc, int Size){
	float mse = 0;
	for (int i=0; i<Size; i++) mse += ((real[i] - ((float) calc[i])) * (real[i] - ((float) calc[i])));
	return mse / Size;
}

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

void CallFFT(int Nfft, int Type, int Parallel){

	// FFT: reset buffers, run and check mse
	int start=0, elapsed=0, elapsedFFT=0, Q = 0, Qin=12;
	FFT_InstallArg_T ArgIns;
	FFT_Arg_T FFTArg;
	AT_L2_EVENT DmaR_Evt1;
	void (*FFTFun)(FFT_Arg_T*) = 0;
	void (*SwapFun)(SwapSamples_Arg_T*) = 0;
	char *FFTDataType = 0;
	void *InBuff = 0;
	int Log2Nfft = gap_fl1(Nfft >> 1);

  	ArgIns.Nfft = Nfft; 
  	ArgIns.Radix = (Nfft==256 || Nfft==1024 || Nfft==4096)?4:2;
  	ArgIns.L1_Twiddles  = L1_Twiddles;
	ArgIns.L1_SwapLUT   = L1_SwapLUT;
	switch (Type) {
		case 0 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf32), ((AT_L2_INT_ADDR_TYPE) InBuff_f32), 2*Nfft*sizeof(float), 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f32;
			FFTDataType = "F32";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_float_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_float_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_float_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_float_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_float_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_float_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; break;
				case 4096: ArgIns.Twiddles = R4_Twiddles_float_4096; ArgIns.SwapLUT = R4_SwapTable_fix_4096; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_f32;
			else 				   FFTFun = &Radix4FFT_DIF_Par_f32;
			SwapFun = &SwapSamples_Par_f32;
			break;
		case 1 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataQ16), ((AT_L2_INT_ADDR_TYPE) InBuff_q16), 2*Nfft*sizeof(short), 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_q16;
			FFTDataType = "Q16";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_fix_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   Q = 8; break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_fix_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  Q = 7; break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_fix_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  Q = 6; break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_fix_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  Q = 5; break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_fix_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; Q = 4; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_fix_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; Q = 3; break;
				case 4096: ArgIns.Twiddles = R4_Twiddles_fix_4096; ArgIns.SwapLUT = R4_SwapTable_fix_4096; Q = 2; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_Fix16;
			else 				   FFTFun = &Radix4FFT_DIF_Par_Fix16;
			SwapFun = &SwapSamples_Par;
			break;
		case 2 :
			#ifdef __gap9__
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf16), ((AT_L2_INT_ADDR_TYPE) InBuff_f16), 2*Nfft*sizeof(F16_DSP), 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f16;
			FFTDataType = "F16";
			switch (Nfft) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_f16_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_f16_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_f16_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_f16_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_f16_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_f16_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; break;
				case 4096: ArgIns.Twiddles = R4_Twiddles_f16_4096; ArgIns.SwapLUT = R4_SwapTable_fix_4096; break;
			}
			if (ArgIns.Radix == 2) FFTFun = &Radix2FFT_DIF_Par_f16;
			else 				   FFTFun = &Radix4FFT_DIF_Par_f16;
			SwapFun = &SwapSamples_Par_f16;
			#endif
			break;
	}
	if (DEBUG) {
		if (Type == 0) {
			printf("\nInData%d_f32 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f%+fj, ", ((float *)InBuff)[2*i], ((float *)InBuff)[2*i+1]); printf("])\n");
		} else if (Type == 1) {
			printf("\nInData%d_q16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f%+fj, ", FIX2FP(((short int *)InBuff)[2*i], Qin), FIX2FP(((short int *)InBuff)[2*i+1], Qin)); printf("])\n");
		} else if (Type == 2) {
			#ifdef __gap9__
			printf("\nInData%d_f16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f%+fj, ", ((F16_DSP *)InBuff)[2*i], ((F16_DSP *)InBuff)[2*i+1]); printf("])\n");
			#else
			printf("\n");
			#endif
		}
	}

  	start = gap_cl_readhwtimer(); FFT_InstallTwiddlesAndSwapLUT(&ArgIns, Type==0); elapsed = gap_cl_readhwtimer() - start; //printf("\tInstall cyc: %d\n", elapsed);
  	if (Parallel) {
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
		elapsed = gap_cl_readhwtimer() - start;
	} else {
		switch (Type) {
			case 0:
				if (ArgIns.Radix == 2) {start = gap_cl_readhwtimer(); Radix2FFT_DIF_Seq_f32(InBuff_f32, (float *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				else 				   {start = gap_cl_readhwtimer(); Radix4FFT_DIF_Seq_f32(InBuff_f32, (float *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				start = gap_cl_readhwtimer(); SwapSamples_Seq_f32((float *) InBuff, L1_SwapLUT, Nfft); elapsed = gap_cl_readhwtimer() - start;
				break;
			case 1:
				if (ArgIns.Radix == 2) {start = gap_cl_readhwtimer(); Radix2FFT_DIF_Seq_Fix16(InBuff_q16, (short int *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				else 				   {start = gap_cl_readhwtimer(); Radix4FFT_DIF_Seq_Fix16(InBuff_q16, (short int *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				start = gap_cl_readhwtimer(); SwapSamples_Seq_Fix16((short int *) InBuff, L1_SwapLUT, Nfft); elapsed = gap_cl_readhwtimer() - start;
				break;
			case 2:
				if (ArgIns.Radix == 2) {start = gap_cl_readhwtimer(); Radix2FFT_DIF_Seq_f16(InBuff_f16, (F16_DSP *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				else 				   {start = gap_cl_readhwtimer(); Radix4FFT_DIF_Seq_f16(InBuff_f16, (F16_DSP *) L1_Twiddles, Nfft, 0); elapsedFFT = gap_cl_readhwtimer() - start;}
				start = gap_cl_readhwtimer(); SwapSamples_Seq_f16((F16_DSP *) InBuff, L1_SwapLUT, Nfft); elapsed = gap_cl_readhwtimer() - start;
				break;
		}
	}

	if (DEBUG) {
		if (Type == 0) {
	  		PRINTF("\nOutFFT%d_f32 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) PRINTF("%f%+fj, ", InBuff_f32[2*i], InBuff_f32[2*i+1]); PRINTF("])\n");
		} else if (Type == 1) {
	  		PRINTF("\nOutFFT%d_q16 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) PRINTF("%d%+dj, ", ((short int*)InBuff_q16)[2*i], ((short int*)InBuff_q16)[2*i+1]); PRINTF("])\n");
	  	} else if (Type == 2) {
	  		#ifdef __gap9__
	  		PRINTF("\nOutFFT%d_f16 = np.array([\n", Nfft); for(int i=0;i<(Nfft); i++) PRINTF("%f%+fj, ", ((F16_DSP*)InBuff_f16)[2*i], ((F16_DSP*)InBuff_f16)[2*i+1]); PRINTF("])\n");
	  		#else
	  		printf("\n");
	  		#endif
	  	}
	}
  	PRINTF("|     %4d | %3s %6s %3s | %6d | %5d | %6d", Nfft, FFTDataType, ArgIns.Radix==2?"Radix2":"Radix4", Parallel?"Par":"Seq", elapsedFFT, elapsed, elapsed+elapsedFFT);
	#if !defined(__EMUL__) && defined(PERF_ALL)
  	PRINTF(" | %7d | %7d | %7d | %8d | %7d |", pi_perf_read(PI_PERF_INSTR), pi_perf_read(PI_PERF_ACTIVE_CYCLES), pi_perf_read(PI_PERF_TCDM_CONT), pi_perf_read(PI_PERF_LD_STALL), pi_perf_read(PI_PERF_IMISS));
  	#else
  	PRINTF(" |         |         |         |          |         |");
  	#endif
  	if (Type == 0) {
  		PRINTF("          |\n");
  	} else if (Type == 1) {
  		MSE_ARR[Nfft/MINDIM][0] = MSE_16(InBuff_f32, (short int*) InBuff_q16, 2*Nfft, Q);
  		PRINTF(" %f |\n", MSE_ARR[Nfft/MINDIM][0]);
  	} else if (Type == 2) {
  		#ifdef __gap9__
  		MSE_ARR[Nfft/MINDIM][1] = MSE_f16(InBuff_f32, (F16_DSP *) InBuff_f16, 2*Nfft);
  		PRINTF(" %f |\n", MSE_ARR[Nfft/MINDIM][1]);
  		#else
  		PRINTF("\n");
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
    PRINTF("Initializing inputs....\n");
	#if defined(__gap9__) && ((DATA_TYPE==2) || (DATA_TYPE<0))
	for (int i=0; i<2*MAXDIM; i++) InDataf16[i] = (F16_DSP) InDataf32[i];
	#endif
	PRINTF("Done!\n");

  	gap_cl_resethwtimer();

  	int FFTBins = MINDIM;
  	PRINTF("|----------+----------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
  	PRINTF("| FFT BINS | Type           | FFT    | Swap  | Tot    | Instr   | Act Cyc | TCDM Co | LD Stall | Imiss   | MSE Err  |\n");
  	PRINTF("|----------+----------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
	while (FFTBins <= MAXDIM){
	  	CallFFT(FFTBins, 0, 0);
	  	CallFFT(FFTBins, 0, 1);
	  	#if (DATA_TYPE==1) || (DATA_TYPE<0)
	  	CallFFT(FFTBins, 1, 0);
	  	CallFFT(FFTBins, 1, 1);
	  	#endif
	  	#if defined(__gap9__) && ((DATA_TYPE==2) || (DATA_TYPE<0)) 
	  	CallFFT(FFTBins, 2, 0);
	  	CallFFT(FFTBins, 2, 1);
	  	#endif
	  	FFTBins *= 2;
  		PRINTF("|----------+----------------+--------+-------+--------+---------+---------+---------+----------+---------+----------|\n");
  	}
  	PRINTF("Finished\n");
}

void test_kickoff(void *arg)
{
	#ifndef __EMUL__
		struct pi_device cluster_dev;
		struct pi_cluster_conf cl_conf;
		pi_cluster_conf_init(&cl_conf);
		cl_conf.id = 0;

		pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
		if (pi_cluster_open(&cluster_dev))
		{
			printf("Cluster open failed !\n");
			pmsis_exit(-4);
		}
		/* Frequency Settings */
		pi_freq_set(PI_FREQ_DOMAIN_FC, 50*1000*1000);
		pi_freq_set(PI_FREQ_DOMAIN_CL, 50*1000*1000);
		pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 50*1000*1000);
		printf("FC Frequency as %d Hz, CL Frequency = %d Hz, PERIIPH Frequency = %d Hz\n", 
		        pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));
	#endif

	int L1_size = 2*MAXDIM*sizeof(short)+2*MAXDIM*sizeof(float) + 2*MAXDIM*sizeof(float) + 3*MAXDIM*sizeof(short) + MAXDIM*sizeof(short);
	#ifdef __gap9__
	L1_size += 2*MAXDIM*sizeof(F16_DSP);
	#endif
	printf("Going to Allocate: %d for L1 memory\n", L1_size);
	#if (DATA_TYPE<0) || (DATA_TYPE==1)
	InBuff_q16  = (short int *) AT_L1_ALLOC(0, 2*MAXDIM*sizeof(short));
	#endif
	#if defined(__gap9__) && ((DATA_TYPE==2) || (DATA_TYPE<0)) 
	InDataf16  = (F16_DSP *)        AT_L2_ALLOC(0, 2*MAXDIM*sizeof(F16_DSP));
	InBuff_f16 = (F16_DSP *)        AT_L1_ALLOC(0, 2*MAXDIM*sizeof(F16_DSP));
	#endif

	InBuff_f32  = (float *)     AT_L1_ALLOC(0, 2*MAXDIM*sizeof(float));
	OutBuff_f32 = (float *)     AT_L1_ALLOC(0, 2*MAXDIM*sizeof(float));
	L1_Twiddles	= (short int *) AT_L1_ALLOC(0, 3*MAXDIM*sizeof(short));
	L1_SwapLUT	= (short int *) AT_L1_ALLOC(0,   MAXDIM*sizeof(short));
	if (InDataf32==0 || InBuff_f32==0 || OutBuff_f32==0 || L1_SwapLUT==0 || L1_Twiddles==0) {
		printf("Failed to allocated Buffers\n");
		pmsis_exit(-1);
	}

	#ifdef __EMUL__
		RunFFT();
	#else
		struct pi_cluster_task task;
		pi_cluster_task(&task, RunFFT, NULL);
		task.stack_size = (unsigned int) STACK_SIZE;
		task.slave_stack_size = (unsigned int) 1048;
		pi_cluster_send_task(&cluster_dev, &task);
	#endif

  	int FFTBins = 64;
	while (FFTBins <= MAXDIM){
		if (MSE_ARR[FFTBins/MINDIM][0] > 0.26) {
			printf("Error: MSE too large for %d FFT Q16: %f\n", FFTBins, MSE_ARR[FFTBins/MINDIM][0]);
			printf("Test FAILED\n");
			pmsis_exit(-1);
		}
		#if defined(__gap9__) && ((DATA_TYPE==2) || (DATA_TYPE<0)) 
		if (MSE_ARR[FFTBins/MINDIM][1] > 0.00027) {
			printf("Error: MSE too large for %d FFT F16: %f\n", FFTBins, MSE_ARR[FFTBins/MINDIM][1]);
			printf("Test FAILED\n");
			pmsis_exit(-1);
		}
		#endif
	  	FFTBins *= 2;
	}
 
	printf("Test PASSED\n");
    pmsis_exit(0);
}

int main(int argc, char *argv[])
{
    printf("\n\t *** FFT Test ***\n\n");
    #ifndef __EMUL__
    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    return pmsis_kickoff((void *) test_kickoff);
    #else
    test_kickoff(NULL);
    #endif
    return 0;
}
