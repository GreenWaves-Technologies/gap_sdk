/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifdef BUILD_LUT
#include <stdlib.h>
#include <math.h>
#endif

#include "Gap.h"
#include "FFTLib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#if (FP==1)
#define TYPE_GENERIC complex_fp
#define RADIX4KERNEL Radix4FFTKernelDIF_FP
#define RADIX4KERNEL_TWID0 Radix4FFTKernel_Twiddle0_FP
#elif (FP==2)
#define TYPE_GENERIC v2h
#define RADIX4KERNEL Radix4FFTKernelDIF_FP16
#define RADIX4KERNEL_TWID0 Radix4FFTKernel_Twiddle0_FP16
#else
#define TYPE_GENERIC v2s
#define RADIX4KERNEL Radix4FFTKernelDIF
#define RADIX4KERNEL_TWID0 Radix4FFTKernel_Twiddle0
#endif

#ifndef abs
#define abs(a)  (((a)<0) ? (-(a)) : (a))
#endif
#ifndef Min
#define Min(x, y)       (((x)<(y))?(x):(y))
#endif
#define F2Q(V, N)       ((int) roundf(((float) (V))*((1<<(N))-0)))

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
  unsigned int NCore;
  unsigned int Log2Core;
  unsigned int Chunk;

  NCore = gap_ncore();
  Log2Core = gap_fl1(NCore);
  Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
  return Chunk;

}

void printout_scale(void *Data, int N, int scale)
{
  int i;
  char Outr[10], Outi[10];
  int Prec = 10;
  for(i=0;i<2*N;i+=2) {

#if (FP==1)
    __FP(F2Q(((float *) Data)[i], Prec), Prec, Outr);
    __FP(F2Q(((float *) Data)[i+1], Prec), Prec, Outi);
    printf("%d %s %s\n",i, Outr,Outi);
#elif (FP==2)
    __FP(F2Q((float)((float16 *) Data)[i], Prec), Prec, Outr);
    __FP(F2Q((float)((float16 *) Data)[i+1], Prec), Prec, Outi);
    printf("%d %s %s\n",i, Outr,Outi);
#else
    printf("%d %d %d\n",i,((short int*) Data)[i]*scale,((short int*) Data)[i+1]*scale);
#endif

  }


}

void printout(void *Data, int N)
{
  int i;
  char Outr[10], Outi[10];
  int Prec = 10;
  for(i=0;i<2*N;i+=2) {

#if (FP==1)
    __FP(F2Q(((float *) Data)[i], Prec), Prec, Outr);
    __FP(F2Q(((float *) Data)[i+1], Prec), Prec, Outi);
    printf("%d %s %s\n",i, Outr,Outi);
#elif (FP==2)
    __FP(F2Q((float)((float16 *) Data)[i], Prec), Prec, Outr);
    __FP(F2Q((float)((float16 *) Data)[i+1], Prec), Prec, Outi);
    printf("%d %s %s\n",i, Outr,Outi);
#else
    printf("%d %d %d\n",i,((short int*) Data)[i],((short int*) Data)[i+1]);
#endif

  }


}

#if (FP==2)
float A=0.0, B=0.0;
void Dmp_twid(float16 *Twid, int N, int size)

{
	int i;
        float Theta = (2*(float)M_PI)/N;
	char CP[10], SP[10];
	char C16P[10], S16P[10];
	char EcP[10], EsP[10];
	int Prec = 10;
	for (i=0; i<size; i++) {
        	float Phi = Theta*i;
		float C =  cosf(Phi), S =  sinf(-Phi);
		float Ec = Twid[2*i]-C;
		float Es = Twid[2*i+1]-S;
		float C16 = Twid[2*i]-A;
		float S16 = Twid[2*i+1]-B;
		__FP(F2Q(C, Prec), Prec, CP);
		__FP(F2Q(S, Prec), Prec, SP);
		__FP(F2Q(C16, Prec), Prec, C16P);
		__FP(F2Q(S16, Prec), Prec, S16P);
		__FP(F2Q(Ec, Prec), Prec, EcP);
		__FP(F2Q(Es, Prec), Prec, EsP);

		printf("%d, Cos: %s, Sin: %s,[fp16] %s %s  Error: %s, %s\n", i, CP, SP, C16P, S16P, EcP, EsP);
	}
}

v2h cplxmul_fp16(v2h A, v2h B) {


  v2h tmp;

  tmp[0] = A[0] * B[0] -  A[1] * B[1];
  tmp[1] = A[0] * B[1] +  A[1] * B[0];

  return tmp;

}

static inline v2h cplxmul_fp16_1(v2h A, v2h B)

{
        v2h P0, P1, P2, P3;

        P0 = A*B;
        B = __builtin_shuffle(B, (v2s){1,0});
        P1 = A*B;
        P2 = __builtin_shuffle(P0, P1, (v2s){0,2});
        P3 = __builtin_shuffle(P0, P1, (v2s){1,3});
        P3 = P3 * (v2h) {-1.0, 1.0};
        return P3;
}
void SetupTwiddlesLUT_f16(float16 *Twiddles, float *Twiddlesf, int N)

{
	int i;
	v2h *P_Twid = (v2h *) Twiddles;

	/* Radix 4: 3/4 of the twiddles
	Radix 2: 1/2 of the twiddles
	*/

	for (i=0; i<2*N; i++) {
	  float16 Ct16 =  (float16) Twiddlesf[2*i], St16 =  (float16) Twiddlesf[2*i+1];
	  P_Twid[i] = (v2h) {Ct16, St16};
	}
}
static void Radix4FFTKernelDIF_FP16(v2h *InOutA, v2h *InOutB, v2h *InOutC, v2h *InOutD,
    v2h W1, v2h W2, v2h W3, unsigned int Inverse)

{
  v2h A1, B1, C1, D1;
  v2h A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;
  //v2h rotmjB = B-D;

  //rotmjB = __builtin_shuffle(rotmjB, (v2s){1, 0});


  v2h rotmjB = { B[1] , -B[0] };
  v2h rotmjD = { -D[1] , D[0] };

  if (Inverse) {
    A1 = ((A + C) +               (B + D));
    B1 = ((A - C) - (rotmjB + rotmjD) );
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) + (rotmjB  + rotmjD) );
  } else {
    A1 = ((A + C) +               (B + D));
    B1 = ((A - C) + (rotmjB + rotmjD)  );
    //B1 = ((A - C) + rotmjB  );
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) - (rotmjB + rotmjD)  );
    //D1 = ((A - C) - rotmjB  );
  }

    B1 =  (v2h) cplxmul_fp16(B1, W1); C1 = (v2h) cplxmul_fp16(C1, W2); D1 = (v2h) cplxmul_fp16(D1, W3);

  *InOutA = A1; *InOutB = C1; *InOutC = B1; *InOutD = D1;
}
// produce a bit reversed output (swap B an C)
static void Radix4FFTKernel_Twiddle0_FP16(v2h *InOutA, v2h *InOutB, v2h *InOutC, v2h *InOutD, unsigned int Inverse)

