#include "Gap8.h"
#include "CNN_BasicKernels.h"

#define VOL volatile


#define Min(a, b)               (((a)<(b))?(a):(b))
#define Max(a, b)               (((a)>(b))?(a):(b))
#define Minu(a, b)              (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

static int CoreCountDynamic = 1;
static int ActiveCore = gap8_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}



/* Addition basic kernel followed by optional Relu
 *
 */

void KerAddReLU_fp(KerAddReLU_fp_T *Arg)

{
        short int * __restrict__ In1 = Arg->In1;
        short int * __restrict__ In2 = Arg->In2;
        int W = Arg->W;
        int H = Arg->H;
        short int * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W*H)/2);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);
        unsigned int i, j;
        short int M1, M2;

        for (i=First; i<Last; i++){
            M1 =  gap8_clip(In1[i*2]+ In2[i*2],15);
            M2 =  gap8_clip(In1[i*2+1]+ In2[i*2+1],15);
            
            Out[i*2]   =  Max(M1,0);
            Out[i*2+1] =  Max(M2,0);
        }
        
        if((W*H)&0x1) Out[W*H-1] = Max(gap8_clip(In1[W*H-1] + In2[W*H-1],15),0);

}

void KerAdd_fp(KerAddReLU_fp_T *Arg)

{
        short int * __restrict__ In1 = Arg->In1;
        short int * __restrict__ In2 = Arg->In2;
        int W = Arg->W;
        int H = Arg->H;
        short int * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W*H)/2);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);

        unsigned int i, j;
        short int M1, M2;

        for (i=First; i<Last; i++){
    
            M1 =  gap8_clip(In1[i*2]+ In2[i*2],15);
            M2 =  gap8_clip(In1[i*2+1]+ In2[i*2+1],15);

            Out[i*2]   =  M1;
            Out[i*2+1] =  M2;
        }
        
        if((W*H)&0x1) Out[W*H-1] = gap8_clip(In1[W*H-1] + In2[W*H-1],15);

}

/* Set output features maps initial bias group
	KerParSetBias_fp		Features are shorts, output feature maps are evaluated in parallel (one per core)
	KerParSetBias_fps		Features are bytes, output feature maps are evaluated in parallel (one per core)
	KerParSetNormedBias_fp_fps	Features are short, bias is byte, << Norm applied to bias, output feature maps are evaluated in parallel (one per core)
	KerParSetNormedBias_fpd_fp	Features are int, bias is short, << Norm applied to bias, output feature maps are evaluated in parallel (one per core)

	KerSetBias_fp			Features are shorts, a single output feature map is evaluated in parallel on all cores
	KerSetBias_fps			Features are bytes, a single output feature map is evaluated in parallel on all cores
	KerSetNormedBias_fp_fps		Features are short, bias is byte, << Norm applied to bias, a single output feature map is evaluated in parallel on all cores
	KerSetNormedBias_fpd_fp		Features are int, bias is short, << Norm applied to bias, a single output feature map is evaluated in parallel on all cores
*/

void KerParSetBias_fp(KerParSetBias_fp_T *Arg)

{
        short int * __restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        short int * __restrict__ Bias = Arg->Bias;

        unsigned int CoreId = gap8_coreid();
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
        gap8_waitbarrier(0);
}

void KerParSetBias_fps(KerParSetBias_fps_T *Arg)

{
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        signed char * __restrict__ Bias = Arg->Bias;

        unsigned int CoreId = gap8_coreid();
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
        gap8_waitbarrier(0);
}


void KerParSetNormedBias_fp_fps(KerParSetNormedBias_fp_fps_T *Arg)

{
    short int * __restrict__ Out = Arg->Out;
    unsigned int W = Arg->W;
    unsigned int H = Arg->H;
    unsigned int OutFeatures = Arg->OutFeatures;
    signed char * __restrict__ Bias = Arg->Bias;
    unsigned int Norm = Arg->Norm;

    unsigned int CoreId = gap8_coreid();
    unsigned int Chunk = ChunkSize(OutFeatures);
    unsigned int First = Chunk*CoreId;
    unsigned int Last = Min(First+Chunk, OutFeatures);

    for (unsigned int of=First; of<Last; of++) {
            v2s *LineOut = (v2s *) (Out+W*H*of);
            v2s B = (v2s) {Bias[of]<<Norm, Bias[of]<<Norm};
            for (unsigned int i=0; i<((W*H)/4); i++) {
                    LineOut[2*i] = B; LineOut[2*i+1] = B;
            }
            for (unsigned int i=(4*((W*H)/4)); i<(W*H); i++) Out[W*H*of+i] = Bias[of];
    }
    gap8_waitbarrier(0);
}

