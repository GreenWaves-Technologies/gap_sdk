#include "Gap.h"
#include "DSP_Lib.h"

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

/* Util function to Copy Twiddles and SwapLUT table from L2 to shared L1 */
void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format)
{
    AT_L2_EVENT DmaR_Evt1, DmaR_Evt2;
    int TwidSize, LUTSize;

    if (Arg->Radix == 2) TwidSize = Arg->Nfft * sizeof(short);
    else TwidSize = 3 * Arg->Nfft * (sizeof(short)/2);

    // when floating 32, size is double
    if (format==1) TwidSize *=2;
    LUTSize = Arg->Nfft*sizeof(short);


    AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->SwapLUT,  (AT_L2_INT_ADDR_TYPE) Arg->L1_SwapLUT,  LUTSize,  0, &DmaR_Evt1);
    AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->Twiddles, (AT_L2_INT_ADDR_TYPE) Arg->L1_Twiddles, TwidSize, 0, &DmaR_Evt2);

    AT_L2_WAIT(0, &DmaR_Evt1);
    AT_L2_WAIT(0, &DmaR_Evt2);
}

/* Util function to Copy Twiddles and SwapLUT table from L2 to shared L1 */
void RFFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format)
{
    AT_L2_EVENT DmaR_Evt1, DmaR_Evt2, DmaR_Evt3;
    int TwidSize, RTwidSize, LUTSize;

    if (Arg->Radix == 2) TwidSize = (Arg->Nfft>>1) * sizeof(short);
    else TwidSize = 3 * (Arg->Nfft>>1) * (sizeof(short)/2);

    // when floating 32, size is double
    if (format==1) TwidSize *=2;
    LUTSize = (Arg->Nfft>>1)*sizeof(short);

    if (format==1) RTwidSize = Arg->Nfft * sizeof(float);
    else           RTwidSize = Arg->Nfft * sizeof(short);

    AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->SwapLUT,   (AT_L2_INT_ADDR_TYPE) Arg->L1_SwapLUT,   LUTSize,   0, &DmaR_Evt1);
    AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->Twiddles,  (AT_L2_INT_ADDR_TYPE) Arg->L1_Twiddles,  TwidSize,  0, &DmaR_Evt2);
    AT_L2_COPY(0, (AT_L2_EXT_ADDR_TYPE) Arg->RTwiddles, (AT_L2_INT_ADDR_TYPE) Arg->L1_RTwiddles, RTwidSize, 0, &DmaR_Evt3);

    AT_L2_WAIT(0, &DmaR_Evt1);
    AT_L2_WAIT(0, &DmaR_Evt2);
    AT_L2_WAIT(0, &DmaR_Evt3);
}

/* ------------------------------------------- FFT Butterfly Kernels for Radix4 ------------------------------------------- */

static inline void Radix4FFTKernel_Twiddle0(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD, int Inverse)

{
        v2s A = *InOutA;
        v2s B = *InOutB;
        v2s C = *InOutC;
        v2s D = *InOutD;

        *InOutA = ((A + C) + (B  + D ));
        *InOutB = ((A - C) + gap_sub2rotmj(B, D));
        *InOutC = ((A + C) - (B  + D ));
        *InOutD = ((A - C) - gap_sub2rotmj(B, D));

        A = *InOutA;
        B = *InOutB;
        C = *InOutC;
        D = *InOutD;
}

static inline void Radix4FFTKernel_Twiddle0_f32(float *InOutA, float *InOutB, float *InOutC, float *InOutD, unsigned int Inverse)

{
        float B1r, D1r;
        float B1i, D1i;

        float Ar = InOutA[0];
        float Ai = InOutA[1];
        float Br = InOutB[0];
        float Bi = InOutB[1];
        float Cr = InOutC[0];
        float Ci = InOutC[1];
        float Dr = InOutD[0];
        float Di = InOutD[1];

        B1r = Bi; B1i = -Br;
        D1r = Di; D1i = -Dr;

        InOutA[0] = ((Ar + Cr) + (Br  + Dr ));
        InOutA[1] = ((Ai + Ci) + (Bi  + Di ));
        InOutB[0] = ((Ar - Cr) + (B1r - D1r));
        InOutB[1] = ((Ai - Ci) + (B1i - D1i));
        InOutC[0] = ((Ar + Cr) - (Br  + Dr ));
        InOutC[1] = ((Ai + Ci) - (Bi  + Di ));
        InOutD[0] = ((Ar - Cr) - (B1r - D1r));
        InOutD[1] = ((Ai - Ci) - (B1i - D1i));
}

static inline void Radix4FFTKernel_Twiddle0_f16(F16V_DSP *InOutA, F16V_DSP *InOutB, F16V_DSP *InOutC, F16V_DSP *InOutD, int Inverse)

{
        F16V_DSP A = *InOutA;
        F16V_DSP B = *InOutB;
        F16V_DSP C = *InOutC;
        F16V_DSP D = *InOutD;

        F16V_DSP rotmjB = B-D;
        rotmjB = __builtin_shuffle(rotmjB, (V2S){1, 0});
        rotmjB = rotmjB * (F16V_DSP){1.0, -1.0}; // --> rotmjB = {Bi - Di, -(Br - Dr)}

        *InOutA = ((A + C) + (B  + D ));
        *InOutB = ((A - C) + rotmjB);
        *InOutC = ((A + C) - (B  + D ));
        *InOutD = ((A - C) - rotmjB);

        A = *InOutA;
        B = *InOutB;
        C = *InOutC;
        D = *InOutD;
}

static inline F16V_DSP CplxMult_f16(F16V_DSP A, F16V_DSP B)

{
        F16V_DSP P0, P1, P2, P3;

        P0 = A*B;
        B = __builtin_shuffle(B, (V2S){1,0});
        P1 = A*B;
        P2 = __builtin_shuffle(P0, P1, (V2S){0,2});
        P3 = __builtin_shuffle(P0, P1, (V2S){1,3});
        P3 = P3 * (F16V_DSP) {-1.0, 1.0};
        return P3+P2;
}

static inline F16V_DSP CplxMult_f16_Bis(F16V_DSP A, F16V_DSP B)

{
        F16_DSP Ar=A[0];
        F16_DSP Ai=A[1];
        F16_DSP Br=A[0];
        F16_DSP Bi=B[1];

        return (F16V_DSP){(Ar*Br-Ai*Bi), (Ar*Bi+Ai*Br)};
}

static inline void Radix4FFTKernelDIF_f16(F16V_DSP *InOutA, F16V_DSP *InOutB, F16V_DSP *InOutC, F16V_DSP *InOutD,
                                   F16V_DSP W1, F16V_DSP W2, F16V_DSP W3, int Inverse)

{
        F16V_DSP A = *InOutA;
        F16V_DSP B = *InOutB;
        F16V_DSP C = *InOutC;
        F16V_DSP D = *InOutD;

        F16V_DSP A1, B1, C1, D1, B2, D2;
        F16V_DSP rotmjB = B-D;

        rotmjB = __builtin_shuffle(rotmjB, (V2S){1, 0});
        rotmjB = rotmjB * (F16V_DSP){1.0, -1.0}; // --> rotmjB = {Bi - Di, -(Br - Dr)}

        A1 = ((A + C) + (B + D)); // A1 = {(Ar + Cr) + (Br + Dr), (Ai + Ci) + (Bi + Di)}
        B1 = ((A - C) + rotmjB ); // B1 = {(Ar - Cr) + (Bi - Di), (Ai - Ci) - (Br - Dr)}
        C1 = ((A + C) - (B + D)); // C1 = {(Ar + Cr) - (Br + Dr), (Ai + Ci) - (Bi + Di)}
        D1 = ((A - C) - rotmjB ); // D1 = {(Ar - Cr) - (Bi - Di), (Ai - Ci) + (Br - Dr)}

        B1 = CplxMult_f16(B1, W1); C1 = CplxMult_f16(C1, W2); D1 = CplxMult_f16(D1, W3);

        *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}

static inline void Radix4FFTKernelDIF_f32(float *InOutA, float *InOutB, float *InOutC, float *InOutD,
                                   float W1r, float W1i, float W2r, float W2i, float W3r, float W3i, unsigned int Inverse)

