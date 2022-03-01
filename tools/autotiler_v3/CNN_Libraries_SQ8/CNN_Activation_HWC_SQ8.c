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
#if 0

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#include "CNN_BasicKernels_SQ8.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static int FirstDefinedOutput(int F, int Pad, int Stride)

{
	// k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

	return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(int DimIn, int F, int PadL, int Stride)

{
	// k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

	return ((DimIn - ((F-1)/2 - PadL + (F/2)) + Stride-1)/Stride);
}

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

/*
 * Buffer compaction, scattered by chunk size groups of 8b moved to a contiguous representation through a parallel reduction tree
*/
static void __attribute__ ((noinline)) KerReductIO_Compact_HWC_SQ8(int *__restrict__ In, unsigned int Size, unsigned int CoreId, unsigned int ChunkCell)

{
	unsigned int U = gap_ncore()/2, Log2Core = gap_fl1(gap_ncore()), A = 2, B = 1;
	for (int k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+((sizeof(int)/sizeof(signed char))*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (int i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (int i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
	}
	gap_waitbarrier(0);
}

#define B_CLR(x, bits)  ((x)&(~((1<<(bits))-1)))
static void KerReductIO_Compact_HWC_SQ8_1(char *__restrict__ To, char *__restrict__ From, int Size, int TotalSize)

{
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
        unsigned int Iter = Max(0, Last-First);

	for (int i=Size; i<TotalSize; i+=Size) {
		From += Size*4; To += Size;

        	int *pFrom = (int *) (From+First), *pTo = (int *) (To+First);
        	for (int j=0; j<Iter/8; j++) {
                	int V0 = pFrom[2*j], V1 = pFrom[2*j+1];
                	pTo[2*j] = V0; pTo[2*j+1] = V1;
        	}
        	if (Iter & 0x4) *((int *) (To + First + B_CLR(Iter, 3))) = *((int *) (From + First + B_CLR(Iter, 3)));
        	if (Iter & 0x2) *((short int *) (To + First + B_CLR(Iter, 2))) = *((short int *) (From + First + B_CLR(Iter, 2)));
        	if (Iter & 0x1) *((signed char *) (To + First + Iter - 1)) = *((signed char *) (From + First + Iter - 1));
		gap_waitbarrier(0);
	}
}

void KerParReduct_CC_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat; c++) {
	                Out[i*Feat + c] = gap_clip(AT_SCALE(In[i*Feat+c], Scale[c],   ScaleN[c]), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c],   ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

	                Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 7); 
	                Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Max(0, Acc1), ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLUN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c],   ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

	                Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 7);
	                Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1, A0), ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUM_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c],   ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Max(A0, Acc1), ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUMN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c],   ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc1)), ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_HSigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c], ScaleN[2*c]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_HSwish_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c  ], ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_LeakyReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c  ], ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);


			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
		        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Sigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c], ScaleN[2*c]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Sigmoid(Acc0 << 8) >> 8, ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Sigmoid(Acc1 << 8) >> 8, ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Sigmoid(Acc0 << 8) >> 8, ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Tanh_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + 2*c+0], Scale[2*c  ], ScaleN[2*c  ]), 7);
	                int Acc1 = gap_clip(AT_SCALE(In[i*Feat + 2*c+1], Scale[2*c+1], ScaleN[2*c+1]), 7);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Tanh(Acc0 << 8) >> 8, ActScale, ActScaleN), 7);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Tanh(Acc1 << 8) >> 8, ActScale, ActScaleN), 7);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(In[i*Feat + Feat-1], Scale[Feat-1], ScaleN[Feat-1]), 7);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Tanh(Acc0 << 8) >> 8, ActScale, ActScaleN), 7);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat; c++) {
	                Out[i*Feat + c] = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + c], Prenorm), Scale[c], ScaleN[c]), 15);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

	                Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 15);
	                Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Max(0, Acc1), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLUN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

	                Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 15);
	                Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1, A0), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUM_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Max(A0, Acc1), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUMN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ],   ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc1)), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_HSigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_HSwish_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_LeakyReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);


			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			Out[i*Feat + Feat-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 15);
		        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Sigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Sigmoid(Acc0 << 8), ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Sigmoid(Acc1 << 8), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Sigmoid(Acc0 << 8) >> 8, ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Tanh_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed short * __restrict__ Out = (signed short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clip(AT_SCALE(Tanh(Acc0 << 8), ActScale, ActScaleN), 15);
			Out[i*Feat + 2*c+1] = gap_clip(AT_SCALE(Tanh(Acc1 << 8), ActScale, ActScaleN), 15);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clip(AT_SCALE(Tanh(Acc0 << 8) >> 8, ActScale, ActScaleN), 15);
	        }
	}
	gap_waitbarrier(0);
}


