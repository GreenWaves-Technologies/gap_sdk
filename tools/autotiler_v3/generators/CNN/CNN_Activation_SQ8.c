#include "Gap.h"
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

/*
 * Standalone activation
*/
static void Ker_Activation_SQ8(
        signed char * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
        CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<N/2; i++) {
                int Acc0 = In[2*i], Acc1 = In[2*i+1];
		switch (Activation) {
			case ACT_NONE:     Acc0 = AT_SCALE(Acc0, ActScale, ActScaleN); Acc1 = AT_SCALE(Acc1, ActScale, ActScaleN); break;
			case ACT_RELU:     Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Acc1), ActScale, ActScaleN); break;
			case ACT_RELUN:    Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN); Acc1 = AT_SCALE(Min(A0, Max(0, Acc1)), ActScale, ActScaleN); break;
			case ACT_HSIGMOID: Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN); break;
			case ACT_HSWISH:   Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN); break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
					int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
					int Acc1N = AT_NORM(Acc1 * A0, 7);
					Acc1 = AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN);
				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM((Acc0 * A0), 7):Acc0), ActScale, ActScaleN);
				//	Acc1 = AT_SCALE(((Acc1<0) ? AT_NORM((Acc1 * A0), 7):Acc1), ActScale, ActScaleN);
				}
				break;
		}
                Out[2*i] = gap_clip(Acc0, 7), Out[2*i+1] = gap_clip(Acc1, 7);
        }
	if (N&0x1) {
        	unsigned int i=N-1;
                int Acc0 = In[i];
		switch (Activation) {
			case ACT_NONE:     Acc0 = AT_SCALE(Acc0, ActScale, ActScaleN); break;
			case ACT_RELU:     Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN); break;
			case ACT_RELUN:    Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN); break;
			case ACT_HSIGMOID: Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN); break;
			case ACT_HSWISH:   Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN); break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM((Acc0 * A0), 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
                Out[i] = gap_clip(Acc0, 7);
	}
}

/*
 * Standalone activation variant with Scale = 1.0
*/
static void Ker_ActivationScale1_SQ8(
        signed char * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
        CNN_ActivationOper_T Activation,
	int A0
        )

{
        for (unsigned int i=0; i<N/2; i++) {
                int Acc0 = In[2*i], Acc1 = In[2*i+1];
		switch (Activation) {
			case ACT_RELU: Acc0 = Max(0, Acc0); Acc1 = Max(0, Acc1); break;
			case ACT_RELUN: Acc0 = Min(A0, Max(0, Acc0)); Acc1 = Min(A0, Max(0, Acc1)); break;
		}
                Out[2*i] = Acc0; Out[2*i+1] = Acc1;
        }
	if (N&0x1) {
        	unsigned int i=N-1;
                int Acc0 = In[i];
		switch (Activation) {
			case ACT_RELU: Acc0 = Max(0, Acc0); break;
			case ACT_RELUN: Acc0 = Min(A0, Max(0, Acc0)); break;
		}
                Out[i] = Acc0;
	}
}

