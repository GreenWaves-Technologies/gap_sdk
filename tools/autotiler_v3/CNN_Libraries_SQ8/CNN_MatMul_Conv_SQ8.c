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

#include <stdio.h>
#include "CNN_BasicKernels_SQ8.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wpointer-sign"

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

/*
 * 1D Convolutional Kernels kernels based on MatMul (im2col: ColBuff) with CHW inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv1D_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = Min(CoreId*ChunkCell, OutFeat), Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int Iter = (L-F)*Fx;
	int IterOut = Last-First;
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
	                        S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(S0, 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv1D_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv1D_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv1D_ReLUM_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv1D_ReLUMN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv1D_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv1D_HSigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv1D_HSwish_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv1D_Sigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv1D_Tanh_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_SQ8_act(Arg, ACT_TANH);
}

/*
 * 1x1 Convolutional Kernels kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Parallelized on the output feature dimension
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv1x1_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S11 = AT_SCALE(S11, Sc, ScN); ACT_SWITCH(S11, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				*pOut1 = gap_clip(S11, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S21 = AT_SCALE(S21, Sc, ScN); ACT_SWITCH(S21, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				*pOut1 = gap_clip(S21, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S31 = AT_SCALE(S31, Sc, ScN); ACT_SWITCH(S31, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
				*pOut1 = gap_clip(S31, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv1x1_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv1x1_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv1x1_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv1x1_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv1x1_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv1x1_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv1x1_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv1x1_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv1x1_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv1x1_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_TANH);
}


/*
 * 1x1 Convolutional Kernels kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Parallelized on the spatial height dimension
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) Ker_MM_Conv1x1_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S11 = AT_SCALE(S11, Sc, ScN); ACT_SWITCH(S11, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				*pOut1 = gap_clip(S11, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S21 = AT_SCALE(S21, Sc, ScN); ACT_SWITCH(S21, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				*pOut1 = gap_clip(S21, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S31 = AT_SCALE(S31, Sc, ScN); ACT_SWITCH(S31, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
				*pOut1 = gap_clip(S31, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
			}
		}
	}
	gap_waitbarrier(0);
}

void Ker_MM_Conv1x1_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_NONE);
}

void Ker_MM_Conv1x1_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELU);
}

void Ker_MM_Conv1x1_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUN);
}

void Ker_MM_Conv1x1_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUM);
}

void Ker_MM_Conv1x1_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void Ker_MM_Conv1x1_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void Ker_MM_Conv1x1_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void Ker_MM_Conv1x1_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void Ker_MM_Conv1x1_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void Ker_MM_Conv1x1_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv1x1_HWC_SQ8_act(Arg, ACT_TANH);
}

/*
 * 1D Convolutional Kernels kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv1D_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	signed char * __restrict__ ColBuff1 = ColBuff + 4*Tail;
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	((int *)ColBuff1)[Tail-1] = 0; ((int *)ColBuff1)[Tail-2] = 0;
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	int IterOut = Max(0, Last - First);
	// for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<(Wo/2); c++) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff+F*Fx))[i]=0;
				((int *)(ColBuff1+F*Fx))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
				((short int *)(ColBuff1+F*Fx))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
				((signed char *)(ColBuff1+F*Fx))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W), Off = -Lb - Min(PosC, 0);
			int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Fx+Sx, W), Off1 = -Lb1 - Min(PosC+Sx, 0);
			if (Iter>0) {
				if (Iter>=4) {
					for (int f=0; f<(Iter/4); f++) {
						for (int i=Lb; i<Rb; i++) ((int *)(ColBuff+(i+Off)*InFeat+F))[f] = ((int *)(In+ i*InFeat+F))[f];
						for (int i=Lb1; i<Rb1; i++) ((int *)(ColBuff1+(i+Off1)*InFeat+F))[f] = ((int *)(In+ i*InFeat+F))[f];
					}
					if (Iter&0x2) {
						for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+ i*InFeat+F))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++) ((short int *)(ColBuff1+(i+Off1)*InFeat+F))[Iter/2-1] = ((short int *)(In+ i*InFeat+F))[Iter/2-1];
					}
					if (Iter&0x1) {
						for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+ i*InFeat+F))[Iter-1];
						for (int i=Lb1; i<Rb1; i++) ((signed char *)(ColBuff1+(i+Off1)*InFeat+F))[Iter-1] = ((signed char *)(In+ i*InFeat+F))[Iter-1];
					}
				} else if (Iter>=2) {
					if (Iter&0x2) {
						for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter/2-1] = ((short int *)(In+ i*InFeat+F))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++) ((short int *)(ColBuff1+(i+Off1)*InFeat+F))[Iter/2-1] = ((short int *)(In+ i*InFeat+F))[Iter/2-1];
					}
					if (Iter&0x1) {
						for (int i=Lb; i<Rb; i++) ((signed char *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((signed char *)(In+ i*InFeat+F))[Iter-1];
						for (int i=Lb1; i<Rb1; i++) ((signed char *)(ColBuff1+(i+Off1)*InFeat+F))[Iter-1] = ((signed char *)(In+ i*InFeat+F))[Iter-1];
					}
				} else {
					for (int i=Lb; i<Rb; i++) ColBuff[(i+Off)*InFeat + F] = In[ i*InFeat + F];
					for (int i=Lb1; i<Rb1; i++) ColBuff1[(i+Off1)*InFeat + F] = In[ i*InFeat + F];
				}
			}
			PosC += 2*Sx;
			gap_waitbarrier(0);

			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out + (2*c+0)*OutFeat+First;
			signed char *pOut1 = Out + (2*c+1)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S0 = (*pBias)<<NormBias, S4=S0; pBias++;
	                        int S1 = (*pBias)<<NormBias, S5=S1; pBias++;
	                        int S2 = (*pBias)<<NormBias, S6=S2; pBias++;
	                        int S3 = (*pBias)<<NormBias, S7=S3; pBias++;
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4); 
	                                S1 = gap_sumdotp4(V0, C1, S1); S5 = gap_sumdotp4(V1, C1, S5); 
	                                S2 = gap_sumdotp4(V0, C2, S2); S6 = gap_sumdotp4(V1, C2, S6); 
	                                S3 = gap_sumdotp4(V0, C3, S3); S7 = gap_sumdotp4(V1, C3, S7); 
					pIn+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0; S4 += V1*C0;
					S1 += V0*C1; S5 += V1*C1;
					S2 += V0*C2; S6 += V1*C2;
					S3 += V0*C3; S7 += V1*C3;
					pIn++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
				S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S5 = AT_SCALE(S5, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S5, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S6 = AT_SCALE(S6, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S6, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S7 = AT_SCALE(S7, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S7, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
				v4s R2 = gap_pack4(gap_clip(S4, 7), gap_clip(S5, 7), gap_clip(S6, 7), gap_clip(S7, 7));
				*((v4s *) (pOut0+4*Line)) = R1;
				*((v4s *) (pOut1+4*Line)) = R2;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        int S0 = (*pBias)<<NormBias, S4=S0; pBias++;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4);
					pIn+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC;
					S0 += V0*C0; S4 += V1*C0;
					pIn++; pIn1++; pC++;
				}
				S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[i], pScN[i]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*(pOut0+i) = gap_clip(S0, 7);
				*(pOut1+i) = gap_clip(S4, 7);
			}
			gap_waitbarrier(0);
		}
		if (Wo&0x1) {
			int c = Wo-1;
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

			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out + (c)*OutFeat+First;
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
				S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
				*((v4s *) (pOut0+4*Line)) = R1;
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
				S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*(pOut0+i) = gap_clip(S0, 7);
			}
			gap_waitbarrier(0);
		}
	// 	PosL += Sy;
	// }
}

void KerPar_MM_Conv1D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv1D_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv1D_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv1D_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv1D_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv1D_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv1D_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv1D_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv1D_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv1D_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_HWC_SQ8_act(Arg, ACT_TANH);
}


/*
 * 1D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with CHW inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv1D_DxDy_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

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
	                        S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(S0, 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_DxDy_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv1D_DxDy_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv1D_DxDy_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv1D_DxDy_ReLUM_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv1D_DxDy_ReLUMN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv1D_DxDy_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv1D_DxDy_HSwish_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv1D_DxDy_HSigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv1D_DxDy_Sigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv1D_DxDy_Tanh_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_SQ8_act(Arg, ACT_TANH);
}


/*
 * 1D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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

	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+7)/8);
	signed char * __restrict__ ColBuff1 = ColBuff + 4*Tail;
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	((int *)ColBuff1)[Tail-1] = 0; ((int *)ColBuff1)[Tail-2] = 0;

	int IterOut = Max(0, Last - First);
	
	int DFx = Dx*(Fx-1)+1;
	// int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	int PosC = -PadL;
	for (int c=0; c<(Wo/2); c++) {
		for (int i=0; i<(Iter1/4); i++) {
			((int *)(ColBuff+F*Fx))[i]=0;
			((int *)(ColBuff1+F*Fx))[i]=0;
		}
		if (Iter1&0x2) {
			((short int *)(ColBuff+F*Fx))[Iter1/2-1]=0;
			((short int *)(ColBuff1+F*Fx))[Iter1/2-1]=0;
		}
		if (Iter1&0x1) {
			((signed char *)(ColBuff+F*Fx))[Iter1-1]=0;
			((signed char *)(ColBuff1+F*Fx))[Iter1-1]=0;
		}
		int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
		int OffBuffX = Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec));
		int OffInX = OffBuffX?(Dx*OffBuffX+PosC):0;
		int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;

		int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Sx+DFx, W);
		int OffBuffX1 = Max(0, gap_mulsN(-PosC-Sx+Dx-1, InvDx, Prec));
		int OffInX1 = OffBuffX1?(Dx*OffBuffX1+PosC+Sx):0;
		int IterX1 = gap_mulsN(Rb1-Lb1-1, InvDx, Prec) + 1;

		if (Iter>=4) {
			for (int f=0; f<(Iter/4); f++) {
				for (int i=0; i<IterX; i++) 
					((int *)(ColBuff+(i+OffBuffX)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[f];
				for (int i=0; i<IterX1; i++) 
					((int *)(ColBuff1+(i+OffBuffX1)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat+F))[f];
			}
			if (Iter&0x2) {
				for (int i=0; i<IterX; i++)
					((short int *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter/2-1];
				for (int i=0; i<IterX1; i++)
					((short int *)(ColBuff1+(i+OffBuffX1)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat+F))[Iter/2-1];
			}
			if (Iter&0x1) {
				for (int i=0; i<IterX; i++)
					((signed char *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter-1];
				for (int i=0; i<IterX1; i++)
					((signed char *)(ColBuff1+(i+OffBuffX1)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat+F))[Iter-1];
			}
		} else if (Iter>=2) {
			if (Iter&0x2) {
				for (int i=0; i<IterX; i++)
					((short int *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter/2-1];
				for (int i=0; i<IterX1; i++)
					((short int *)(ColBuff1+(i+OffBuffX1)*InFeat+F))[Iter/2-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat+F))[Iter/2-1];
			}
			if (Iter&0x1){
				for (int i=0; i<IterX; i++)
					((signed char *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter-1];
				for (int i=0; i<IterX1; i++)
					((signed char *)(ColBuff1+(i+OffBuffX1)*InFeat+F))[Iter-1] = ((signed char *)(In+PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat+F))[Iter-1];
			}
		} else if (Iter>0) {
			for (int i=0; i<IterX; i++)
				ColBuff[(i+OffBuffX)*InFeat + F] = In[PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat + F];
			for (int i=0; i<IterX1; i++)
				ColBuff1[(i+OffBuffX1)*InFeat + F] = In[PosL*W*InFeat + (i*Dx+OffInX1+Lb1)*InFeat + F];
		}
		PosC += 2*Sx;
		gap_waitbarrier(0);


		int *pBias = Bias + First;
		signed char *pC = Filter + W_In1*First;
		signed char *pOut0 = Out + (2*c+0)*OutFeat+First;
		signed char *pOut1 = Out + (2*c+1)*OutFeat+First;
		unsigned char *pSc = Scale + First;
		unsigned char *pScN = ScaleN + First;
                for (int Line=0; Line<IterOut/4; Line++) {
			signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
			pC=pC3+W_In1;
                        int S0 = (*pBias)<<NormBias, S4=S0; pBias++;
                        int S1 = (*pBias)<<NormBias, S5=S1; pBias++;
                        int S2 = (*pBias)<<NormBias, S6=S2; pBias++;
                        int S3 = (*pBias)<<NormBias, S7=S3; pBias++;
			signed char *pIn = ColBuff;
			signed char *pIn1 = ColBuff1;
                        for (int i=0; i<(W_In1/4); i++) {
				v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
                                S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4); 
                                S1 = gap_sumdotp4(V0, C1, S1); S5 = gap_sumdotp4(V1, C1, S5); 
                                S2 = gap_sumdotp4(V0, C2, S2); S6 = gap_sumdotp4(V1, C2, S6); 
                                S3 = gap_sumdotp4(V0, C3, S3); S7 = gap_sumdotp4(V1, C3, S7); 
				pIn+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
                        }
			for (int f=4*(W_In1/4); f<W_In1; f++) {
				int V0 = *pIn, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
				S0 += V0*C0; S4 += V1*C0;
				S1 += V0*C1; S5 += V1*C1;
				S2 += V0*C2; S6 += V1*C2;
				S3 += V0*C3; S7 += V1*C3;
				pIn++; pIn1++; pC0++; pC1++; pC2++; pC3++;
			}
			S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S4 = AT_SCALE(S4, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S5 = AT_SCALE(S5, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S5, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S6 = AT_SCALE(S6, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S6, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S7 = AT_SCALE(S7, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S7, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
			v4s R2 = gap_pack4(gap_clip(S4, 7), gap_clip(S5, 7), gap_clip(S6, 7), gap_clip(S7, 7));
			*((v4s *) (pOut0+4*Line)) = R1;
			*((v4s *) (pOut1+4*Line)) = R2;
                }
		for (int i=4*(IterOut/4); i<IterOut; i++) {
			signed char *pIn = ColBuff;
			signed char *pIn1 = ColBuff1;
                        int S0 = (*pBias)<<NormBias, S4=S0; pBias++;
                        for (int i=0; i<(W_In1/4); i++) {
				v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
                               	S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4);
				pIn+=4; pIn1+=4; pC+=4;
			}
			for (int f=4*(W_In1/4); f<W_In1; f++) {
				int V0 = *pIn, V1 = *pIn1, C0 = *pC;
				S0 += V0*C0; S4 += V1*C0;
				pIn++; pIn1++; pC++;
			}
			S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S4 = AT_SCALE(S4, pSc[i], pScN[i]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			*(pOut0+i) = gap_clip(S0, 7);
			*(pOut1+i) = gap_clip(S4, 7);
		}
		gap_waitbarrier(0);
	}
	if (Wo&0x1) {
		int c = Wo-1;
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

		int *pBias = Bias + First;
		signed char *pC = Filter + W_In1*First;
		signed char *pOut0 = Out + (c)*OutFeat+First;
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
			S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
			*((v4s *) (pOut0+4*Line)) = R1;
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
			S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
			*(pOut0+i) = gap_clip(S0, 7);
		}
		gap_waitbarrier(0);
	}
}

void KerPar_MM_Conv1D_DxDy_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv1D_DxDy_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv1D_DxDy_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv1D_DxDy_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv1D_DxDy_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv1D_DxDy_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv1D_DxDy_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv1D_DxDy_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv1D_DxDy_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv1D_DxDy_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv1D_DxDy_HWC_SQ8_act(Arg, ACT_TANH);
}

/*
 * 2D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with CHW inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv2D_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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

	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

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
				S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(S0, 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv2D_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv2D_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv2D_ReLUM_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv2D_ReLUMN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv2D_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv2D_HSwish_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv2D_HSigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv2D_Sigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv2D_Tanh_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_SQ8_act(Arg, ACT_TANH);
}

/*
 * 2D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv2D_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation)

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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);

	signed char * __restrict__ ColBuff1 = ColBuff + 4*Tail;
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	((int *)ColBuff1)[Tail-1] = 0; ((int *)ColBuff1)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Max(0, Last - First);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<(Wo/2); c++) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff+F*FS))[i]=0;
				((int *)(ColBuff1+F*FS))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
				((short int *)(ColBuff1+F*FS))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
				((signed char *)(ColBuff1+F*FS))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Sx+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
			int OffC1 = -Lb1 - Min(PosC+Sx, 0);
                        if (Iter>=4) {
                                for (int f=0; f<(Iter/4); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++) {
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
                                        	}
                                        	for (int i=Lb1; i<Rb1; i++) {
							((int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
                                        	}
					}
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
					}
                        } else if (Iter>=2) {
                                if (Iter&0x2)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((signed char *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
					}
                        } else if (Iter>0) {
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
					for (int i=Lb1; i<Rb1; i++)
						ColBuff1[(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
				}
			}
			PosC += 2*Sx;
			gap_waitbarrier(0);

			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat+First;
			signed char *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S0 = (pBias[4*Line  ])<<NormBias, S4=S0;
	                        int S1 = (pBias[4*Line+1])<<NormBias, S5=S1;
	                        int S2 = (pBias[4*Line+2])<<NormBias, S6=S2;
	                        int S3 = (pBias[4*Line+3])<<NormBias, S7=S3;
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4); 
	                                S1 = gap_sumdotp4(V0, C1, S1); S5 = gap_sumdotp4(V1, C1, S5); 
	                                S2 = gap_sumdotp4(V0, C2, S2); S6 = gap_sumdotp4(V1, C2, S6); 
	                                S3 = gap_sumdotp4(V0, C3, S3); S7 = gap_sumdotp4(V1, C3, S7); 
					pIn+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0; S4 += V1*C0;
					S1 += V0*C1; S5 += V1*C1;
					S2 += V0*C2; S6 += V1*C2;
					S3 += V0*C3; S7 += V1*C3;
					pIn++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
				S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S5 = AT_SCALE(S5, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S5, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S6 = AT_SCALE(S6, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S6, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S7 = AT_SCALE(S7, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S7, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
				v4s R2 = gap_pack4(gap_clip(S4, 7), gap_clip(S5, 7), gap_clip(S6, 7), gap_clip(S7, 7));
				*((v4s *) (pOut0+4*Line)) = R1;
				*((v4s *) (pOut1+4*Line)) = R2;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        int S0 = (pBias[i])<<NormBias, S4=S0;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4);
					pIn+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC;
					S0 += V0*C0; S4 += V1*C0;
					pIn++; pIn1++; pC++;
				}
				S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[i], pScN[i]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*(pOut0+i) = gap_clip(S0, 7);
				*(pOut1+i) = gap_clip(S4, 7);
			}
			gap_waitbarrier(0);
		}
		if (Wo&0x1) {
			int c = Wo-1;
		// for (int c=0; c<Wo; c++) {
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

			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (c)*OutFeat+First;
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
				S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
				*((v4s *) (pOut0+4*Line)) = R1;
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
				S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*(pOut0+i) = gap_clip(S0, 7);
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv2D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv2D_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv2D_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv2D_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv2D_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv2D_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv2D_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv2D_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv2D_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv2D_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_HWC_SQ8_act(Arg, ACT_TANH);
}

/*
 * 2D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Parallelization on the output Width spatial dimension -> Each Core has 2 im2col buffer
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) Ker_MM_Conv2D_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);


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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S11 = AT_SCALE(S11, Sc, ScN); ACT_SWITCH(S11, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				*pOut1 = gap_clip(S11, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S21 = AT_SCALE(S21, Sc, ScN); ACT_SWITCH(S21, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				*pOut1 = gap_clip(S21, 7); pOut1++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S31 = AT_SCALE(S31, Sc, ScN); ACT_SWITCH(S31, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
				*pOut1 = gap_clip(S31, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S01 = AT_SCALE(S01, Sc, ScN); ACT_SWITCH(S01, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				*pOut1 = gap_clip(S01, 7); pOut1++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S10 = AT_SCALE(S10, Sc, ScN); ACT_SWITCH(S10, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S10, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S20 = AT_SCALE(S20, Sc, ScN); ACT_SWITCH(S20, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S20, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S30 = AT_SCALE(S30, Sc, ScN); ACT_SWITCH(S30, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S30, 7); pOut0++;
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
				S00 = AT_SCALE(S00, Sc, ScN); ACT_SWITCH(S00, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S00, 7); pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void Ker_MM_Conv2D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_NONE);
}

void Ker_MM_Conv2D_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELU);
}

void Ker_MM_Conv2D_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUN);
}

void Ker_MM_Conv2D_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUM);
}

void Ker_MM_Conv2D_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void Ker_MM_Conv2D_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void Ker_MM_Conv2D_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void Ker_MM_Conv2D_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void Ker_MM_Conv2D_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void Ker_MM_Conv2D_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	Ker_MM_Conv2D_HWC_SQ8_act(Arg, ACT_TANH);
}

/*
 * 2D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with CHW inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv2D_DxDy_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

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
	                        S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
	                        Out[Line*Wo*Ho + l*Wo + c] = gap_clip(S0, 7);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv2D_DxDy_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv2D_DxDy_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv2D_DxDy_ReLUM_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv2D_DxDy_ReLUMN_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv2D_DxDy_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv2D_DxDy_HSwish_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv2D_DxDy_HSigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv2D_DxDy_Sigmoid_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv2D_DxDy_Tanh_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_SQ8_act(Arg, ACT_TANH);
}


/*
 * 2D Convolutional Kernels with Dilation kernels based on MatMul (im2col: ColBuff) with HWC inout tensor order
 * Optional Activation fused applied to the 32bits accumulator -> ACT_SWITCH defined in CNN_BasicKernels_SQ8.h
 */
