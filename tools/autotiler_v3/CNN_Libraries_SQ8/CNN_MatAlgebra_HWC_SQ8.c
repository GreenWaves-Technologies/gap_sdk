#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
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


	---------------- Feat -------------------
	|		Core0			|
	|_______________________________________|
	|					|
	H*W		Core1			|
	|_______________________________________|
	|					|
	|		...			|
	|_______________________________________|
	|					|
	|		...			|
	-----------------------------------------

*************************************************************************************************************************************************/

void KerParMatAdd_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
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

void KerParMatAdd_ReLU_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (ActScale) {
		if (In1Scale && OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), ActScale, ActScaleN), 7);		
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), ActScale, ActScaleN), 7);
		} else if (In1Scale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, ActScale, ActScaleN), 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1],ActScale, ActScaleN), 7);
		} else if (OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I10 + I20, OutScale, OutScaleN),ActScale, ActScaleN), 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I11 + I21, OutScale, OutScaleN),ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN),ActScale, ActScaleN), 7);
		} else {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(I10 + I20, ActScale, ActScaleN), 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(I11 + I21, ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(I1[S-1] + I2[S-1],ActScale, ActScaleN), 7);
		}
	} else {
		if (In1Scale && OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
		} else if (In1Scale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
		} else if (OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
				O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
		} else {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = AT_CLIP_POS_IMM(I10 + I20, 7);
				O[2*i+1] = AT_CLIP_POS_IMM(I11 + I21, 7);
			}
			if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(I1[S-1] + I2[S-1], 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_ReLUN_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];

	int A0 = Arg->Infos[AT_INF_A0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (ActScale) {
		if (In1Scale && OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
				O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
		} else if (In1Scale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, A0), ActScale, ActScaleN), 7);
				O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, A0), ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], A0), ActScale, ActScaleN), 7);
		} else if (OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I10 + I20, OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
				O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I11 + I21, OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), A0), ActScale, ActScaleN), 7);
		} else {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(I10 + I20, A0), OutScale, OutScaleN), 7);
				O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(I11 + I21, A0), OutScale, OutScaleN), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(I1[S-1] + I2[S-1], A0), OutScale, OutScaleN), 7);
		}
	} else {
		if (In1Scale && OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_CLIP_POS(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), A0), 7);
				O[2*i+1] = gap_clip(AT_CLIP_POS(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), A0), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_CLIP_POS(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), A0), 7);
		} else if (In1Scale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_CLIP_POS(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, A0), 7);
				O[2*i+1] = gap_clip(AT_CLIP_POS(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, A0), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_CLIP_POS(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], A0), 7);
		} else if (OutScale) {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_CLIP_POS(AT_SCALE(I10 + I20, OutScale, OutScaleN), A0), 7);
				O[2*i+1] = gap_clip(AT_CLIP_POS(AT_SCALE(I11 + I21, OutScale, OutScaleN), A0), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_CLIP_POS(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), A0), 7);
		} else {
			for (int i=0; i<S/2; i++) {
				int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
				O[2*i  ] = gap_clip(AT_CLIP_POS(I10 + I20, A0), 7);
				O[2*i+1] = gap_clip(AT_CLIP_POS(I11 + I21, A0), 7);
			}
			if (S&0x1) O[S-1] = gap_clip(AT_CLIP_POS(I1[S-1] + I2[S-1], A0), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_HSigmoid_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			int Acc1 = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(I10 + I20, 7);
			int Acc1 = gap_clip(I11 + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(I1[S-1] + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScale, ActScaleN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_HSwish_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (In1Scale && OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else if (In1Scale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, 7);
			int Acc1 = gap_clip(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else if (OutScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(AT_SCALE(I10 + I20, OutScale, OutScaleN), 7);
			int Acc1 = gap_clip(AT_SCALE(I11 + I21, OutScale, OutScaleN), 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(AT_SCALE(I1[S-1] + I2[S-1], OutScale, OutScaleN), 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			int Acc0 = gap_clip(I10 + I20, 7);
			int Acc1 = gap_clip(I11 + I21, 7);
			O[2*i  ] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
			O[2*i+1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScale, ActScaleN), 7);
		}
		if (S&0x1) {
			int Acc0 = gap_clip(I1[S-1] + I2[S-1], 7);
			O[S-1] = gap_clip(AT_SCALE(AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScale, ActScaleN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_LeakyReLU_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int SpatialSize 		= H*W;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(SpatialSize), First = Chunk*CoreId, Last = Min(First+Chunk, SpatialSize);
	unsigned int F = First*Arg->Feat, S = Max(0, Last*Arg->Feat-F);
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

#pragma GCC diagnostic pop
