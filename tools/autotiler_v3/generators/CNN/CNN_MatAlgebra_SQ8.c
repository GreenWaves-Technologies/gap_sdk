#include <stdio.h>
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


/*************************************************************************************************************************************************
	Tensor Addition with Input1 and Output scaling followed by optional activation
	Scaling is optional, no scaling is expressed using Scale=0
*************************************************************************************************************************************************/

void KerParMatAdd_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			O[2*i+1] = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
		}
		if (S&0x1) O[S-1] = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(I10 + I20, 7);
			O[2*i+1] = gap_clip(I11 + I21, 7);
		}
		if (S&0x1) O[S-1] = gap_clip(I1[S-1] + I2[S-1], 7);
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_ReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = Max(0, gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7));
			O[2*i+1] = Max(0, gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7));
		}
		if (S&0x1) O[S-1] = Max(0, gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7));
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = Max(0, gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7));
			O[2*i+1] = Max(0, gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7));
		}
		if (S&0x1) O[S-1] = Max(0, gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7));
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = Max(0, gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7));
			O[2*i+1] = Max(0, gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7));
		}
		if (S&0x1) O[S-1] = Max(0, gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7));
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = Max(0, gap_clip(I10 + I20, 7));
			O[2*i+1] = Max(0, gap_clip(I11 + I21, 7));
		}
		if (S&0x1) O[S-1] = Max(0, gap_clip(I1[S-1] + I2[S-1], 7));
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_ReLUN_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(Max(0, Min(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), A0)), 7);
			O[2*i+1] = gap_clip(Max(0, Min(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), A0)), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(Max(0, Min(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), A0)), 7);
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(Max(0, Min(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, A0)), 7);
			O[2*i+1] = gap_clip(Max(0, Min(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, A0)), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(Max(0, Min(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], A0)), 7);
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(Max(0, Min(AT_SCALE(I10 + I20, OutScale, OutScaleN), A0)), 7);
			O[2*i+1] = gap_clip(Max(0, Min(AT_SCALE(I11 + I21, OutScale, OutScaleN), A0)), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(Max(0, Min(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), A0)), 7);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clip(Max(0, Min(I10 + I20, A0)), 7);
			O[2*i+1] = gap_clip(Max(0, Min(I11 + I21, A0)), 7);
		}
		if (S&0x1) O[S-1] = gap_clip(Max(0, Min(I1[S-1] + I2[S-1], A0)), 7);
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_HSigmoid_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
		}
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			int Acc1 = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
		}
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
		}
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(I10 + I20, 7);
			int Acc1 = gap_clip(I11 + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(I1[S-1] + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_HSwish_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			int Acc1 = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(I10 + I20, 7);
			int Acc1 = gap_clip(I11 + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(I1[S-1] + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	unsigned int F = First*W*H, S = Max(0, Last*W*H-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[2*i  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
	       		int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			O[2*i+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[S-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
		}
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			int Acc1 = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[2*i  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
	       		int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			O[2*i+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[S-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
		}
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[2*i  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
	       		int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			O[2*i+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[S-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
		}
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(I10 + I20, 7);
			int Acc1 = gap_clip(I11 + I21, 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[2*i  ] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
	       		int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
			int Acc1N = AT_NORM(Acc1 * A0, 7);
			O[2*i+1] = gap_clip(AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(I1[S-1] + I2[S-1], 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
	       		int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[S-1] = gap_clip(AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN), 7);
		}
	}
	gap_waitbarrier(0);
}

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling, followed by optional activation: ReLU and ReLUN. Other activations are implemented using
	standalone activation kernels
	Used to implement 1x1 convolution including strided convolutions
*************************************************************************************************************************************************/

/* 	Byte Bias */
void KerParMatMulB8_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
	int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB8_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_SCALE(S, Sc, ScN), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Max(0, gap_clip(AT_SCALE(S, Sc, ScN), 7));
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(Max(0, Min(AT_SCALE(S, Sc, ScN), A0)), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

/* 	Half Word Bias */
void KerParMatMulB16_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        short int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        short int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        short int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
	int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB16_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_SCALE(S, Sc, ScN), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Max(0, gap_clip(AT_SCALE(S, Sc, ScN), 7));
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(Max(0, Min(AT_SCALE(S, Sc, ScN), A0)), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

/* 	Word Bias */
void KerParMatMulB32_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
	int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        for (Col=0; Col<W_In2/4; Col++) {
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+4*Col+0];
			int X1 = In2[i*W_In2+4*Col+1];
			int X2 = In2[i*W_In2+4*Col+2];
			int X3 = In2[i*W_In2+4*Col+3];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = X2; // In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = X3; // In2[i*W_In2+4*Col+3];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0, S2=S0, S3=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                                S2 = gap_sumdotp4(V0, VBuff2[i], S2);
                                S3 = gap_sumdotp4(V0, VBuff3[i], S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+(Line+OffLine)*W_Out+4*Col+0+OffCol)) = R;
                }
                gap_waitbarrier(0);
        }
	if (W_In2&0x2) {
		Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+2*Col+0];
			int X1 = In2[i*W_In2+2*Col+1];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+2*Col+0];
			BufferColIn2[i+1*H_In2] = X1; // In2[i*W_In2+2*Col+1];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                                S1 = gap_sumdotp4(V0, VBuff1[i], S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
	if (W_In2&0x1) {
		Col = W_In2-1;
                for (i=F;i<L; i++) {
			int X0 = In2[i*W_In2+1*Col+0];
			BufferColIn2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
		}
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
                        int S0 = (Bias[Line]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB32_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_SCALE(S, Sc, ScN), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Max(0, gap_clip(AT_SCALE(S, Sc, ScN), 7));
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

void KerParMatMulSxSyB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	int A0 = Arg->Infos[AT_INF_A0];

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			unsigned int Sc = Scale[Line], ScN = ScaleN[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(Max(0, Min(AT_SCALE(S, Sc, ScN), A0)), 7);
	       	}
		int nF = F+Sx;
		if (nF<Wi) {
			F = nF; At += Sx; L -= Sx; Oo++;
		} else {
			int d = Wi-F+(Sy-1)*Wi;
			F = 0; L -= d; At += d; Oo++;
		}
	       	gap_waitbarrier(0);
	}
}

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling for small first matrix in the product, goal is to improve parallelism in this specific situation
	Followed by optionnal activation; ReLU and ReLUN. Other activations are implemented using stand alone activation kernels.

	Used to implement 1x1 convolution with unit stride
   	In1 fits completly in shared L1, convolution weights
	In2 has been transposed before being used, convolution Features
	Parallelization scheme partition In2 along H_In2
*************************************************************************************************************************************************/

/* 	Byte Bias */
void KerParMatMulB8_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB8_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[l1*H_In2 + l2+1] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB8_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);
	int A0 = Arg->Infos[AT_INF_A0];

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
		}
	}
	gap_waitbarrier(0);
}

/* 	Half Word Bias */
void KerParMatMulB16_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB16_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[l1*H_In2 + l2+1] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB16_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);
	int A0 = Arg->Infos[AT_INF_A0];

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
		}
	}
	gap_waitbarrier(0);
}