{
  v2h A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  /* Used for IFFT here.
     IFFT uses (1,  1,  1,  1)  FFT uses (1,  1,  1,  1)
     (1,  j, -1, -j)	       (1, -j, -1,  j)
     (1, -1,  1, -1)	       (1, -1,  1, -1)
     (1, -j, -1,  j)	       (1,  j, -1, -j)
     To use this code for FFT:
   *InOutA = ((A + C) +	           (B + D ));
   *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
   */

  v2h rotmjB = { B[1] , -B[0] };
  v2h rotmjD = { -D[1] , D[0] };

  if (!Inverse) {
    *InOutA = ((A + C) +               (B + D));
    *InOutD = ((A - C) - (rotmjB +  rotmjD));
    *InOutB = ((A + C) -               (B + D));
    *InOutC = ((A - C) + (rotmjB +  rotmjD));
  } else {
    *InOutA = ((A + C) +               (B + D));
    *InOutD = ((A - C) + (rotmjB  + rotmjD));
    *InOutB = ((A + C) -               (B + D));
    *InOutC = ((A - C) - (rotmjB  + rotmjD));
  }

}
// cluster compatible  version of the mixed radix FFT vectorized, fixed point
// with 8 cores, size must be 128 min (32 will produce stages withi M=4 and iL=4 => max 4 cores))
void RadixMixedFFT_DIF_Par_FP16(FFT_Arg_T *Arg)

{
  unsigned int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;

  // we assume N_FFT is not a power of 4=> divide by 2 to get power of 4
  // N_FFT must be > 64 to operate on 8 cores
  unsigned int N_fft = Arg->N_fft;
  unsigned int Inverse = Arg-> Inverse;
  unsigned int N_FFTDIV = N_fft>>1;
  unsigned int iLog4N  = (gap_fl1(N_FFTDIV))>>1;
  v2h *DataV  = (v2h *) Arg->Data;
  v2h *CoeffV = (v2h *) Arg->Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};

  int i;
  CoreId = gap_coreid();
  int GAP8_NCORE = gap_ncore();
  //printf("log4n %d\n",iLog4N);
  // Layers 0,1, ... , (iLog4N-2)
  if (CoreId==0) {
    printf("input stage0 inv %d\n",Inverse);
    printout((void*)DataV, N_fft);
  }
  gap_waitbarrier(0);
  iM = N_fft >> 2; iL = 1;iQ=0;
  for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
    Chunk = (iM/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iQ    = First*iL;
    for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
      iA = iCnt2;
      v2h W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
	//printf("0 %d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
        Radix4FFTKernelDIF_FP16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA + iM),
            (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM),
            W1, W2, W3, Inverse);
        iA = iA + 4 * iM;
      }
      iQ += iL;
    }
    iL <<= 2; iM >>= 2;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);

#ifdef DBGFFT
    if (CoreId==0) {
      printf("stage %d\n",iCnt1);
      printout((void*)DataV, N_fft);
    }
    gap_waitbarrier(0);
#endif

  }

  // This stage will work for nfft > 32 (il=4 im=4) for 8 cores
 #ifdef FFTRMexec
  // last radix4 Layer iLog4N - 1
  iM = 2; iL = (N_fft>>3); iQ = 0;
#endif
#ifdef FFTR4exec
  // last radix4 Layer iLog4N - 2
  iM = 4; iL = (N_fft>>4); iQ = 0;
#endif


  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    v2h W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
    Chunk = (iL/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      //printf("1 %d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
      Radix4FFTKernelDIF_FP16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA + iM),
          (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM),
          W1, W2, W3, Inverse);
      iA = iA + 4 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);

  }

#ifdef DBGFFT
  if (CoreId==0) {
    printf("**stage %d\n",iCnt1);
    printout((void*)DataV, N_fft);
  }
  gap_waitbarrier(0);
#endif

#ifdef FFTR4exec
  // last Layer in radix4 (iLog4N - 1)
  iM = 1; iL = (N_fft>>2);
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA =  CoreId*Chunk*4*iM;
  for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
    Radix4FFTKernel_Twiddle0_FP16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA +   iM),
        (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
  // Synchronize all cores for last layer of the trellis
  gap_waitbarrier(0);

#ifdef DBGFFT
  if (CoreId==0) {
    printf("(last R4)stage %d\n",iCnt1+1);
    printout((void*)DataV, N_fft);
  }
  gap_waitbarrier(0);
#endif

#endif

#ifdef FFTRMexec

  /* Last Layer: Radix 2 with W = (1, 0) */
  iA = 0;
  iL = N_fft>>1;
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA = CoreId * Chunk * 2;

  for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
    v2h Tmp;
    iB = iA + 1;
    Tmp = ((DataV[iA]) - (DataV[iB]));
    DataV[iA] = ((DataV[iA]) + (DataV[iB]));
    DataV[iB] = Tmp;
    iA = iA + 2;
  }
  gap_waitbarrier(0);

#ifdef DBGFFT
  if (CoreId==0) {
    printf("(last RM)stage %d\n",iCnt1+1);
    printout((void*)DataV, N_fft);
  }
  gap_waitbarrier(0);
#endif

#endif

}

#endif

complex_fp cplxmul(complex_fp A, complex_fp B) {


  complex_fp tmp;

  tmp.r = A.r * B.r -  A.i * B.i;
  tmp.i = A.r * B.i +  A.i * B .r;

  return tmp;

}



/* Number of points for the Radix4 FFT */

/*
   Input samples are in Q<FFT4_SAMPLE_DYN>
   Coeff ares in Q<FFT_TWIDDLE_DYN>

   Out samples in Q<FFT4_SAMPLE_DYN>
   produce a bit reversed output (B and C are swapped compared to native radix4 butterfly)
   */

static void Radix4FFTKernelDIF(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
    v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
  v2s A1, B1, C1, D1;
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  if (Inverse) {
    A1 = ((A + C) +              (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) - gap_sub2rotmj(B,  D));
    C1 = ((A + C) -              (B + D));
    D1 = ((A - C) + gap_sub2rotmj(B,  D));
  } else {
    A1 = ((A + C) +              (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) + gap_sub2rotmj(B,  D));
    C1 = ((A + C) -              (B + D));
    D1 = ((A - C) - gap_sub2rotmj(B,  D));
  }

  B1 =  (v2s) gap_cplxmulsdiv4(B1, W1); C1 = (v2s) gap_cplxmulsdiv4(C1, W2); D1 = (v2s) gap_cplxmulsdiv4(D1, W3);
  //B1 =  (v2s)gap8_cplxmulsdiv4(B1, W1); C1 = (v2s) gap8_cplxmulsdiv4(C1, W2); D1 = (v2s) gap8_cplxmulsdiv4(D1, W3);

  *InOutA = A1; *InOutB = C1; *InOutC = B1; *InOutD = D1;
}

static void Radix4FFTKernelDIF_FP(complex_fp *InOutA, complex_fp *InOutB, complex_fp *InOutC, complex_fp *InOutD,
    complex_fp W1, complex_fp W2, complex_fp W3, unsigned int Inverse)

