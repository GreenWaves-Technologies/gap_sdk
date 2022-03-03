
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include <stdio.h>
#include "CNN_BasicKernels_fp16.h"

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

void KerPar_MM_Conv1D_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int Iter = (L-F)*Fx;
	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter&0x1) ((short int *)(ColBuff+F*Fx))[Iter-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>2) {
				if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/2; i++) *((int *)(ColBuff+Fx*f+Off+2*i+Lb)) = *((int *)(In+f*W*H+PosL*W+2*i+Lb));
						*((short int *)(ColBuff+Fx*f+Off+Rb-1)) = *((short int *)(In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/2; i++) *((int *)(ColBuff+Fx*f+Off+2*i+Lb)) = *((int *)(In+f*W*H+PosL*W+2*i+Lb));
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1=(F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = S0[0]+S0[1]+S1[0]+S1[1];
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1x1_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	unsigned int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);
	int IterOut = Last - First;

	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = 0;
		for (int c=0; c<Wo/2; c++) {
			F16 *pBias = Bias + First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (2*c+0)*OutFeat+First;
			F16 *pOut1 = Out+l*Wo*OutFeat + (2*c+1)*OutFeat+First;
			F16 *pC = Filter + First*InFeat;
			F16 *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);

			for (int i=0; i<(IterOut/4); i++) {
				F16 *pIn0 = pI, *pIn1 = pIn0 + InFeat,
				    *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        F16V S00 = (F16V){*pBias,0.0}, S01 = S00; pBias++;
	                        F16V S10 = (F16V){*pBias,0.0}, S11 = S10; pBias++;
	                        F16V S20 = (F16V){*pBias,0.0}, S21 = S20; pBias++;
	                        F16V S30 = (F16V){*pBias,0.0}, S31 = S30; pBias++;
				for (int f=0; f<(InFeat/2); f++) {
					F16V V0 = *((F16V *)pIn0), V1 = *((F16V *)pIn1), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
	                               	S00 += V0*C0; S01 += V1*C0;
	                               	S10 += V0*C1; S11 += V1*C1;
	                               	S20 += V0*C2; S21 += V1*C2;
	                               	S30 += V0*C3; S31 += V1*C3;
					pIn0+=2; pIn1+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
				}
				if (InFeat&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, V1 = (F16V){*pIn1,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut1 = S01[0]+S01[1]; pOut1++;
				*pOut0 = S10[0]+S10[1]; pOut0++;
				*pOut1 = S11[0]+S11[1]; pOut1++;
				*pOut0 = S20[0]+S20[1]; pOut0++;
				*pOut1 = S21[0]+S21[1]; pOut1++;
				*pOut0 = S30[0]+S30[1]; pOut0++;
				*pOut1 = S31[0]+S31[1]; pOut1++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn0 = pI, *pIn1 = pIn0 + InFeat;
	                        F16V S00 = (F16V){*pBias,0.0}, S01 = S00; pBias++;
				for (int f=0; f<(InFeat/2); f++) {
					F16V V0 = *((F16V *)pIn0), V1 = *((F16V *)pIn1), C0 = *((F16V *)pC);
	                               	S00 += V0 * C0; S01 += V1 * C0;
					pIn0+=2; pIn1+=2; pC+=2;
				}
				if (InFeat&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, V1 = (F16V){*pIn1,0.0}, C0 = *((F16V *)pC);
					S00 += V0*C0; S01 += V1*C0;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut1 = S01[0]+S01[1]; pOut1++;
			}
			PosC += 2*Sx;
		}
		if (Wo&0X1) {
			PosC = (Wo/2)*2*Sx;
			F16 *pBias = Bias + First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (Wo-1)*OutFeat+First;
			F16 *pC = Filter + First*InFeat;
			F16 *pI = (In+PosL*W*InFeat + (PosC+0)*InFeat);

			for (int i=0; i<(IterOut/4); i++) {
				F16 *pIn0 = pI, *pC0 = pC, *pC1 = pC0+InFeat, *pC2 = pC1+InFeat, *pC3 = pC2+InFeat;
				pC=pC3+InFeat;
	                        F16V S00 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S10 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S20 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S30 = (F16V){*pBias,0.0}; pBias++;
				for (int f=0; f<(InFeat/2); f++) {
					F16V V0 = *((F16V *)pIn0), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
	                               	S00 += V0 * C0;
	                               	S10 += V0 * C1;
	                               	S20 += V0 * C2;
	                               	S30 += V0 * C3;
					pIn0+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
				}
				if (InFeat&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut0 = S10[0]+S10[1]; pOut0++;
				*pOut0 = S20[0]+S20[1]; pOut0++;
				*pOut0 = S30[0]+S30[1]; pOut0++;
			}
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn0 = pI;
	                        F16V S00 = (F16V){*pBias,0.0}; pBias++;
				for (int f=0; f<(InFeat/2); f++) {
					F16V V0 = *((F16V *)pIn0), C0 = *((F16V *)pC);
	                               	S00 += V0 * C0;
					pIn0+=2; pC+=2;
				}
				if (InFeat&0x1) {
					F16V V0 = (F16V){*pIn0,0}, C0 = *((F16V *)pC);
					S00 += V0*C0;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv1D_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	int IterOut = Last - First;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			if (Iter>=2) {
				for (int f=0; f<(Iter/2); f++)
					for (int i=Lb; i<Rb; i++) ((int *)(ColBuff+(i+Off)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + i*InFeat+F))[f];
				if (Iter&0x1) for (int i=Lb; i<Rb; i++) ((short int *)(ColBuff+(i+Off)*InFeat+F))[Iter-1] = ((short int *)(In+PosL*W*InFeat + i*InFeat+F))[Iter-1];
			} else for (int i=Lb; i<Rb; i++) ColBuff[(i+Off)*InFeat + F] = In[PosL*W*InFeat + i*InFeat + F];
			PosC += Sx;
			gap_waitbarrier(0);

	                // for (int Line=First; Line<Last; Line++) {
			F16 *pBias = Bias + First;
			F16 *pC = Filter + W_In1*First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S1 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S2 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S3 = (F16V){*pBias,0.0}; pBias++;
				F16 *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
				*pOut0 = S1[0]+S1[1]; pOut0++;
				*pOut0 = S2[0]+S2[1]; pOut0++;
				*pOut0 = S3[0]+S3[1]; pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn = ColBuff;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC);
					S0 += V0*C0;
					pIn+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC);
					S0 += V0*C0;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_DxDy_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;
	int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*Fx))[Iter1-1]=0;
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
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1 = (F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = S0[0]+S0[1]+S1[0]+S1[1];
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_DxDy_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	int IterOut = Last - First;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = PosC, Rb = PosC+DFx;
			if (Iter>=2) {
				for (int f=0; f<(Iter/2); f++)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (i>=0 && i<W) 
							((int *) (ColBuff + ii*InFeat + F))[f] = ((int *)(In+i*InFeat + F))[f];
				if (Iter&0x1)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (i>=0 && i<W) 
							((short int *) (ColBuff + ii*InFeat + F))[Iter-1] = ((short int *)(In+i*InFeat + F))[Iter-1];
			} else {
			       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
			       		if (i>=0 && i<W) 
						ColBuff[ii*InFeat + F] = In[i*InFeat + F];
			}
			PosC += Sx;
			gap_waitbarrier(0);

	                // for (int Line=First; Line<Last; Line++) {
			F16 *pBias = Bias + First;
			F16 *pC = Filter + W_In1*First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S1 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S2 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S3 = (F16V){*pBias,0.0}; pBias++;
				F16 *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
				*pOut0 = S1[0]+S1[1]; pOut0++;
				*pOut0 = S2[0]+S2[1]; pOut0++;
				*pOut0 = S3[0]+S3[1]; pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn = ColBuff;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC);
					S0 += V0*C0;
					pIn+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC);
					S0 += V0*C0;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}



