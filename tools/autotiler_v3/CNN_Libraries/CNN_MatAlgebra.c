#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels.h"

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

void KerParMatAdd_fp(KerMat3_fp_T *Arg)

{
	short int * __restrict__ In1	= Arg->In1;
	short int * __restrict__ In2	= Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		short int * __restrict__ I1 = In1 + i*W*H;
		short int * __restrict__ I2 = In2 + i*W*H;
		short int * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			O[2*j] = gap_clip(I1[2*j]+I2[2*j], 15); O[2*j+1] = gap_clip(I1[2*j+1]+I2[2*j+1], 15);
		}
		O[W*H-1] = gap_clip(I1[W*H-1] + I2[W*H-1], 15);
	}
	gap_waitbarrier(0);
}

void KerParMatAddDynAdjust_fp(KerMat3_fp_T *Arg)

{
	short int * __restrict__ In1	= Arg->In1;
	short int * __restrict__ In2	= Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int LB				= Arg->LB;
	int UB				= Arg->UB;
	int In1_Q			= Arg->In1_Q;
	int In2_Q			= Arg->In2_Q;
	int Out_Q			= Arg->Out_Q;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	short int * __restrict__ I1, * __restrict__ I2;
	short int * __restrict__ O  = Out + First*W*H;
	int OffIn = Abs(In1_Q-In2_Q);
	int OffOut = Out_Q-Min(In1_Q, In2_Q);

	if (In1_Q<=In2_Q) {
		I1 = In2 + First*W*H; I2 = In1 + First*W*H;
	} else {
		I1 = In1 + First*W*H; I2 = In2 + First*W*H;
	}
	if (OffOut==0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max((I1[2*j]>>OffIn)+I2[2*j], LB), UB); O[2*j+1] = Min(Max((I1[2*j+1]>>OffIn)+I2[2*j+1], LB), UB);
			}
			O[W*H-1] = Min(Max((I1[W*H-1]>>OffIn) + I2[W*H-1], LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else if (OffOut>0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]>>OffIn)+I2[2*j])<<OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]>>OffIn)+I2[2*j+1])<<OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]>>OffIn) + I2[W*H-1])<<OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else {
		OffOut = Abs(OffOut);
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]>>OffIn)+I2[2*j])>>OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]>>OffIn)+I2[2*j+1])>>OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]>>OffIn) + I2[W*H-1])>>OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAdd_fps(KerMat3_fps_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		signed char * __restrict__ I1 = In1 + i*W*H;
		signed char * __restrict__ I2 = In2 + i*W*H;
		signed char * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			O[2*j] = gap_clip(I1[2*j]+I2[2*j], 7); O[2*j+1] = gap_clip(I1[2*j+1]+I2[2*j+1], 7);
		}
		O[W*H-1] = gap_clip(I1[W*H-1] + I2[W*H-1], 7);
	}
	gap_waitbarrier(0);
}

void KerParMatAddDynAdjust_fps(KerMat3_fps_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int LB				= Arg->LB;
	int UB				= Arg->UB;
	int In1_Q			= Arg->In1_Q;
	int In2_Q			= Arg->In2_Q;
	int Out_Q			= Arg->Out_Q;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	signed char * __restrict__ I1, * __restrict__ I2;
	signed char * __restrict__ O  = Out + First*W*H;
	int OffIn = Abs(In1_Q-In2_Q);
	int OffOut = Out_Q-Min(In1_Q, In2_Q);

	if (In1_Q<In2_Q) {
		I1 = In2 + First*W*H; I2 = In1 + First*W*H;
	} else {
		I1 = In1 + First*W*H; I2 = In2 + First*W*H;
	}
	if (OffOut==0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max((I1[2*j]>>OffIn)+I2[2*j], LB), UB); O[2*j+1] = Min(Max((I1[2*j+1]>>OffIn)+I2[2*j+1], LB), UB);
			}
			O[W*H-1] = Min(Max((I1[W*H-1]>>OffIn) + I2[W*H-1], LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else if (OffOut>0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]>>OffIn)+I2[2*j])<<OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]>>OffIn)+I2[2*j+1])<<OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]>>OffIn) + I2[W*H-1])<<OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else {
		OffOut = Abs(OffOut);
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]>>OffIn)+I2[2*j])>>OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]>>OffIn)+I2[2*j+1])>>OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]>>OffIn) + I2[W*H-1])>>OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	}
	gap_waitbarrier(0);
}