{
        float A1r, A1i, B1r, B1i, C1r, C1i, D1r, D1i;
        float Tmp; //B2r, B2i, D2r, D2i,

        float Ar = InOutA[0];
        float Ai = InOutA[1];
        float Br = InOutB[0];
        float Bi = InOutB[1];
        float Cr = InOutC[0];
        float Ci = InOutC[1];
        float Dr = InOutD[0];
        float Di = InOutD[1];

        A1r = ((Ar + Cr) + (Br  +  Dr));
        A1i = ((Ai + Ci) + (Bi  +  Di));
        B1r = ((Ar - Cr) + (Bi  -  Di));
        B1i = ((Ai - Ci) - (Br  -  Dr));
        C1r = ((Ar + Cr) - (Br  +  Dr));
        C1i = ((Ai + Ci) - (Bi  +  Di));
        D1r = ((Ar - Cr) - (Bi  -  Di));
        D1i = ((Ai - Ci) + (Br  -  Dr));

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

static inline void Radix4FFTKernelDIF(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
                               v2s W1, v2s W2, v2s W3, int Inverse)

{
        v2s A = *InOutA;
        v2s B = *InOutB;
        v2s C = *InOutC;
        v2s D = *InOutD;

        int B1r, B1i, C1r, C1i, D1r, D1i;
        v2s A1, B1, C1, D1, B2, D2;

        A1 = ((A + C) + (B + D)) >> (v2s) {FFT4_SCALEDOWN, FFT4_SCALEDOWN};
        B1 = ((A - C) + gap_sub2rotmj(B,  D));
        C1 = ((A + C) - (B + D));
        D1 = ((A - C) - gap_sub2rotmj(B,  D));

        B1 = gap_cplxmulsdiv4(B1, W1); C1 = gap_cplxmulsdiv4(C1, W2); D1 = gap_cplxmulsdiv4(D1, W3);

        *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}

static inline void Radix4FFTKernelDIF_NoScale(v2s *InOutA, v2s *InOutB, v2s *InOutC, v2s *InOutD,
                                       v2s W1, v2s W2, v2s W3, unsigned int Inverse)

{
        v2s A1, B1, C1, D1;
        v2s A = *InOutA, B = *InOutB, C = *InOutC, D = *InOutD;

        A1 = ((A + C) +               (B + D));
        B1 = ((A - C) + gap_sub2rotmj(B,  D));
        C1 = ((A + C) -               (B + D));
        D1 = ((A - C) - gap_sub2rotmj(B,  D));

        B1 = gap_cplxmuls(B1, W1); C1 = gap_cplxmuls(C1, W2); D1 = gap_cplxmuls(D1, W3);
        *InOutA = A1; *InOutB = B1; *InOutC = C1; *InOutD = D1;
}

static inline void Radix4FFTKernelDIF_Fix32(int *InOutA, int *InOutB, int *InOutC, int *InOutD,
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


        A1r = ((Ar + Cr) + (Br  +  Dr));
        A1i = ((Ai + Ci) + (Bi  +  Di));
        B1r = ((Ar - Cr) + (Bi  -  Di));
        B1i = ((Ai - Ci) - (Br  -  Dr));
        C1r = ((Ar + Cr) - (Br  +  Dr));
        C1i = ((Ai + Ci) - (Bi  +  Di));
        D1r = ((Ar - Cr) - (Bi  -  Di));
        D1i = ((Ai - Ci) + (Br  -  Dr));

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
static inline void Radix4FFTKernelDIF_Fix32_NoScale(int *InOutA, int *InOutB, int *InOutC, int *InOutD,
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

        A1r = ((Ar + Cr) + (Br  +  Dr));
        A1i = ((Ai + Ci) + (Bi  +  Di));
        B1r = ((Ar - Cr) + (Bi  -  Di));
        B1i = ((Ai - Ci) - (Br  -  Dr));
        C1r = ((Ar + Cr) - (Br  +  Dr));
        C1i = ((Ai + Ci) - (Bi  +  Di));
        D1r = ((Ar - Cr) - (Bi  -  Di));
        D1i = ((Ai - Ci) + (Br  -  Dr));

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
static inline void Radix4FFTKernel_Twiddle0_Fix32(int *InOutA, int *InOutB, int *InOutC, int *InOutD, unsigned int Inverse)

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

        InOutA[0] = ((Ar + Cr) + (Br  + Dr ));
        InOutA[1] = ((Ai + Ci) + (Bi  + Di ));
        InOutB[0] = ((Ar - Cr) + (B1r - D1r));
        InOutB[1] = ((Ai - Ci) + (B1i - D1i));
        InOutC[0] = ((Ar + Cr) - (Br  + Dr ));
        InOutC[1] = ((Ai + Ci) - (Bi  + Di ));
        InOutD[0] = ((Ar - Cr) - (B1r - D1r));
        InOutD[1] = ((Ai - Ci) - (B1i - D1i));
}


static inline void kernel_fftrad2_scal(cmplx *DataV, v2s W, signed char *shift_fft, int iA, int iB) {
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
        int norm = (Abs( Tmp[0]) + Abs(Tmp[1])); 
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
        
        // multiply coeff Q15, data Q29  Q15 * (Q29>>15) => Q29  
        DataV[iB][0] = (Tmp[0] >> 15) * W[0] - (Tmp[1] >> 15) * W[1];
        DataV[iB][1] = (Tmp[0] >> 15) * W[1] + (Tmp[1] >> 15) * W[0];

        // normalize iA mantissa to Q29 
        norm = (Abs(DataV[iA][0]) + Abs(DataV[iA][1]));
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


/* ---------------------------------------- FFT Kernels -------------------------------------------- */

/*
  Radix 4, Decimated in Frequency, fft. Sequential implementation.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/

void Radix4FFT_DIF_Seq_Fix16(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        v2s *DataV  = (v2s *) Data;
        v2s *CoeffV = (v2s *) Twiddles;
        int CoreId;

        int i;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }
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
                                                   W1, W2, W3, 0);
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
                                         (v2s *) (DataV + iA + 2*iM), (v2s *) (DataV + iA + 3*iM), 0);
                iA =  iA + 4 * iM;
        }
        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }

}


void Radix4FFT_DIF_Seq_f16(F16_DSP *__restrict__ Data, F16_DSP *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        F16V_DSP *DataV  = (F16V_DSP *) Data;
        F16V_DSP *CoeffV = (F16V_DSP *) Twiddles;
        int CoreId;

        int i;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }
        // Layers 0,1,2 ...
        iL = 1; iM = N_fft >> 2;
        for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                        iA = iCnt2;
                        F16V_DSP W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_f16((F16V_DSP *) (DataV + iA       ), (F16V_DSP *) (DataV + iA + iM), 
                                                       (F16V_DSP *) (DataV + iA + 2*iM), (F16V_DSP *) (DataV + iA + 3*iM),
                                                       W1, W2, W3, 0);
                                iA = iA + 4 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 2; iM >>= 2;
        }
        // Final layer for known twiddles
        iA = 0; iL = (N_fft>>2); iM = 1;
        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                Radix4FFTKernel_Twiddle0_f16((F16V_DSP *) (DataV + iA       ), (F16V_DSP *) (DataV + iA +   iM),
                                             (F16V_DSP *) (DataV + iA + 2*iM), (F16V_DSP *) (DataV + iA + 3*iM), 0);
                iA =  iA + 4 * iM;
        }
        if (Inverse) {
                float invN = 1.0 / (float) N_fft;
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][0] =  DataV[iCnt1][0] * invN;
                        DataV[iCnt1][1] = -DataV[iCnt1][1] * invN;
                }
        }
}


void Radix4FFT_DIF_Seq_f32(float *__restrict__ Data, float *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        int CoreId;

        int i;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1];
                }
        }
        // Layers 0,1,2 ...
        iL = 1; iM = N_fft >> 2;
        for (iCnt1 = 0; iCnt1 < (iLog4N-1); ++iCnt1) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                        float W1r = Twiddles[2*  iQ], W1i = Twiddles[2*  iQ + 1],
                              W2r = Twiddles[2*2*iQ], W2i = Twiddles[2*2*iQ + 1],
                              W3r = Twiddles[2*3*iQ], W3i = Twiddles[2*3*iQ + 1];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                                       (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                                       W1r, W1i, W2r, W2i, W3r, W3i, 0);
                                iA = iA + 4 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 2; iM >>= 2;
        }
        // Final layer for known twiddles
        iA = 0; iL = (N_fft>>2); iM = 1;
        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                Radix4FFTKernel_Twiddle0_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)),
                                             (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), 0);
                iA =  iA + 4 * iM;
        }
        if (Inverse) {
                float invN = 1.0 / (float) N_fft;
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        Data[2*iCnt1  ] =  Data[2*iCnt1  ] * invN;
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1] * invN;
                }
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
void Radix4FFT_DIF_Par_Fix16(FFT_Arg_T *Arg)

{
        signed short *__restrict__ Data = (signed short *__restrict__) Arg->Data;
        signed short *__restrict__ Twiddles = (signed short *__restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        v2s *DataV  = (v2s *) Data;
        v2s *CoeffV = (v2s *) Twiddles;
        unsigned int CoreId;
        int First, Last, Chunk;

        int i;

        CoreId = gap_coreid();
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }

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
                Chunk = (iL/gap_ncore());
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
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }
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
        int iLog4N  = (gap_fl1(N_fft))>>1;
        unsigned int CoreId;
        int First, Last, Chunk;

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
                Chunk = (iL/gap_ncore());
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

void Radix4FFT_DIF_Par_f16(FFT_Arg_T *Arg)

{
        F16_DSP *__restrict__ Data = (F16_DSP *) Arg->Data;
        F16_DSP *__restrict__ Twiddles = (F16_DSP *) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        F16V_DSP *DataV  = (F16V_DSP *) Data;
        F16V_DSP *CoeffV = (F16V_DSP *) Twiddles;
        unsigned int CoreId;
        int First, Last, Chunk;

        int i;

        CoreId = gap_coreid();
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }

        // Layers 0,1, ... , (iLog4N-2)
        iM = N_fft >> 2; iL = 1;
        for (iCnt1 = 0; iCnt1 < (iLog4N-2); ++iCnt1) {
                Chunk = (iM/gap_ncore());
                First =  CoreId*Chunk; Last = First+Chunk;
                iQ    = First*iL;
                for (iCnt2 = First; iCnt2 < Last; ++iCnt2) {
                        iA = iCnt2;
                        F16V_DSP W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                        for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                                Radix4FFTKernelDIF_f16((F16V_DSP *) (DataV + iA       ), (F16V_DSP *) (DataV + iA + iM), 
                                                       (F16V_DSP *) (DataV + iA + 2*iM), (F16V_DSP *) (DataV + iA + 3*iM),
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
                F16V_DSP W1 = CoeffV[  iQ], W2 = CoeffV[2*iQ], W3 = CoeffV[3*iQ];
                Chunk = (iL/gap_ncore());
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_f16((F16V_DSP *) (DataV + iA       ), (F16V_DSP *) (DataV + iA + iM), 
                                               (F16V_DSP *) (DataV + iA + 2*iM), (F16V_DSP *) (DataV + iA + 3*iM),
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
                Radix4FFTKernel_Twiddle0_f16((F16V_DSP *) (DataV + iA       ), (F16V_DSP *) (DataV + iA +   iM),
                                             (F16V_DSP *) (DataV + iA + 2*iM), (F16V_DSP *) (DataV + iA + 3*iM), Inverse);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);
        if (Arg->Inverse) {
                F16_DSP invN = (F16_DSP) 1.0 / N_fft;
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][0] =  DataV[iCnt1][0]*invN;
                        DataV[iCnt1][1] = -DataV[iCnt1][1]*invN;
                }
                gap_waitbarrier(0);
        }
}

void Radix4FFT_DIF_Par_f32(FFT_Arg_T *Arg)

{
        float *__restrict__ Data = (float *) Arg->Data;
        float *__restrict__ Twiddles = (float *) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;
        int Inverse = Arg->Inverse;

        int iCnt1, iCnt2, iCnt3,
            iL,    iM,    iQ,
            iA,    iB,    iC,     iD;
        int iLog4N  = (gap_fl1(N_fft))>>1;
        unsigned int CoreId;
        int First, Last, Chunk;

        int i;

        CoreId = gap_coreid();
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1];
                }
                gap_waitbarrier(0);
        }

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
                                                       W1r, W1i, W2r, W2i, W3r, W3i, 0);
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
                Chunk = (iL/gap_ncore());
                First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*4*iM;
                for (iCnt3 = First; iCnt3 < Last; ++iCnt3) {
                        Radix4FFTKernelDIF_f32((Data + 2*(iA       )), (Data + 2*(iA +   iM)), 
                                               (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)),
                                               W1r, W1i, W2r, W2i, W3r, W3i, 0);
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
                                             (Data + 2*(iA + 2*iM)), (Data + 2*(iA + 3*iM)), 0);
                iA =  iA + 4 * iM;
        }
        // Synchronize all cores for last layer of the trellis
        gap_waitbarrier(0);

        if (Arg->Inverse) {
                float invN = 1.0 / (float) N_fft;
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        Data[2*iCnt1]   =  Data[2*iCnt1]   * invN;
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1] * invN;
                }
                gap_waitbarrier(0);
        }
}

/* ------------------------------------------------------------ Radix 2 FFT Implementations -------------------------------------------------------- */

void Radix2FFT_DIF_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft)

{
        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;

        iL = 1;
        iM = N_fft / 2;

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
        for (iCnt3 = 0; iCnt3 < (N_fft>>1); iCnt3++) {
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
void Radix2FFT_DIF_Seq_Fix16(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        v2s *CoeffV = (v2s *) Twiddles;
        v2s *DataV  = (v2s *) Data;

        iL = 1;
        iM = N_fft / 2;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }
        for (iCnt1 = 0; iCnt1 < (iLog2N-2); iCnt1++) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                        v2s W = CoeffV[iQ];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                v2s Tmp;
                                iB = iA + iM;
                                Tmp = DataV[iA] - DataV[iB];
                                DataV[iA] = gap_add2div2(DataV[iA], DataV[iB]);
                                DataV[iB] = gap_cplxmulsdiv2(Tmp, W);
                                iA = iA + 2 * iM;
                        }
                        iQ += iL;
                }
                iL <<= 1;
                iM >>= 1;
        }
        // Layer iLog2N - 2
        iM = 2; iL = (N_fft>>(1+1)); iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; ++iCnt2) {
                v2s W = CoeffV[  iQ];
                iA = iCnt2;
                for (iCnt3 = 0; iCnt3 < iL; ++iCnt3) {
                        v2s Tmp;
                        iB = iA + iM;
                        //printf("core %d stage %d iL %d idx %d %d\n",CoreId,iCnt1+1,iL,iA,iB);
                        Tmp = (DataV[iA]) - (DataV[iB]);
                        DataV[iA] = (DataV[iA] + DataV[iB]);
                        DataV[iB] = (v2s) gap_cplxmuls(Tmp, W);
                        iA = iA + 2 * iM;
                }
                iQ += iL;
        }

        iA = 0;
        /* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_fft>>1); iCnt3++) {
                v2s Tmp;
                iB = iA + 1;
                Tmp = (DataV[iA] - DataV[iB]);
                DataV[iA] = (DataV[iA] + DataV[iB]);
                DataV[iB] = Tmp;
                iA = iA + 2;
        }
        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }
}


void Radix2FFT_DIF_Seq_f16(F16_DSP *__restrict__ Data, F16_DSP *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        F16V_DSP *CoeffV = (F16V_DSP *) Twiddles;
        F16V_DSP *DataV  = (F16V_DSP *) Data;

        iL = 1;
        iM = N_fft / 2;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
        }
        for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
                        F16V_DSP W = CoeffV[iQ];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                F16V_DSP Tmp;
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
        }
        iA = 0;
        /* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_fft>>1); iCnt3++) {
                F16V_DSP Tmp;
                iB = iA + 1;
                Tmp = (DataV[iA] - DataV[iB]);
                DataV[iA] = (DataV[iA] + DataV[iB]);
                DataV[iB] = Tmp;
                iA = iA + 2;
        }
        if (Inverse) {
                F16_DSP invN = 1.0 / (F16_DSP) N_fft;
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        DataV[iCnt1][0] =  DataV[iCnt1][0] * invN;
                        DataV[iCnt1][1] = -DataV[iCnt1][1] * invN;
                }
        }
}


void Radix2FFT_DIF_Seq_f32(float *__restrict__ Data, float *__restrict__ Twiddles, int N_fft, int Inverse)

{
        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;

        iL = 1;
        iM = N_fft / 2;

        if (Inverse) {
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1];
                }
        }
        for (iCnt1 = 0; iCnt1 < (iLog2N-1); iCnt1++) {
                iQ = 0;
                for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
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
        }
        iA = 0;
        /* Last Layer: W = (1, 0) */
        for (iCnt3 = 0; iCnt3 < (N_fft>>1); iCnt3++) {
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
        if (Inverse) {
                float invN = 1.0 / (float) N_fft;
                for (iCnt1=0; iCnt1<N_fft; iCnt1++){
                        Data[2*iCnt1  ] =  Data[2*iCnt1  ] * invN;
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1] * invN;
                }
        }
}

