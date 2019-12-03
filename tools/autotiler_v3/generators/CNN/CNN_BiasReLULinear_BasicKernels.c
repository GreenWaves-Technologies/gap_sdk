#include "Gap.h"
#include "CNN_BasicKernels.h"

#define VOL volatile


#define Min(a, b)	       (((a)<(b))?(a):(b))
#define Max(a, b)	       (((a)>(b))?(a):(b))
#define Minu(a, b)	      (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

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
	KerParSetBias_fp		Features are shorts, output feature maps are evaluated in parallel (one per core)
	KerParSetBias_fps		Features are bytes, output feature maps are evaluated in parallel (one per core)
	KerParSetNormedBias_fp_fps	Features are short, bias is byte, << Norm applied to bias, output feature maps are evaluated in parallel (one per core)
	KerParSetNormedBias_fpd_fps     Features are int, bias is byte, << Norm applied to bias, output feature maps are evaluated in parallel (one per core)
	KerParSetNormedBias_fpd_fp	Features are int, bias is short, << Norm applied to bias, output feature maps are evaluated in parallel (one per core)

	KerSetBias_fp			Features are shorts, a single output feature map is evaluated in parallel on all cores
	KerSetBias_fps			Features are bytes, a single output feature map is evaluated in parallel on all cores
	KerSetNormedBias_fp_fps		Features are short, bias is byte, << Norm applied to bias, a single output feature map is evaluated in parallel on all cores
        KerSetNormedBias_fpd_fps        Features are int, bias is byte, << Norm applied to bias, a single output feature map is evaluated in parallel on all cores
	KerSetNormedBias_fpd_fp		Features are int, bias is short, << Norm applied to bias, a single output feature map is evaluated in parallel on all cores
*/

void KerParSetBias_fp(KerSetBias_fp_T *Arg)

{
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Bias = Arg->Bias;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	for (unsigned int of=First; of<Last; of++) {
		v2s *LineOut = (v2s *) (Out+W*H*of);
		v2s B = (v2s) {Bias[of], Bias[of]};
		for (unsigned int i=0; i<((W*H)/4); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		for (unsigned int i=(4*((W*H)/4)); i<(W*H); i++) Out[W*H*of+i] = Bias[of];
	}
	gap_waitbarrier(0);
}

void KerParSetBias_fps(KerSetBias_fps_T *Arg)

{
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Bias = Arg->Bias;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) {
		v4s *LineOut = (v4s *) (Out+W*H*of);
		v4s B = (v4s) {Bias[of], Bias[of], Bias[of], Bias[of]};
		for (unsigned int i=0; i<((W*H)/8); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		for (unsigned int i=(8*((W*H)/8)); i<(W*H); i++) Out[W*H*of+i] = Bias[of];
	}
	gap_waitbarrier(0);
}


void KerParSetNormedBias_fp_fps(KerSetNormedBias_fp_fps_T *Arg)

{
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) {
		int b = Bias[of]<<Norm;
		v2s *LineOut = (v2s *) (Out+W*H*of);
		v2s B = (v2s) {b, b};
		for (unsigned int i=0; i<((W*H)/4); i++) {
			LineOut[2*i] = B; LineOut[2*i+1] = B;
		}
		for (unsigned int i=(4*((W*H)/4)); i<(W*H); i++) Out[W*H*of+i] = b;
	}
	gap_waitbarrier(0);
}

void KerParSetNormedBias_fpd_fps(KerSetNormedBias_fpd_fps_T *Arg)

{
        int * __restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        signed char * __restrict__ Bias = Arg->Bias;
        unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);

        for (unsigned int of=First; of<Last; of++) {
                int B = Bias[of]<<Norm;
                for (unsigned int i=0; i<((W*H)/2); i++) {
                        Out[W*H*of+2*i] = B; Out[W*H*of+2*i+1] = B;
                }
                Out[W*H*of + W*H-1] = B;
        }
        gap_waitbarrier(0);
}

void KerParSetNormedBias_fpd_fp(KerSetNormedBias_fpd_fp_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) {
		int B = Bias[of]<<Norm;
		for (unsigned int i=0; i<(W*H); i++) Out[W*H*of+i] = B;
	}
	gap_waitbarrier(0);
}

void KerSetBias_fp(KerSetBias_fp_T *Arg)

{
	short int * __restrict__ Out = Arg->Out;
	short int Bias = *Arg->Bias;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	v2s * __restrict__ VectOut = (v2s *) (Out+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
	if (Iter&0x1) Out[Last-1] = Bias;

	gap_waitbarrier(0);
}

void KerSetBias_fps(KerSetBias_fps_T *Arg)

{
	signed char * __restrict__ Out = Arg->Out;
	signed char Bias = *Arg->Bias;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	v4s * __restrict__ VectOut = (v4s *) (Out+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/4); i++) VectOut[i] = (v4s) {Bias, Bias, Bias, Bias};
	for (i=(Iter/4)*4; i<Iter; i++) Out[i] = Bias;

	gap_waitbarrier(0);
}