void KerParMatAddReLU_fp(KerMat3_fp_T *Arg)

{
	short int * __restrict__ In1	= Arg->In1;
	short int * __restrict__ In2	= Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		short int * __restrict__ I1 = In1 + i*W*H;
		short int * __restrict__ I2 = In2 + i*W*H;
		short int * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			O[2*j] = Max(gap_clip(I1[2*j]+I2[2*j], 15), 0); O[2*j+1] = Max(gap_clip(I1[2*j+1]+I2[2*j+1], 15), 0);
		}
		O[W*H-1] = Max(gap_clip(I1[W*H-1] + I2[W*H-1], 15), 0);
	}
	gap_waitbarrier(0);
}

void KerParMatAddReLU_fps(KerMat3_fps_T *Arg)

{
	signed char * __restrict__ In1	= Arg->In1;
	signed char * __restrict__ In2	= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		signed char * __restrict__ I1 = In1 + i*W*H;
		signed char * __restrict__ I2 = In2 + i*W*H;
		signed char * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			O[2*j] = Max(gap_clip(I1[2*j]+I2[2*j], 7), 0); O[2*j+1] = Max(gap_clip(I1[2*j+1]+I2[2*j+1], 7), 0);
		}
		O[W*H-1] = Max(gap_clip(I1[W*H-1] + I2[W*H-1], 7), 0);
	}
	gap_waitbarrier(0);
}

/* Matrix multiplication with cliping/ReLU/ReLUN */

