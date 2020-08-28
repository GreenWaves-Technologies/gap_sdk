/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <math.h>
#include "Gap.h"
#include "FFTBasicKernels.h"
#include "TwiddlesDef.h"
#include "SwapTablesDef.h"

#define DIM 1024

void InitData(short int *Data, int Size, int Freq)

{
	int i;
	float Theta = (2*M_PI)/Freq;

	for (i=0; i<Size; i++) {
		if ((i%128)==0) printf("**** %d\n", i);
		// Data[i] = FP2FIXR(sinf(Theta*i), 15);
		Data[i] = FP2FIXR(cosf(Theta*i), 15);
	}
}

void InitData4(short int *Data, int Size, int F1, int F2, int F3, int F4, float p1, float p2, float p3, float p4)

{
	int i;
	float Theta1 = (2*M_PI)/F1;
	float Theta2 = (2*M_PI)/F2;
	float Theta3 = (2*M_PI)/F3;
	float Theta4 = (2*M_PI)/F4;

	for (i=0; i<Size; i++) {
		if ((i%128)==0) printf("**** %d\n", i);
		// Data[i] = FP2FIXR(sinf(Theta*i), 15);
		float V = (cosf(Theta1*i+p1)+cosf(Theta2*i+p2)+cosf(Theta3*i+p3)+cosf(Theta4*i+p4))/4;
		Data[i] = FP2FIXR(V, 15); // FP2FIXR(cosf(Theta1*i), 15);
	}
}

void DumpData(char *Mess, short int *Data, int Size)

{
	int i, Cnt=0;

	printf("%s\n", Mess);

	for (i=0; i<Size; i++) {
		// if (((Cnt+1)%15)==0) printf("\n"); Cnt++;
		printf("%d\n", Data[i]);

	}
	printf("\n");
}


short int *InData;

void Process()