void KerSetNormedBias_fp_fps(KerSetNormedBias_fp_fps_T *Arg)

{
	short int * __restrict__ Out = Arg->Out;
	int Bias = (*Arg->Bias)<<Arg->Norm;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	v2s * __restrict__ VectOut = (v2s *) (Out+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
	if (Iter&0x1) Out[Last-1] = Bias;

	gap_waitbarrier(0);
}

void KerSetNormedBias_fpd_fps(KerSetNormedBias_fpd_fps_T *Arg)

{
        int * __restrict__ Out = Arg->Out;
        int Bias = (*Arg->Bias)<<Arg->Norm;
        int W = Arg->W;
        int H = Arg->H;
        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, W*H);
        int Iter = (Last-First);
        int i;

        for (i=0; i<(Iter/2); i++) {
                Out[2*i] = Bias; Out[2*i+1] = Bias;
        }
        Out[Last-1] = Bias;

        gap_waitbarrier(0);
}

void KerSetNormedBias_fpd_fp(KerSetNormedBias_fpd_fp_T *Arg)

{
	int * __restrict__ Out = Arg->Out;
	int Bias = (*Arg->Bias)<<Arg->Norm;
	int W = Arg->W;
	int H = Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);

	for (int i=First; i<Last; i++) Out[i] = Bias;

	gap_waitbarrier(0);
}

/* Linear rectification group:

	Feature maps in and out are bytes:
		KerParReLU_fps				Feature maps are evaluated in parallel, each core process one feature map
			|------	KerParDoReLU_fps
		KerReLU_fps				A single feature map is evaluated on all cores in parallel

	Feature maps in and out are short words:
		KerParReLU_fp				Feature maps are evaluated in parallel, each core process one feature map
			|------	KerParDoReLU_fp
		KerReLU_fp				A single feature map is evaluated on all cores in parallel
*/

static void KerParDoReLU_fp(
	short int * __restrict__ In,
	unsigned int W,
	unsigned int H,
	short int * __restrict__ Out,
	int LB,
	int UB
	)

{
	v2s * VectIn  = (v2s *) In;
	v2s * VectOut = (v2s *) Out;

	for (unsigned int i=0; i<((W*H)/4); i++) {
		v2s X = gap_min2(gap_max2(VectIn[2*i], ((v2s) {LB, LB})), ((v2s) {UB, UB}));
		v2s Y = gap_min2(gap_max2(VectIn[2*i+1], ((v2s) {LB, LB})), ((v2s) {UB, UB}));
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = Min(Max(In[i], LB), UB);
}

static void KerParDoReLU_fps(
	signed char * __restrict__ In,
	unsigned int W,
	unsigned int H,
	signed char * __restrict__ Out,
	int LB,
	int UB
	)

{
	v4s * VectIn  = (v4s *) In;
	v4s * VectOut = (v4s *) Out;

	for (unsigned int i=0; i<((W*H)/4); i++) {
		v4s X = gap_min4(gap_max4(VectIn[i], ((v4s) {LB, LB, LB, LB})), ((v4s) {UB, UB, UB, UB}));
		VectOut[i] = X;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = Min(Max(In[i], LB), UB);
}

void KerParReLU_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoReLU_fp(In+of*W*H, W, H, Out+of*Wo*Ho, LB, UB);

	gap_waitbarrier(0);
}

void KerParReLU_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerParDoReLU_fps(In+of*W*H, W, H, Out+of*Wo*Ho, LB, UB);

	gap_waitbarrier(0);
}

void KerReLU_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize((W*H)/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);
	v2s * VectIn  = (v2s *) In;
	v2s * VectOut = (v2s *) Out;
	int i, j;

	for (i=First; i<Last; i++) VectOut[i] = gap_min2(gap_max2(VectIn[i], ((v2s) {LB, LB})), ((v2s) {UB, UB}));
	if ((W*H)&0x1) Out[W*H-1] = Min(Max(In[W*H-1], LB), UB);
	gap_waitbarrier(0);
}

void KerReLU_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize((W*H)/4);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W*H)/4);
	v4s * VectIn  = (v4s *) In;
	v4s * VectOut = (v4s *) Out;
	int i, j;

	for (i=First; i<Last; i++) VectOut[i] = gap_min4(gap_max4(VectIn[i], ((v4s) {LB, LB, LB, LB})), ((v4s) {UB, UB, UB, UB}));
	for (i=((W*H)/4)*4; i<(W*H); i++) Out[i] = Min(Max(In[i], LB), UB);
	gap_waitbarrier(0);
}