void Radix2FFT_DIF_Par_Fix16(FFT_Arg_T *Arg)
{
        unsigned int N_fft = Arg->N_fft;
        unsigned int Inverse = Arg->Inverse;
        unsigned int iLog2N  = gap_fl1(N_fft);
        unsigned int iCnt1, iCnt2, iCnt3,
        iQ,    iL,    iM,
        iA,    iB;
        v2s *CoeffV = (v2s *) Arg->Twiddles;
        v2s *DataV  = (v2s *) Arg->Data;
        unsigned int CoreId;
        unsigned int First, Last, Chunk;

        CoreId = gap_coreid();
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }
        iL = 1;
        iM = N_fft / 2;
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
                                DataV[iA] = (v2s) gap_add2div2(DataV[iA], DataV[iB]); // >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
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
        iM = 4; iL = (N_fft>>(1+1+1)); iQ = 0;
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
                        DataV[iA] = (v2s) gap_add2div2(DataV[iA], DataV[iB]); // >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                        DataV[iB] = (v2s) gap_cplxmulsdiv2(Tmp, W) ;
                        iA = iA + 2 * iM;
                }
                iQ += iL;
                // Synchronize all cores for current layer of the trellis
        }
        gap_waitbarrier(0);

        // Layer iLog2N - 2
        iM = 2; iL = (N_fft>>(1+1)); iQ = 0;
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
        Chunk = ((N_fft>>1)/gap_ncore()); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
        iA = 2*Chunk*CoreId;
        for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                v2s A= DataV[iA];
                v2s B= DataV[iA+1];
                DataV[iA] = A + B;
                DataV[iA+1] = A - B;
                iA = iA + 2;
        }
        gap_waitbarrier(0);
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }
}


