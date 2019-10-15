/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Matrix add example main file. This code actually runs on GAP */
// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#include "SwapTablesDef.h"
#include "TwiddlesDef.h"

#define CORE_NUMBER         8
#define DIM                256
#define L1_BUFFER_SIZE     49152  // 40K

GAP_L2_DATA uint8_t *ImageIn;
GAP_L2_DATA int *Out;
GAP_L2_DATA performance_t cluster_perf;
char *L1_Memory;

#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))


typedef int16_t v2s __attribute__((vector_size (4)));

typedef struct {
    uint8_t * __restrict__ In; /**< Pointer to a byte image (or image tile) */
    int * __restrict__ Out;    /**< Pointer to the (I, Q) expansion of In, I=0, Q=In */
    uint16_t W;                /**< Image width */
    uint16_t H;                /**< Image height */
} Image2Complex_Arg_T;

typedef struct {
        int16_t * __restrict__ Data;/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        int16_t * __restrict__ Twiddles;/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
        uint16_t N_fft;
        uint16_t Nb;
} FFT_2D_Arg_T;

typedef struct {
        v2s *__restrict__ Data;/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        int16_t *__restrict__ SwapTable;/**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        uint16_t Ni;/**< FFT dimension */
        uint16_t Nb;/**< Number of FFT rows or column */
} SwapSamples_2D_Arg_T;

inline static int gap_ncore() {return CORE_NUMBER;}
inline static uint32_t __attribute__((always_inline)) ChunkSize(uint32_t X)
{
    uint32_t Log2Core = __FL1(CORE_NUMBER);
    uint32_t Chunk = (X >> Log2Core) + ((X & (CORE_NUMBER - 1)) != 0);
    return Chunk;
}

#define FFT4_SCALEDOWN 2
#define FFT2_SCALEDOWN 1

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
  int iLog2N  = __FL1(N_FFT2);
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
        DataV[TileW*iB] = __CPLX_MULS_DIV2(Tmp, W);
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

  /* Vertical part of a Radix2 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
  void Radix2FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg)

  {
    int i;
    signed short int * __restrict__ In = Arg->Data;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = __core_ID();
    Chunk = ChunkSize(Arg->Nb);
    First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

    // for (i=0; i<Arg->Nb; i++) {
    for (i=First; i<Last; i++) {
      Radix2FFT_DIF_Vertical_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb);
      // In += 2;
    }
    CLUSTER_SynchBarrier();
  }

static void Radix4FFTKernelDIF(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
    v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
  v2s A1, B1, C1, D1;
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  if (Inverse) {
    A1 = ((A + C) +               (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) - __SUB2_ROTMJ(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) + __SUB2_ROTMJ(B,  D));
  } else {
    A1 = ((A + C) +               (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
    B1 = ((A - C) + __SUB2_ROTMJ(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) - __SUB2_ROTMJ(B,  D));
  }

  B1 = __CPLX_MULS_DIV4(B1, W1); C1 = __CPLX_MULS_DIV4(C1, W2); D1 = __CPLX_MULS_DIV4(D1, W3);
  *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}


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
   *InOutB = ((A - C) + __SUB2_ROTMJ(B,  D));
   *InOutC = ((A + C) -	           (B + D ));
   *InOutD = ((A - C) - __SUB2_ROTMJ(B,  D));
   */

  if (Inverse) {
    *InOutA = ((A + C) +	           (B + D));
    *InOutB = ((A - C) - __SUB2_ROTMJ(B,  D));
    *InOutC = ((A + C) -	           (B + D));
    *InOutD = ((A - C) + __SUB2_ROTMJ(B,  D));
  } else {
    *InOutA = ((A + C) +	           (B + D));
    *InOutB = ((A - C) + __SUB2_ROTMJ(B,  D));
    *InOutC = ((A + C) -	           (B + D));
    *InOutD = ((A - C) - __SUB2_ROTMJ(B,  D));
  }
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
  int iLog4N  = __FL1(N_FFT4)>>1;
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
  int iLog4N  = __FL1(N_FFT4)>>1;
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

