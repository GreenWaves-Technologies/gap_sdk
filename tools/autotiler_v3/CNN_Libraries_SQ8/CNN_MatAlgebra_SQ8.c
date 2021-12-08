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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
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

void KerMatAdd_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	int S = Last-First;
	signed char * __restrict__ I1 = In1 + First, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + First;
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

void KerMatAdd_ReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int F = First, S = Max(0, Last-F);
	signed char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + F;

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

void KerMatAdd_ReLUN_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];

	int A0 = Arg->Infos[AT_INF_A0];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);

	unsigned int F = First, S = Max(0, Last-F);
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

void KerMatAdd_HSigmoid_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);

	unsigned int F = First, S = Max(0, Last-F);
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

void KerMatAdd_HSwish_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);

	unsigned int F = First, S = Max(0, Last-F);
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

void KerMatAdd_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	int A0 = Arg->Infos[AT_INF_A0], B0 = Arg->Infos[AT_INF_B0], C0 = Arg->Infos[AT_INF_C0];

	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int F = First, S = Max(0, Last-F);
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

void KerMatAdd_USQ8(KerMat3_SQ8_T *Arg)

{
	unsigned char * __restrict__ In1	= Arg->In1;
	unsigned char * __restrict__ In2	= Arg->In2;
	unsigned char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	short int AddBias = *((short int *)(((unsigned char *)Arg->Infos)+AT_INF_ADD_BIAS));
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	int S = Last-First;
	unsigned char * __restrict__ I1 = In1 + First, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + First;
	for (int i=0; i<S/2; i++) {
		int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
		O[2*i  ] = gap_clipu(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 8);
		O[2*i+1] = gap_clipu(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 8);
	}
	if (S&0x1) O[S-1] = gap_clipu(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 8);
	gap_waitbarrier(0);
}

void KerMatAdd_ReLU_USQ8(KerMat3_SQ8_T *Arg)

{
	unsigned char * __restrict__ In1	= Arg->In1;
	unsigned char * __restrict__ In2	= Arg->In2;
	unsigned char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	short int AddBias = *((short int *)(((unsigned char *)Arg->Infos)+AT_INF_ADD_BIAS));
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int F = First, S = Max(0, Last-F);
	unsigned char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + F;

	if (ActScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = AT_CLIP_POS_IMM(AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), ActScale, ActScaleN), 8);		
			O[2*i+1] = AT_CLIP_POS_IMM(AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), ActScale, ActScaleN), 8);
		}
		if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AddBias + AT_SCALE(AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), ActScale, ActScaleN), 8);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = AT_CLIP_POS_IMM(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), 8);
			O[2*i+1] = AT_CLIP_POS_IMM(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), 8);
		}
		if (S&0x1) O[S-1] = AT_CLIP_POS_IMM(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), 8);
	}
	gap_waitbarrier(0);
}

void KerMatAdd_ReLUN_USQ8(KerMat3_SQ8_T *Arg)

{
	unsigned char * __restrict__ In1	= Arg->In1;
	unsigned char * __restrict__ In2	= Arg->In2;
	unsigned char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	short int AddBias = *((short int *)(((unsigned char *)Arg->Infos)+AT_INF_ADD_BIAS));

	int A0 = Arg->Infos[AT_INF_A0];
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);

	unsigned int F = First, S = Max(0, Last-F);
	unsigned char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + F, *__restrict__ O  = Out + F;
	if (ActScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(AT_SCALE(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), A0), ActScale, ActScaleN), 8);
			O[2*i+1] = gap_clipu(AT_SCALE(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), A0), ActScale, ActScaleN), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(AT_SCALE(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), A0), ActScale, ActScaleN), 8);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), A0), 8);
			O[2*i+1] = gap_clipu(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), A0), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(AT_CLIP_POS(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), A0), 8);
	}
	gap_waitbarrier(0);
}

void KerMatAdd_ReLUM_USQ8(KerMat3_SQ8_T *Arg)

