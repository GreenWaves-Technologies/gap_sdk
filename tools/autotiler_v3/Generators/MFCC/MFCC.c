#include <stdio.h>
#include <math.h>
#include "Gap8.h"
#include "../FFT_Lib/FFT_Lib.h"
#include "../FFT_Lib/TwiddlesDef.h"
#include "../FFT_Lib/SwapTablesDef.h"
#include "MFCC.h"
#ifdef DUMP
#include "MFCC_Dump.h"
#endif
#include "MFCC_Display.h"
#include "LUT.def"
#include "MFCC_FB40.def"

#define Q15			15
#define Q12			12
#define Q6			6

#ifdef FROM_FILE
#include <sndfile.h>
#include "ReadInput.h"
short int DataIn[FRAME];
#else
#include "DataIn.def"
#endif
short int *InSignal = DataIn;
v2s 	  W_Frame[N_FFT];
short int FramePower[HALF_FFT];
short int Frame[FRAME];

short int MFCC   [7*(MFCC_BANK_CNT)];
short int MFCC_D1[7*(MFCC_BANK_CNT)];
short int MFCC_D2[7*(MFCC_BANK_CNT)];

short int *MFCC_Group   [7] = {MFCC,    			MFCC+1*(MFCC_BANK_CNT),		MFCC+2*(MFCC_BANK_CNT),		MFCC+3*(MFCC_BANK_CNT),
			       MFCC+4*(MFCC_BANK_CNT),		MFCC+4*(MFCC_BANK_CNT),		MFCC+5*(MFCC_BANK_CNT)};

short int *MFCC_D1_Group[7] = {MFCC_D1,    			MFCC_D1+1*(MFCC_BANK_CNT),	MFCC_D1+2*(MFCC_BANK_CNT),	MFCC_D1+3*(MFCC_BANK_CNT),
			       MFCC_D1+4*(MFCC_BANK_CNT),	MFCC_D1+4*(MFCC_BANK_CNT),	MFCC_D1+5*(MFCC_BANK_CNT)};

short int *MFCC_D2_Group[7] = {MFCC_D2,    			MFCC_D2+1*(MFCC_BANK_CNT),	MFCC_D2+2*(MFCC_BANK_CNT),	MFCC_D2+3*(MFCC_BANK_CNT),
			       MFCC_D2+4*(MFCC_BANK_CNT),	MFCC_D2+4*(MFCC_BANK_CNT),	MFCC_D2+5*(MFCC_BANK_CNT)};


static void PreEmphasis(short int * __restrict__ Frame, short int * __restrict__ Out, int FrameSize)

{
	static int Active = 0;
	static int Gain = 2;
	unsigned int i;
	int S = 0; // Frame[0];

	// Y[n]=X[n]−0.95⋅X[n−1]

  	for (i=0; i<FrameSize; i++) {
		if (Active) {
			Out[i] = Frame[i] - gap8_mulsRN(FP2FIX(0.95, Q15), S, Q15);
			S = Frame[i];
		} else {
			Out[i] = Frame[i]<<Gain;
		}
		
	}
}

static void WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame,
		          short int *__restrict__ Window, int FrameSize, int FFT_Dim)

{
	unsigned int i;

  	for (i=0; i<FrameSize; i++) {
		/* Frame and Window are Q15, we want OutFrame with precision FFT2_SAMPLE_DYN */
		OutFrame[i] = (v2s) {gap8_mulsRN(Frame[i], Window[i], Q15+(Q15-FFT2_SAMPLE_DYN)), 0};
	}
	for (i=FrameSize; i<FFT_Dim;i++) OutFrame[i] = (v2s) {0, 0};
}

#define	INVSQRT2	FP2FIX(0.707106781, FFT2_SAMPLE_DYN)

static void EstimatePower(v2s *__restrict__ FrameIn, short int *__restrict__ Power, int N, int N_fft)

