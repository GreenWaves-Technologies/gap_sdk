#include "Gap8.h"
#include "../FFT_Lib/FFT_Lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "MFCC.h"

#define Q15	15

static short int HammingLUT[2*FRAME];
static short int TwiddlesLUT[N_FFT*2];
static short int SwapLUT[N_FFT];

void BuildHammingWindow(short int * __restrict__ HammingLUT, int FrameSize)

{
	unsigned int i;

  	for (i=0; i<FrameSize; i++) {
		HammingLUT[i] = FP2FIX(0.54 - 0.46 * cos((2.0 * M_PI / (FrameSize - 1)) * (i - 1)), Q15);
	}
}

void BuildLUT()

{
	int i;
	FILE *fi;

	fi = fopen("LUT.def", "w");

#ifdef HAMMING
	BuildHammingWindow(HammingLUT, FRAME);
	fprintf(fi, "short int HammingLUT[FRAME] = {\n\t");
	for (i=0; i<FRAME;i++) {
		fprintf(fi, "%d, ", HammingLUT[i]);
		if (((i+1)%12)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");
#endif

#ifdef TWIDDLES
	SetupTwiddlesLUT(TwiddlesLUT, N_FFT, 0);
	fprintf(fi, "short int TwiddlesLUT[2*(N_FFT/2)] = {\n\t");
	for (i=0; i<N_FFT/2;i++) {
		fprintf(fi, "%d, %d, ", TwiddlesLUT[2*i], TwiddlesLUT[2*i+1]);
		if (((i+1)%6)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");
#endif

#ifdef SWAP
	SetupR2SwapTable(SwapLUT, N_FFT);
	fprintf(fi, "short int SwapLUT[N_FFT] = {\n\t");
	for (i=0; i<N_FFT;i++) {
		fprintf(fi, "%d, ", SwapLUT[i]);
		if (((i+1)%13)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");
#endif

	fclose(fi);
}


void main()

{
	BuildLUT();
}
