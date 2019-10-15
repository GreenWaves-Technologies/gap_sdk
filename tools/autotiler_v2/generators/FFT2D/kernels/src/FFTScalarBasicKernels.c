/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Gap8.h"
#include "FFTBasicKernels.h"

#ifdef OLD_RUNTIME
#define wait_synch_barrier()          eu_bar_trig_wait_clr(eu_bar_addr(0))
#else
#define wait_synch_barrier()          rt_team_barrier()
#endif

/*

	Without input/output reordering:

	Radix 2 DIT FFT on 16 points:   488     Cycles, Stall Penalty: 7.0%
	Radix 2 DIF FFT on 16 points:   480     Cycles, Stall Penalty: 7.1%

	Radix 2 DIT FFT on 32 points:   1102    Cycles, Stall Penalty: 7.8%
	Radix 2 DIF FFT on 32 points:   1057    Cycles, Stall Penalty: 8.2%

	Radix 2 DIT FFT on 64 points:   2476    Cycles, Stall Penalty: 8.4%
	Radix 2 DIF FFT on 64 points:   2338    Cycles, Stall Penalty: 8.9%

	Radix 2 DIT FFT on 128 points:  5530    Cycles, Stall Penalty: 8.8%
	Radix 2 DIF FFT on 128 points:  5171    Cycles, Stall Penalty: 9.5%

	Radix 2 DIT FFT on 256 points:  12264   Cycles, Stall Penalty: 9.1%
	Radix 2 DIF FFT on 256 points:  11396   Cycles, Stall Penalty: 9.9%

	Radix 2 DIT FFT on 512 points:  26998   Cycles, Stall Penalty: 9.3%
	Radix 2 DIF FFT on 512 points:  24981   Cycles, Stall Penalty: 10.2%

	Radix 2 DIT FFT on 1024 points: 59012   Cycles, Stall Penalty: 9.5%
	Radix 2 DIF FFT on 1024 points: 54438   Cycles, Stall Penalty: 10.4%

	Radix 4 DIT FFT on 16 points:   319     Cycles, Stall Penalty: 0.3%
	Radix 4 DIF FFT on 16 points:   314     Cycles, Stall Penalty: 0.6%

	Radix 4 DIT FFT on 64 points:   1543    Cycles, Stall Penalty: 0.1%
	Radix 4 DIF FFT on 64 points:   1495    Cycles, Stall Penalty: 0.1%

	Radix 4 DIT FFT on 256 points:  7579    Cycles, Stall Penalty: 0.0%
	Radix 4 DIF FFT on 256 points:  7344    Cycles, Stall Penalty: 0.0%

	Radix 4 DIT FFT on 1024 points: 36463   Cycles, Stall Penalty: 0.0%
	Radix 4 DIF FFT on 1024 points: 35465   Cycles, Stall Penalty: 0.0%

	Assuming 16Khz PCM.

	1/4 overlap between 2 adjacent fft:

		N FFT	Cycles/FFT	Total Cycles	Mips
	64	333.33	1495.00		498333.33	0.50
	256	83.33	7344.00		612000.00	0.61
	1024	20.83	35465.00	738854.17	0.74


	Without cplx mult:

	Radix 2 DIT FFT on 16 points:   727,    Stall Penalty: 6.8%
	Radix 2 DIF FFT on 16 points:   743,    Stall Penalty: 6.6%

	Radix 2 DIT FFT on 32 points:   1709,   Stall Penalty: 6.9%
	Radix 2 DIF FFT on 32 points:   1728,   Stall Penalty: 6.8%

	Radix 2 DIT FFT on 64 points:   3947,   Stall Penalty: 6.9%
	Radix 2 DIF FFT on 64 points:   3969,   Stall Penalty: 6.8%

	Radix 2 DIT FFT on 128 points:  8985,   Stall Penalty: 6.8%
	Radix 2 DIF FFT on 128 points:  9010,   Stall Penalty: 6.8%

	Radix 2 DIT FFT on 256 points:  20199,  Stall Penalty: 6.8%
	Radix 2 DIF FFT on 256 points:  20227,  Stall Penalty: 6.7%

	Radix 2 DIT FFT on 512 points:  44917,  Stall Penalty: 6.7%
	Radix 2 DIF FFT on 512 points:  44948,  Stall Penalty: 6.7%

	Radix 2 DIT FFT on 1024 points: 98947,  Stall Penalty: 6.6%
	Radix 2 DIF FFT on 1024 points: 98981,  Stall Penalty: 6.6%

	Radix 4 DIT FFT on 16 points:   489,    Stall Penalty: 0.0%
	Radix 4 DIF FFT on 16 points:   498,    Stall Penalty: 1.4%

	Radix 4 DIT FFT on 64 points:   2698,   Stall Penalty: 0.0%
	Radix 4 DIF FFT on 64 points:   2702,   Stall Penalty: 0.9%

	Radix 4 DIT FFT on 256 points:  13955,  Stall Penalty: 0.0%
	Radix 4 DIF FFT on 256 points:  13906,  Stall Penalty: 0.7%

	Radix 4 DIT FFT on 1024 points: 68892,  Stall Penalty: 0.0%
	Radix 4 DIF FFT on 1024 points: 68598,  Stall Penalty: 0.5%


*/

