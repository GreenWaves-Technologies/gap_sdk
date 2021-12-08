/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#include <stdio.h>
#include "CNN_BasicKernels_SQ8.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
        Log2Core = gap_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

static int FirstDefinedOutput(int F, int Pad, int Stride)

{
        // k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

        return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(int DimIn, int F, int PadL, int Stride, int D)

{
        // k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

        return ((DimIn - ((F-1)/2*D - PadL + (F/2)*D) + Stride-1)/Stride);
}

// #define OLD
void KerPar_MM_Conv1D_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = Min(CoreId*ChunkCell, OutFeat), Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int Iter = (L-F)*Fx;
	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter&0x2) ((short int *)(ColBuff+F*Fx))[Iter/2-1]=0;
			if (Iter&0x1) ((signed char *)(ColBuff+F*Fx))[Iter-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
#ifdef OLD
			for (int f=F; f<L; f++) {
				for (int i=Lb; i<Rb; i++) ColBuff[Fx*f+i+Off] = In[f*W*H+l*W+i];
			}
#else
			int Size = Rb-Lb;
			if (Size>4) {
				if (Size&0x2) {
					if (Size&0x1) {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-3)) = *((short int *)(In+f*W*H+PosL*W+Rb-3));
							*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
						}
					} else {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-2)) = *((short int *)(In+f*W*H+PosL*W+Rb-2));
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
						*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
			} else if (Size>=2) {
				if (Size&0x4) {
					for (int f=F; f<L; f++) {
						*((int *)(ColBuff+Fx*f+Off+Lb)) = *((int *)(In+f*W*H+PosL*W+Lb));
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
						ColBuff[Fx*f+Off+Lb+2] = In[f*W*H+PosL*W+Lb+2];
					}
				} else {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
					}
				}
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
#endif
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void _KerPar_MM_Conv1x1_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
		ColBuff must be 4 byte aligned large enough to accomodate 2*Align(Feat, 4)
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff0 = Arg->ColBuff;
        signed char * __restrict__ ColBuff1 = Arg->ColBuff+ALIGN(InFeat, 2);

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(InFeat, 8) elements */
	v4s * __restrict__ VBuff0 = (v4s *) ColBuff0;
	v4s * __restrict__ VBuff1 = (v4s *) ColBuff1;
	unsigned int W_In1 = InFeat;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = ((W_In1+3)/4);
	((int *)ColBuff0)[Tail-1] = 0;
	((int *)ColBuff1)[Tail-1] = 0;
	
	int PosL = 0;
	int Iter = L-F;
	for (int l=0; l<Ho; l++) {
		int PosC = 0;
		for (int c=0; c<Wo/2; c++) {
			if (Iter>=4) {
				for (int f=0; f<(Iter/4); f++) {
					int X0 = ((int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[f];
					int X1 = ((int *)(In+PosL*W*InFeat + (PosC+1)*InFeat+F))[f];
					((int *)(ColBuff0+F))[f] = X0;
					((int *)(ColBuff1+F))[f] = X1;
				}
				if (Iter&0x2) {
					int X0 = ((short int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter/2-1];
					int X1 = ((short int *)(In+PosL*W*InFeat + (PosC+1)*InFeat+F))[Iter/2-1];
					((short int *)(ColBuff0+F))[Iter/2-1] = X0;
					((short int *)(ColBuff1+F))[Iter/2-1] = X1;
				}
				if (Iter&0x1) {
					int X0 = ((signed char *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter-1];
					int X1 = ((signed char *)(In+PosL*W*InFeat + (PosC+1)*InFeat+F))[Iter-1];
					((signed char *)(ColBuff0+F))[Iter-1] = X0;
					((signed char *)(ColBuff1+F))[Iter-1] = X1;
				}
			} else if (Iter>=2) {
				if (Iter&0x2) {
					int X0 = ((short int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter/2-1];
					int X1 = ((short int *)(In+PosL*W*InFeat + (PosC+1)*InFeat+F))[Iter/2-1];
					((short int *)(ColBuff0+F))[Iter/2-1] = X0;
					((short int *)(ColBuff1+F))[Iter/2-1] = X1;
				}
				if (Iter&0x1) {
					int X0 = ((signed char *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter-1];
					int X1 = ((signed char *)(In+PosL*W*InFeat + (PosC+1)*InFeat+F))[Iter-1];
					((signed char *)(ColBuff0+F))[Iter-1] = X0;
					((signed char *)(ColBuff1+F))[Iter-1] = X1;
				}
			} else {
				int X0 = In[PosL*W*InFeat + (PosC+0)*InFeat + F];
				int X1 = In[PosL*W*InFeat + (PosC+1)*InFeat + F];
				ColBuff0[F] = X0;
				ColBuff1[F] = X1;
			}
			PosC += 2*Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
				int S1 = S0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                v4s C0 = VIn1[i];
					v4s V0 = VBuff0[i], V1 = VBuff1[i];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S1 = gap_sumdotp4(V1, C0, S1);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[l*Wo*OutFeat + (2*c+0)*OutFeat + Line] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                        Out[l*Wo*OutFeat + (2*c+1)*OutFeat + Line] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		if (Wo&0X1) {
			int c = Wo-1;
			((int *)ColBuff0)[Tail] = 0;
			if (Iter>=4) {
				for (int f=0; f<(Iter/4); f++) {
					int X0 = ((int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[f];
					((int *)(ColBuff0+F))[f] = X0;
				}
				if (Iter&0x2) {
					int X0 = ((short int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter/2-1];
					((short int *)(ColBuff0+F))[Iter/2-1] = X0;
				}
				if (Iter&0x1) {
					int X0 = ((signed char *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter-1];
					((signed char *)(ColBuff0+F))[Iter-1] = X0;
				}
			} else if (Iter>=2) {
				if (Iter&0x2) {
					int X0 = ((short int *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter/2-1];
					((short int *)(ColBuff0+F))[Iter/2-1] = X0;
				}
				if (Iter&0x1) {
					int X0 = ((signed char *)(In+PosL*W*InFeat + (PosC+0)*InFeat+F))[Iter-1];
					((signed char *)(ColBuff0+F))[Iter-1] = X0;
				}
			} else {
				int X0 = In[PosL*W*InFeat + (PosC+0)*InFeat + F];
				ColBuff0[F] = X0;
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
				int S1 = S0;
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff0[2*i], C1 = VBuff0[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[l*Wo*OutFeat + (c)*OutFeat + Line] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

static void __attribute__ ((noinline)) MatMul2Out(
		signed char *__restrict__ pI,
		signed char *__restrict__ pC,
		int *__restrict__ pBias,
		unsigned char *__restrict__ pSc,
		unsigned char *__restrict__ pScN,
		signed char *__restrict__ pOut0,
		signed char *__restrict__ pOut1,
		unsigned int InFeat,
		unsigned int IterOut,
		unsigned int NormBias
		)

{
	for (int i=0; i<(IterOut/4); i++) {
		signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat,
			    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
		pC+=4;
		int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
		int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
		int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
		int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
		for (int f=0; f<(InFeat/4); f++) {
			v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                               	S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                               	S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                               	S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
			pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
		}
		for (int f=4*(InFeat/4); f<InFeat; f++) {
			int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
			S00 += V0*C0; S01 += V1*C0;
			S10 += V0*C1; S11 += V1*C1;
			S20 += V0*C2; S21 += V1*C2;
			S30 += V0*C3; S31 += V1*C3;
			pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
		}
		unsigned int Sc, ScN;
		Sc = *pSc; ScN = *pScN; pSc++; pScN++;
		*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
		*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
		Sc = *pSc; ScN = *pScN; pSc++; pScN++;
		*pOut0 = gap_clip(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
		*pOut1 = gap_clip(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
		Sc = *pSc; ScN = *pScN; pSc++; pScN++;
		*pOut0 = gap_clip(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
		*pOut1 = gap_clip(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
		Sc = *pSc; ScN = *pScN; pSc++; pScN++;
		*pOut0 = gap_clip(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
		*pOut1 = gap_clip(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	}
	for (int i=4*(IterOut/4); i<IterOut; i++) {
		signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat, *pC0 = pC; // &Filter[(First+i)*InFeat + 0];
		pC++;
		int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
		for (int f=0; f<(InFeat/4); f++) {
			v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0);
	          	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
			pIn0+=4; pIn1+=4; pC0+=4;
		}
		for (int f=4*(InFeat/4); f<InFeat; f++) {
			int V0 = *pIn0, V1 = *pIn1, C0 = *pC0;
			S00 += V0*C0; S01 += V1*C0;
			pIn0++; pIn1++; pC0++;
		}
		unsigned int Sc, ScN;
		Sc = *pSc; ScN = *pScN; pSc++; pScN++;
		*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
		*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
	}
}

void KerPar_MM_Conv1x1_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	unsigned int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);
	int IterOut = Last - First;

	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = 0;
		for (int c=0; c<Wo/2; c++) {
			int *pBias = Bias + First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat+First;
			signed char *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat+First;
			signed char *pC = Filter + First*InFeat;
			signed char *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;

			for (int i=0; i<(IterOut/4); i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
	                        int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
	                        int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                               	S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                               	S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                               	S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
					pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
					pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
					pIn0+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC;
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
			}
			PosC += 2*Sx;
		}
		if (Wo&0X1) {
			PosC = (Wo/2)*2*Sx;
			int *pBias = Bias + First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (Wo-1)*OutFeat+First;
			signed char *pC = Filter + First*InFeat;
			signed char *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;

			for (int i=0; i<(IterOut/4); i++) {
				signed char *pIn0 = pI,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        int S10 = (*pBias)<<NormBias; pBias++;
	                        int S20 = (*pBias)<<NormBias; pBias++;
	                        int S30 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
	                               	S10 = gap_sumdotp4(V0, C1, S10);
	                               	S20 = gap_sumdotp4(V0, C2, S20);
	                               	S30 = gap_sumdotp4(V0, C3, S30);
					pIn0+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
					pIn0++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
			}
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = pI;
	                        int S00 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
					pIn0+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC;
					S00 += V0*C0;
					pIn0++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}


void KerPar_MM_Conv1x1_ReLU_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	unsigned int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);
	int IterOut = Last - First;

	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = 0;
		for (int c=0; c<Wo/2; c++) {
			int *pBias = Bias + First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat+First;
			signed char *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat+First;
			signed char *pC = Filter + First*InFeat;
			signed char *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;

			for (int i=0; i<(IterOut/4); i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + Sx*InFeat,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
	                        int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
	                        int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                               	S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                               	S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                               	S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
					pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
					pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
					pIn0+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC;
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
			}
			PosC += 2*Sx;
		}
		if (Wo&0X1) {
			PosC = (Wo/2)*2*Sx;
			int *pBias = Bias + First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (Wo-1)*OutFeat+First;
			signed char *pC = Filter + First*InFeat;
			signed char *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;

			for (int i=0; i<(IterOut/4); i++) {
				signed char *pIn0 = pI,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        int S10 = (*pBias)<<NormBias; pBias++;
	                        int S20 = (*pBias)<<NormBias; pBias++;
	                        int S30 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
	                               	S10 = gap_sumdotp4(V0, C1, S10);
	                               	S20 = gap_sumdotp4(V0, C2, S20);
	                               	S30 = gap_sumdotp4(V0, C3, S30);
					pIn0+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
					pIn0++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
			}
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = pI;
	                        int S00 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
					pIn0+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC;
					S00 += V0*C0;
					pIn0++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void Ker_MM_Conv1x1_ReLU_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	unsigned int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Ho), First = Min(Ho, CoreId*ChunkCell), Last  = Min(Ho, First+ChunkCell);
	int IterOut = Last - First;

	for (int l=First; l<Last; l++) {
		int PosC = 0;
		for (int c=0; c<Wo/2; c++) {
			int *pBias = Bias;
			signed char *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat;
			signed char *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat;
			signed char *pC = Filter;
			signed char *pI = (In+l*Sy*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;

			for (int i=0; i<(OutFeat/4); i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + Sx*InFeat,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
	                        int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
	                        int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                               	S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                               	S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                               	S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
					pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
					pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	                }
			for (int i=4*(OutFeat/4); i<OutFeat; i++) {
				signed char *pIn0 = pI, *pIn1 = pIn0 + InFeat;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
					pIn0+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC;
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
			}
			PosC += 2*Sx;
		}
		if (Wo&0X1) {
			PosC = (Wo/2)*2*Sx;
			int *pBias = Bias;
			signed char *pOut0 = Out+l*Wo*OutFeat + (Wo-1)*OutFeat;
			signed char *pC = Filter;
			signed char *pI = (In+Sy*l*W*InFeat + (PosC+0)*InFeat);
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;

			for (int i=0; i<(OutFeat/4); i++) {
				signed char *pIn0 = pI,
					    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        int S10 = (*pBias)<<NormBias; pBias++;
	                        int S20 = (*pBias)<<NormBias; pBias++;
	                        int S30 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
	                               	S10 = gap_sumdotp4(V0, C1, S10);
	                               	S20 = gap_sumdotp4(V0, C2, S20);
	                               	S30 = gap_sumdotp4(V0, C3, S30);
					pIn0+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
					pIn0++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
			}
			for (int i=4*(OutFeat/4); i<OutFeat; i++) {
				signed char *pIn0 = pI;
	                        int S00 = (*pBias)<<NormBias; pBias++;
				for (int f=0; f<(InFeat/4); f++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
					pIn0+=4; pC+=4;
				}
				for (int f=4*(InFeat/4); f<InFeat; f++) {
					int V0 = *pIn0, C0 = *pC;
					S00 += V0*C0;
					pIn0++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
			}
		}
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv1D_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			if (Iter>0) {
				if (Iter>=4) {
					for (int f=0; f<(Iter/4); f++)
						for (int i=Lb; i<Rb; i++) ((int *)(ColBuff+(i+Off)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + i*InFeat+F))[f];
					if (Iter&0x2) for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1) for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + i*InFeat+F))[Iter-1];
				} else if (Iter>=2) {
					if (Iter&0x2) for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1) for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + i*InFeat+F))[Iter-1];
				} else for (int i=Lb; i<Rb; i++) ColBuff[(i+Off)*InFeat + F] = In[PosL*W*InFeat + i*InFeat + F];
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[l*Wo*OutFeat + c*OutFeat + Line] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

// void KerPar_MM_Conv1D_HWC_SQ8(
// 	Ker_MM_Conv_SQ8_T *Arg
// 	)

// {
// 	/*
// 		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
// 	*/
// 	signed char *__restrict__ In = Arg->In;
// 	int W = Arg->W, H = Arg->H;
// 	signed char *__restrict__ Filter = Arg->Filter;
// 	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
// 	int PadL = Arg->Pad[0];
// 	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

//         int * __restrict__ Bias = Arg->Bias;
// 	int NormBias = Arg->Infos[AT_INF_BIASN];
//         signed char * __restrict__ Out = Arg->Out;
//         unsigned char * __restrict__ Scale = Arg->Scale;
//         unsigned char * __restrict__ ScaleN = Arg->ScaleN;

// 	int Wo = Arg->Wo, Ho = Arg->Ho;

// 	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
// 	unsigned int W_In1 = InFeat*Fx;
// 	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
// 	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);
//         unsigned int Iter = (Last>First)?(Last-First):0;

// 	int PosL = 0;
// 	int PosC = -PadL;
// 	for (int c=0; c<Wo/4; c++) {
// 		v4s * __restrict__ VIn1 = (v4s *) (In + (4*c  )*Sx*InFeat);
// 		v4s * __restrict__ VIn2 = (v4s *) (In + (4*c+1)*Sx*InFeat);
// 		v4s * __restrict__ VIn3 = (v4s *) (In + (4*c+2)*Sx*InFeat);
// 		v4s * __restrict__ VIn4 = (v4s *) (In + (4*c+3)*Sx*InFeat);
//                 for (int c_out=0; c_out<Iter/2; c_out++) {
//                 	int col = First + 2*c_out;
//                         v4s *VFilter1 = (v4s *) (&Filter[(col  )*W_In1]);
//                         v4s *VFilter2 = (v4s *) (&Filter[(col+1)*W_In1]);
//                         int S1 = (Bias[col  ]<<NormBias), S3=S1, S5=S1, S7=S1;
//                         int S2 = (Bias[col+1]<<NormBias), S4=S2, S6=S2, S8=S2;
//                         for (int i=0; i<(W_In1/4); i++) {
//                                 S1 = gap_sumdotp4(VIn1[i], VFilter1[i], S1); S2 = gap_sumdotp4(VIn1[i], VFilter2[i], S2);
//                                 S3 = gap_sumdotp4(VIn2[i], VFilter1[i], S3); S4 = gap_sumdotp4(VIn2[i], VFilter2[i], S4);
//                                 S5 = gap_sumdotp4(VIn3[i], VFilter1[i], S5); S6 = gap_sumdotp4(VIn3[i], VFilter2[i], S6);
//                                 S7 = gap_sumdotp4(VIn4[i], VFilter1[i], S7); S8 = gap_sumdotp4(VIn4[i], VFilter2[i], S8);
//                         }
//                         for (int i=(W_In1/4)*4; i<W_In1; i++) {
//                         	S1 += In[(4*c  )*Sx*InFeat+i]*Filter[(col)*W_In1+i]; S2 += In[(4*c  )*Sx*InFeat+i]*Filter[(col+1)*W_In1+i];
//                         	S3 += In[(4*c+1)*Sx*InFeat+i]*Filter[(col)*W_In1+i]; S4 += In[(4*c+1)*Sx*InFeat+i]*Filter[(col+1)*W_In1+i];
//                         	S5 += In[(4*c+2)*Sx*InFeat+i]*Filter[(col)*W_In1+i]; S6 += In[(4*c+2)*Sx*InFeat+i]*Filter[(col+1)*W_In1+i];
//                         	S7 += In[(4*c+3)*Sx*InFeat+i]*Filter[(col)*W_In1+i]; S8 += In[(4*c+3)*Sx*InFeat+i]*Filter[(col+1)*W_In1+i];
//                         }
//                         unsigned int Sc1 = Scale[col  ], ScN1 = ScaleN[col  ];
//                         unsigned int Sc2 = Scale[col+1], ScN2 = ScaleN[col+1];
// 			v2s R1 = gap_pack2(gap_clip(AT_SCALE(S1, Sc1, ScN1), 7), gap_clip(AT_SCALE(S2, Sc2, ScN2), 7));
// 			v2s R2 = gap_pack2(gap_clip(AT_SCALE(S3, Sc1, ScN1), 7), gap_clip(AT_SCALE(S4, Sc2, ScN2), 7));
// 			v2s R3 = gap_pack2(gap_clip(AT_SCALE(S5, Sc1, ScN1), 7), gap_clip(AT_SCALE(S6, Sc2, ScN2), 7));
// 			v2s R4 = gap_pack2(gap_clip(AT_SCALE(S7, Sc1, ScN1), 7), gap_clip(AT_SCALE(S8, Sc2, ScN2), 7));
//                         *((v2s *) (&Out[(4*c  )*OutFeat + (col)])) = R1;
//                         *((v2s *) (&Out[(4*c+1)*OutFeat + (col)])) = R2;
//                         *((v2s *) (&Out[(4*c+2)*OutFeat + (col)])) = R3;
//                         *((v2s *) (&Out[(4*c+3)*OutFeat + (col)])) = R4;
//                 }
//                 if (Iter&0x1) {
//                 	int col = Last-1;
//                         v4s *VFilter1 = (v4s *) (&Filter[(col)*W_In1]);
//                         int S1 = (Bias[col  ]<<NormBias), S3=S1, S5=S1, S7=S1;
//                         for (int i=0; i<(W_In1/4); i++) {
//                                 S1 = gap_sumdotp4(VIn1[i], VFilter1[i], S1);
//                                 S3 = gap_sumdotp4(VIn2[i], VFilter1[i], S3);
//                                 S5 = gap_sumdotp4(VIn3[i], VFilter1[i], S5);
//                                 S7 = gap_sumdotp4(VIn4[i], VFilter1[i], S7);
//                         }
//                         for (int i=(W_In1/4)*4; i<W_In1; i++) {
//                         	S1 += In[(4*c  )*Sx*InFeat+i]*Filter[(col)*W_In1+i];
//                         	S3 += In[(4*c+1)*Sx*InFeat+i]*Filter[(col)*W_In1+i];
//                         	S5 += In[(4*c+2)*Sx*InFeat+i]*Filter[(col)*W_In1+i];
//                         	S7 += In[(4*c+3)*Sx*InFeat+i]*Filter[(col)*W_In1+i];
//                         }
//                         unsigned int Sc1 = Scale[col  ], ScN1 = ScaleN[col  ];
// 			Out[(4*c  )*OutFeat + (col)] = gap_clip(AT_SCALE(S1, Sc1, ScN1), 7);
// 			Out[(4*c+1)*OutFeat + (col)] = gap_clip(AT_SCALE(S3, Sc1, ScN1), 7);
// 			Out[(4*c+2)*OutFeat + (col)] = gap_clip(AT_SCALE(S5, Sc1, ScN1), 7);
// 			Out[(4*c+3)*OutFeat + (col)] = gap_clip(AT_SCALE(S7, Sc1, ScN1), 7);

//                 }
// 		gap_waitbarrier(0);
// 	}
// 	for (int c=(Wo/4)*4; c<Wo; c++) {
// 		v4s * __restrict__ VBuff1 = (v4s *) (In + (c)*Sx*InFeat);
//                 for (int c_out=First; c_out<Last; c_out++) {
//                         v4s *VIn1 = (v4s *) (&Filter[c_out*W_In1]);
//                         int S1 = (Bias[c_out]<<NormBias);
//                         for (int i=0; i<(W_In1/4); i++) {
//                                 v4s V1 = VIn1[i];
// 				v4s C11 = VBuff1[i];
//                                 S1 = gap_sumdotp4(V1, C11, S1);
//                         }
//                         for (int i=(W_In1/4)*4; i<W_In1; i++) {
//                         	S1 += In[(c)*Sx*InFeat+i]*Filter[c_out*W_In1+i];
//                         }
//                         unsigned int Sc = Scale[c_out], ScN = ScaleN[c_out];
//                         Out[(c)*OutFeat + c_out] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
//                 }
// 		gap_waitbarrier(0);
// 	}
// }

void KerPar_MM_Conv1D_DxDy_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;
	//int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int OffBuffX = Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec));
			int OffInX = OffBuffX?(Dx*OffBuffX+PosC):0;
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			for (int f=F; f<L; f++) {
				for (int i=0; i<IterX; i++) {
					ColBuff[Fx*f + i+OffBuffX] = In[f*W*H + PosL*W+Lb + i*Dx+OffInX];
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_DxDy_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;
	// int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int OffBuffX = Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec));
			int OffInX = OffBuffX?(Dx*OffBuffX+PosC):0;
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			if (Iter>=4) {
				for (int f=0; f<(Iter/4); f++)
					for (int i=0; i<IterX; i++)
						((int *)(ColBuff+(i+OffBuffX)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[f];
				if (Iter&0x2)
					for (int i=0; i<IterX; i++)
						((short int *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter/2-1];
				if (Iter&0x1)
					for (int i=0; i<IterX; i++)
						((signed char *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter-1];
			} else if (Iter>=2) {
				if (Iter&0x2)
					for (int i=0; i<IterX; i++)
						((short int *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter/2-1];
				if (Iter&0x1)
					for (int i=0; i<IterX; i++)
						((signed char *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter-1];
			} else if (Iter>0)
				for (int i=0; i<IterX; i++)
					ColBuff[(i+OffBuffX)*InFeat + F] = In[PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat + F];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[l*Wo*OutFeat + c*OutFeat + Line] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_DxDy_ReLU_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;
	// int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int Off = -Lb - Min(PosC, 0);
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			for (int f=F; f<L; f++) {
				for (int i=0; i<IterX; i++) ColBuff[Fx*f+i+Off] = In[f*W*H+PosL*W+Lb+i*Dx];
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_ReLU_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>4) {
				if (Size&0x2) {
					if (Size&0x1) {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-3)) = *((short int *)(In+f*W*H+PosL*W+Rb-3));
							*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
						}
					} else {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-2)) = *((short int *)(In+f*W*H+PosL*W+Rb-2));
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
						*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
			} else if (Size>=2) {
				if (Size&0x4) {
					for (int f=F; f<L; f++) {
						*((int *)(ColBuff+Fx*f+Off+Lb)) = *((int *)(In+f*W*H+PosL*W+Lb));
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
						ColBuff[Fx*f+Off+Lb+2] = In[f*W*H+PosL*W+Lb+2];
					}
				} else {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
					}
				}
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_ReLUN_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>4) {
				if (Size&0x2) {
					if (Size&0x1) {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-3)) = *((short int *)(In+f*W*H+PosL*W+Rb-3));
							*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
						}
					} else {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-2)) = *((short int *)(In+f*W*H+PosL*W+Rb-2));
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
						*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
			} else if (Size>=2) {
				if (Size&0x4) {
					for (int f=F; f<L; f++) {
						*((int *)(ColBuff+Fx*f+Off+Lb)) = *((int *)(In+f*W*H+PosL*W+Lb));
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
						ColBuff[Fx*f+Off+Lb+2] = In[f*W*H+PosL*W+Lb+2];
					}
				} else {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
					}
				}
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_LeakyReLU_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>4) {
				if (Size&0x2) {
					if (Size&0x1) {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-3)) = *((short int *)(In+f*W*H+PosL*W+Rb-3));
							*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
						}
					} else {
						for (int f=F; f<L; f++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
							*((short int *)(ColBuff+Fx*f+Off+Rb-2)) = *((short int *)(In+f*W*H+PosL*W+Rb-2));
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
						*((ColBuff+Fx*f+Off+Rb-1)) = *((In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/4; i++) *((int *)(ColBuff+Fx*f+Off+4*i+Lb)) = *((int *)(In+f*W*H+PosL*W+4*i+Lb));
			} else if (Size>=2) {
				if (Size&0x4) {
					for (int f=F; f<L; f++) {
						*((int *)(ColBuff+Fx*f+Off+Lb)) = *((int *)(In+f*W*H+PosL*W+Lb));
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
						ColBuff[Fx*f+Off+Lb+2] = In[f*W*H+PosL*W+Lb+2];
					}
				} else {
					for (int f=F; f<L; f++) {
						*((short int *)(ColBuff+Fx*f+Off+Lb)) = *((short int *)(In+f*W*H+PosL*W+Lb));
					}
				}
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
				S0 = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
				int Neg0 = gap_bitextractu(S0, 1, 31), Pos0 = !Neg0;
				int S0N = AT_NORM(S0 * A0, 7);
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_SCALE(ActScale, (Neg0*S0N+Pos0*S0), ActScaleN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv2D_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	// printf("If: %3d, Of: %3d, W: %3d, H: %3d, Wo: %3d, Ho: %3d, PosL: %d\n", InFeat, OutFeat, W, H, Wo, Ho, PosL);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>4) {
				if (Size&0x2) {
					if (Size&0x1) {
						for (int f=F; f<L; f++) {
							for (int j=Tb; j<Db; j++) {
								for (int i=0; i<Size/4; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+4*i+Lb)) = *((int *)(In+f*W*H+j*W+4*i+Lb));
								*((short int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-3)) = *((short int *)(In+f*W*H+j*W+Rb-3));
								*((ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-1)) = *((In+f*W*H+j*W+Rb-1));
							}
						}
					} else {
						for (int f=F; f<L; f++) {
							for (int j=Tb; j<Db; j++) {
								for (int i=0; i<Size/4; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+4*i+Lb)) = *((int *)(In+f*W*H+j*W+4*i+Lb));
								*((short int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-2)) = *((short int *)(In+f*W*H+j*W+Rb-2));
							}
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+4*i+Lb)) = *((int *)(In+f*W*H+j*W+4*i+Lb));
							*((ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-1)) = *((In+f*W*H+j*W+Rb-1));
						}
					}
				} else {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/4; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+4*i+Lb)) = *((int *)(In+f*W*H+j*W+4*i+Lb));
						}
					}
				}
			} else if (Size>=2) {
				if (Size&0x4) {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							*((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+Lb)) = *((int *)(In+f*W*H+j*W+Lb));
						}
					}
				} else if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							*((short int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+Lb)) = *((short int *)(In+f*W*H+j*W+Lb));
							ColBuff[FS*f + Fx*(j+OffL)+OffC+Lb+2] = In[f*W*H+j*W+Lb+2];
						}
					}
				} else {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							*((short int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+Lb)) = *((short int *)(In+f*W*H+j*W+Lb));
						}
					}
				}
			} else if (Size) {
				for (int f=F; f<L; f++) {
					for (int j=Tb; j<Db; j++) {
						ColBuff[FS*f + Fx*(j+OffL)+OffC+Lb] = In[f*W*H+j*W+Lb];
					}
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
			/*
			printf("Line: %d, Col: %d, OutFeat: %d to %d\n", l, c, First, Last-1);
			printf("Feat : "); for (int i=0; i<(((W_In1+7)/8)*8); i++) printf("%2d ", ColBuff[i]); printf("\n");
			*/
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
				// printf("S0   : %d\n", S0); printf("Filt%d: ", Line); for (int i=0; i<W_In1; i++) printf("%2d ", ((signed char *)VIn1)[i]); printf("\n");
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
				// printf("Out[F:%d, H:%d, W:%d] = (%d * %d) >> %d = %d\n", Line, l, c, S0, Sc, ScN, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Max(0, Last - First);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++)
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
                        } else if (Iter>0)
				for (int j=Tb; j<Db; j++) 
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];

			PosC += Sx;
			gap_waitbarrier(0);
	                // for (int Line=First; Line<Last; Line++) {
			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S0 = (*pBias)<<NormBias; pBias++;
	                        int S1 = (*pBias)<<NormBias; pBias++;
	                        int S2 = (*pBias)<<NormBias; pBias++;
	                        int S3 = (*pBias)<<NormBias; pBias++;
				signed char *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S1 = gap_sumdotp4(V0, C1, S1);
	                                S2 = gap_sumdotp4(V0, C2, S2);
	                                S3 = gap_sumdotp4(V0, C3, S3);
					pIn+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S1, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S2, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S3, Sc, ScN), 7); pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
	                        int S0 = (*pBias)<<NormBias; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC);
	                               	S0 = gap_sumdotp4(V0, C0, S0);
					pIn+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC;
					S0 += V0*C0;
					pIn++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}


