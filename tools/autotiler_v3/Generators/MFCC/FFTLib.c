#include "Gap.h"
#include "FFTLib.h"


/* Number of points for the Radix4 FFT */

/*
        Input samples are in Q<FFT4_SAMPLE_DYN>
        Coeff are in Q<FFT_TWIDDLE_DYN>

        Out samples in Q<FFT4_SAMPLE_DYN>
*/

// #define cplxmuls(x, y) (v2s) {(x[0]*y[0] - x[1]*y[1])>>15, (x[0]*y[1]+x[1]*y[0])>>15}
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

/* Reorder from natural indexes to digitally-reversed one. Uses a pre computed LUT */
/* data is complex int */
/* reorder the exponet table */ 
void SwapSamples_scal( SwapSamples_scal_Arg_T *Arg)
{
  int i;
  
  cmplx *__restrict__ Datav = (cmplx *) Arg->Data;
  signed char *scale = (signed char*) Arg->shift_fft;
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
      printf("\nout_fft_swapped_c = np.array([\n\t");
      for (int j=0; j<Ni; j++) {
        if (Datav[j][1]<0) printf("%d%dj, ",  Datav[j][0], Datav[j][1]);
        else               printf("%d+%dj, ", Datav[j][0], Datav[j][1]);
      }
      printf("])\nout_fft_shift_swapped_c = np.array([\n\t");
      for (int j=0; j<Ni; j++) printf("%d, ", scale[j]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

void SwapSamples_Par_Fix32( FFT_SwapSamples_T *Arg)
{
  int i;
  
  cmplx *__restrict__ Datav = (cmplx *) Arg->Data;
  short *__restrict__ SwapTable = Arg->SwapTable;
  int Ni = Arg->Ni;

  unsigned int CoreId;
  int First, Last, Chunk;

  CoreId = gap_coreid();

  Chunk = ChunkSize(Ni); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
  
  for (i = First; i < Last; i++) {
    cmplx S =  Datav[i];
    int SwapIndex = SwapTable[i];
    if (i < SwapIndex) {
        Datav[i] = Datav[SwapIndex]; Datav[SwapIndex] = S;
    }
  }

  gap_waitbarrier(0);

  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_swapped_fft = np.array([\n\t");
      for (int j=0; j<Ni; j++) {
        if (Datav[j][1]<0) printf("%d%dj, ",  Datav[j][0], Datav[j][1]);
        else               printf("%d+%dj, ", Datav[j][0], Datav[j][1]);
      }
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

static void Radix4FFTKernel_Twiddle0(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, int Inverse)

{
        v2s A = *InOutA;
        v2s B = *InOutB;
        v2s C = *InOutC;
        v2s D = *InOutD;

        if (Inverse) {
                *InOutA = ((A + C) + (B  + D ));
                *InOutB = ((A - C) - gap_sub2rotmj(B, D));
                *InOutC = ((A + C) - (B  + D ));
                *InOutD = ((A - C) + gap_sub2rotmj(B, D));
        } else {
                *InOutA = ((A + C) + (B  + D ));
                *InOutB = ((A - C) + gap_sub2rotmj(B, D));
                *InOutC = ((A + C) - (B  + D ));
                *InOutD = ((A - C) - gap_sub2rotmj(B, D));
        }

        A = *InOutA;
        B = *InOutB;
        C = *InOutC;
        D = *InOutD;
}

static void Radix4FFTKernel_Twiddle0_f32(float *InOutA, float *InOutB, float *InOutC, float *InOutD, unsigned int Inverse)

{
        float Ar = InOutA[0];
        float Ai = InOutA[1];
        float Br = InOutB[0];
        float Bi = InOutB[1];
        float Cr = InOutC[0];
        float Ci = InOutC[1];
        float Dr = InOutD[0];
        float Di = InOutD[1];

        float B1r, D1r;
        float B1i, D1i;

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

#ifdef __gap9__
static void Radix4FFTKernel_Twiddle0_f16(v2h *InOutA, v2h *InOutB, v2h *InOutC, v2h *InOutD, int Inverse)

{
        v2h A = *InOutA;
        v2h B = *InOutB;
        v2h C = *InOutC;
        v2h D = *InOutD;

        v2h rotmjB = B-D;
        rotmjB = __builtin_shuffle(rotmjB, (v2s){1, 0});
        rotmjB = rotmjB * (v2h){1.0, -1.0}; // --> rotmjB = {Bi - Di, -(Br - Dr)}

        if (Inverse) {
                *InOutA = ((A + C) + (B  + D ));
                *InOutB = ((A - C) - rotmjB);
                *InOutC = ((A + C) - (B  + D ));
                *InOutD = ((A - C) + rotmjB);
        } else {
                *InOutA = ((A + C) + (B  + D ));
                *InOutB = ((A - C) + rotmjB);
                *InOutC = ((A + C) - (B  + D ));
                *InOutD = ((A - C) - rotmjB);
        }

        A = *InOutA;
        B = *InOutB;
        C = *InOutC;
        D = *InOutD;
}

static inline v2h CplxMult_f16(v2h A, v2h B)

{
        v2h P0, P1, P2, P3;

        P0 = A*B;
        B = __builtin_shuffle(B, (v2s){1,0});
        P1 = A*B;
        P2 = __builtin_shuffle(P0, P1, (v2s){0,2});
        P3 = __builtin_shuffle(P0, P1, (v2s){1,3});
        P3 = P3 * (v2h) {-1.0, 1.0};
        return P3+P2;
}

static inline v2h CplxMult_f16_Bis(v2h A, v2h B)

{
        f16 Ar=A[0];
        f16 Ai=A[1];
        f16 Br=A[0];
        f16 Bi=B[1];

        return (v2h){(Ar*Br-Ai*Bi), (Ar*Bi+Ai*Br)};
}

static void Radix4FFTKernelDIF_f16(v2h *InOutA, v2h *InOutB, v2h *InOutC, v2h *InOutD,
                                   v2h W1, v2h W2, v2h W3, int Inverse)

{
        v2h A = *InOutA;
        v2h B = *InOutB;
        v2h C = *InOutC;
        v2h D = *InOutD;

        v2h A1, B1, C1, D1, B2, D2;
        v2h rotmjB = B-D;

        rotmjB = __builtin_shuffle(rotmjB, (v2s){1, 0});
        rotmjB = rotmjB * (v2h){1.0, -1.0}; // --> rotmjB = {Bi - Di, -(Br - Dr)}

        if (Inverse) {
                A1 = ((A + C) + (B + D));
                B1 = ((A - C) - rotmjB);
                C1 = ((A + C) - (B + D));
                D1 = ((A - C) + rotmjB);
        } else {
                A1 = ((A + C) + (B + D)); // A1 = {(Ar + Cr) + (Br + Dr), (Ai + Ci) + (Bi + Di)}
                B1 = ((A - C) + rotmjB ); // B1 = {(Ar - Cr) + (Bi - Di), (Ai - Ci) - (Br - Dr)}
                C1 = ((A + C) - (B + D)); // C1 = {(Ar + Cr) - (Br + Dr), (Ai + Ci) - (Bi + Di)}
                D1 = ((A - C) - rotmjB ); // D1 = {(Ar - Cr) - (Bi - Di), (Ai - Ci) + (Br - Dr)}
        }

        B1 = CplxMult_f16(B1, W1); C1 = CplxMult_f16(C1, W2); D1 = CplxMult_f16(D1, W3);

        *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}
#endif

static void Radix4FFTKernelDIF_f32(float *InOutA, float *InOutB, float *InOutC, float *InOutD,
                                   float W1r, float W1i, float W2r, float W2i, float W3r, float W3i, unsigned int Inverse)

{
        float Ar = InOutA[0];
        float Ai = InOutA[1];
        float Br = InOutB[0];
        float Bi = InOutB[1];
        float Cr = InOutC[0];
        float Ci = InOutC[1];
        float Dr = InOutD[0];
        float Di = InOutD[1];

        float A1r, A1i, B1r, B1i, C1r, C1i, D1r, D1i;
        float Tmp; //B2r, B2i, D2r, D2i,

        // B2r = Bi; B2i = -Br;
        // D2r = Di; D2i = -Dr;

        if (Inverse) {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) - (Bi  -  Di));
                B1i = ((Ai - Ci) + (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) + (Bi  -  Di));
                D1i = ((Ai - Ci) - (Br  -  Dr));
        } else {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) + (Bi  -  Di));
                B1i = ((Ai - Ci) - (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) - (Bi  -  Di));
                D1i = ((Ai - Ci) + (Br  -  Dr));
        }

        Tmp = B1r; B1r = (B1r*W1r - B1i*W1i); B1i = (Tmp*W1i + B1i*W1r);
        Tmp = C1r; C1r = (C1r*W2r - C1i*W2i); C1i = (Tmp*W2i + C1i*W2r);
        Tmp = D1r; D1r = (D1r*W3r - D1i*W3i); D1i = (Tmp*W3i + D1i*W3r);

        InOutA[0] = A1r;
        InOutA[1] = A1i;
        InOutB[0] = B1r;
        InOutB[1] = B1i;
        InOutC[0] = C1r;
        InOutC[1] = C1i;
        InOutD[0] = D1r;
        InOutD[1] = D1i;

}

static void Radix4FFTKernelDIF(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
                               v2s W1, v2s W2, v2s W3, int Inverse)

{
        v2s A = *InOutA;
        v2s B = *InOutB;
        v2s C = *InOutC;
        v2s D = *InOutD;

        int B1r, B1i, C1r, C1i, D1r, D1i;
        v2s A1, B1, C1, D1, B2, D2;

        if (Inverse) {
                A1 = ((A + C) + (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
                B1 = ((A - C) - gap_sub2rotmj(B,  D));
                C1 = ((A + C) - (B + D));
                D1 = ((A - C) + gap_sub2rotmj(B,  D));
        } else {
                A1 = ((A + C) + (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
                B1 = ((A - C) + gap_sub2rotmj(B,  D));
                C1 = ((A + C) - (B + D));
                D1 = ((A - C) - gap_sub2rotmj(B,  D));
        }

        B1 = gap_cplxmulsdiv4(B1, W1); C1 = gap_cplxmulsdiv4(C1, W2); D1 = gap_cplxmulsdiv4(D1, W3);

        *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}

static void Radix4FFTKernelDIF_NoScale(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
    v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
  v2s A1, B1, C1, D1;
  v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

  if (Inverse) {
    A1 = ((A + C) +               (B + D));
    B1 = ((A - C) - gap_sub2rotmj(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) + gap_sub2rotmj(B,  D));
  } else {
    A1 = ((A + C) +               (B + D));
    B1 = ((A - C) + gap_sub2rotmj(B,  D));
    C1 = ((A + C) -               (B + D));
    D1 = ((A - C) - gap_sub2rotmj(B,  D));
  }

  B1 = gap_cplxmuls(B1, W1); C1 = gap_cplxmuls(C1, W2); D1 = gap_cplxmuls(D1, W3);
  *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}

static void Radix4FFTKernelDIF_Fix32(int *InOutA, int *InOutB, int *InOutC, int *InOutD,
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
        int Tmp; //B2r, B2i, D2r, D2i,

        // B2r = Bi; B2i = -Br;
        // D2r = Di; D2i = -Dr;

        if (Inverse) {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) - (Bi  -  Di));
                B1i = ((Ai - Ci) + (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) + (Bi  -  Di));
                D1i = ((Ai - Ci) - (Br  -  Dr));
        } else {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) + (Bi  -  Di));
                B1i = ((Ai - Ci) - (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) - (Bi  -  Di));
                D1i = ((Ai - Ci) + (Br  -  Dr));
        }

        Tmp = B1r >> FFT4_SCALEDOWN; B1r = (((B1r*W1r)>>15) - ((B1i*W1i)>>15)) >> FFT4_SCALEDOWN; B1i = (((Tmp*W1i)>>15) + ((B1i*W1r)>>15)) >> FFT4_SCALEDOWN;
        Tmp = C1r >> FFT4_SCALEDOWN; C1r = (((C1r*W2r)>>15) - ((C1i*W2i)>>15)) >> FFT4_SCALEDOWN; C1i = (((Tmp*W2i)>>15) + ((C1i*W2r)>>15)) >> FFT4_SCALEDOWN;
        Tmp = D1r >> FFT4_SCALEDOWN; D1r = (((D1r*W3r)>>15) - ((D1i*W3i)>>15)) >> FFT4_SCALEDOWN; D1i = (((Tmp*W3i)>>15) + ((D1i*W3r)>>15)) >> FFT4_SCALEDOWN;

        InOutA[0] = A1r;
        InOutA[1] = A1i;
        InOutB[0] = B1r;
        InOutB[1] = B1i;
        InOutC[0] = C1r;
        InOutC[1] = C1i;
        InOutD[0] = D1r;
        InOutD[1] = D1i;

}
static void Radix4FFTKernelDIF_Fix32_NoScale(int *InOutA, int *InOutB, int *InOutC, int *InOutD,
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
        int Tmp; //B2r, B2i, D2r, D2i,

        // B2r = Bi; B2i = -Br;
        // D2r = Di; D2i = -Dr;

        if (Inverse) {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) - (Bi  -  Di));
                B1i = ((Ai - Ci) + (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) + (Bi  -  Di));
                D1i = ((Ai - Ci) - (Br  -  Dr));
        } else {
                A1r = ((Ar + Cr) + (Br  +  Dr));
                A1i = ((Ai + Ci) + (Bi  +  Di));
                B1r = ((Ar - Cr) + (Bi  -  Di));
                B1i = ((Ai - Ci) - (Br  -  Dr));
                C1r = ((Ar + Cr) - (Br  +  Dr));
                C1i = ((Ai + Ci) - (Bi  +  Di));
                D1r = ((Ar - Cr) - (Bi  -  Di));
                D1i = ((Ai - Ci) + (Br  -  Dr));
        }

        Tmp = B1r; B1r = (((B1r*W1r)>>15) - ((B1i*W1i)>>15)); B1i = (((Tmp*W1i)>>15) + ((B1i*W1r)>>15));
        Tmp = C1r; C1r = (((C1r*W2r)>>15) - ((C1i*W2i)>>15)); C1i = (((Tmp*W2i)>>15) + ((C1i*W2r)>>15));
        Tmp = D1r; D1r = (((D1r*W3r)>>15) - ((D1i*W3i)>>15)); D1i = (((Tmp*W3i)>>15) + ((D1i*W3r)>>15));

        InOutA[0] = A1r;
        InOutA[1] = A1i;
        InOutB[0] = B1r;
        InOutB[1] = B1i;
        InOutC[0] = C1r;
        InOutC[1] = C1i;
        InOutD[0] = D1r;
        InOutD[1] = D1i;
}
static void Radix4FFTKernel_Twiddle0_Fix32(int *InOutA, int *InOutB, int *InOutC, int *InOutD, unsigned int Inverse)