void KerParSetNormedBias_fpd_fp(KerParSetNormedBias_fpd_fp_T *Arg)

{
        int * __restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        short int * __restrict__ Bias = Arg->Bias;
	unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);

        for (unsigned int of=First; of<Last; of++) {
		int B = Bias[of]<<Norm;
		for (unsigned int i=0; i<(W*H); i++) Out[W*H*of+i] = B;
        }
        gap8_waitbarrier(0);
}

void KerSetBias_fp(KerSetBias_fp_T *Arg)

{
        short int * __restrict__ Out = Arg->Out;
        short int Bias = Arg->Bias;
        int W = Arg->W;
        int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, W*H);
        v2s * __restrict__ VectOut = (v2s *) (Out+First);
        int Iter = (Last-First);
        int i;

        for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
        if (Iter&0x1) Out[Last-1] = Bias;

        gap8_waitbarrier(0);
}

void KerSetBias_fps(KerSetBias_fps_T *Arg)

{
        signed char * __restrict__ Out = Arg->Out;
        signed char Bias = Arg->Bias;
        int W = Arg->W;
        int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, W*H);
        v4s * __restrict__ VectOut = (v4s *) (Out+First);
        int Iter = (Last-First);
        int i;

        for (i=0; i<(Iter/4); i++) VectOut[i] = (v4s) {Bias, Bias, Bias, Bias};
	for (i=(Iter/4)*4; i<Iter; i++) Out[i] = Bias;

        gap8_waitbarrier(0);
}

void KerSetNormedBias_fp_fps(KerSetNormedBias_fp_fps_T *Arg)

{
        short int * __restrict__ Out = Arg->Out;
        int Bias = Arg->Bias<<Arg->Norm;
        int W = Arg->W;
        int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, W*H);
        v2s * __restrict__ VectOut = (v2s *) (Out+First);
        int Iter = (Last-First);
        int i;

        for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
        if (Iter&0x1) Out[Last-1] = Bias;

        gap8_waitbarrier(0);
}

void KerSetNormedBias_fpd_fp(KerSetNormedBias_fpd_fp_T *Arg)

{
        int * __restrict__ Out = Arg->Out;
        int Bias = Arg->Bias<<Arg->Norm;
        int W = Arg->W;
        int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, W*H);

	for (unsigned int i=First; i<Last; i++) Out[i] = Bias;

        gap8_waitbarrier(0);
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
	short int * __restrict__ Out
	)
{
    v2s * VectIn  = (v2s *) In;
    v2s * VectOut = (v2s *) Out;

	for (unsigned int i=0; i<((W*H)/4); i++) {
		v2s X = gap8_max2(VectIn[2*i], ((v2s) {0, 0}));
		v2s Y = gap8_max2(VectIn[2*i+1], ((v2s) {0, 0}));
		VectOut[2*i] = X; VectOut[2*i+1] = Y;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = Max(In[i], 0);
}

static void KerParDoReLU_fps(
	signed char * __restrict__ In,
	unsigned int W,
	unsigned int H,
	signed char * __restrict__ Out
	)

{
        v4s * VectIn  = (v4s *) In;
        v4s * VectOut = (v4s *) Out;

	for (unsigned int i=0; i<((W*H)/4); i++) {
		v4s X = gap8_max4(VectIn[i], ((v4s) {0, 0, 0, 0}));
		VectOut[i] = X;
	}
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = Max(In[i], 0);
}

void KerParReLU_fp(KerParReLUPool_fp_T *Arg)

{
    short int * __restrict__ In = Arg->In;
    unsigned int W = Arg->W;
    unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
    unsigned int OutFeatures = Arg->OutFeatures;
    short int * __restrict__ Out = Arg->Out;

    unsigned int CoreId = gap8_coreid();
    unsigned int Chunk = ChunkSize(OutFeatures);
    unsigned int First = Chunk*CoreId;
    unsigned int Last = Min(First+Chunk, OutFeatures);

    for (unsigned int of=First; of<Last; of++) KerParDoReLU_fp(In+of*W*H, W, H, Out+of*Wo*Ho);

    gap8_waitbarrier(0);
}

void KerParReLU_fps(KerParReLUPool_fps_T *Arg)

{
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
	unsigned int Wo = W;
	unsigned int Ho = H;
        unsigned int OutFeatures = Arg->OutFeatures;
        signed char * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk  = ChunkSize(OutFeatures);
        unsigned int First  = Chunk*CoreId;
        unsigned int Last   = Min(First+Chunk, OutFeatures);

        for (unsigned int of=First; of<Last; of++) KerParDoReLU_fps(In+of*W*H, W, H, Out+of*Wo*Ho);

        gap8_waitbarrier(0);
}

void KerReLU_fp(KerReLUPool_fp_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        int W = Arg->W;
        int H = Arg->H;
        short int * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W*H)/2);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);
        v2s * VectIn  = (v2s *) In;
        v2s * VectOut = (v2s *) Out;
        unsigned int i, j;

        for (i=First; i<Last; i++) VectOut[i] = gap8_max2(VectIn[i], ((v2s) {0, 0}));
        if ((W*H)&0x1) Out[W*H-1] = Max(In[W*H-1], 0);
        gap8_waitbarrier(0);
}