/* 	Word Bias */
void KerParMatMulB32_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7), gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_SCALE(S1, Sc, ScN), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(AT_SCALE(S0, Sc, ScN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB32_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7)),
					  Max(0, gap_clip(AT_SCALE(S2, Sc, ScN), 7)), Max(0, gap_clip(AT_SCALE(S3, Sc, ScN), 7)));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
			Out[l1*H_In2 + l2+1] = Max(0, gap_clip(AT_SCALE(S1, Sc, ScN), 7));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = Max(0, gap_clip(AT_SCALE(S0, Sc, ScN), 7));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB32_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
	unsigned int NormBias = Arg->NormBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);
	int A0 = Arg->Infos[AT_INF_A0];

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7),
					  gap_clip(Max(0, Min(AT_SCALE(S2, Sc, ScN), A0)), 7), gap_clip(Max(0, Min(AT_SCALE(S3, Sc, ScN), A0)), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
		}
	}
	if (Iter&0x2) {
		int l2 = (4*(Iter/4)) + First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2);
		v4s *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c], V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(Max(0, Min(AT_SCALE(S1, Sc, ScN), A0)), 7);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1], V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0); S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			Out[l1*H_In2 + l2+0] = gap_clip(Max(0, Min(AT_SCALE(S0, Sc, ScN), A0)), 7);
		}
	}
	gap_waitbarrier(0);
}

/*************************************************************************************************************************************************
	Matrix by Vector Multiplication with optional Activation (all of them are supported)
*************************************************************************************************************************************************/

void KerParMatVectMul_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7);
				int P2 = gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7);
				int P2 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
		}
	gap_waitbarrier(0);
}

void KerParMatVectMul_ReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = Max(0, gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7));
				int P2 = Max(0, gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7));
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = Max(0, gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7));
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = Max(0, gap_clip(AT_SCALE(I10, I2, ScaleN), 7));
				int P2 = Max(0, gap_clip(AT_SCALE(I11, I2, ScaleN), 7));
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = Max(0, gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7));
		}
	gap_waitbarrier(0);
}