{
        int Ar = InOutA[0];
        int Ai = InOutA[1];
        int Br = InOutB[0];
        int Bi = InOutB[1];
        int Cr = InOutC[0];
        int Ci = InOutC[1];
        int Dr = InOutD[0];
        int Di = InOutD[1];

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

/*
  Radix 4, Decimated in Frequency, fft. Sequential implementation.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/

void Radix4FFT_DIF_Seq(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, unsigned int N_fft, int Inverse)

{
        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        v2s *DataV  = (v2s *) Data;
        v2s *CoeffV = (v2s *) Twiddles;
        int CoreId;

        int i;

        // Layers 0,1,2 ...
        iL = 1; iM = N_fft >> 2;
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
                iL <<= 2; iM >>= 2;
        }
        // Final layer for known twiddles
        iA = 0; iL = (N_fft>>2); iM = 1;
        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
                                         (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
                iA =  iA + 4 * iM;
        }

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
void Radix4FFT_DIF_Par_Fix16(FFT_Arg_T *Arg)

{
        signed short *__restrict__ Data = (signed short *__restrict__) Arg->Data;
        signed short *__restrict__ Twiddles = (signed short *__restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
        v2s *DataV  = (v2s *) Data;
        v2s *CoeffV = (v2s *) Twiddles;
        unsigned int CoreId;
        int First, Last, Chunk;
        int Off[] ={1,-1};

        int i;

        CoreId = gap_coreid();

        // Layers 0,1, ... , (iLog4N-2)
        iM = N_fft >> 2; iL = 1;
        for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
                Chunk = (iM/gap_ncore());
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
        // Layer iLog4N - 2
        iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                v2s W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                Chunk = (iL/gap_ncore()); //  + Off[CoreId&0x1];
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_NoScale((v2s *) (DataV + iA       ), (v2s *) (DataV + iA + iM), 
                                                   (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
                                                   W1, W2, W3, Inverse);
                        iA = iA + 4 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        // Layer iLog4N - 1
        iM = 1; iL = (N_fft>>2);
        Chunk = iL/gap_ncore();
        First =  CoreId*Chunk; Last = First+Chunk;
        iA =  CoreId*Chunk*4*iM;
        for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
                                         (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
}

void Radix4FFT_DIF_Par_Fix32(FFT_Arg_T *Arg)

{
        int *__restrict__ Data = (int *) Arg->Data;
        short int *__restrict__ Twiddles = (short int *) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
        unsigned int CoreId;
        int First, Last, Chunk;
        int Off[] ={1,-1};

        int i;

        CoreId = gap_coreid();

        // Layers 0,1, ... , (iLog4N-2)
        iM = N_fft >> 2; iL = 1;
        for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
                Chunk = (iM/gap_ncore());
                First = CoreId*Chunk; Last = First+Chunk;
                iQ    = First*iL;
                for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
                        short int W1r = Twiddles[2*  iQ], W1i = Twiddles[2*  iQ + 1],
                                  W2r = Twiddles[2*2*iQ], W2i = Twiddles[2*2*iQ + 1],
                                  W3r = Twiddles[2*3*iQ], W3i = Twiddles[2*3*iQ + 1];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_Fix32((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                                         (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                                         W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
                                iA = iA + 4 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 2; iM >>= 2;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }
        // Layer iLog4N - 2
        iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                short int W1r = Twiddles[2*  iQ], W1i = Twiddles[2*  iQ + 1],
                          W2r = Twiddles[2*2*iQ], W2i = Twiddles[2*2*iQ + 1],
                          W3r = Twiddles[2*3*iQ], W3i = Twiddles[2*3*iQ + 1];
                Chunk = (iL/gap_ncore()); //  + Off[CoreId&0x1];
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_Fix32_NoScale((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                                         (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                                         W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
                        iA = iA + 4 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        // Layer iLog4N - 1
        iM = 1; iL = (N_fft>>2);
        Chunk = iL/gap_ncore();
        First =  CoreId*Chunk; Last = First+Chunk;
        iA =  CoreId*Chunk*4*iM;
        for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                Radix4FFTKernel_Twiddle0_Fix32((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
                                               (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
}

#ifdef __gap9__
void Radix4FFT_DIF_Par_f16(FFT_Arg_T *Arg)

{
        float16 *__restrict__ Data = (float16 *) Arg->Data;
        float16 *__restrict__ Twiddles = (float16 *) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
        v2h *DataV  = (v2h *) Data;
        v2h *CoeffV = (v2h *) Twiddles;
        unsigned int CoreId;
        int First, Last, Chunk;
        int Off[] ={1,-1};

        int i;

        CoreId = gap_coreid();

        // Layers 0,1, ... , (iLog4N-2)
        iM = N_fft >> 2; iL = 1;
        for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
                Chunk = (iM/gap_ncore());
                First =  CoreId*Chunk; Last = First+Chunk;
                iQ    = First*iL;
                for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
                        iA = iCnt2;
                        v2h W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_f16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA + iM), 
                                                       (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM),
                                                       W1, W2, W3, Inverse);
                                iA = iA + 4 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 2; iM >>= 2;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }
        // Layer iLog4N - 2
        iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                v2h W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                Chunk = (iL/gap_ncore()); //  + Off[CoreId&0x1];
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_f16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA + iM), 
                                               (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM),
                                               W1, W2, W3, Inverse);
                        iA = iA + 4 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        // Layer iLog4N - 1
        iM = 1; iL = (N_fft>>2);
        Chunk = iL/gap_ncore();
        First =  CoreId*Chunk; Last = First+Chunk;
        iA =  CoreId*Chunk*4*iM;
        for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                Radix4FFTKernel_Twiddle0_f16((v2h *) (DataV + iA       ), (v2h *) (DataV + iA +   iM),
                                             (v2h *) (DataV + iA + 2*iM), (v2h *) (DataV + iA + 3*iM), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
}
#endif

void Radix4FFT_DIF_Par_f32(FFT_Arg_T *Arg)

{
        float *__restrict__ Data = (float *) Arg->Data;
        float *__restrict__ Twiddles = (float *) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        unsigned int iLog4N  = (gap_fl1(N_fft))>>1;
        unsigned int CoreId;
        int First, Last, Chunk;
        int Off[] ={1,-1};

        int i;

        CoreId = gap_coreid();

        // Layers 0,1, ... , (iLog4N-2)
        iM = N_fft >> 2; iL = 1;
        for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
                Chunk = (iM/gap_ncore());
                First = CoreId*Chunk; Last = First+Chunk;
                iQ    = First*iL;
                for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
                        float W1r = Twiddles[2*  iQ], W1i = Twiddles[2*  iQ + 1],
                              W2r = Twiddles[2*2*iQ], W2i = Twiddles[2*2*iQ + 1],
                              W3r = Twiddles[2*3*iQ], W3i = Twiddles[2*3*iQ + 1];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                                       (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                                       W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
                                iA = iA + 4 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 2; iM >>= 2;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }
        // Layer iLog4N - 2
        iM = 4; iL = (N_fft>>(2+2)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                float W1r = Twiddles[2*  iQ], W1i = Twiddles[2*  iQ + 1],
                      W2r = Twiddles[2*2*iQ], W2i = Twiddles[2*2*iQ + 1],
                      W3r = Twiddles[2*3*iQ], W3i = Twiddles[2*3*iQ + 1];
                Chunk = (iL/gap_ncore()); //  + Off[CoreId&0x1];
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                               (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                               W1r, W1i, W2r, W2i, W3r, W3i, Inverse);
                        iA = iA + 4 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        // Layer iLog4N - 1
        iM = 1; iL = (N_fft>>2);
        Chunk = iL/gap_ncore();
        First =  CoreId*Chunk; Last = First+Chunk;
        iA =  CoreId*Chunk*4*iM;
        for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                Radix4FFTKernel_Twiddle0_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
                                             (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
}

void Radix2FFT_DIF_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2)

{
        int iLog2N  = gap_fl1(N_FFT2);
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
void Radix2FFT_DIF_Seq(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2)

{
        int iLog2N  = gap_fl1(N_FFT2);
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
                                DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
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

#if 0
void Radix2FFT_DIF_Par_Fix16(FFT_Arg_T *Arg)

{
        signed short *__restrict__ Data = (signed short *__restrict__) Arg->Data;
        signed short *__restrict__ Twiddles = (signed short *__restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        v2s *CoeffV = (v2s *) Twiddles;
        v2s *DataV  = (v2s *) Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_fft / 2;

        Chunk = ((iLog2N-1)/gap_ncore()); First =  CoreId*Chunk; Last = Min((First+Chunk), (iLog2N-1));

        // for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
        for (iCnt1 = 0; iCnt1 < Min((iLog2N-1), 4); iCnt1++) {
                Chunk = (iM/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
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
                                DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        for (iCnt1 = 4; iCnt1 < (iLog2N-1); iCnt1++) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                        v2s W = CoeffV[iQ];
                        iA = iCnt2;
                        Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;

                        // for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                                v2s Tmp;
                                iB = iA + iM;
                                Tmp       = (DataV[iA] - DataV[iB]);
                                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                                DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        iA = 0;
        /* Last Layer: W = (1, 0) */
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
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
        gap_waitbarrier(0);
}
#endif