/*
   Radix 2, Decimated in Frequency, fft. Internal version, used for FFT 2D horizontal part
   Input are natural order, output is digitally-reversed.
   The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
   */
static inline void __attribute__ ((__always_inline__)) Radix2FFT_DIF_Internal(signed short *__restrict__ Data,
    signed short *__restrict__ Twiddles,
    int N_FFT2)
{
  int iLog2N  = __FL1(N_FFT2);
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
        DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
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

  /* 2D FFT */
  /* Transform a W x H image (byte) into a W x H Q15 complex plane */
  void Image2Complex(Image2Complex_Arg_T *Arg)

  {
    int i;
    unsigned char * __restrict__ In = Arg->In;
    v2s * __restrict__ Out = (v2s *) Arg->Out;
    unsigned int W = Arg->W;
    unsigned int H = Arg->H;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = __core_ID();
    Chunk = ((W*H)/gap_ncore()); First =  CoreId*Chunk; Last = Min( (unsigned int) (First+Chunk), (W*H));

    // for (i=0; i<(W*H); i++) {
    for (i=First; i<Last; i++) {
      Out[i] = (v2s) {In[i], 0};
    }
    CLUSTER_SynchBarrier();
  }


  /* Horizontal part of a Radix4 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
  void Radix4FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg)

  {
    int i;
    signed short int * __restrict__ In = Arg->Data;
    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = __core_ID();
    Chunk = ChunkSize(Arg->Nb);
    First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

    // for (i=0; i<Arg->Nb; i++) {
    for (i=First; i<Last; i++) {
      Radix4FFT_DIF_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft, 0);
      // In += 2*Arg->N_fft;
    }
    CLUSTER_SynchBarrier();
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

    CoreId = __core_ID();

    Chunk = (Ni/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
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
    CLUSTER_SynchBarrier();
  }

  /* Vertical part of a Radix4 2D FFT (in place), W=Arg->Nb, H=Arg->N_fft */
  void Radix4FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg)

  {
    int i;
    signed short int * __restrict__ In = Arg->Data;

    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = __core_ID();
    Chunk = ChunkSize(Arg->Nb);
    First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

    // for (i=0; i<Arg->Nb; i++) {
    for (i=First; i<Last; i++) {
      Radix4FFT_DIF_Vertical_Internal(In + (2*i), Arg->Twiddles, Arg->N_fft, Arg->Nb, 0);
      // In += 2;
    }
    CLUSTER_SynchBarrier();
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

    CoreId = __core_ID();

    Chunk = (Ni/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
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
    CLUSTER_SynchBarrier();
  }

  /* Horizontal part of a Radix2 2D FFT (in place), W=Arg->N_fft, H=Arg->Nb */
  void Radix2FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg)

  {
    int i;
    signed short int * __restrict__ In = Arg->Data;

    unsigned int CoreId;
    int First, Last, Chunk;

    CoreId = __core_ID();
    Chunk = ChunkSize(Arg->Nb);
    First =  CoreId*Chunk; Last = Min(First+Chunk, Arg->Nb);

    // for (i=0; i<Arg->Nb; i++) {
    for (i=First; i<Last; i++) {
      Radix2FFT_DIF_Internal(In + (2*Arg->N_fft*i), Arg->Twiddles, Arg->N_fft);
      // In += 2*Arg->N_fft;
    }
    CLUSTER_SynchBarrier();
  }