void KerParMatVectMul_ReLUN_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int A0				= Arg->Infos[AT_INF_A0];
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = Max(0, Min(AT_SCALE(I10*Scale, I2, ScaleN), A0));
				int P2 = Max(0, Min(AT_SCALE(I11*Scale, I2, ScaleN), A0));
				O[2*j  ] = gap_clip(P1, 7); O[2*j+1] = gap_clip(P2, 7);
			}
			O[W*H-1] = gap_clip(Max(0, Min(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), A0)), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = Max(0, Min(AT_SCALE(I10, I2, ScaleN), A0));
				int P2 = Max(0, Min(AT_SCALE(I11, I2, ScaleN), A0));
				O[2*j  ] = gap_clip(P1, 7); O[2*j+1] = gap_clip(P2, 7);
			}
			O[W*H-1] = gap_clip(Max(0, Min(AT_SCALE(I1[W*H-1], I2, ScaleN), A0)), 7);
		}
	gap_waitbarrier(0);
}

void KerParMatVectMul_HSigmoid_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int ActScale		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALE];
	unsigned int ActScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALEN];
	int A0				= Arg->Infos[AT_INF_A0];
	int B0				= Arg->Infos[AT_INF_B0];
	int C0				= Arg->Infos[AT_INF_C0];
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, Max(0, Min(A0, Acc1 + B0)) * C0, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0, ActScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, Max(0, Min(A0, Acc1 + B0)) * C0, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0, ActScaleN), 7);
		}
	gap_waitbarrier(0);
}

void KerParMatVectMul_HSwish_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int ActScale		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALE];
	unsigned int ActScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALEN];
	int A0				= Arg->Infos[AT_INF_A0];
	int B0				= Arg->Infos[AT_INF_B0];
	int C0				= Arg->Infos[AT_INF_C0];
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScaleN), 7);
		}
	gap_waitbarrier(0);
}

void KerParMatVectMul_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	unsigned int ActScale		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALE];
	unsigned int ActScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_ACTSCALEN];
	int A0				= Arg->Infos[AT_INF_A0];
	int B0				= Arg->Infos[AT_INF_B0];
	int C0				= Arg->Infos[AT_INF_C0];
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Arg->Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Arg->Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
				int Acc0N = AT_NORM(Acc0 * A0, 7);
				Acc0 = AT_SCALE(ActScale, (Neg0*Acc0N+Pos0*Acc0), ActScaleN);
				int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
				int Acc1N = AT_NORM(Acc1 * A0, 7);
				Acc1 = AT_SCALE(ActScale, (Neg1*Acc1N+Pos1*Acc1), ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, (Neg0*Acc0N+Pos0*Acc0), ActScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
				int Acc0N = AT_NORM(Acc0 * A0, 7);
				Acc0 = AT_SCALE(ActScale, (Neg0*Acc0N+Pos0*Acc0), ActScaleN);
				int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
				int Acc1N = AT_NORM(Acc1 * A0, 7);
				Acc1 = AT_SCALE(ActScale, (Neg1*Acc1N+Pos1*Acc1), ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
			int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
			int Acc0N = AT_NORM(Acc0 * A0, 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, (Neg0*Acc0N+Pos0*Acc0), ActScaleN), 7);
		}
	gap_waitbarrier(0);
}

/*************************************************************************************************************************************************
	Matrix Transposition, no scaling. Feature Parallel and Parallel Feature
*************************************************************************************************************************************************/

static void CNN_Transpose_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int H,
	unsigned int Ho_F,
	unsigned int Ho_L
	)
       
