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

#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels_fp16.h"
#include "FastFloatApprox16.h"

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

void KerParMatAdd_fp16(KerMat3_fp16_T *Arg)

{
	F16 * __restrict__ In1	= Arg->In1;
	F16 * __restrict__ In2	= Arg->In2;
	F16 * __restrict__ Out	= Arg->Out;
	int W			= Arg->W;
	int H			= Arg->H;
	int Feat 		= Arg->N;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Feat*W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Feat*W*H);
	int Iter = Last - First;
	int i, j;

	F16V * __restrict__ I1 = (F16V *) (In1 + First), * __restrict__ I2 = (F16V *) (In2 + First), * __restrict__ O  = (F16V *) (Out + First);
	for (i=0; i<Iter/2; i++) {
		O[i] = I1[i] + I2[i];
	}
	Out[First+Iter-1] = In1[First+Iter-1] + In2[First+Iter-1];
	gap_waitbarrier(0);
}

void KerParMatAddReLU_fp16(KerMat3_fp16_T *Arg)

{
	F16 * __restrict__ In1	= Arg->In1;
	F16 * __restrict__ In2	= Arg->In2;
	F16 * __restrict__ Out	= Arg->Out;
	int W			= Arg->W;
	int H			= Arg->H;
	int Feat 		= Arg->N;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Feat*W*H);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Feat*W*H);
	int Iter = Last - First;
	int i, j;

	F16V M = (F16V){0,0};
	F16V * __restrict__ I1 = (F16V *) (In1 + First), * __restrict__ I2 = (F16V *) (In2 + First), * __restrict__ O  = (F16V *) (Out + First);
	for (i=0; i<Iter/2; i++) {
		O[i] = MaxF2(M, I1[i] + I2[i]);
	}
	Out[First+Iter-1] = MaxF((F16)0.0f, In1[First+Iter-1] + In2[First+Iter-1]);
	gap_waitbarrier(0);
}

/* Matrix multiplication */

void KerParMatMul_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = s4;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = s5;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = s6;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = s7;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = s8;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = s4;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = s5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBias_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = s4;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = s5;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = s6;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = s7;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = s8;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = s4;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = s5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		}
		gap_waitbarrier(0);
	}
}


void KerParMatMulTransposed_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = s0;
		       	pOut[      (4*Col+1)] = s1;
		       	pOut[      (4*Col+2)] = s2;
		       	pOut[      (4*Col+3)] = s3;
		       	pOut[W_Out+(4*Col  )] = s4;
		       	pOut[W_Out+(4*Col+1)] = s5;
		       	pOut[W_Out+(4*Col+2)] = s6;
		       	pOut[W_Out+(4*Col+3)] = s7;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = s0;
		       	pOut[W_Out+(W_In2-1)] = s4;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = s0;
		       	pOut[      (4*Col+1)] = s1;
		       	pOut[      (4*Col+2)] = s2;
		       	pOut[      (4*Col+3)] = s3;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = s0;
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = s0;
		       	pOut[      (4*Col+1)] = s1;
		       	pOut[      (4*Col+2)] = s2;
		       	pOut[      (4*Col+3)] = s3;
		       	pOut[W_Out+(4*Col  )] = s4;
		       	pOut[W_Out+(4*Col+1)] = s5;
		       	pOut[W_Out+(4*Col+2)] = s6;
		       	pOut[W_Out+(4*Col+3)] = s7;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = s0;
		       	pOut[W_Out+(W_In2-1)] = s4;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = s0;
		       	pOut[      (4*Col+1)] = s1;
		       	pOut[      (4*Col+2)] = s2;
		       	pOut[      (4*Col+3)] = s3;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = s0;
		}
        }
	gap_waitbarrier(0);
}


/* Matrix multiplication with ReLU */

void KerParMatMulReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MaxF(LB, s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MaxF(LB, s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MaxF(LB, s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MaxF(LB, s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MaxF(LB, s0);
		       	pOut[      (4*Col+1)] = MaxF(LB, s1);
		       	pOut[      (4*Col+2)] = MaxF(LB, s2);
		       	pOut[      (4*Col+3)] = MaxF(LB, s3);
		       	pOut[W_Out+(4*Col  )] = MaxF(LB, s4);
		       	pOut[W_Out+(4*Col+1)] = MaxF(LB, s5);
		       	pOut[W_Out+(4*Col+2)] = MaxF(LB, s6);
		       	pOut[W_Out+(4*Col+3)] = MaxF(LB, s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MaxF(LB, s0);
		       	pOut[W_Out+(W_In2-1)] = MaxF(LB, s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MaxF(LB, s0);
		       	pOut[      (4*Col+1)] = MaxF(LB, s1);
		       	pOut[      (4*Col+2)] = MaxF(LB, s2);
		       	pOut[      (4*Col+3)] = MaxF(LB, s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MaxF(LB, s0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MaxF(LB, s0);
		       	pOut[      (4*Col+1)] = MaxF(LB, s1);
		       	pOut[      (4*Col+2)] = MaxF(LB, s2);
		       	pOut[      (4*Col+3)] = MaxF(LB, s3);
		       	pOut[W_Out+(4*Col  )] = MaxF(LB, s4);
		       	pOut[W_Out+(4*Col+1)] = MaxF(LB, s5);
		       	pOut[W_Out+(4*Col+2)] = MaxF(LB, s6);
		       	pOut[W_Out+(4*Col+3)] = MaxF(LB, s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MaxF(LB, s0);
		       	pOut[W_Out+(W_In2-1)] = MaxF(LB, s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MaxF(LB, s0);
		       	pOut[      (4*Col+1)] = MaxF(LB, s1);
		       	pOut[      (4*Col+2)] = MaxF(LB, s2);
		       	pOut[      (4*Col+3)] = MaxF(LB, s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MaxF(LB, s0);
		}
        }
	gap_waitbarrier(0);
}

/* ReLUN */

void KerParMatMulReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s2));
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s3));
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s4));
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s5));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s6));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s7));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s8));
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s2));
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s3));
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s4));
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s5));
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s2));
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s3));
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s4));
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s5));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s6));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s7));
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s8));
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(UB, MaxF(LB, s2));
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(UB, MaxF(LB, s3));
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(UB, MaxF(LB, s4));
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s5));
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(UB, MaxF(LB, s1));
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(UB, MaxF(LB, s0));
		       	pOut[      (4*Col+1)] = MinF(UB, MaxF(LB, s1));
		       	pOut[      (4*Col+2)] = MinF(UB, MaxF(LB, s2));
		       	pOut[      (4*Col+3)] = MinF(UB, MaxF(LB, s3));
		       	pOut[W_Out+(4*Col  )] = MinF(UB, MaxF(LB, s4));
		       	pOut[W_Out+(4*Col+1)] = MinF(UB, MaxF(LB, s5));
		       	pOut[W_Out+(4*Col+2)] = MinF(UB, MaxF(LB, s6));
		       	pOut[W_Out+(4*Col+3)] = MinF(UB, MaxF(LB, s7));
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(UB, MaxF(LB, s0));
		       	pOut[W_Out+(W_In2-1)] = MinF(UB, MaxF(LB, s4));
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(UB, MaxF(LB, s0));
		       	pOut[      (4*Col+1)] = MinF(UB, MaxF(LB, s1));
		       	pOut[      (4*Col+2)] = MinF(UB, MaxF(LB, s2));
		       	pOut[      (4*Col+3)] = MinF(UB, MaxF(LB, s3));
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(UB, MaxF(LB, s0));
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(UB, MaxF(LB, s0));
		       	pOut[      (4*Col+1)] = MinF(UB, MaxF(LB, s1));
		       	pOut[      (4*Col+2)] = MinF(UB, MaxF(LB, s2));
		       	pOut[      (4*Col+3)] = MinF(UB, MaxF(LB, s3));
		       	pOut[W_Out+(4*Col  )] = MinF(UB, MaxF(LB, s4));
		       	pOut[W_Out+(4*Col+1)] = MinF(UB, MaxF(LB, s5));
		       	pOut[W_Out+(4*Col+2)] = MinF(UB, MaxF(LB, s6));
		       	pOut[W_Out+(4*Col+3)] = MinF(UB, MaxF(LB, s7));
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(UB, MaxF(LB, s0));
		       	pOut[W_Out+(W_In2-1)] = MinF(UB, MaxF(LB, s4));
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(UB, MaxF(LB, s0));
		       	pOut[      (4*Col+1)] = MinF(UB, MaxF(LB, s1));
		       	pOut[      (4*Col+2)] = MinF(UB, MaxF(LB, s2));
		       	pOut[      (4*Col+3)] = MinF(UB, MaxF(LB, s3));
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(UB, MaxF(LB, s0));
		}
        }
	gap_waitbarrier(0);
}