/**************************************************************************************************************************************
	Linear Layer followed by optional linear rectification group
**************************************************************************************************************************************/

void KerLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const short int * __restrict__ Filter = Arg->Filter;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;
	short int * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	int LB = Arg->LB, UB = Arg->UB;
	static L1_CL_MEM int Reduct[8];

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize+First]);
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/2); j++) Acc = gap_sumdotp2(VectIn[j], Filt[j], Acc);
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<Norm;
			for (int j=0;j<gap_ncore();j++) Acc += Reduct[j];
			Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
       	gap_waitbarrier(0);
}

void KerLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const signed char * __restrict__ Filter = Arg->Filter;
	const signed char * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;
	signed char * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	int LB = Arg->LB, UB = Arg->UB;
	static L1_CL_MEM int Reduct[8];

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v4s * __restrict__ Filt = (v4s *) (&Filter[i*InSize+First]);
		v4s * __restrict__ VectIn = (v4s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/4); j++) Acc = gap_sumdotp4(VectIn[j], Filt[j], Acc);
		for (int j=4*(Iter/4); j<Iter; j++) Acc += In[First+j]*Filter[i*InSize+First+j];
		Reduct[CoreId] = Acc;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<Norm;
			for (int j=0;j<gap_ncore();j++) Acc += Reduct[j];
			Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
       	gap_waitbarrier(0);
}

void KerLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const signed char * __restrict__ Filter = Arg->Filter;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	short int * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	int LB = Arg->LB, UB = Arg->UB;
	static L1_CL_MEM int Reduct[8];

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/2); j++) {
			v2s F = gap_pack2(Filter[i*InSize+First+2*j], Filter[i*InSize+First+2*j+1]);
			Acc = gap_sumdotp2(VectIn[j], F, Acc);
		}
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<NormBias;
			for (int j=0;j<gap_ncore();j++) Acc += Reduct[j];
			Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
       	gap_waitbarrier(0);
}

void KerLinearLayerReLU_fps_fps_fp(KerLinearLayerReLU_fps_fps_fp_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const signed char * __restrict__ Filter = Arg->Filter;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	short int * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	int LB = Arg->LB, UB = Arg->UB;
	static L1_CL_MEM int Reduct[8];

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v4s * __restrict__ Filt = (v4s *) (&Filter[i*InSize+First]);
		v4s * __restrict__ VectIn = (v4s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/4); j++) Acc = gap_sumdotp4(VectIn[j], Filt[j], Acc);
		for (int j=4*(Iter/4); j<Iter; j++) Acc += In[First+j]*Filter[i*InSize+First+j];
		Reduct[CoreId] = Acc;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<NormBias;
			for (int j=0;j<gap_ncore();j++) Acc += Reduct[j];
			Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
		}
		gap_waitbarrier(0);

	}
	gap_waitbarrier(0);
}

void KerLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const short int * __restrict__ Filter = Arg->Filter;
	const short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	int LB = Arg->LB, UB = Arg->UB;
	static L1_CL_MEM int Reduct[8];

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize+First]);
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/2); j++) Acc = gap_sumdotp2(VectIn[j], Filt[j], Acc);
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<NormBias;
			for (int j=0;j<gap_ncore();j++) Acc += Reduct[j];
			Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
       	gap_waitbarrier(0);
}

void KerDPLinearLayer_fp(KerDPLinear_fp_T *Arg)

{
	short int * __restrict__ In = (short int *__restrict__) Arg->In;
	int InSize = Arg->InSize;
	short int * __restrict__ Filter = (short int *__restrict__) Arg->Filter;
	int * __restrict__ Out = (int *__restrict__) Arg->Out;
	int Tile = Arg->Tile;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	v2s * __restrict__ Filt = (v2s *) (&Filter[First]);
	v2s * __restrict__ VectIn = (v2s *) (&In[First]);
	int Acc = 0;
	if (Tile==0) Out[CoreId] = 0;
	for (int i=0; i<(Iter/4); i++) {
		Acc = gap_sumdotp2(VectIn[2*i],   Filt[2*i],   Acc);
		Acc = gap_sumdotp2(VectIn[2*i+1], Filt[2*i+1], Acc);
	}
	for (int i=(Iter/4)*4; i<Iter; i++) Acc += In[First+i]*Filter[First+i];
	Out[CoreId] += Acc;
}

typedef enum {
        KACT_NONE = 0,
        KACT_RELU,
        KACT_RELUN,
        KACT_HSIGMOID,
        KACT_HSWISH,
        KACT_LEAKY,
} CNN_ActivationOper_T;