{
	int IterL = Ho_L-Ho_F;
	int IterW = Wo_L-Wo_F;
	for (int l=0; l<IterL/4; l++) {
		v4s *pV0 = (v4s *) (In + (Ho_F+4*l+0)*W + Wo_F);
		v4s *pV1 = (v4s *) (In + (Ho_F+4*l+1)*W + Wo_F);
		v4s *pV2 = (v4s *) (In + (Ho_F+4*l+2)*W + Wo_F);
		v4s *pV3 = (v4s *) (In + (Ho_F+4*l+3)*W + Wo_F);
		signed char *pO  = (Out + Ho_F + 4*l + Wo_F*H);
		for (int c=0; c<IterW/4; c++) {
			v4s X, Y;
			v4s A = pV0[c], B = pV1[c], C = pV2[c], D = pV3[c];
			v4s rA, rB, rC, rD;

			X  = __builtin_shuffle(A, B, (v4s) {0,4,1,5});
			Y  = __builtin_shuffle(C, D, (v4s) {0,4,1,5});
			rA = __builtin_shuffle(X, Y, (v4s) {0,1,4,5});
			rB = __builtin_shuffle(X, Y, (v4s) {2,3,6,7});

			X  = __builtin_shuffle(A, B, (v4s) {2,6,3,7});
			Y  = __builtin_shuffle(C, D, (v4s) {2,6,3,7});
			rC = __builtin_shuffle(X, Y, (v4s) {0,1,4,5});
			rD = __builtin_shuffle(X, Y, (v4s) {2,3,6,7});

			*((v4s *) (pO+(4*c+0)*H)) = rA;
			*((v4s *) (pO+(4*c+1)*H)) = rB;
			*((v4s *) (pO+(4*c+2)*H)) = rC;
			*((v4s *) (pO+(4*c+3)*H)) = rD;
		}
		for (int c=4*(IterW/4); c<IterW; c++) {
			int X0 = In[(Ho_F+4*l+0)*W + (Wo_F+c)], X1 = In[(Ho_F+4*l+1)*W + (Wo_F+c)], X2 = In[(Ho_F+4*l+2)*W + (Wo_F+c)], X3 = In[(Ho_F+4*l+3)*W + (Wo_F+c)];
			Out[Ho_F+4*l+0 + (Wo_F+c)*H] = X0; Out[Ho_F+4*l+1 + (Wo_F+c)*H] = X1; Out[Ho_F+4*l+2 + (Wo_F+c)*H] = X2; Out[Ho_F+4*l+3 + (Wo_F+c)*H] = X3;
		}
	}
	for (int l=4*(IterL/4); l<IterL; l++) {
		for (int c=0; c<IterW/2; c++) {
			signed char X0 = In[(Ho_F+l)*W + Wo_F+2*c+0], X1 = In[(Ho_F+l)*W + Wo_F+2*c+1];
			Out[Ho_F+l + (Wo_F+2*c+0)*H] = X0;
			Out[Ho_F+l + (Wo_F+2*c+1)*H] = X1;
		}
		if (IterW&0x1) Out[Ho_F+l+ (Wo_L-1)*H] = In[(Ho_F+l)*W+Wo_L-1];
	}
}

static void CNN_TransposeSxSy_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int Ho,
	unsigned int Ho_F,
	unsigned int Ho_L,
	unsigned int Sx,
	unsigned int Sy
	)
{
	for (int l=Ho_F; l<Ho_L; l++) {
		int IterW = Wo_L-Wo_F;
		for (int c=0; c<IterW/2; c++) {
			signed char X0 = In[Sy*l*W + Sx*(Wo_F+2*c+0)], X1 = In[Sy*l*W + Sx*(Wo_F+2*c+1)];
			Out[l + (Wo_F+2*c+0)*Ho] = X0;
			Out[l + (Wo_F+2*c+1)*Ho] = X1;
		}
		if (IterW&0x1) Out[l+ (Wo_L-1)*Ho] = In[Sy*l*W+Sx*(Wo_L-1)];
	}
}

void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_Transpose_Body_fps(In+W*H*f, Out+W*H*f, W, 0, W, H, 0, H);
	gap_waitbarrier(0);
}

void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_TransposeSxSy_Body_fps(In+W*H*f, Out+Wo*Ho*f, W, 0, Wo, Ho, 0, Ho, Sx, Sy);
	gap_waitbarrier(0);
}

void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (H>W) {
		/* Tile horizontally */
		Chunk = ChunkSize(H); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, H); Wo_F = 0; Wo_L = W;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(W); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, W); Ho_F = 0; Ho_L = H;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_Transpose_Body_fps(In+W*H*f, Out+W*H*f, W, Wo_F, Wo_L, H, Ho_F, Ho_L); 
	}
	gap_waitbarrier(0);
}

void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (Ho>Wo) {
		/* Tile horizontally */
		Chunk = ChunkSize(Ho); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, Ho); Wo_F = 0; Wo_L = Wo;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(Wo); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, Wo); Ho_F = 0; Ho_L = Ho;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_TransposeSxSy_Body_fps(In+W*H*f, Out+Wo*Ho*f, W, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy); 
	}
	gap_waitbarrier(0);
}

/*************************************************************************************************************************************************
	3D Tensor permutations, no scaling.
*************************************************************************************************************************************************/

void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W + h*W + (w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[c*H*W + (w+0)*H + h] = V0;
				Out[c*H*W + (w+1)*H + h] = V1;
			}
			if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*W*C + (w+0)*C + c] = V0;
				Out[h*W*C + (w+1)*C + c] = V1;
			}
			if (W&0x1) Out[h*W*C + (W-1)*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*H*C + h*C + c] = V0;
				Out[(w+1)*H*C + h*C + c] = V1;
			}
			if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*C*H + c*H + h] = V0;
				Out[(w+1)*C*H + c*H + h] = V1;
			}
			if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*C*W + c*W + (w+0)] = V0;
				Out[h*C*W + c*W + (w+1)] = V1;
			}
			if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

