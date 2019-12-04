#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels.h"

#define Minu(a, b)	      (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

#ifdef __pulp__
#define Abs(a)			__builtin_pulp_abs((a))
#define Min(a, b)	       __builtin_pulp_minsi((a), (b))
#define Max(a, b)	       __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)			(((int)(a)<0)?(-(a)):(a))
#define Min(a, b)	       (((a)<(b))?(a):(b))
#define Max(a, b)	       (((a)>(b))?(a):(b))
#endif

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
	int OffOut = Out_Q-Max(In1_Q, In2_Q);

	if (In1_Q>In2_Q) {
		I1 = In2 + First*W*H; I2 = In1 + First*W*H;
	} else {
		I1 = In1 + First*W*H; I2 = In2 + First*W*H;
	}
	if (OffOut==0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max((I1[2*j]<<OffIn)+I2[2*j], LB), UB); O[2*j+1] = Min(Max((I1[2*j+1]<<OffIn)+I2[2*j+1], LB), UB);
			}
			O[W*H-1] = Min(Max((I1[W*H-1]<<OffIn) + I2[W*H-1], LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else if (OffOut>0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]<<OffIn)+I2[2*j])<<OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]<<OffIn)+I2[2*j+1])<<OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]<<OffIn) + I2[W*H-1])<<OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]<<OffIn)+I2[2*j])>>OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]<<OffIn)+I2[2*j+1])>>OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]<<OffIn) + I2[W*H-1])>>OffOut, LB), UB);
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
	int OffOut = Out_Q-Max(In1_Q, In2_Q);

	if (In1_Q>In2_Q) {
		I1 = In2 + First*W*H; I2 = In1 + First*W*H;
	} else {
		I1 = In1 + First*W*H; I2 = In2 + First*W*H;
	}
	if (OffOut==0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max((I1[2*j]<<OffIn)+I2[2*j], LB), UB); O[2*j+1] = Min(Max((I1[2*j+1]<<OffIn)+I2[2*j+1], LB), UB);
			}
			O[W*H-1] = Min(Max((I1[W*H-1]<<OffIn) + I2[W*H-1], LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else if (OffOut>0) {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]<<OffIn)+I2[2*j])<<OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]<<OffIn)+I2[2*j+1])<<OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]<<OffIn) + I2[W*H-1])<<OffOut, LB), UB);
			I1 += W*H; I2 += W*H; O += W*H;
		}
	} else {
		for (i=First; i<Last; i++) {
			for (j=0; j<((W*H)/2); j++) {
				O[2*j] = Min(Max(((I1[2*j]<<OffIn)+I2[2*j])>>OffOut, LB), UB); O[2*j+1] = Min(Max(((I1[2*j+1]<<OffIn)+I2[2*j+1])>>OffOut, LB), UB);
			}
			O[W*H-1] = Min(Max(((I1[W*H-1]<<OffIn) + I2[W*H-1])>>OffOut, LB), UB);
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
			int S = (Bias[Line]<<Norm);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
			int S = (Bias[Line]<<Norm);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
			int S = (Bias[Line]);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
		}
		gap_waitbarrier(0);
	}
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
			int S = (Bias[Line]);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		   	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<Norm);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
		       	int S = (Bias[Line]<<Norm);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		   	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
		       	int S = (Bias[Line]);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
	       		Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S, Norm), LB), UB);
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
			Acc = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(P, Norm)*M, Norm), LB), UB);
			break;
		case 1: /* H Sigmoid */
			/* C1 = (1<<(2*Norm))-1; C2 = (1<<Norm)-1; */
			Acc = (P+C1)>>1;
                        Acc = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc, Norm)));
			break;
		case 2: /* H Swish */
			/* C1 = 3<<(2*Norm); C2 = (1<<16)/6; 1/6 in Q16 */
			Acc = gap_roundnorm_reg(gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(P + C1, 0), UB), Norm) * P, Norm) * C2, 16);
			break;
		default:
			Acc = Min(Max(gap_roundnorm_reg(P, Norm), LB), UB);

	}
	return Acc;
}

static inline int __attribute__((always_inline)) MatMulReduct_fps(int P, int C1, int C2, int M, int Norm, int LB, int UB, int Oper)