void KerDPLinearLayerReduct_fp(KerDPLinearReduct_fp_T *Arg)

{
	int C1, C2;
	int * __restrict__ In = (int *__restrict__) Arg->In;
	int Acc = (*((short int *)Arg->Bias))<<Arg->NormBias;
	int Norm = Arg->Norm;
	for (int i=0; i<gap_ncore(); i++) Acc += In[i];
	switch (Arg->Oper) {
		case KACT_HSIGMOID:
			/* Out = Max(0, Min((In + 1)/2) */
			C1 = (1<<(2*Norm))-1;
			C2 = (1<<Norm)-1;
			Acc = (Acc+C1)>>1;
			Acc = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc, Norm)));
			break;
		case KACT_HSWISH:
			/* Out = (In * ReLU6(In + 3)) / 6 */
			C1 = 3<<(2*Norm);
			C2 = (1<<16)/6; // 1/6 in Q16
			Acc = gap_roundnorm_reg(gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc + C1, 0), Arg->UB), Norm) * Acc, Norm) * C2, 16);
			break;
		case KACT_LEAKY:
			/* Out = (In<0) ? In*Const : In */
			break;
		default:
			/* No Activation LB=-32768, UB=32767, ReLU: LB=0,UB=32767, ReLUN: LB=0,UB=N<<Norm */
			Acc = Min(Max(Acc, Arg->LB), Arg->UB);
	}
	*((short int *)Arg->Out) = Acc;
}

void KerDPLinearLayer_fps(KerDPLinear_fps_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	int InSize = Arg->InSize;
	signed char * __restrict__ Filter = (signed char *__restrict__) Arg->Filter;
	int * __restrict__ Out = (int *__restrict__) Arg->Out;
	int Tile = Arg->Tile;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(InSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	v4s * __restrict__ Filt = (v4s *) (&Filter[First]);
	v4s * __restrict__ VectIn = (v4s *) (&In[First]);
	int Acc = 0;
	if (Tile==0) Out[CoreId] = 0;
	for (int i=0; i<(Iter/8); i++) {
		Acc = gap_sumdotp4(VectIn[2*i],   Filt[2*i],   Acc);
		Acc = gap_sumdotp4(VectIn[2*i+1], Filt[2*i+1], Acc);
	}
	for (int i=(Iter/8)*8; i<Iter; i++) Acc += In[First+i]*Filter[First+i];
	Out[CoreId] += Acc;
}

void KerDPLinearLayerReduct_fps(KerDPLinearReduct_fps_T *Arg)

{
	int C1, C2;
	int * __restrict__ In = (int *__restrict__) Arg->In;
	int Acc = (*((signed char *)Arg->Bias))<<Arg->NormBias;
	int Norm = Arg->Norm;
	for (int i=0; i<gap_ncore(); i++) Acc += In[i];
	switch (Arg->Oper) {
		case KACT_HSIGMOID:
			/* Out = Max(0, Min((In + 1)/2) */
			C1 = (1<<(2*Norm))-1;
			C2 = (1<<Norm)-1;
			Acc = (Acc+C1)>>1;
			Acc = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc, Norm)));
			break;
		case KACT_HSWISH:
			/* Out = (In * ReLU6(In + 3)) / 6 */
			C1 = 3<<(2*Norm);
			C2 = (1<<16)/6; // 1/6 in Q16
			Acc = gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc + C1, 0), Arg->UB) * Acc, 2*Norm) * C2, 16);
			break;
		case KACT_LEAKY:
			/* Out = (In<0) ? In*Const : In */
			break;
		default:
			/* No Activation LB=-32768, UB=32767, ReLU: LB=0,UB=32767, ReLUN: LB=0,UB=N<<Norm */
			Acc = Min(Max(Acc, Arg->LB), Arg->UB);
	}
	*((signed char *)Arg->Out) = Acc;
}

void KerParLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int TotalInSize = Arg->TotalInSize;
	int InSize = Arg->InSize;
	const short int * __restrict__ Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	int OutSize = Arg->OutSize;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutSize);
	int i,j;
	v2s * __restrict__ VectIn = (v2s *) In;

	// Linear combination
	for (i=First; i<Last; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*TotalInSize]);
		int Acc = Out[i]<<Norm;
		for (j = 0; j<(InSize/2); j++) Acc = gap_sumdotp2(VectIn[j], Filt[j], Acc);
		if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
		Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
	}
	gap_waitbarrier(0);

}

void KerParLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	int TotalInSize = Arg->TotalInSize;
	int InSize = Arg->InSize;
	const signed char * __restrict__ Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	int OutSize = Arg->OutSize;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutSize);
	int i,j;
	v4s * __restrict__ VectIn = (v4s *) In;

	// Linear combination
	for (i=First; i<Last; i++) {
		v4s * __restrict__ Filt = (v4s *) (&Filter[i*TotalInSize]);
		int Acc = Out[i]<<Norm;
		for (j=0; j<(InSize/4); j++) Acc = gap_sumdotp4(VectIn[j], Filt[j], Acc);
		for (j=4*(InSize/4); j<InSize; j++) Acc += In[j]*Filter[i*TotalInSize+j];
		Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int TotalInSize = Arg->TotalInSize;
	int InSize = Arg->InSize;
	const signed char * __restrict__ Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	int OutSize = Arg->OutSize;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutSize);
	int i,j;
	v4s * __restrict__ VectIn = (v4s *) In;

	// Linear combination
	for (i=First; i<Last; i++) {
		v4s * __restrict__ Filt = (v4s *) (&Filter[i*TotalInSize]);
		int Acc = Out[i]<<Norm;
		for (j=0; j<(InSize/2); j++) {
			v2s F = gap_pack2(Filter[i*TotalInSize+2*j], Filter[i*TotalInSize+2*j+1]);
			Acc = gap_sumdotp2(VectIn[j], F, Acc);
		}
		if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
		Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	int TotalInSize = Arg->TotalInSize;
	int InSize = Arg->InSize;
	const short int * __restrict__ Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;

	int OutSize = Arg->OutSize;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(OutSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, OutSize);
	int i,j;
	v2s * __restrict__ VectIn = (v2s *) In;

	// Linear combination
	for (i=First; i<Last; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*TotalInSize]);
		int Acc = Out[i]<<Norm;
		for (j = 0; j<(InSize/2); j++) Acc = gap_sumdotp2(VectIn[j], Filt[j], Acc);
		if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
		Out[i] = Min(Max(gap_roundnorm_reg(Acc, Norm), LB), UB);
	}
	gap_waitbarrier(0);

}


/**************************************************************************************************************************************
	Double precision to single precision reduction
**************************************************************************************************************************************/

void KerDP_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int Norm = Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int * __restrict__ I = &In[First];
	v2s * __restrict__ O = (v2s *)(&Out[First]);

	for (i=0; i<(Size/2); i++) {
		int Acc0 = I[2*i], Acc1 = I[2*i+1];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1, Norm), LB), UB);
		O[i] = gap_pack2(Acc0, Acc1);
	}
	Out[Last-1] = Min(Max(gap_roundnorm_reg(I[Size-1], Norm), LB), UB);
	gap_waitbarrier(0);
}

void KerDPMulBiasScalar_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int M = *Arg->MulBias;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int * __restrict__ I = &In[First];
	v2s * __restrict__ O = (v2s *)(&Out[First]);

	for (i=0; i<(Size/2); i++) {
		int Acc0 = I[2*i], Acc1 = I[2*i+1];
		Acc0 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc0, Norm)*M, Norm), LB), UB);
		Acc1 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc1, Norm)*M, Norm), LB), UB);
		O[i] = gap_pack2(Acc0, Acc1);
	}
	Out[Last-1] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(I[Size-1], Norm)*M, Norm), LB), UB);
	gap_waitbarrier(0);
}

void KerDPMulBias_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int Size = Arg->W*Arg->H;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	short int * __restrict__ MB = Arg->MulBias;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);

	for (j=First; j<Last; j++) {
		int * __restrict__ I = &In[Size*j];
		v2s * __restrict__ O = (v2s *)(&Out[Size*j]);
		int M = MB[j];
		for (i=0; i<(Size/2); i++) {
			int Acc0 = I[2*i], Acc1 = I[2*i+1];
			Acc0 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc0, Norm)*M, Norm), LB), UB);
			Acc1 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc1, Norm)*M, Norm), LB), UB);
			O[i] = gap_pack2(Acc0, Acc1);
		}
		if (Size&0x1) Out[Size*j+Size-1] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(I[Size-1], Norm)*M, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerDP_hswish_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int Size = Arg->W*Arg->H;
	unsigned int Norm = Arg->Norm;
	int UB = Arg->UB;
	short int * __restrict__ MB = Arg->MulBias;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);

	int C1 = 3<<(2*Norm);
	int C2 = (1<<16)/6; // 1/6 in Q16

	for (j=First; j<Last; j++) {
		int * __restrict__ I = &In[Size*j];
		short int * __restrict__ O = (short int *)(&Out[Size*j]);
		for (i=0; i<Size; i++) {
			int Acc0 = I[i];
			O[i] = gap_roundnorm_reg(gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc0 + C1, 0), UB), Norm) * Acc0, Norm) * C2, 16);
		}
	}
	gap_waitbarrier(0);
}