#define Min(x, y)       (((x)<(y))?(x):(y))

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        NCore = gap8_ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;

}


/* Number of points for the Radix4 FFT */

/*
	Input samples are in Q<FFT4_SAMPLE_DYN>
	Coeff are in Q<FFT_TWIDDLE_DYN>

	Out samples in Q<FFT4_SAMPLE_DYN>
*/

static void Radix4FFTKernel_Twiddle0(short int *InOutA, short int *InOutB, short int *InOutC, short int *InOutD, unsigned int Inverse)

{
	short int Ar = InOutA[0];
	short int Ai = InOutA[1];
	short int Br = InOutB[0];
	short int Bi = InOutB[1];
	short int Cr = InOutC[0];
	short int Ci = InOutC[1];
	short int Dr = InOutD[0];
	short int Di = InOutD[1];

        int B1r, D1r;
        int B1i, D1i;

	B1r = Bi; B1i = -Br;
	D1r = Di; D1i = -Dr;

	if (Inverse) {
        	InOutA[0] = ((Ar + Cr) + (Br  + Dr ));
        	InOutA[1] = ((Ai + Ci) + (Bi  + Di ));
        	InOutB[0] = ((Ar - Cr) - (B1r - D1r));
        	InOutB[1] = ((Ai - Ci) - (B1i - D1i));
        	InOutC[0] = ((Ar + Cr) - (Br  + Dr ));
        	InOutC[1] = ((Ai + Ci) - (Bi  + Di ));
        	InOutD[0] = ((Ar - Cr) + (B1r - D1r));
        	InOutD[1] = ((Ai - Ci) + (B1i - D1i));
	} else {
        	InOutA[0] = ((Ar + Cr) + (Br  + Dr ));
        	InOutA[1] = ((Ai + Ci) + (Bi  + Di ));
        	InOutB[0] = ((Ar - Cr) + (B1r - D1r));
        	InOutB[1] = ((Ai - Ci) + (B1i - D1i));
        	InOutC[0] = ((Ar + Cr) - (Br  + Dr ));
        	InOutC[1] = ((Ai + Ci) - (Bi  + Di ));
        	InOutD[0] = ((Ar - Cr) - (B1r - D1r));
        	InOutD[1] = ((Ai - Ci) - (B1i - D1i));
	}
}

static void Radix4FFTKernel_Twiddle0_Scaled(short int *InOutA, short int *InOutB, short int *InOutC, short int *InOutD, unsigned int Inverse)

