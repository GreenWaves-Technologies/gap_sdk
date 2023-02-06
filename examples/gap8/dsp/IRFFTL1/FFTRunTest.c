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
#include "RFFTTwiddlesDef.h"
#include "SwapTablesDef.h"
#include "In_Data.h"

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

void CallFFT(int Nfft, int Type, int Compare, int Parallel, int Debug){

	// FFT: reset buffers, run and check mse
	int start, elapsed, elapsed_forward=0, elapsed_inverse=0, Q=13, Qinv=13, Qin=13;
	FFT_InstallArg_T ArgIns;
	RFFT_Arg_T FFTArg;
	AT_L2_EVENT DmaR_Evt1;
	void (*FFTFun)(RFFT_Arg_T*) = 0;
	void (*IFFTFun)(RFFT_Arg_T*) = 0;
	char *FFTDataType = 0;
	void *InBuff = (float *) InBuff_f32;

	int Log2Nfft = gap_fl1(Nfft >> 1);

  	ArgIns.Nfft = Nfft; 
  	ArgIns.Radix = ((Nfft>>1)==256 || (Nfft>>1)==1024)?4:2;
  	ArgIns.L1_Twiddles  = L1_Twiddles;
  	ArgIns.L1_RTwiddles = L1_RTwiddles;
	ArgIns.L1_SwapLUT   = L1_SwapLUT;
	switch (Type) {
		case 0 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf32), ((AT_L2_INT_ADDR_TYPE) InBuff_f32), 4*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f32;
			FFTDataType = "F32";
			switch (Nfft>>1) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_float_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   ArgIns.RTwiddles = RFFT_Twiddles_float_128; break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_float_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  ArgIns.RTwiddles = RFFT_Twiddles_float_256; break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_float_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  ArgIns.RTwiddles = RFFT_Twiddles_float_512; break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_float_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  ArgIns.RTwiddles = RFFT_Twiddles_float_1024; break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_float_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; ArgIns.RTwiddles = RFFT_Twiddles_float_2048; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_float_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; ArgIns.RTwiddles = RFFT_Twiddles_float_4096; break;
			}
			FFTFun = &RFFT_DIF_Par_f32;
			IFFTFun = &IRFFT_DIF_Par_f32;
  			FFTArg.RFFT_Out = OutBuff_f32;
			break;
		case 1 :
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataQ16), ((AT_L2_INT_ADDR_TYPE) InBuff_q16), 2*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_q16;
			FFTDataType = "Q16";
			switch (Nfft>>1) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_fix_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   ArgIns.RTwiddles = RFFT_Twiddles_fix_128;  Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_fix_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  ArgIns.RTwiddles = RFFT_Twiddles_fix_256;  Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_fix_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  ArgIns.RTwiddles = RFFT_Twiddles_fix_512;  Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_fix_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  ArgIns.RTwiddles = RFFT_Twiddles_fix_1024; Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_fix_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; ArgIns.RTwiddles = RFFT_Twiddles_fix_2048; Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_fix_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; ArgIns.RTwiddles = RFFT_Twiddles_fix_4096; Q = Qin - (Log2Nfft - 1); Qinv = Q - (Log2Nfft-1) + (Log2Nfft+1); break;
			}
			FFTFun = &RFFT_DIF_Par_Fix16;
			IFFTFun = &IRFFT_DIF_Par_Fix16;
  			FFTArg.RFFT_Out = (short int *)OutBuff;
			break;
		case 2 :
			#ifdef __gap9__
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) InDataf16), ((AT_L2_INT_ADDR_TYPE) InBuff_f16), 2*Nfft, 0, &DmaR_Evt1);
			AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
			InBuff = (void *) InBuff_f16;
			FFTDataType = "F16";
			switch (Nfft>>1) {
				case 64:   ArgIns.Twiddles = R2_Twiddles_f16_64;   ArgIns.SwapLUT = R2_SwapTable_fix_64;   ArgIns.RTwiddles = RFFT_Twiddles_f16_128;  break;
				case 128:  ArgIns.Twiddles = R2_Twiddles_f16_128;  ArgIns.SwapLUT = R2_SwapTable_fix_128;  ArgIns.RTwiddles = RFFT_Twiddles_f16_256;  break;
				case 256:  ArgIns.Twiddles = R4_Twiddles_f16_256;  ArgIns.SwapLUT = R4_SwapTable_fix_256;  ArgIns.RTwiddles = RFFT_Twiddles_f16_512;  break;
				case 512:  ArgIns.Twiddles = R2_Twiddles_f16_512;  ArgIns.SwapLUT = R2_SwapTable_fix_512;  ArgIns.RTwiddles = RFFT_Twiddles_f16_1024; break;
				case 1024: ArgIns.Twiddles = R4_Twiddles_f16_1024; ArgIns.SwapLUT = R4_SwapTable_fix_1024; ArgIns.RTwiddles = RFFT_Twiddles_f16_2048; break;
				case 2048: ArgIns.Twiddles = R2_Twiddles_f16_2048; ArgIns.SwapLUT = R2_SwapTable_fix_2048; ArgIns.RTwiddles = RFFT_Twiddles_f16_4096; break;
			}
			FFTFun = &RFFT_DIF_Par_f16;
			IFFTFun = &IRFFT_DIF_Par_f16;
  			FFTArg.RFFT_Out = (f16 *)OutBuff;
			#endif
			break;
	}
	if (Debug) {
		if (Type == 0) {
	  		printf("\nInData%d_f32 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", ((float *)InBuff)[i]); printf("])\n");
		} else if (Type == 1) {
	  		printf("\nInData%d_q16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", FIX2FP(((short int *)InBuff)[i], Qin)); printf("])\n");
	  	} else if (Type == 2) {
	  		#ifdef __gap9__
	  		printf("\nInData%d_f16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", ((f16 *)InBuff)[i]); printf("])\n");
	  		#else
	  		printf("\n");
	  		#endif
	  	}
	}
	
	int iLog2N  = gap_fl1(Nfft);
  	start = gap_cl_readhwtimer(); RFFT_InstallTwiddlesAndSwapLUT(&ArgIns, Type==0); elapsed = gap_cl_readhwtimer() - start; //printf("\tInstall cyc: %d\n", elapsed);
  	if (Parallel) {
		FFTArg.Data = InBuff;
		FFTArg.Twiddles = L1_Twiddles;
		FFTArg.RTwiddles = L1_RTwiddles;
		FFTArg.SwapTable = L1_SwapLUT;
		FFTArg.N_fft = Nfft; FFTArg.Inverse = 0;
		start = gap_cl_readhwtimer();
		AT_FORK(8, (void *) (*FFTFun), (void *) &FFTArg);
		__CALL((*FFTFun), &FFTArg);
		elapsed_forward = gap_cl_readhwtimer() - start;
		FFTArg.Data = (Type==0)?OutBuff_f32:OutBuff;
		FFTArg.RFFT_Out = InBuff;
		start = gap_cl_readhwtimer();
		AT_FORK(8, (void *) (*IFFTFun), (void *) &FFTArg);
		__CALL((*IFFTFun), &FFTArg);
		elapsed_inverse = gap_cl_readhwtimer() - start;
	} else {
		switch (Type) {
			case 0:
				start = gap_cl_readhwtimer();
				RFFT_DIF_Seq_f32((float *)InBuff, (float *)OutBuff_f32, (float *)L1_Twiddles, (float *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_forward = gap_cl_readhwtimer() - start;
				start = gap_cl_readhwtimer();
				IRFFT_DIF_Seq_f32((float *)OutBuff_f32, (float *)InBuff, (float *)L1_Twiddles, (float *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_inverse = gap_cl_readhwtimer() - start;
				break;
			case 1:
				start = gap_cl_readhwtimer();
				RFFT_DIF_Seq_Fix16((short int *)InBuff, (short int *)OutBuff, (short int *)L1_Twiddles, (short int *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_forward = gap_cl_readhwtimer() - start;
				start = gap_cl_readhwtimer();
				IRFFT_DIF_Seq_Fix16((short int *)OutBuff, (short int *)InBuff, (short int *)L1_Twiddles, (short int *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_inverse = gap_cl_readhwtimer() - start;
				break;
			case 2:
				start = gap_cl_readhwtimer();
				RFFT_DIF_Seq_f16((F16_DSP *)InBuff, (F16_DSP *)OutBuff, (F16_DSP *)L1_Twiddles, (F16_DSP *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_forward = gap_cl_readhwtimer() - start;
				start = gap_cl_readhwtimer();
				IRFFT_DIF_Seq_f16((F16_DSP *)OutBuff, (F16_DSP *)InBuff, (F16_DSP *)L1_Twiddles, (F16_DSP *)L1_RTwiddles, L1_SwapLUT, Nfft);
				elapsed_inverse = gap_cl_readhwtimer() - start;				
				break;
		}
	}
	float MSE_For = 0, MSE_Inv = 0; 
  	if (Compare) {
	  	if (Type == 0) {
	  		MSE_Inv = MSE_f32(InDataf32, (float*) InBuff, Nfft);
	   	} else if (Type == 1) {
	  		MSE_For = MSE_16(OutBuff_f32, (short int*) OutBuff, Nfft, Q);
	  		MSE_Inv = MSE_16(InDataf32, (short int*) InBuff, Nfft, Qinv);
	  	} else if (Type == 2) {
	  		#ifdef __gap9__
	  		MSE_For = MSE_f16(OutBuff_f32, (f16 *) OutBuff, Nfft);
	  		MSE_Inv = MSE_f16(InDataf32, (f16*) InBuff, Nfft);
	  		#else
	  		return;
	  		#endif
	  	}
	}
  	printf("|      %d | For %4d | %3s %6s | %6d | %f |\n", Parallel?8:1, Nfft, FFTDataType, ArgIns.Radix==2?"Radix2":"Radix4", elapsed_forward, MSE_For);
  	printf("|      %d | Inv %4d | %3s %6s | %6d | %f |\n", Parallel?8:1, Nfft, FFTDataType, ArgIns.Radix==2?"Radix2":"Radix4", elapsed_inverse, MSE_Inv);
	if (Debug) {
		if (Type == 0) {
	  		printf("\nOutFFT%d_f32 = np.array([\n", Nfft); for(int i=0;i<((Nfft>>1)+1); i++) printf("%f%+fj, ", OutBuff_f32[2*i], OutBuff_f32[2*i+1]); printf("])\n");
	  		printf("\nInBuff%d_f32 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", ((float *)InBuff)[i]); printf("])\n");
		} else if (Type == 1) {
	  		printf("\nOutFFT%d_q16 = np.array([\n", Nfft); for(int i=0;i<((Nfft>>1)+1); i++) printf("%d%+dj, ", ((short int*)OutBuff)[2*i], ((short int*)OutBuff)[2*i+1]); printf("])\n");
	  		printf("\nInBuff%d_q16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", FIX2FP(((short int *)InBuff)[i], Qinv)); printf("])\n");
	  	} else if (Type == 2) {
	  		#ifdef __gap9__
	  		printf("\nOutFFT%d_f16 = np.array([\n", Nfft); for(int i=0;i<((Nfft>>1)+1); i++) printf("%f%+fj, ", ((f16*)OutBuff)[2*i], ((f16*)OutBuff)[2*i+1]); printf("])\n");
	  		printf("\nInBuff%d_f16 = np.array([\n", Nfft); for(int i=0;i<Nfft; i++) printf("%f, ", ((f16 *)InBuff)[i]); printf("])\n");
	  		#else
	  		printf("\n");
	  		#endif
	  	}
	}
}

static void RunFFT()
{
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    int	start, elapsed, timef32;
    printf("Initializing inputs....\n");
	#ifdef __gap9__
	for (int i=0; i<MAXDIM; i++) InDataf16[i] = (f16) InDataf32[i];
	#endif
	printf("Done!\n");

  	gap_cl_resethwtimer();

  	int FFTBins = MINDIM;
	printf("|========+==========+============+========+==========|\n");
  	printf("| NCores | FFT BINS | Type       | Cyc    | MSE Err  |\n");
	printf("|========+==========+============+========+==========|\n");
	while (FFTBins <= MAXDIM){
		#if (DATA_TYPE<0)
		  	CallFFT(FFTBins, 0, 1, 1, 0);
  			printf("|--------+----------+------------+--------+----------|\n");
		  	CallFFT(FFTBins, 0, 1, 0, 0);
  			printf("|--------+----------+------------+--------+----------|\n");
		  	CallFFT(FFTBins, 1, 1, 1, 0);
  			printf("|--------+----------+------------+--------+----------|\n");
		  	CallFFT(FFTBins, 1, 1, 0, 0);
		  	#ifdef __gap9__
  			printf("|--------+----------+------------+--------+----------|\n");
		  	CallFFT(FFTBins, 2, 1, 1, 0);
  			printf("|--------+----------+------------+--------+----------|\n");
		  	CallFFT(FFTBins, 2, 1, 0, 0);
		  	#endif
		#else
			CallFFT(FFTBins, DATA_TYPE, 0, 1, 1);
			CallFFT(FFTBins, DATA_TYPE, 0, 0, 1);
	  	#endif
		printf("|========+==========+============+========+==========|\n");
	  	FFTBins *= 2;
  	}
  	printf("Finished\n");
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

	#if (DATA_TYPE<0) || (DATA_TYPE==1)
	InBuff_q16 = (short int *)  AT_L1_ALLOC(0, MAXDIM*sizeof(short));
	if (InBuff_q16==0) printf("Failed to allocated InData\n");
	#endif
	#if defined(__gap9__) && ((DATA_TYPE<0) || (DATA_TYPE==2))
	InDataf16  = (f16 *)        AT_L2_ALLOC(0, MAXDIM*sizeof(f16));
	InBuff_f16 = (f16 *)        AT_L1_ALLOC(0, MAXDIM*sizeof(f16));
	if (InDataf16==0 || InBuff_f16==0) printf("Failed to allocated InData\n");
	#endif

	InBuff_f32  = (float *)     AT_L1_ALLOC(0, MAXDIM      *sizeof(float));
	OutBuff_f32 = (float *)     AT_L1_ALLOC(0, 2*(MAXDIM+1)*sizeof(float));
	OutBuff     = (void *)      AT_L1_ALLOC(0, 2*(MAXDIM+1)*sizeof(signed short));
	L1_Twiddles	= (short int *) AT_L1_ALLOC(0, 3*MAXDIM/2  *sizeof(short));
	L1_RTwiddles= (short int *) AT_L1_ALLOC(0, 2*MAXDIM    *sizeof(short));
	L1_SwapLUT	= (short int *) AT_L1_ALLOC(0, MAXDIM      *sizeof(short));

	if (InBuff_f32==0 || OutBuff_f32==0 || OutBuff==0 || L1_Twiddles==0 || L1_RTwiddles==0 || L1_SwapLUT==0) printf("Failed to allocated InData\n");

	#ifdef __EMUL__
		RunFFT();
	#else
		struct pi_cluster_task task;
		pi_cluster_task(&task, RunFFT, NULL);
		pi_cluster_send_task(&cluster_dev, &task);
	#endif
	printf("Exiting\n");
    pmsis_exit(0);
}

int main(int argc, char *argv[])
{
	printf("\n\n\t *** FFT Test ***\n\n");

	#ifndef __EMUL__
    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    return pmsis_kickoff((void *) test_kickoff);
    #else
    test_kickoff(NULL);
    return 0;
    #endif
}