void KerParReduct_CC_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);
	        }
		Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);

	                Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 16);
	                Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(Max(0, Acc1), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
	                Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(Max(0, Acc0), ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLUN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);

	                Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 16);
	                Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc1, A0), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
	                Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0, A0), ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUM_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);

			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(Max(A0, Acc1), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
			Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(Max(A0, Acc0), ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_ReLUMN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);

			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(Min(B0, Max(A0, Acc1)), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
			Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(Min(B0, Max(A0, Acc0)), ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);

}

void KerParReduct_CC_HSigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];
	// Output is asymmetric with zeropoint at 0
	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat; c++) {
	                int Acc0 = AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c],   ScaleN[2*c  ]);
	                int Acc1 = AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]);

			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]);
	                Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_HSwish_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];
	// Output is asymmetric with zeropoint at 0
	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);

			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
	                Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_LeakyReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 16);
	                int Acc1 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 16);


			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clipu(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 16);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 16);
		        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Sigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];
	// Output is asymmetric with zeropoint at 0
	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = gap_clipu(AT_SCALE(SigmoidU(Acc0), ActScale, ActScaleN), 16);
			Out[i*Feat + 2*c+1] = gap_clipu(AT_SCALE(SigmoidU(Acc1), ActScale, ActScaleN), 16);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = gap_clipu(AT_SCALE(SigmoidU(Acc0), ActScale, ActScaleN), 16);
	        }
	}
	gap_waitbarrier(0);
}

void KerParReduct_CC_Tanh_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg)

{
	int Feat = Arg->Feat;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H*W), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, H*W);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	unsigned short * __restrict__ Out = (unsigned short *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	int Prenorm = Infos[AT_INF_PRENORM];

	for (int i=First; i<Last; i++) {
		for (int c=0; c<Feat/2; c++) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+0], Prenorm), Scale[2*c  ], ScaleN[2*c  ]), 15);
	                int Acc1 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + 2*c+1], Prenorm), Scale[2*c+1], ScaleN[2*c+1]), 15);

			Out[i*Feat + 2*c  ] = (unsigned short) (gap_clip(AT_SCALE(Tanh(Acc0), ActScale, ActScaleN), 15) + 32768);
			Out[i*Feat + 2*c+1] = (unsigned short) (gap_clip(AT_SCALE(Tanh(Acc1), ActScale, ActScaleN), 15) + 32768);
	        }
	        if (Feat&0x1) {
	                int Acc0 = gap_clip(AT_SCALE(AT_NORM(In[i*Feat + Feat-1], Prenorm), Scale[Feat-1], ScaleN[Feat-1]), 15);
	                Out[i*Feat + Feat-1] = (unsigned short) (gap_clip(AT_SCALE(Tanh(Acc0), ActScale, ActScaleN), 15)  + 32768);
	        }
	}
	gap_waitbarrier(0);
}