{
	short int Ar = InOutA[0];
	short int Ai = InOutA[1];
	short int Br = InOutB[0];
	short int Bi = InOutB[1];
	short int Cr = InOutC[0];
	short int Ci = InOutC[1];
	short int Dr = InOutD[0];
	short int Di = InOutD[1];

        int B1r, D1r;
        int B1i, D1i;

	B1r = Bi; B1i = -Br;
	D1r = Di; D1i = -Dr;

	if (Inverse) {
        	InOutA[0] = ((Ar + Cr) + (Br  + Dr )) >> FFT4_SCALEDOWN;
        	InOutA[1] = ((Ai + Ci) + (Bi  + Di )) >> FFT4_SCALEDOWN;
        	InOutB[0] = ((Ar - Cr) - (B1r - D1r)) >> FFT4_SCALEDOWN;
        	InOutB[1] = ((Ai - Ci) - (B1i - D1i)) >> FFT4_SCALEDOWN;
        	InOutC[0] = ((Ar + Cr) - (Br  + Dr )) >> FFT4_SCALEDOWN;
        	InOutC[1] = ((Ai + Ci) - (Bi  + Di )) >> FFT4_SCALEDOWN;
        	InOutD[0] = ((Ar - Cr) + (B1r - D1r)) >> FFT4_SCALEDOWN;
        	InOutD[1] = ((Ai - Ci) + (B1i - D1i)) >> FFT4_SCALEDOWN;
	} else {
        	InOutA[0] = ((Ar + Cr) + (Br  + Dr )) >> FFT4_SCALEDOWN;
        	InOutA[1] = ((Ai + Ci) + (Bi  + Di )) >> FFT4_SCALEDOWN;
        	InOutB[0] = ((Ar - Cr) + (B1r - D1r)) >> FFT4_SCALEDOWN;
        	InOutB[1] = ((Ai - Ci) + (B1i - D1i)) >> FFT4_SCALEDOWN;
        	InOutC[0] = ((Ar + Cr) - (Br  + Dr )) >> FFT4_SCALEDOWN;
        	InOutC[1] = ((Ai + Ci) - (Bi  + Di )) >> FFT4_SCALEDOWN;
        	InOutD[0] = ((Ar - Cr) - (B1r - D1r)) >> FFT4_SCALEDOWN;
        	InOutD[1] = ((Ai - Ci) - (B1i - D1i)) >> FFT4_SCALEDOWN;
	}
}


/*
   Inputs are in Q<FFT4_SAMPLE_DYN>, Twidlle factors are in Q<FFT_TWIDDLE_DYN>,
   Outputs are in Q<FFT4_SAMPLE_DYN> therefore we need to shift by <FFT_TWIDDLE_DYN> to get a Q<FFT4_SAMPLE_DYN> number
   Note that output could be > 1.0 and go up to 8.0
*/

static void Radix4FFTKernelDIT(short int *InOutA, short int *InOutB, short int *InOutC, short int *InOutD,
		     	       short int W1r, short int W1i, short int W2r, short int W2i, short int W3r, short int W3i, unsigned int Inverse)

{
	int Ar = InOutA[0];
	int Ai = InOutA[1];
	int Br = InOutB[0];
	int Bi = InOutB[1];
	int Cr = InOutC[0];
	int Ci = InOutC[1];
	int Dr = InOutD[0];
	int Di = InOutD[1];

        int B1r, B1i, C1r, C1i, D1r, D1i;
	int B2r, B2i, D2r, D2i;

	B1r = (Br*W1r - Bi*W1i)>>15; B1i = (Br*W1i + Bi*W1r)>>15;
	C1r = (Cr*W2r - Ci*W2i)>>15; C1i = (Cr*W2i + Ci*W2r)>>15;
	D1r = (Dr*W3r - Di*W3i)>>15; D1i = (Dr*W3i + Di*W3r)>>15;

	B2r = B1i; B2i = -B1r;
	D2r = D1i; D2i = -D1r;

	if (Inverse) {
        	InOutA[0] = ((Ar + C1r) + (B1r + D1r)) >> FFT4_SCALEDOWN;
        	InOutA[1] = ((Ai + C1i) + (B1i + D1i)) >> FFT4_SCALEDOWN;
        	InOutB[0] = ((Ar - C1r) - (B2r - D2r)) >> FFT4_SCALEDOWN;
        	InOutB[1] = ((Ai - C1i) - (B2i - D2i)) >> FFT4_SCALEDOWN;
        	InOutC[0] = ((Ar + C1r) - (B1r + D1r)) >> FFT4_SCALEDOWN;
        	InOutC[1] = ((Ai + C1i) - (B1i + D1i)) >> FFT4_SCALEDOWN;
        	InOutD[0] = ((Ar - C1r) + (B2r - D2r)) >> FFT4_SCALEDOWN;
        	InOutD[1] = ((Ai - C1i) + (B2i - D2i)) >> FFT4_SCALEDOWN;
	} else {
        	InOutA[0] = ((Ar + C1r) + (B1r + D1r)) >> FFT4_SCALEDOWN;
        	InOutA[1] = ((Ai + C1i) + (B1i + D1i)) >> FFT4_SCALEDOWN;
        	InOutB[0] = ((Ar - C1r) + (B2r - D2r)) >> FFT4_SCALEDOWN;
        	InOutB[1] = ((Ai - C1i) + (B2i - D2i)) >> FFT4_SCALEDOWN;
        	InOutC[0] = ((Ar + C1r) - (B1r + D1r)) >> FFT4_SCALEDOWN;
        	InOutC[1] = ((Ai + C1i) - (B1i + D1i)) >> FFT4_SCALEDOWN;
        	InOutD[0] = ((Ar - C1r) - (B2r - D2r)) >> FFT4_SCALEDOWN;
        	InOutD[1] = ((Ai - C1i) - (B2i - D2i)) >> FFT4_SCALEDOWN;
	}
}