{
  complex_fp A1, B1, C1, D1;
  complex_fp A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  /* Used for IFFT here.
     IFFT uses (1,  1,  1,  1)  FFT uses (1,  1,  1,  1)
     (1,  j, -1, -j)	       (1, -j, -1,  j)
     (1, -1,  1, -1)	       (1, -1,  1, -1)
     (1, -j, -1,  j)	       (1,  j, -1, -j)
     To use this code for FFT:
   *InOutA = ((A + C) +	           (B + D ));
   *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
   */

  /*
  fRealA = sFFT.pfReal[iA] + sFFT.pfReal[iB]
    + sFFT.pfReal[iC] + sFFT.pfReal[iD];
  fImagA = sFFT.pfImag[iA] + sFFT.pfImag[iB]
    + sFFT.pfImag[iC] + sFFT.pfImag[iD];
  fRealB = sFFT.pfReal[iA] + sFFT.pfImag[iB]
    - sFFT.pfReal[iC] - sFFT.pfImag[iD];
  fImagB = sFFT.pfImag[iA] - sFFT.pfReal[iB]
    - sFFT.pfImag[iC] + sFFT.pfReal[iD];
  fRealC = sFFT.pfReal[iA] - sFFT.pfReal[iB]
    + sFFT.pfReal[iC] - sFFT.pfReal[iD];
  fImagC = sFFT.pfImag[iA] - sFFT.pfImag[iB]
    + sFFT.pfImag[iC] - sFFT.pfImag[iD];
  fRealD = sFFT.pfReal[iA] - sFFT.pfImag[iB]
    - sFFT.pfReal[iC] + sFFT.pfImag[iD];
  fImagD = sFFT.pfImag[iA] + sFFT.pfReal[iB]
    - sFFT.pfImag[iC] - sFFT.pfReal[iD];
  */

  if (Inverse) {

    A1.r = ((A.r + C.r) + (B.r + D.r));
    A1.i = ((A.i + C.i) + (B.i + D.i));
    B1.r = ((A.r - C.r) - (B.i - D.i));
    B1.i = ((A.i - C.i) + (B.r - D.r));
    C1.r = ((A.r + C.r) - (B.r + D.r));
    C1.i = ((A.i + C.i) - (B.i + D.i));
    D1.r = ((A.r - C.r) + (B.i - D.i));
    D1.i = ((A.i - C.i) - (B.r - D.r));

  } else {


    A1.r = ((A.r + C.r) + (B.r + D.r));
    A1.i = ((A.i + C.i) + (B.i + D.i));
    B1.r = ((A.r - C.r) + (B.i - D.i));
    B1.i = ((A.i - C.i) - (B.r - D.r));
    C1.r = ((A.r + C.r) - (B.r + D.r));
    C1.i = ((A.i + C.i) - (B.i + D.i));
    D1.r = ((A.r - C.r) - (B.i - D.i));
    D1.i = ((A.i - C.i) + (B.r - D.r));

  }

  B1 = cplxmul(B1, W1); C1 = cplxmul(C1, W2); D1 = cplxmul(D1, W3);
  // with swap of the outputs 1 and 2 (inoutC and inoutB)
  *InOutA = A1; *InOutC = B1; *InOutB = C1; *InOutD = D1;
}

static void Radix4FFTKernelDIF_FP_REAL(float *InA, float *InB, float *InC, float *InD,
				       complex_fp *OutA,complex_fp *OutB,complex_fp *OutC,complex_fp *OutD,
				       complex_fp W1, complex_fp W2, complex_fp W3, unsigned int Inverse)

{
  complex_fp A1, B1, C1, D1;
  float A = *InA, B = *InB, C = *InC, D = *InD;

  /* Used for IFFT here.
     IFFT uses (1,  1,  1,  1)  FFT uses (1,  1,  1,  1)
     (1,  j, -1, -j)	       (1, -j, -1,  j)
     (1, -1,  1, -1)	       (1, -1,  1, -1)
     (1, -j, -1,  j)	       (1,  j, -1, -j)
     To use this code for FFT:
   *InOutA = ((A + C) +	           (B + D ));
   *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
   */

  /*
  fRealA = sFFT.pfReal[iA] + sFFT.pfReal[iB]
    + sFFT.pfReal[iC] + sFFT.pfReal[iD];
  fImagA = sFFT.pfImag[iA] + sFFT.pfImag[iB]
    + sFFT.pfImag[iC] + sFFT.pfImag[iD];
  fRealB = sFFT.pfReal[iA] + sFFT.pfImag[iB]
    - sFFT.pfReal[iC] - sFFT.pfImag[iD];
  fImagB = sFFT.pfImag[iA] - sFFT.pfReal[iB]
    - sFFT.pfImag[iC] + sFFT.pfReal[iD];
  fRealC = sFFT.pfReal[iA] - sFFT.pfReal[iB]
    + sFFT.pfReal[iC] - sFFT.pfReal[iD];
  fImagC = sFFT.pfImag[iA] - sFFT.pfImag[iB]
    + sFFT.pfImag[iC] - sFFT.pfImag[iD];
  fRealD = sFFT.pfReal[iA] - sFFT.pfImag[iB]
    - sFFT.pfReal[iC] + sFFT.pfImag[iD];
  fImagD = sFFT.pfImag[iA] + sFFT.pfReal[iB]
    - sFFT.pfImag[iC] - sFFT.pfReal[iD];
  */


    A1.r = ((A + C) + (B + D));
    A1.i = 0;
    B1.r = (A - C) ;
    B1.i = ( - (B - D));
    C1.r = ((A + C) - (B + D));
    C1.i = 0;
    D1.r = (A - C);
    D1.i = (B - D);


  B1   = cplxmul(B1, W1);
  C1.r = C1.r * W2.r;
  C1.i = C1.r * W2.i;
  D1   = cplxmul(D1, W3);
  // with swap of the outputs 1 and 2 (inoutC and inoutB)
  *OutA = A1; *OutC = B1; *OutB = C1; *OutD = D1;
}


// produce a bit reversed output (swap B an C)
static void Radix4FFTKernel_Twiddle0(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, unsigned int Inverse)

{
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  /* Used for IFFT here.
     IFFT uses (1,  1,  1,  1)  FFT uses (1,  1,  1,  1)
               (1,  j, -1, -j)   	       (1, -j, -1,  j)
               (1, -1,  1, -1)   	       (1, -1,  1, -1)
               (1, -j, -1,  j)   	       (1,  j, -1, -j)
     To use this code for FFT:
   *InOutA = ((A + C) +	           (B + D ));
   *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
   */

  if (Inverse) {
    *InOutA = ((A + C) +	           (B + D));
    *InOutD = ((A - C) - gap_sub2rotmj(B,  D));
    *InOutB = ((A + C) -	           (B + D));
    *InOutC = ((A - C) + gap_sub2rotmj(B,  D));
  } else {
    *InOutA = ((A + C) +	           (B + D));
    *InOutD = ((A - C) + gap_sub2rotmj(B,  D));
    *InOutB = ((A + C) -	           (B + D));
    *InOutC = ((A - C) - gap_sub2rotmj(B,  D));
  }
}


static void Radix4FFTKernel_Twiddle0_FP(complex_fp *InOutA, complex_fp *InOutB, complex_fp *InOutC,complex_fp *InOutD, unsigned int Inverse)

