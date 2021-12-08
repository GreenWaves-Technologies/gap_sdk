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

#include "Gap.h"
#include "CNN_BasicKernels_fp16.h"
#include "CNN_Defines_fp16.h"

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

/* Set output features maps initial bias group
 
	Single precision form is called by KOP_SETBIAS

	KerParSetBias_fp16		Bias and out are short floats, output feature maps are evaluated in parallel (one per core)
	KerSetBias_fp16			Bias and out are short floats, a single output feature map is evaluated in parallel on all cores
*/

/* Set Bias, all outputs evalauted in parallel */

void KerParSetBias_fp16(KerSetBias_fp16_T *Arg)

{
	/* Bias in non DP context: point position b, no need to adjust */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Bias = Arg->Bias;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) {
		F16V *LineOut = (F16V *) (Out+W*H*of);
		F16 B = Bias[of];
		F16V Bv = (F16V) {B, B};
		for (unsigned int i=0; i<((W*H)/4); i++) {
			LineOut[2*i] = Bv; LineOut[2*i+1] = Bv;
		}
		for (unsigned int i=(4*((W*H)/4)); i<(W*H); i++) Out[W*H*of+i] = B;
	}
	gap_waitbarrier(0);
}

/* Set bias, one output evaluated in parallel */

void KerSetBias_fp16(KerSetBias_fp16_T *Arg)

{
	/* Bias in non DP context: point position b. No need to shift this is just a container size change. */
	F16 * __restrict__ Out = Arg->Out;
	int W = Arg->W;
	int H = Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	F16V * __restrict__ VectOut = (F16V *) (Out+First);
	int Iter = Max(0, Last-First);
	int i;

	F16 Bias = *Arg->Bias;
	F16V Vb = (F16V) {Bias, Bias};
	for (i=0; i<(Iter/2); i++) VectOut[i] = Vb;
	if (Iter&0x1) Out[Last-1] = Bias;
	gap_waitbarrier(0);
}

/* Activation group:

	Feature maps in and out are short floats:
		KerParReLU_fp16				All Feature maps are evaluated in parallel
		KerParReLUN_fp16			All Feature maps are evaluated in parallel
		KerParSwish_fp16			All Feature maps are evaluated in parallel
		KerParSigmoid_fp16			All Feature maps are evaluated in parallel
		KerParLeakyReLU_fp16			All Feature maps are evaluated in parallel
*/

void KerParReLU_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;
	F16V * VectIn  = (F16V *) (In+First);
	F16V * VectOut = (F16V *) (Out+First);
	F16V LB = (F16V) {0.0, 0.0};

	for (unsigned int i=0; i<Iter/4; i++) {
		F16V X = MaxF2(VectIn[2*i  ], LB);
		F16V Y = MaxF2(VectIn[2*i+1], LB);
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*(Iter/4); i<Iter; i++) Out[First+i] = MaxF(In[First+i], (F16)0.0);
	gap_waitbarrier(0);
}

void KerParReLUN_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;
	F16 UB = Arg->UB;

	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;
	F16V * VectIn  = (F16V *) (In+First);
	F16V * VectOut = (F16V *) (Out+First);

	F16V LBv = (F16V) {0.0, 0.0};
	F16V UBv = (F16V) {UB, UB};

	for (unsigned int i=0; i<Iter/4; i++) {
		F16V X = MinF2(MaxF2(VectIn[2*i  ], LBv), UBv);
		F16V Y = MinF2(MaxF2(VectIn[2*i+1], LBv), UBv);
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*(Iter/4); i<Iter; i++) Out[First+i] = MinF(MaxF(In[First+i], (F16)0.0), UB);

	gap_waitbarrier(0);
}

void KerParSwish_fp16(KerActivation_fp16_T *Arg)

{
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;

	F16V *vIn  = (F16V*)(Arg->In+First), *vOut = (F16V*)(Arg->Out+First);
	for (unsigned int i=0; i<Iter/2; i++) {
		*vOut++ = FastSwishF16V(*vIn);
		vIn++;
	}
	if (Iter&0x1) *((F16 *)vOut) = FastSwishF16(*((F16 *)vIn));
	gap_waitbarrier(0);
}