void KerPar_MM_Conv1D_DxDy_ReLU_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy, Dx = Arg->Dx;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int DFx = Dx*(Fx-1)+1;
	int Prec=10;
	int InvDx = ((1<<Prec)+Dx-1)/Dx;

	int PosL = 0;
	int Iter = L-F;
	int Iter1 = Iter*Fx;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*Fx))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+DFx, W);
			int OffBuffX = Max(0, gap_mulsN(-PosC+Dx-1, InvDx, Prec));
			int OffInX = OffBuffX?(Dx*OffBuffX+PosC):0;
			int IterX = gap_mulsN(Rb-Lb-1, InvDx, Prec) + 1;
			if (Iter>=2) {
				for (int f=0; f<(Iter/2); f++)
					for (int i=0; i<IterX; i++)
						((int *)(ColBuff+(i+OffBuffX)*InFeat+F))[f] = ((int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[f];
				if (Iter&0x1)
					for (int i=0; i<IterX; i++)
						((short int *)(ColBuff+(i+OffBuffX)*InFeat+F))[Iter-1] = ((short int *)(In+PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat+F))[Iter-1];
			} else
				for (int i=0; i<IterX; i++)
					ColBuff[(i+OffBuffX)*InFeat + F] = In[PosL*W*InFeat + (i*Dx+OffInX+Lb)*InFeat + F];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1 = (F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[l*Wo*OutFeat + c*OutFeat + Line] = MaxF(S0[0]+S0[1]+S1[0]+S1[1], (F16)0.0);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv1D_ReLU_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Sy = Arg->Sy;
	int PadL = Arg->Pad[0];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;

        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;

	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	
	int Iter = (L-F)*Fx;
	int PosL = 0;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter/2); i++) ((int *)(ColBuff+F*Fx))[i]=0;
			if (Iter&0x1) ((short int *)(ColBuff+F*Fx))[Iter-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Off = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>2) {
				if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int i=0; i<Size/2; i++) *((int *)(ColBuff+Fx*f+Off+2*i+Lb)) = *((int *)(In+f*W*H+PosL*W+2*i+Lb));
						*((short int *)(ColBuff+Fx*f+Off+Rb-1)) = *((short int *)(In+f*W*H+PosL*W+Rb-1));
					}
				} else for (int f=F; f<L; f++) for (int i=0; i<Size/2; i++) *((int *)(ColBuff+Fx*f+Off+2*i+Lb)) = *((int *)(In+f*W*H+PosL*W+2*i+Lb));
			} else if (Size) for (int f=F; f<L; f++) ColBuff[Fx*f+Off+Lb] = In[f*W*H+PosL*W+Lb];
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1 = (F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = MaxF(S0[0]+S0[1]+S1[0]+S1[1], (F16)0.0);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
}