{
  complex_fp A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;
  complex_fp A1,B1,C1,D1;
  /* Used for IFFT here.
     IFFT uses (1,  1,  1,  1)  FFT uses (1,  1,  1,  1)
     (1,  j, -1, -j)	       (1, -j, -1,  j)
     (1, -1,  1, -1)	       (1, -1,  1, -1)
     (1, -j, -1,  j)	       (1,  j, -1, -j)
     To use this code for FFT:
   *InOutA = ((A + C) +	           (B + D ));
   *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
   */

  if (Inverse) {

    (A1).r = ((A.r + C.r) + (B.r + D.r));
    (A1).i = ((A.i + C.i) + (B.i + D.i));
    (B1).r = ((A.r - C.r) - (B.i - D.i));
    (B1).i = ((A.i - C.i) + (B.r - D.r));
    (C1).r = ((A.r + C.r) - (B.r + D.r));
    (C1).i = ((A.i + C.i) - (B.i + D.i));
    (D1).r = ((A.r - C.r) + (B.i - D.i));
    (D1).i = ((A.i - C.i) - (B.r - D.r));

  } else {

    (A1).r = ((A.r + C.r) + (B.r + D.r));
    (A1).i = ((A.i + C.i) + (B.i + D.i));
    (B1).r = ((A.r - C.r) + (B.i - D.i));
    (B1).i = ((A.i - C.i) - (B.r - D.r));
    (C1).r = ((A.r + C.r) - (B.r + D.r));
    (C1).i = ((A.i + C.i) - (B.i + D.i));
    (D1).r = ((A.r - C.r) - (B.i - D.i));
    (D1).i = ((A.i - C.i) + (B.r - D.r));


  }
  // with swap of the 1 and 2 outputs (inoutb and inoutc)
  *InOutA = A1; *InOutC = B1; *InOutB = C1; *InOutD = D1;


}



/* Reorder from natural indexes to digitally-reversed one. Uses a pre computed LUT */
/* data is complex int */
/* reorder the exponet table */ 
void SwapSamples_scal( SwapSamples_scal_Arg_T *Arg)
{
  int i;
  
  cmplx *__restrict__ Datav = (cmplx *) Arg->Data;
  signed char *scale = (signed char*) Arg->shift_BF;
  short *__restrict__ SwapTable = Arg->SwapTable;
  int Ni = Arg->Ni;

  unsigned int CoreId;
  int First, Last, Chunk;

  CoreId = gap_coreid();

  Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
  
  for (i = First; i < Last; i++) {
    cmplx S =  Datav[i];
    signed char TMP = scale[i];
    int SwapIndex = SwapTable[i];
    //printf("%d swapindex %d S %d %d\n",i,SwapIndex,S[0],S[1]);
    if (i < SwapIndex) {
    	scale[i] = scale[SwapIndex]; scale[SwapIndex] = TMP;
    	Datav[i] = Datav[SwapIndex]; Datav[SwapIndex] = S;
    }
  }

  gap_waitbarrier(0);

  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_fft_swapped_c = np.array([ \n\t");
      for (int j=0; j<Ni; j++) {
        if (Datav[j][1]<0) printf("%d%dj, ",  Datav[j][0], Datav[j][1]);
        else               printf("%d+%dj, ", Datav[j][0], Datav[j][1]);
      }
      printf("])\nout_fft_shift_swapped_c = np.array([ \n\t");
      for (int j=0; j<Ni; j++) printf("%d, ", scale[j]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}



void SwapSamples_Par(SwapSamples_Arg_T *Arg)
  {
    TYPE_GENERIC *__restrict__ Data = (TYPE_GENERIC *) Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int i;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = gap_coreid();

    Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
    // for (i = 0; i < Ni; i++) {
    for (i = First; i < Last; i++) {
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        TYPE_GENERIC S = Data[i];
        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
      }
    }
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);

    #ifdef PRINTDEB
      if (CoreId==0){
        printf("\nout_swapped_fft = np.array([\n\t");
        for (int j=0; j<Ni; j++) {
          if (Data[j][1]<0) printf("%d%dj, ",  Data[j][0], Data[j][1]);
          else              printf("%d+%dj, ", Data[j][0], Data[j][1]);
        }
        printf("])\n");
      }
    #endif
  }

void SwapSamples_FP_FC(float *Datai, int Ni, short int *SwapTable)

  {
    complex_fp *__restrict__ Data = (complex_fp *) Datai;
    int i;
    unsigned int CoreId;
    int First, Last, Chunk;


    for (i = 0; i < Ni; i++) {
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        complex_fp S = Data[i];
        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
      }
    }
  }





void Radix4FFT_FP(FFT_Arg_T *Arg) {
  unsigned int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;
  unsigned int N_fft = Arg->N_fft;
  unsigned int Inverse = Arg->Inverse;
  unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
  TYPE_GENERIC *DataV  = (TYPE_GENERIC *) Arg->Data;
  TYPE_GENERIC *CoeffV = (TYPE_GENERIC *) Arg->Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};

  int i;

  CoreId = gap_coreid();
  int GAP8_NCORE = gap_ncore();
  // Layers 0,1, ... , (iLog4N-2)
  iM = N_fft >> 2; iL = 1;

#ifdef FFTR4exec
  for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1)
#endif
#ifdef FFTRMexec
  for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1)
#endif

    {
    Chunk = (iM/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iQ    = First*iL;
    for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
      iA = iCnt2;
      TYPE_GENERIC W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
	//printf("%d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
        RADIX4KERNEL((TYPE_GENERIC *) (DataV + iA       ), (TYPE_GENERIC *) (DataV + iA + iM),
            (TYPE_GENERIC *) (DataV + iA + 2*iM), (TYPE_GENERIC *) (DataV + iA + 3*iM),
            W1, W2, W3, Inverse);
        iA = iA + 4 * iM;
      }
      iQ += iL;
    }
    iL <<= 2; iM >>= 2;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);

#ifdef DBGFFT
    if (CoreId==0) {
      printf("stage %d\n",iCnt1);
      printout_scale((void*)DataV, N_fft,(1<<(iCnt1+1+2)));
    }
    gap_waitbarrier(0);
#endif

 }

#ifdef FFTRMexec
  // last radix4 Layer iLog4N - 1
  iM = 2; iL = (N_fft>>3); iQ = 0;
#endif
#ifdef FFTR4exec
  //  radix4 Layer iLog4N - 2
  iM = 4; iL = (N_fft>>4); iQ = 0;
#endif

  // Layer iLog4N - 2
  //iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    TYPE_GENERIC W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
    Chunk = (iL/GAP8_NCORE); //  + Off[CoreId&0x1];
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      RADIX4KERNEL((TYPE_GENERIC *) (DataV + iA       ), (TYPE_GENERIC *) (DataV + iA + iM),
          (TYPE_GENERIC *) (DataV + iA + 2*iM), (TYPE_GENERIC *) (DataV + iA + 3*iM),
          W1, W2, W3, Inverse);
      iA = iA + 4 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);
  }
#ifdef DBGFFT
  if (CoreId==0) {
    printf("stage %d\n",iCnt1);
    printout_scale((void*)DataV, N_fft,(1<<(iCnt1+1+2)));
    }
  gap_waitbarrier(0);
#endif