{
	unsigned char * __restrict__ In1	= Arg->In1;
	unsigned char * __restrict__ In2	= Arg->In2;
	unsigned char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	short int AddBias = *((short int *)(((unsigned char *)Arg->Infos)+AT_INF_ADD_BIAS));
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int F = First, S = Max(0, Last-F);
	unsigned char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + F;

	int A0 = Arg->Infos[AT_INF_A0];
	int B0 = Arg->Infos[AT_INF_B0];
	if (ActScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), ActScale, ActScaleN)), 8);
			O[2*i+1] = gap_clipu(Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), ActScale, ActScaleN)), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), ActScale, ActScaleN)), 8);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(Max(A0, AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN)), 8);
			O[2*i+1] = gap_clipu(Max(A0, AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN)), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(Max(A0, AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN)), 8);
	}
	gap_waitbarrier(0);
}

void KerMatAdd_ReLUMN_USQ8(KerMat3_SQ8_T *Arg)

{
	unsigned char * __restrict__ In1	= Arg->In1;
	unsigned char * __restrict__ In2	= Arg->In2;
	unsigned char * __restrict__ Out	= Arg->Out;
	unsigned int In1Scale = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALE], In1ScaleN = ((unsigned char *)Arg->Infos)[AT_INF_IN1SCALEN];
	unsigned int OutScale = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALE], OutScaleN = ((unsigned char *)Arg->Infos)[AT_INF_OUTSCALEN];
	unsigned int ActScale = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Arg->Infos)[AT_INF_ACTSCALEN];
	short int AddBias = *((short int *)(((unsigned char *)Arg->Infos)+AT_INF_ADD_BIAS));
	int Size = Arg->Feat;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int F = First, S = Max(0, Last-F);
	unsigned char * __restrict__ I1 = In1 + F, *__restrict__ I2 = In2 + First, *__restrict__ O  = Out + F;

	int A0 = Arg->Infos[AT_INF_A0];
	int B0 = Arg->Infos[AT_INF_B0];
	if (ActScale) {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(Min(B0, Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN), ActScale, ActScaleN))), 8);
			O[2*i+1] = gap_clipu(Min(B0, Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN), ActScale, ActScaleN))), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(Min(B0, Max(A0, AT_SCALE(AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN), ActScale, ActScaleN))), 8);
	} else {
		for (int i=0; i<S/2; i++) {
			int I10=I1[2*i], I20=I2[2*i], I11=I1[2*i+1], I21=I2[2*i+1];
			O[2*i  ] = gap_clipu(Min(B0, Max(A0, AddBias + AT_SCALE(AT_SCALE(I10, In1Scale, In1ScaleN) + I20, OutScale, OutScaleN))), 8);
			O[2*i+1] = gap_clipu(Min(B0, Max(A0, AddBias + AT_SCALE(AT_SCALE(I11, In1Scale, In1ScaleN) + I21, OutScale, OutScaleN))), 8);
		}
		if (S&0x1) O[S-1] = gap_clipu(Min(B0, Max(A0, AddBias + AT_SCALE(AT_SCALE(I1[S-1], In1Scale, In1ScaleN) + I2[S-1], OutScale, OutScaleN))), 8);
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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = AT_CLIP_POS_IMM(AT_SCALE(S, Sc, ScN), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_CLIP_POS(AT_SCALE(S, Sc, ScN), A0), 7);
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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = AT_CLIP_POS_IMM(AT_SCALE(S, Sc, ScN), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_CLIP_POS(AT_SCALE(S, Sc, ScN), A0), 7);
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

void KerParMatMulB32_2x4_SQ8(KerMatMul_SQ8_T *Arg)

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
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = (Bias[l1]<<NormBias), S1=S0, S2=S0, S3=S0;
                        int S4 = (Bias[l1+1]<<NormBias), S5=S4, S6=S4, S7=S4;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			unsigned int Sc1 = Scale[l1+1], ScN1 = ScaleN[l1+1];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7),
					   gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Sc1, ScN1), 7), gap_clip(AT_SCALE(S5, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S6, Sc1, ScN1), 7), gap_clip(AT_SCALE(S7, Sc1, ScN1), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = (Bias[l1]<<NormBias), S1=S0, S2=S0, S3=S0;
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc, ScN), 7), gap_clip(AT_SCALE(S1, Sc, ScN), 7),
					   gap_clip(AT_SCALE(S2, Sc, ScN), 7), gap_clip(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB32_2x4_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

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
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = (Bias[l1]<<NormBias), S1=S0, S2=S0, S3=S0;
                        int S4 = (Bias[l1+1]<<NormBias), S5=S4, S6=S4, S7=S4;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			unsigned int Sc1 = Scale[l1+1], ScN1 = ScaleN[l1+1];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Sc1, ScN1), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Sc1, ScN1), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Sc1, ScN1), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = (Bias[l1]<<NormBias), S1=S0, S2=S0, S3=S0;
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = AT_CLIP_POS_IMM(AT_SCALE(S, Sc, ScN), 7);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = gap_clip(AT_CLIP_POS(AT_SCALE(S, Sc, ScN), A0), 7);
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
        unsigned int Iter = (Last>First)?(Last-First):0;

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
        unsigned int Iter = (Last>First)?(Last-First):0;

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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
        unsigned int Iter = (Last>First)?(Last-First):0;
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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
        unsigned int Iter = (Last>First)?(Last-First):0;

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
        unsigned int Iter = (Last>First)?(Last-First):0;

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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
	unsigned int Iter = (Last>First)?(Last-First):0;
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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
	unsigned int Iter = (Last>First)?(Last-First):0;

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
	unsigned int Iter = (Last>First)?(Last-First):0;

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
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulB32_2x4_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg)

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
	unsigned int Iter = (Last>First)?(Last-First):0;

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int j=0; j<H_In1/2; j++) {
			int l1 = 2*j;
			v4s *pIn1_0 = (v4s *) (In1 + l1*W_In1);
			v4s *pIn1_1 = (v4s *) (In1 + (l1+1)*W_In1);
			int S0 = Bias[l1]  <<NormBias, S1=S0, S2=S0, S3=S0;
			int S4 = Bias[l1+1]<<NormBias, S5=S4, S6=S4, S7=S4;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1_0[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
				v4s C1 = pIn1_1[c];
				S4 = gap_sumdotp4(C1, V0, S4); S5 = gap_sumdotp4(C1, V1, S5); S6 = gap_sumdotp4(C1, V2, S6); S7 = gap_sumdotp4(C1, V3, S7);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
				int C1 = In1[(l1+1)*W_In1+c];
				S4 += C1 * In2[(l2+0)*W_In2+c]; S5 += C1 * In2[(l2+1)*W_In2+c]; S6 += C1 * In2[(l2+2)*W_In2+c]; S7 += C1 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
			*((v4s *) (Out+l1*H_In2 + l2)) = R;
			unsigned int Sc1 = Scale[l1+1], ScN1 = ScaleN[l1+1];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Sc1, ScN1), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Sc1, ScN1), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Sc1, ScN1), 7));
			*((v4s *) (Out+(l1+1)*H_In2 + l2)) = R1;
		}
		if (H_In1&0x1) {
			int l1 = H_In1 - 1;
			v4s *pIn1_0 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1_0[c], V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0); S1 = gap_sumdotp4(C0, V1, S1); S2 = gap_sumdotp4(C0, V2, S2); S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c]; S1 += C0 * In2[(l2+1)*W_In2+c]; S2 += C0 * In2[(l2+2)*W_In2+c]; S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			unsigned int Sc = Scale[l1], ScN = ScaleN[l1];
			v4s R = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7),
					  AT_CLIP_POS_IMM(AT_SCALE(S2, Sc, ScN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc, ScN), 7));
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
			Out[l1*H_In2 + l2+1] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc, ScN), 7);
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
			Out[l1*H_In2 + l2+0] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc, ScN), 7);
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
	unsigned int Iter = (Last>First)?(Last-First):0;
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
			v4s R = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7),
					  gap_clip(AT_CLIP_POS(AT_SCALE(S2, Sc, ScN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Sc, ScN), A0), 7));
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
			Out[l1*H_In2 + l2+1] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Sc, ScN), A0), 7);
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
			Out[l1*H_In2 + l2+0] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Sc, ScN), A0), 7);
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