void KerPar_MM_Conv2D_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>2) {
				if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/2; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+2*i+Lb)) = *((int *)(In+f*W*H+j*W+2*i+Lb));
							*((ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-1)) = *((In+f*W*H+j*W+Rb-1));
						}
					}
				} else {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/2; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+2*i+Lb)) = *((int *)(In+f*W*H+j*W+2*i+Lb));
						}
					}
				}
			} else if (Size) {
				for (int f=F; f<L; f++) {
					for (int j=Tb; j<Db; j++) ColBuff[FS*f + Fx*(j+OffL)+OffC+Lb] = In[f*W*H+j*W+Lb];
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1 = (F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = S0[0]+S0[1]+S1[0]+S1[1];
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_ReLU_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
			int Size = Rb-Lb;
			if (Size>2) {
				if (Size&0x1) {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/2; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+2*i+Lb)) = *((int *)(In+f*W*H+j*W+2*i+Lb));
							*((ColBuff+FS*f + Fx*(j+OffL)+OffC+Rb-1)) = *((In+f*W*H+j*W+Rb-1));
						}
					}
				} else {
					for (int f=F; f<L; f++) {
						for (int j=Tb; j<Db; j++) {
							for (int i=0; i<Size/2; i++) *((int *)(ColBuff+FS*f + Fx*(j+OffL)+OffC+2*i+Lb)) = *((int *)(In+f*W*H+j*W+2*i+Lb));
						}
					}
				}
			} else if (Size) {
				for (int f=F; f<L; f++) {
					for (int j=Tb; j<Db; j++) ColBuff[FS*f + Fx*(j+OffL)+OffC+Lb] = In[f*W*H+j*W+Lb];
				}
			}
			PosC += Sx;
			gap_waitbarrier(0);
	                for (int Line=First; Line<Last; Line++) {
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0}, S1 = (F16V) 0;
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S1 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = MaxF(S0[0]+S0[1]+S1[0]+S1[1], (F16)0.0);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Last - First;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=2) {
                                for (int f=0; f<(Iter/2); f++)
					for (int j=Tb; j<Db; j++)
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
                        } else
				for (int j=Tb; j<Db; j++) 
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];

			PosC += Sx;
			gap_waitbarrier(0);
	                // for (int Line=First; Line<Last; Line++) {
			F16 *pBias = Bias + First;
			F16 *pC = Filter + W_In1*First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S1 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S2 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S3 = (F16V){*pBias,0.0}; pBias++;
				F16 *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
				*pOut0 = S1[0]+S1[1]; pOut0++;
				*pOut0 = S2[0]+S2[1]; pOut0++;
				*pOut0 = S3[0]+S3[1]; pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn = ColBuff;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC);
					S0 += V0*C0;
					pIn+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC);
					S0 += V0*C0;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}