static inline void __attribute__((always_inline)) KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(
	Ker_MM_Conv_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
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
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	v4s * __restrict__ VBuff = (v4s *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Ho_F = Min(Ho, FirstDefinedOutput(Fy, Arg->Pad[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fy, Arg->Pad[2], Sy, Dy));
	int Wo_F = Min(Wo, FirstDefinedOutput(Fx, Arg->Pad[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fx, Arg->Pad[0], Sx, Dx));

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+7)/8);
	signed char * __restrict__ ColBuff1 = ColBuff + 4*Tail;
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	((int *)ColBuff1)[Tail-1] = 0; ((int *)ColBuff1)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*Fx, DFy = Dy*Fy;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Max(0, Last - First);
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = PosL, Db = PosL+DFy;
		for (int c=0; c<(Wo/2); c++) {
			for (int i=0; i<(Iter1/4); i++) {
				((int *)(ColBuff+F*FS))[i]=0;
				((int *)(ColBuff1+F*FS))[i]=0;
			}
			if (Iter1&0x2) {
				((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
				((short int *)(ColBuff1+F*FS))[Iter1/2-1]=0;
			}
			if (Iter1&0x1) {
				((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
				((signed char *)(ColBuff1+F*FS))[Iter1-1]=0;
			}
			int Lb = PosC, Rb = PosC+DFx;
			int Lb1 = PosC+Sx, Rb1 = PosC+Sx+DFx;
/*
This part is more efficient but NOT WORKING ???? TOCHECK
			if (Iter>=4) {
				for (int f=0; f<(Iter/4); f++)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
						for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
							if (j>=0 && j<H && i>=0 && i<W)
								((int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[f] = ((int *)In+j*W*InFeat + i*InFeat + F)[f];
				if (Iter&0x2)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
						for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
							if (j>=0 && j<H && i>=0 && i<W)
								((short int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter/2-1] = ((short int *)In+j*W*InFeat + i*InFeat + F)[Iter/2-1];
				if (Iter&0x1)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
						for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
							if (j>=0 && j<H && i>=0 && i<W)
								((signed char *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter-1] = ((signed char *)In+j*W*InFeat + i*InFeat + F)[Iter-1];
			} else if (Iter>=2) {
				if (Iter&0x2)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
						for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
							if (j>=0 && j<H && i>=0 && i<W)
								((short int *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter/2-1] = ((short int *)In+j*W*InFeat + i*InFeat + F)[Iter/2-1];
				if (Iter&0x1)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
						for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
							if (j>=0 && j<H && i>=0 && i<W)
								((signed char *)ColBuff+jj*InFeat*Fx+ii*InFeat + F)[Iter-1] = ((signed char *)In+j*W*InFeat + i*InFeat + F)[Iter-1];				
			} else if (Iter>0)
				for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W)
							ColBuff[jj*InFeat*Fx+ii*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
*/
			for (int f=0; f<Iter; f++)
				for (int j=Tb, jj=0; j<Db; j+=Dy, jj++) {
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W) 
							ColBuff[jj*InFeat*Fx+ii*InFeat + F+f] = In[j*W*InFeat + i*InFeat + F+f];
				       	for (int i=Lb1, ii=0; i<Rb1; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W) 
							ColBuff1[jj*InFeat*Fx+ii*InFeat + F+f] = In[j*W*InFeat + i*InFeat + F+f];
				}
			PosC += 2*Sx;
			gap_waitbarrier(0);

			int *pBias = Bias + First;
			signed char *pC = Filter + W_In1*First;
			signed char *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat+First;
			signed char *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat+First;
			unsigned char *pSc = Scale + First;
			unsigned char *pScN = ScaleN + First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				signed char *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        int S0 = (pBias[4*Line  ])<<NormBias, S4=S0;
	                        int S1 = (pBias[4*Line+1])<<NormBias, S5=S1;
	                        int S2 = (pBias[4*Line+2])<<NormBias, S6=S2;
	                        int S3 = (pBias[4*Line+3])<<NormBias, S7=S3;
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC0), C1 = *((v4s *)pC1), C2 = *((v4s *)pC2), C3 = *((v4s *)pC3);
	                                S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4); 
	                                S1 = gap_sumdotp4(V0, C1, S1); S5 = gap_sumdotp4(V1, C1, S5); 
	                                S2 = gap_sumdotp4(V0, C2, S2); S6 = gap_sumdotp4(V1, C2, S6); 
	                                S3 = gap_sumdotp4(V0, C3, S3); S7 = gap_sumdotp4(V1, C3, S7); 
					pIn+=4; pIn1+=4; pC0+=4; pC1+=4; pC2+=4; pC3+=4;
	                        }
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC0, C1 = *pC1, C2 = *pC2, C3 = *pC3;
					S0 += V0*C0; S4 += V1*C0;
					S1 += V0*C1; S5 += V1*C1;
					S2 += V0*C2; S6 += V1*C2;
					S3 += V0*C3; S7 += V1*C3;
					pIn++; pIn1++; pC0++; pC1++; pC2++; pC3++;
				}
				S0 = AT_SCALE(S0, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S1 = AT_SCALE(S1, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S2 = AT_SCALE(S2, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S3 = AT_SCALE(S3, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[4*Line  ], pScN[4*Line  ]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S5 = AT_SCALE(S5, pSc[4*Line+1], pScN[4*Line+1]); ACT_SWITCH(S5, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S6 = AT_SCALE(S6, pSc[4*Line+2], pScN[4*Line+2]); ACT_SWITCH(S6, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S7 = AT_SCALE(S7, pSc[4*Line+3], pScN[4*Line+3]); ACT_SWITCH(S7, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				v4s R1 = gap_pack4(gap_clip(S0, 7), gap_clip(S1, 7), gap_clip(S2, 7), gap_clip(S3, 7));
				v4s R2 = gap_pack4(gap_clip(S4, 7), gap_clip(S5, 7), gap_clip(S6, 7), gap_clip(S7, 7));
				*((v4s *) (pOut0+4*Line)) = R1;
				*((v4s *) (pOut1+4*Line)) = R2;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				signed char *pIn = ColBuff;
				signed char *pIn1 = ColBuff1;
	                        int S0 = (pBias[i])<<NormBias, S4=S0;
	                        for (int i=0; i<(W_In1/4); i++) {
					v4s V0 = *((v4s *)pIn), V1 = *((v4s *)pIn1), C0 = *((v4s *)pC);
	                               	S0 = gap_sumdotp4(V0, C0, S0); S4 = gap_sumdotp4(V1, C0, S4);
					pIn+=4; pIn1+=4; pC+=4;
				}
				for (int f=4*(W_In1/4); f<W_In1; f++) {
					int V0 = *pIn, V1 = *pIn1, C0 = *pC;
					S0 += V0*C0; S4 += V1*C0;
					pIn++; pIn1++; pC++;
				}
				S0 = AT_SCALE(S0, pSc[i], pScN[i]); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				S4 = AT_SCALE(S4, pSc[i], pScN[i]); ACT_SWITCH(S4, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*(pOut0+i) = gap_clip(S0, 7);
				*(pOut1+i) = gap_clip(S4, 7);
			}
			gap_waitbarrier(0);
		}
		if (Wo&0x1) {
			int c = Wo-1;
			for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
			if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = PosC, Rb = PosC+DFx;

			for (int f=0; f<Iter; f++)
				for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W)
							ColBuff[jj*InFeat*Fx+ii*InFeat + F+f] = In[j*W*InFeat + i*InFeat + F+f];
			PosC += Sx;
			gap_waitbarrier(0);

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
				S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S0, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S1 = AT_SCALE(S1, Sc, ScN); ACT_SWITCH(S1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S1, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S2 = AT_SCALE(S2, Sc, ScN); ACT_SWITCH(S2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S2, 7); pOut0++;
				Sc = *pSc; ScN = *pScN; pSc++; pScN++;
				S3 = AT_SCALE(S3, Sc, ScN); ACT_SWITCH(S3, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S3, 7); pOut0++;
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
				S0 = AT_SCALE(S0, Sc, ScN); ACT_SWITCH(S0, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
				*pOut0 = gap_clip(S0, 7); pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_NONE);
}

void KerPar_MM_Conv2D_DxDy_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_RELU);
}

void KerPar_MM_Conv2D_DxDy_ReLUN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_RELUN);
}

void KerPar_MM_Conv2D_DxDy_ReLUM_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_RELUM);
}

void KerPar_MM_Conv2D_DxDy_ReLUMN_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_RELUMN);
}

void KerPar_MM_Conv2D_DxDy_LeakyReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerPar_MM_Conv2D_DxDy_HSwish_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_HSWISH);
}

void KerPar_MM_Conv2D_DxDy_HSigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerPar_MM_Conv2D_DxDy_Sigmoid_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_SIGMOID);
}

void KerPar_MM_Conv2D_DxDy_Tanh_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg) {
	KerPar_MM_Conv2D_DxDy_HWC_SQ8_act(Arg, ACT_TANH);
}

#pragma GCC diagnostic pop