void KerParMatVectMul_HWC_SQ8(KerMat3_SQ8_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Feat			= Arg->Feat;
	unsigned int Scale		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALE];
	unsigned int ScaleN		= ((unsigned char *)(Arg->Infos))[AT_INF_SCALEN];
	int S				= W*H;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (Scale)
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i;
			for (int j=0; j<(S/2); j++) {
				int I10 = I1[(2*j)*Feat], I11 = I1[(2*j+1)*Feat];
				int P1 = gap_clip(AT_SCALE(I10*Scale, I2, ScaleN), 7);
				int P2 = gap_clip(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				O[(2*j)*Feat] = P1; O[(2*j+1)*Feat] = P2;
			}
			O[(S-1)*Feat] = gap_clip(AT_SCALE(I1[(S-1)*Feat]*Scale, I2, ScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i;
			for (int j=0; j<(S/2); j++) {
				int I10 = I1[(2*j)*Feat], I11 = I1[(2*j+1)*Feat];
				int P1 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7);
				int P2 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				O[(2*j+0)*Feat] = P1; O[(2*j+1)*Feat] = P2;
			}
			O[(S-1)*Feat] = gap_clip(AT_SCALE(I1[(S-1)*Feat], I2, ScaleN), 7);
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
				int P1 = AT_CLIP_POS_IMM(AT_SCALE(I10*Scale, I2, ScaleN), 7);
				int P2 = AT_CLIP_POS_IMM(AT_SCALE(I11*Scale, I2, ScaleN), 7);
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = AT_CLIP_POS_IMM(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = AT_CLIP_POS_IMM(AT_SCALE(I10, I2, ScaleN), 7);
				int P2 = AT_CLIP_POS_IMM(AT_SCALE(I11, I2, ScaleN), 7);
				O[2*j  ] = P1; O[2*j+1] = P2;
			}
			O[W*H-1] = AT_CLIP_POS_IMM(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
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
				int P1 = AT_CLIP_POS(AT_SCALE(I10*Scale, I2, ScaleN), A0);
				int P2 = AT_CLIP_POS(AT_SCALE(I11*Scale, I2, ScaleN), A0);
				O[2*j  ] = gap_clip(P1, 7); O[2*j+1] = gap_clip(P2, 7);
			}
			O[W*H-1] = gap_clip(AT_CLIP_POS(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), A0), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int P1 = AT_CLIP_POS(AT_SCALE(I10, I2, ScaleN), A0);
				int P2 = AT_CLIP_POS(AT_SCALE(I11, I2, ScaleN), A0);
				O[2*j  ] = gap_clip(P1, 7); O[2*j+1] = gap_clip(P2, 7);
			}
			O[W*H-1] = gap_clip(AT_CLIP_POS(AT_SCALE(I1[W*H-1], I2, ScaleN), A0), 7);
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
				Acc0 = AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, AT_CLIP_POS(Acc1 + B0, A0) * C0, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0, ActScaleN), 7);
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
				Acc0 = AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1]*Scale, I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScaleN), 7);
		}
	else
		for (int i=First; i<Last; i++) {
			signed char * __restrict__ I1 = In1 + i*W*H;
			int I2 = In2[i];
			signed char * __restrict__ O  = Out + i*W*H;
			for (int j=0; j<((W*H)/2); j++) {
				int I10 = I1[2*j], I11 = I1[2*j+1];
				int Acc0 = gap_clip(AT_SCALE(I10, I2, ScaleN), 7), Acc1 = gap_clip(AT_SCALE(I11, I2, ScaleN), 7);
				Acc0 = AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScaleN);
				Acc1 = AT_SCALE(ActScale, AT_CLIP_POS(Acc1 + B0, A0) * C0 * Acc1, ActScaleN);
				O[2*j  ] = gap_clip(Acc0, 7); O[2*j+1] = gap_clip(Acc1, 7);
			}
			int Acc0 = gap_clip(AT_SCALE(I1[W*H-1], I2, ScaleN), 7);
			O[W*H-1] = gap_clip(AT_SCALE(ActScale, AT_CLIP_POS(Acc0 + B0, A0) * C0 * Acc0, ActScaleN), 7);
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