void KerDP_hsigmoid_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int Size = Arg->W*Arg->H;
	int Norm = Arg->Norm;
	short int * __restrict__ MB = Arg->MulBias;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);

	int C1 = (1<<(2*Norm))-1;
	int C2 = (1<<Norm)-1;

	for (j=First; j<Last; j++) {
		int * __restrict__ I = &In[Size*j];
		short int * __restrict__ O = (short int *)(&Out[Size*j]);
		for (i=0; i<Size; i++) {
			int Acc0 = (I[i]+C1)>>1;
			// O[i] = Max(0, Min(C2, gap_roundnorm_reg(Acc0, Norm)));
			O[i] = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc0, Norm)));
		}
	}
	gap_waitbarrier(0);
}


void KerDP_IO_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int *I = &In[First];
	short int *Is = (short int *)In + First;
	v2s *O = (v2s *)(&In[First]);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (i=0; i<(Size/2); i++) {
		int Acc0 = I[2*i], Acc1 = I[2*i+1];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1, Norm), LB), UB);
		O[i] = gap_pack2(Acc0, Acc1);
	}
	if (Size&0x1) ((short int *)I)[Size-1] = Min(Max(gap_roundnorm_reg(I[Size-1], Norm), LB), UB);
	gap_waitbarrier(0);
	/* Now this is the reduction phase */
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			short int *__restrict__ OOs = ((short int *)In+(A*CoreId+B)*ChunkCell);
			short int *__restrict__ IIs = ((short int *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<(Size/4);i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/4)*4); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDP_IO_hsigmoid_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int *I = &In[First];
	short int *Is = (short int *)In + First;
	v2s *O = (v2s *)(&In[First]);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */

	int C1 = (1<<(2*Norm))-1;
	int C2 = (1<<Norm)-1;

	for (j=First; j<Last; j++) {
		int * __restrict__ I = &In[Size*j];
		short int * __restrict__ O = (short int *)(&Out[Size*j]);
		for (i=0; i<Size; i++) {
			int Acc0 = (I[i]+C1)>>1;
			// O[i] = Max(0, Min(C2, gap_roundnorm_reg(Acc0, Norm)));
			O[i] = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc0, Norm)));
		}
	}
	gap_waitbarrier(0);
	/* Now this is the reduction phase */
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			short int *__restrict__ OOs = ((short int *)In+(A*CoreId+B)*ChunkCell);
			short int *__restrict__ IIs = ((short int *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<(Size/4);i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/4)*4); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDP_IO_hswish_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int UB = Arg->UB;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int *I = &In[First];
	short int *Is = (short int *)In + First;
	short int *O = (short int *)(&In[First]);

	int C1 = 3<<(2*Norm);
	int C2 = (1<<16)/6; // 1/6 in Q16

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (i=0; i<Size; i++) {
		int Acc0 = I[i];
		O[i] = gap_roundnorm_reg(gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc0 + C1, 0), UB), Norm) * Acc0, Norm) * C2, 16);
	}
	gap_waitbarrier(0);
	/* Now this is the reduction phase */
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			short int *__restrict__ OOs = ((short int *)In+(A*CoreId+B)*ChunkCell);
			short int *__restrict__ IIs = ((short int *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<(Size/4);i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/4)*4); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDPMulBiasScalar_IO_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	int M = *Arg->MulBias;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	int *I = &In[First];
	short int *Is = (short int *)In + First;
	v2s *O = (v2s *)(&In[First]);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (i=0; i<(Size/2); i++) {
		int Acc0 = I[2*i], Acc1 = I[2*i+1];
		Acc0 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc0, Norm)*M, Norm), LB), UB);
		Acc1 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc1, Norm)*M, Norm), LB), UB);
		O[i] = gap_pack2(Acc0, Acc1);
	}
	if (Size&0x1) ((short int *)I)[Size-1] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(I[Size-1], Norm)*M, Norm), LB), UB);
	gap_waitbarrier(0);
	/* Now this is the reduction phase */
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			short int *__restrict__ OOs = ((short int *)In+(A*CoreId+B)*ChunkCell);
			short int *__restrict__ IIs = ((short int *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<(Size/4);i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/4)*4); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDPMulBias_IO_fp(KerDP_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	short int * __restrict__ MB = Arg->MulBias;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Arg->W*Arg->H;

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (j=First; j<Last; j++) {
		int *I = &In[Size*j];
		v2s *O = (v2s *)(&In[Size*j]);
		int M = MB[j];
		for (i=0; i<(Size/2); i++) {
			int Acc0 = I[2*i], Acc1 = I[2*i+1];
			Acc0 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc0, Norm)*M, Norm), LB), UB);
			Acc1 = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(Acc1, Norm)*M, Norm), LB), UB);
			O[i] = gap_pack2(Acc0, Acc1);
		}
		if (Size&0x1) ((short int *)I)[Size-1] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(I[Size-1], Norm)*M, Norm), LB), UB);
	}
	gap_waitbarrier(0);
	/* Now this is the reduction phase */
	ChunkCell *= Size;
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			short int *__restrict__ OOs = ((short int *)In+(A*CoreId+B)*ChunkCell);
			short int *__restrict__ IIs = ((short int *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<(Size/4);i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/4)*4); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDP_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int Norm = Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T * __restrict__ I = &In[First];
	v4s * __restrict__ O = (v4s *)(&Out[First]);
	signed char *__restrict__ Os = &Out[First];

	for (i=0; i<(Size/4); i++) {
		int Acc0 = I[4*i  ], Acc1 = I[4*i+1],
		    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1, Norm), LB), UB);
		Acc2 = Min(Max(gap_roundnorm_reg(Acc2, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3, Norm), LB), UB);
		O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
	}
	for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i], Norm), LB), UB);

	gap_waitbarrier(0);
}