static void Radix4FFTKernelDIF(short int *InOutA, short int *InOutB, short int *InOutC, short int *InOutD,
		     	       short int W1r, short int W1i, short int W2r, short int W2i, short int W3r, short int W3i, unsigned int Inverse)

{
	int Ar = InOutA[0];
	int Ai = InOutA[1];
	int Br = InOutB[0];
	int Bi = InOutB[1];
	int Cr = InOutC[0];
	int Ci = InOutC[1];
	int Dr = InOutD[0];
	int Di = InOutD[1];

        int A1r, A1i, B1r, B1i, C1r, C1i, D1r, D1i;
	int B2r, B2i, D2r, D2i, Tmp;

	B2r = Bi; B2i = -Br;
	D2r = Di; D2i = -Dr;

	if (Inverse) {
		A1r = ((Ar + Cr) + (Br  +  Dr));
		A1i = ((Ai + Ci) + (Bi  +  Di));
		B1r = ((Ar - Cr) - (B2r - D2r));
		B1i = ((Ai - Ci) - (B2i - D2i));
		C1r = ((Ar + Cr) - (Br  +  Dr));
		C1i = ((Ai + Ci) - (Bi  +  Di));
		D1r = ((Ar - Cr) + (B2r - D2r));
		D1i = ((Ai - Ci) + (B2i - D2i));
	} else {
		A1r = ((Ar + Cr) + (Br  +  Dr));
		A1i = ((Ai + Ci) + (Bi  +  Di));
		B1r = ((Ar - Cr) + (B2r - D2r));
		B1i = ((Ai - Ci) + (B2i - D2i));
		C1r = ((Ar + Cr) - (Br  +  Dr));
		C1i = ((Ai + Ci) - (Bi  +  Di));
		D1r = ((Ar - Cr) - (B2r - D2r));
		D1i = ((Ai - Ci) - (B2i - D2i));
	}

	Tmp = B1r; B1r = (B1r*W1r - B1i*W1i)>>15; B1i = (Tmp*W1i + B1i*W1r)>>15;
	Tmp = C1r; C1r = (C1r*W2r - C1i*W2i)>>15; C1i = (Tmp*W2i + C1i*W2r)>>15;
	Tmp = D1r; D1r = (D1r*W3r - D1i*W3i)>>15; D1i = (Tmp*W3i + D1i*W3r)>>15;

        InOutA[0] = A1r >> FFT4_SCALEDOWN;
        InOutA[1] = A1i >> FFT4_SCALEDOWN;
        InOutB[0] = B1r >> FFT4_SCALEDOWN;
        InOutB[1] = B1i >> FFT4_SCALEDOWN;
        InOutC[0] = C1r >> FFT4_SCALEDOWN;
        InOutC[1] = C1i >> FFT4_SCALEDOWN;
        InOutD[0] = D1r >> FFT4_SCALEDOWN;
        InOutD[1] = D1i >> FFT4_SCALEDOWN;
}