void FFT2D_256Horizontal(
    uint8_t * __restrict__ In,
    v2s * __restrict__ Out,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    /* Local variables used by this kernel */
    dma_req_t DmaR_Evt1;
    dma_req_t DmaW_Evt1;
    int Iter;
    int Last, NextLast, NextNextLast;
    int N_Ti = 0;
    int N_TiIp = 0, InPlane, OutPlane=0;
    Image2Complex_Arg_T S_KerArg0, *KerArg0 = &S_KerArg0;
    FFT_2D_Arg_T S_KerArg1, *KerArg1 = &S_KerArg1;
    SwapSamples_2D_Arg_T S_KerArg2, *KerArg2 = &S_KerArg2;

    /* Initialize KerArg, Kernel invariant arguments */
    KerArg0->W = (uint16_t) (256);
    KerArg0->H = (uint16_t) (16);
    KerArg1->Twiddles = (Twiddles);
    KerArg1->N_fft = (uint16_t) (256);
    KerArg1->Nb = (uint16_t) (16);
    KerArg2->SwapTable = (SwapTable);
    KerArg2->Ni = (uint16_t) (256);
    KerArg2->Nb = (uint16_t) (16);
    /* =======================Read First Tile=========================================== */
    /* Initial reads in L2, O_DB or O_BUFF */
    DMAMCHAN_Memcpy_1D((unsigned int) In+(0), (unsigned int) (L1_Memory + 0)+0, 4096, GAP_DMA_L22TCDM, &DmaR_Evt1);
    /* ===================End Read First Tile=========================================== */
    /* Kernel Iteration Loop on tiled inner space 0 */
    for (Iter=0; Iter<16; Iter++) {
        /* Loop Iteration Body on tiled inner space 0 */
        /* Elaborate Last, Next_Last, Next_Next_Last */
        Last = ((Iter+1) == 16); NextLast = ((Iter+2) == 16); NextNextLast = ((Iter+3) == 16);
        /* =======================Read Next Tile=========================================== */
        DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
        if (!Last) {
            DMAMCHAN_Memcpy_1D((unsigned int) In + ((Iter+1)*4096),
                          (unsigned int) (L1_Memory + 0) + 4096*((N_Ti+1) % 2), 4096, GAP_DMA_L22TCDM, &DmaR_Evt1);
        }
        /* ===================End Read Next Tile=========================================== */
        /* Call Kernel LOC_INNER_LOOP */
        KerArg0->In = (uint8_t * __restrict__) ((unsigned int) (L1_Memory + 0) + 4096*((N_Ti) % 2));
        KerArg0->Out = (int * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) Image2Complex, (void *) KerArg0);
        KerArg1->Data = (int16_t * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) Radix4FFT_DIF_2D_Horizontal, (void *) KerArg1);
        KerArg2->Data = (v2s * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) SwapSamples_2D_Horizontal_Par, (void *) KerArg2);
        /* =======================Write Tile=========================================== */
        if (Iter) {
            DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
        }
        DMAMCHAN_Memcpy_1D((unsigned int) Out + ((Iter)*16384),
                      (unsigned int) (L1_Memory + 8192) + 16384*(N_Ti % 2), 16384, GAP_DMA_TCDM2L2, &DmaW_Evt1);
        /* ===================End Write Tile=========================================== */
        N_Ti++;
        /* End Kernel Iteration Loop on tiled inner space 0 */
    }
    Iter=16;
    /* =======================Write Last Tile=========================================== */
    DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
    /* ===================End Write Last Tile=========================================== */
}