void KerDP_hswish_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int UB = Arg->UB;
	unsigned int Norm = 2*Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T * __restrict__ I = &In[First];
	signed char * __restrict__ O = (signed char *)(&Out[First]);
	signed char *__restrict__ Os = &Out[First];
	int C1 = 3<<Norm;
	int C2 = (1<<16)/6; // 1/6 in Q16

	for (i=0; i<Size; i++) {
		int Acc0 = I[i];
		O[i] = gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc0 + C1, 0), UB) * Acc0, Norm) * C2, 16);
	}
	gap_waitbarrier(0);
}

void KerDP_hsigmoid_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int Size = Arg->W*Arg->H;
	unsigned int Norm = Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);

	int C1 = (1<<(2*Norm))-1;
	int C2 = (1<<Norm)-1;

	for (j=First; j<Last; j++) {
		DP_fps_T * __restrict__ I = &In[Size*j];
		signed char * __restrict__ O = (signed char *)(&Out[Size*j]);
		for (i=0; i<Size; i++) {
			int Acc0 = (I[i]+C1)>>1;
			// O[i] = Max(0, Min(C2, gap_roundnorm_reg(Acc0, Norm)));
			O[i] = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc0, Norm)));
		}
	}
	gap_waitbarrier(0);
}

void KerDPMulBiasScalar_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	int M = *Arg->MulBias;
	unsigned int Norm = 2*Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T * __restrict__ I = &In[First];
	v4s * __restrict__ O = (v4s *)(&Out[First]);
	signed char *__restrict__ Os = &Out[First];

	for (i=0; i<(Size/4); i++) {
		int Acc0 = I[4*i  ], Acc1 = I[4*i+1],
		    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0*M, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1*M, Norm), LB), UB);
		Acc2 = Min(Max(gap_roundnorm_reg(Acc2*M, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3*M, Norm), LB), UB);
		O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
	}
	for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i]*M, Norm), LB), UB);

	gap_waitbarrier(0);
}

void KerDPMulBias_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	signed char * __restrict__ MB = Arg->MulBias;
	unsigned int Norm = 2*Arg->Norm;
	int i, j;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Arg->W*Arg->H;


	for (j=First; j<Last; j++) {
		DP_fps_T * __restrict__ I = &In[Size*j];
		v4s * __restrict__ O = (v4s *)(&Out[Size*j]);
		signed char *__restrict__ Os = &Out[Size*j];
		int M = MB[j];
		for (i=0; i<(Size/4); i++) {
			int Acc0 = I[4*i  ], Acc1 = I[4*i+1],
			    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
			Acc0 = Min(Max(gap_roundnorm_reg(Acc0*M, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1*M, Norm), LB), UB);
			Acc2 = Min(Max(gap_roundnorm_reg(Acc2*M, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3*M, Norm), LB), UB);
			O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
		}
		for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i]*M, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerDP_IO_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T *I = &In[First];
	v4s *O = (v4s *)(&In[First]);
	signed char *Os = (signed char *) (In + First);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (i=0; i<(Size/4); i++) {
		int Acc0 = I[4*i], Acc1 = I[4*i+1],
		    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1, Norm), LB), UB);
		Acc2 = Min(Max(gap_roundnorm_reg(Acc2, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3, Norm), LB), UB);
		O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
	}
	for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i], Norm), LB), UB);

	gap_waitbarrier(0);

	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}