/*
   Radix-4 Decimated in Time FFT. Input have to be digitally-reversed, output is naturally ordered.
   First stage uses the fact that twiddles are all (1, 0)
*/

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIT_Scalar_Internal(signed short *__restrict__ Data,
							      		      signed short *__restrict__ Twiddles,
							      		      int N_FFT4, unsigned int Inverse)

{
    	int iCnt1, iCnt2, iCnt3;
        int iL,    iM,    iQ;
        int iA,    iB,    iC,     iD;
	int iLog4N  = gap8_fl1(N_FFT4)>>1;


	iL = N_FFT4 / 4; iM = 1;
	iA = 0;
	for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
		Radix4FFTKernel_Twiddle0_Scaled((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
				                (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), Inverse);
		iA =  iA + 4 * iM;
	}
	iL = iL >> 2;
	iM = iM << 2;
    	for (iCnt1 = 1; iCnt1 < iLog4N; ++iCnt1) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
			short int W1r = Twiddles[2*  iQ];
			short int W1i = Twiddles[2*  iQ + 1];
			short int W2r = Twiddles[2*2*iQ];
			short int W2i = Twiddles[2*2*iQ + 1];
			short int W3r = Twiddles[2*3*iQ];
			short int W3i = Twiddles[2*3*iQ + 1];
			iA = iCnt2;
			for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
				Radix4FFTKernelDIT((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
						   (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
						   W1r, W1i, W2r, W2i, W3r, W3i, Inverse);

				iA =  iA + 4 * iM;
			}
			iQ += iL;
		}
		iL = iL >> 2;
		iM = iM << 2;
	}
}

void Radix4FFT_DIT_Scalar(FFT_Arg_T *Arg)

{
        Radix4FFT_DIT_Scalar_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 0);
}

void Radix4FFT_DIT_Scalar_Inverse(FFT_Arg_T *Arg)

{
        Radix4FFT_DIT_Scalar_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 1);
}