{
	unsigned int i;
	unsigned int Log2N_fft = gap8_fl1(N_fft);
	static int Margin = 8;
	

	for (i=0; i<N; i++) {
		int P = gap8_dotp2(FrameIn[i], FrameIn[i])>>(FFT2_SAMPLE_DYN-Margin);
		Power[i] = P; // gap8_mulsRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
		/* Power[i] = P>>5; //>>Log2N_fft; */
/*
		if (Power[i]<0) {
			printf("OVERFLOW, P= %X, %d, Precision: %d\n", P, gap8_fl1(P), FFT2_SAMPLE_DYN);
			printf("\t[%f, %f]^2, [%X, %X] ^2:[%X, %X], Sum: %X\n",
				FIX2FP(FrameIn[i][0], FFT2_SAMPLE_DYN), FIX2FP(FrameIn[i][1], FFT2_SAMPLE_DYN),
				FrameIn[i][0], FrameIn[i][1],
				FrameIn[i][0]*FrameIn[i][0], FrameIn[i][1]*FrameIn[i][1],
				(FrameIn[i][0]*FrameIn[i][0], FrameIn[i][1]*FrameIn[i][1])>>FFT2_SAMPLE_DYN);
			printf("\tRe^2 = %f [RN:%X, N:%X], Im^2 = %f [RN:%X, N:%X]\n",
				FIX2FP(gap8_mulsRN(FrameIn[i][0], FrameIn[i][0], FFT2_SAMPLE_DYN), FFT2_SAMPLE_DYN),
				gap8_mulsRN(FrameIn[i][0], FrameIn[i][0], FFT2_SAMPLE_DYN),
				gap8_mulsN(FrameIn[i][0], FrameIn[i][0], FFT2_SAMPLE_DYN),
				FIX2FP(gap8_mulsRN(FrameIn[i][1], FrameIn[i][1], FFT2_SAMPLE_DYN), FFT2_SAMPLE_DYN),
				gap8_mulsRN(FrameIn[i][1], FrameIn[i][1], FFT2_SAMPLE_DYN),
				gap8_mulsN(FrameIn[i][1], FrameIn[i][1], FFT2_SAMPLE_DYN));
		}
*/
	}
}

static void ComputeMFCC_Derivative(short int *MFCC_Group[], short int *MFCC_D)

{
	unsigned int i;
	int FilterBanks = MFCC_BANK_CNT;
	short int *MFCC_N  = (MFCC_Group[0]);
	short int *MFCC_NN = (MFCC_Group[1]);
	short int *MFCC_P  = (MFCC_Group[3]);
	short int *MFCC_PP = (MFCC_Group[4]);

	/* Compute derivative:
		Deriv(i) = Sum[n=1..N, n*(MFCC(i+n) - MFCC(i-1))] / 2*(Sum[n=1..N, n*n])
		We use N=2

	   MFCC is in FFT2_SAMPLE_DYN
	   MFCC_D is in FFT2_SAMPLE_DYN
	*/
	for (i=0; i<FilterBanks; i++) {
		int Delta;
		Delta = ( MFCC_N [i] - MFCC_P [i]) +
			((MFCC_NN[i] - MFCC_PP[i])<<1);
		Delta = gap8_mulsRN(Delta, MFCC_DERIV_COEFF, MFCC_DERIV_COEFF_DYN);
		MFCC_D[i] = Delta;
	}
}


static void ComputeMFCC(short int *__restrict__ FramePower, short int *__restrict__ MFCC)

{
	unsigned int i, j, k;
	int FilterBanks = MFCC_BANK_CNT;

	/*
		FramePower is in Q FFT2_SAMPLE_DYN	13
		MFCC_Coeffs is in MFCC_COEFF_DYN	10
		MFCC is in FFT2_SAMPLE_DYN		13
	*/

	for (i=0; i<FilterBanks; i++) {
		int Coeff = 0;
		int Base = MFCC_FilterBank[i].Base;
		int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);

		for (k=0, j=MFCC_FilterBank[i].Start; j<(MFCC_FilterBank[i].Start+Count); j++, k++) {
			Coeff = Coeff + MFCC_Coeffs[Base+k]*FramePower[j];
		}
		Coeff = Coeff >> MFCC_COEFF_DYN;
		MFCC[i] = (Coeff*MFCC_FilterBank[i].Norm)>>(MFCC_COEFF_DYN);
		// MFCC[i] = Coeff>>MFCC_COEFF_DYN;
	}
}


void main()

