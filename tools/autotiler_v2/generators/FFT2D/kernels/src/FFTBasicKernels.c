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

static void Radix4FFTKernel_Twiddle0(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, unsigned int Inverse)

{
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

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
    *InOutA = ((A + C) +	           (B + D));
    *InOutB = ((A - C) - gap8_sub2rotmj(B,  D));
    *InOutC = ((A + C) -	           (B + D));
    *InOutD = ((A - C) + gap8_sub2rotmj(B,  D));
  } else {
    *InOutA = ((A + C) +	           (B + D));
    *InOutB = ((A - C) + gap8_sub2rotmj(B,  D));
    *InOutC = ((A + C) -	           (B + D));
    *InOutD = ((A - C) - gap8_sub2rotmj(B,  D));
  }
}

static void Radix4FFTKernel_Twiddle0_Scaled(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, unsigned int Inverse)

{
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

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
    *InOutA = ((A + C) +	           (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutB = ((A - C) - gap8_sub2rotmj(B,  D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutC = ((A + C) -	           (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutD = ((A - C) + gap8_sub2rotmj(B,  D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
  } else {
    *InOutA = ((A + C) +	           (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutB = ((A - C) + gap8_sub2rotmj(B,  D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutC = ((A + C) -	           (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutD = ((A - C) - gap8_sub2rotmj(B,  D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
  }
}

/*
   Inputs are in Q<FFT4_SAMPLE_DYN>, Twidlle factors are in Q<FFT_TWIDDLE_DYN>,
   Outputs are in Q<FFT4_SAMPLE_DYN> therefore we need to shift by <FFT_TWIDDLE_DYN> to get a Q<FFT4_SAMPLE_DYN> number
   Note that output could be > 1.0 and go up to 8.0
   */

static void Radix4FFTKernelDIT(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
  v2s B1, C1, D1;
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  B1 = gap8_cplxmuls(B, W1); C1 = gap8_cplxmuls(C, W2); D1 = gap8_cplxmuls(D, W3);

  if (Inverse) {
    *InOutA = ((A + C1) +               (B1 + D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutB = ((A - C1) - gap8_sub2rotmj(B1,  D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutC = ((A + C1) -               (B1 + D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutD = ((A - C1) + gap8_sub2rotmj(B1,  D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
  } else {
    *InOutA = ((A + C1) +               (B1 + D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutB = ((A - C1) + gap8_sub2rotmj(B1,  D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutC = ((A + C1) -               (B1 + D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    *InOutD = ((A - C1) - gap8_sub2rotmj(B1,  D1)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
  }
}

static void Radix4FFTKernelDIF(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
    v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
  v2s A1, B1, C1, D1;
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  if (Inverse) {
    A1 = ((A + C) +               (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) - gap8_sub2rotmj(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) + gap8_sub2rotmj(B,  D));
  } else {
    A1 = ((A + C) +               (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) + gap8_sub2rotmj(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) - gap8_sub2rotmj(B,  D));
  }

  B1 = gap8_cplxmulsdiv4(B1, W1); C1 = gap8_cplxmulsdiv4(C1, W2); D1 = gap8_cplxmulsdiv4(D1, W3);
  *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}



/*
   Radix-4 Decimated in Time FFT. Input have to be digitally-reversed, output is naturally ordered.
   First stage uses the fact that twiddles are all (1, 0)
   */

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIT_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT4,
    unsigned int Inverse)

{
  int iCnt1, iCnt2, iCnt3;
  int iL,    iM,    iQ;
  int iA,    iB,    iC,     iD;
  int iLog4N  = gap8_fl1(N_FFT4)>>1;
  v2s *DataV  = (v2s *) Data;
  v2s *CoeffV = (v2s *) Twiddles;

  iL = N_FFT4 / 4; iM = 1;
  iA = 0;
  for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
    Radix4FFTKernel_Twiddle0_Scaled((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
        (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
  iL = iL >> 2;
  iM = iM << 2;
  for (iCnt1 = 1; iCnt1 < iLog4N; ++iCnt1) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
      iA = iCnt2;
      v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
        Radix4FFTKernelDIT((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
            (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
            W1, W2, W3, Inverse);

        iA =  iA + 4 * iM;
      }
      iQ += iL;
    }
    iL = iL >> 2;
    iM = iM << 2;
  }
}

void Radix4FFT_DIT(FFT_Arg_T *Arg)

{
  Radix4FFT_DIT_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 0);
}

void Radix4FFT_DIT_Inverse(FFT_Arg_T *Arg)

{
  Radix4FFT_DIT_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 1);
}

/*
   Radix 4, Decimated in Frequency, fft.
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIF_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT4,
    unsigned int Inverse)

{
  int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;
  int iLog4N  = gap8_fl1(N_FFT4)>>1;
  v2s *DataV  = (v2s *) Data;
  v2s *CoeffV = (v2s *) Twiddles;

  iL = 1;
  iM = N_FFT4 / 4;

  for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
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
    iL <<= 2;
    iM >>= 2;
  }
  iA = 0; iL = (N_FFT4>>2);
  for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
    Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
        (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
}

/*
   Radix 4, Decimated in Frequency, fft_2d: W=TileW, H=N_FFT4.
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIF_Vertical_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT4, unsigned int TileW,
    unsigned int Inverse)

{
  int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;
  int iLog4N  = gap8_fl1(N_FFT4)>>1;
  v2s *DataV  = (v2s *) Data;
  v2s *CoeffV = (v2s *) Twiddles;

  iL = 1;
  iM = N_FFT4 / 4;

  for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
      iA = iCnt2;
      v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
      for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
        Radix4FFTKernelDIF((v2s *) (DataV + TileW*(iA)       ), (v2s *) (DataV + TileW*(iA + iM)),
            (v2s *) (DataV + TileW*(iA + 2*iM)), (v2s *) (DataV + TileW*(iA + 3*iM)),
            W1, W2, W3, Inverse);

        iA = iA + 4 * iM;
      }
      iQ += iL;
    }
    iL <<= 2;
    iM >>= 2;
  }
  iA = 0; iL = (N_FFT4>>2);
  for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
    Radix4FFTKernel_Twiddle0((v2s *) (DataV + TileW*(iA)       ), (v2s *) (DataV + TileW*(iA +   iM)),
        (v2s *) (DataV + TileW*(iA + 2*iM)), (v2s *) (DataV + TileW*(iA + 3*iM)), Inverse);
    iA =  iA + 4 * iM;
  }
}

void Radix4FFT_DIF(FFT_Arg_T *Arg)

{
  Radix4FFT_DIF_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 0);
}

void Radix4FFT_DIF_Inverse(FFT_Arg_T *Arg)

{
  Radix4FFT_DIF_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 1);
}

/*
   Radix 4, Decimated in Frequency, fft. Parallel implementation.
   Each level of the butterfly trellis is evaluated in parralel with a synchronization point
   before entering the next level. The 2 last levels of the trellis are handled in a specific
   way since final level uses static twiddles values and final minus 1 evaluates only 4 groups (iCnt2 loop)
   and 4 is not enough to load 8 processors. This final - 1 level could be merged with the common case
   if 4 cores is assumed.
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */

// void Radix4FFT_DIF_Par(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, unsigned int N_fft)

static inline void __attribute__ ((__always_inline__)) Radix4FFT_DIF_Par_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_fft,
    unsigned int Inverse)

{
  unsigned int iCnt1, iCnt2, iCnt3,
      iL,    iM,    iQ,
      iA,    iB,    iC,     iD;
  unsigned int iLog4N  = (gap8_fl1(N_fft))>>1;
  v2s *DataV  = (v2s *) Data;
  v2s *CoeffV = (v2s *) Twiddles;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;
  //int Off[] ={1,-1};

  int i;

  CoreId = gap8_coreid();

  // Layers 0,1, ... , (iLog4N-2)
  iM = N_fft >> 2; iL = 1;
  for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
    Chunk = (iM/gap8_ncore());
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
    wait_synch_barrier();
  }
  // Layer iLog4N - 2
  iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
  for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
    v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
    Chunk = (iL/gap8_ncore()); //  + Off[CoreId&0x1];
    First =  CoreId*Chunk; Last = First+Chunk;
    iA = iCnt2 + Chunk*CoreId*4*iM;
    for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
      Radix4FFTKernelDIF((v2s *) (DataV + iA       ), (v2s *) (DataV + iA + iM),
          (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
          W1, W2, W3, Inverse);
      iA = iA + 4 * iM;
    }
    iQ += iL;
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
  }

  // Layer iLog4N - 1
  iM = 1; iL = (N_fft>>2);
  Chunk = iL/gap8_ncore();
  First =  CoreId*Chunk; Last = First+Chunk;
  iA =  CoreId*Chunk*4*iM;
  for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
    Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
        (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
    iA =  iA + 4 * iM;
  }
  // Synchronize all cores for last layer of the trellis
  wait_synch_barrier();
}

void Radix4FFT_DIF_Par(FFT_Arg_T *Arg)

{
  Radix4FFT_DIF_Par_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 0);
}

void Radix4FFT_DIF_Par_Inverse(FFT_Arg_T *Arg)

{
  Radix4FFT_DIF_Par_Internal(Arg->Data, Arg->Twiddles, Arg->N_fft, 1);
}

/*
   Radix 2, Decimated in Frequency, fft.
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */

void Radix2FFT_DIF(FFT_Arg_T *Arg)

{
  signed short * __restrict__ Data = Arg->Data;
  signed short * __restrict__ Twiddles = Arg->Twiddles;
  unsigned int N_FFT2 = Arg->N_fft;
  unsigned int iLog2N  = gap8_fl1(N_FFT2);
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Twiddles;
  v2s *DataV  = (v2s *) Data;

  iL = 1;
  iM = N_FFT2 / 2;

  for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        Tmp = DataV[iA] - DataV[iB];
        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[iB] = gap8_cplxmulsdiv2(Tmp, W);
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
    v2s Tmp;
    iB = iA + 1;
    Tmp = (DataV[iA] - DataV[iB]);
    DataV[iA] = (DataV[iA] + DataV[iB]);
    DataV[iB] = Tmp;
    iA = iA + 2;
  }
}

/*
   Radix 2, Decimated in Frequency, fft. Internal version, used for FFT 2D horizontal part
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */
static inline void __attribute__ ((__always_inline__)) Radix2FFT_DIF_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT2)
{
  int iLog2N  = gap8_fl1(N_FFT2);
  int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Twiddles;
  v2s *DataV  = (v2s *) Data;

  iL = 1;
  iM = N_FFT2 / 2;

  for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        Tmp = DataV[iA] - DataV[iB];
        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[iB] = gap8_cplxmulsdiv2(Tmp, W);
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
    v2s Tmp;
    iB = iA + 1;
    Tmp = (DataV[iA] - DataV[iB]);
    DataV[iA] = (DataV[iA] + DataV[iB]);
    DataV[iB] = Tmp;
    iA = iA + 2;
  }
}

/*
   Radix 2, Decimated in Frequency, fft 2D, vertical part.
   TileW is the width of the tile currently in memory
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */

static inline void __attribute__ ((__always_inline__)) Radix2FFT_DIF_Vertical_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT2, unsigned int TileW)
{
  int iLog2N  = gap8_fl1(N_FFT2);
  int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Twiddles;
  v2s *DataV  = (v2s *) Data;

  iL = 1;
  iM = N_FFT2 / 2;

  for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        Tmp = DataV[TileW*iA] - DataV[TileW*iB];
        DataV[TileW*iA] = (DataV[TileW*iA] + DataV[TileW*iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[TileW*iB] = gap8_cplxmulsdiv2(Tmp, W);
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
    v2s Tmp;
    iB = iA + 1;
    Tmp = (DataV[TileW*iA] - DataV[TileW*iB]);
    DataV[TileW*iA] = (DataV[TileW*iA] + DataV[TileW*iB]);
    DataV[TileW*iB] = Tmp;
    iA = iA + 2;
  }
}

/*
   Radix-2 Decimated in Time FFT. Input have to be digitally-reversed, output is naturally ordered.
   First stage uses the fact that twiddles are all (1, 0)
   */
void Radix2FFT_DIT(FFT_Arg_T *Arg)

{
  signed short * __restrict__ Data = Arg->Data;
  signed short * __restrict__ Twiddles = Arg->Twiddles;
  unsigned int N_FFT2 = Arg->N_fft;
  unsigned int iLog2N  = gap8_fl1(N_FFT2);
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Twiddles;
  v2s *DataV  = (v2s *) Data;

  iL = N_FFT2 >> 1; iM = 1; iA = 0; iQ = 0;
  /* First Layer: W = (1, 0) */
  for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
    v2s Tmp;
    iB = iA + iM;
    Tmp = DataV[iB];
    DataV[iB] = (DataV[iA] - Tmp); //  >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
    DataV[iA] = (DataV[iA] + Tmp); //  >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
    iA = iA + 2;
  }
  iQ += iL; iL >>= 1; iM <<= 1;

  for (iCnt1 = 1; iCnt1 < iLog2N; ++iCnt1) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp, Tmp1;
        iB = iA + iM;
        Tmp = gap8_cplxmuls(DataV[iB], W);
        Tmp1 = DataV[iA];

        DataV[iB] = (Tmp1 - Tmp) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[iA] = (Tmp1 + Tmp) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL >>= 1;
    iM <<= 1;
  }
}

void Radix2FFT_DIF_Par(FFT_Arg_T *Arg)

{
  signed short *__restrict__ Data = Arg->Data;
  signed short *__restrict__ Twiddles = Arg->Twiddles;
  unsigned int N_fft = Arg->N_fft;

  unsigned int iLog2N  = gap8_fl1(N_fft);
  unsigned int iCnt1, iCnt2, iCnt3,
      iQ,    iL,    iM,
      iA,    iB;
  v2s *CoeffV = (v2s *) Twiddles;
  v2s *DataV  = (v2s *) Data;
  unsigned int CoreId;
  unsigned int First, Last, Chunk;

  unsigned int CNT = 0;
  unsigned int CNT1 = 0;

  CoreId = gap8_coreid();
  iL = 1;
  iM = N_fft / 2;

  Chunk = ((iLog2N-1)/gap8_ncore()); First =  CoreId*Chunk; Last = Min((First+Chunk), (iLog2N-1));

  // for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
  for (iCnt1 = 0; iCnt1 < Min((iLog2N-1), 4); iCnt1++) {
    Chunk = (iM/gap8_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
    // iQ = 0;
    iQ = First*iL;
    // for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
    for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
      v2s W = CoeffV[iQ];
      v2s A = DataV[iCnt2];
      v2s B = DataV[iCnt2+iM];
      iA = iCnt2;
      for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        // Tmp       = (DataV[iA] - DataV[iB]);
        // DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        Tmp       = (A - B);
        DataV[iA] = (A + B) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        A = DataV[iA + 2*iM]; B = DataV[iA + 3*iM];
        DataV[iB] = gap8_cplxmulsdiv2(Tmp, W);
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL <<= 1;
    iM >>= 1;
  }
  // Synchronize all cores for current layer of the trellis
  wait_synch_barrier();
  for (iCnt1 = 4; iCnt1 < (iLog2N-1); iCnt1++) {
    iQ = 0;
    for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
      v2s W = CoeffV[iQ];
      iA = iCnt2;
      Chunk = (iL/gap8_ncore()); First =  CoreId*Chunk; Last = First+Chunk;

      // for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
      for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
        v2s Tmp;
        iB = iA + iM;
        Tmp       = (DataV[iA] - DataV[iB]);
        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
        DataV[iB] = gap8_cplxmulsdiv2(Tmp, W);
        iA = iA + 2 * iM;
      }
      iQ += iL;
    }
    iL <<= 1;
    iM >>= 1;
    }
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
    iA = 0;
    /* Last Layer: W = (1, 0) */
    Chunk = ((N_fft>>1)/gap8_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
    // for (iCnt3 = 0; iCnt3 < (N_fft>>1); iCnt3++) {
    /*
       for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
       v2s Tmp     = (DataV[iA] - DataV[iA+1]);
       DataV[iA]   = (DataV[iA] + DataV[iA+1]);
       DataV[iA+1] = Tmp;
       iA = iA + 2;
       }
       */
    {
      v2s A = DataV[iA  ];
      v2s B = DataV[iA+1];
      for (iCnt3 = First; iCnt3 < Last; iCnt3+=2) {
        v2s A1 = DataV[iA+2];
        v2s B1 = DataV[iA+3];
        DataV[iA  ] = A + B;
        DataV[iA+1] = A - B;
        DataV[iA+3] = A1 + B1;
        DataV[iA+4] = A1 - B1;
        iA = iA + 4;
        A = DataV[iA  ]; B = DataV[iA+1];
      }
    }
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
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

  void SwapSamples_Par(SwapSamples_Arg_T *Arg)

  {
    v2s *__restrict__ Data = Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int i;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = gap8_coreid();

    Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
    // for (i = 0; i < Ni; i++) {
    for (i = First; i < Last; i++) {
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        v2s S = Data[i];
        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
      }
    }
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
  }

  void SwapSamples_2D_Horizontal_Par(SwapSamples_2D_Arg_T *Arg)

  {
    v2s *__restrict__ Data = Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int H = Arg->Nb;
    int i;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = gap8_coreid();

    Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
    // for (i = 0; i < Ni; i++) {
    for (i = First; i < Last; i++) {
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        int j;
        for (j=0; j<H; j++) {
          v2s S = Data[j*Ni+i];
          Data[j*Ni+i] = Data[j*Ni+SwapIndex]; Data[j*Ni+SwapIndex] = S;
        }
      }
    }
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
  }

  void SwapSamples_2D_Vertical_Par(SwapSamples_2D_Arg_T *Arg)

  {
    v2s *__restrict__ Data = Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int W = Arg->Nb;
    int i;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = gap8_coreid();

    Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
    // for (i = 0; i < Ni; i++) {
    for (i = First; i < Last; i++) {
      int SwapIndex = SwapTable[i];
      if (i < SwapIndex) {
        int j;
        for (j=0; j<W; j++) {
          v2s S = Data[i*W+j];
          Data[i*W+j] = Data[W*SwapIndex+j]; Data[W*SwapIndex+j] = S;
        }
      }
    }
    // Synchronize all cores for current layer of the trellis
    wait_synch_barrier();
  }

  /* 2D FFT */
  /* Transform a W x H image (byte) into a W x H Q15 complex plane */
  void Image2Complex(Image2Complex_Arg_T *Arg)

  {
    unsigned int i;
    unsigned char * __restrict__ In = Arg->In;
    v2s * __restrict__ Out = (v2s *) Arg->Out;
    unsigned int W = Arg->W;
    unsigned int H = Arg->H;
    unsigned int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap8_coreid();
    Chunk = ChunkSize(W*H); First =  CoreId*Chunk; Last = Min(First+Chunk, (W*H));

    // for (i=0; i<(W*H); i++) {
    for (i=First; i<Last; i++) {
      Out[i] = (v2s) {In[i], 0};
    }
    wait_synch_barrier();
  }

  void Samples2Complex(Samples2Complex_Arg_T *Arg)

  {
    void * __restrict__ Samples;            /**< Pointer to Samples */
    int SampleSize = Arg->SampleSize;
    v2s * __restrict__ Out = Arg->Out;
    unsigned int Precision = Arg->Precision;
    unsigned short int W = Arg->W;
    unsigned short int H = Arg->H;

    switch (SampleSize) {
      case 1: /* Unsigned char */
        {
          unsigned int i;
          unsigned char *In = (unsigned char *) Arg->Samples;
          for (i=0; i<W*H; i++) Out[i] = (v2s) {In[i]<<(Precision-8), 0};
        }
        break;
      case -1: /* Signed char */
        {
          unsigned int i;
          signed char *In = (signed char *) Arg->Samples;
          for (i=0; i<W*H; i++) Out[i] = (v2s) {In[i]<<(Precision-7), 0};
        }
        break;
      case 2: /* Unsigned short */
        {
          unsigned int i;
          unsigned short int *In = (unsigned short int *) Arg->Samples;
          for (i=0; i<W*H; i++) Out[i] = (v2s) {In[i]>>(16-Precision), 0};
        }
        break;
      case -2: /* Signed short */
        {
          unsigned int i;
          short int *In = (short int *) Arg->Samples;
          for (i=0; i<W*H; i++) Out[i] = (v2s) {In[i]>>(15-Precision), 0};
        }
        break;
      default: ;
    }
  }

  void Samples2Complex_Par(Samples2Complex_Arg_T *Arg)

  {
    void * __restrict__ Samples;            /**< Pointer to Samples */
    int SampleSize = Arg->SampleSize;
    v2s * __restrict__ Out = Arg->Out;
    unsigned int Precision = Arg->Precision;
    unsigned short int W = Arg->W;
    unsigned short int H = Arg->H;
    unsigned int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap8_coreid();
    Chunk = ChunkSize(W*H); First =  CoreId*Chunk; Last = Min( (unsigned int) (First+Chunk), (W*H));

    switch (SampleSize) {
      case 1: /* Unsigned char */
        {
          unsigned int i;
          unsigned char *In = (unsigned char *) Arg->Samples;
          for (i=First; i<Last; i++) Out[i] = (v2s) {In[i]<<(Precision-8), 0};
        }
        break;
      case -1: /* Signed char */
        {
          unsigned int i;
          signed char *In = (signed char *) Arg->Samples;
          for (i=First; i<Last; i++) Out[i] = (v2s) {In[i]<<(Precision-8), 0};
        }
        break;
      case 2: /* Unsigned short */
        {
          unsigned int i;
          unsigned short int *In = (unsigned short int *) Arg->Samples;
          for (i=First; i<Last; i++) Out[i] = (v2s) {In[i]>>(16-Precision), 0};
        }
        break;
      case -2: /* Signed short */
        {
          unsigned int i;
          short int *In = (short int *) Arg->Samples;
          for (i=First; i<Last; i++) Out[i] = (v2s) {In[i]>>(16-Precision), 0};
        }
        break;
      default: ;
    }
    wait_synch_barrier();
  }

  /* Horizontal part of a Radix4 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
  void Radix4FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg)

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
      Radix4FFT_DIF_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft, 0);
      // In += 2*Arg->N_fft;
    }
    wait_synch_barrier();
  }

  /* Vertical part of a Radix4 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
  void Radix4FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg)

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
      Radix4FFT_DIF_Vertical_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb, 0);
      // In += 2;
    }
    wait_synch_barrier();
  }

  /* Horizontal part of a Radix2 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
  void Radix2FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg)

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
      Radix2FFT_DIF_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft);
      // In += 2*Arg->N_fft;
    }
    wait_synch_barrier();
  }

  /* Vertical part of a Radix2 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
  void Radix2FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg)

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
      Radix2FFT_DIF_Vertical_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb);
      // In += 2;
    }
    wait_synch_barrier();
  }

  /*
     The full 2D FFT is as follow:

     Input Image: W: N_fft, H: N_fft

     for Tile in All Horizontal tiles (Input Image) {
     Radix4FFT_DIF_2D_Horizontal(Tile);
     SwapSamples_2D_Horizontal(Tile);
     }
     for Tile in All Vertical tiles (Input Image) {
     Radix4FFT_DIF_2D_Vertical(Tile);
     SwapSamples_2D_Vertical(Tile);
     }

*/

  /* Copy Twiddles and SwapLUT table from L2 to shared L1 */
  void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg)

  {
    gap8_dma_type DmaR_Evt1, DmaR_Evt2;
    int TwidSize, LUTSize;

    if (Arg->Radix == 2) TwidSize = Arg->Nfft * sizeof(short);
    else TwidSize = 3 * Arg->Nfft * (sizeof(short)/2);
    LUTSize = Arg->Nfft*sizeof(short);

    gap8_dma_memcpy(Arg->Twiddles, Arg->L1_Twiddles, TwidSize, DMA_COPY_IN, 0, &DmaR_Evt1);
    gap8_dma_memcpy(Arg->SwapLUT, Arg->L1_SwapLUT,  LUTSize,  DMA_COPY_IN, 0, &DmaR_Evt2);
    gap8_dma_wait(DmaR_Evt1);
    gap8_dma_wait(DmaR_Evt2);
  }


#ifdef BUILD_LUT
  /* Setup twiddles factors */

  void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse)

  {
    int i;
    v2s *P_Twid = (v2s *) Twiddles;

    /* Radix 4: 3/4 of the twiddles
       Radix 2: 1/2 of the twiddles
       */

    if (Inverse) {
      float Theta = (2*M_PI)/Nfft;
      for (i=0; i<Nfft; i++) {
        float Phi = Theta*i;
        P_Twid[i] = (v2s) {(short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
          (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
        // Twiddles[2*i  ] = (short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
        // Twiddles[2*i+1] = (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
      }
    } else {
      float Theta = (2*M_PI)/Nfft;
      for (i=0; i<Nfft; i++) {
        float Phi = Theta*i;
        P_Twid[i] = (v2s) {(short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
          (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
        // Twiddles[2*i  ] = (short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
        // Twiddles[2*i+1] = (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
      }
    }
  }

  /* Setup a LUT for digitally reversed indexed, base is 4 */

  void SetupR4SwapTable (short int *SwapTable, int Ni)

  {
    int iL, iM, i, j;
    int Log4N  = gap8_fl1(Ni)>>1;

    iL = Ni / 4; iM = 1;
    SwapTable[0] = 0;

    for (i = 0; i < Log4N; ++i) {
      for (j = 0; j < iM; ++j) {
        SwapTable[    iM + j] = SwapTable[j] +     iL;
        SwapTable[2 * iM + j] = SwapTable[j] + 2 * iL;
        SwapTable[3 * iM + j] = SwapTable[j] + 3 * iL;
      }
      iL >>= 2; iM <<= 2;
    }
  }

  void SetupR2SwapTable (short int *SwapTable, int Ni)

  {
    int i, j, iL, iM;
    int Log2N  = gap8_fl1(Ni);

    iL = Ni / 2;
    iM = 1;
    SwapTable[0] = 0;

    for (i = 0; i < Log2N; ++i) {
      for (j = 0; j < iM; ++j) SwapTable[j + iM] = SwapTable[j] + iL;
      iL >>= 1; iM <<= 1;
    }
  }
#endif
