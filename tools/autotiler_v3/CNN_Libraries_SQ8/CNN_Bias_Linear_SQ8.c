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
#include "CNN_BasicKernels_SQ8.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"

#define VOL volatile


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

	KerParSetBiasB32_SQ8		Bias and out are int, output feature maps are evaluated in parallel (one per core)
	KerParSetBiasB16_SQ8		Bias is half word, out is int, output feature maps are evaluated in parallel (one per core)
	KerParSetBiasB8_SQ8		Bias is byte, out is int, output feature maps are evaluated in parallel (one per core)

	KerSetBiasB32_SQ8		Bias and out are int, a single output feature map is evaluated in parallel on all cores
	KerSetBiasB16_SQ8		Bias is short, out is int, a single output feature map is evaluated in parallel on all cores
	KerSetBiasB8_SQ8		Bias is byte, out is int, a single output feature map is evaluated in parallel on all cores
*/

/* Set Bias, all outputs evalauted in parallel */
void KerParSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) {
		int *LineOut = (int *) (Out+W*H*of);
		int B = AT_LSHIFT(Bias[of], NormBias);
		for (unsigned int i=0; i<((W*H)/2); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		LineOut[W*H-1] = B;
	}
	gap_waitbarrier(0);
}

void KerParSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) {
		int *LineOut = (int *) (Out+W*H*of);
		int B = AT_LSHIFT(Bias[of], NormBias);
		for (unsigned int i=0; i<((W*H)/2); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		LineOut[W*H-1] = B;
	}
	gap_waitbarrier(0);
}

void KerParSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) {
		int *LineOut = (int *) (Out+W*H*of);
		int B = AT_LSHIFT(Bias[of], NormBias);
		for (unsigned int i=0; i<((W*H)/2); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		LineOut[W*H-1] = B;
	}
	gap_waitbarrier(0);
}

/* Set bias, one output evaluated in parallel */
void KerSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (unsigned int f=0; f<Feat; f++) {
		int B = AT_LSHIFT(Bias[f], NormBias);
		for (unsigned int i=0; i<Iter/2; i++) {
			Out[W*H*f + First + 2*i] = B; Out[W*H*f + First + 2*i+1] = B;
		}
		if (Iter&0x1) Out[W*H*f + First + Iter-1] = B;
	}
	gap_waitbarrier(0);
}

void KerSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (int f=0; f<Feat; f++) {
		int B = AT_LSHIFT(Bias[f], NormBias);
		for (int i=0; i<Iter/2; i++) {
			Out[W*H*f + First + 2*i] = B; Out[W*H*f + First + 2*i+1] = B;
		}
		if (Iter&0x1) Out[W*H*f + First + Iter-1] = B;
	}
	gap_waitbarrier(0);
}

void KerSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (int f=0; f<Feat; f++) {
		int B = AT_LSHIFT(Bias[f], NormBias);
		for (int i=0; i<Iter/2; i++) {
			Out[W*H*f + First + 2*i] = B; Out[W*H*f + First + 2*i+1] = B;
		}
		if (Iter&0x1) Out[W*H*f + First + Iter-1] = B;
	}
	gap_waitbarrier(0);
}