void Ker_MM_Conv2D_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
        F16 * __restrict__ ColBuff1;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(Wo), First = CoreId*ChunkCell, Last  = Min(Wo, First+ChunkCell);

	int FS = Fx*Fy;
	int PosL = Arg->FirstTile?(-PadT):0;

	int Iter = InFeat;
	int Iter1 = Iter*FS;
	int IterOut = OutFeat;
	int IterW = Last-First;
	ColBuff += 2*CoreId*InFeat*FS;
	ColBuff1 = ColBuff + InFeat*FS;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL + First*Sx;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		// for (int c=0; c<Wo; c++) {
		// for (int c=First; c<Last; c++) {
		for (int c=0; c<IterW/2; c++) {
			for (int i=0; i<(Iter1/2); i++) {
				((int *)(ColBuff))[i]=0;
				((int *)(ColBuff1))[i]=0;
			}
			if (Iter1&0x1) {
				((short int *)(ColBuff))[Iter1-1]=0;
				((short int *)(ColBuff1))[Iter1-1]=0;
			}
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int Lb1 = Max(PosC+Sx, 0), Rb1 = Min(PosC+Fx+Sx, W);
			int OffC = -Lb - Min(PosC, 0);
			int OffC1 = -Lb1 - Min(PosC+Sx, 0);
                        if (Iter>=2) {
                                for (int f=0; f<(Iter/2); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
                                        	for (int i=Lb1; i<Rb1; i++)
							((int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter-1];
						for (int i=Lb1; i<Rb1; i++)
							((short int *)(ColBuff1+(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat))[Iter-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
					for (int i=Lb1; i<Rb1; i++)
						ColBuff1[(j+OffL)*InFeat*Fx+(i+OffC1)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			PosC += 2*Sx;
			F16 *pBias = Bias;
			F16 *pC = Filter;
			F16 *pOut0 = Out+l*Wo*OutFeat + (First + 2*c)*OutFeat;
			F16 *pOut1 = Out+l*Wo*OutFeat + (First + 2*c+1)*OutFeat;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S00 = (F16V){*pBias,0.0}, S01 = S00; pBias++;
	                        F16V S10 = (F16V){*pBias,0.0}, S11 = S10; pBias++;
	                        F16V S20 = (F16V){*pBias,0.0}, S21 = S20; pBias++;
	                        F16V S30 = (F16V){*pBias,0.0}, S31 = S30; pBias++;
				F16 *pIn0 = ColBuff, *pIn1 = ColBuff + Iter1;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn0), V1 = *((F16V *)pIn1), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
	                               	S00 += V0*C0; S01 += V1*C0;
	                               	S10 += V0*C1; S11 += V1*C1;
	                               	S20 += V0*C2; S21 += V1*C2;
	                               	S30 += V0*C3; S31 += V1*C3;
					pIn0+=2; pIn1+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, V1 = (F16V){*pIn1,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S00 += V0*C0; S01 += V1*C0;
					S10 += V0*C1; S11 += V1*C1;
					S20 += V0*C2; S21 += V1*C2;
					S30 += V0*C3; S31 += V1*C3;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut1 = S01[0]+S01[1]; pOut1++;
				*pOut0 = S10[0]+S10[1]; pOut0++;
				*pOut1 = S11[0]+S11[1]; pOut1++;
				*pOut0 = S20[0]+S20[1]; pOut0++;
				*pOut1 = S21[0]+S21[1]; pOut1++;
				*pOut0 = S30[0]+S30[1]; pOut0++;
				*pOut1 = S31[0]+S31[1]; pOut1++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn0 = ColBuff, *pIn1 = ColBuff + Iter1;
	                        F16V S00 = (F16V){*pBias,0.0}, S01 = S00; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn0), V1 = *((F16V *)pIn1), C0 = *((F16V *)pC);
	                               	S00 += V0*C0; S01 += V1 * C0;
					pIn0+=2; pIn1+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, V1 = (F16V){*pIn1,0.0}, C0 = *((F16V *)pC);
					S00 += V0*C0; S01 += V1*C0;
					pIn0++; pIn1++; pC++;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut1 = S01[0]+S01[1]; pOut1++;
			}
		}
		if (IterW&0x1) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff))[Iter1-1]=0;
			int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
			int OffC = -Lb - Min(PosC, 0);
                        if (Iter>=2) {
                                for (int f=0; f<(Iter/2); f++)
					for (int j=Tb; j<Db; j++) {
                                        	for (int i=Lb; i<Rb; i++)
							((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[f] = ((int *)(In+j*W*InFeat + i*InFeat))[f];
					}
                                if (Iter&0x1)
					for (int j=Tb; j<Db; j++) {
						for (int i=Lb; i<Rb; i++)
							((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat))[Iter-1] = ((short int *)(In+j*W*InFeat + i*InFeat))[Iter-1];
					}
                        } else
				for (int j=Tb; j<Db; j++) {
					for (int i=Lb; i<Rb; i++)
						ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat] = In[j*W*InFeat + i*InFeat];
				}
			F16 *pBias = Bias;
			F16 *pC = Filter;
			F16 *pOut0 = Out+l*Wo*OutFeat + (Last-1)*OutFeat;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S00 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S10 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S20 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S30 = (F16V){*pBias,0.0}; pBias++;
				F16 *pIn0 = ColBuff;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn0), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
	                               	S00 += V0 * C0;
	                               	S10 += V0 * C1;
	                               	S20 += V0 * C2;
	                               	S30 += V0 * C3;
					pIn0+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S00 += V0*C0;
					S10 += V0*C1;
					S20 += V0*C2;
					S30 += V0*C3;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
				*pOut0 = S10[0]+S10[1]; pOut0++;
				*pOut0 = S20[0]+S20[1]; pOut0++;
				*pOut0 = S30[0]+S30[1]; pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn0 = ColBuff;
	                        F16V S00 = (F16V){*pBias,0.0}; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn0), C0 = *((F16V *)pC);
					S00 += V0*C0;
					pIn0+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn0,0.0}, C0 = *((F16V *)pC);
					S00 += V0*C0;
				}
				*pOut0 = S00[0]+S00[1]; pOut0++;
			}
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*(Fx-1)+1, DFy =  Dy*(Fy-1)+1;
	int Prec=10;
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
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
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
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0};
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0 * C0;
	                                S0 += V1 * C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = S0[0]+S0[1];
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_HWC_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	/*
		For HWC weights (4D Tensor) are expected to be organized as [OutFeat x Fy x Fx x InFeat]
	*/
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*(Fx-1)+1, DFy =  Dy*(Fy-1)+1;
	// int Prec=10;
	// int InvDx = ((1<<Prec)+Dx-1)/Dx;
	// int InvDy = ((1<<Prec)+Dy-1)/Dy;
	int Iter = L-F;
	int Iter1 = Iter*FS;
	int IterOut = Last - First;

	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = PosL, Db = PosL+DFy;
		for (int c=0; c<Wo; c++) {
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
			int Lb = PosC, Rb = PosC+DFx;
			if (Iter>=2) {
				for (int f=0; f<(Iter/2); f++)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
					       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
					       		if (j>=0 && j<H && i>=0 && i<W) 
								((int *) (ColBuff + jj*InFeat*Fx+ii*InFeat + F))[f] = ((int *)(In+j*W*InFeat + i*InFeat + F))[f];
				if (Iter&0x1)
					for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
					       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
					       		if (j>=0 && j<H && i>=0 && i<W) 
								((short int *) (ColBuff + jj*InFeat*Fx+ii*InFeat + F))[Iter-1] = ((short int *)(In+j*W*InFeat + i*InFeat + F))[Iter-1];
			} else {
				for (int j=Tb, jj=0; j<Db; j+=Dy, jj++)
				       	for (int i=Lb, ii=0; i<Rb; i+=Dx, ii++)
				       		if (j>=0 && j<H && i>=0 && i<W) 
							ColBuff[jj*InFeat*Fx+ii*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
			}
			PosC += Sx;
			gap_waitbarrier(0);

	                // for (int Line=First; Line<Last; Line++) {
			F16 *pBias = Bias + First;
			F16 *pC = Filter + W_In1*First;
			F16 *pOut0 = Out+l*Wo*OutFeat + (c+0)*OutFeat+First;
	                for (int Line=0; Line<IterOut/4; Line++) {
				F16 *pC0 = pC, *pC1 = pC0+W_In1, *pC2 = pC1+W_In1, *pC3 = pC2+W_In1;
				pC=pC3+W_In1;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S1 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S2 = (F16V){*pBias,0.0}; pBias++;
	                        F16V S3 = (F16V){*pBias,0.0}; pBias++;
				F16 *pIn = ColBuff;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
					pIn+=2; pC0+=2; pC1+=2; pC2+=2; pC3+=2;
	                        }
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC0), C1 = *((F16V *)pC1), C2 = *((F16V *)pC2), C3 = *((F16V *)pC3);
					S0 += V0*C0;
					S1 += V0*C1;
					S2 += V0*C2;
					S3 += V0*C3;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
				*pOut0 = S1[0]+S1[1]; pOut0++;
				*pOut0 = S2[0]+S2[1]; pOut0++;
				*pOut0 = S3[0]+S3[1]; pOut0++;
	                }
			for (int i=4*(IterOut/4); i<IterOut; i++) {
				F16 *pIn = ColBuff;
	                        F16V S0 = (F16V){*pBias,0.0}; pBias++;
	                        for (int i=0; i<(W_In1/2); i++) {
					F16V V0 = *((F16V *)pIn), C0 = *((F16V *)pC);
					S0 += V0*C0;
					pIn+=2; pC+=2;
				}
				if (W_In1&0x1) {
					F16V V0 = (F16V){*pIn,0.0}, C0 = *((F16V *)pC);
					S0 += V0*C0;
				}
				*pOut0 = S0[0]+S0[1]; pOut0++;
			}
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