{
	int FromFcTcdm = 0;
	Samples2Complex_Arg_T C0;
	FFT_Arg_T C1;
	SwapSamples_Arg_T C2;
	v2s *OutData;
	short int *Twiddles = R4_Twiddles_1024;
	short int *SwapLUT = R4_SwapTable_1024;

	printf("FC available memory: %d Bytes\n", plp_getFcTcdmHeapSize());
	InData = (short int *) plp_alloc_l2(DIM*sizeof(short int));
	if (InData==0) printf("Failed to allocated InData\n");

	// InitData(InData, DIM, 37);
	InitData4(InData, DIM, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	// DumpData("Input: ", InData, DIM);

	if (FromFcTcdm) {
		C0 = (Samples2Complex_Arg_T) {InData, -2, (v2s *) plp_alloc_fc(DIM*sizeof(v2s)), 13, DIM, 1};
		Twiddles = (short int *) plp_alloc_fc(3*DIM*(sizeof(short)/2));
		if (Twiddles) for (int i=0; i<(sizeof(R4_Twiddles_1024)/sizeof(short int)); i++) Twiddles[i] = R4_Twiddles_1024[i];
		SwapLUT =  (short int *) plp_alloc_fc(DIM*sizeof(short int));
		if (SwapLUT) for (int i=0; i<(sizeof(R4_SwapTable_1024)/sizeof(short int)); i++) SwapLUT[i] = R4_SwapTable_1024[i];
	} else
		C0 = (Samples2Complex_Arg_T) {InData, -2, (v2s *) plp_alloc_l2(DIM*sizeof(v2s)), 13, DIM, 1};
	OutData = C0.Out;
	if (OutData == 0 || Twiddles == 0 || SwapLUT == 0) printf("Failed to allocate FFT input\n");
	Samples2Complex(&C0);

	// for (int i=0; i<5; i++) printf("In: %X, InC: %X\n", InData[i], C0.Out[i][0]);

	printf("Done with Sample expansion\n");

	C1.Data = (short int *) C0.Out; C1.Twiddles = Twiddles; C1.N_fft = DIM;
	// C1.Data = (short int *) C0.Out; C1.Twiddles = R2_Twiddles_1024; C1.N_fft = DIM;
	gap8_resethwtimer();
	unsigned int Ti = gap8_readhwtimer();
	Radix4FFT_DIF(&C1);
	// Radix2FFT_DIF(&C1);
	printf("FFT1D Vectorial Radix4 %d, Cycles = %d\n", DIM, gap8_readhwtimer()-Ti);
/*
	Ti = gap8_readhwtimer();
	Radix4FFT_DIF_Scalar(&C1);
	printf("FFT1D Scalar Radix4 %d, Cycles = %d (Init: %d)\n", DIM, gap8_readhwtimer()-Ti, Ti);
*/
	printf("Done with FFT\n");

	C2.Data = (v2s *) C1.Data; C2.SwapTable = SwapLUT; C2.Ni = DIM;
	// C2.Data = (v2s *) C1.Data; C2.SwapTable = R2_SwapTable_1024; C2.Ni = DIM;
	Ti = gap8_readhwtimer();
	SwapSamples(&C2);
	printf("Done with Swap Samples, Cycles = %d\n", gap8_readhwtimer()-Ti);
	int MaxE = 0, MaxEIndex = 0;
	for (int i=0; i<(DIM/2); i++) {
		int E = gap8_dotp2(OutData[i], OutData[i])>>1;
		if (E>MaxE) {
			MaxE = E; MaxEIndex = i;
		}
	}
	printf("Dominant Frequency at Index %d: %d * Sample Frequency\n", MaxEIndex, gap_roundnorm((DIM<<16)/MaxEIndex, 16));
/*
	{
		int Cnt = 0;
		for (int i=0; i<DIM; i++) {
			if (((Cnt+1)%4)==0) printf("\n"); Cnt++;
			// printf("%4d: %10d, %10d -> %10d ", i, OutData[i][0], OutData[i][1], gap8_dotp2(OutData[i], OutData[i]));
			printf("%4d: %5d ", i, gap8_dotp2(OutData[i], OutData[i])>>15);
		}
	}
*/
	// for (int i=0; i<DIM; i++) printf("%d\n", gap8_dotp2(OutData[i], OutData[i])>>15);


}

void Process1()

{
	int InL1 = 1;
	int Scalar = 0;
	int Parallel = 1;
	Samples2Complex_Arg_T C0;
	FFT_Arg_T C1;
	SwapSamples_Arg_T C2;
	v2s *OutData = (v2s *) plp_alloc_l1(DIM*sizeof(v2s));
        FFT_InstallArg_T ArgIns;
	unsigned int Ti;

	ArgIns = (FFT_InstallArg_T) {DIM, 4, R4_Twiddles_1024, R4_SwapTable_1024,
				     (short int *) plp_alloc_l1(3*DIM*(sizeof(short)/2)), (short int *) plp_alloc_l1(DIM*sizeof(short int))};

       if (ArgIns.L1_Twiddles == 0 || ArgIns.L1_SwapLUT == 0 || OutData == 0) {
                printf("Failed to allocated Twiddles or Swap LUT\n"); exit(0);
        }
	FFT_InstallTwiddlesAndSwapLUT(&ArgIns);
	printf("Install Twiddles and Swap LUT done\n");


	C0 = (Samples2Complex_Arg_T) {InData, -2, OutData, 13, DIM, 1};
	Samples2Complex(&C0);

	printf("Done with Sample expansion\n");

	C1 = (FFT_Arg_T) {(short int *) OutData, InL1?ArgIns.L1_Twiddles:R4_Twiddles_1024, DIM};

	gap8_resethwtimer();
	if (Scalar) Radix4FFT_DIF_Scalar(&C1);
	else if (Parallel) {
		plp_fork1((unsigned int) Radix4FFT_DIF_Par, (unsigned int) &C1);
		// Radix4FFT_DIF_Par(&C1);
	} else Radix4FFT_DIF(&C1);
	printf("FFT1D Radix4 %d, Cycles = %d\n", DIM, gap8_readhwtimer());
	// Radix4FFT_DIF_Scalar(&C1);
	printf("Done with FFT\n");

	C2 = (SwapSamples_Arg_T) {(v2s *) C1.Data, InL1?ArgIns.L1_SwapLUT:R4_SwapTable_1024, DIM};
	Ti = gap8_readhwtimer();
	if (Parallel)
		plp_fork1((unsigned int) SwapSamples_Par, (unsigned int) &C2);
	else SwapSamples(&C2);
	printf("Done with Swap Samples, Cycles = %d\n", gap8_readhwtimer()-Ti);
	int MaxE = 0, MaxEIndex = 0;
	for (int i=0; i<(DIM/2); i++) {
		int E = gap_dotp2(OutData[i], OutData[i])>>1;
		if (E>MaxE) {
			MaxE = E; MaxEIndex = i;
		}
	}
	printf("Dominant Frequency at Index %d: %d * Sample Frequency, Energy: %d\n", MaxEIndex, gap_roundnorm((DIM<<16)/MaxEIndex, 16), MaxE);
	for (int i=0; i<(DIM/2); i++) {
		int E = gap_dotp2(OutData[i], OutData[i])>>1;
		if ((E>>12)>(900)) printf("Peak found at index %d:  %d * Sample Frequency, E: %d\n", i, gap_roundnorm((DIM<<16)/i, 16), E>>12);
	}
/*
	for (int i=0; i<(DIM/2); i++) {
		int E = gap8_dotp2(OutData[i], OutData[i])>>1;
		printf("%d\n", E>>12);
	}
*/
}


int main()

{
        if (gap_clusterid()!=0) {
                printf("FC Launched\n");
		Process();
		plp_cluster_fetch(0x1);
                gap_cluster_wait(0);

        } else {
		unsigned int i, MaxCore = gap_ncore();
                unsigned int stacks = plp_alloc_l1(plp_stackSize()*(MaxCore-1));

                printf("Cluster core %d Launched, %d cores configuration\n", gap_coreid(), gap_ncore());
                plp_setSlaveStacks(((1<<MaxCore)-1), stacks, plp_stackSize());
                gap_setupbarrier(0, (1<<MaxCore)-1);

		Process1();

	}
}