/* Set Bias, all outputs evalauted in parallel */
void KerParSetBiasB32_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int i=0; i<(W*H); i++) {
		for (unsigned int of=First; of<Last; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

void KerParSetBiasB16_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int i=0; i<(W*H); i++) {
		for (unsigned int of=First; of<Last; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

void KerParSetBiasB8_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int i=0; i<(W*H); i++) {
		for (unsigned int of=First; of<Last; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

/* Set bias, one output evaluated in parallel */
void KerSetBiasB32_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (unsigned int i=First; i<Last; i++) {
		for (unsigned int of=0; of<Feat; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

void KerSetBiasB16_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (unsigned int i=First; i<Last; i++) {
		for (unsigned int of=0; of<Feat; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

void KerSetBiasB8_HWC_SQ8(KerSetBias_SQ8_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H, Feat = Arg->Feat;
	signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(W*H), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, W*H), Iter = Max(0, Last-First);

	for (unsigned int i=First; i<Last; i++) {
		for (unsigned int of=0; of<Feat; of++) {
			int B = AT_LSHIFT(Bias[of], NormBias);
			Out[i*Feat+of] = B;
		}
	}
	gap_waitbarrier(0);
}

/**************************************************************************************************************************************
	Linear Layer with 32b output. Bias assumed to be set before. Scaling and activation done after
**************************************************************************************************************************************/

/* Output cannnot be evaluated completly, result on 32b, Bias assumed to be set before and reduction/scaling/activation after */
void KerParLinearLayer_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	int TotalInDim = Arg->TotalInDim;				// Reorganize weight layout to make TotalInDim = InDim
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(OutDim), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, OutDim);
	v4s * __restrict__ VectIn = (v4s *) In;

	for (int i=First; i<Last; i++) {
		v4s * __restrict__ W = (v4s *) (&Weights[i*TotalInDim]);
		int Acc = Out[i];
		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C0=W[2*j], C1=W[2*j+1];
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		}
		if (InDim&0x4) Acc = gap_sumdotp4(VectIn[InDim/4-1], W[InDim/4-1], Acc);
		for (int j=4*(InDim/4); j<InDim; j++) Acc += In[j]*Weights[i*TotalInDim+j];
		Out[i] = Acc;
	}
	gap_waitbarrier(0);
}

/* Output can be evaluated completly */
/* 8b Bias */
static inline void __attribute__((always_inline)) KerParLinearLayerFullFeatB8_SQ8_act(
	KerLinear_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(OutDim), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, OutDim);
	v4s * __restrict__ VectIn = (v4s *) In;
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	for (int i=First; i<Last; i++) {
		v4s * __restrict__ W = (v4s *) (&Weights[i*InDim]);
		int Acc = AT_LSHIFT(Bias[i], NormBias);
		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C0=W[2*j], C1=W[2*j+1];
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		}
		if (InDim&0x4) Acc = gap_sumdotp4(VectIn[InDim/4-1], W[InDim/4-1], Acc);
		for (int j=4*(InDim/4); j<InDim; j++) Acc += In[j]*Weights[i*InDim+j];
		Acc = AT_SCALE(Acc, Scale[i], ScaleN[i]); ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Out[i] = gap_clip(Acc, 7);
	}
	gap_waitbarrier(0);
}


void KerParLinearLayerFullFeatB8_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_NONE);
}

void KerParLinearLayerFullFeatB8_ReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_RELU);
}

void KerParLinearLayerFullFeatB8_ReLUN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_RELUN);
}

void KerParLinearLayerFullFeatB8_ReLUM_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_RELUM);
}

void KerParLinearLayerFullFeatB8_ReLUMN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_RELUMN);
}

void KerParLinearLayerFullFeatB8_LeakyReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerParLinearLayerFullFeatB8_HSigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerParLinearLayerFullFeatB8_HSwish_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_HSWISH);
}

void KerParLinearLayerFullFeatB8_Sigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_SIGMOID);
}

void KerParLinearLayerFullFeatB8_Tanh_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB8_SQ8_act(Arg, ACT_TANH);
}


/* 16b Bias */
static inline void __attribute__((always_inline)) KerParLinearLayerFullFeatB16_SQ8_act(
	KerLinear_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)
{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(OutDim), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, OutDim);
	v4s * __restrict__ VectIn = (v4s *) In;
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	for (int i=First; i<Last; i++) {
		v4s * __restrict__ W = (v4s *) (&Weights[i*InDim]);
		int Acc = AT_LSHIFT(Bias[i], NormBias);
		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C0=W[2*j], C1=W[2*j+1];
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		}
		if (InDim&0x4) Acc = gap_sumdotp4(VectIn[InDim/4-1], W[InDim/4-1], Acc);
		for (int j=4*(InDim/4); j<InDim; j++) Acc += In[j]*Weights[i*InDim+j];
		Acc = AT_SCALE(Acc, Scale[i], ScaleN[i]); ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Out[i] = gap_clip(Acc, 7);
	}
	gap_waitbarrier(0);
}


void KerParLinearLayerFullFeatB16_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_NONE);
}

void KerParLinearLayerFullFeatB16_ReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_RELU);
}

void KerParLinearLayerFullFeatB16_ReLUN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_RELUN);
}

void KerParLinearLayerFullFeatB16_ReLUM_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_RELUM);
}

void KerParLinearLayerFullFeatB16_ReLUMN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_RELUMN);
}

void KerParLinearLayerFullFeatB16_LeakyReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerParLinearLayerFullFeatB16_HSigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerParLinearLayerFullFeatB16_HSwish_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_HSWISH);
}

void KerParLinearLayerFullFeatB16_Sigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_SIGMOID);
}

void KerParLinearLayerFullFeatB16_Tanh_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB16_SQ8_act(Arg, ACT_TANH);
}