void Radix2FFT_DIF_Par_f16(FFT_Arg_T *Arg)

{
        F16_DSP *__restrict__ Data = (F16_DSP * __restrict__) Arg->Data;
        F16_DSP *__restrict__ Twiddles = (F16_DSP * __restrict__) Arg->Twiddles;
        unsigned int N_fft = Arg->N_fft;

        int iLog2N  = gap_fl1(N_fft);
        int iCnt1, iCnt2, iCnt3,
            iQ,    iL,    iM,
            iA,    iB;
        F16V_DSP *CoeffV = (F16V_DSP *) Twiddles;
        F16V_DSP *DataV  = (F16V_DSP *) Data;
        unsigned int CoreId;
        int First, Last, Chunk;

        CoreId = gap_coreid();
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][1] = -DataV[iCnt1][1];
                }
                gap_waitbarrier(0);
        }

        iL = 1;
        iM = N_fft / 2;
        for (iCnt1 = 0; iCnt1 < (iLog2N-3); iCnt1++) {
                Chunk = (iM/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                iQ = First*iL;
                for (iCnt2 = First; iCnt2 < Last; iCnt2++) {
                        F16V_DSP W = CoeffV[iQ];
                        iA = iCnt2;
                        for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                                F16V_DSP Tmp;
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
                F16V_DSP W = CoeffV[iQ];
                Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;

                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        F16V_DSP Tmp;
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
                F16V_DSP W = CoeffV[iQ];
                Chunk = (iL/gap_ncore()); First =  CoreId*Chunk; Last = First+Chunk;
                iA = iCnt2 + Chunk*CoreId*2*iM;

                for (iCnt3 = First; iCnt3 < Last; iCnt3++) {
                        F16V_DSP Tmp;
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
                F16V_DSP A = DataV[iA];
                F16V_DSP B = DataV[iA+1];
                DataV[iA  ] = A + B;
                DataV[iA+1] = A - B;
                iA = iA + 2;
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        if (Arg->Inverse) {
                F16_DSP invN = (F16_DSP) 1.0 / N_fft;
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        DataV[iCnt1][0] =  DataV[iCnt1][0]*invN;
                        DataV[iCnt1][1] = -DataV[iCnt1][1]*invN;
                }
                gap_waitbarrier(0);
        }
}


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
        if (Arg->Inverse) {
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1];
                }
                gap_waitbarrier(0);
        }

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

        if (Arg->Inverse) {
                float invN = 1.0 / (float) N_fft;
                Chunk = (N_fft/gap_ncore()); First = CoreId*Chunk; Last = First+Chunk;
                for (iCnt1=First; iCnt1<Last; iCnt1++){
                        Data[2*iCnt1]   =  Data[2*iCnt1]   * invN;
                        Data[2*iCnt1+1] = -Data[2*iCnt1+1] * invN;
                }
                gap_waitbarrier(0);
        }
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


/*
  32 bits version of the fft, with "pseudo floating point": an exponent is attached to each complex data
     Twiddles are short int Q15
     Data are int Q29 (mantissa) + exponent
*/
void Radix2FFT_DIF_Par_Fix32_Scal(FFT_scal_Arg_T *Arg)
{
 
        unsigned int N_fft = Arg->N_fft;
        unsigned int Inverse = Arg->Inverse;
        unsigned int iLog2N  = gap_fl1(N_fft);
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
        Chunk = N_fft/nbcore;
        First =  CoreId*Chunk; Last = Min( First+Chunk,N_fft);
        for (unsigned int i = First; i < Last; i++) shift_fft[i]=0; 
        gap_waitbarrier(0);

        // compute fft 
        iL = 1;
        iM = N_fft / 2;

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
        iM = 4; iL = (N_fft>>(1+1+1)); iQ = 0;
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
        iM = 2; iL = (N_fft>>(1+1)); iQ = 0;
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
        Chunk = ((N_fft>>1)/nbcore); First =  CoreId*Chunk; Last = Min(First+Chunk, (N_fft>>1));
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

/* Real Sample FFT functions: Sequential */

void RFFT_DIF_Seq_Fix16(short int *Data, short int *RFFT_Out, short int *Twiddles, short int *RTwiddles, short int *SwapTable, int N_fft)
{
        v2s *__restrict__ RFFT_OutV = (v2s * __restrict__) RFFT_Out;
        v2s *__restrict__ RTwiddlesV = (v2s * __restrict__) RTwiddles;

        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_Fix16(Data, Twiddles, (N_fft>>1), 0);
        } else {
                Radix2FFT_DIF_Seq_Fix16(Data, Twiddles, (N_fft>>1), 0);
        }
        SwapSamples_Seq_Fix16(Data, SwapTable, (N_fft>>1));

        int  k;                          /* Loop Counter */
        signed short int twR, twI;       /* RFFT Twiddle coefficients */
        v2s *pA = (v2s *) Data;          /* increasing pointer */
        v2s *pB = (v2s *) Data;          /* decreasing pointer */
        v2s xA, xB;                      /* temporary variables */
        v2s t1, t2, tw;                  /* temporary variables */
        signed short int p0, p1, p2, p3; /* temporary variables */
        int xBR, xBI, xAR, xAI;

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        xBR = pB[0][0];
        xBI = pB[0][1];
 
        // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
        // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
        // XA(1) = 1/2*( U1 - imag(U2) +  i*( U1 +imag(U2) ));
        RFFT_OutV[0][0] = ( xBR + xBI ) >> 1;
        RFFT_OutV[0][1] = 0;

        // Gr(N) = Gr(0) - Gi(0)
        // Gi(N) = 0
        RFFT_OutV[k+1][0] = ( xBR - xBI );
        RFFT_OutV[k+1][1] = 0;

        pB  = pA + k;
        pA += 1;
        RTwiddlesV += 1;
        RFFT_OutV += 1;
        for (int i=0; i<k; i++) {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                    XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */

                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddlesV[i];
                /* t1 = {xBR - xAR, xBI + xAI} */
                t1 = gap_add2div4(-xA, gap_cplxconj(xB));
                t2 = gap_add2div4( xA, gap_cplxconj(xB));
                RFFT_OutV[i] = gap_cplxmuls(tw, t1) + t2;
        }
        #ifdef PRINTDEB
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%d%+dj, ",  RFFT_OutV[j-1][0], RFFT_OutV[j-1][1]); printf("])\n");
        #endif
}

void RFFT_DIF_Seq_f16(F16_DSP *Data, F16_DSP *RFFT_Out, F16_DSP *Twiddles, F16_DSP *RTwiddles, short int *SwapTable, int N_fft)
{
        F16V_DSP *__restrict__ RFFT_OutV = (F16V_DSP * __restrict__) RFFT_Out;
        F16V_DSP *__restrict__ RTwiddlesV = (F16V_DSP * __restrict__) RTwiddles;

        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_f16(Data, Twiddles, (N_fft>>1), 0);
        } else {
                Radix2FFT_DIF_Seq_f16(Data, Twiddles, (N_fft>>1), 0);
        }
        SwapSamples_Seq_f16(Data, SwapTable, (N_fft>>1));

        int  k;                                 /* Loop Counter */
        F16_DSP twR, twI;                         /* RFFT Twiddle coefficients */
        F16V_DSP *pA = (F16V_DSP *) Data;               /* increasing pointer */
        F16V_DSP *pB = (F16V_DSP *) Data;               /* decreasing pointer */
        F16_DSP xAR, xAI, xBR, xBI;               /* temporary variables */
        F16V_DSP xA, xB, t1, t2, tw;
        F16_DSP t1a, t1b;                         /* temporary variables */
        F16_DSP p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        xBR = pB[0][0];
        xBI = pB[0][1];
 
        // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
        // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
        RFFT_OutV[0][0] = xBR + xBI;
        RFFT_OutV[0][1] = 0.0f;
        // XA(1) = 1/2*( U1 - imag(U2) +  i*( U1 +imag(U2) ));
        RFFT_OutV[k+1][0] = xBR - xBI;
        RFFT_OutV[k+1][1] = 0.0f;

        pB  = pA + k;
        pA += 1;
        RTwiddlesV += 1;
        RFFT_OutV += 1;
        for (int i=0; i<k; i++) {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                    XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */
                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddlesV[i];
                /* t1 = {xBR - xAR, xBI + xAI} */
                /* t1 = {xBR + xAR,-xBI + xAI} */
                t2 = xB * (F16V_DSP) {1.0, -1.0};
                t1 = t2 - xA;
                t2 = t2 + xA;
                RFFT_OutV[i] = (CplxMult_f16(tw, t1) + t2) * (F16V_DSP) {0.5f, 0.5f};
        }
        #ifdef PRINTDEB
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%f%+fj, ",  RFFT_OutV[j-1][0], RFFT_OutV[j-1][1]); printf("])\n");
        #endif
}