// void parray(unsigned char * Out, int Feat, int H, int W) {
// 	for (int c=0; c<Feat; c++){
//         for (int h=0; h<H; h++){
//             for (int w=0; w<W; w++){
//                 printf("%3u ", Out[i*Feat + c]);
//             }
//             printf("\n");
//         }
//         printf("--- chan %d\n", c);
//     }
// }

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Border_HWC_USQ8(
	unsigned char * __restrict__ In,
	unsigned char * __restrict__ Out,
	int Fw,
	int Fh,
	v4s Pad,
	unsigned char PadVal,
	int C,
	int First,
	int Limit4,
	int Count,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideW,
	int StrideH
	)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadT;
	int Hi_L = Hi_F + (Ho_L-1)*StrideH;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadL;
	int Wi_L = Wi_F + (Wo_L-1)*StrideW;	// iff Wi_L>Wi_F

	int Last = First+Limit4;
	v4u vPadVal = (v4u) {PadVal, PadVal, PadVal, PadVal};
	for (unsigned int c=First; c<Last+(Count%4?4:0); c+=4) {
		int LastLen = (c >= Last?Count-Limit4:0);
		if (PadT)
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int ht = PadT, hb = H - Hi_F + Fh2;
				/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
				for (unsigned int h=0; h<Ho_F; h++) {
					v4u Acc = vPadVal;
					int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=0; j<Fw; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
					if (LastLen) {
						unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
						if (LastLen&2) {
							*(pOut++) = *((unsigned short *) &Acc);
						}
						if (LastLen&1) {
							*((unsigned char *)pOut) = Acc[0];
						}
					} else {
						*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
					}
					ht -= StrideH; hb -= StrideH;
				}
			}
		if (PadB) 
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int ht = 0, hb = H - (Hi_L+StrideH) + Fh2;
				/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
				for (unsigned int h=Ho_L; h<Ho; h++) {
					int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
					v4u Acc = vPadVal;
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=0; j<Fw; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
					if (LastLen) {
						unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
						if (LastLen&2) {
							*(pOut++) = *((unsigned short *) &Acc);
						}
						if (LastLen&1) {
							*((unsigned char *)pOut) = Acc[0];
						}
					} else {
						*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
					}
					hb -= StrideH;
				}
			}
		if (PadL)
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int wl = PadL, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					v4u Acc = vPadVal;
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
					for (unsigned int i=0; i<Fh; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
					if (LastLen) {
						unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
						if (LastLen&2) {
							*(pOut++) = *((unsigned short *) &Acc);
						}
						if (LastLen&1) {
							*((unsigned char *)pOut) = Acc[0];
						}
					} else {
						*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
					}
					wl -= StrideW; wr -= StrideW;
				}
			}
		if (PadR) 
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int wl = 0, wr = W - (Wi_L+StrideW) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					v4u Acc = vPadVal;
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
					for (unsigned int i=0; i<Fh; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
					if (LastLen) {
						unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
						if (LastLen&2) {
							*(pOut++) = *((unsigned short *) &Acc);
						}
						if (LastLen&1) {
							*((unsigned char *)pOut) = Acc[0];
						}
					} else {
						*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
					}
					wr -= StrideW;
				}
			}
		if (PadT) {
			if (PadL) { /* Upper left corner */
				int ht = PadT, hb = H - Hi_F + Fh2;
				for (unsigned int h=0; h<Ho_F; h++) {
					int wl = PadL, wr = W - Wi_F + Fw2;
					for (unsigned int w=0; w<Wo_F; w++) {
						v4u Acc = vPadVal;
						// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
						int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
						for (unsigned int i=Fh_min; i<Fh_max; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
						if (LastLen) {
							unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
							if (LastLen&2) {
								*(pOut++) = *((unsigned short *) &Acc);
							}
							if (LastLen&1) {
								*((unsigned char *)pOut) = Acc[0];
							}
						} else {
							*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
						}
						wl -= StrideW; wr -= StrideW;
					}
					ht -= StrideH; hb -= StrideH;
				}
			}
			if (PadR) { /* Upper right corner */
				int ht = PadT, hb = H - Hi_F + Fh2;
				for (unsigned int h=0; h<Ho_F; h++) {
					int wl = 0, wr = W - (Wi_L+StrideW) + Fw2;
					for (unsigned int w=Wo_L; w<Wo; w++) {
						v4u Acc = vPadVal;
						// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
						int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
						for (unsigned int i=Fh_min; i<Fh_max; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
						if (LastLen) {
							unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
							if (LastLen&2) {
								*(pOut++) = *((unsigned short *) &Acc);
							}
							if (LastLen&1) {
								*((unsigned char *)pOut) = Acc[0];
							}
						} else {
							*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
						}
						wr -= StrideW;
					}
					ht -= StrideH; hb -= StrideH;
				}
			}
		}
		if (PadB) {
			if (PadL) { /* Bottom Left corner */
				int ht = 0, hb = H - (Hi_L+StrideH) + Fh2;
				for (unsigned int h=Ho_L; h<Ho; h++) {
					int wl = PadL, wr = W - Wi_F + Fw2;
					for (unsigned int w=0; w<Wo_F; w++) {
						v4u Acc = vPadVal;
						// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
						int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
						for (unsigned int i=Fh_min; i<Fh_max; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
						if (LastLen) {
							unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
							if (LastLen&2) {
								*(pOut++) = *((unsigned short *) &Acc);
							}
							if (LastLen&1) {
								*((unsigned char *)pOut) = Acc[0];
							}
						} else {
							*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
						}
						wl -= StrideW; wr -= StrideW;
					}
					hb -= StrideH;
				}
			}
			if (PadR) { /* Bottom Right corner */
				int ht = 0, hb = H - (Hi_L+StrideH) + Fh2;
				for (unsigned int h=Ho_L; h<Ho; h++) {
					int wl = 0, wr = W - (Wi_L+StrideW) + Fw2;
					for (unsigned int w=Wo_L; w<Wo; w++) {
						v4u Acc = vPadVal;
						// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
						int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
						for (unsigned int i=Fh_min; i<Fh_max; i++) 
							for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = gap_maxu4(Acc, *((v4u *)&In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c]));
						if (LastLen) {
							unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
							if (LastLen&2) {
								*(pOut++) = *((unsigned short *) &Acc);
							}
							if (LastLen&1) {
								*((unsigned char *)pOut) = Acc[0];
							}
						} else {
							*((v4u*) &Out[C*Wo*h+C*w+c]) = Acc;
						}
						wr -= StrideW;
					}
					hb -= StrideH;
				}
			}
		}
	}
}