void FFT2D_256Vertical(
    int16_t * __restrict__ InOut,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    /* Local variables used by this kernel */
    dma_req_t DmaR_Evt1;
    dma_req_t DmaW_Evt1;
    int Iter;
    int Last, NextLast, NextNextLast;
    int N_Ti = 0;
    int N_TiIp = 0, InPlane, OutPlane=0;
    FFT_2D_Arg_T S_KerArg0, *KerArg0 = &S_KerArg0;
    SwapSamples_2D_Arg_T S_KerArg1, *KerArg1 = &S_KerArg1;

    /* Initialize KerArg, Kernel invariant arguments */
    KerArg0->Twiddles = (Twiddles);
    KerArg0->N_fft = (uint16_t) (256);
    KerArg0->Nb = (uint16_t) (16);
    KerArg1->SwapTable = (SwapTable);
    KerArg1->Ni = (uint16_t) (256);
    KerArg1->Nb = (uint16_t) (16);
    /* =======================Read First Tile=========================================== */
    /* Initial reads in L2, O_DB or O_BUFF */
    DMAMCHAN_Memcpy_1D((unsigned int) InOut+(0), (unsigned int) (L1_Memory + 0)+0, 16384, GAP_DMA_L22TCDM, &DmaR_Evt1);
    /* ===================End Read First Tile=========================================== */
    /* Kernel Iteration Loop on tiled inner space 0 */
    for (Iter=0; Iter<16; Iter++) {
        /* Loop Iteration Body on tiled inner space 0 */
        /* Elaborate Last, Next_Last, Next_Next_Last */
        Last = ((Iter+1) == 16); NextLast = ((Iter+2) == 16); NextNextLast = ((Iter+3) == 16);
        /* =======================Read Next Tile=========================================== */
        DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
        if (!Last) {
            DMAMCHAN_Memcpy_1D((unsigned int) InOut + ((Iter+1)*64),
                          (unsigned int) (L1_Memory + 0) + 16384*((N_Ti+1) % 3), 16384, GAP_DMA_L22TCDM, &DmaR_Evt1);
        }
        /* ===================End Read Next Tile=========================================== */
        /* Call Kernel LOC_INNER_LOOP */
        KerArg0->Data = (int16_t * __restrict__) ((unsigned int) (L1_Memory + 0) + 16384*((N_Ti) % 3));
        CLUSTER_CoresFork((void *) Radix4FFT_DIF_2D_Vertical, (void *) KerArg0);
        KerArg1->Data = (v2s * __restrict__) ((unsigned int) (L1_Memory + 0) + 16384*((N_Ti) % 3));
        CLUSTER_CoresFork((void *) SwapSamples_2D_Vertical_Par, (void *) KerArg1);
        /* =======================Write Tile=========================================== */
        if (Iter) {
            DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
        }
        DMAMCHAN_Memcpy_1D((unsigned int) InOut + ((Iter)*64),
                      (unsigned int) (L1_Memory + 0) + 16384*(N_Ti % 3), 16384, GAP_DMA_TCDM2L2, &DmaW_Evt1);
        /* ===================End Write Tile=========================================== */
        N_Ti++;
        /* End Kernel Iteration Loop on tiled inner space 0 */
    }
    Iter=16;
    /* =======================Write Last Tile=========================================== */
    DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
    /* ===================End Write Last Tile=========================================== */
}