void KerParMatMul_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(S, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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

void KerParMatMul_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(S, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulTransposed_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	short int * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=First; Line<Last; Line++) {
        	short int *pIn2 = In2;
        	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
		for (Col=0; Col<W_In2/2; Col++) {
			v2s *VBuff1 = (v2s *) (pIn2);
			v2s *VBuff2 = (v2s *) (pIn2+H_In2);
			int S1 = (Bias[(2*Col  )]<<NormBias);
			int S2 = (Bias[(2*Col+1)]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S1 = gap_sumdotp2(VIn1[2*i  ], VBuff1[2*i  ], S1);
				S1 = gap_sumdotp2(VIn1[2*i+1], VBuff1[2*i+1], S1);
				S2 = gap_sumdotp2(VIn1[2*i  ], VBuff2[2*i  ], S2);
				S2 = gap_sumdotp2(VIn1[2*i+1], VBuff2[2*i+1], S2);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) {
				S1 += In1[Line*W_In1 + i] * pIn2[i];
				S2 += In1[Line*W_In1 + i] * pIn2[i+H_In2];
			}
		       	pOut[(2*Col  )] = Min(Max(AT_NORM(S1, Norm), LB), UB);
		       	pOut[(2*Col+1)] = Min(Max(AT_NORM(S2, Norm), LB), UB);
		       	pIn2 += 2*H_In2;
		}
		if (W_In2&0x1) { // Last column
			v2s *VBuff1 = (v2s *) (pIn2);
			int S1 = (Bias[(2*Col  )]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S1 = gap_sumdotp2(VIn1[2*i  ], VBuff1[2*i  ], S1);
				S1 = gap_sumdotp2(VIn1[2*i+1], VBuff1[2*i+1], S1);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S1 += In1[Line*W_In1 + i] * pIn2[i];
		       	pOut[(W_In2-1)] = Min(Max(AT_NORM(S1, Norm), LB), UB);
		}
		pOut += W_Out;
	}
	gap_waitbarrier(0);
}

void KerParMatMulTransposedNoBias_fp(KerMatMul_fpd_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	int OffLine = 0, OffCol = 0;

	if (ColFirst) OffLine = OutFirstCol; else OffCol = OutFirstCol;
	short int * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=First; Line<Last; Line++) {
        	short int *pIn2 = In2;
        	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
		for (Col=0; Col<W_In2/2; Col++) {
			v2s *VBuff1 = (v2s *) (pIn2);
			v2s *VBuff2 = (v2s *) (pIn2+H_In2);
			int S1 = 0;
			int S2 = 0;
			for (i=0; i<W_In1/4; i++) {
				S1 = gap_sumdotp2(VIn1[2*i  ], VBuff1[2*i  ], S1);
				S1 = gap_sumdotp2(VIn1[2*i+1], VBuff1[2*i+1], S1);
				S2 = gap_sumdotp2(VIn1[2*i  ], VBuff2[2*i  ], S2);
				S2 = gap_sumdotp2(VIn1[2*i+1], VBuff2[2*i+1], S2);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) {
				S1 += In1[Line*W_In1 + i] * pIn2[i];
				S2 += In1[Line*W_In1 + i] * pIn2[i+H_In2];
			}
		       	pOut[(2*Col  )] = Min(Max(AT_NORM(S1, Norm), LB), UB);
		       	pOut[(2*Col+1)] = Min(Max(AT_NORM(S2, Norm), LB), UB);
		       	pIn2 += 2*H_In2;
		}
		if (W_In2&0x1) { // Last column
			v2s *VBuff1 = (v2s *) (pIn2);
			int S1 = 0;
			for (i=0; i<W_In1/4; i++) {
				S1 = gap_sumdotp2(VIn1[2*i  ], VBuff1[2*i  ], S1);
				S1 = gap_sumdotp2(VIn1[2*i+1], VBuff1[2*i+1], S1);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S1 += In1[Line*W_In1 + i] * pIn2[i];
		       	pOut[(W_In2-1)] = Min(Max(AT_NORM(S1, Norm), LB), UB);
		}
		pOut += W_Out;
	}
	gap_waitbarrier(0);
}

void KerParMatMulSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		   	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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

void KerParMatMul_fps(KerMatMul_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(Min(Max(AT_NORM(S0, Norm), LB), UB), Min(Max(AT_NORM(S1, Norm), LB), UB), Min(Max(AT_NORM(S2, Norm), LB), UB), Min(Max(AT_NORM(S3, Norm), LB), UB));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1, Norm), LB), UB);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSy_fps(KerMatMul_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
		   	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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

void KerParMatMul_fp_fps(KerMatMul_fp_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(Min(Max(AT_NORM(S0, Norm), LB), UB), Min(Max(AT_NORM(S1, Norm), LB), UB), Min(Max(AT_NORM(S2, Norm), LB), UB), Min(Max(AT_NORM(S3, Norm), LB), UB));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1, Norm), LB), UB);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSy_fp_fps(KerMatMul_fp_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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

/* Matrix multiplication with cliping/ChannelScale/Sigmoid/Swish */
static inline int __attribute__((always_inline)) MatMulReduct_fp(int P, int C1, int C2, int M, int Norm, int LB, int UB, int Oper)

{
	int Acc;
	switch (Oper) {
		case 0: /* Channel Scale */
			Acc = Min(Max(AT_NORM(AT_NORM(P, Norm)*M, Norm), LB), UB);
			break;
		case 1: /* H Sigmoid */
			/* C1 = (1<<(2*Norm))-1; C2 = (1<<Norm)-1; */
			Acc = (P+C1)>>1;
                        Acc = gap_max(0, gap_min(C2, AT_NORM(Acc, Norm)));
			break;
		case 2: /* H Swish */
			/* C1 = 3<<(2*Norm); C2 = (1<<16)/6; 1/6 in Q16 */
			Acc = AT_NORM(AT_NORM(AT_NORM(gap_min(gap_max(P + C1, 0), UB), Norm) * P, Norm) * C2, 16);
			break;
		default:
			Acc = Min(Max(AT_NORM(P, Norm), LB), UB);

	}
	return Acc;
}

static inline int __attribute__((always_inline)) MatMulReduct_fps(int P, int C1, int C2, int M, int Norm, int LB, int UB, int Oper)

{
	int Acc;
	switch (Oper) {
		case 0: /* Channel Scale */
			/* Norm = 2*Norm */
			Acc = Min(Max(AT_NORM(P*M, Norm), LB), UB);
			break;
		case 1: /* H Sigmoid */
			/* C1 = (1<<(2*Norm))-1; C2 = (1<<Norm)-1; */
			Acc = (P+C1)>>1;
                        Acc = gap_max(0, gap_min(C2, AT_NORM(Acc, Norm)));
			break;
		case 2: /* H Swish */
			/* Norm = 2*Norm; C1 = 3<<(2*Norm); C2 = (1<<16)/6; 1/6 in Q16 */
			Acc = AT_NORM(AT_NORM(gap_min(gap_max(P + C1, 0), UB) * P, Norm) * C2, 16);
			break;
		default:
			Acc = Min(Max(AT_NORM(P, Norm), LB), UB);

	}
	return Acc;
}

/* Matrix multiplication with output scaling by the same scalar for all channels */

void KerParMatMulScaleScalar_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulScaleScalarSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
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

void KerParMatMulScaleScalar_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulScaleScalarSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
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

void KerParMatMulScaleScalar_fps(KerMatMul_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1*M, Norm), LB), UB);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulScaleScalarSxSy_fps(KerMatMul_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormT = Arg->Norm+Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S*M, NormT), LB), UB);
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