void KerParMatMulNoBias_2x4_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
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
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
                        int S4 = 0, S5=S4, S6=S4, S7=S4;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Scale, ScaleN), 7), gap_clip(AT_SCALE(S5, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S6, Scale, ScaleN), 7), gap_clip(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = 0, S1=S0, S2=S0, S3=S0;
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = 0, S1=S0;
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_SCALE(S1, Scale, ScaleN), 7);
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
                        int S0 = 0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
                }
                gap_waitbarrier(0);
	}
}
void KerParMatMulNoBias_2x4_ReLU_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
                        int S4 = 0, S5=S4, S6=S4, S7=S4;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = 0, S1=S0, S2=S0, S3=S0;
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = 0, S1=S0;
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7);
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
                        int S0 = 0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulNoBias_2x4_ReLUN_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
                        int S4 = 0, S5=S4, S6=S4, S7=S4;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			v4s R2 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = 0, S1=S0, S2=S0, S3=S0;
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = 0, S1=S0;
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7);
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
                        int S0 = 0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7);
                }
                gap_waitbarrier(0);
	}
}


void KerParMatMulB32_2x4_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
	int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
        unsigned int W_Out = Arg->W_Out;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
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
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
                        int S4 = (Bias[4*Col]<<NormBias), S5 = (Bias[4*Col+1]<<NormBias), S6 = (Bias[4*Col+2]<<NormBias), S7 = (Bias[4*Col+3]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Scale, ScaleN), 7), gap_clip(AT_SCALE(S5, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S6, Scale, ScaleN), 7), gap_clip(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
			int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = S0 = (Bias[2*Col]<<NormBias), S1 = (Bias[2*Col+1]<<NormBias);
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
			Out[(Line+OffLine)*W_Out+2*Col  +OffCol] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_SCALE(S1, Scale, ScaleN), 7);
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
                        int S0 = (Bias[Col]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB32_2x4_ReLU_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
	int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
                        int S4 = (Bias[4*Col]<<NormBias), S5 = (Bias[4*Col+1]<<NormBias), S6 = (Bias[4*Col+2]<<NormBias), S7 = (Bias[4*Col+3]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
                        int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = (Bias[2*Col]<<NormBias), S1 = (Bias[2*Col+1]<<NormBias);
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7);
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
                        int S0 = (Bias[Col]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulB32_2x4_ReLUN_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
	int * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
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
                for (Line=0; Line<Iter/2; Line++) {
                	int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
                        int S4 = (Bias[4*Col]<<NormBias), S5 = (Bias[4*Col+1]<<NormBias), S6 = (Bias[4*Col+2]<<NormBias), S7 = (Bias[4*Col+3]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * BufferColIn2[i];
				S5 += V1 * BufferColIn2[i+1*H_In2];
				S6 += V1 * BufferColIn2[i+2*H_In2];
				S7 += V1 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			v4s R2 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
			*((v4s *) (Out+(l1+OffLine+1)*W_Out+4*Col+0+OffCol)) = R2;
                }
		if (Iter&0x1) {
			int l1 = Last - 1;
			v4s *VIn1 = (v4s *) (&In1[l1*W_In1 + 0]);
                        int S0 = (Bias[4*Col]<<NormBias), S1 = (Bias[4*Col+1]<<NormBias), S2 = (Bias[4*Col+2]<<NormBias), S3 = (Bias[4*Col+3]<<NormBias);
			for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i]; S0 = gap_sumdotp4(V0, A, S0);;
				v4s B = VBuff1[i]; S1 = gap_sumdotp4(V0, B, S1);;
				v4s C = VBuff2[i]; S2 = gap_sumdotp4(V0, C, S2);;
				v4s D = VBuff3[i]; S3 = gap_sumdotp4(V0, D, S3);;
			}
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
				S1 += V0 * BufferColIn2[i+1*H_In2];
				S2 += V0 * BufferColIn2[i+2*H_In2];
				S3 += V0 * BufferColIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7),
					   gap_clip(AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), 7), gap_clip(AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0), 7));
			*((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
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
                        int S0 = (Bias[2*Col]<<NormBias), S1 = (Bias[2*Col+1]<<NormBias);
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0), 7);
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
                        int S0 = (Bias[Col]<<NormBias);
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
                                S0 = gap_sumdotp4(V0, VBuff0[i], S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[Line*W_In1 + i];
				S0 += V0 * BufferColIn2[i];
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), 7);
                }
                gap_waitbarrier(0);
	}
}


