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

/* Matrix multiplication with cliping/ReLU/ReLUN */

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
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
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
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = s;
		}
		gap_waitbarrier(0);
	}
}


void KerParMatMulReLU_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 LB = (F16) 0.0;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
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
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = MaxF(LB, s);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulReLUN_fp16(KerMatMul_fp16_T *Arg)

{
	F16 * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	F16 * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	F16 * __restrict__ Bias = Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	F16 LB = (F16) 0.0;
	F16 UB = (F16) Arg->UB;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	F16 *BufferColIn2 = Arg->BufferColIn2;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
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
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = MinF(MaxF(LB, s), UB);
		}
		gap_waitbarrier(0);
	}
}

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

/* Matrix multiply with h-sigmoid reduction */

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

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		// for (Line=0; Line<H_In1; Line++) {
		for (Line=First; Line<Last; Line++) {
			F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V){Bias[Line], 0.0};
			for (i=0; i<W_In1/4; i++) {
				S += VIn1[2*i  ] * VBuff[2*i  ];
				S += VIn1[2*i+1] * VBuff[2*i+1];
			}
			F16 s = S[0]+S[1];
			for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = FastSigmoidF16(s);
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

/* Matrix multiply with h-swish reduction */

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

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		// for (Line=0; Line<H_In1; Line++) {
		for (Line=First; Line<Last; Line++) {
			F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
			for (i=0; i<W_In1/4; i++) {
				S += VIn1[2*i  ] * VBuff[2*i  ];
				S += VIn1[2*i+1] * VBuff[2*i+1];
			}
			F16 s = S[0]+S[1];
			for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = FastSwishF16(s);
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

/* Matrix multiply with tanh reduction */

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

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		// for (Line=0; Line<H_In1; Line++) {
		for (Line=First; Line<Last; Line++) {
			F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
			for (i=0; i<W_In1/4; i++) {
				S += VIn1[2*i  ] * VBuff[2*i  ];
				S += VIn1[2*i+1] * VBuff[2*i+1];
			}
			F16 s = S[0]+S[1];
			for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = FastTanhF16(s);
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


/* Matrix multiply with Leaky ReLU reduction */

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
	F16V *VBuff = (F16V *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	for (Col=0; Col<W_In2; Col++) {
		for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=First; Line<Last; Line++) {
			F16V *VIn1 = (F16V *) (&In1[Line*W_In1 + 0]);
			F16V S = (F16V) {Bias[Line], 0.0};
			for (i=0; i<W_In1/4; i++) {
				S += VIn1[2*i  ], VBuff[2*i  ];
				S += VIn1[2*i+1], VBuff[2*i+1];
			}
			F16 s = S[0]+S[1];
			for (i=(W_In1/4)*4; i<W_In1; i++) s += In1[Line*W_In1 + i] * BufferColIn2[i];
			if (s<0) s *= LEAK_CONSTANT;
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = s;
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

void KerParMatMulSwishSmallFeat_fp16(KerMatMul_fp16_T *Arg)

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

void KerParMatMulSigmoidSmallFeat_fp16(KerMatMul_fp16_T *Arg)

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

void KerParMatMulTanhSmallFeat_fp16(KerMatMul_fp16_T *Arg)

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

void KerParMatMulLeakyreluSmallFeat_fp16(KerMatMul_fp16_T *Arg)

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