/* Sigmoid */

void KerParMatMulSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSigmoidF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSigmoidF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSigmoidF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSigmoidF16(s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSigmoidF16(s0);
		       	pOut[      (4*Col+1)] = FastSigmoidF16(s1);
		       	pOut[      (4*Col+2)] = FastSigmoidF16(s2);
		       	pOut[      (4*Col+3)] = FastSigmoidF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastSigmoidF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastSigmoidF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastSigmoidF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastSigmoidF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastSigmoidF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastSigmoidF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSigmoidF16(s0);
		       	pOut[      (4*Col+1)] = FastSigmoidF16(s1);
		       	pOut[      (4*Col+2)] = FastSigmoidF16(s2);
		       	pOut[      (4*Col+3)] = FastSigmoidF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastSigmoidF16(s0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSigmoidF16(s0);
		       	pOut[      (4*Col+1)] = FastSigmoidF16(s1);
		       	pOut[      (4*Col+2)] = FastSigmoidF16(s2);
		       	pOut[      (4*Col+3)] = FastSigmoidF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastSigmoidF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastSigmoidF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastSigmoidF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastSigmoidF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastSigmoidF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastSigmoidF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSigmoidF16(s0);
		       	pOut[      (4*Col+1)] = FastSigmoidF16(s1);
		       	pOut[      (4*Col+2)] = FastSigmoidF16(s2);
		       	pOut[      (4*Col+3)] = FastSigmoidF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastSigmoidF16(s0);
		}
        }
	gap_waitbarrier(0);
}

/* HSigmoid */

void KerParMatMulHSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 Offset = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasHSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 Offset = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedHSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 Offset = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasHSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 Offset = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
        }
	gap_waitbarrier(0);
}

/* Swish */

void KerParMatMulSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastSwishF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastSwishF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastSwishF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastSwishF16(s1);
		}
		gap_waitbarrier(0);
	}
}


void KerParMatMulTransposedSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSwishF16(s0);
		       	pOut[      (4*Col+1)] = FastSwishF16(s1);
		       	pOut[      (4*Col+2)] = FastSwishF16(s2);
		       	pOut[      (4*Col+3)] = FastSwishF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastSwishF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastSwishF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastSwishF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastSwishF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastSwishF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastSwishF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSwishF16(s0);
		       	pOut[      (4*Col+1)] = FastSwishF16(s1);
		       	pOut[      (4*Col+2)] = FastSwishF16(s2);
		       	pOut[      (4*Col+3)] = FastSwishF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastSwishF16(s0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSwishF16(s0);
		       	pOut[      (4*Col+1)] = FastSwishF16(s1);
		       	pOut[      (4*Col+2)] = FastSwishF16(s2);
		       	pOut[      (4*Col+3)] = FastSwishF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastSwishF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastSwishF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastSwishF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastSwishF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastSwishF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastSwishF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastSwishF16(s0);
		       	pOut[      (4*Col+1)] = FastSwishF16(s1);
		       	pOut[      (4*Col+2)] = FastSwishF16(s2);
		       	pOut[      (4*Col+3)] = FastSwishF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastSwishF16(s0);
		}
        }
	gap_waitbarrier(0);
}

/* HSwish */

void KerParMatMulHSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 Offset = 3.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasHSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 Offset = 3.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedHSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 Offset = 3.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasHSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	F16 Offset = 3.0;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0 + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s1);
		}
		gap_waitbarrier(0);
	}
}

/* TanH */

void KerParMatMulNoBiasTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s4);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s5);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s6);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s7);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s8);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = FastTanhF16(s2);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = FastTanhF16(s3);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = FastTanhF16(s4);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s1);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s5);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = FastTanhF16(s1);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastTanhF16(s0);
		       	pOut[      (4*Col+1)] = FastTanhF16(s1);
		       	pOut[      (4*Col+2)] = FastTanhF16(s2);
		       	pOut[      (4*Col+3)] = FastTanhF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastTanhF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastTanhF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastTanhF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastTanhF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastTanhF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastTanhF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastTanhF16(s0);
		       	pOut[      (4*Col+1)] = FastTanhF16(s1);
		       	pOut[      (4*Col+2)] = FastTanhF16(s2);
		       	pOut[      (4*Col+3)] = FastTanhF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastTanhF16(s0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastTanhF16(s0);
		       	pOut[      (4*Col+1)] = FastTanhF16(s1);
		       	pOut[      (4*Col+2)] = FastTanhF16(s2);
		       	pOut[      (4*Col+3)] = FastTanhF16(s3);
		       	pOut[W_Out+(4*Col  )] = FastTanhF16(s4);
		       	pOut[W_Out+(4*Col+1)] = FastTanhF16(s5);
		       	pOut[W_Out+(4*Col+2)] = FastTanhF16(s6);
		       	pOut[W_Out+(4*Col+3)] = FastTanhF16(s7);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = FastTanhF16(s0);
		       	pOut[W_Out+(W_In2-1)] = FastTanhF16(s4);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = FastTanhF16(s0);
		       	pOut[      (4*Col+1)] = FastTanhF16(s1);
		       	pOut[      (4*Col+2)] = FastTanhF16(s2);
		       	pOut[      (4*Col+3)] = FastTanhF16(s3);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = FastTanhF16(s0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulHTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8, (F16)-1.0), (F16)1.0);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		}
		gap_waitbarrier(0);
	}
}

/* HTanh */

void KerParMatMulNoBiasHTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s6, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s7, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s8, (F16)-1.0), (F16)1.0);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedHTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7, (F16)-1.0), (F16)1.0);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasHTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col  )] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+1)] = MinF(MaxF(s5, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+2)] = MinF(MaxF(s6, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(4*Col+3)] = MinF(MaxF(s7, (F16)-1.0), (F16)1.0);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[W_Out+(W_In2-1)] = MinF(MaxF(s4, (F16)-1.0), (F16)1.0);
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+1)] = MinF(MaxF(s1, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+2)] = MinF(MaxF(s2, (F16)-1.0), (F16)1.0);
		       	pOut[      (4*Col+3)] = MinF(MaxF(s3, (F16)-1.0), (F16)1.0);
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = MinF(MaxF(s0, (F16)-1.0), (F16)1.0);
		}
        }
	gap_waitbarrier(0);
}

/* LeakyRelu */