void KerParHSwish_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;
	F16 Offset = (F16) 3.0;
	F16V Offsetv = (F16V) {Offset, Offset};

	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;
	F16V * VectIn  = (F16V *) (In+First);
	F16V * VectOut = (F16V *) (Out+First);

	F16V LBv = (F16V) {0.0, 0.0};
	F16V UBv = (F16V) {6.0, 6.0};

	for (unsigned int i=0; i<Iter/4; i++) {
		F16V X = MinF2(MaxF2(VectIn[2*i  ] + Offsetv, LBv), UBv) / UBv;
		F16V Y = MinF2(MaxF2(VectIn[2*i+1] + Offsetv, LBv), UBv) / UBv;
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*(Iter/4); i<Iter; i++) Out[First+i] = MinF(MaxF(In[First+i] + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;

	gap_waitbarrier(0);
}

void KerParSigmoid_fp16(KerActivation_fp16_T *Arg)

{
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;

	F16V *vIn  = (F16V*)(Arg->In+First), *vOut = (F16V*)(Arg->Out+First);
	for (unsigned int i=0; i<Iter/2; i++) {
		*vOut++ = FastSigmoidF16V(*vIn);
		vIn++;
	}
	if (Iter&0x1) *((F16 *)vOut) = FastSigmoidF16(*((F16 *)vIn));
	gap_waitbarrier(0);
}

void KerParHSigmoid_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;
	F16 Offset = Arg->UB;
	F16V Offsetv = (F16V) {Offset, Offset};

	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;
	F16V * VectIn  = (F16V *) (In+First);
	F16V * VectOut = (F16V *) (Out+First);

	F16V LBv = (F16V) {0.0, 0.0};
	F16V UBv = (F16V) {6.0, 6.0};

	for (unsigned int i=0; i<Iter/4; i++) {
		F16V X = MinF2(MaxF2(VectIn[2*i  ] + Offsetv, LBv), UBv) / UBv;
		F16V Y = MinF2(MaxF2(VectIn[2*i+1] + Offsetv, LBv), UBv) / UBv;
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*(Iter/4); i<Iter; i++) Out[First+i] = MinF(MaxF(In[First+i] + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;

	gap_waitbarrier(0);
}

void KerParTanh_fp16(KerActivation_fp16_T *Arg)

{
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;

	F16V *vIn  = (F16V*)(Arg->In+First), *vOut = (F16V*)(Arg->Out+First);
	for (unsigned int i=0; i<Iter/2; i++) {
		*vOut++ = FastTanhF16V(*vIn);
		vIn++;
	}
	if (Iter&0x1) *((F16 *)vOut) = FastTanhF16(*((F16 *)vIn));
	gap_waitbarrier(0);
}

void KerParHTanh_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;
	F16V * VectIn  = (F16V *) (In+First);
	F16V * VectOut = (F16V *) (Out+First);

	F16V LBv = (F16V) {-1.0, -1.0};
	F16V UBv = (F16V) { 1.0,  1.0};

	for (unsigned int i=0; i<Iter/4; i++) {
		F16V X = MinF2(MaxF2(VectIn[2*i  ], LBv), UBv);
		F16V Y = MinF2(MaxF2(VectIn[2*i+1], LBv), UBv);
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*(Iter/4); i<Iter; i++) Out[First+i] = MinF(MaxF(In[First+i], (F16)-1.0), (F16)1.0);

	gap_waitbarrier(0);
}


void KerParLeakyReLU_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;


	unsigned int S = OutFeatures*W*H, CoreId = gap_coreid(), Chunk = ChunkSize(S), First = Chunk*CoreId, Last = Min(First+Chunk, S), Iter = Last-First;

	In  = (In+First); Out = (Out+First);
	for (unsigned int i=0; i<Iter/2; i++) {
		F16 Acc0 = In[2*i], Acc1 = In[2*i+1];
		Out[2*i]   = LeakyReLU_fp16(Acc0);
		Out[2*i+1] = LeakyReLU_fp16(Acc1);
	}
	if (Iter&0x1) Out[Iter-1] = LeakyReLU_fp16(In[Iter-1]);
	gap_waitbarrier(0);
}

/**************************************************************************************************************************************
	Linear Layer followed by optional activation 
**************************************************************************************************************************************/

/*
	All the KerParLinearLayer family evaluates output channels in parallel
*/

void KerParLinearLayer_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = A;
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerReLU_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = MaxF(A, (F16) 0.0);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerReLUN_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 UB = Arg->UB;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = MinF(MaxF(A, (F16) 0.0), UB);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerSwish_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = FastSwishF16(A);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerHSwish_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = MinF(MaxF(A + (F16)3.0, (F16)0.0), (F16)6.0) / (F16)6.0;
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerSigmoid_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);

		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = FastSigmoidF16(A);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerHSigmoid_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 Offset = Arg->UB;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);
		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = MinF(MaxF(A + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerTanh_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);

		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = FastTanhF16(A);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerHTanh_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);

		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = MinF(MaxF(A, (F16)-1.0), (F16)1.0);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerLeakyReLU_fp16(KerLinear_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;
	int InDim = Arg->InDim;
	int Init = Arg->TileIndex==0;
	const F16 * __restrict__ Filter = Arg->Filter;
	const F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	int OutDim = Arg->OutDim;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutDim);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutDim);
	int i,j;
	F16V * __restrict__ VectIn = (F16V *) In;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ Filt = (F16V *) (&Filter[i*TotalInDim]);

		F16V Acc = (F16V) {0.0, 0.0};
		for (j = 0; j<(InDim/(2*2)); j++) {
			Acc += VectIn[2*j] * Filt[2*j];
			Acc += VectIn[2*j+1] * Filt[2*j+1];
		}
		if (InDim&0x2) Acc += VectIn[InDim/2-1] * Filt[InDim/2-1];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		if (A<0.0) A *= LEAK_CONSTANT;
		Out[i] = A;
	}
	gap_waitbarrier(0);
}