/*
 * Conv/Linear DP scaling followed by an optional activation, Out buffer is different from In Buffer
*/
static void KerReduct_Activation_SQ8(
        int * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<N; i++) {
                int Acc0 = gap_clip(AT_SCALE(In[i], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
                Out[i] = gap_clip(Acc0, 7);
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, variant for ScaleAct=1.0, Out buffer is different from In Buffer
*/
static void KerReduct_ActivationScale1_SQ8(
        int * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<N; i++) {
                int Acc0 = gap_clip(AT_SCALE(Scale, In[i], ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				break;
		}
                Out[i] = Acc0;
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, In place version
 * Input is 32b int output is 8b
*/
static void KerReductIO_Activation_SQ8(
        signed char *__restrict__ Out,
        int *__restrict__ In,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<N; i++) {
                int Acc0 = gap_clip(AT_SCALE(In[i], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
                Out[i] = gap_clip(Acc0, 7);
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, variant for ActScale=1.0, In place version
 * Input is 32b int output is 8b
*/
static void KerReductIO_ActivationScale1_SQ8(
        signed char *__restrict__ Out,
        int *__restrict__ In,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<N; i++) {
                int Acc0 = gap_clip(AT_SCALE(In[i], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				break;
		}
                Out[i] = Acc0;
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, Out buffer is different from In Buffer
 * Partial unroll to avoid load use penalty
*/
static void _KerReduct_Activation_SQ8(
        int * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<(N/2); i++) {
                int Acc0 = gap_clip(AT_SCALE(In[2*i+0], Scale, ScaleN), 7);
                int Acc1 = gap_clip(AT_SCALE(In[2*i+1], Scale, ScaleN), 7);
		
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Acc1), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				Acc1 = AT_SCALE(Min(A0, Max(0, Acc1)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					int Acc1N = AT_NORM(Acc1 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
					Acc1 = AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN);

				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				//	Acc1 = AT_SCALE(((Acc1<0) ? AT_NORM(Acc1 * A0, 7):Acc1), ActScale, ActScaleN);
				}
				break;
		}
                Out[2*i] = gap_clip(Acc0, 7); Out[2*i+1] = gap_clip(Acc1, 7);
        }
        if (N&0x1) {
                int Acc0 = gap_clip(AT_SCALE(In[N-1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);

					// Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
                Out[N-1] = gap_clip(Acc0, 7);
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, variant for ActScale=1.0, Out buffer is different from In Buffer
 * Partial unroll to avoid load use penalty
*/
static void _KerReduct_ActivationScale1_SQ8(
        int * __restrict__ In,
        signed char * __restrict__ Out,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<(N/2); i++) {
                int Acc0 = gap_clip(AT_SCALE(In[2*i+0], Scale, ScaleN), 7);
                int Acc1 = gap_clip(AT_SCALE(In[2*i+1], Scale, ScaleN), 7);
		
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				Acc1 = Max(0, Acc1);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				Acc1 = Min(A0, Max(0, Acc1));
				break;
		}
                Out[2*i]   = Acc0; Out[2*i+1] = Acc1;
        }
        if (N&0x1) {
                int Acc0 = gap_clip(AT_SCALE(In[N-1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				break;
		}
                Out[N-1] = Acc0;
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, In place version
 * Input is 32b int output is 8b
 * Partially unrolled version to avoid load use penalty
*/
static void _KerReductIO_Activation_SQ8(
        signed char * __restrict__ Out,
        int *__restrict__ In,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<(N/2); i++) {
                int Acc0 = gap_clip(AT_SCALE(In[2*i+0], Scale, ScaleN), 7);
                int Acc1 = gap_clip(AT_SCALE(In[2*i+1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Acc1), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				Acc1 = AT_SCALE(Min(A0, Max(0, Acc1)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					int Acc1N = AT_NORM(Acc1 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
					Acc1 = AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN);

				//	Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				//	Acc1 = AT_SCALE(((Acc1<0) ? AT_NORM(Acc1 * A0, 7):Acc1), ActScale, ActScaleN);
				}
				break;
		}
                Out[2*i]   = gap_clip(Acc0, 7); Out[2*i+1] = gap_clip(Acc1, 7);
        }
        if (N&0x1) {
                int Acc0 = gap_clip(AT_SCALE(In[N-1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN);
				break;
			case ACT_RELUN:
				Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN);
				break;
			case ACT_HSIGMOID:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN);
				break;
			case ACT_HSWISH:
				Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN);
				break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);

					// Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM(Acc0 * A0, 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
                Out[N-1] = gap_clip(Acc0, 7);
        }
}

/*
 * Conv/Linear DP scaling followed by an optional activation, Variant for ActScale=1.0, In place version
 * Input is 32b int output is 8b
 * Partially unrolled version to avoid load use penalty
*/
static void _KerReductIO_ActivationScale1_SQ8(
        signed char *__restrict__ Out,
        int *__restrict__ In,
	unsigned int N,
	unsigned int Scale,
	unsigned int ScaleN,
        CNN_ActivationOper_T Activation,
	int A0, int B0, int C0
        )

{
        for (unsigned int i=0; i<(N/2); i++) {
                int Acc0 = gap_clip(AT_SCALE(In[2*i+0], Scale, ScaleN), 7);
                int Acc1 = gap_clip(AT_SCALE(In[2*i+1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				Acc1 = Max(0, Acc1);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				Acc1 = Min(A0, Max(0, Acc1));
				break;
		}
                Out[2*i]   = Acc0; Out[2*i+1] = Acc1;
        }
        if (N&0x1) {
                int Acc0 = gap_clip(AT_SCALE(In[N-1], Scale, ScaleN), 7);
		switch (Activation) {
			case ACT_NONE:
				break;
			case ACT_RELU:
				Acc0 = Max(0, Acc0);
				break;
			case ACT_RELUN:
				Acc0 = Min(A0, Max(0, Acc0));
				break;
		}
                Out[N-1] = Acc0;
        }
}

/*
 * Buffer compaction, scattered by chunk size groups of 8b moved to a contiguous representation through a parallel reduction tree
*/
void static __attribute__ ((noinline)) KerReductIO_Compact_SQ8(int *__restrict__ In, unsigned int Size, unsigned int CoreId, unsigned int ChunkCell)

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


/*
 * Input Scaling and reduction to 8b then channel cnetric activation, Out location != In location. Features are evaluated in parallel
*/
void KerParReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_ActivationScale1_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_NONE, A0, B0, C0);
	gap_waitbarrier(0);
}


void KerParReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_ActivationScale1_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_RELU, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerParReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_ActivationScale1_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_RELUN, A0, B0, C0);
	gap_waitbarrier(0);

}

void KerParReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_Activation_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerParReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_Activation_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerParReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	int S = Arg->Feat;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) KerReduct_Activation_SQ8(In+Size*c, Out+Size*c, Size, Scale[c], ScaleN[c], ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated in parallel
*/
extern void DumpFeaturePlanes(char *Mess, int DataSize, void *Plane, unsigned int NPlanes, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax);

void KerParReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_ActivationScale1_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_NONE, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

void KerParReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_ActivationScale1_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_RELU, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

void KerParReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_ActivationScale1_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_RELUN, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

void KerParReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_Activation_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

void KerParReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_Activation_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

void KerParReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat;
	unsigned int Size = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size);
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	S = Size*Max(0, Last-First);
	for (int c=First; c<Last; Out+=Size, c++) KerReductIO_Activation_SQ8(Out, In+Size*c, Size, Scale[c], ScaleN[c], ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
	KerReductIO_Compact_SQ8(In, S, CoreId, ChunkCell*Size);
}

/* Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated one after the other in parallel */
void KerReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_ActivationScale1_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_NONE, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_ActivationScale1_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_RELU, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_ActivationScale1_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_RELUN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_Activation_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_Activation_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) KerReduct_Activation_SQ8(In+S*c+First, Out+S*c+First, Size, Scale[c], ScaleN[c], ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

/* Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated one after the other in parallel */
void KerReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_ActivationScale1_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_NONE, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

void KerReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_ActivationScale1_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_RELU, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

void KerReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_ActivationScale1_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_RELUN, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

void KerReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_Activation_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

void KerReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_Activation_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

void KerReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg)

{
	unsigned int Feat = Arg->Feat;
	unsigned int S = Arg->W*Arg->H;
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	int * __restrict__ InOut = (int *__restrict__) Arg->In;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=0; c<Feat; c++) {
		KerReductIO_Activation_SQ8((signed char *__restrict__)(InOut+S*c+First), InOut+S*c+First, Size, Scale[c], ScaleN[c], ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
		gap_waitbarrier(0);
		KerReductIO_Compact_SQ8(InOut+S*c, Size, CoreId, ChunkCell);
	}
	ChunkCell = ChunkSize(Feat); First = CoreId*ChunkCell; Last  = Min(First+ChunkCell, Feat); Size = S*Max(0, Last-First);
	KerReductIO_Compact_SQ8(InOut, Size, CoreId, ChunkCell*Size);
}

/*
 * Standalone Scaled Activation, Features are evaluated in parallel
*/

void KerPar_ReLU_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	unsigned int Size = Arg->W*Arg->H;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	if (ActScale) for (int c=First; c<Last; c++) Ker_Activation_SQ8(In + Size*c, Out + Size*c, Size, ACT_RELU, ActScale, ActScaleN, A0, B0, C0);
	else for (int c=First; c<Last; c++) Ker_ActivationScale1_SQ8(In + Size*c, Out + Size*c, Size, ACT_RELU, A0);
	gap_waitbarrier(0);
}

void KerPar_ReLUN_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	unsigned int Size = Arg->W*Arg->H;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	if (ActScale) for (int c=First; c<Last; c++) Ker_Activation_SQ8(In + Size*c, Out + Size*c, Size, ACT_RELUN, ActScale, ActScaleN, A0, B0, C0);
	else for (int c=First; c<Last; c++) Ker_ActivationScale1_SQ8(In + Size*c, Out + Size*c, Size, ACT_RELUN, A0);
	gap_waitbarrier(0);
}

void KerPar_HSigmoid_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	unsigned int Size = Arg->W*Arg->H;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) Ker_Activation_SQ8(In + Size*c, Out + Size*c, Size, ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerPar_HSwish_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	unsigned int Size = Arg->W*Arg->H;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) Ker_Activation_SQ8(In + Size*c, Out + Size*c, Size, ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void KerPar_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	unsigned int Size = Arg->W*Arg->H;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

	for (int c=First; c<Last; c++) Ker_Activation_SQ8(In + Size*c, Out + Size*c, Size, ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

/*
 * Standalone Scaled Activation, Features are evaluated one after the other in parallel
*/

void Ker_ReLU_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];


	if (ActScale) Ker_Activation_SQ8(In+First, Out+First, Size, ACT_RELU, ActScale, ActScaleN, A0, B0, C0);
	else Ker_ActivationScale1_SQ8(In+First, Out+First, Size, ACT_RELU, A0);
	gap_waitbarrier(0);
}

void Ker_ReLUN_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];


	if (ActScale) Ker_Activation_SQ8(In+First, Out+First, Size, ACT_RELUN, ActScale, ActScaleN, A0, B0, C0);
	else Ker_ActivationScale1_SQ8(In+First, Out+First, Size, ACT_RELUN, A0);
	gap_waitbarrier(0);
}

void Ker_HSigmoid_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];


	Ker_Activation_SQ8(In+First, Out+First, Size, ACT_HSIGMOID, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void Ker_HSwish_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];


	Ker_Activation_SQ8(In+First, Out+First, Size, ACT_HSWISH, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

void Ker_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg)

{
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S);
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos;
	unsigned int Size = Max(0, Last-First);
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];


	Ker_Activation_SQ8(In+First, Out+First, Size, ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}