void RFFT_DIF_Seq_f32(float *Data, float *RFFT_Out, float *Twiddles, float *RTwiddles, short int *SwapTable, int N_fft)
{
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_f32(Data, Twiddles, (N_fft>>1), 0);
        } else {
                Radix2FFT_DIF_Seq_f32(Data, Twiddles, (N_fft>>1), 0);
        }
        SwapSamples_Seq_f32(Data, SwapTable, (N_fft>>1));

        int  k;                                 /* Loop Counter */
        float twR, twI;                         /* RFFT Twiddle coefficients */
        float *pA = Data;                       /* increasing pointer */
        float *pB = Data;                       /* decreasing pointer */
        float xAR, xAI, xBR, xBI;               /* temporary variables */
        float t1a, t1b;                         /* temporary variables */
        float p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        xBR = pB[0];
        xBI = pB[1];
        RFFT_Out[0] = xBR + xBI;
        RFFT_Out[1] = 0.0f;
        RFFT_Out[2*(k+1)]   = xBR - xBI;
        RFFT_Out[2*(k+1)+1] = 0.0f;

        pB  = pA + 2*k;
        pA += 2;
        RTwiddles += 2;
        RFFT_Out += 2;
        for (int i=0; i<k; i++)
        {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                        XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */

                xBR = pB[-2*i];
                xBI = pB[-2*i+1];
                xAR = pA[2*i];
                xAI = pA[2*i+1];

                twR = RTwiddles[2*i];
                twI = RTwiddles[2*i+1];

                t1a = xBR - xAR ;
                t1b = xBI + xAI ;

                // printf("%d xAr %10.8f, xAI %10.8f, xBR %10.8f, xBI %10.8f, twR %10.8f, twI %10.8f\n", i, xAR, xAI, xBR, xBI, twR, twI);
                // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
                // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
                p0 = twR * t1a;
                p1 = twI * t1a;
                p2 = twR * t1b;
                p3 = twI * t1b;

                RFFT_Out[2*i]   = 0.5f * (xAR + xBR + p0 + p3 ); //xAR
                RFFT_Out[2*i+1] = 0.5f * (xAI - xBI + p1 - p2 ); //xAI
        }
        #ifdef PRINTDEB
        RFFT_Out -= 2;
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%f%+fj, ",  RFFT_Out[2*(j)], RFFT_Out[2*(j)+1]); printf("])\n");
        #endif
}


void IRFFT_DIF_Seq_Fix16(short int *Data, short int *RFFT_Out, short int *Twiddles, short int *RTwiddles, short int *SwapTable, int N_fft)
{
        v2s *__restrict__ RFFT_OutV = (v2s * __restrict__) RFFT_Out;
        v2s *__restrict__ RTwiddlesV = (v2s * __restrict__) RTwiddles;

        int  k;                                 /* Loop Counter */
        short int twR, twI;                         /* RFFT Twiddle coefficients */
        v2s *pA = (v2s *) Data;               /* increasing pointer */
        v2s *pB = (v2s *) Data;               /* decreasing pointer */
        short int xAR, xAI, xBR, xBI;               /* temporary variables */
        v2s xA, xB, t1, t2, tw;

        k = (N_fft>>1) - 1;
        xAR = pA[0][0];
        xAI = pA[0][1];
        xBR = pA[k+1][0];

        RFFT_OutV[0][0] = (xAR + xAI + xBR) >> 1;
        RFFT_OutV[0][1] = (xAR + xAI - xBR) >> 1;

        pB  =  pA + k;
        pA +=  1;
        RTwiddlesV += 1;
        RFFT_OutV += 1;

        for (int i=0; i<k; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddlesV[i];
                /* t1 = {xAR - xBR, xAI + xBI} */
                t1 = gap_sub2div2(xA, gap_cplxconj(xB));
                t2 = gap_add2div2(xA, gap_cplxconj(xB));
                RFFT_OutV[i] = t2 - gap_cplxmuls(gap_cplxconj(tw), t1);
        }
        RFFT_OutV -= 1;

        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_Fix16(RFFT_Out, Twiddles, (N_fft>>1), 1);
        } else {
                Radix2FFT_DIF_Seq_Fix16(RFFT_Out, Twiddles, (N_fft>>1), 1);
        }
        SwapSamples_Seq_Fix16(RFFT_Out, SwapTable, (N_fft>>1));

        #ifdef PRINTDEB
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%d%+dj, ",  RFFT_OutV[j][0], RFFT_OutV[j][1]); printf("])\n");
        #endif
}

void IRFFT_DIF_Seq_f16(F16_DSP *Data, F16_DSP *RFFT_Out, F16_DSP *Twiddles, F16_DSP *RTwiddles, short int *SwapTable, int N_fft)
{
        F16V_DSP *__restrict__ RFFT_OutV = (F16V_DSP * __restrict__) RFFT_Out;
        F16V_DSP *__restrict__ RTwiddlesV = (F16V_DSP * __restrict__) RTwiddles;

        int  k;                                 /* Loop Counter */
        F16_DSP twR, twI;                         /* RFFT Twiddle coefficients */
        F16V_DSP *pA = (F16V_DSP *) Data;               /* increasing pointer */
        F16V_DSP *pB = (F16V_DSP *) Data;               /* decreasing pointer */
        F16_DSP xAR, xAI, xBR, xBI;               /* temporary variables */
        F16V_DSP xA, xB, xBconj, t1, t2, tw;
        F16_DSP t1a, t1b;                         /* temporary variables */
        F16_DSP p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        xAR = pA[0][0];
        xAI = pA[0][1];
        xBR = pA[k+1][0];

        RFFT_OutV[0][0] = 0.5f * ( xAR + xAI + xBR);
        RFFT_OutV[0][1] = 0.5f * ( xAR + xAI - xBR);

        pB  =  pA + k;
        pA +=  1;
        RTwiddlesV += 1;
        RFFT_OutV += 1;
        for (int i=0; i<k; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xB =  pB[-i];
                xA =  pA[ i];

                tw = RTwiddlesV[i];
                xBconj = xB * (F16V_DSP) {1.0, -1.0};
                t1 = xA - xBconj;
                t2 = xA + xBconj;

                // xA + xB* - {t1R*twR + twI*t1I, -t1R*twI + t1I*twR} = 
                // {xAR + xBR - t1R*twR - twI*t1I = xAR + xBR - r - s,
                //  xAI - xBI + t1R*twI - twR*t1I = xAI - xBI + t - u}
                RFFT_OutV[i] = (t2 - CplxMult_f16(tw*(F16V_DSP){1.0, -1.0}, t1)) * (F16V_DSP) {0.5f, 0.5f};
        }
        RFFT_OutV -= 1;

        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_f16(RFFT_Out, Twiddles, (N_fft>>1), 1);
        } else {
                Radix2FFT_DIF_Seq_f16(RFFT_Out, Twiddles, (N_fft>>1), 1);
        }
        SwapSamples_Seq_f16(RFFT_Out, SwapTable, (N_fft>>1));

        #ifdef PRINTDEB
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%f%+fj, ",  RFFT_OutV[j][0], RFFT_OutV[j][1]); printf("])\n");
        #endif
}