{
#define Max(a, b)	(((a)>=(b))?(a):(b))
	int i, j, k, Last;
	int FrameCount = 0;
	int FilterBanks = MFCC_BANK_CNT;
	FFT_Arg_T F_Arg = {(signed short *) W_Frame, R2_Twiddles_512, N_FFT};
	SwapSamples_Arg_T S_Arg = {W_Frame, R2_SwapTable_512, N_FFT};

	// DumpShortInt("Hamming", HammingLUT, FRAME, 0, Q15);
#ifdef FROM_FILE
	OpenSamplesFile("file.wav");
	GetNextSample(InSignal, FRAME);
	i=0;
	do {
//		DumpShortInt("Input", InSignal, FRAME, FrameCount, Q15);
		PreEmphasis(InSignal, Frame, FRAME);
#else
	for (i=0; i<(sizeof(DataIn)/(2*sizeof(short int))); i+=FRAME_STEP) {
		// DumpShortInt("Input", &InSignal[i], FRAME, FrameCount, Q15);

		PreEmphasis(&InSignal[i], Frame, FRAME);
#endif
		DumpShortInt("Premphasis Input", Frame, FRAME, FrameCount, Q15);

		WindowedFrame(Frame, W_Frame, HammingLUT, FRAME, N_FFT);
		DumpComplex("Windowed", W_Frame, N_FFT, FrameCount, FFT2_SAMPLE_DYN);

		Radix2FFT_DIF(&F_Arg);
		SwapSamples(&S_Arg);
		DumpComplex("FFT", W_Frame, N_FFT, FrameCount, FFT2_SAMPLE_DYN);

		EstimatePower(W_Frame, FramePower, HALF_FFT, N_FFT);
		DumpShortInt("Power", FramePower, HALF_FFT, FrameCount, FFT2_SAMPLE_DYN);

		ComputeMFCC(FramePower, MFCC_Group[6]);
		Last = Max(0, 6-FrameCount);
		for (j=0; j<Last; j++) {
			for (k=0; k<MFCC_BANK_CNT; k++) (MFCC_Group[j])[k] = (MFCC_Group[Last])[k];
		}
		DumpShortInt("MFCC", MFCC_Group[6], MFCC_BANK_CNT, FrameCount, FFT2_SAMPLE_DYN);

		ComputeMFCC_Derivative(MFCC_Group, MFCC_D1_Group[4]);
		Last = Max(0, 4-FrameCount);
		for (j=0; j<Last; j++) {
			for (k=0; k<MFCC_BANK_CNT; k++) (MFCC_D1_Group[j])[k] = (MFCC_D1_Group[Last])[k];
		}
		DumpShortInt("MFCC Delta      ", MFCC_D1_Group[4], MFCC_BANK_CNT, FrameCount, FFT2_SAMPLE_DYN);

		ComputeMFCC_Derivative(MFCC_D1_Group, MFCC_D2_Group[2]);
		DumpShortInt("MFCC Delta Delta", MFCC_D2_Group[2], MFCC_BANK_CNT, FrameCount, FFT2_SAMPLE_DYN);

		{
			short int *T    = MFCC_Group[0];
			short int *T_D1 = MFCC_D1_Group[0];
			short int *T_D2 = MFCC_D2_Group[0];

			PushFrame(T, T_D1, T_D2, 0, FFT2_SAMPLE_DYN);
			DumpShortInt("Pushed MFCC ", T, MFCC_BANK_CNT, FrameCount, FFT2_SAMPLE_DYN);
			for (j=0; j<6; j++) {
				MFCC_Group   [j] = MFCC_Group   [j+1];
				MFCC_D1_Group[j] = MFCC_D1_Group[j+1];
				MFCC_D2_Group[j] = MFCC_D2_Group[j+1];
			}
			MFCC_Group   [6] = T;
			MFCC_D1_Group[6] = T_D1;
			MFCC_D2_Group[6] = T_D2;
		}

		FrameCount++;
#ifdef FROM_FILE
		for (j=FRAME_STEP; j<FRAME; j++) InSignal[j-FRAME_STEP] = InSignal[j];
		i+=FRAME_STEP;
	} while (GetNextSample(&InSignal[FRAME-FRAME_STEP], FRAME_STEP));
#else
	}
#endif
	for (k=0; k<MFCC_BANK_CNT; k++) {

		(MFCC_D1_Group[6])[k] = (MFCC_D1_Group[4])[k];
		(MFCC_D1_Group[5])[k] = (MFCC_D1_Group[4])[k];

		(MFCC_D2_Group[6])[k] = (MFCC_D2_Group[2])[k];
		(MFCC_D2_Group[5])[k] = (MFCC_D2_Group[2])[k];
		(MFCC_D2_Group[4])[k] = (MFCC_D2_Group[2])[k];
		(MFCC_D2_Group[3])[k] = (MFCC_D2_Group[2])[k];
	}
	
	PushFrame(0, 0, 0, 1, FFT2_SAMPLE_DYN);
#undef Max
}