void KerDP_IO_hswish_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	int UB = Arg->UB;
	unsigned int Norm = 2*Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T *I = &In[First];
	signed char *O = (signed char *)(&In[First]);
	signed char *Os = (signed char *) (In + First);
	int C1 = 3<<Norm;
	int C2 = (1<<16)/6; // 1/6 in Q16

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */

	for (i=0; i<Size; i++) {
		int Acc0 = I[i];
		O[i] = gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(Acc0 + C1, 0), UB) * Acc0, Norm) * C2, 16);
	}
	gap_waitbarrier(0);

	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDP_IO_hsigmoid_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	unsigned int Norm = Arg->Norm;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T *I = &In[First];
	signed char *O = (signed char *)(&In[First]);
	signed char *Os = (signed char *) (In + First);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */

	int C1 = (1<<(2*Norm))-1;
	int C2 = (1<<Norm)-1;

	for (j=First; j<Last; j++) {
		DP_fps_T * __restrict__ I = &In[Size*j];
		signed char * __restrict__ O = (signed char *)(&Out[Size*j]);
		for (i=0; i<Size; i++) {
			int Acc0 = (I[i]+C1)>>1;
			// O[i] = Max(0, Min(C2, gap_roundnorm_reg(Acc0, Norm)));
			O[i] = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc0, Norm)));
		}
	}
	gap_waitbarrier(0);

	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDPMulBiasScalar_IO_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->W*Arg->H*Arg->InFeatures;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int M = *Arg->MulBias;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Last-First;

	DP_fps_T *I = &In[First];
	v4s *O = (v4s *)(&In[First]);
	signed char *Os = (signed char *) (In + First);

	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (i=0; i<(Size/4); i++) {
		int Acc0 = I[4*i], Acc1 = I[4*i+1],
		    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
		Acc0 = Min(Max(gap_roundnorm_reg(Acc0*M, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1*M, Norm), LB), UB);
		Acc2 = Min(Max(gap_roundnorm_reg(Acc2*M, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3*M, Norm), LB), UB);
		O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
	}
	for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i]*M, Norm), LB), UB);

	gap_waitbarrier(0);

	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}

void KerDPMulBias_IO_fps(KerDP_fps_T *Arg)

{
	DP_fps_T * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int S = Arg->InFeatures;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	signed char * __restrict__ MB = Arg->MulBias;
	int i,j,k,U,A,B,Log2Core;
       	unsigned int CoreId = gap_coreid();
       	unsigned int ChunkCell = ChunkSize(S);
       	unsigned int First = CoreId*ChunkCell;
       	unsigned int Last  = Min(First+ChunkCell, S);
	int Size = Arg->W*Arg->H;


	/* First normalize In, each parallel chunk overwrites it's own input
	   After we are done In contains groups of contiguous normalized values
	   each group beeing followed by an empty group of exactly the same size, these
	   one need to be supressed, second step is taking care of this reduction */
	for (j=First; j<Last; j++) {
		DP_fps_T *I = &In[Size*j];
		v4s *O = (v4s *)(&In[Size*j]);
		signed char *Os = (signed char *) (In + Size*j);
		int M = MB[j];
		for (i=0; i<(Size/4); i++) {
			int Acc0 = I[4*i], Acc1 = I[4*i+1],
		    	    Acc2 = I[4*i+2], Acc3 = I[4*i+3];
			Acc0 = Min(Max(gap_roundnorm_reg(Acc0*M, Norm), LB), UB); Acc1 = Min(Max(gap_roundnorm_reg(Acc1*M, Norm), LB), UB);
			Acc2 = Min(Max(gap_roundnorm_reg(Acc2*M, Norm), LB), UB); Acc3 = Min(Max(gap_roundnorm_reg(Acc3*M, Norm), LB), UB);
			O[i] = gap_pack4(Acc0, Acc1, Acc2, Acc3);
		}
		for (i=((Size/4)*4); i<Size; i++) Os[i] = Min(Max(gap_roundnorm_reg(I[i]*M, Norm), LB), UB);
	}

	gap_waitbarrier(0);

	ChunkCell *= Size;
	U = gap_ncore()/2; Log2Core = gap_fl1(gap_ncore()); A = 2; B = 1;
	for (k=0; k<Log2Core; k++) {
		if (CoreId<U) {
			signed char *__restrict__ OOs = ((signed char *)In+(A*CoreId+B)*ChunkCell);
			signed char *__restrict__ IIs = ((signed char *)In+(2*(A*CoreId+B))*ChunkCell);
			int *__restrict__ II = (int *) IIs;
			int *__restrict__ OO = (int *) OOs;
			for (i=0;i<Size/8;i++) {
				int V0 = II[2*i], V1 = II[2*i+1];
				OO[2*i] = V0; OO[2*i+1] = V1;
			}
			for (i=((Size/8)*8); i<Size; i++) OOs[i] = IIs[i];
		}
		U = U/2; A = A*2; B = B*2;
		gap_waitbarrier(0);
	}
}