void KerParMatMulTransposedB32_SQ8(KerMatMul_SQ8_T *Arg)

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
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
		        int S0=0, S1=0, S2=0, S3=0, S4=0, S5=0, S6=0, S7=0;
		        if (Bias) {
	                        S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
	                        S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
	                        S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
	                        S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
	                }
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc0, ScN0), 7), gap_clip(AT_SCALE(S1, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S2, Sc2, ScN2), 7), gap_clip(AT_SCALE(S3, Sc3, ScN3), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Sc0, ScN0), 7), gap_clip(AT_SCALE(S5, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S6, Sc2, ScN2), 7), gap_clip(AT_SCALE(S7, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Sc0, ScN0), 7);
                        pOut[(l1+1)*W_Out + Col] = gap_clip(AT_SCALE(S1, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc0, ScN0), 7), gap_clip(AT_SCALE(S1, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S2, Sc2, ScN2), 7), gap_clip(AT_SCALE(S3, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

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
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
                        int S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
                        int S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
                        int S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc3, ScN3), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

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
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
                        int S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
                        int S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
                        int S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S1, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S3, Sc3, ScN3), A0));
			v4s R2 = gap_pack4(AT_CLIP_POS(AT_SCALE(S4, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S5, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S6, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S7, Sc3, ScN3), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S1, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S3, Sc3, ScN3), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}


