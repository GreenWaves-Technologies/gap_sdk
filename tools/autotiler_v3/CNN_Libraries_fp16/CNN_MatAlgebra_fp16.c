#include <stdio.h>
#include <math.h>
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

void KerParMatAdd_fp16(KerMat3_fp16_T *Arg)

{
	F16 * __restrict__ In1	= Arg->In1;
	F16 * __restrict__ In2	= Arg->In2;
	F16 * __restrict__ Out	= Arg->Out;
	int W			= Arg->W;
	int H			= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		F16V * __restrict__ I1 = (F16V *) (In1 + i*W*H);
		F16V * __restrict__ I2 = (F16V *) (In2 + i*W*H);
		F16V * __restrict__ O  = (F16V *) (Out + i*W*H);
		for (j=0; j<((W*H)/4); j++) {
			F16V V0 = I1[2*j] + I2[2*j], V1 = I1[2*j+1] + I2[2*j+1];
			O[2*j] = V0; O[2*j+1] = V1;
		}
		O[(W*H)/2] = I1[(W*H)/2] + I2[(W*H)/2];
		Out[i*W*H-1] = In1[i*W*H-1] + In2[i*W*H-1];
	}
	gap_waitbarrier(0);
}

void KerParMatAddReLU_fp16(KerMat3_fp16_T *Arg)

{
	F16 * __restrict__ In1	= Arg->In1;
	F16 * __restrict__ In2	= Arg->In2;
	F16 * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	F16V M = (F16V){0,0};
	for (i=First; i<Last; i++) {
		F16V * __restrict__ I1 = (F16V *) (In1 + i*W*H);
		F16V * __restrict__ I2 = (F16V *) (In2 + i*W*H);
		F16V * __restrict__ O  = (F16V *) (Out + i*W*H);
		for (j=0; j<((W*H)/4); j++) {
			F16V V0 = I1[2*j] + I2[2*j], V1 = I1[2*j+1] + I2[2*j+1];
			O[2*j] = MaxF2(M, V0); O[2*j+1] = MaxF2(M, V1);
		}
		O[(W*H)/2] = MaxF2(M, I1[(W*H)/2] + I2[(W*H)/2]);
		Out[i*W*H-1] = MaxF((F16)0.0, In1[i*W*H-1] + In2[i*W*H-1]);
	}
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
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Fast_Sigmoid_fp16(s);
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
			Out[(Line+OffLine)*W_Out+Oo] = Fast_Sigmoid_fp16(s);
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
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Fast_Swish_fp16(s);
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
			Out[(Line+OffLine)*W_Out+Oo] = Fast_Swish_fp16(s);
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
			Out[l1*H_In2 + l2] = Fast_Swish_fp16(A);
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
			Out[l1*H_In2 + l2] = Fast_Sigmoid_fp16(A);
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

/* Matrix transposition */

static void CNN_Transpose_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
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
	for (int l=0; l<IterL/2; l++) {
		F16V *pV0 = (F16V *) (In + (Ho_F+2*l+0)*W + Wo_F);
		F16V *pV1 = (F16V *) (In + (Ho_F+2*l+1)*W + Wo_F);
		F16 *pO  = (Out + Ho_F + 4*l + Wo_F*H);
		for (int c=0; c<IterW/2; c++) {
			F16V A = pV0[c], B = pV1[c];
			F16V rA, rB;
			rA = __builtin_shuffle(A, B, (v2s) {0,2});
			rB = __builtin_shuffle(A, B, (v2s) {1,3});
			*((F16V *) (pO+(2*c+0)*H)) = rA;
			*((F16V *) (pO+(2*c+1)*H)) = rB;
		}
		if (IterW&0x1) {
			int c = IterW-1;
			F16 X0 = In[(Ho_F+2*l+0)*W + (Wo_F+c)], X1 = In[(Ho_F+2*l+1)*W + (Wo_F+c)];
			Out[Ho_F+2*l+0 + (Wo_F+c)*H] = X0; Out[Ho_F+2*l+1 + (Wo_F+c)*H] = X1;
		}
	}
	if (IterL&0x1) {
		int l = IterL-1;
		for (int c=0; c<IterW/2; c++) {
			F16 X0 = In[(Ho_F+l)*W + Wo_F+2*c+0], X1 = In[(Ho_F+l)*W + Wo_F+2*c+1];
			Out[Ho_F+l + (Wo_F+2*c+0)*H] = X0;
			Out[Ho_F+l + (Wo_F+2*c+1)*H] = X1;
		}
		if (IterW&0x1) Out[Ho_F+l+ (Wo_L-1)*H] = In[(Ho_F+l)*W+Wo_L-1];
	}
}

static void CNN_TransposeSxSy_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int Ho_F,
	unsigned int Ho_L,
	unsigned int Ho,
	unsigned int Sx,
	unsigned int Sy
	)
{
	for (int l=Ho_F; l<Ho_L; l++) {
		int IterW = Wo_L-Wo_F;
		for (int c=0; c<IterW/2; c++) {
			F16 X0 = In[Sy*l*W + Sx*(Wo_F+2*c+0)], X1 = In[Sy*l*W + Sx*(Wo_F+2*c+1)];
			Out[l + (Wo_F+2*c+0)*Ho] = X0;
			Out[l + (Wo_F+2*c+1)*Ho] = X1;
		}
		if (IterW&0x1) Out[l+ (Wo_L-1)*Ho] = In[Sy*l*W+Sx*(Wo_L-1)];
	}
}

void CNN_ParTranspose_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_Transpose_Body_fp16(In+W*H*f, Out+W*H*f, W, 0, W, H, 0, H);
	gap_waitbarrier(0);
}

void CNN_ParTransposeSxSy_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
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

	for (int f=First; f<Last; f++) CNN_TransposeSxSy_Body_fp16(In+W*H*f, Out+Wo*Ho*f, W, 0, Wo, Ho, 0, Ho, Sx, Sy);
	gap_waitbarrier(0);
}

void CNN_Transpose_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
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
		for (int f=0; f<Feat; f++) CNN_Transpose_Body_fp16(In+W*H*f, Out+W*H*f, W, Wo_F, Wo_L, H, Ho_F, Ho_L); 
	}
	gap_waitbarrier(0);
}

void CNN_TransposeSxSy_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
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
		for (int f=0; f<Feat; f++) CNN_TransposeSxSy_Body_fp16(In+W*H*f, Out+Wo*Ho*f, W, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy); 
	}
	gap_waitbarrier(0);
}

/* 3D Tensor permutations */

void CNN_MatPermCHW2CWH_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				F16 V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[c*H*W + (w+0)*H + h] = V0;
				Out[c*H*W + (w+1)*H + h] = V1;
			}
			if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2HWC_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				F16 V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*W*C + (w+0)*C + c] = V0;
				Out[h*W*C + (w+1)*C + c] = V1;
			}
			if (W&0x1) Out[h*W*C + (W-1)*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2WHC_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				F16 V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*H*C + h*C + c] = V0;
				Out[(w+1)*H*C + h*C + c] = V1;
			}
			if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2WCH_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				F16 V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*C*H + c*H + h] = V0;
				Out[(w+1)*C*H + c*H + h] = V1;
			}
			if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermCHW2HCW_fp16(KerMatTranspose_fp16_T *Arg)

{
	F16 *__restrict__ In = Arg->In;
	F16 *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				F16 V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*C*W + c*W + (w+0)] = V0;
				Out[h*C*W + c*W + (w+1)] = V1;
			}
			if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