void KerParMatMulScaleScalar_fp_fps(KerMatMul_fp_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1*M, Norm), LB), UB);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulScaleScalarSxSy_fp_fps(KerMatMul_fp_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormT = Norm+Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S*M, NormT), LB), UB);
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

/* Matrix multiplication with output scaling by scalar[Channel] for all channels */

void KerParMatMulScale_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ MulBias = Arg->MulBias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulScaleSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ MulBias = Arg->MulBias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
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

void KerParMatMulScale_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	int * __restrict__ MulBias = Arg->MulBias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulScaleSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	int * __restrict__ Bias = Arg->Bias;
	int * __restrict__ MulBias = Arg->MulBias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormMulBias = Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(AT_NORM(S, Norm)*M, NormMulBias), LB), UB);
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

void KerParMatMulScale_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ MulBias = Arg->MulBias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			int M = MulBias[Line];
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1*M, Norm), LB), UB);
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
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulScaleSxSy_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ MulBias = Arg->MulBias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormT = Norm+Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S*M, NormT), LB), UB);
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

void KerParMatMulScale_fp_fps(KerMatMul_fp_fps_T *Arg)

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
	short int * __restrict__ MulBias = Arg->MulBias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			int M = MulBias[Line];
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1*M, Norm), LB), UB);
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
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0*M, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulScaleSxSy_fp_fps(KerMatMul_fp_fps_T *Arg)

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
	short int * __restrict__ MulBias = Arg->MulBias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	unsigned int NormT = Norm+Arg->NormMulBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S*M, NormT), LB), UB);
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

void KerParMatMulHsigmoid_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulHsigmoidSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
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

void KerParMatMulHsigmoid_fps(KerMatMul_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut, LB=0;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S2, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S3, Norm)+C1, UB))*C2, 15));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulHsigmoidSxSy_fps(KerMatMul_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
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

void KerParMatMulHswish_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;


	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			S = AT_NORM(S, Norm);
			Out[(Line+OffLine)*W_Out+Col+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulHswishSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			S = AT_NORM(S, Norm);
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
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

void KerParMatMulHswish_fps(KerMatMul_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut, LB=0;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			v4s R = gap_pack4(AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S1, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S2 + C1, 0), UB) * S2, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S3 + C1, 0), UB) * S3, NormOut) * C2, 15));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S0, NormOut) * C2, 15);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulHswishSxSy_fps(KerMatMul_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			S = AT_NORM(S, Norm);
		       	Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
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

void KerParMatMulLeakyrelu_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 15);
		       		Out[(Line+OffLine)*W_Out+Col+OffCol] = Acc0;
			}
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulLeakyreluSxSy_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<NormBias);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 15);
				Out[(Line+OffLine)*W_Out+Oo] = Acc0;
			}
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

void KerParMatMulLeakyrelu_fps(KerMatMul_fps_T *Arg)

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
        unsigned int OutFirstCol = Arg->OutFirstCol;
        signed char * __restrict__ BufferColIn2 = Arg->BufferColIn2;
        unsigned int Norm = Arg->Norm;
        unsigned int NormBias = Arg->NormBias;
	int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S2, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S2 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S3, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S3 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			v4s R = gap_pack4(S0, S1, S2, S3);
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = S0;
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = S1;
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = S0;
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulLeakyreluSxSy_fps(KerMatMul_fps_T *Arg)

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
	unsigned int Norm = Arg->Norm;
	unsigned int NormBias = Arg->NormBias;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 7);
		       		Out[(Line+OffLine)*W_Out+Oo] = Acc0;
			}
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

void KerParMatMulSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			v4s R = gap_pack4(Min(Max(AT_NORM(S0, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1, Norm), LB), UB),
					  Min(Max(AT_NORM(S2, Norm), LB), UB),
					  Min(Max(AT_NORM(S3, Norm), LB), UB));
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0, Norm), LB));
			Out[l1*H_In2 + l2+1] = gap_min(UB, gap_max(AT_NORM(S1, Norm), LB));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0, Norm), LB));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulHswishSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int C1 = 3<<NormOut;
	int C2 = (1<<15)/6; // 1/6 in Q15
	int UB = 6<<NormOut;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			v4s R = gap_pack4(AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S1, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S2 + C1, 0), UB) * S2, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S3 + C1, 0), UB) * S3, NormOut) * C2, 15));
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
			Out[l1*H_In2 + l2+1] = AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S1, NormOut) * C2, 15);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulHsigmoidSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int C1 = 3<<NormOut;
	int C2 = (1<<15)/6; // 1/6 in Q15
	int UB = 6<<NormOut;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			v4s R = gap_pack4(AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S2, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S3, Norm)+C1, UB))*C2, 15));
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
			Out[l1*H_In2 + l2+1] = AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15);
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulLeakyreluSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	int LB = Arg->LB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S2, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S2 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S3, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S3 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			v4s R = gap_pack4(S0, S1, S2, S3);
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[l1*H_In2 + l2+0] = S0;
			Out[l1*H_In2 + l2+1] = S1;
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[l1*H_In2 + l2+0] = S0;
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulScaleScalarSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	int M = *Arg->MulBias;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0*M, Norm), LB));
			Out[l1*H_In2 + l2+1] = gap_min(UB, gap_max(AT_NORM(S1*M, Norm), LB));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0*M, Norm), LB));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulScaleSmallFeat_fps(KerMatMul_fps_T *Arg)

{
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ MulBias = Arg->MulBias;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm+Arg->NormMulBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	unsigned int Iter = Max(0, Last-First);

	for (int i=0; i<Iter/4; i++) {
		int l2 = 4*i+First;
		v4s *pIn2_0 = (v4s *) (In2 + (l2+0)*W_In2), *pIn2_1 = (v4s *) (In2 + (l2+1)*W_In2), *pIn2_2 = (v4s *) (In2 + (l2+2)*W_In2), *pIn2_3 = (v4s *) (In2 + (l2+3)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias, S1=S0, S2=S0, S3=S0;
			for (int c=0; c<W_In1/4; c++) {
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c], V2 = pIn2_2[c], V3 = pIn2_3[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
				S2 = gap_sumdotp4(C0, V2, S2);
				S3 = gap_sumdotp4(C0, V3, S3);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
				S2 += C0 * In2[(l2+2)*W_In2+c];
				S3 += C0 * In2[(l2+3)*W_In2+c];
			}
			int M = MulBias[l1];
			v4s R = gap_pack4(Min(Max(AT_NORM(S0*M, Norm), LB), UB), 
					  Min(Max(AT_NORM(S1*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S2*M, Norm), LB), UB),
					  Min(Max(AT_NORM(S3*M, Norm), LB), UB));
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
				v4s C0 = pIn1[c];
			       	v4s V0 = pIn2_0[c], V1 = pIn2_1[c];
				S0 = gap_sumdotp4(C0, V0, S0);
				S1 = gap_sumdotp4(C0, V1, S1);
			}
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
				S1 += C0 * In2[(l2+1)*W_In2+c];
			}
			int M = MulBias[l1];
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0*M, Norm), LB));
			Out[l1*H_In2 + l2+1] = gap_min(UB, gap_max(AT_NORM(S1*M, Norm), LB));
		}
	}
	if (Iter&0x1) {
		int l2 = Last-1;
		v4s *pIn2 = (v4s *) (In2 + (l2+0)*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v4s *pIn1 = (v4s *) (In1 + l1*W_In1);
			int S0 = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(4*2); c++) {
				v4s C0 = pIn1[2*c], C1 = pIn1[2*c+1];
			       	v4s V0 = pIn2[2*c], V1 = pIn2[2*c+1];
				S0 = gap_sumdotp4(C0, V0, S0);
				S0 = gap_sumdotp4(C1, V1, S0);
			}
			if (W_In1&0x4) S0 = gap_sumdotp4(pIn1[W_In1/4-1], pIn2[W_In1/4-1], S0);
			for (int c=(W_In1/4)*4; c<W_In1; c++) {
				int C0 = In1[l1*W_In1+c];
				S0 += C0 * In2[(l2+0)*W_In2+c];
			}
			int M = MulBias[l1];
			Out[l1*H_In2 + l2+0] = gap_min(UB, gap_max(AT_NORM(S0*M, Norm), LB));
		}
	}
	gap_waitbarrier(0);
}