#ifdef FFTR4exec
  // Layer iLog4N - 1
  iM = 1; iL = (N_fft>>2);
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA =  CoreId*Chunk*4*iM;
  for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
    RADIX4KERNEL_TWID0((TYPE_GENERIC *) (DataV + iA       ), (TYPE_GENERIC *) (DataV + iA +   iM),
        (TYPE_GENERIC *) (DataV + iA + 2*iM), (TYPE_GENERIC *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
  // Synchronize all cores for last layer of the trellis
  gap_waitbarrier(0);
#endif

#ifdef DBGFFT
  if (CoreId==0) {
    printf("stage %d\n",iCnt1);
    printout_scale((void*)DataV, N_fft,(1<<(iCnt1+1+2)));
    }
  gap_waitbarrier(0);
#endif
}




// paralelized version of the radix2 fft floating point
void Radix2FFT_DIF_FP_FC(float *__restrict__ Data,    float *__restrict__ Twiddles,
			 int N_FFT2) 
{

  unsigned int iLog2N  = gap_fl1(N_FFT2);
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  complex_fp *CoeffV = (complex_fp *) Twiddles;
  complex_fp *DataV  = (complex_fp *) Data;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;

  CoreId = gap_coreid();

  iL = 1;
  iM = N_FFT2 / 2;

  for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
    iQ    = 0;
    for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
      complex_fp W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        complex_fp Tmp;
        iB = iA + iM;
        Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
        Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
        (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
        (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
        DataV[iB] = cplxmul(Tmp, W);
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL <<= 1;
    iM >>= 1;

#ifdef DBGFFT
    printf("stage %d\n",iCnt1);
    printout((void*)DataV, N_FFT2);
#endif

  }

  iA = 0;
  /* Last Layer: W = (1, 0) */
  for (iCnt3 = 0; iCnt3 < (unsigned int)(N_FFT2>>1); iCnt3++) {
    complex_fp Tmp;
    iB = iA + 1;
    Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
    Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
    (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
    (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
    DataV[iB] = Tmp;
    iA = iA + 2;
  }
}

// paralelized version of the radix2 fft floating point
void Radix2FFT_DIF_Par_FP(FFT_Arg_T *Arg)

{
  unsigned int N_FFT2 = Arg->N_fft;

  unsigned int iLog2N  = gap_fl1(N_FFT2);
  unsigned int Inverse = Arg->Inverse;
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  complex_fp *CoeffV = (complex_fp *) Arg->Twiddles;
  complex_fp *DataV  = (complex_fp *) Arg->Data;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;

  CoreId = gap_coreid();
  int GAP8_NCORE = gap_ncore();
  iL = 1;
  iM = N_FFT2 / 2;


  for (iCnt1 = 0; iCnt1 < (iLog2N-3); ++iCnt1) {
    Chunk = (iM/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
     //First =  0; Last = iM;
    iQ    = First*iL;
    for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
      complex_fp W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
        complex_fp Tmp;
        iB = iA + iM;
 	//printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1,iL,iA,iB);
	Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
        Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
        (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
        (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
        DataV[iB] = cplxmul(Tmp, W);
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL <<= 1;
    iM >>= 1;
    //}
    gap_waitbarrier(0);

#ifdef DBGFFT
    if (CoreId==0) {
      printf("stage %d\n",iCnt1);
      printout((void*)DataV, N_FFT2);
    }
    gap_waitbarrier(0);
#endif

  }

  // Layer iLog2N - 3
  iM = 4; iL = (N_FFT2>>(1+1+1)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    complex_fp W = CoeffV[  iQ];
    Chunk = (iL/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*2*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      complex_fp Tmp;
      iB = iA + iM;
      //printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1,iL,iA,iB);
      Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
      Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
      (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
      (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
      DataV[iB] = cplxmul(Tmp, W);
      iA = iA + 2 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
  }
  gap_waitbarrier(0);

#ifdef DBGFFT
  if (CoreId==0) {
    printf("stage %d\n",iCnt1);
    printout((void*)DataV, N_FFT2);
  }
  gap_waitbarrier(0);
#endif

  // Layer iLog2N - 2
  iM = 2; iL = (N_FFT2>>(1+1)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    complex_fp W = CoeffV[  iQ];
    Chunk = (iL/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*2*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      complex_fp Tmp;
        iB = iA + iM;
	//printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1+1,iL,iA,iB);
        Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
        Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
        (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
        (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
        DataV[iB] = cplxmul(Tmp, W);
	iA = iA + 2 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
  }
  gap_waitbarrier(0);

#ifdef DBGFFT
  if (CoreId==0) {
    printf("stage %d\n",iCnt1+1);
    printout((void*)DataV, N_FFT2);
  }
  gap_waitbarrier(0);
#endif

  /* Last Layer: W = (1, 0) */
  iM=1;
  iL = N_FFT2>>1;
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA = CoreId * Chunk * 2;
  for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
    complex_fp Tmp;
    iB = iA + 1;
    Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
    Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
    (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
    (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
    DataV[iB] = Tmp;
    iA = iA + 2 * iM;
  }

  gap_waitbarrier(0);
#ifdef DBGFFT
  if (CoreId==0) {
    printf("stage %d\n",iCnt1+1);
    printout((void*)DataV, N_FFT2);
  }
  gap_waitbarrier(0);
#endif
}


// radix 4 floating point sequential (for FC)
void Radix4FFT_DIF_FP_FC(float *__restrict__ Data,
    float *__restrict__ Twiddles,
    int N_fft,
    unsigned int Inverse) 
{
  unsigned int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;
  unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
  complex_fp *DataV  = (complex_fp *) Data;
  complex_fp *CoeffV = (complex_fp *) Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};

  int i;


  // Layers 0,1, ... , (iLog4N-2)
  CoreId=0;
  iM = N_fft >> 2; iL = 1;
  for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
    Chunk = (iM);
    First =  CoreId*Chunk; Last = First+Chunk;
    iQ    = First*iL;
    for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
      iA = iCnt2;
      complex_fp W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
	//printf("0 %d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
        Radix4FFTKernelDIF_FP((complex_fp *) (DataV + iA       ), (complex_fp *) (DataV + iA + iM),
            (complex_fp *) (DataV + iA + 2*iM), (complex_fp *) (DataV + iA + 3*iM),
            W1, W2, W3, Inverse);
        iA = iA + 4 * iM;
      }
      iQ += iL;
    }
    iL <<= 2; iM >>= 2;

  }
  
  // Layer iLog4N - 2
  iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    complex_fp W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
    Chunk = (iL); //  + Off[CoreId&0x1];
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      //printf("1 %d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
      Radix4FFTKernelDIF_FP((complex_fp *) (DataV + iA       ), (complex_fp *) (DataV + iA + iM),
          (complex_fp *) (DataV + iA + 2*iM), (complex_fp *) (DataV + iA + 3*iM),
          W1, W2, W3, Inverse);
      iA = iA + 4 * iM;
    }
    iQ += iL;
  }

  #ifdef DBGFFT
    printf("stage %d\n",iCnt1);
    printout((void*)DataV,N_fft);
  #endif

    // Layer iLog4N - 1
    iM = 1; iL = (N_fft>>2);
    Chunk = iL;
    First =  CoreId*Chunk; Last = First+Chunk;
    iA =  CoreId*Chunk*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      //printf("2 %d %d %d %d\n",CoreId,iCnt3,iA,iM);
      Radix4FFTKernel_Twiddle0_FP((complex_fp *) (DataV + iA       ), (complex_fp *) (DataV + iA +   iM),
          (complex_fp *) (DataV + iA + 2*iM), (complex_fp *) (DataV + iA + 3*iM), Inverse);
      iA =  iA + 4 * iM;
    }
}
  

// set mixed radix flag
#define FFTRMexec 1
#define SHIFT_RMIXED 0


// cluster compatible  version of the mixed radix input real output complex floating point
// with 8 cores, size must be 128 min (32 will produce stages withi M=4 and iL=4 => max 4 cores))
void RadixMixedFFT_DIF_Par_FP(FFT_Arg_T *Arg){
  unsigned int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;

  // we assume N_FFT is not a power of 4=> divide by 2 to get power of 4
  // N_FFT must be > 64 to operate on 8 cores
  unsigned int N_fft = Arg->N_fft;
  unsigned Inverse = Arg->Inverse;
  unsigned int N_FFTDIV = N_fft>>1;


  TYPE_GENERIC *DataV  = (TYPE_GENERIC *) Arg->Data;

  unsigned int iLog4N  = (gap_fl1(N_FFTDIV))>>1;
  TYPE_GENERIC *CoeffV = (TYPE_GENERIC *) Arg->Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};
  int i;
  CoreId = gap_coreid();
  int gap8_ncore = gap_ncore();
  Radix4FFT_FP(Arg);
  #ifdef FFTRMexec
    // if mixed radix
    /* Last Layer: Radix 2 with W = (1, 0) */
    iA = 0;
    iL = N_fft>>1;
    Chunk = iL/gap8_ncore;
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = CoreId * Chunk * 2;
    
    for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
      TYPE_GENERIC Tmp;
      iB = iA + 1;
      #if (FP==1)
          Tmp.r = (DataV[iA]).r - (DataV[iB]).r;
          Tmp.i = (DataV[iA]).i - (DataV[iB]).i;
          (DataV[iA]).r = (DataV[iA]).r + (DataV[iB]).r;
          (DataV[iA]).i = (DataV[iA]).i + (DataV[iB]).i;
      #elif (FP==2)
          Tmp = ((DataV[iA]) - (DataV[iB]));
          DataV[iA] = ((DataV[iA]) + (DataV[iB]));
      #else
          Tmp = ((DataV[iA]) - (DataV[iB]))<<1;
          DataV[iA] = ((DataV[iA]) + (DataV[iB]))<<1;
      #endif
        DataV[iB] = Tmp;
        iA = iA + 2;
    }
  gap_waitbarrier(0);
  #endif
}




// cluster compatible  version of the mixed radix FFT vectorized, fixed point
// with 8 cores, size must be 128 min (32 will produce stages withi M=4 and iL=4 => max 4 cores))
static void RadixMixedFFT_DIF_Par_Ker(FFT_Arg_T *Arg)
{
  unsigned int iCnt1, iCnt2, iCnt3,
               iL,    iM,    iQ,
               iA,    iB,    iC,     iD;

  // we assume N_FFT is not a power of 4=> divide by 2 to get power of 4
  // N_FFT must be > 64 to operate on 8 cores
  unsigned int N_fft = Arg->N_fft;
  unsigned Inverse = Arg->Inverse;
  unsigned int N_FFTDIV = N_fft>>1;
  unsigned int iLog4N  = (gap_fl1(N_FFTDIV))>>1;
  v2s *DataV  = (v2s *) Arg->Data;
  v2s *CoeffV = (v2s *) Arg->Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};

  int i;
  CoreId = gap_coreid();
  int GAP8_NCORE  = gap_ncore();


  // Layers 0,1, ... , (iLog4N-2)
  iM = N_fft >> 2; iL = 1;iQ=0;
  for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
    Chunk = (iM/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iQ    = First*iL;
    for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
      iA = iCnt2;
      v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
        Radix4FFTKernelDIF((v2s *) (DataV + iA       ), (v2s *) (DataV + iA + iM),
                           (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
                           W1, W2, W3, Inverse);
        iA = iA + 4 * iM;
      }
      iQ += iL;
    }
    iL <<= 2; iM >>= 2;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);
  }

  // This stage will work for nfft > 32 (il=4 im=4) for 8 cores
#ifdef FFTRMexec  
  // last radix4 Layer iLog4N - 1
  iM = 2; iL = (N_fft>>3); iQ = 0;
#endif
#ifdef FFTR4exec
  // last radix4 Layer iLog4N - 2
  iM = 4; iL = (N_fft>>4); iQ = 0;
#endif


  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
    Chunk = (iL/GAP8_NCORE);
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      //printf("1 %d %d %d %d %d\n",CoreId,iCnt2,iCnt3,iA,iM);
      Radix4FFTKernelDIF((v2s *) (DataV + iA       ), (v2s *) (DataV + iA + iM),
                         (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
                         W1, W2, W3, Inverse);
      iA = iA + 4 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
    gap_waitbarrier(0);

  }

#ifdef DBGFFT
  if (CoreId==0) {
    printf("**stage %d\n",iCnt1);
    printout_scale((void*)DataV, N_fft,(1<<2*(iCnt1+2)));
  }
  gap_waitbarrier(0);
#endif


#ifdef FFTR4exec
  // last Layer in radix4 (iLog4N - 1)
  iM = 1; iL = (N_fft>>2);
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA =  CoreId*Chunk*4*iM;
  for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
    Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
        (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
  // Synchronize all cores for last layer of the trellis
  gap_waitbarrier(0);
  #ifdef DBGFFT
    if (CoreId==0) {
      printf("(last R4)stage %d\n",iCnt1+1);
      printout_scale((void*)DataV, N_fft,(1<<2*(iCnt1+2)));
    }
    gap_waitbarrier(0);
  #endif
#endif

#ifdef FFTRMexec

  /* Last Layer: Radix 2 with W = (1, 0) */
  iA = 0;
  iL = N_fft>>1;
  Chunk = iL/GAP8_NCORE;
  First =  CoreId*Chunk; Last = First+Chunk;
  iA = CoreId * Chunk * 2;

  for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
    v2s Tmp;
    iB = iA + 1;
    Tmp = ((DataV[iA]) - (DataV[iB]))<<SHIFT_RMIXED;
    DataV[iA] = ((DataV[iA]) + (DataV[iB]))<<SHIFT_RMIXED;
    DataV[iB] = Tmp;
    iA = iA + 2;
  }
  gap_waitbarrier(0);
#endif
}



/*
   Radix 2, Decimated in Frequency, fft. v2s
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   cluster compatibles version
   */

static void Radix2FFT_DIF_Par_Ker(FFT_Arg_T *Arg)

{
  unsigned int N_FFT2 = Arg->N_fft;
  unsigned int Inverse = Arg->Inverse;
  unsigned int iLog2N  = gap_fl1(N_FFT2);
  signed char *shift_fft = Arg->shift_fft;
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Arg->Twiddles;
  v2s *DataV  = (v2s *) Arg->Data;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;

  CoreId = gap_coreid();
  iL = 1;
  iM = N_FFT2 / 2;

  if (CoreId==0) memset(shift_fft,0,N_FFT2*sizeof(signed char));


  gap_waitbarrier(0);

  for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
    Chunk = (iM/gap_ncore());
    First =  CoreId*Chunk; Last = First+Chunk;
    iQ = First*iL;
    for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        Tmp = DataV[iA] - DataV[iB];
        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[iB] = (v2s) gap_cplxmulsdiv2(Tmp, W) ;
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL <<= 1;
    iM >>= 1;

    gap_waitbarrier(0);
  }

  // Layer iLog2N - 3
  iM = 4; iL = (N_FFT2>>(1+1+1)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    v2s W = CoeffV[  iQ];
    Chunk = (iL/gap_ncore()); 
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*2*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      v2s Tmp;
      iB = iA + iM;
      //printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1,iL,iA,iB);
      Tmp = (DataV[iA]) - (DataV[iB]);
      DataV[iA] = (DataV[iA] + DataV[iB])>> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
      DataV[iB] = (v2s) gap_cplxmulsdiv2(Tmp, W);
      iA = iA + 2 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
  }
  gap_waitbarrier(0);

  // Layer iLog2N - 2
  iM = 2; iL = (N_FFT2>>(1+1)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    v2s W = CoeffV[  iQ];
    Chunk = (iL/gap_ncore()); 
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*2*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      v2s Tmp;
      iB = iA + iM;
      //printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1+1,iL,iA,iB);
      Tmp = (DataV[iA]) - (DataV[iB]);
      DataV[iA] = (DataV[iA] + DataV[iB])>> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
      DataV[iB] = (v2s) gap_cplxmulsdiv2(Tmp, W);
      iA = iA + 2 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
  }
  gap_waitbarrier(0);

  /* Last Layer: W = (1, 0) */
  Chunk = ((N_FFT2>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_FFT2>>1));
  iA = 2*Chunk*CoreId;
  for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
    //printf("core %d stage %d iL %d iA %d\n",CoreId,iCnt3,iL,iA);
    v2s A= DataV[iA];
    v2s B= DataV[iA+1];
    DataV[iA] = A + B;
    DataV[iA+1] = A - B;
    iA = iA + 2;
  }
  gap_waitbarrier(0);
}