void Radix2FFT_DIF_Par_Fix16(FFT_Arg_T *Arg)
{
        unsigned int N_FFT2 = Arg->N_fft;
        unsigned int Inverse = Arg->Inverse;
        unsigned int iLog2N  = gap_fl1(N_FFT2);
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
                        DataV[iA] = (DataV[iA] + DataV[iB]);
                        DataV[iB] = (v2s) gap_cplxmuls(Tmp, W);
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
                        DataV[iA] = (DataV[iA] + DataV[iB]);
                        DataV[iB] = (v2s) gap_cplxmuls(Tmp, W);
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
                v2s A= DataV[iA];
                v2s B= DataV[iA+1];
                DataV[iA] = A + B;
                DataV[iA+1] = A - B;
                iA = iA + 2;
        }
        gap_waitbarrier(0);
}


#ifdef __gap9__
void Radix2FFT_DIF_Par_f16(FFT_Arg_T *Arg)

{
        float16 *__restrict__ Data = (float16 * __restrict__) Arg->Data;
        float16 *__restrict__ Twiddles = (float16 * __restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        v2h *CoeffV = (v2h *) Twiddles;
        v2h *DataV  = (v2h *) Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_fft / 2;
        for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
                Chunk = (iM/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                iQ = First*iL;
                for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                        v2h W = CoeffV[iQ];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                v2h Tmp;
                                iB = iA + iM;
                                Tmp       = DataV[iA] - DataV[iB];
                                DataV[iA] = DataV[iA] + DataV[iB];
                                DataV[iB] = CplxMult_f16(Tmp, W);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        iM = 4;
        iL = N_fft >> 3;
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                v2h W = CoeffV[iQ];
                Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;

                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        v2h Tmp;
                        iB = iA + iM;
                        Tmp       = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]);
                        DataV[iB] = CplxMult_f16(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);

        iM = 2;
        iL = N_fft >> 2;
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                v2h W = CoeffV[iQ];
                Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;

                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        v2h Tmp;
                        iB = iA + iM;
                        Tmp       = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]);
                        DataV[iB] = CplxMult_f16(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);

        /* Last Layer: W = (1, 0) */
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
        iA = 2*Chunk*CoreId;
        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                v2h A = DataV[iA];
                v2h B = DataV[iA+1];
                DataV[iA  ] = A + B;
                DataV[iA+1] = A - B;
                iA = iA + 2;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
}
#endif