void FFT2D_128Horizontal(
    uint8_t * __restrict__ In,
    v2s * __restrict__ Out,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    /* Local variables used by this kernel */
    dma_req_t DmaR_Evt1;
    dma_req_t DmaW_Evt1;
    int Iter;
    int Last, NextLast, NextNextLast;
    int N_Ti = 0;
    int N_TiIp = 0, InPlane, OutPlane=0;
    Image2Complex_Arg_T S_KerArg0, *KerArg0 = &S_KerArg0;
    FFT_2D_Arg_T S_KerArg1, *KerArg1 = &S_KerArg1;
    SwapSamples_2D_Arg_T S_KerArg2, *KerArg2 = &S_KerArg2;

    /* Initialize KerArg, Kernel invariant arguments */
    KerArg0->W = (uint16_t) (128);
    KerArg0->H = (uint16_t) (32);
    KerArg1->Twiddles = (Twiddles);
    KerArg1->N_fft = (uint16_t) (128);
    KerArg1->Nb = (uint16_t) (32);
    KerArg2->SwapTable = (SwapTable);
    KerArg2->Ni = (uint16_t) (128);
    KerArg2->Nb = (uint16_t) (32);
    /* =======================Read First Tile=========================================== */
    /* Initial reads in L2, O_DB or O_BUFF */
    DMAMCHAN_Memcpy_1D((unsigned int) In+(0), (unsigned int) (L1_Memory + 0)+0, 4096, GAP_DMA_L22TCDM, &DmaR_Evt1);
    /* ===================End Read First Tile=========================================== */
    /* Kernel Iteration Loop on tiled inner space 0 */
    for (Iter=0; Iter<4; Iter++) {
        /* Loop Iteration Body on tiled inner space 0 */
        /* Elaborate Last, Next_Last, Next_Next_Last */
        Last = ((Iter+1) == 4); NextLast = ((Iter+2) == 4); NextNextLast = ((Iter+3) == 4);
        /* =======================Read Next Tile=========================================== */
        DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
        if (!Last) {
            DMAMCHAN_Memcpy_1D((unsigned int) In + ((Iter+1)*4096),
                          (unsigned int) (L1_Memory + 0) + 4096*((N_Ti+1) % 2), 4096, GAP_DMA_L22TCDM, &DmaR_Evt1);
        }
        /* ===================End Read Next Tile=========================================== */
        /* Call Kernel LOC_INNER_LOOP */
        KerArg0->In = (uint8_t * __restrict__) ((unsigned int) (L1_Memory + 0) + 4096*((N_Ti) % 2));
        KerArg0->Out = (int * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) Image2Complex, (void *) KerArg0);
        KerArg1->Data = (int16_t * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) Radix2FFT_DIF_2D_Horizontal, (void *) KerArg1);
        KerArg2->Data = (v2s * __restrict__) ((unsigned int) (L1_Memory + 8192) + 16384*((N_Ti) % 2));
        CLUSTER_CoresFork((void *) SwapSamples_2D_Horizontal_Par, (void *) KerArg2);
        /* =======================Write Tile=========================================== */
        if (Iter) {
            DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
        }
        DMAMCHAN_Memcpy_1D((unsigned int) Out + ((Iter)*16384),
                      (unsigned int) (L1_Memory + 8192) + 16384*(N_Ti % 2), 16384, GAP_DMA_TCDM2L2, &DmaW_Evt1);
        /* ===================End Write Tile=========================================== */
        N_Ti++;
        /* End Kernel Iteration Loop on tiled inner space 0 */
    }
    Iter=4;
    /* =======================Write Last Tile=========================================== */
    DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
    /* ===================End Write Last Tile=========================================== */
}

void FFT2D_128Vertical(
    int16_t * __restrict__ InOut,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    /* Local variables used by this kernel */
    dma_req_t DmaR_Evt1;
    dma_req_t DmaW_Evt1;
    int Iter;
    int Last, NextLast, NextNextLast;
    int N_Ti = 0;
    int N_TiIp = 0, InPlane, OutPlane=0;
    FFT_2D_Arg_T S_KerArg0, *KerArg0 = &S_KerArg0;
    SwapSamples_2D_Arg_T S_KerArg1, *KerArg1 = &S_KerArg1;

    /* Initialize KerArg, Kernel invariant arguments */
    KerArg0->Twiddles = (Twiddles);
    KerArg0->N_fft = (uint16_t) (128);
    KerArg0->Nb = (uint16_t) (32);
    KerArg1->SwapTable = (SwapTable);
    KerArg1->Ni = (uint16_t) (128);
    KerArg1->Nb = (uint16_t) (32);
    /* =======================Read First Tile=========================================== */
    /* Initial reads in L2, O_DB or O_BUFF */
    DMAMCHAN_Memcpy_1D((unsigned int) InOut+(0), (unsigned int) (L1_Memory + 0)+0, 16384, GAP_DMA_L22TCDM, &DmaR_Evt1);
    /* ===================End Read First Tile=========================================== */
    /* Kernel Iteration Loop on tiled inner space 0 */
    for (Iter=0; Iter<4; Iter++) {
        /* Loop Iteration Body on tiled inner space 0 */
        /* Elaborate Last, Next_Last, Next_Next_Last */
        Last = ((Iter+1) == 4); NextLast = ((Iter+2) == 4); NextNextLast = ((Iter+3) == 4);
        /* =======================Read Next Tile=========================================== */
        DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
        if (!Last) {
            DMAMCHAN_Memcpy_1D((unsigned int) InOut + ((Iter+1)*128),
                          (unsigned int) (L1_Memory + 0) + 16384*((N_Ti+1) % 3), 16384, GAP_DMA_L22TCDM, &DmaR_Evt1);
        }
        /* ===================End Read Next Tile=========================================== */
        /* Call Kernel LOC_INNER_LOOP */
        KerArg0->Data = (int16_t * __restrict__) ((unsigned int) (L1_Memory + 0) + 16384*((N_Ti) % 3));
        CLUSTER_CoresFork((void *) Radix2FFT_DIF_2D_Vertical, (void *) KerArg0);
        KerArg1->Data = (v2s * __restrict__) ((unsigned int) (L1_Memory + 0) + 16384*((N_Ti) % 3));
        CLUSTER_CoresFork((void *) SwapSamples_2D_Vertical_Par, (void *) KerArg1);
        /* =======================Write Tile=========================================== */
        if (Iter) {
            DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
        }
        DMAMCHAN_Memcpy_1D((unsigned int) InOut + ((Iter)*128),
                      (unsigned int) (L1_Memory + 0) + 16384*(N_Ti % 3), 16384, GAP_DMA_TCDM2L2, &DmaW_Evt1);
        /* ===================End Write Tile=========================================== */
        N_Ti++;
        /* End Kernel Iteration Loop on tiled inner space 0 */
    }
    Iter=4;
    /* =======================Write Last Tile=========================================== */
    DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
    /* ===================End Write Last Tile=========================================== */
}