/* Reorder from natural indexes to digitally-reversed one. Uses a pre computed LUT */
  void SwapSamples(SwapSamples_Arg_T *Arg)

  {
    v2s *__restrict__ Data = Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int i;

    for (i = 0; i < Ni; i++) {
      v2s S = Data[i];
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
      }
    }
  }



  /* Copy Twiddles and SwapLUT table from L2 to shared L1 */
void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format)

  {
    AT_L2_EVENT DmaR_Evt1, DmaR_Evt2;
    int TwidSize, LUTSize;

    if (Arg->Radix == 2) TwidSize = Arg->Nfft * sizeof(short);
    else TwidSize = 3 * Arg->Nfft * (sizeof(short)/2);

    // when floating 32, size is double
    if (format==1) TwidSize *=2;
    LUTSize = Arg->Nfft*sizeof(short);


    AT_L2_COPY((AT_L2_EXT_ADDR_TYPE) Arg->SwapLUT, (AT_L2_INT_ADDR_TYPE) Arg->L1_SwapLUT,  LUTSize,  0, 0, &DmaR_Evt2);
    AT_L2_COPY((AT_L2_EXT_ADDR_TYPE) Arg->Twiddles, (AT_L2_INT_ADDR_TYPE)Arg->L1_Twiddles, TwidSize, 0, 0, &DmaR_Evt1);

    AT_L2_WAIT(0, &DmaR_Evt1);
    AT_L2_WAIT(0, &DmaR_Evt2);
    //printf("L1 twid\n");
    //printout((void*)Arg->L1_Twiddles,Arg->Nfft/2);
  }