void KerParMatMulTransposedNoBias_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0, S4=0;
                        int S1=0, S5=0;
                        int S2=0, S6=0;
                        int S3=0, S7=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc0, ScN0), 7), gap_clip(AT_SCALE(S1, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S2, Sc2, ScN2), 7), gap_clip(AT_SCALE(S3, Sc3, ScN3), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Sc0, ScN0), 7), gap_clip(AT_SCALE(S5, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S6, Sc2, ScN2), 7), gap_clip(AT_SCALE(S7, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Sc0, ScN0), 7);
                        pOut[(l1+1)*W_Out + Col] = gap_clip(AT_SCALE(S1, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0;
                        int S1=0;
                        int S2=0;
                        int S3=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Sc0, ScN0), 7), gap_clip(AT_SCALE(S1, Sc1, ScN1), 7),
					   gap_clip(AT_SCALE(S2, Sc2, ScN2), 7), gap_clip(AT_SCALE(S3, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_ReLU_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0, S4=0;
                        int S1=0, S5=0;
                        int S2=0, S6=0;
                        int S3=0, S7=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc3, ScN3), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S1, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0;
                        int S1=0;
                        int S2=0;
                        int S3=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Sc1, ScN1), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Sc2, ScN2), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Sc3, ScN3), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Sc0, ScN0), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_ReLUN_SQ8(KerMatMul_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char * __restrict__ Scale = Arg->Scale;
	unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0, S4=0;
                        int S1=0, S5=0;
                        int S2=0, S6=0;
                        int S3=0, S7=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S1, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S3, Sc3, ScN3), A0));
			v4s R2 = gap_pack4(AT_CLIP_POS(AT_SCALE(S4, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S5, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S6, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S7, Sc3, ScN3), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0=0;
                        int S1=0;
                        int S2=0;
                        int S3=0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
                        int Sc0 = Scale[4*Col],   ScN0 = ScaleN[4*Col];
                        int Sc1 = Scale[4*Col+1], ScN1 = ScaleN[4*Col+1];
                        int Sc2 = Scale[4*Col+2], ScN2 = ScaleN[4*Col+2];
                        int Sc3 = Scale[4*Col+3], ScN3 = ScaleN[4*Col+3];
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0), AT_CLIP_POS(AT_SCALE(S1, Sc1, ScN1), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Sc2, ScN2), A0), AT_CLIP_POS(AT_SCALE(S3, Sc3, ScN3), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0=0, S1=0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        int Sc0 = Scale[Col],   ScN0 = ScaleN[Col];
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Sc0, ScN0), A0);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}