void KerParMatMulLeakyrelu_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0}, S6 = (F16V) {Bias[l1+1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0}, S7 = (F16V) {Bias[l1+1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0}, S8 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = (s6<0)?(s6*LEAK_CONSTANT):s6;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = (s7<0)?(s7*LEAK_CONSTANT):s7;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = (s8<0)?(s8*LEAK_CONSTANT):s8;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			F16V S2 = (F16V) {Bias[l1], 0.0};
			F16V S3 = (F16V) {Bias[l1], 0.0};
			F16V S4 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0}, S5 = (F16V) {Bias[l1+1], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {Bias[l1], 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulNoBiasLeakyrelu_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff1 = (F16V *) (&BufferColIn2[0]);
	F16V *VBuff2 = (F16V *) (&BufferColIn2[1*H_In2]);
	F16V *VBuff3 = (F16V *) (&BufferColIn2[2*H_In2]);
	F16V *VBuff4 = (F16V *) (&BufferColIn2[3*H_In2]);

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2/4; Col++) {
		for (i=F; i<L; i++) {
			BufferColIn2[i        ] = In2[i*W_In2+4*Col];
			BufferColIn2[i+1*H_In2] = In2[i*W_In2+4*Col+1];
			BufferColIn2[i+2*H_In2] = In2[i*W_In2+4*Col+2];
			BufferColIn2[i+3*H_In2] = In2[i*W_In2+4*Col+3];
		}
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0}, S6 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0}, S7 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0}, S8 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
				S8 += VIn2[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			F16 s8 = S8[0]+S8[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s8 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = (s6<0)?(s6*LEAK_CONSTANT):s6;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = (s7<0)?(s7*LEAK_CONSTANT):s7;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = (s8<0)?(s8*LEAK_CONSTANT):s8;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			F16V S4 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s2 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+1*H_In2];
				s3 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+2*H_In2];
				s4 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0}, S5 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16 s1 = S1[0]+S1[1];
			F16 s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
			F16V S1 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16 s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposedLeakyrelu_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0}, S4=S0;
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0}, S5=S1;
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0}, S6=S2;
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[      (4*Col+1)] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	pOut[      (4*Col+2)] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	pOut[      (4*Col+3)] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	pOut[W_Out+(4*Col  )] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		       	pOut[W_Out+(4*Col+1)] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		       	pOut[W_Out+(4*Col+2)] = (s6<0)?(s6*LEAK_CONSTANT):s6;
		       	pOut[W_Out+(4*Col+3)] = (s7<0)?(s7*LEAK_CONSTANT):s7;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[W_Out+(W_In2-1)] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {Bias[(4*Col  )], 0.0};
			F16V S1 = (F16V) {Bias[(4*Col+1)], 0.0};
			F16V S2 = (F16V) {Bias[(4*Col+2)], 0.0};
			F16V S3 = (F16V) {Bias[(4*Col+3)], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[      (4*Col+1)] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	pOut[      (4*Col+2)] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	pOut[      (4*Col+3)] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {Bias[Col], 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		}
        }
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBiasLeakyrelu_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Iter = (Last>First)?(Last-First):0;
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	F16 * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16 *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	F16V *VIn2 = (F16V *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			F16V S1 = (F16V) {0.0, 0.0}, S5=S1;
			F16V S2 = (F16V) {0.0, 0.0}, S6=S2;
			F16V S3 = (F16V) {0.0, 0.0}, S7=S3;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn2[i] * VBuff0[i];
				S5 += VIn2[i] * VBuff1[i];
				S6 += VIn2[i] * VBuff2[i];
				S7 += VIn2[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			F16 s4 = S4[0]+S4[1];
			F16 s5 = S5[0]+S5[1];
			F16 s6 = S6[0]+S6[1];
			F16 s7 = S7[0]+S7[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s6 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s7 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[      (4*Col+1)] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	pOut[      (4*Col+2)] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	pOut[      (4*Col+3)] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	pOut[W_Out+(4*Col  )] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		       	pOut[W_Out+(4*Col+1)] = (s5<0)?(s5*LEAK_CONSTANT):s5;
		       	pOut[W_Out+(4*Col+2)] = (s6<0)?(s6*LEAK_CONSTANT):s6;
		       	pOut[W_Out+(4*Col+3)] = (s7<0)?(s7*LEAK_CONSTANT):s7;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[W_Out+(W_In2-1)] = (s4<0)?(s4*LEAK_CONSTANT):s4;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16 *pIn2 = In2;
        	int l1 = Last-1;
        	F16V *VIn1 = (F16V *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V *VBuff1 = (F16V *) (pIn2+H_In2);
			F16V *VBuff2 = (F16V *) (pIn2+2*H_In2);
			F16V *VBuff3 = (F16V *) (pIn2+3*H_In2);
			F16V S0 = (F16V) {0.0, 0.0};
			F16V S1 = (F16V) {0.0, 0.0};
			F16V S2 = (F16V) {0.0, 0.0};
			F16V S3 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16 s0 = S0[0]+S0[1];
			F16 s1 = S1[0]+S1[1];
			F16 s2 = S2[0]+S2[1];
			F16 s3 = S3[0]+S3[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s1 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+H_In2];
				s2 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+2*H_In2];
				s3 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1+3*H_In2];
			}
		       	pOut[      (4*Col  )] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		       	pOut[      (4*Col+1)] = (s1<0)?(s1*LEAK_CONSTANT):s1;
		       	pOut[      (4*Col+2)] = (s2<0)?(s2*LEAK_CONSTANT):s2;
		       	pOut[      (4*Col+3)] = (s3<0)?(s3*LEAK_CONSTANT):s3;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			F16V *VBuff0 = (F16V *) (pIn2);
			F16V S0 = (F16V) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16 s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = (s0<0)?(s0*LEAK_CONSTANT):s0;
		}
        }
	gap_waitbarrier(0);
}


