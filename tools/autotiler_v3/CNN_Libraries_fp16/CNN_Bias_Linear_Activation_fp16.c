#include "Gap.h"
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

static F16 Fast_Sigmoid_fp16(F16 x)

{
        return ((F16)0.5f * (x / ((F16)1.0f + AbsF(x))) + (F16)0.5f);
}

static F16 Fast_Tanh_fp16(F16 x)

{
        return (((F16)2.0 * x) / ((F16) 1.0f + AbsF((F16)2.0 * x)));
}

static F16 Fast_Swish_fp16(F16 x)

{
	F16 Beta = (F16) 1.0;
        return (x * Fast_Sigmoid_fp16(Beta * x));
}

static F16 LeakyReLU_fp16(F16 x)

{
	if (x<0) x *= LEAK_CONSTANT;
	return x;
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
		KerParReLU_fp16				Feature maps are evaluated in parallel, each core process one feature map
			|------	KerParDoReLU_fp16
		KerReLU_fp16				A single feature map is evaluated on all cores in parallel
*/

static void KerParDoReLU_fp16(
	F16 * __restrict__ In,
	unsigned int W,
	unsigned int H,
	F16 * __restrict__ Out
	)

{
	F16V * VectIn  = (F16V *) In;
	F16V * VectOut = (F16V *) Out;
	F16V LB = (F16V) {0.0, 0.0};

	for (unsigned int i=0; i<((W*H)/4); i++) {
		F16V X = MaxF2(VectIn[2*i  ], LB);
		F16V Y = MaxF2(VectIn[2*i+1], LB);
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = MaxF(In[i], (F16)0.0);
}

static void KerParDoReLUN_fp16(
	F16 * __restrict__ In,
	unsigned int W,
	unsigned int H,
	F16 * __restrict__ Out,
	F16 UB
	)

{
	F16V * VectIn  = (F16V *) In;
	F16V * VectOut = (F16V *) Out;
	F16V LBv = (F16V) {0.0, 0.0};
	F16V UBv = (F16V) {UB, UB};

	for (unsigned int i=0; i<((W*H)/4); i++) {
		F16V X = MinF2(MaxF2(VectIn[2*i  ], LBv), UBv);
		F16V Y = MinF2(MaxF2(VectIn[2*i+1], LBv), UBv);
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = MinF(MaxF(In[i], (F16)0.0), UB);
}

static void KerParDoSwish_fp16(
	F16 * __restrict__ In,
	unsigned int W,
	unsigned int H,
	F16 * __restrict__ Out
	)

{
	for (unsigned int i=0; i<((W*H)/2); i++) {
		F16 Acc0 = In[2*i], Acc1 = In[2*i+1];
		Out[2*i]   = Fast_Swish_fp16(Acc0);
		Out[2*i+1] = Fast_Swish_fp16(Acc1);
	}
	if ((W*H)&0x1) {
		F16 Acc = In[W*H-1];
		Out[W*H-1] = Fast_Swish_fp16(Acc);
	}
}

static void KerParDoSigmoid_fp16(
	F16 * __restrict__ In,
	unsigned int W,
	unsigned int H,
	F16 * __restrict__ Out
	)

{
	for (unsigned int i=0; i<((W*H)/2); i++) {
		F16 Acc0 = In[2*i], Acc1 = In[2*i+1];
		Out[2*i]   = Fast_Sigmoid_fp16(Acc0);
		Out[2*i+1] = Fast_Sigmoid_fp16(Acc1);
	}
	if ((W*H)&0x1) {
		F16 Acc = In[W*H-1];
		Out[W*H-1] = Fast_Sigmoid_fp16(Acc);
	}
}

static void KerParDoLeakyReLU_fp16(
	F16 * __restrict__ In,
	unsigned int W,
	unsigned int H,
	F16 * __restrict__ Out
	)

{
	for (unsigned int i=0; i<((W*H)/2); i++) {
		F16 Acc0 = In[2*i], Acc1 = In[2*i+1];
		Out[2*i]   = LeakyReLU_fp16(Acc0);
		Out[2*i+1] = LeakyReLU_fp16(Acc1);
	}
	if ((W*H)&0x1) {
		F16 Acc = In[W*H-1];
		Out[W*H-1] = LeakyReLU_fp16(Acc);
	}
}

void KerParReLU_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoReLU_fp16(In+of*W*H, W, H, Out+of*Wo*Ho);

	gap_waitbarrier(0);
}

void KerParReLUN_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;
	F16 UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoReLUN_fp16(In+of*W*H, W, H, Out+of*Wo*Ho, UB);

	gap_waitbarrier(0);
}

void KerParSwish_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoSwish_fp16(In+of*W*H, W, H, Out+of*Wo*Ho);

	gap_waitbarrier(0);
}

void KerParSigmoid_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->Feat;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoSigmoid_fp16(In+of*W*H, W, H, Out+of*Wo*Ho);

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

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoLeakyReLU_fp16(In+of*W*H, W, H, Out+of*Wo*Ho);

	gap_waitbarrier(0);
}

void KerReLU_fp16(KerActivation_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	F16 * __restrict__ Out = Arg->Out;
	F16V LB = (F16V) {0.0, 0.0};

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize((W*H)/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);
	F16V * VectIn  = (F16V *) In;
	F16V * VectOut = (F16V *) Out;
	int i, j;

	for (i=First; i<Last; i++) VectOut[i] = MaxF2(VectIn[i], LB);
	if ((W*H)&0x1) Out[W*H-1] = MaxF(In[W*H-1], (F16) 0.0);
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = Fast_Swish_fp16(A);
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		Out[i] = Fast_Sigmoid_fp16(A);
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
		if (InDim&0x2) Acc += VectIn[InDim/2] * Filt[InDim/2];
		F16 A = Acc[0]+Acc[1];
		if (InDim&0x1) A += In[InDim-1]*Filter[i*InDim+InDim-1];
		if (Init) A += Bias[i]; else A += Out[i];
		if (A<0.0) A *= LEAK_CONSTANT;
		Out[i] = A;
	}
	gap_waitbarrier(0);
}