/*
  Radix 4, Decimated in Frequency, fft.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIF_Scalar_Internal(signed short *__restrict__ Data,
									      signed short *__restrict__ Twiddles,
									      int N_FFT4, unsigned int Inverse)

{
    	int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
	int iLog4N  = gap8_fl1(N_FFT4)>>1;

    	iL = 1;
    	iM = N_FFT4 / 4;

    	for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
			short int W1r = Twiddles[2*  iQ];
			short int W1i = Twiddles[2*  iQ + 1];
			short int W2r = Twiddles[2*2*iQ];
			short int W2i = Twiddles[2*2*iQ + 1];
			short int W3r = Twiddles[2*3*iQ];
			short int W3i = Twiddles[2*3*iQ + 1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
				Radix4FFTKernelDIF((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
						   (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
						   W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
				iA = iA + 4 * iM;
            		}
            		iQ += iL;
        	}
        	iL <<= 2;
        	iM >>= 2;
    	}
	iA = 0; iL = (N_FFT4>>2);
	for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
		Radix4FFTKernel_Twiddle0((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
				         (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), Inverse);
		iA =  iA + 4 * iM;
	}
}

/*
  Radix 4, Decimated in Frequency, fft_2d: W=TileW, H=N_FFT4.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/
static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIF_Vertical_Scalar_Internal(	signed short *__restrict__ Data,
                                                                             		signed short *__restrict__ Twiddles,
                                                                             		int N_FFT4, unsigned int TileW,
                                                                           		unsigned int Inverse)

{
    	int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
	int iLog4N  = gap8_fl1(N_FFT4)>>1;

    	iL = 1;
    	iM = N_FFT4 / 4;

    	for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
			short int W1r = Twiddles[2*  iQ];
			short int W1i = Twiddles[2*  iQ + 1];
			short int W2r = Twiddles[2*2*iQ];
			short int W2i = Twiddles[2*2*iQ + 1];
			short int W3r = Twiddles[2*3*iQ];
			short int W3i = Twiddles[2*3*iQ + 1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
				Radix4FFTKernelDIF((Data + TileW*2*(iA       )), (Data + TileW*2*(iA +   iM)),
						   (Data + TileW*2*(iA + 2*iM)), (Data + TileW*2*(iA + 3*iM)),
						   W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
				iA = iA + 4 * iM;
            		}
            		iQ += iL;
        	}
        	iL <<= 2;
        	iM >>= 2;
    	}
	iA = 0; iL = (N_FFT4>>2);
	for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
		Radix4FFTKernel_Twiddle0((Data + TileW*2*(iA       )), (Data + TileW*2*(iA +   iM)),
				         (Data + TileW*2*(iA + 2*iM)), (Data + TileW*2*(iA + 3*iM)), Inverse);
		iA =  iA + 4 * iM;
	}
}

void Radix4FFT_DIF_Scalar(FFT_Arg_T *Arg)

{
        Radix4FFT_DIF_Scalar_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 0);
}

void Radix4FFT_DIF_Scalar_Inverse(FFT_Arg_T *Arg)

{
        Radix4FFT_DIF_Scalar_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 1);
}

/*
  Radix 2, Decimated in Frequency, fft.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/
void Radix2FFT_DIF_Scalar(FFT_Arg_T *Arg)

{
	signed short * __restrict__ Data = Arg->Data;
	signed short * __restrict__ Twiddles = Arg->Twiddles;
	int N_FFT2 = Arg->N_fft;

	int iLog2N  = gap8_fl1(N_FFT2);
	int iCnt1, iCnt2, iCnt3,
      	    iQ,    iL,    iM,
      	    iA,    iB;

    	iL = 1;
    	iM = N_FFT2 / 2;

    	for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
			short int Wr = Twiddles[2*iQ];
			short int Wi = Twiddles[2*iQ+1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
				int Tmpr, Tmpi;
                		iB = iA + iM;
				Tmpr = Data[2*iA  ] - Data[2*iB  ];
				Tmpi = Data[2*iA+1] - Data[2*iB+1];
				Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]) >> FFT2_SCALEDOWN;
				Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]) >> FFT2_SCALEDOWN;

				Data[2*iB  ] = (Tmpr*Wr - Tmpi*Wi)>>(15+FFT2_SCALEDOWN);
				Data[2*iB+1] = (Tmpr*Wi + Tmpi*Wr)>>(15+FFT2_SCALEDOWN);

                		iA = iA + 2 * iM;
            		}
            		iQ += iL;
        	}
        	iL <<= 1;
        	iM >>= 1;
    	}
        iA = 0;
	/* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
		int Tmpr, Tmpi;
               	iB = iA + 1;
		Tmpr = Data[2*iA  ] - Data[2*iB  ];
		Tmpi = Data[2*iA+1] - Data[2*iB+1];
		Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
		Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
		Data[2*iB  ] = Tmpr;
		Data[2*iB+1] = Tmpi;
               	iA = iA + 2;
        }
}

/*
  Radix 2, Decimated in Frequency, fft. Internal version, used for FFT 2D horizontal part
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/
static inline void __attribute__ ((__always_inline__)) Radix2FFT_DIF_Scalar_Internal(signed short *__restrict__ Data,
                                                                       	      signed short *__restrict__ Twiddles,
                                                                       	      int N_FFT2)

{
	int iLog2N  = gap8_fl1(N_FFT2);
	int iCnt1, iCnt2, iCnt3,
      	    iQ,    iL,    iM,
      	    iA,    iB;

    	iL = 1;
    	iM = N_FFT2 / 2;

    	for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
			short int Wr = Twiddles[2*iQ];
			short int Wi = Twiddles[2*iQ+1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
				int Tmpr, Tmpi;
                		iB = iA + iM;
				Tmpr = Data[2*iA  ] - Data[2*iB  ];
				Tmpi = Data[2*iA+1] - Data[2*iB+1];
				Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]) >> FFT2_SCALEDOWN;
				Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]) >> FFT2_SCALEDOWN;

				Data[2*iB  ] = (Tmpr*Wr - Tmpi*Wi)>>(15+FFT2_SCALEDOWN);
				Data[2*iB+1] = (Tmpr*Wi + Tmpi*Wr)>>(15+FFT2_SCALEDOWN);

                		iA = iA + 2 * iM;
            		}
            		iQ += iL;
        	}
        	iL <<= 1;
        	iM >>= 1;
    	}
        iA = 0;
	/* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
		int Tmpr, Tmpi;
               	iB = iA + 1;
		Tmpr = Data[2*iA  ] - Data[2*iB  ];
		Tmpi = Data[2*iA+1] - Data[2*iB+1];
		Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
		Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
		Data[2*iB  ] = Tmpr;
		Data[2*iB+1] = Tmpi;
               	iA = iA + 2;
        }
}

/*
  Radix 2, Decimated in Frequency, fft 2D, vertical part.
  TileW is the width of the tile currently in memory
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/

static inline void __attribute__ ((__always_inline__)) Radix2FFT_DIF_Vertical_Scalar_Internal(signed short *__restrict__ Data,
                                                                                       signed short *__restrict__ Twiddles,
                                                                                       int N_FFT2, unsigned int TileW)

{
	int iLog2N  = gap8_fl1(N_FFT2);
	int iCnt1, iCnt2, iCnt3,
      	    iQ,    iL,    iM,
      	    iA,    iB;

    	iL = 1;
    	iM = N_FFT2 / 2;

    	for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
			short int Wr = Twiddles[2*iQ];
			short int Wi = Twiddles[2*iQ+1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
				int Tmpr, Tmpi;
                		iB = iA + iM;
				Tmpr = Data[TileW*2*iA    ] - Data[TileW*2*iB    ];
				Tmpi = Data[TileW*(2*iA+1)] - Data[TileW*(2*iB+1)];
				Data[TileW*2*iA    ] = (Data[TileW*2*iA    ] + Data[TileW*2*iB    ]) >> FFT2_SCALEDOWN;
				Data[TileW*(2*iA+1)] = (Data[TileW*(2*iA+1)] + Data[TileW*(2*iB+1)]) >> FFT2_SCALEDOWN;

				Data[TileW*2*iB    ] = (Tmpr*Wr - Tmpi*Wi)>>(15+FFT2_SCALEDOWN);
				Data[TileW*(2*iB+1)] = (Tmpr*Wi + Tmpi*Wr)>>(15+FFT2_SCALEDOWN);

                		iA = iA + 2 * iM;
            		}
            		iQ += iL;
        	}
        	iL <<= 1;
        	iM >>= 1;
    	}
        iA = 0;
	/* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
		int Tmpr, Tmpi;
               	iB = iA + 1;
		Tmpr = Data[TileW*2*(iA)  ] - Data[TileW*(2*iB)  ];
		Tmpi = Data[TileW*2*(iA+1)] - Data[TileW*(2*iB+1)];
		Data[TileW*(2*iA)  ] = (Data[TileW*(2*iA  )] + Data[TileW*(2*iB)  ]);
		Data[TileW*(2*iA+1)] = (Data[TileW*(2*iA+1)] + Data[TileW*(2*iB+1)]);
		Data[TileW*(2*iB)  ] = Tmpr;
		Data[TileW*(2*iB+1)] = Tmpi;
               	iA = iA + 2;
        }
}


/*
   Radix-2 Decimated in Time FFT. Input have to be digitally-reversed, output is naturally ordered.
   First stage uses the fact that twiddles are all (1, 0)
*/
void Radix2FFT_DIT_Scalar(FFT_Arg_T *Arg)