/* In1 fits completly in shared L1, usually conv weights
   In2 has been transposed before being used, usually conv Features
   Parallelization scheme partition In2 along H_In2
*/
void KerParMatMulSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = gap_min(UB, gap_max(AT_NORM(Acc, Norm), LB));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulHswishSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int C1 = 3<<NormOut;
	int C2 = (1<<15)/6; // 1/6 in Q15
	int UB = 6<<NormOut;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Acc = AT_NORM(Acc, Norm);
			Out[l1*H_In2 + l2] = AT_NORM(AT_NORM(gap_min(gap_max(Acc + C1, 0), UB) * Acc, NormOut) * C2, 15);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulHsigmoidSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int C1 = 3<<NormOut;
	int C2 = (1<<15)/6; // 1/6 in Q15
	int UB = 6<<NormOut;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Acc = gap_max(0, gap_min(AT_NORM(Acc, Norm)+C1, UB));
			Out[l1*H_In2 + l2] = AT_NORM(Acc*C2, 15);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulLeakyreluSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];


			int Input = AT_NORM(Acc, Norm);
			int Neg = (Input<0), Pos = (Input>=0);
			int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
			Out[l1*H_In2 + l2] = gap_clip(Neg*Input1+Pos*Input, 7);
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulScaleScalarSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;
	int M = *Arg->MulBias;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	unsigned int NormMulBias = Arg->NormMulBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = gap_min(UB, gap_max(AT_NORM(AT_NORM(Acc, Norm)*M, NormMulBias), LB));
		}
	}
	gap_waitbarrier(0);
}

void KerParMatMulScaleSmallFeat_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int H_In2 = Arg->W_In2;
	unsigned int W_In2 = W_In1;
	short int * __restrict__ Bias = Arg->Bias;
	short int * __restrict__ MulBias = Arg->MulBias;
	short int * __restrict__ Out = Arg->Out;
	int LB = Arg->LB, UB = Arg->UB;
	unsigned int NormBias = Arg->NormBias;
	unsigned int Norm = Arg->Norm;
	unsigned int NormMulBias = Arg->NormMulBias;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H_In2), First = Chunk*CoreId, Last = Min(First+Chunk, H_In2);
	// for (int l2=0; l2<H_In2; l2++) {
	for (int l2=First; l2<Last; l2++) {
		v2s *pIn2 = (v2s *) (In2 + l2*W_In2);
		for (int l1=0; l1<H_In1; l1++) {
			v2s *pIn1 = (v2s *) (In1 + l1*W_In1);
			int Acc = Bias[l1]<<NormBias;
			for (int c=0; c<W_In1/(2*2); c++) {
				v2s V0 = pIn1[2*c], V1 = pIn1[2*c+1];
				v2s V2 = pIn2[2*c], V3 = pIn2[2*c+1];
				Acc = gap_sumdotp2(V0, V2, Acc);
				Acc = gap_sumdotp2(V1, V3, Acc);
			}
			if (W_In1&0x2) Acc = gap_sumdotp2(pIn1[W_In1/2-1], pIn2[W_In1/2-1], Acc);
			if (W_In1&0x1) Acc += In1[l1*W_In1+W_In1-1] * In2[l2*W_In2+W_In1-1];
			Out[l1*H_In2 + l2] = gap_min(UB, gap_max(AT_NORM(AT_NORM(Acc, Norm)*MulBias[l1], NormMulBias), LB));
		}
	}
	gap_waitbarrier(0);
}

/* Matrix scaling */

void KerParMatScaleVector_fp(KerMatScale_fp_T *Arg)