void IRFFT_DIF_Seq_f32(float *Data, float *RFFT_Out, float *Twiddles, float *RTwiddles, short int *SwapTable, int N_fft)
{
        int  k;                                 /* Loop Counter */
        float twR, twI;                         /* RFFT Twiddle coefficients */
        float *pA = Data;                       /* increasing pointer */
        float *pB = Data;                       /* decreasing pointer */
        float xAR, xAI, xBR, xBI;               /* temporary variables */
        float t1a, t1b;                         /* temporary variables */
        float r, s, t, u;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        xAR = pA[0];
        xAI = pA[1];
        xBR = pA[2*(k+1)];

        RFFT_Out[0] = 0.5f * ( xAR + xAI + xBR );
        RFFT_Out[1] = 0.5f * ( xAR + xAI - xBR );

        pB  =  pA + 2*k;
        pA +=  2;
        RTwiddles += 2;
        RFFT_Out += 2;

        for (int i=0; i<k; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xBR =  pB[-2*i];
                xBI =  pB[-2*i+1];
                xAR =  pA[2*i];
                xAI =  pA[2*i+1];

                twR = RTwiddles[2*i];
                twI = RTwiddles[2*i+1];

                t1a = xAR - xBR ;
                t1b = xAI + xBI ;

                r = twR * t1a;
                s = twI * t1b;
                t = twI * t1a;
                u = twR * t1b;

                // real(tw * (xA - xB)) = twR * (xAR - xBR) - twI * (xAI - xBI);
                // imag(tw * (xA - xB)) = twI * (xAR - xBR) + twR * (xAI - xBI);
                RFFT_Out[2*i]   = 0.5f * (xAR + xBR - r - s ); //xAR
                RFFT_Out[2*i+1] = 0.5f * (xAI - xBI + t - u ); //xAI
        }
        RFFT_Out -= 2;

        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Seq_f32(RFFT_Out, Twiddles, (N_fft>>1), 1);
        } else {
                Radix2FFT_DIF_Seq_f32(RFFT_Out, Twiddles, (N_fft>>1), 1);
        }
        SwapSamples_Seq_f32(RFFT_Out, SwapTable, (N_fft>>1));

        #ifdef PRINTDEB
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%f%+fj, ",  RFFT_Out[2*j+0], RFFT_Out[2*j+1]); printf("])\n");
        #endif
}


/* Real Sample FFT functions: Parallel */

void RFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg){
        v2s *__restrict__ Data = (v2s * __restrict__) Arg->Data;
        v2s *__restrict__ RFFT_Out = (v2s * __restrict__) Arg->RFFT_Out;
        v2s *__restrict__ Twiddles = (v2s * __restrict__) Arg->Twiddles;
        v2s *__restrict__ RTwiddles = (v2s * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        FFT_Arg_T FFTArg = (FFT_Arg_T){Data, Twiddles, N_fft>>1, 0};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_Fix16(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_Fix16(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){Data, SwapTable, N_fft>>1};
        SwapSamples_Par(&SwapArg);

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                          /* Loop Counter */
        signed short int twR, twI;       /* RFFT Twiddle coefficients */
        v2s *pA = (v2s *) Data;          /* increasing pointer */
        v2s *pB = (v2s *) Data;          /* decreasing pointer */
        v2s xA, xB;                      /* temporary variables */
        v2s t1, t2, tw;                  /* temporary variables */
        signed short int p0, p1, p2, p3; /* temporary variables */
        int xBR, xBI, xAR, xAI;

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        if (CoreId == 0){
                xBR = pB[0][0];
                xBI = pB[0][1];
         
                // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
                // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
                // XA(1) = 1/2*( U1 - imag(U2) +  i*( U1 +imag(U2) ));
                RFFT_Out[0][0] = ( xBR + xBI ) >> 1;
                RFFT_Out[0][1] = 0;

                // Gr(N) = Gr(0) - Gi(0)
                // Gi(N) = 0
                RFFT_Out[k+1][0] = ( xBR - xBI );
                RFFT_Out[k+1][1] = 0;
        }
        gap_waitbarrier(0);

        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  = pA + k;
        pA += 1;
        RTwiddles += 1;
        RFFT_Out += 1;
        for (int i=First; i<Last; i++) {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                    XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */

                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddles[i];
                /* t1 = {xBR - xAR, xBI + xAI} */
                t1 = gap_add2div4(-xA, gap_cplxconj(xB));
                t2 = gap_add2div4( xA, gap_cplxconj(xB));
                RFFT_Out[i] = gap_cplxmuls(tw, t1) + t2;
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%d%+dj, ",  RFFT_Out[j-1][0], RFFT_Out[j-1][1]); printf("])\n");
}
#endif
}

void RFFT_DIF_Par_Fix32_Scal(RFFT_scal_Arg_T *Arg){
        int *__restrict__ Data = (int * __restrict__) Arg->Data;
        cmplx *__restrict__ RFFT_Out = (cmplx * __restrict__) Arg->RFFT_Out;
        v2s *__restrict__ Twiddles = (v2s * __restrict__) Arg->Twiddles;
        v2s *__restrict__ RTwiddles = (v2s * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        signed char *__restrict__ shift_fft = (signed char *__restrict__) Arg->shift_fft;
        signed char *__restrict__ shift_rfft = (signed char *__restrict__) Arg->shift_rfft;
        unsigned int N_fft = Arg->N_fft;

        FFT_scal_Arg_T FFTArg = (FFT_scal_Arg_T){Data, Twiddles, N_fft>>1, 0, shift_fft};
        Radix2FFT_DIF_Par_Fix32_Scal(&FFTArg);
        SwapSamples_scal_Arg_T SwapArg = (SwapSamples_scal_Arg_T){Data, shift_fft, SwapTable, N_fft>>1};
        SwapSamples_scal(&SwapArg);

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                          /* Loop Counter */
        int twR, twI;       /* RFFT Twiddle coefficients */
        cmplx *pA = (cmplx *) Data;          /* increasing pointer */
        cmplx *pB = (cmplx *) Data;          /* decreasing pointer */
        cmplx xA, xB;                      /* temporary variables */
        int t1r, t1i, t2r, t2i;
        v2s tw;                  /* temporary variables */

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        if (CoreId == 0){         
                // U1 = XA(1) + XB(1); % It is real
                int t1a = (pB[0][0]) + (pA[0][0]);
         
                // U2 = XB(1) - XA(1); % It is imaginary
                int t1b = (pB[0][1]) + (pA[0][1]);
         
                // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
                // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
                RFFT_Out[0][0] = t1a + t1b;
                RFFT_Out[0][1] = t1a - t1b;
                // XA(1) = 1/2*( U1 - imag(U2) +  i*( U1 +imag(U2) ));
                shift_rfft[0] = shift_fft[0];
        }
        gap_waitbarrier(0);

        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  = pA + k;
        pA += 1;
        signed char *shift_fftB = shift_fft + k;
        signed char *shift_fftA = shift_fft + 1;
        RTwiddles += 1;
        RFFT_Out += 1;
        for (int i=First; i<Last; i++) {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                    XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */
                // xA = pA[ i];
                // xB = pB[-i];

                // align exponents for xA and pB[-i];
                int Dshift = shift_fftB[-i] - shift_fftA[i];
                if (Dshift > 0) {
                        pB[-i] >>= (cmplx) {Dshift,Dshift};
                        shift_fftB[-i] = shift_fftA[i];
                }
                else {
                        pA[i] >>= (cmplx) {-Dshift,-Dshift};
                        shift_fftA[i] = shift_fftB[-i];
                }


                tw = RTwiddles[i];

                t1r = -(pA[i][0]) + (pB[-i][0]);
                t1i = -(pA[i][1]) - (pB[-i][1]);
                t2r =  (pA[i][0]) + (pB[-i][0]);
                t2i =  (pA[i][1]) - (pB[-i][1]);

                // printf("shift %d\n", shift);
                RFFT_Out[i][0] = ((t1r>>15) * (((int) tw[1])) - ((t1i>>15) * ((int) tw[1]))) + t2r;
                RFFT_Out[i][1] = ((t1r>>15) * (((int) tw[0])) + ((t1i>>15) * ((int) tw[0]))) + t2i;
                shift_rfft[i+1] = shift_fftA[i];
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%d%+dj, ",  RFFT_Out[j-1][0], RFFT_Out[j-1][1]); printf("])\n");
        printf("\nshift_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%d, ",  shift_rfft[j]); printf("])\n");
}
#endif
}