void KerReLU_fps(KerReLUPool_fps_T *Arg)

{
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W*H)/4);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Minu(First+ChunkCell, (W*H)/4);
        v4s * VectIn  = (v4s *) In;
        v4s * VectOut = (v4s *) Out;
        unsigned int i, j;

        for (i=First; i<Last; i++) VectOut[i] = gap8_max4(VectIn[i], ((v4s) {0, 0, 0, 0}));
	for (i=((W*H)/4)*4; i<(W*H); i++) Out[i] = Max(In[i], 0);
        gap8_waitbarrier(0);
}

/**************************************************************************************************************************************
	Linear Layer followed by optional linear rectification group
**************************************************************************************************************************************/

void KerLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg)

{
    short int * __restrict__ In = Arg->In;
    unsigned int InSize = Arg->InSize;
    const short int * __restrict__ Filter = Arg->Filter;
    const short int * __restrict__ Bias = Arg->Bias;
    unsigned int Norm = Arg->Norm;
    short int * __restrict__ Out = Arg->Out;
    int OutSize = Arg->OutSize;
	int ReVal = Arg->DoReLU?0:0x80000000;
	static L1_CL_MEM int Reduct[8];

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(InSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, InSize);
	unsigned int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize+First]);
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (unsigned int j=0; j<(Iter/2); j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
        	gap8_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<Norm;
			for (int j=0;j<gap8_ncore();j++) Acc += Reduct[j];
			Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
        	gap8_waitbarrier(0);
	}
       	gap8_waitbarrier(0);
}

void KerLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg)

{
        signed char * __restrict__ In = Arg->In;
        unsigned int InSize = Arg->InSize;
        const signed char * __restrict__ Filter = Arg->Filter;
        const signed char * __restrict__ Bias = Arg->Bias;
        unsigned int Norm = Arg->Norm;
        signed char * __restrict__ Out = Arg->Out;
        int OutSize = Arg->OutSize;
	int ReVal = Arg->DoReLU?0:0x80000000;
	static L1_CL_MEM int Reduct[8];

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(InSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, InSize);
	int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v4s * __restrict__ Filt = (v4s *) (&Filter[i*InSize+First]);
		v4s * __restrict__ VectIn = (v4s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/4); j++) Acc = gap8_sumdotp4(VectIn[j], Filt[j], Acc);
		for (int j=4*(Iter/4); j<Iter; j++) Acc += In[First+j]*Filter[i*InSize+First+j];
		Reduct[CoreId] = Acc;
        	gap8_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<Norm;
			for (int j=0;j<gap8_ncore();j++) Acc += Reduct[j];
			Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
		}
        	gap8_waitbarrier(0);
	}
       	gap8_waitbarrier(0);
}

void KerLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        unsigned int InSize = Arg->InSize;
        const signed char * __restrict__ Filter = Arg->Filter;
        const short int * __restrict__ Bias = Arg->Bias;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
        short int * __restrict__ Out = Arg->Out;
        int OutSize = Arg->OutSize;
	int ReVal = Arg->DoReLU?0:0x80000000;
	static L1_CL_MEM int Reduct[8];

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(InSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, InSize);
	int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/2); j++) {
			v2s F = gap8_pack2(Filter[i*InSize+First+2*j], Filter[i*InSize+First+2*j+1]);
			Acc = gap8_sumdotp2(VectIn[j], F, Acc);
		}
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
        	gap8_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<NormBias;
			for (int j=0;j<gap8_ncore();j++) Acc += Reduct[j];
			Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
        	gap8_waitbarrier(0);
	}
       	gap8_waitbarrier(0);
}

void KerLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        unsigned int InSize = Arg->InSize;
        const short int * __restrict__ Filter = Arg->Filter;
        const short int * __restrict__ Bias = Arg->Bias;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
        int * __restrict__ Out = Arg->Out;
        int OutSize = Arg->OutSize;
	int ReVal = Arg->DoReLU?0:0x80000000;
	static L1_CL_MEM int Reduct[8];

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(InSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, InSize);
	int Iter = Last-First;

	for (int i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize+First]);
		v2s * __restrict__ VectIn = (v2s *) (&In[First]);
		int Acc = 0;
		for (int j=0; j<(Iter/2); j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
		if (Iter%2) Acc += In[Last-1]*Filter[i*InSize+Last-1];
		Reduct[CoreId] = Acc;
        	gap8_waitbarrier(0);
		if (CoreId==0) {
			Acc = Bias[i]<<NormBias;
			for (int j=0;j<gap8_ncore();j++) Acc += Reduct[j];
			Out[i] = Max(ReVal, gap8_roundnorm_reg(Acc, Norm));
		}
        	gap8_waitbarrier(0);
	}
       	gap8_waitbarrier(0);
}

void KerParLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        int TotalInSize = Arg->TotalInSize;
        unsigned int InSize = Arg->InSize;
        int InBase = Arg->InBase;
        const short int * __restrict__ Filter = Arg->Filter;
        unsigned int Norm = Arg->Norm;
        short int * __restrict__ Out = Arg->Out;
	int ReVal = Arg->DoReLU?0:0x80000000;

        unsigned int OutSize = Arg->OutSize;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(OutSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, OutSize);
        unsigned int i,j;
        v2s * __restrict__ VectIn = (v2s *) In;

        // Linear combination
        for (i=First; i<Last; i++) {
                v2s * __restrict__ Filt = (v2s *) (&Filter[i*TotalInSize+InBase]);
                int Acc = Out[i]<<Norm;
                for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
                if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InBase+InSize-1];
                Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
        }
        gap8_waitbarrier(0);

}

void KerParLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg)

{
        signed char * __restrict__ In = Arg->In;
        int TotalInSize = Arg->TotalInSize;
        unsigned int InSize = Arg->InSize;
        int InBase = Arg->InBase;
        const signed char * __restrict__ Filter = Arg->Filter;
        unsigned int Norm = Arg->Norm;
        signed char * __restrict__ Out = Arg->Out;
	int ReVal = Arg->DoReLU?0:0x80000000;


        unsigned int OutSize = Arg->OutSize;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(OutSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, OutSize);
        unsigned int i,j;
        v4s * __restrict__ VectIn = (v4s *) In;

        // Linear combination
        for (i=First; i<Last; i++) {
                v4s * __restrict__ Filt = (v4s *) (&Filter[i*TotalInSize+InBase]);
                int Acc = Out[i]<<Norm;
                for (j=0; j<(InSize/4); j++) Acc = gap8_sumdotp4(VectIn[j], Filt[j], Acc);
		for (j=4*(InSize/4); j<InSize; j++) Acc += In[j]*Filter[i*TotalInSize+InBase+j];
                Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
        }
        gap8_waitbarrier(0);
}

void KerParLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        int TotalInSize = Arg->TotalInSize;
        unsigned int InSize = Arg->InSize;
        int InBase = Arg->InBase;
        const signed char * __restrict__ Filter = Arg->Filter;
        unsigned int Norm = Arg->Norm;
        short int * __restrict__ Out = Arg->Out;
	int ReVal = Arg->DoReLU?0:0x80000000;


        unsigned int OutSize = Arg->OutSize;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(OutSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, OutSize);
        unsigned int i,j;
        v4s * __restrict__ VectIn = (v4s *) In;

        // Linear combination
        for (i=First; i<Last; i++) {
                v4s * __restrict__ Filt = (v4s *) (&Filter[i*TotalInSize+InBase]);
                int Acc = Out[i]<<Norm;
                for (j=0; j<(InSize/2); j++) {
			v2s F = gap8_pack2(Filter[i*TotalInSize+InBase+2*j], Filter[i*TotalInSize+InBase+2*j+1]);
			Acc = gap8_sumdotp2(VectIn[j], F, Acc);
		}
                if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InBase+InSize-1];
                Out[i] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
        }
        gap8_waitbarrier(0);
}

void KerParLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg)