{
	short int * __restrict__ In	= Arg->In1;
	short int * __restrict__ Scale	= Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		short int * __restrict__ I = In + i*W*H;
		int S = Scale[i];
		short int * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(I[2*j  ]*S, Norm);
			int P2 = AT_NORM(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleScalar_fp(KerMatScale_fp_T *Arg)

{
	short int * __restrict__ In	= Arg->In1;
	int Scale	                = *Arg->ScaleScalar;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		short int * __restrict__ I = In + i*W*H;
		int S = Scale;
		short int * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(I[2*j  ]*S, Norm);
			int P2 = AT_NORM(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleVectorScalar_fp(KerMatScale_fp_T *Arg)

{
	short int * __restrict__ In	= Arg->In1;
	short int * __restrict__ Scale	= Arg->In2;
	int ScaleScalar	                = *Arg->ScaleScalar;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm;
	int Scale_Q			= Arg->Scale_Q;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		short int * __restrict__ I = In + i*W*H;
		int S = Scale[i];
		short int * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(AT_NORM(I[2*j  ]*S, Norm)*ScaleScalar, Scale_Q);
			int P2 = AT_NORM(AT_NORM(I[2*j+1]*S, Norm)*ScaleScalar, Scale_Q);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(AT_NORM(I[W*H-1]*S, Norm)*ScaleScalar, Scale_Q), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleVector_fps(KerMatScale_fps_T *Arg)

{
	signed char * __restrict__ In	= Arg->In1;
	signed char * __restrict__ Scale= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		signed char * __restrict__ I = In + i*W*H;
		int S = Scale[i];
		signed char * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(I[2*j  ]*S, Norm);
			int P2 = AT_NORM(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleScalar_fps(KerMatScale_fps_T *Arg)

{
	signed char * __restrict__ In	= Arg->In1;
	int Scale                       = *Arg->ScaleScalar;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		signed char * __restrict__ I = In + i*W*H;
		int S = Scale;
		signed char * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(I[2*j  ]*S, Norm);
			int P2 = AT_NORM(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleVectorScalar_fps(KerMatScale_fps_T *Arg)

{
	signed char * __restrict__ In	= Arg->In1;
	signed char * __restrict__ Scale= Arg->In2;
	int ScaleScalar                 = *Arg->ScaleScalar;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Norm+Arg->Scale_Q;
	int LB = Arg->LB, UB = Arg->UB;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Arg->N);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, Arg->N);
	int i, j;

	for (i=First; i<Last; i++) {
		signed char * __restrict__ I = In + i*W*H;
		int S = Scale[i];
		signed char * __restrict__ O  = Out + i*W*H;
		for (j=0; j<((W*H)/2); j++) {
			int P1 = AT_NORM(I[2*j  ]*S*ScaleScalar, Norm);
			int P2 = AT_NORM(I[2*j+1]*S*ScaleScalar, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(AT_NORM(I[W*H-1]*S*ScaleScalar, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}


void KerParMatMul_NoBias_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(AT_NORM(S, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulSxSy_NoBias_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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


void KerParMatMul_NoBias_fps(KerMatMul_fps_T *Arg)

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
        unsigned int Norm = Arg->Norm;
        int LB = Arg->LB, UB = Arg->UB;
        int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
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
			v4s R = gap_pack4(Min(Max(AT_NORM(S0, Norm), LB), UB), Min(Max(AT_NORM(S1, Norm), LB), UB), Min(Max(AT_NORM(S2, Norm), LB), UB), Min(Max(AT_NORM(S3, Norm), LB), UB));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = Min(Max(AT_NORM(S1, Norm), LB), UB);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = Min(Max(AT_NORM(S0, Norm), LB), UB);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulSxSy_NoBias_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = 0;
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		   	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(AT_NORM(S, Norm), LB), UB);
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

void KerParMatMulHswish_NoBias_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;


	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			S = AT_NORM(S, Norm);
			Out[(Line+OffLine)*W_Out+Col+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulHswishSxSy_NoBias_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			S = AT_NORM(S, Norm);
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
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

void KerParMatMulHswish_NoBias_fps(KerMatMul_fps_T *Arg)

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
        unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut, LB=0;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
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
			v4s R = gap_pack4(AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S1, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S2 + C1, 0), UB) * S2, NormOut) * C2, 15),
					  AT_NORM(AT_NORM(gap_min(gap_max(S3 + C1, 0), UB) * S3, NormOut) * C2, 15));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S1 + C1, 0), UB) * S0, NormOut) * C2, 15);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_NORM(AT_NORM(gap_min(gap_max(S0 + C1, 0), UB) * S0, NormOut) * C2, 15);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulHswishSxSy_NoBias_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = 0;
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			S = AT_NORM(S, Norm);
		       	Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(AT_NORM(gap_min(gap_max(S + C1, 0), UB) * S, NormOut) * C2, 15);
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



void KerParMatMulHsigmoid_NoBias_fps(KerMatMul_fps_T *Arg)

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
        unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut, LB=0;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
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
			v4s R = gap_pack4(AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S2, Norm)+C1, UB))*C2, 15),
					  AT_NORM(gap_max(0, gap_min(AT_NORM(S3, Norm)+C1, UB))*C2, 15));
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
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S1, Norm)+C1, UB))*C2, 15);
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
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S0, Norm)+C1, UB))*C2, 15);
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulHsigmoidSxSy_NoBias_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = 0;
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
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