void RFFT_DIF_Par_f16(RFFT_Arg_T *Arg){
        F16V_DSP *__restrict__ Data = (F16V_DSP * __restrict__) Arg->Data;
        F16V_DSP *__restrict__ RFFT_Out = (F16V_DSP * __restrict__) Arg->RFFT_Out;
        F16V_DSP *__restrict__ Twiddles = (F16V_DSP * __restrict__) Arg->Twiddles;
        F16V_DSP *__restrict__ RTwiddles = (F16V_DSP * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        FFT_Arg_T FFTArg = (FFT_Arg_T){Data, Twiddles, N_fft>>1, 0};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_f16(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_f16(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){Data, SwapTable, N_fft>>1};
        SwapSamples_Par_f16(&SwapArg);
        // for (int i=0; i<N_fft>>1; i++) printf("%f%+fj, ", Data[i][0], Data[i][1]);

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                                 /* Loop Counter */
        F16_DSP twR, twI;                         /* RFFT Twiddle coefficients */
        F16V_DSP *pA = (F16V_DSP *) Data;               /* increasing pointer */
        F16V_DSP *pB = (F16V_DSP *) Data;               /* decreasing pointer */
        F16_DSP xAR, xAI, xBR, xBI;               /* temporary variables */
        F16V_DSP xA, xB, t1, t2, tw;
        F16_DSP t1a, t1b;                         /* temporary variables */
        F16_DSP p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        if (CoreId == 0){
                xBR = pB[0][0];
                xBI = pB[0][1];
         
                // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
                // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
                RFFT_Out[0][0] = xBR + xBI;
                RFFT_Out[0][1] = 0.0f;
                // XA(1) = 1/2*( U1 - imag(U2) +  i*( U1 +imag(U2) ));
                RFFT_Out[k+1][0] = xBR - xBI;
                RFFT_Out[k+1][1] = 0.0f;
        }
        gap_waitbarrier(0);

        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  = pA + k;
        pA += 1;
        RTwiddles += 1;
        RFFT_Out += 1;
        for (int i=First; i<Last; i++) {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                    XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */
                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddles[i];
                /* t1 = {xBR - xAR, xBI + xAI} */
                /* t1 = {xBR + xAR,-xBI + xAI} */
                t2 = xB * (F16V_DSP) {1.0, -1.0};
                t1 = t2 - xA;
                t2 = t2 + xA;
                RFFT_Out[i] = (CplxMult_f16(tw, t1) + t2) * (F16V_DSP) {0.5f, 0.5f};
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%f%+fj, ",  RFFT_Out[j-1][0], RFFT_Out[j-1][1]); printf("])\n");
}
#endif
}

void RFFT_DIF_Par_f32(RFFT_Arg_T *Arg){
        float *__restrict__ Data = (float * __restrict__) Arg->Data;
        float *__restrict__ RFFT_Out = (float * __restrict__) Arg->RFFT_Out;
        float *__restrict__ Twiddles = (float * __restrict__) Arg->Twiddles;
        float *__restrict__ RTwiddles = (float * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        FFT_Arg_T FFTArg = (FFT_Arg_T){Data, Twiddles, N_fft>>1, 0};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_f32(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_f32(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){Data, SwapTable, N_fft>>1};
        SwapSamples_Par_f32(&SwapArg);

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                                 /* Loop Counter */
        float twR, twI;                         /* RFFT Twiddle coefficients */
        float *pA = Data;                       /* increasing pointer */
        float *pB = Data;                       /* decreasing pointer */
        float xAR, xAI, xBR, xBI;               /* temporary variables */
        float t1a, t1b;                         /* temporary variables */
        float p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        /* Pack first and last sample of the frequency domain together */
        if (CoreId == 0){
                xBR = pB[0];
                xBI = pB[1];
                RFFT_Out[0] = xBR + xBI;
                RFFT_Out[1] = 0.0f;
                RFFT_Out[2*(k+1)]   = xBR - xBI;
                RFFT_Out[2*(k+1)+1] = 0.0f;
        }
        gap_waitbarrier(0);

        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  = pA + 2*k;
        pA += 2;
        RTwiddles += 2;
        RFFT_Out += 2;
        for (int i=First; i<Last; i++)
        {
                /*
                    function X = my_split_rfft(X, ifftFlag)
                    % X is a series of real numbers
                    L  = length(X);
                    XC = X(1:2:end) +i*X(2:2:end);
                    XA = fft(XC);
                    XB = conj(XA([1 end:-1:2]));
                    TW = i*exp(-2*pi*i*[0:L/2-1]/L).';
                    for l = 2:L/2
                        XA(l) = 1/2 * (XA(l) + XB(l) + TW(l) * (XB(l) - XA(l)));
                    end
                    XA(1) = 1/2* (XA(1) + XB(1) + TW(1) * (XB(1) - XA(1))) + i*( 1/2*( XA(1) + XB(1) + i*( XA(1) - XB(1))));
                    X = XA;
                */

                xBR = pB[-2*i];
                xBI = pB[-2*i+1];
                xAR = pA[2*i];
                xAI = pA[2*i+1];

                twR = RTwiddles[2*i];
                twI = RTwiddles[2*i+1];

                t1a = xBR - xAR ;
                t1b = xBI + xAI ;

                // printf("%d xAr %10.8f, xAI %10.8f, xBR %10.8f, xBI %10.8f, twR %10.8f, twI %10.8f\n", i, xAR, xAI, xBR, xBI, twR, twI);
                // real(tw * (xB - xA)) = twR * (xBR - xAR) - twI * (xBI - xAI);
                // imag(tw * (xB - xA)) = twI * (xBR - xAR) + twR * (xBI - xAI);
                p0 = twR * t1a;
                p1 = twI * t1a;
                p2 = twR * t1b;
                p3 = twI * t1b;

                RFFT_Out[2*i]   = 0.5f * (xAR + xBR + p0 + p3 ); //xAR
                RFFT_Out[2*i+1] = 0.5f * (xAI - xBI + p1 - p2 ); //xAI
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
if (CoreId==0){
        RFFT_Out -= 2;
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<((N_fft>>1)+1); j++) printf("%f%+fj, ",  RFFT_Out[2*(j)], RFFT_Out[2*(j)+1]); printf("])\n");
}
#endif
}

void IRFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg){
        v2s *__restrict__ Data = (v2s * __restrict__) Arg->Data;
        v2s *__restrict__ RFFT_Out = (v2s * __restrict__) Arg->RFFT_Out;
        v2s *__restrict__ Twiddles = (v2s * __restrict__) Arg->Twiddles;
        v2s *__restrict__ RTwiddles = (v2s * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                                 /* Loop Counter */
        short int twR, twI;                         /* RFFT Twiddle coefficients */
        v2s *pA = (v2s *) Data;               /* increasing pointer */
        v2s *pB = (v2s *) Data;               /* decreasing pointer */
        short int xAR, xAI, xBR, xBI;               /* temporary variables */
        v2s xA, xB, t1, t2, tw;

        k = (N_fft>>1) - 1;
        if (CoreId == 0){
                xAR = pA[0][0];
                xAI = pA[0][1];
                xBR = pA[k+1][0];

                RFFT_Out[0][0] = (xAR + xAI + xBR) >> 1;
                RFFT_Out[0][1] = (xAR + xAI - xBR) >> 1;
        }
        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  =  pA + k;
        pA +=  1;
        RTwiddles += 1;
        RFFT_Out += 1;

        for (int i=First; i<Last; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xA = pA[ i];
                xB = pB[-i];

                tw = RTwiddles[i];
                /* t1 = {xAR - xBR, xAI + xBI} */
                t1 = gap_sub2div2(xA, gap_cplxconj(xB));
                t2 = gap_add2div2(xA, gap_cplxconj(xB));
                RFFT_Out[i] = t2 - gap_cplxmuls(gap_cplxconj(tw), t1);
        }
        gap_waitbarrier(0);
        RFFT_Out -= 1;
#ifdef PRINTDEB
if (CoreId==0){
        printf("\nin_ifft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%d%+dj, ",  RFFT_Out[j][0], RFFT_Out[j][1]); printf("])\n");
}
#endif
        FFT_Arg_T FFTArg = (FFT_Arg_T){RFFT_Out, Twiddles, N_fft>>1, 1};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_Fix16(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_Fix16(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){RFFT_Out, SwapTable, N_fft>>1};
        SwapSamples_Par(&SwapArg);
#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%d, %d, ",  RFFT_Out[j][0], RFFT_Out[j][1]); printf("])\n");
}
#endif
}

void IRFFT_DIF_Par_f16(RFFT_Arg_T *Arg){
        F16V_DSP *__restrict__ Data = (F16V_DSP * __restrict__) Arg->Data;
        F16V_DSP *__restrict__ RFFT_Out = (F16V_DSP * __restrict__) Arg->RFFT_Out;
        F16V_DSP *__restrict__ Twiddles = (F16V_DSP * __restrict__) Arg->Twiddles;
        F16V_DSP *__restrict__ RTwiddles = (F16V_DSP * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();

        int  k;                                 /* Loop Counter */
        F16_DSP twR, twI;                         /* RFFT Twiddle coefficients */
        F16V_DSP *pA = (F16V_DSP *) Data;               /* increasing pointer */
        F16V_DSP *pB = (F16V_DSP *) Data;               /* decreasing pointer */
        F16_DSP xAR, xAI, xBR, xBI;               /* temporary variables */
        F16V_DSP xA, xB, xBconj, t1, t2, tw;
        F16_DSP t1a, t1b;                         /* temporary variables */
        F16_DSP p0, p1, p2, p3;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        if (CoreId == 0){
                xAR = pA[0][0];
                xAI = pA[0][1];
                xBR = pA[k+1][0];

                RFFT_Out[0][0] = 0.5f * ( xAR + xAI + xBR);
                RFFT_Out[0][1] = 0.5f * ( xAR + xAI - xBR);
        }
        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  =  pA + k;
        pA +=  1;
        RTwiddles += 1;
        RFFT_Out += 1;
        for (int i=First; i<Last; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xB =  pB[-i];
                xA =  pA[ i];

                tw = RTwiddles[i];
                xBconj = xB * (F16V_DSP) {1.0, -1.0};
                t1 = xA - xBconj;
                t2 = xA + xBconj;

                // xA + xB* - {t1R*twR + twI*t1I, -t1R*twI + t1I*twR} = 
                // {xAR + xBR - t1R*twR - twI*t1I = xAR + xBR - r - s,
                //  xAI - xBI + t1R*twI - twR*t1I = xAI - xBI + t - u}
                RFFT_Out[i] = (t2 - CplxMult_f16(tw*(F16V_DSP){1.0, -1.0}, t1)) * (F16V_DSP) {0.5f, 0.5f};
        }
        gap_waitbarrier(0);
        RFFT_Out -= 1;

#ifdef PRINTDEB
if (CoreId==0){
        printf("\nin_ifft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%f%+fj, ",  RFFT_Out[j][0], RFFT_Out[j][1]); printf("])\n");
}
#endif
        FFT_Arg_T FFTArg = (FFT_Arg_T){RFFT_Out, Twiddles, N_fft>>1, 1};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_f16(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_f16(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){RFFT_Out, SwapTable, N_fft>>1};
        SwapSamples_Par_f16(&SwapArg);

#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%f, %f, ",  RFFT_Out[j][0], RFFT_Out[j][1]); printf("])\n");
}
#endif
}

void IRFFT_DIF_Par_f32(RFFT_Arg_T *Arg){
        float *__restrict__ Data = (float * __restrict__) Arg->Data;
        float *__restrict__ RFFT_Out = (float * __restrict__) Arg->RFFT_Out;
        float *__restrict__ Twiddles = (float * __restrict__) Arg->Twiddles;
        float *__restrict__ RTwiddles = (float * __restrict__) Arg->RTwiddles;
        short *__restrict__ SwapTable = (short int *__restrict__) Arg->SwapTable;
        unsigned int N_fft = Arg->N_fft;

        int First, Last, Chunk, CoreId;
        CoreId = gap_coreid();
        int  k;                                 /* Loop Counter */
        float twR, twI;                         /* RFFT Twiddle coefficients */
        float *pA = Data;                       /* increasing pointer */
        float *pB = Data;                       /* decreasing pointer */
        float xAR, xAI, xBR, xBI;               /* temporary variables */
        float t1a, t1b;                         /* temporary variables */
        float r, s, t, u;                   /* temporary variables */

        k = (N_fft>>1) - 1;
        if (CoreId == 0){
                xAR = pA[0];
                xAI = pA[1];
                xBR = pA[2*(k+1)];

                RFFT_Out[0] = 0.5f * ( xAR + xAI + xBR );
                RFFT_Out[1] = 0.5f * ( xAR + xAI - xBR );
        }
        Chunk = ChunkSize(k);
        First = CoreId*Chunk; Last = Min(First+Chunk, k);
        pB  =  pA + 2*k;
        pA +=  2;
        RTwiddles += 2;
        RFFT_Out += 2;

        for (int i=First; i<Last; i++)
        {
                /* G is half of the frequency complex spectrum */
                //for k = 2:N
                //    Xk(k) = 1/2 * (G(k) + conj(G(N-k+2)) + Tw(k)*( G(k) - conj(G(N-k+2))));
                xBR =  pB[-2*i];
                xBI =  pB[-2*i+1];
                xAR =  pA[2*i];
                xAI =  pA[2*i+1];

                twR = RTwiddles[2*i];
                twI = RTwiddles[2*i+1];

                t1a = xAR - xBR ;
                t1b = xAI + xBI ;

                r = twR * t1a;
                s = twI * t1b;
                t = twI * t1a;
                u = twR * t1b;

                // real(tw * (xA - xB)) = twR * (xAR - xBR) - twI * (xAI - xBI);
                // imag(tw * (xA - xB)) = twI * (xAR - xBR) + twR * (xAI - xBI);
                RFFT_Out[2*i]   = 0.5f * (xAR + xBR - r - s ); //xAR
                RFFT_Out[2*i+1] = 0.5f * (xAI - xBI + t - u ); //xAI
        }
        gap_waitbarrier(0);
        RFFT_Out -= 2;

#ifdef PRINTDEB
if (CoreId==0){
        printf("\nin_ifft = np.array([\n\t"); for (int j=0; j<(N_fft>>1); j++) printf("%f%+fj, ",  RFFT_Out[2*j], RFFT_Out[2*j+1]); printf("])\n");
}
#endif
        FFT_Arg_T FFTArg = (FFT_Arg_T){RFFT_Out, Twiddles, N_fft>>1, 1};
        if (((N_fft >> 1) == 256) || ((N_fft >> 1) == 1024)) {
                Radix4FFT_DIF_Par_f32(&FFTArg);
        } else {
                Radix2FFT_DIF_Par_f32(&FFTArg);
        }
        SwapSamples_Arg_T SwapArg = (SwapSamples_Arg_T){RFFT_Out, SwapTable, N_fft>>1};
        SwapSamples_Par_f32(&SwapArg);

#ifdef PRINTDEB
if (CoreId==0){
        printf("\nout_rfft = np.array([\n\t"); for (int j=0; j<N_fft; j++) printf("%f, ",  RFFT_Out[j]); printf("])\n");
}
#endif
}


/* -------------------------------------------------------- Swap Functions ------------------------------------------------------------- */

void SwapSamples_Seq_Fix16(short int *Data, short int *SwapTable, int Ni)

{
        v2s *__restrict__ DataV = (v2s *) Data;
        for (int i = 0; i < Ni; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                        v2s S = DataV[i];
                       DataV[i] = DataV[SwapIndex]; DataV[SwapIndex] = S;
                }
        }
}

void SwapSamples_Seq_f16(F16_DSP *Data, short int *SwapTable, int Ni)

{
        F16V_DSP *__restrict__ DataV = (F16V_DSP *) Data;
        for (int i = 0; i < Ni; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                F16V_DSP S = DataV[i];
                       DataV[i] = DataV[SwapIndex]; DataV[SwapIndex] = S;
                }
        }
}

void SwapSamples_Seq_f32(float *Data, short int *SwapTable, int Ni)

{
        for (int i = 0; i < Ni; i++) {
                int SwapIndex = SwapTable[i];
                if (i < SwapIndex) {
                        float R = Data[2*i], I = Data[2*i+1];
                        Data[2*i  ] = Data[2*SwapIndex];   Data[2*SwapIndex] = R;
                        Data[2*i+1] = Data[2*SwapIndex+1]; Data[2*SwapIndex+1] = I;
                }
        }
}

void SwapSamples_Par(SwapSamples_Arg_T *Arg)

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
                printf("\nout_swapped_fft = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%d%+dj, ",  Data[j][0], Data[j][1]); printf("])\n");
        }
        #endif
}