void KerPar_MM_Conv2D_DxDy_ReLU_fp16(
	Ker_MM_Conv_fp16_T *Arg
	)

{
	F16 *__restrict__ In = Arg->In;
	int W = Arg->W, H = Arg->H;
	F16 *__restrict__ Filter = Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx, Dx = Arg->Dx;
	int Fy = Arg->Fy, Sy = Arg->Sy, Dy = Arg->Dy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->InFeat, OutFeat = Arg->OutFeat;
        F16 * __restrict__ Bias = Arg->Bias;
        F16 * __restrict__ Out = Arg->Out;
        F16 * __restrict__ ColBuff = Arg->ColBuff;
	int Wo = Arg->Wo, Ho = Arg->Ho;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	F16V * __restrict__ VBuff = (F16V *) ColBuff;
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = CoreId*C, L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	int FS = Fx*Fy;
	int Tail = 2*((W_In1+3)/4);
	((int *)ColBuff)[Tail-1] = 0; ((int *)ColBuff)[Tail-2] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int DFx = Dx*(Fx-1)+1, DFy =  Dy*(Fy-1)+1;
	int Prec=10;
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
			for (int i=0; i<(Iter1/2); i++) ((int *)(ColBuff+F*FS))[i]=0;
			if (Iter1&0x1) ((short int *)(ColBuff+F*FS))[Iter1-1]=0;
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
	                        F16V *VIn1 = (F16V *) (&Filter[Line*W_In1 + 0]);
	                        F16V S0 = (F16V){Bias[Line],0.0};
	                        for (int i=0; i<((W_In1+3)/4); i++) {
	                                F16V V0 = VIn1[2*i], V1 = VIn1[2*i+1];
					F16V C0 = VBuff[2*i], C1 = VBuff[2*i+1];
	                                S0 += V0*C0;
	                                S0 += V1*C1;
	                        }
	                        Out[Line*Wo*Ho + l*Wo + c] = MaxF(S0[0]+S0[1], 0.0);
	                }
			gap_waitbarrier(0);
		}
		PosL += Sy;
	}
	gap_waitbarrier(0);
}

#pragma GCC diagnostic pop