{
	signed short * __restrict__ Data = Arg->Data;
	signed short * __restrict__ Twiddles = Arg->Twiddles;
	int N_FFT2 = Arg->N_fft;
	int iLog2N  = gap8_fl1(N_FFT2);
	int iCnt1, iCnt2, iCnt3,
      	    iQ,    iL,    iM,
      	    iA,    iB;

    	iL = N_FFT2 >> 1; iM = 1; iA = 0; iQ = 0;
	/* First Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
		int Tmpr, Tmpi;
		iB = iA + iM;
		Tmpr = Data[2*iB];
		Tmpi = Data[2*iB+1];
		Data[2*iB  ] = (Data[2*iA  ] - Tmpr);
		Data[2*iB+1] = (Data[2*iA+1] - Tmpi);
		Data[2*iA  ] = (Data[2*iA  ] + Tmpr);
		Data[2*iA+1] = (Data[2*iA+1] + Tmpi);
               	iA = iA + 2;
	}
        iQ += iL; iL >>= 1; iM <<= 1;

    	for (iCnt1 = 1; iCnt1 < iLog2N; ++iCnt1) {
        	iQ = 0;
        	for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
			short int Wr = Twiddles[2*iQ  ];
			short int Wi = Twiddles[2*iQ+1];
            		iA = iCnt2;
            		for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
				int Tmpr, Tmpi;
                		iB = iA + iM;
				Tmpr = (Data[2*iB  ]*Wr - Data[2*iB+1]*Wi)>>15;
				Tmpi = (Data[2*iB  ]*Wi + Data[2*iB+1]*Wr)>>15;
				Data[2*iB  ] = (Data[2*iA  ] - Tmpr) >> (FFT2_SCALEDOWN);
				Data[2*iB+1] = (Data[2*iA+1] - Tmpi) >> (FFT2_SCALEDOWN);
				Data[2*iA  ] = (Data[2*iA  ] + Tmpr) >> (FFT2_SCALEDOWN);
				Data[2*iA+1] = (Data[2*iA+1] + Tmpi) >> (FFT2_SCALEDOWN);
                		iA = iA + 2 * iM;
            		}
            		iQ += iL;
        	}
        	iL >>= 1;
        	iM <<= 1;
    	}
}

/* 2D FFT */
/* Transform a W x H image (byte) into a W x H Q15 complex plane */

/* Horizontal part of a Radix4 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
void Radix4FFT_DIF_2D_Scalar_Horizontal(FFT_2D_Arg_T *Arg)

{
        int i;
        signed short int * __restrict__ In = Arg->Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap8_coreid();
        Chunk = ChunkSize(Arg->Nb);
        First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

        // for (i=0; i<Arg->Nb; i++) {
        for (i=First; i<Last; i++) {
                Radix4FFT_DIF_Scalar_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft, 0);
                // In += 2*Arg->N_fft;
        }
        wait_synch_barrier();
}

/* Vertical part of a Radix4 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
void Radix4FFT_DIF_2D_Scalar_Vertical(FFT_2D_Arg_T *Arg)

{
        int i;
        signed short int * __restrict__ In = Arg->Data;

        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap8_coreid();
        Chunk = ChunkSize(Arg->Nb);
        First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

        // for (i=0; i<Arg->Nb; i++) {
        for (i=First; i<Last; i++) {
                Radix4FFT_DIF_Vertical_Scalar_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb, 0);
                // In += 2;
        }
        wait_synch_barrier();
}

/* Horizontal part of a Radix2 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
void Radix2FFT_DIF_2D_Scalar_Horizontal(FFT_2D_Arg_T *Arg)

{
        int i;
        signed short int * __restrict__ In = Arg->Data;

        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap8_coreid();
        Chunk = ChunkSize(Arg->Nb);
        First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

        // for (i=0; i<Arg->Nb; i++) {
        for (i=First; i<Last; i++) {
                Radix2FFT_DIF_Scalar_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft);
                // In += 2*Arg->N_fft;
        }
        wait_synch_barrier();
}

/* Vertical part of a Radix2 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
void Radix2FFT_DIF_2D_Scalar_Vertical(FFT_2D_Arg_T *Arg)

{
        int i;
        signed short int * __restrict__ In = Arg->Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap8_coreid();
        Chunk = ChunkSize(Arg->Nb);
        First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

        // for (i=0; i<Arg->Nb; i++) {
        for (i=First; i<Last; i++) {
                Radix2FFT_DIF_Vertical_Scalar_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb);
                // In += 2;
        }
        wait_synch_barrier();
}


#ifdef BUILD_LUT
void SetupScalarTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse)

{
        int i;

        /* Radix 4: 3/4 of the twiddles
           Radix 2: 1/2 of the twiddles
        */

        if (Inverse) {
                float Theta = (2*M_PI)/Nfft;
                for (i=0; i<Nfft; i++) {
                        float Phi = Theta*i;
                        Twiddles[2*i  ] = (short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                        Twiddles[2*i+1] = (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                }
        } else {
                float Theta = (2*M_PI)/Nfft;
                for (i=0; i<Nfft; i++) {
                        float Phi = Theta*i;
                        Twiddles[2*i  ] = (short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                        Twiddles[2*i+1] = (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                }
        }
}
#endif