/* 32b Bias */
static inline void __attribute__((always_inline)) KerParLinearLayerFullFeatB32_SQ8_act(
	KerLinear_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)
{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * Infos = (unsigned char *) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = *((signed char *) &Infos[AT_INF_A0]); int B0 = *((signed char *) &Infos[AT_INF_B0]); int C0 = *((signed char *) &Infos[AT_INF_C0]);

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(OutDim), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, OutDim);
	v4s * __restrict__ VectIn = (v4s *) In;

	for (int i=First; i<Last; i++) {
		v4s * __restrict__ W = (v4s *) (&Weights[i*InDim]);
		int Acc = AT_LSHIFT(Bias[i], NormBias);
		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C0=W[2*j], C1=W[2*j+1];
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		}
		if (InDim&0x4) Acc = gap_sumdotp4(VectIn[InDim/4-1], W[InDim/4-1], Acc);
		for (int j=4*(InDim/4); j<InDim; j++) Acc += In[j]*Weights[i*InDim+j];
		Acc = AT_SCALE(Acc, Scale[i], ScaleN[i]); ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Out[i] = gap_clip(Acc, 7);
	}
	gap_waitbarrier(0);
}

/*	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(OutDim), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, OutDim);
	int Iter = Last-First;
	v4s * __restrict__ VectIn = (v4s *) In;

	for (int i=0; i<(Iter/2); i++) {
		int line1 = First + 2*i;
		int line2 = First + 2*i+1;
		v4s * __restrict__ W1 = (v4s *) (&Weights[(line1)*InDim]);
		v4s * __restrict__ W2 = (v4s *) (&Weights[(line2)*InDim]);
		int Acc1 = AT_LSHIFT(Bias[line1], NormBias);
		int Acc2 = AT_LSHIFT(Bias[line2], NormBias);

		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C10=W1[2*j], C11=W1[2*j+1];
			v4s C20=W2[2*j], C21=W2[2*j+1];
			Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1);
			Acc2 = gap_sumdotp4(V0, C20, Acc2); Acc2 = gap_sumdotp4(V1, C21, Acc2);
		}
		if (InDim&0x4) {
			Acc1 = gap_sumdotp4(VectIn[InDim/4-1], W1[InDim/4-1], Acc1);
			Acc2 = gap_sumdotp4(VectIn[InDim/4-1], W2[InDim/4-1], Acc2);
		}
		for (int j=4*(InDim/4); j<InDim; j++) {
			Acc1 += In[j]*Weights[(line1)*InDim+j];
			Acc2 += In[j]*Weights[(line2)*InDim+j];
		}
		Acc1 = AT_SCALE(Acc1, Scale[line1], ScaleN[line1]); ACT_SWITCH(Acc1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Acc2 = AT_SCALE(Acc2, Scale[line2], ScaleN[line2]); ACT_SWITCH(Acc2, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Out[line1] = gap_clip(Acc1, 7);
		Out[line2] = gap_clip(Acc2, 7);
	}
	if (Iter&0x1) {
		v4s * __restrict__ W1 = (v4s *) (&Weights[(Last-1)*InDim]);
		int Acc1 = AT_LSHIFT(Bias[Last-1], NormBias);
		for (int j=0; j<(InDim/(4*2)); j++) {
			v4s V0=VectIn[2*j], V1=VectIn[2*j+1];
			v4s C10=W1[2*j], C11=W1[2*j+1];
			Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1);
		}
		if (InDim&0x4) {
			Acc1 = gap_sumdotp4(VectIn[InDim/4-1], W1[InDim/4-1], Acc1);
		}
		for (int j=4*(InDim/4); j<InDim; j++) {
			Acc1 += In[j]*Weights[(Last-1)*InDim+j];
		}
		Acc1 = AT_SCALE(Acc1, Scale[Last-1], ScaleN[Last-1]); ACT_SWITCH(Acc1, Activation, ActScale, ActScaleN, A0, B0, C0, 0, 0);
		Out[Last-1] = gap_clip(Acc1, 7);
	}
	gap_waitbarrier(0);
}*/

void KerParLinearLayerFullFeatB32_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_NONE);
}

void KerParLinearLayerFullFeatB32_ReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_RELU);
}

void KerParLinearLayerFullFeatB32_ReLUN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParLinearLayerFullFeatB32_ReLUM_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_RELUM);
}

void KerParLinearLayerFullFeatB32_ReLUMN_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_RELUMN);
}

void KerParLinearLayerFullFeatB32_LeakyReLU_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_LEAKYRELU);
}

void KerParLinearLayerFullFeatB32_HSigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_HSIGMOID);
}

void KerParLinearLayerFullFeatB32_HSwish_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_HSWISH);
}

void KerParLinearLayerFullFeatB32_Sigmoid_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_SIGMOID);
}

void KerParLinearLayerFullFeatB32_Tanh_SQ8(KerLinear_SQ8_T *Arg) {
	KerParLinearLayerFullFeatB32_SQ8_act(Arg, ACT_TANH);
}

#pragma GCC diagnostic pop
