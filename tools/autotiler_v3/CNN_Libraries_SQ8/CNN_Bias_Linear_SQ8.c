#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

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
void KerParLinearLayerFullFeatB8_SQ8(KerLinear_SQ8_T *Arg)

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
		Out[i] = gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7);
	}
	gap_waitbarrier(0);
}


void KerParLinearLayerFullFeatB8_ReLU_SQ8(KerLinear_SQ8_T *Arg)

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
		Out[i] = Max(0, gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7));
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerFullFeatB8_ReLUN_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const signed char * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

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
		Out[i] = Max(0, Min(A0, AT_SCALE(Acc, Scale[i], ScaleN[i])));
	}
	gap_waitbarrier(0);
}

/* 16b Bias */
void KerParLinearLayerFullFeatB16_SQ8(KerLinear_SQ8_T *Arg)

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
		Out[i] = gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7);
	}
	gap_waitbarrier(0);
}


void KerParLinearLayerFullFeatB16_ReLU_SQ8(KerLinear_SQ8_T *Arg)

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
		Out[i] = Max(0, gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7));
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerFullFeatB16_ReLUN_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

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
		Out[i] = Max(0, Min(A0, AT_SCALE(Acc, Scale[i], ScaleN[i])));
	}
	gap_waitbarrier(0);
}

/* 32b Bias */
void KerParLinearLayerFullFeatB32_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

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
		Out[i] = gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7);
	}
	gap_waitbarrier(0);
}


void KerParLinearLayerFullFeatB32_ReLU_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

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
		Out[i] = Max(0, gap_clip(AT_SCALE(Acc, Scale[i], ScaleN[i]), 7));
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerFullFeatB32_ReLUN_SQ8(KerLinear_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int InDim = Arg->InDim, OutDim = Arg->OutDim;
	const signed char * __restrict__ Weights = Arg->Weights;
	const int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = ((unsigned char *)Arg->Infos)[AT_INF_BIASN];
	unsigned char *Scale = Arg->Scale;
	unsigned char *ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;

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
		Out[i] = Max(0, Min(A0, AT_SCALE(Acc, Scale[i], ScaleN[i])));
	}
	gap_waitbarrier(0);
}