void FFT2D_256(
    uint8_t * __restrict__ In,
    int * __restrict__ Out,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    FFT2D_256Horizontal(
        (uint8_t * __restrict__) (In),
        (v2s * __restrict__) (Out),
        (int16_t * __restrict__) (Twiddles),
        (int16_t * __restrict__) (SwapTable)
        );
    FFT2D_256Vertical(
        (int16_t * __restrict__) (Out),
        (int16_t * __restrict__) (Twiddles),
        (int16_t * __restrict__) (SwapTable)
        );
}

void FFT2D_128(
    uint8_t * __restrict__ In,
    int * __restrict__ Out,
    int16_t * __restrict__ Twiddles,
    int16_t * __restrict__ SwapTable)

{
    FFT2D_128Horizontal(
        (uint8_t * __restrict__) (In),
        (v2s * __restrict__) (Out),
        (int16_t * __restrict__) (Twiddles),
        (int16_t * __restrict__) (SwapTable)
        );
    FFT2D_128Vertical(
        (int16_t * __restrict__) (Out),
        (int16_t * __restrict__) (Twiddles),
        (int16_t * __restrict__) (SwapTable)
        );
}

static void fft_2d_test() {
    FFT2D_128(ImageIn,
              Out,
              R2_Twiddles_128,
              R2_SwapTable_128);

    FFT2D_256(ImageIn,
              Out,
              R4_Twiddles_256,
              R4_SwapTable_256);
}

static void fft_2d_init(){
    for(int i = 0; i < DIM*DIM; i++){
        ImageIn[i] = 1;
        Out[i] = 0;
    }
}

int main()
{
    printf("Fabric controller code execution for mbed_os matrix addition test\n");

    ImageIn = malloc(DIM * DIM * sizeof(char));
    Out = malloc(DIM * DIM * sizeof(int));

    /* Cluster Start - power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* Allocate a buffer in the shared L1 memory */
    L1_Memory = L1_Malloc(L1_BUFFER_SIZE);

    /* Allocation check */
    if(L1_Memory == NULL ){
        printf("Memory Allocation Error! Quit...\n");
        return 1;
    }

    fft_2d_init();

    /* FC send a task to cluster */
    CLUSTER_SendTask(0, fft_2d_test, 0, 0);

    /* Cluster Stop - power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
