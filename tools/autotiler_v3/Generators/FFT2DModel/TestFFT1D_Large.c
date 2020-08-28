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
#include "Gap8.h"
#include "FFTBasicKernels.h"
#include "TwiddlesDef.h"
#include "SwapTablesDef.h"

#define DIM 16384

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

/*
        x is a Q16.15 value, it represents a fraction of pi => 0x7FFF is equivalent to pi

        sin (x) = 3.140625*x + 0.02026367*x^2 - 5.325196*x^3 + 0.5446778*x^4 + 1.800293*x^5

        This approximation fits very well sin(x) in the [0..pi/2] range => use symetry to always evaluate in [0..pi/2]
        Result is return in Q15
*/
int Sin(int X)

{
#define Abs(a)  (((a)<0)?-(a):(a))
        static short int SinCoeffLUT[] = { 0x00003240, 0x00000053, 0xAACC, 0x000008B7, 0x00001CCE, 0 };  // 4.12 fixed point
        short int R, R0;
        int Res, R1;
        v2s *VCoeff = (v2s *)SinCoeffLUT;
        v2s V0, V1;
        X = X+FP2FIX(1.0, 15);
        X = gap8_bitinsert(X, gap8_bitextract(X, 1, 31), 16, 16) + FP2FIX(-1.0, 15);    /* X = (X+1)%2 - 1, Reduces to [-pi..pi] */
        X = (short) X;
        int x = Abs(X);
        x = FP2FIX(-1.0, 15)*gap8_bitextract(x, 1, 14) + (2*gap8_bitextract(x, 1, 14)+1)*x;     /* x in [0..pi/2] */
        int Sign = gap8_bitextract(X, 1, 31)*2 + 1;

        R = __builtin_pulp_mulshRN(x, x, 15, 1<<14);
        V0 = gap8_pack2(x, R);                          // x, x^2
   R0 = R = __builtin_pulp_mulshRN(R, x, 15, 1<<14);
        R = __builtin_pulp_mulshRN(R, x, 15, 1<<14);
        V1 = gap8_pack2(R0, R);                         // x^3, x^4
        R1 = __builtin_pulp_mulsRN(R, x, 15, 1<<14);
                                                        // x^5, ??
        Res = gap8_dotp2   (V0,       VCoeff[0]);
        Res = gap8_sumdotp2(V1,       VCoeff[1], Res);
        Res = gap8_sumdotp2((v2s) R1, VCoeff[2], Res);
        Res = gap8_clip(Sign*gap8_roundnorm(Res, 12), 15);
        return Res;
#undef Abs
}

int Cos(int X)

{
        return Sin(X+FP2FIX(0.5, 15));
}

static inline int Div(int X, int Y)

{
        return ((X<<15)/Y);
}

void InitData4FP(short int *Data, int N, int F1, int F2, int F3, int F4, int p1, int p2, int p3, int p4)

{
        int i;

        for (i=0; i<N; i++) {
                Data[i] = (Cos(Div(2*i, F1)+p1) + Cos(Div(2*i, F2)+p2) + Cos(Div(2*i, F3)+p3) + Cos(Div(2*i, F4)+p4))>>2;
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
	short int *Twiddles = R4_Twiddles_16384;
	short int *SwapLUT = R4_SwapTable_16384;

	printf("FC available memory: %d Bytes\n", plp_getFcTcdmHeapSize());
	InData = (short int *) plp_alloc_l2(DIM*sizeof(short int));
	if (InData==0) printf("Failed to allocated InData\n");

	// InitData(InData, DIM, 37);
	// InitData4(InData, DIM, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	InitData4FP(InData, DIM, 37, 15, 23, 73, FP2FIXR(0.1/M_PI, 15), FP2FIXR(0.5/M_PI, 15), FP2FIXR(0.6/M_PI, 15), FP2FIXR(0.8/M_PI, 15));
	printf("Done with Init\n");
	// DumpData("Input: ", InData, DIM);

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
	printf("Dominant Frequency at Index %d: %d * Sample Frequency\n", MaxEIndex, gap8_roundnorm((DIM<<16)/MaxEIndex, 16));
#ifdef TRACE
	{
		int Cnt = 0;
		for (int i=0; i<DIM/2; i++) {
			if (((Cnt+1)%8)==0) printf("\n"); Cnt++;
			// printf("%4d: %10d, %10d -> %10d ", i, OutData[i][0], OutData[i][1], gap8_dotp2(OutData[i], OutData[i]));
			printf("%5d: %7d ", i, gap8_dotp2(OutData[i], OutData[i])>>1);
		}
	}
#endif
	// for (int i=0; i<DIM; i++) printf("%d\n", gap8_dotp2(OutData[i], OutData[i])>>15);


}

int main()

{
	printf("FC Launched\n");
	Process();
	printf("Test done\n");
}