{
        short int * __restrict__ In = Arg->In;
        int TotalInSize = Arg->TotalInSize;
        unsigned int InSize = Arg->InSize;
        int InBase = Arg->InBase;
        const short int * __restrict__ Filter = Arg->Filter;
        unsigned int Norm = Arg->Norm;
        int * __restrict__ Out = Arg->Out;
	int ReVal = Arg->DoReLU?0:0x80000000;

        unsigned int OutSize = Arg->OutSize;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(OutSize);
        unsigned int First = CoreId*ChunkCell;
        unsigned int Last  = Min(First+ChunkCell, OutSize);
        unsigned int i,j;
        v2s * __restrict__ VectIn = (v2s *) In;

        // Linear combination
        for (i=First; i<Last; i++) {
                v2s * __restrict__ Filt = (v2s *) (&Filter[i*TotalInSize+InBase]);
                int Acc = Out[i]<<Norm;
                for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
                if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InBase+InSize-1];
                Out[i] = Max(ReVal, gap8_roundnorm_reg(Acc, Norm));
        }
        gap8_waitbarrier(0);

}

/*
  Convolution related function. Set the convolution output to the bias value.
  Out:	short int * pointer
  W, H:	Size of the output
  Bias:	short int, bias value.
*/
void __attribute__ ((noinline)) KerSetInBias(KerSetInBiasT *Arg)

{
    short int * __restrict__ Out = Arg->Out;
    short int Bias = Arg->Bias;
    int W = Arg->W;
    int H = Arg->H;
    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize(W*H);
    unsigned int First = CoreId*ChunkCell;
    unsigned int Last  = Minu(First+ChunkCell, W*H);
    v2s * __restrict__ VectOut = (v2s *) (Out+First);
    int Iter = (Last-First);
    int i;

    for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
    if (Iter&0x1) Out[Last-1] = Bias;

    gap8_waitbarrier(0);
}

/*
  Convolution related function. Set two convolution outputs to their respective bias values.
  Out0:	short int * pointer, first output
  Out1:	short int * pointer, second output
  W, H:	Size of the output, outputs dimension
  Bias0:	short int, bias value for Out0.
  Bias1:	short int, bias value for Out1.
*/

void __attribute__ ((noinline)) KerSetInBias2(KerSetInBias2T *Arg)

{
    short int * __restrict__ Out0 = Arg->Out0;
    short int * __restrict__ Out1 = Arg->Out1;
    short int Bias0 = Arg->Bias0;
    short int Bias1 = Arg->Bias1;
    int W = Arg->W;
    int H = Arg->H;
    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize((W/2)*H);
    unsigned int First = CoreId*ChunkCell;
    unsigned int Last  = Minu(First+ChunkCell, (W/2)*H);
    v2s * __restrict__ VectOut0 = (v2s *) (Out0+First);
    v2s * __restrict__ VectOut1 = (v2s *) (Out1+First);
    int Iter = (Last-First);
    int i;

    for (i=0; i<(Iter/2); i++) {
        VectOut0[i] = (v2s) {Bias0, Bias0};
        VectOut1[i] = (v2s) {Bias1, Bias1};
    }
    if (Iter&0x1) {
        Out0[Last-1] = Bias0;
        Out1[Last-1] = Bias1;
    }

    gap8_waitbarrier(0);
}

/*
  Convolution related function. Set three convolution outputs to their respective bias values.
  Out0:	short int * pointer, first output
  Out1:	short int * pointer, second output
  Out2:	short int * pointer, third output
  W, H:	Size of the output, outputs dimension
  Bias0:	short int, bias value for Out0.
  Bias1:	short int, bias value for Out1.
  Bias2:	short int, bias value for Out2.
*/

void __attribute__ ((noinline)) KerSetInBias3(KerSetInBias3T *Arg)

{
    short int * __restrict__ Out0 = Arg->Out0;
    short int * __restrict__ Out1 = Arg->Out1;
    short int * __restrict__ Out2 = Arg->Out2;
    short int Bias0 = Arg->Bias0;
    short int Bias1 = Arg->Bias1;
    short int Bias2 = Arg->Bias2;
    int W = Arg->W;
    int H = Arg->H;
    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize((W/2)*H);
    unsigned int First = CoreId*ChunkCell;
    unsigned int Last  = Minu(First+ChunkCell, (W/2)*H);
    v2s * __restrict__ VectOut0 = (v2s *) (Out0+First);
    v2s * __restrict__ VectOut1 = (v2s *) (Out1+First);
    v2s * __restrict__ VectOut2 = (v2s *) (Out2+First);
    int Iter = (Last-First);
    int i;

    for (i=0; i<(Iter/2); i++) {
        VectOut0[i] = (v2s) {Bias0, Bias0};
        VectOut1[i] = (v2s) {Bias1, Bias1};
        VectOut2[i] = (v2s) {Bias2, Bias2};
    }
    if (Iter&0x1) {
        Out0[Last-1] = Bias0;
        Out1[Last-1] = Bias1;
        Out2[Last-1] = Bias2;
    }

    gap8_waitbarrier(0);
}