void KerPar_MM_Conv2D_ReLU_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Max(0, Last - First);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++)
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
                        } else if (Iter>0)
				for (int j=Tb; j<Db; j++) 
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
			PosC += Sx;
			gap_waitbarrier(0);
	                // for (int Line=First; Line<Last; Line++) {
			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S0 = (*pBias)<<NormBias; pBias++;
	                        int S1 = (*pBias)<<NormBias; pBias++;
	                        int S2 = (*pBias)<<NormBias; pBias++;
	                        int S3 = (*pBias)<<NormBias; pBias++;
				signed char *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S1 = gap_sumdotp4(V0, C1, S1);
	                                S2 = gap_sumdotp4(V0, C2, S2);
	                                S3 = gap_sumdotp4(V0, C3, S3);
					pIn+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7); pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
	                        int S0 = (*pBias)<<NormBias; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC);
	                               	S0 = gap_sumdotp4(V0, C0, S0);
					pIn+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC;
					S0 += V0*C0;
					pIn++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void Ker_MM_Conv2D_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
        signed char * __restrict__ ColBuff1;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];


	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(Wo), First = CoreId*ChunkCell, Last  = Min(Wo, First+ChunkCell);

	int FS = Fx*Fy;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = InFeat;
	int Iter1 = Iter*FS;
	int IterOut = OutFeat;
	int IterW = Max(0, Last-First);
	ColBuff += 2*CoreId*InFeat*FS;
	ColBuff1 = ColBuff + InFeat*FS;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL + First*Sx;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		// for (int c=0; c<Wo; c++) {
		// for (int c=First; c<Last; c++) {
		for (int c=0; c<IterW/2; c++) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff))[i]=0;
				((int *)(ColBuff1))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff))[Iter1/2-1]=0;
				((short int *)(ColBuff1))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff))[Iter1-1]=0;
				((signed char *)(ColBuff1))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Fx+Sx, W);
			int OffC = -Lb - Min(PosC, 0);
			int OffC1 = -Lb1 - Min(PosC+Sx, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
                                        	for (int i=Lb1; i<Rb1; i++)
							((int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>0) {
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
					for (int i=Lb1; i<Rb1; i++)
						ColBuff1[(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			}

			PosC += 2*Sx;
			int *pBias = Bias;
			signed char *pC = Filter;
			signed char *pOut0 = Out+l*Wo*OutFeat + (First + 2*c)*OutFeat;
			signed char *pOut1 = Out+l*Wo*OutFeat + (First + 2*c+1)*OutFeat;
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
	                        int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
	                        int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
				signed char *pIn0 = ColBuff, *pIn1 = ColBuff1;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                                S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                                S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                                S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
					pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
					pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	                }
			for (int Line=4*(IterOut/4); Line<IterOut; Line++) {
				signed char *pIn0 = ColBuff, *pIn1 = ColBuff1;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
					pIn0+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC;
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = gap_clip(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
			}
		}
		if (IterW&0x1) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>0) {
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			}
			int *pBias = Bias;
			signed char *pC = Filter;
			signed char *pOut0 = Out+l*Wo*OutFeat + (Last-1)*OutFeat;
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        int S10 = (*pBias)<<NormBias; pBias++;
	                        int S20 = (*pBias)<<NormBias; pBias++;
	                        int S30 = (*pBias)<<NormBias; pBias++;
				signed char *pIn0 = ColBuff;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S00 = gap_sumdotp4(V0, C0, S00);
	                                S10 = gap_sumdotp4(V0, C1, S10);
	                                S20 = gap_sumdotp4(V0, C2, S20);
	                                S30 = gap_sumdotp4(V0, C3, S30);
					pIn0+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
					pIn0++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = ColBuff;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
					pIn0+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, C0 = *pC;
					S00 += V0*C0;
					pIn0++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void Ker_MM_Conv2D_ReLU_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
        signed char * __restrict__ ColBuff1;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];


	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(Wo), First = CoreId*ChunkCell, Last  = Min(Wo, First+ChunkCell);

	int FS = Fx*Fy;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = InFeat;
	int Iter1 = Iter*FS;
	int IterOut = OutFeat;
	int IterW = Max(0, Last-First);
	ColBuff += 2*CoreId*InFeat*FS;
	ColBuff1 = ColBuff + InFeat*FS;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL + First*Sx;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		// for (int c=0; c<Wo; c++) {
		// for (int c=First; c<Last; c++) {
		for (int c=0; c<IterW/2; c++) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff))[i]=0;
				((int *)(ColBuff1))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff))[Iter1/2-1]=0;
				((short int *)(ColBuff1))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff))[Iter1-1]=0;
				((signed char *)(ColBuff1))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Fx+Sx, W);
			int OffC = -Lb - Min(PosC, 0);
			int OffC1 = -Lb1 - Min(PosC+Sx, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
                                        	for (int i=Lb1; i<Rb1; i++)
							((int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>0) {
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
					for (int i=Lb1; i<Rb1; i++)
						ColBuff1[(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			}

			PosC += 2*Sx;
			int *pBias = Bias;
			signed char *pC = Filter;
			signed char *pOut0 = Out+l*Wo*OutFeat + (First + 2*c)*OutFeat;
			signed char *pOut1 = Out+l*Wo*OutFeat + (First + 2*c+1)*OutFeat;
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        int S10 = (*pBias)<<NormBias, S11 = S10; pBias++;
	                        int S20 = (*pBias)<<NormBias, S21 = S20; pBias++;
	                        int S30 = (*pBias)<<NormBias, S31 = S30; pBias++;
				signed char *pIn0 = ColBuff, *pIn1 = ColBuff1;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
	                                S10 = gap_sumdotp4(V0, C1, S10); S11 = gap_sumdotp4(V1, C1, S11);
	                                S20 = gap_sumdotp4(V0, C2, S20); S21 = gap_sumdotp4(V1, C2, S21);
	                                S30 = gap_sumdotp4(V0, C3, S30); S31 = gap_sumdotp4(V1, C3, S31);
					pIn0+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
					pIn0++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S11, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S21, Sc, ScN), 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S31, Sc, ScN), 7); pOut1++;
	                }
			for (int Line=4*(IterOut/4); Line<IterOut; Line++) {
				signed char *pIn0 = ColBuff, *pIn1 = ColBuff1;
	                        int S00 = (*pBias)<<NormBias, S01 = S00; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00); S01 = gap_sumdotp4(V1, C0, S01);
					pIn0+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, V1 = *pIn1, C0 = *pC;
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				*pOut1 = AT_CLIP_POS_IMM(AT_SCALE(S01, Sc, ScN), 7); pOut1++;
			}
		}
		if (IterW&0x1) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else if (Iter>0) {
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			}
			int *pBias = Bias;
			signed char *pC = Filter;
			signed char *pOut0 = Out+l*Wo*OutFeat + (Last-1)*OutFeat;
			unsigned char *pSc = Scale;
			unsigned char *pScN = ScaleN;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        int S10 = (*pBias)<<NormBias; pBias++;
	                        int S20 = (*pBias)<<NormBias; pBias++;
	                        int S30 = (*pBias)<<NormBias; pBias++;
				signed char *pIn0 = ColBuff;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S00 = gap_sumdotp4(V0, C0, S00);
	                                S10 = gap_sumdotp4(V0, C1, S10);
	                                S20 = gap_sumdotp4(V0, C2, S20);
	                                S30 = gap_sumdotp4(V0, C3, S30);
					pIn0+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
					pIn0++; pC0++; pC1++; pC2++; pC3++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S10, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S20, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S30, Sc, ScN), 7); pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn0 = ColBuff;
	                        int S00 = (*pBias)<<NormBias; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn0), C0 = *((v4s *)pC);
	                               	S00 = gap_sumdotp4(V0, C0, S00);
					pIn0+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn0, C0 = *pC;
					S00 += V0*C0;
					pIn0++; pC++;
				}
	                        unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = AT_CLIP_POS_IMM(AT_SCALE(S00, Sc, ScN), 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*(Fx-1)+1, DFy =  Dy*(Fy-1)+1;
	// int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;
	int InvDy = ((1<<Prec)+Dy-1)/Dy;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+DFy, H);
		int OffLBuffY = Max(0, gap_mulsN(-PosL+Dy-1, InvDy, Prec));
		int OffLInY = OffLBuffY?(Dy*OffLBuffY+PosL):0;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int OffCBuffX = (Lb==0)?Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec)):0;
			int OffCInX = OffCBuffX?(Dx*OffCBuffX+PosC):0;
			int IterY = gap_mulsN(Db-Tb-1, InvDy, Prec) + 1;
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			for (int f=F; f<L; f++) {
				for (int j=0; j<IterY; j++) {
					for (int i=0; i<IterX; i++) {
						ColBuff[FS*f + Fx*(j+OffLBuffY) + i+OffCBuffX] = In[f*W*H + (Tb+j*Dy+OffLInY)*W + Lb+i*Dx+OffCInX];
					}
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Ho_F = Min(Ho, FirstDefinedOutput(Fy, Arg->Pad[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fy, Arg->Pad[2], Sy, Dy));
	int Wo_F = Min(Wo, FirstDefinedOutput(Fx, Arg->Pad[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fx, Arg->Pad[0], Sx, Dx));

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*Fx, DFy = Dy*Fy;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Max(0, Last - First);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = PosL, Db = PosL+DFy;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = PosC, Rb = PosC+DFx;
			// This part is more efficient but NOT WORKING ???? TOCHECK
			// if (Iter>=4) {
			// 	for (int f=0; f<(Iter/4); f++)
			// 		for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 			for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 				if (j>=0 && j<H && i>=0 && i<W)
			// 					((int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[f] = ((int *)In+j*W*InFeat + i*InFeat + F)[f];
			// 	if (Iter&0x2)
			// 		for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 			for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 				if (j>=0 && j<H && i>=0 && i<W)
			// 					((short int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter/2-1] = ((short int *)In+j*W*InFeat + i*InFeat + F)[Iter/2-1];
			// 	if (Iter&0x1)
			// 		for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 			for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 				if (j>=0 && j<H && i>=0 && i<W)
			// 					((signed char *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter-1] = ((signed char *)In+j*W*InFeat + i*InFeat + F)[Iter-1];
			// } else if (Iter>=2) {
			// 	if (Iter&0x2)
			// 		for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 			for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 				if (j>=0 && j<H && i>=0 && i<W)
			// 					((short int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter/2-1] = ((short int *)In+j*W*InFeat + i*InFeat + F)[Iter/2-1];
			// 	if (Iter&0x1)
			// 		for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 			for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 				if (j>=0 && j<H && i>=0 && i<W)
			// 					((signed char *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter-1] = ((signed char *)In+j*W*InFeat + i*InFeat + F)[Iter-1];				
			// } else if (Iter>0)
			// 	for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
			// 	       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			// 	       		if (j>=0 && j<H && i>=0 && i<W)
			// 				ColBuff[jj*InFeat*Fx+ii*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
			for (int f=0; f<Iter; f++)
				for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W)
							ColBuff[jj*InFeat*Fx+ii*InFeat + F+f] = In[j*W*InFeat + i*InFeat + F+f];
			PosC += Sx;
			gap_waitbarrier(0);
			// for (int Line=First; Line<Last; Line++) {
			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
			for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
				int S0 = (*pBias)<<NormBias; pBias++;
				int S1 = (*pBias)<<NormBias; pBias++;
				int S2 = (*pBias)<<NormBias; pBias++;
				int S3 = (*pBias)<<NormBias; pBias++;
				signed char *pIn = ColBuff;
				for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
					S0 = gap_sumdotp4(V0, C0, S0);
					S1 = gap_sumdotp4(V0, C1, S1);
					S2 = gap_sumdotp4(V0, C2, S2);
					S3 = gap_sumdotp4(V0, C3, S3);
					pIn+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn++; pC0++; pC1++; pC2++; pC3++;
				}
				unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S1, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S2, Sc, ScN), 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S3, Sc, ScN), 7); pOut0++;
			}
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
				int S0 = (*pBias)<<NormBias; pBias++;
				for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), C0 = *((v4s *)pC);
				       	S0 = gap_sumdotp4(V0, C0, S0);
					pIn+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, C0 = *pC;
					S0 += V0*C0;
					pIn++; pC++;
				}
				unsigned int Sc, ScN;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				*pOut0 = gap_clip(AT_SCALE(S0, Sc, ScN), 7); pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_ReLU_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*(Fx-1)+1, DFy =  Dy*(Fy-1)+1;
	// int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;
	int InvDy = ((1<<Prec)+Dy-1)/Dy;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+DFy, H);
		int OffLBuffY = Max(0, gap_mulsN(-PosL+Dy-1, InvDy, Prec));
		int OffLInY = OffLBuffY?(Dy*OffLBuffY+PosL):0;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int OffCBuffX = (Lb==0)?Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec)):0;
			int OffCInX = OffCBuffX?(Dx*OffCBuffX+PosC):0;
			int IterY = gap_mulsN(Db-Tb-1, InvDy, Prec) + 1;
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			for (int f=F; f<L; f++) {
				for (int j=0; j<IterY; j++) {
					for (int i=0; i<IterX; i++) ColBuff[FS*f + Fx*(j+OffLBuffY) + i+OffCBuffX] = In[f*W*H + (Tb+j*Dy+OffLInY)*W + Lb+i*Dx+OffCInX];
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*W_In1 + 0]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<((W_In1+7)/8); i++) {
	                                v4s V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[Line*Wo*Ho + l*Wo + c] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_ConvDW1D_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate ((((InFeat/NCores)+3)/4)*4)*8 elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	v4s M0 = (v4s){-1,0,0,0}, M1 = (v4s){0,-1,0,0}, M2 = (v4s){0,0,-1,0}, M3 = (v4s){0,0,0,-1};

	int PosL = 0;
	int Iter = L-F;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=F*Fx; i<(L*Fx); i++) ColBuff[i] = 0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			if (Iter) {
				if (Iter>=4) {
					for (int f=0; f<(Iter/4); f++)
						for (int i=Lb; i<Rb; i++) ((int *)(ColBuff+(i+Off)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + i*InFeat+F))[f];
					if (Iter&0x2) for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1) for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + i*InFeat+F))[Iter-1];
				} else if (Iter>=2) {
					if (Iter&0x2) for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1) for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + i*InFeat+F))[Iter-1];
				} else if (Iter>0)
					for (int i=Lb; i<Rb; i++) ColBuff[(i+Off)*InFeat + F] = In[PosL*W*InFeat + i*InFeat + F];
			}
			PosC += Sx;
			gap_waitbarrier(0);
			v4s *VIn   = (v4s *)(ColBuff + F);
			/* Filter: Fx x InFeat -> fx*Infeat + f */
			for (int f=0; f<(Iter/4); f++) {
				v4s B = ((v4s *)(Bias + F))[f];
				int S0 = B[0], S1 = B[1], S2 = B[2], S3 = B[3];
				for (int i=0; i<Fx; i++) {
					v4s Coeff = ((v4s *)(Filter + i*InFeat + F))[f];
					v4s In    = ((v4s *)(ColBuff + i*InFeat + F))[f];
	                                S0 = gap_sumdotp4(In, Coeff & M0, S0);
	                                S1 = gap_sumdotp4(In, Coeff & M1, S1);
	                                S2 = gap_sumdotp4(In, Coeff & M2, S2);
	                                S3 = gap_sumdotp4(In, Coeff & M3, S3);
				}
	                        v4s Sc = ((v4s *) Scale+F)[f], ScN = ((v4s *) ScaleN+F)[f];
				S0 = gap_clip(AT_SCALE(S0, Sc[0], ScN[0]), 7);
				S1 = gap_clip(AT_SCALE(S1, Sc[1], ScN[1]), 7);
				S2 = gap_clip(AT_SCALE(S2, Sc[2], ScN[2]), 7);
				S3 = gap_clip(AT_SCALE(S3, Sc[3], ScN[3]), 7);
	                        ((v4s *) Out+l*Wo*OutFeat + c*OutFeat + F)[f] = gap_pack4(S0, S1, S2, S3);
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_ConvDW2D_HWC_SQ8(
	Ker_MM_Conv_SQ8_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	signed char *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	signed char *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        int * __restrict__ Bias = Arg->Bias;
	int NormBias = Arg->Infos[AT_INF_BIASN];
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        signed char * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;
	int A0 = Arg->Infos[AT_INF_A0];

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	//unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int PosL = Arg->FirstTile?(-PadT):0;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=F; i<L; i++) ColBuff[i] = 0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        int Iter = L-F;
                        // Transpose the input from HWxC -> CxHW: InFeatxFyxFx
                        for (int c=F; c<L; c++)
				for (int j=Tb; j<Db; j++) 
					for (int i=Lb; i<Rb; i++)
						ColBuff[c*FS + (j+OffL)*Fy + (i+OffC)] = In[j*W*InFeat + i*InFeat + c];

			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=F; Line<L; Line++) {
	                        v4s *VIn1 = (v4s *) (&Filter[Line*FS]);
	                        v4s *VBuff = (v4s *) (&ColBuff[Line*FS]);
	                        int S0 = (Bias[Line]<<NormBias);
	                        for (int i=0; i<(FS/8); i++){
	                                v4s V0 = VIn1[2*i] , V1 = VIn1[2*i+1];
					v4s C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 = gap_sumdotp4(V0, C0, S0);
	                                S0 = gap_sumdotp4(V1, C1, S0);
	                        }
	                        for (int i=(FS/8)*8; i<FS; i++) S0 += ColBuff[i]*Filter[Line*FS+i];
	                        unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
	                        Out[l*OutFeat*Wo+c*OutFeat+Line] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}
#pragma GCC diagnostic pop