void KerParPool_PoolNxMStrideSxSyBody__HWC_USQ8(
	unsigned char * __restrict__ In,
	unsigned char * __restrict__ Out,
	int Fw,
	int Fh,
	int PadL,
	int PadT,
	int W,
	int H,
	int C,
	int First,
	int Count,
	int Limit4,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideW,
	int StrideH
	)

{
	for (unsigned int c=First; c<First+(Limit4); c+=4) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				v4u Acc = (v4u) {0, 0, 0, 0};
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++){
						int * val = (int *) &In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c];
						Acc = gap_maxu4(Acc, (v4u) *val);
					}
				}
				*((v4u *) &Out[C*Wo*h+C*w+c]) = Acc;
			}
		}
	}
	int Left = Count%4;
	if (Left) {
		unsigned int c = First + Count - Left;
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				v4u Acc = (v4u) {0, 0, 0, 0};
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++){
						int * val = (int *) &In[(h*StrideH-PadT+i)*W*C + (w*StrideW-PadL+j)*C + c];
						Acc = gap_maxu4(Acc, (v4u) *val);
					}
				}
				unsigned short * pOut = (unsigned short *) &Out[C*Wo*h+C*w+c];
				if (Left&2) {
					*(pOut++) = *((unsigned short *) &Acc);
				}
				if (Left&1) {
					*((unsigned char *)pOut) = Acc[0];
				}
			}
		}
	}
}



void KerParPool_MaxPoolNxMStrideSxSy__HWC_USQ8(KerPool_HWC_USQ8_T *Arg)
{
	unsigned int FSx=Arg->FS, Sx=Arg->S;
	unsigned int FSy=Arg->FSy, Sy=Arg->Sy;
	unsigned char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	unsigned char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	int Count = Last-First;
	int Limit4 = (Count/4) * 4;
	KerParPool_PoolNxMStrideSxSyBody__HWC_USQ8(In, Out, FSx, FSy, PadIn[0], PadIn[2], W, H, Feat, First, Count, Limit4, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
	if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_HWC_USQ8(In, Out, FSx, FSy, PadIn, (unsigned char) Infos[AT_INF_NE16_PADVAL], Feat, First, Limit4, Count, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);

	gap_waitbarrier(0);
}

#endif
#pragma GCC diagnostic pop