void SwapSamples_Par_f16(SwapSamples_Arg_T *Arg)

{
        F16V_DSP *__restrict__ Data = (F16V_DSP *) Arg->Data;
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
                        F16V_DSP S = Data[i];
                        Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
                }
        }
        // Synchronize all cores for current layer of the trellis
        gap_waitbarrier(0);
        #ifdef PRINTDEB
        if (CoreId==0){
                printf("\nout_swapped_fft = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%f%+fj, ",  Data[j][0], Data[j][1]); printf("])\n");
        }
        #endif
}

void SwapSamples_Par_f32(SwapSamples_Arg_T *Arg)

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
                printf("\nout_swapped_fft = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%f%+fj, ",  Data[2*j], Data[2*j+1]); printf("])\n");
        }
        #endif
}

void SwapSamples_Par_Fix32( SwapSamples_Arg_T *Arg)
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
                        printf("\nout_swapped_fft = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%d%+dj, ",  Datav[j][0], Datav[j][1]); printf("])\n");
                }
                gap_waitbarrier(0);
        #endif
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
                if (i < SwapIndex) {
                        scale[i] = scale[SwapIndex]; scale[SwapIndex] = TMP;
                        Datav[i] = Datav[SwapIndex]; Datav[SwapIndex] = S;
                }
        }

        gap_waitbarrier(0);

        #ifdef PRINTDEB
                if (CoreId==0) {
                        printf("\nout_swapped_fft = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%d%+dj, ",  Datav[j][0], Datav[j][1]); 
                        printf("])\nout_fft_shift = np.array([\n\t"); for (int j=0; j<Ni; j++) printf("%d, ", scale[j]); printf("])\n");
                }
                gap_waitbarrier(0);
        #endif
}