static void kernel_fftrad2_scal(cmplx *DataV, v2s W, signed char *shift_fft, int iA, int iB) {

  cmplx Tmp;
  int Dshift;
  int shift=0;
  
  //printf("%d %d %d \t==  %d %d \t%d \t== %d %d \t%d\n",iCnt1,iCnt2,iCnt3, DataV[iA][0]>>16, DataV[iA][1]>>16,shift_fft[iA], DataV[iB][0]>>16, DataV[iB][1]>>16,shift_fft[iB]);
  
  // align exponents for iA and iB
  Dshift = shift_fft[iB] - shift_fft[iA];
  
  if (Dshift > 0) {
    DataV[iB] >>= (cmplx) {Dshift,Dshift};
    shift_fft[iB] = shift_fft[iA];
  }
  else {
    DataV[iA] >>= (cmplx) {-Dshift,-Dshift};
    shift_fft[iA] = shift_fft[iB];
  }
  
  // compute first part of butterfly 
  Tmp = DataV[iA] - DataV[iB];
  DataV[iA] = (DataV[iA] + DataV[iB]) ;
  
  
  // normalize iB mantissa to Q29 before mult
  int norm = (abs( Tmp[0]) + abs(Tmp[1])); 
  shift = __builtin_clz(norm) - 3;
  if (norm) {
    if (shift > 0) { 
      Tmp <<= (cmplx) {shift,shift};
      shift_fft[iB] += (shift);
    } else if (shift <= -1) {
      Tmp >>= (cmplx) {1,1};
      shift_fft[iB]--;
    }
  }
  
  // multiply coeff Q15, data Q29   Q15 * (Q29>>15)  => Q29   
  DataV[iB][0] = (Tmp[0] >> 15) * W[0] - (Tmp[1] >> 15) * W[1];
  DataV[iB][1] = (Tmp[0] >> 15) * W[1] + (Tmp[1] >> 15) * W[0];
  
  //printf("mult %d d %d %d c %x %x r %x %x\n",iCnt3,Tmp[0],Tmp[1],W[0],W[1],DataV[iB][0],DataV[iB][1]);
  //printf("iM %d iL %d i1 %d iQ %d iA %d iB %d i2 %d i3 %d\n",iM,iL,iCnt1,iQ,iA,iB,iCnt2,iCnt3 );
  //DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
  
  // normalize iA mantissa to Q29 
  norm = (abs(DataV[iA][0]) + abs(DataV[iA][1]));
  shift = __builtin_clz(norm) - 3;
  if (norm) {
    if (shift > 0) { 
      DataV[iA] <<= (cmplx) {shift, shift};
      shift_fft[iA] += (shift);
    } else if (shift <= -1) {
      DataV[iA] >>= (cmplx) {1,1};
      shift_fft[iA]--;
    }
  }
  
  //printf("add %d d %d %d n %d s %d %d\n",iCnt3,DataV[iA][0]>>16,DataV[iA][1]>>16,norm,shift,shift_fft[iA]);
  

}



/*
  32 bits version of the fft, with "pseudo floating point": an exponent is attached to each complex data
     Twiddles are short int Q15
     Data are int Q29 (mantissa) + exponent

 */