void KerParMatMulHsigmoid_NoBias_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulHsigmoidSxSy_NoBias_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int NormOut = Arg->UB;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;
        int C1 = 3<<NormOut;
        int C2 = (1<<15)/6; // 1/6 in Q15
        int UB = 6<<NormOut;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = AT_NORM(gap_max(0, gap_min(AT_NORM(S, Norm)+C1, UB))*C2, 15);
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

void KerParMatMulLeakyrelu_NoBias_fp(KerMatMul_fp_T *Arg)

{
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 15);
		       		Out[(Line+OffLine)*W_Out+Col+OffCol] = Acc0;
			}
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulLeakyreluSxSy_NoBias_fp(KerMatMul_fp_T *Arg)

{
/*
	In1 is usually the Conv1x1 filter set, e,g In1 is [OutFeat][InFeat]
	In2 is  [InFeat][Width*Height]

	When we receive tiles In2 and if StrideY is != 1 tile is always [OutFeat][K*(Width*Scy)]
*/
	short int * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	short int * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	short int * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	short int *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v2s *VBuff = (v2s *) BufferColIn2;

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
		       	v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = 0;
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 15);
				Out[(Line+OffLine)*W_Out+Oo] = Acc0;
			}
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

void KerParMatMulLeakyrelu_NoBias_fps(KerMatMul_fps_T *Arg)

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
        unsigned int Norm = Arg->Norm;
	int ColFirst = Arg->ColFirst;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) BufferColIn2;
        v4s * __restrict__ VBuff1 = (v4s *) (BufferColIn2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (BufferColIn2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (BufferColIn2+3*H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
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
                        int S0 = 0, S1=S0, S2=S0, S3=S0;
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S2, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S2 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S3, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S3 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			v4s R = gap_pack4(S0, S1, S2, S3);
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
				Input = AT_NORM(S1, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S1 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = S0;
			Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = S1;
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
			{
				int Input, Neg, Pos, Input1;
				Input = AT_NORM(S0, Norm); Neg = (Input<0); Pos = (Input>=0); Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT); S0 = gap_clip(Neg*Input1+Pos*Input, 7);
			}
			Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = S0;
                }
                gap_waitbarrier(0);
	}
}

void KerParMatMulLeakyreluSxSy_NoBias_fps(KerMatMul_fps_T *Arg)

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
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	int Pi = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int Wi = Arg->W, Hi = Arg->H;
	int Sx = Arg->Sx, Sy = Arg->Sy;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;

	int Wo  = (Wi+Sx-1)/Sx, Ho = (Hi+Sy-1)/Sy;
	int Oo, OffLine;
	int At, F=0, L = W_In2;

	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(H_In1);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
	unsigned int Ci = ChunkSize(H_In2), Fi = CoreId*Ci, Li  = Min(H_In2, Fi+Ci);

	At = 0; OffLine = 0; Oo = 0;
	if (ColFirst) OffLine = Pi; else Oo = Pi;

	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = 0;
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4-1], VBuff[W_In1/4-1], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			{
				int Input = AT_NORM(S, Norm);
				int Neg = (Input<0), Pos = (Input>=0);
				int Input1 = AT_NORM(Input*LEAK_CONSTANT_FIX, LEAK_CONSTANT_FORMAT);
				int Acc0 = gap_clip(Neg*Input1+Pos*Input, 7);
		       		Out[(Line+OffLine)*W_Out+Oo] = Acc0;
			}
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

#pragma GCC diagnostic pop