void Radix2FFT_DIF_Par_f32(FFT_Arg_T *Arg)

{
        float *__restrict__ Data = (float * __restrict__) Arg->Data;
        float *__restrict__ Twiddles = (float * __restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_fft / 2;
        for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
                Chunk = (iM/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                iQ = First*iL;
                for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                        float Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                float Tmpr, Tmpi;
                                iB = iA + iM;
                                Tmpr = Data[2*iA  ] - Data[2*iB  ];
                                Tmpi = Data[2*iA+1] - Data[2*iB+1];
                                Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                                Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                                Data[2*iB  ] = (Tmpr*Wr - Tmpi*Wi);
                                Data[2*iB+1] = (Tmpr*Wi + Tmpi*Wr);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }

        // Layer iLog2N - 3
        iM = 4;
        iL = N_fft >> 3;
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                float Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                Chunk = (iL/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;
                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        float Tmpr, Tmpi;
                        iB = iA + iM;
                        Tmpr = Data[2*iA  ] - Data[2*iB  ];
                        Tmpi = Data[2*iA+1] - Data[2*iB+1];
                        Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                        Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                        Data[2*iB  ] = (Tmpr*Wr - Tmpi*Wi);
                        Data[2*iB+1] = (Tmpr*Wi + Tmpi*Wr);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);

        // Layer iLog2N - 2
        iM = 2;
        iL = N_fft >> 2;
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                float Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                Chunk = (iL/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;
                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        float Tmpr, Tmpi;
                        iB = iA + iM;
                        Tmpr = Data[2*iA  ] - Data[2*iB  ];
                        Tmpi = Data[2*iA+1] - Data[2*iB+1];
                        Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                        Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                        Data[2*iB  ] = (Tmpr*Wr - Tmpi*Wi);
                        Data[2*iB+1] = (Tmpr*Wi + Tmpi*Wr);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);

        /* Last Layer: W = (1, 0) */
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
        iA = 2*Chunk*CoreId;
        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                float Tmpr, Tmpi;
                iB = iA + 1;
                Tmpr = Data[2*iA  ] - Data[2*iB  ];
                Tmpi = Data[2*iA+1] - Data[2*iB+1];
                Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                Data[2*iB  ] = Tmpr;
                Data[2*iB+1] = Tmpi;
                iA = iA + 2;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
}

void Radix2FFT_DIF_Par_Fix16_Fast(FFT_Arg_T *Arg)

{
        signed short *__restrict__ Data = (signed short *__restrict__) Arg->Data;
        signed short *__restrict__ Twiddles = (signed short *__restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        v2s *CoeffV = (v2s *) Twiddles;
        v2s *DataV  = (v2s *) Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_fft / 2;

        // Layer 0
        Chunk = (iM/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
        iQ = First*iL;
        for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                v2s W = CoeffV[iQ];
                iA = iCnt2;
                for (iCnt3 = 0; iCnt3 < iL; iCnt3++) { // 1
                        v2s Tmp;
                        iB = iA + iM;
                        Tmp       = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                        DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        iL <<= 1; iM >>= 1;
        gap_waitbarrier(0);
        // Layer 1
        Chunk = (iM/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
        iQ = First*iL;
        for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                v2s W = CoeffV[iQ];
                iA = iCnt2;
                for (iCnt3 = 0; iCnt3 < iL; iCnt3++) { // 2
                        iB = iA + iM;
                        v2s Tmp   = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                        DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        iL <<= 1; iM >>= 1;
        gap_waitbarrier(0);
        // Layer 2
        Chunk = (iM/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk; iQ = First*iL;
        for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                v2s W = CoeffV[iQ], A = DataV[iCnt2], B = DataV[iCnt2+iM];
                iA = iCnt2;
                for (iCnt3 = 0; iCnt3 < iL; iCnt3++) { // 4
                        iB = iA + iM;
                        v2s Tmp   = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                        DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        iL <<= 1; iM >>= 1;
        gap_waitbarrier(0);
        // Layer 3
        Chunk = (iM/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk; iQ = First*iL;
        for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                v2s W = CoeffV[iQ], A = DataV[iCnt2], B = DataV[iCnt2+iM];
                iA = iCnt2;
                for (iCnt3 = 0; iCnt3 < iL; iCnt3++) { // 8
                        iB = iA + iM;
                        v2s Tmp   = (DataV[iA] - DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                        DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                        iA = iA + 2 * iM;                }
                iQ += iL;
        }
        iL <<= 1; iM >>= 1;
        gap_waitbarrier(0);

        for (iCnt1 = 4; iCnt1 < (iLog2N-2); iCnt1++) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                        v2s W = CoeffV[iQ];
                        iA = iCnt2;
                        Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
                        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                                iB = iA + iM;
                                v2s Tmp   = (DataV[iA] - DataV[iB]);
                                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                                DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        /* Last Layer: W = (1, 0) */
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
        iA = 2*Chunk*CoreId;
        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                v2s A= DataV[iA];
                v2s B= DataV[iA+1];
                DataV[iA] = A + B;
                DataV[iA+1] = A - B;
                iA = iA + 2;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
}

void SwapSamples_Par(FFT_SwapSamples_T *Arg)

{
        v2s *__restrict__ Data = (v2s *) Arg->Data;
        short *__restrict__ SwapTable = Arg->SwapTable;
        int Ni = Arg->Ni;
        int i;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();

        Chunk = (Ni/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
        // for (i = 0; i < Ni; i++) {
        for (i = First; i < Last; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                        v2s S = Data[i];
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

#ifdef __gap9__
void SwapSamples_Par_f16(FFT_SwapSamples_T *Arg)

{
        v2h *__restrict__ Data = (v2h *) Arg->Data;
        short *__restrict__ SwapTable = Arg->SwapTable;
        int Ni = Arg->Ni;
        int i;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();

        Chunk = (Ni/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
        // for (i = 0; i < Ni; i++) {
        for (i = First; i < Last; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                        v2h S = Data[i];
                        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
                }
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_swapped_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) {
                        printf("%f%+fj, ",  Data[j][0], Data[j][1]);
                }
                printf("])\n");
        }
        #endif
}
#endif

void SwapSamples_Par_f32(FFT_SwapSamples_T *Arg)

{
        float *__restrict__ Data = (float *) Arg->Data;
        short *__restrict__ SwapTable = Arg->SwapTable;
        int Ni = Arg->Ni;
        int i;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();

        Chunk = (Ni/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, Ni);
        // for (i = 0; i < Ni; i++) {
        for (i = First; i < Last; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                        float R = Data[2*i], I = Data[2*i+1];
                        Data[2*i  ] = Data[2*SwapIndex];   Data[2*SwapIndex] = R;
                        Data[2*i+1] = Data[2*SwapIndex+1]; Data[2*SwapIndex+1] = I;
                }
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_swapped_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) {
                    printf("%f%+fj, ",  Data[2*j], Data[2*j+1]);
                }
                printf("])\n");
        }
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
	unsigned int iLog4N	= (gap_fl1(N_fft))>>1;
	v2s *DataV  = (v2s *) Arg->Data;
	v2s *CoeffV = (v2s *) Arg->Twiddles;
	unsigned int CoreId;
	unsigned int First, Last, Chunk;
	signed char *shift_fft = Arg->shift_fft;
	//int Off[] ={1,-1};

	int i;
	CoreId = gap_coreid();


        // reset the shift table
        Chunk = N_fft/gap_ncore();
        First =  CoreId*Chunk; Last = Min( First+Chunk,N_fft);
        for (int i = First; i < Last; i++) shift_fft[i]=0;
        gap_waitbarrier(0);

	// Layers 0,1, ... , (iLog4N-2)
	iM = N_fft >> 2; iL = 1;iQ=0;
	for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
		Chunk = (iM/gap_ncore());
		First =	CoreId*Chunk; Last = First+Chunk;
		iQ		= First*iL;
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

	// last radix4 Layer iLog4N - 2
	iM = 4; iL = (N_fft>>4); iQ = 0;
	for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
		v2s W1 = CoeffV[	iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
		Chunk = (iL/gap_ncore());
		First =	CoreId*Chunk; Last = First+Chunk;
		iA = iCnt2 + Chunk*CoreId*4*iM;
		for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
			Radix4FFTKernelDIF((v2s *) (DataV + iA	     ), (v2s *) (DataV + iA + iM),
					   (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM),
                         W1, W2, W3, Inverse);
			iA = iA + 4 * iM;
		}
		iQ += iL;
		// Synchronize all cores for current layer of the trellis
		gap_waitbarrier(0);
	}

	// last Layer in radix4 (iLog4N - 1)
	iM = 1; iL = (N_fft>>2);
	Chunk = iL/gap_ncore();
	First =	CoreId*Chunk; Last = First+Chunk;
	iA =	CoreId*Chunk*4*iM;
	for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
		Radix4FFTKernel_Twiddle0((v2s *) (DataV + iA       ), (v2s *) (DataV + iA +   iM),
                                         (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
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
        TYPE_GENERIC *CoeffV = (TYPE_GENERIC *) Arg->Twiddles;
        TYPE_GENERIC *DataV  = (TYPE_GENERIC *) Arg->Data;
        unsigned int CoreId;
        unsigned int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_FFT2 / 2;

        // reset the shift table
        Chunk = N_FFT2/gap_ncore();
        First =  CoreId*Chunk; Last = Min( First+Chunk,N_FFT2);
        for (int i = First; i < Last; i++) shift_fft[i]=0;
        gap_waitbarrier(0);

        for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
                Chunk = (iM/gap_ncore());
                First =  CoreId*Chunk; Last = First+Chunk;
                iQ = First*iL;
                for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                        TYPE_GENERIC W = CoeffV[iQ];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                TYPE_GENERIC Tmp;
                                iB = iA + iM;
                                Tmp = DataV[iA] - DataV[iB];
                                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                                DataV[iB] = (TYPE_GENERIC) gap_cplxmulsdiv2(Tmp, W) ;
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


	AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->SwapLUT, (AT_L2_INT_ADDR_TYPE) Arg->L1_SwapLUT,  LUTSize,  0, &DmaR_Evt2);
	AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->Twiddles, (AT_L2_INT_ADDR_TYPE)Arg->L1_Twiddles, TwidSize, 0, &DmaR_Evt1);

	AT_L2_WAIT(0, &DmaR_Evt1);
	AT_L2_WAIT(0, &DmaR_Evt2);
}


static void kernel_fftrad2_scal(cmplx *DataV, v2s W, signed char *shift_fft, int iA, int iB) {
	cmplx Tmp;
	int Dshift;
	int shift=0;

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
	
	// multiply coeff Q15, data Q29	 Q15 * (Q29>>15) => Q29	 
	DataV[iB][0] = (Tmp[0] >> 15) * W[0] - (Tmp[1] >> 15) * W[1];
	DataV[iB][1] = (Tmp[0] >> 15) * W[1] + (Tmp[1] >> 15) * W[0];

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
                        kernel_fftrad2_scal(DataV, W, shift_fft, iA, iB);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }
        gap_waitbarrier(0);
    
        // Layer iLog2N - 2
        iM = 2; iL = (N_FFT2>>(1+1)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                v2s W = CoeffV[  iQ];
                Chunk = (iL/nbcore); 
                First =  CoreId*Chunk; Last = Min(First+Chunk,iL);
                iA = iCnt2 + Chunk*CoreId*2*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        iB = iA + iM;
                        kernel_fftrad2_scal(DataV, W, shift_fft, iA, iB);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        // Synchronize all cores for current layer of the trellis
        }
        gap_waitbarrier(0);
    
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

void Radix2FFT_DIF_Par_Fix32(FFT_Arg_T *Arg)

{
        int *__restrict__ Data = (int * __restrict__) Arg->Data;
        short int *__restrict__ Twiddles = (short int * __restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        iL = 1;
        iM = N_fft / 2;
        for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
                Chunk = (iM/gap_ncore()); 
                First = CoreId*Chunk; Last = Min(First+Chunk, iM);
                iQ = First*iL;
                for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                        short int Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                int Tmpr, Tmpi;
                                iB = iA + iM;
                                Tmpr = Data[2*iA  ] - Data[2*iB  ];
                                Tmpi = Data[2*iA+1] - Data[2*iB+1];
                                Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]) >> 1;
                                Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]) >> 1;
                                Data[2*iB  ] = (((Tmpr*Wr) - (Tmpi*Wi)) >> 15) >> 1;
                                Data[2*iB+1] = (((Tmpr*Wi) + (Tmpi*Wr)) >> 15) >> 1;
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
                // Synchronize all cores for current layer of the trellis
                gap_waitbarrier(0);
        }
        // Layer iLog2N - 3
        iM = 4; iL = (N_fft>>3); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                short int Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                Chunk = (iL/gap_ncore()); 
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;
                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        int Tmpr, Tmpi;
                        iB = iA + iM;
                        Tmpr = Data[2*iA  ] - Data[2*iB  ];
                        Tmpi = Data[2*iA+1] - Data[2*iB+1];
                        Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                        Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                        Data[2*iB  ] = (((Tmpr*Wr) - (Tmpi*Wi)) >> 15);
                        Data[2*iB+1] = (((Tmpr*Wi) + (Tmpi*Wr)) >> 15);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
        }
        gap_waitbarrier(0);

        // Layer iLog2N - 2
        iM = 2; iL = (N_fft>>(2)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                short int Wr = Twiddles[2*iQ], Wi = Twiddles[2*iQ+1];
                Chunk = (iL/gap_ncore()); 
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        int Tmpr, Tmpi;
                        iB = iA + iM;
                        Tmpr = Data[2*iA  ] - Data[2*iB  ];
                        Tmpi = Data[2*iA+1] - Data[2*iB+1];
                        Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                        Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                        Data[2*iB  ] = (((Tmpr*Wr) - (Tmpi*Wi)) >> 15);
                        Data[2*iB+1] = (((Tmpr*Wi) + (Tmpi*Wr)) >> 15);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
        }
        gap_waitbarrier(0);

        /* Last Layer: W = (1, 0) */
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
        iA = 2*Chunk*CoreId;
        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                short int Tmpr, Tmpi;
                iB = iA + 1;
                Tmpr = Data[2*iA  ] - Data[2*iB  ];
                Tmpi = Data[2*iA+1] - Data[2*iB+1];
                Data[2*iA  ] = (Data[2*iA  ] + Data[2*iB  ]);
                Data[2*iA+1] = (Data[2*iA+1] + Data[2*iB+1]);
                Data[2*iB  ] = Tmpr;
                Data[2*iB+1] = Tmpi;
                iA = iA + 2;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
}

extern void Conjugate_Fix16_Par(FFT_SwapSamples_T *Arg){
        v2s *Data = (v2s *) Arg->Data;
        int Ni = Arg->Ni;
        unsigned int CoreId = gap_coreid(), Chunk = Ni/gap_ncore();
        unsigned int First = CoreId*Chunk, Last = Min(First+Chunk,Ni);

        for (int i=First; i<Last; i++) Data[i][1] = -Data[i][1];
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_conj_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) printf("%d%+dj, ", Data[j][0], Data[j][1]);
                printf("])\n");
        }
        #endif
}

extern void Conjugate_Fix32_Par(FFT_SwapSamples_T *Arg){
        int *Data = (int *) Arg->Data;
        int Ni = Arg->Ni;
        unsigned int CoreId = gap_coreid(), Chunk = Ni/gap_ncore();
        unsigned int First = CoreId*Chunk, Last = Min(First+Chunk,Ni);

        for (int i=First; i<Last; i++) Data[2*i+1] = -Data[2*i+1];
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_conj_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) printf("%d%+dj, ",  Data[2*j], Data[2*j+1]);
                printf("])\n");
        }
        #endif
}