{
	int Acc;
	switch (Oper) {
		case 0: /* Channel Scale */
			/* Norm = 2*Norm */
			Acc = Min(Max(gap_roundnorm_reg(P*M, Norm), LB), UB);
			break;
		case 1: /* H Sigmoid */
			/* C1 = (1<<(2*Norm))-1; C2 = (1<<Norm)-1; */
			Acc = (P+C1)>>1;
                        Acc = gap_max(0, gap_min(C2, gap_roundnorm_reg(Acc, Norm)));
			break;
		case 2: /* H Swish */
			/* Norm = 2*Norm; C1 = 3<<(2*Norm); C2 = (1<<16)/6; 1/6 in Q16 */
			Acc = gap_roundnorm_reg(gap_roundnorm_reg(gap_min(gap_max(P + C1, 0), UB) * P, Norm) * C2, 16);
			break;
		default:
			Acc = Min(Max(gap_roundnorm_reg(P, Norm), LB), UB);

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
			int S = (Bias[Line]<<Norm);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
			int S = (Bias[Line]<<Norm);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
			int S = (Bias[Line]);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
			int S = (Bias[Line]);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	signed char * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<Norm);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
		       	int S = (Bias[Line]<<Norm);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;
	int M = *Arg->MulBias;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
		       	int S = (Bias[Line]);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
			int S = (Bias[Line]<<Norm);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
			int S = (Bias[Line]<<Norm);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
	short int * __restrict__ MulBias = Arg->MulBias;
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
		for (Line=First; Line<Last; Line++) {
			v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]);
			for (i=0; i<W_In1/4; i++) {
				S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
				S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
	short int * __restrict__ MulBias = Arg->MulBias;
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
			int S = (Bias[Line]);
		       	for (i=0; i<W_In1/4; i++) {
			       	S = gap_sumdotp2(VIn1[2*i  ], VBuff[2*i  ], S);
			       	S = gap_sumdotp2(VIn1[2*i+1], VBuff[2*i+1], S);
		       	}
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
			Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(gap_roundnorm_reg(S, Norm)*M, Norm), LB), UB);
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
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]<<Norm);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
		       	int S = (Bias[Line]<<Norm);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
	signed char * __restrict__ In1 = Arg->In1;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	signed char * __restrict__ In2 = Arg->In2;
	unsigned int W_In2 = Arg->W_In2;
	short int * __restrict__ Bias = Arg->Bias;
	signed char * __restrict__ MulBias = Arg->MulBias;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	signed char *BufferColIn2 = Arg->BufferColIn2;
	unsigned int Norm = Arg->Norm;
	int LB = Arg->LB, UB = Arg->UB;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	v4s *VBuff = (v4s *) BufferColIn2;

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
			v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
			int S = (Bias[Line]);
			for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
			for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Col+OffCol] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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
	signed char * __restrict__ MulBias = Arg->MulBias;
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

	At=0; OffLine=0; Oo=0;
	if (ColFirst) OffLine=Pi; else Oo=Pi;
	while (L>0) {
	       	for (i=Fi;i<Li; i++) BufferColIn2[i] = In2[i*W_In2+At];
	       	gap_waitbarrier(0);
	       	for (Line=First; Line<Last; Line++) {
		       	v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
		       	int S = (Bias[Line]);
		       	for (i=0; i<(W_In1/(4*2)); i++) {
				S = gap_sumdotp4(VIn1[2*i], VBuff[2*i], S);
				S = gap_sumdotp4(VIn1[2*i+1], VBuff[2*i+1], S);
			}
			if (W_In1&0x4) S = gap_sumdotp4(VIn1[W_In1/4], VBuff[W_In1/4], S);
		       	for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
			int M = MulBias[Line];
		       	Out[(Line+OffLine)*W_Out+Oo] = Min(Max(gap_roundnorm_reg(S*M, 2*Norm), LB), UB);
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

/* Matrix scaling */
void KerParMatScale_fp(KerMat3_fp_T *Arg)

{
	short int * __restrict__ In	= Arg->In1;
	short int * __restrict__ Scale	= Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Out_Q;
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
			int P1 = gap_roundnorm_reg(I[2*j  ]*S, Norm);
			int P2 = gap_roundnorm_reg(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(gap_roundnorm_reg(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}


void KerParMatScaleScalar_fp(KerMat3_fp_T *Arg)

{
	short int * __restrict__ In	= Arg->In1;
	int Scale	                = *Arg->In2;
	short int * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Out_Q;
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
			int P1 = gap_roundnorm_reg(I[2*j  ]*S, Norm);
			int P2 = gap_roundnorm_reg(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(gap_roundnorm_reg(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScale_fps(KerMat3_fps_T *Arg)

{
	signed char * __restrict__ In	= Arg->In1;
	signed char * __restrict__ Scale= Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Out_Q;
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
			int P1 = gap_roundnorm_reg(I[2*j  ]*S, Norm);
			int P2 = gap_roundnorm_reg(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(gap_roundnorm_reg(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}

void KerParMatScaleScalar_fps(KerMat3_fps_T *Arg)

{
	signed char * __restrict__ In	= Arg->In1;
	int Scale                       = *Arg->In2;
	signed char * __restrict__ Out	= Arg->Out;
	int W				= Arg->W;
	int H				= Arg->H;
	int Norm			= Arg->Out_Q;
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
			int P1 = gap_roundnorm_reg(I[2*j  ]*S, Norm);
			int P2 = gap_roundnorm_reg(I[2*j+1]*S, Norm);
			O[2*j  ] = Min(Max(P1, LB), UB);
			O[2*j+1] = Min(Max(P2, LB), UB);
		}
		O[W*H-1] = Min(Max(gap_roundnorm_reg(I[W*H-1]*S, Norm), LB), UB);
	}
	gap_waitbarrier(0);
}