void KerParMatMulTransposedB32_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

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
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
                        int S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
                        int S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
                        int S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Scale, ScaleN), 7), gap_clip(AT_SCALE(S5, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S6, Scale, ScaleN), 7), gap_clip(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
                        pOut[(l1+1)*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedB32_ReLU_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

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
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
                        int S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
                        int S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
                        int S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedB32_ReLUN_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

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
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
	unsigned int NormBias = Arg->NormBias;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  , S4=S0;
                        int S1 = (Bias[4*Col+1]<<NormBias), S5=S1;
                        int S2 = (Bias[4*Col+2]<<NormBias), S6=S2;
                        int S3 = (Bias[4*Col+3]<<NormBias), S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0));
			v4s R2 = gap_pack4(AT_CLIP_POS(AT_SCALE(S4, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S5, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S6, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S7, Scale, ScaleN), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = (Bias[4*Col]<<NormBias)  ;
                        int S1 = (Bias[4*Col+1]<<NormBias);
                        int S2 = (Bias[4*Col+2]<<NormBias);
                        int S3 = (Bias[4*Col+3]<<NormBias);
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = (Bias[Col]<<NormBias), S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}


void KerParMatMulTransposedNoBias_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0, S4=S0;
                        int S1 = 0, S5=S1;
                        int S2 = 0, S6=S2;
                        int S3 = 0, S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(gap_clip(AT_SCALE(S4, Scale, ScaleN), 7), gap_clip(AT_SCALE(S5, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S6, Scale, ScaleN), 7), gap_clip(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
                        pOut[(l1+1)*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0;
                        int S1 = 0;
                        int S2 = 0;
                        int S3 = 0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(gap_clip(AT_SCALE(S0, Scale, ScaleN), 7), gap_clip(AT_SCALE(S1, Scale, ScaleN), 7),
					   gap_clip(AT_SCALE(S2, Scale, ScaleN), 7), gap_clip(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = gap_clip(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_ReLU_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0, S4=S0;
                        int S1 = 0, S5=S1;
                        int S2 = 0, S6=S2;
                        int S3 = 0, S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			v4s R2 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S4, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S5, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S6, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S7, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0;
                        int S1 = 0;
                        int S2 = 0;
                        int S3 = 0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S1, Scale, ScaleN), 7),
					   AT_CLIP_POS_IMM(AT_SCALE(S2, Scale, ScaleN), 7), AT_CLIP_POS_IMM(AT_SCALE(S3, Scale, ScaleN), 7));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS_IMM(AT_SCALE(S0, Scale, ScaleN), 7);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_ReLUN_PL_SQ8(KerMatMul_PL_SQ8_T *Arg)

{
	/*
	 	Column buffer has to be sized in order to be able to accomodate up to 4 columns of size H_In2
	*/
        signed char * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        signed char * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
	unsigned char Scale = (unsigned char) Arg->Infos[AT_INF_OUTSCALE];
	unsigned char ScaleN = (unsigned char) Arg->Infos[AT_INF_OUTSCALEN];
        unsigned int OutFirstCol = Arg->OutFirstCol;
        int ColFirst = Arg->ColFirst;
        int A0 = Arg->Infos[AT_INF_A0];

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        int OffLine = 0, OffCol = 0;

        if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
        signed char *pOut = Out + W_Out*OffLine + OffCol;
        for (Line=0; Line<Iter/2; Line++) {
        	signed char *pIn2 = In2;
        	int l1 = 2*Line + First;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
                v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0, S4=S0;
                        int S1 = 0, S5=S1;
                        int S2 = 0, S6=S2;
                        int S3 = 0, S7=S3;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
				v4s V1 = VIn2[i];
                                S4 = gap_sumdotp4(V1, A, S4);
                                S5 = gap_sumdotp4(V1, B, S5);
                                S6 = gap_sumdotp4(V1, C, S6);
                                S7 = gap_sumdotp4(V1, D, S7);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
				int V1 = In1[(l1+1)*W_In1 + i];
				S4 += V1 * pIn2[i];
				S5 += V1 * pIn2[i+1*H_In2];
				S6 += V1 * pIn2[i+2*H_In2];
				S7 += V1 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0));
			v4s R2 = gap_pack4(AT_CLIP_POS(AT_SCALE(S4, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S5, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S6, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S7, Scale, ScaleN), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			*((v4s *) (pOut+(l1+1)*W_Out+4*Col)) = R2;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s V1 = VIn2[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V1, A, S1);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				int V1 = In1[(l1+1)*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V1 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
                        pOut[(l1+1)*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
			pIn2 += H_In2;
        	}
        }
        if (Iter&0x1) {
        	int l1 = Last-1;
        	signed char *pIn2 = In2;
                v4s *VIn1 = (v4s *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<(W_In2/4); Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
		        v4s * __restrict__ VBuff1 = (v4s *) (pIn2+H_In2);
		        v4s * __restrict__ VBuff2 = (v4s *) (pIn2+2*H_In2);
		        v4s * __restrict__ VBuff3 = (v4s *) (pIn2+3*H_In2);
                        int S0 = 0;
                        int S1 = 0;
                        int S2 = 0;
                        int S3 = 0;
        		for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
				v4s B = VBuff1[i];
				v4s C = VBuff2[i];
				v4s D = VBuff3[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                                S1 = gap_sumdotp4(V0, B, S1);
                                S2 = gap_sumdotp4(V0, C, S2);
                                S3 = gap_sumdotp4(V0, D, S3);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[l1*W_In1 + i];
				S0 += V0 * pIn2[i];
				S1 += V0 * pIn2[i+1*H_In2];
				S2 += V0 * pIn2[i+2*H_In2];
				S3 += V0 * pIn2[i+3*H_In2];
			}
			v4s R1 = gap_pack4(AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S1, Scale, ScaleN), A0),
					   AT_CLIP_POS(AT_SCALE(S2, Scale, ScaleN), A0), AT_CLIP_POS(AT_SCALE(S3, Scale, ScaleN), A0));
			*((v4s *) (pOut+(l1  )*W_Out+4*Col)) = R1;
			pIn2 += 4*H_In2;
        	}
        	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		        v4s * __restrict__ VBuff0 = (v4s *) pIn2;
                        int S0 = 0, S1=S0;
                        for (i=0; i<(W_In1/4); i++) {
				v4s V0 = VIn1[i];
				v4s A = VBuff0[i];
                                S0 = gap_sumdotp4(V0, A, S0);
                        }
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
				int V0 = In1[(l1  )*W_In1 + i];
				S0 += V0 * pIn2[i];
			}
                        pOut[(l1  )*W_Out + Col] = AT_CLIP_POS(AT_SCALE(S0, Scale, ScaleN), A0);
			pIn2 += H_In2;
        	}
        }
        gap_waitbarrier(0);
}