#ifdef __gap9__
extern void Conjugate_Float16_Par(FFT_SwapSamples_T *Arg){
        v2h *Data = (v2h *) Arg->Data;
        int Ni = Arg->Ni;
        unsigned int CoreId = gap_coreid(), Chunk = Ni/gap_ncore();
        unsigned int First = CoreId*Chunk, Last = Min(First+Chunk,Ni);

        for (int i=First; i<Last; i++) Data[i][1] = -Data[i][1];
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_conj_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) printf("%d%+dj, ", Data[j][0], Data[j][1]);
                printf("])\n");
        }
        #endif
}
#endif

extern void Conjugate_Float32_Par(FFT_SwapSamples_T *Arg){
        float *Data = (float *) Arg->Data;
        int Ni = Arg->Ni;
        unsigned int CoreId = gap_coreid(), Chunk = Ni/gap_ncore();
        unsigned int First = CoreId*Chunk, Last = Min(First+Chunk,Ni);

        for (int i=First; i<Last; i++) Data[2*i+1] = -Data[2*i+1];
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_conj_fft = np.array([\n\t");
                for (int j=0; j<Ni; j++) printf("%f%+fj, ",  Data[2*j], Data[2*j+1]);
                printf("])\n");
        }
        #endif
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
/*
void RFFT_DIF_Par_f32(RFFT_Arg_T *Arg){
        float *__restrict__ Data = (float * __restrict__) Arg->Data;
        float *__restrict__ Twiddles = (float * __restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        unsigned int CoreId;
        int First, Last, Chunk;

}*/