/* MatMul for CHW pointwise convolution with stride */

void KerParMatMulSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = s;
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

void KerParMatMulSxSyReLU_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	F16 LB = (F16) 0.0;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = MaxF(LB, s);
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

void KerParMatMulSxSyReLUN_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	F16 LB = (F16) 0.0;
	F16 UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = MinF(MaxF(LB, s), UB);
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


void KerParMatMulSigmoidSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = FastSigmoidF16(s);
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

void KerParMatMulHSigmoidSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
	F16 Offset = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = MinF(MaxF(s + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
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


void KerParMatMulSwishSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = FastSwishF16(s);
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

void KerParMatMulHSwishSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = MinF(MaxF(s + (F16)3.0, (F16)0.0), (F16)6.0) / (F16)6.0;
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

void KerParMatMulTanhSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = FastTanhF16(s);
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

void KerParMatMulHTanhSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = MinF(MaxF(s, (F16)-1.0), (F16)1.0);
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


void KerParMatMulLeakyreluSxSy_fp16(KerMatMul_fp16_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
		       	for (i=0; i<W_In1/4; i++) {
			       	S += VIn1[2*i  ] * VBuff[2*i  ];
			       	S += VIn1[2*i+1] * VBuff[2*i+1];
		       	}
			F16 s = S[0]+S[1];
		       	for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			if (s<0) s *= LEAK_CONSTANT;
			Out[(Line+OffLine)*W_Out+Oo] = s;
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


/* Matrix mult for small first matrix in the product, goal is to improve parallelism in this specific situation */

/* In1 fits completly in shared L1, usually conv weights
   In2 has been transposed before being used, usually conv Features
   Parallelization scheme partition In2 along H_In2
*/
void KerParMatMulSmallFeat_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = A;
		}
	}
	gap_waitbarrier(0);
}


void KerParMatMulSmallFeatReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 LB = (F16) 0.0;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = MaxF(LB, A);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 LB = (F16) 0.0;
	F16 UB = Arg->UB;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = MinF(MaxF(LB, A), UB);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = FastSwishF16(A);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatHSwish_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = MinF(MaxF(A + (F16)3.0, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = FastSigmoidF16(A);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatHSigmoid_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 Offset = Arg->UB;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = MinF(MaxF(A + Offset, (F16)0.0), (F16)6.0) / (F16)6.0;
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = FastTanhF16(A);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatHTanh_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = MinF(MaxF(A, (F16)-1.0), (F16)1.0);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulSmallFeatLeakyrelu_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		F16V *pIn2 = (F16V *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			F16V *pIn1 = (F16V *) (In1 + l1*W_In1);
			F16V Acc = (F16V) {Bias[l1], 0.0};
			for (int c=0; c<W_In1/(2*2); c++) {
				F16V V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				F16V V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc += V0 * V2;
				Acc += V1 * V3;
			}
			if (W_In1&0x2) Acc += pIn1[W_In1/2-1] * pIn2[W_In1/2-1];
			F16 A = Acc[0]+Acc[1];
			if (W_In1&0x1) A += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			if (A<0) A *= LEAK_CONSTANT;
			Out[l1*H_In2 + l2] = A;
		}
	}
	gap_waitbarrier(0);
}
