#include <stdio.h>
#include "FastFloatApprox16.h"
#include "DSP_Lib.h"

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

void KerParMatAdd_Fix16(KerMatAdd_DSP_T *Arg)

{
	short int * __restrict__ In1 = (short int * __restrict__) Arg->In1;
	short int * __restrict__ In2 = (short int * __restrict__) Arg->In2;
	short int * __restrict__ Out = (short int * __restrict__) Arg->Out;
	int W		             = Arg->W;
	int H		             = Arg->H;
	int Norm 	             = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, W*H);
	int Iter = Last - First;
	int i, j;

	v2s * __restrict__ I1 = (v2s *) (In1 + First), * __restrict__ I2 = (v2s *) (In2 + First), * __restrict__ O  = (v2s *) (Out + First);
	for (i=0; i<Iter/2; i++) {
		O[i] = gap_clip(gap_roundnorm_reg((I1[i] + I2[i]), Norm), 15);
	}
	Out[First+Iter-1] = gap_clip(gap_roundnorm_reg((In1[First+Iter-1] + In2[First+Iter-1]), Norm), 15);
	gap_waitbarrier(0);
}


void KerParMatAdd_Fix32(KerMatAdd_DSP_T *Arg)

{
	int * __restrict__ In1 = (int * __restrict__) Arg->In1;
	int * __restrict__ In2 = (int * __restrict__) Arg->In2;
	int * __restrict__ Out = (int * __restrict__) Arg->Out;
	int W		       = Arg->W;
	int H		       = Arg->H;
	int Norm 	       = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, W*H);
	int Iter = Last - First;
	int i, j;

	for (i=First; i<Last; i++) {
		Out[i] = gap_clip(gap_roundnorm_reg((In1[i] + In2[i]), Norm), 31);
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_fp16(KerMatAdd_DSP_T *Arg)

{
	F16_DSP * __restrict__ In1 = (F16_DSP * __restrict__) Arg->In1;
	F16_DSP * __restrict__ In2 = (F16_DSP * __restrict__) Arg->In2;
	F16_DSP * __restrict__ Out = (F16_DSP * __restrict__) Arg->Out;
	int W			   = Arg->W;
	int H			   = Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, W*H);
	int Iter = Last - First;
	int i, j;

	F16V_DSP * __restrict__ I1 = (F16V_DSP *) (In1 + First), * __restrict__ I2 = (F16V_DSP *) (In2 + First), * __restrict__ O  = (F16V_DSP *) (Out + First);
	for (i=0; i<Iter/2; i++) {
		O[i] = I1[i] + I2[i];
	}
	Out[First+Iter-1] = In1[First+Iter-1] + In2[First+Iter-1];
	gap_waitbarrier(0);
}

void KerParMatAdd_fp32(KerMatAdd_DSP_T *Arg)

{
	float * __restrict__ In1 = (float * __restrict__) Arg->In1;
	float * __restrict__ In2 = (float * __restrict__) Arg->In2;
	float * __restrict__ Out = (float * __restrict__) Arg->Out;
	int W			 = Arg->W;
	int H			 = Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, W*H);
	int Iter = Last - First;
	int i, j;

	for (i=First; i<Last; i++) {
		Out[i] = In1[i] + In2[i];
	}
	gap_waitbarrier(0);
}