void Radix2FFT_DIF_INT_Scal_Par(FFT_scal_Arg_T *Arg)
{
 
    unsigned int N_FFT2 = Arg->N_fft;
    unsigned int Inverse = Arg->Inverse;
    unsigned int iLog2N  = gap_fl1(N_FFT2);
    unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
    v2s *CoeffV = (v2s *) Arg->Twiddles;
    cmplx *DataV  = (cmplx *) Arg->Data;
    signed char *shift_fft = (signed char *) Arg->shift_fft;
    
    unsigned int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap_coreid();

    int shift=0;
    int Dshift;
    int nbcore = gap_ncore();

    // reset the shift table
    Chunk = N_FFT2/nbcore;
    First =  CoreId*Chunk; Last = Min( First+Chunk,N_FFT2);
    for (int i = First; i < Last; i++) shift_fft[i]=0; 
    gap_waitbarrier(0);

    // compute fft 
    iL = 1;
    iM = N_FFT2 / 2;
    

#ifdef PRINTFFT
    if (CoreId==0) {
      printf("stage input\n");
      for (int i=0; i<N_FFT2; i++)
	printf("%d %d %d >> %d\n",i,DataV[i][0],DataV[i][1], shift_fft[i]);
    } 
    gap_waitbarrier(0);
   
#endif
	
    for (iCnt1 = 0; iCnt1 < (unsigned int)(iLog2N-3); iCnt1++) {
	Chunk = (iM/nbcore);
	First =  CoreId*Chunk; Last = Min( First+Chunk,iM);
	iQ = First*iL;
	for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
            v2s W = CoeffV[iQ];
            iA = iCnt2;
            for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
	      cmplx Tmp;
	      iB = iA + iM;

	      kernel_fftrad2_scal(DataV, W, shift_fft, iA, iB);
	      iA = iA + 2 * iM;
            }
            iQ += iL;
        }
	
	
	iL <<= 1;
        iM >>= 1;
	gap_waitbarrier(0);

#ifdef PRINTFFT
	if (CoreId==0) {
	  printf("stage %d\n",iCnt1);
	  for (int i=0; i<N_FFT2; i++)
	    printf("%d %d %d >> %d\n",i,DataV[i][0]>>16,DataV[i][1]>>16, shift_fft[i]);
	}
	gap_waitbarrier(0);
#endif
    }

    // Layer iLog2N - 3
    iM = 4; iL = (N_FFT2>>(1+1+1)); iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
      v2s W = CoeffV[  iQ];
      Chunk = (iL/nbcore); 
      First =  CoreId*Chunk; Last = Min(First+Chunk,iL);
      iA = iCnt2 + Chunk*CoreId*2*iM;
      for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
	iB = iA + iM;
	//printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1,iL,iA,iB);
	
	kernel_fftrad2_scal(DataV, W, shift_fft, iA, iB);
	
	iA = iA + 2 * iM;
      }
      iQ += iL;
      // Synchronize all cores for current layer of the trellis
    }
    gap_waitbarrier(0);

#ifdef PRINTFFT
    if (CoreId==0) {
      printf("stage %d\n",iCnt1);
      for (int i=0; i<N_FFT2; i++)
	printf("%d %d %d >> %d\n",i,DataV[i][0]>>16,DataV[i][1]>>16, shift_fft[i]);
    }
    gap_waitbarrier(0);
#endif

    
    // Layer iLog2N - 2
    iM = 2; iL = (N_FFT2>>(1+1)); iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
      v2s W = CoeffV[  iQ];
      Chunk = (iL/nbcore); 
      First =  CoreId*Chunk; Last = Min(First+Chunk,iL);
      iA = iCnt2 + Chunk*CoreId*2*iM;
      for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
	iB = iA + iM;
	
	//printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1+1,iL,iA,iB);
	kernel_fftrad2_scal(DataV, W, shift_fft, iA, iB);
	
	iA = iA + 2 * iM;
      }
      iQ += iL;
      // Synchronize all cores for current layer of the trellis
    }
    gap_waitbarrier(0);
#ifdef PRINTFFT
    if (CoreId==0) {
      printf("stage %d\n",iCnt1+1);
      for (int i=0; i<N_FFT2; i++)
	printf("%d %d %d >> %d\n",i,DataV[i][0]>>16,DataV[i][1]>>16, shift_fft[i]);
    }
    gap_waitbarrier(0);
#endif


    
    // last stage    
    Chunk = ((N_FFT2>>1)/nbcore); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_FFT2>>1));
    iA = 2*Chunk*CoreId;
    
    for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
        cmplx Tmp;
	
        iB = iA + 1;
	Dshift = shift_fft[iB] - shift_fft[iA];
	
	if (Dshift>0) {
	  DataV[iB] >>= (cmplx) {Dshift,Dshift};
	  shift_fft[iB]  = shift_fft[iA];
	}
	else {
	  DataV[iA] >>= (cmplx) {-Dshift,-Dshift};
	  shift_fft[iA]  = shift_fft[iB];
	}
	
	Tmp = (DataV[iA] - DataV[iB]);
        DataV[iA] = (DataV[iA] + DataV[iB]);
        DataV[iB] = Tmp;
        iA = iA + 2;
    }
    gap_waitbarrier(0);

}
//
// version of FFT where scaling is done at each add/sub in the butterfly
// data are Q13 (16bits)
//
void Radix2FFT_DIF(FFT_Arg_T *Arg)
{
    signed short * __restrict__ Data = Arg->Data;
    signed short * __restrict__ Twiddles = Arg->Twiddles;
    unsigned int N_FFT2 = Arg->N_fft;
    //int iLog2N  = 31 - __builtin_clz(N_FFT2); //__FL1(N_FFT2);
    int iLog2N  = gap_fl1(N_FFT2);
    unsigned int iCnt1, iCnt2, iCnt3,
        iQ,    iL,    iM,
        iA,    iB;
    v2s *CoeffV = (v2s *) Twiddles;
    v2s *DataV  = (v2s *) Data;

    iL = 1;
    iM = N_FFT2 / 2;

    for (iCnt1 = 0; iCnt1 < (unsigned int)(iLog2N-1); iCnt1++) {
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
            v2s W = CoeffV[iQ];
            iA = iCnt2;
            for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                v2s Tmp;
                iB = iA + iM;
                Tmp = DataV[iA] - DataV[iB];
                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                DataV[iB] = (v2s) gap_cplxmuls(Tmp, W) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                //DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
                iA = iA + 2 * iM;
            }
            iQ += iL;
      //printf("stage %d\n",iCnt1);
      //for (int i=0; i<N_FFT2; i++) printf("%d %d %d\n",i,DataV[i][0],DataV[i][1]);
        }
        iL <<= 1;
        iM >>= 1;
    }
    iA = 0;
    for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
        v2s Tmp;
        iB = iA + 1;
        Tmp = (DataV[iA] - DataV[iB]);
        DataV[iA] = (DataV[iA] + DataV[iB]);
        DataV[iB] = Tmp;
        iA = iA + 2;
    }
}



void RadixMixedFFT_DIF_Inv_Par(FFT_Arg_T *Arg) {
  Arg->Inverse = 1;
  RadixMixedFFT_DIF_Par_Ker(Arg);
}



void Radix2FFT_DIF_Inv_Par(FFT_Arg_T *Arg) {
  Arg->Inverse = 1;
  Radix2FFT_DIF_Par_Ker(Arg);
}

void RadixMixedFFT_DIF_Par(FFT_Arg_T *Arg) {
  Arg->Inverse = 0;
  RadixMixedFFT_DIF_Par_Ker(Arg);
}



void Radix2FFT_DIF_Par(FFT_Arg_T *Arg) {
  Arg->Inverse = 0;
  Radix2FFT_DIF_Par_Ker(Arg);
}
