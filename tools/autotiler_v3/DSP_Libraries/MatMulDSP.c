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

void KerParMatMulDSP_fp16(MatMul_DSP_T *Arg)

{
	F16_DSP * __restrict__ In1 = (F16_DSP * __restrict__) Arg->In1;
	F16_DSP * __restrict__ In2 = (F16_DSP * __restrict__) Arg->In2;
	F16_DSP * __restrict__ Out = (F16_DSP * __restrict__) Arg->Out;
	F16_DSP *BufferColIn2      = (F16_DSP *) Arg->BufferColIn2;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
	unsigned int W_Out = Arg->W_Out;
	unsigned int OutFirstCol = Arg->OutFirstCol;
	int ColFirst = Arg->ColFirst;

	unsigned int H_In2 = W_In1;
	unsigned int H_Out = H_In1;
	unsigned int Line, Col, i;
	F16V_DSP *VBuff1 = (F16V_DSP *) (&BufferColIn2[0]);
	F16V_DSP *VBuff2 = (F16V_DSP *) (&BufferColIn2[1*H_In2]);
	F16V_DSP *VBuff3 = (F16V_DSP *) (&BufferColIn2[2*H_In2]);
	F16V_DSP *VBuff4 = (F16V_DSP *) (&BufferColIn2[3*H_In2]);

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
	        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V_DSP *VIn2 = (F16V_DSP *) (&In1[(l1+1)*W_In1 + 0]);
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0}, S5 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S2 = (F16V_DSP) {0.0, 0.0}, S6 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S3 = (F16V_DSP) {0.0, 0.0}, S7 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S4 = (F16V_DSP) {0.0, 0.0}, S8 = (F16V_DSP) {0.0, 0.0};
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
			F16_DSP s1 = S1[0]+S1[1];
			F16_DSP s2 = S2[0]+S2[1];
			F16_DSP s3 = S3[0]+S3[1];
			F16_DSP s4 = S4[0]+S4[1];
			F16_DSP s5 = S5[0]+S5[1];
			F16_DSP s6 = S6[0]+S6[1];
			F16_DSP s7 = S7[0]+S7[1];
			F16_DSP s8 = S8[0]+S8[1];
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
	        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S2 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S3 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S4 = (F16V_DSP) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
				S4 += VIn1[i] * VBuff4[i];
			}
			F16_DSP s1 = S1[0]+S1[1];
			F16_DSP s2 = S2[0]+S2[1];
			F16_DSP s3 = S3[0]+S3[1];
			F16_DSP s4 = S4[0]+S4[1];
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
	        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
	        	F16V_DSP *VIn2 = (F16V_DSP *) (&In1[(l1+1)*W_In1 + 0]);
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0}, S5 = (F16V_DSP) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S1 += VIn1[i] * VBuff1[i];
				S5 += VIn2[i] * VBuff1[i];
			}
			F16_DSP s1 = S1[0]+S1[1];
			F16_DSP s5 = S5[0]+S5[1];
			if (W_In1&0x1) {
				s1 += In1[(l1  )*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
				s5 += In1[(l1+1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = s5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
	        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) S1 += VIn1[i] * VBuff1[i];
			F16_DSP s1 = S1[0]+S1[1];
			if (W_In1&0x1) s1 += In1[(l1)*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = s1;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulDSPT_fp16(MatMul_DSP_T *Arg)

{
	F16_DSP * __restrict__ In1 = (F16_DSP * __restrict__) Arg->In1;
	F16_DSP * __restrict__ In2 = (F16_DSP * __restrict__) Arg->In2;
	F16_DSP * __restrict__ Out = (F16_DSP * __restrict__) Arg->Out;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
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
	F16_DSP * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	F16_DSP *pIn2 = In2;
        	int l1 = 2*Line + First;
        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
        	F16V_DSP *VIn2 = (F16V_DSP *) (&In1[(l1+1)*W_In1 + 0]);
		for (Col=0; Col<W_In2/4; Col++) {
			F16V_DSP *VBuff0 = (F16V_DSP *) (pIn2);
			F16V_DSP *VBuff1 = (F16V_DSP *) (pIn2+H_In2);
			F16V_DSP *VBuff2 = (F16V_DSP *) (pIn2+2*H_In2);
			F16V_DSP *VBuff3 = (F16V_DSP *) (pIn2+3*H_In2);
			F16V_DSP S0 = (F16V_DSP) {0.0, 0.0}, S4=S0;
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0}, S5=S1;
			F16V_DSP S2 = (F16V_DSP) {0.0, 0.0}, S6=S2;
			F16V_DSP S3 = (F16V_DSP) {0.0, 0.0}, S7=S3;
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
			F16_DSP s0 = S0[0]+S0[1];
			F16_DSP s1 = S1[0]+S1[1];
			F16_DSP s2 = S2[0]+S2[1];
			F16_DSP s3 = S3[0]+S3[1];
			F16_DSP s4 = S4[0]+S4[1];
			F16_DSP s5 = S5[0]+S5[1];
			F16_DSP s6 = S6[0]+S6[1];
			F16_DSP s7 = S7[0]+S7[1];
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
			F16V_DSP *VBuff0 = (F16V_DSP *) (pIn2);
			F16V_DSP S0 = (F16V_DSP) {0.0, 0.0}, S4=S0;
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S4 += VIn2[i] * VBuff0[i];
			}
			F16_DSP s0 = S0[0]+S0[1];
			F16_DSP s4 = S4[0]+S4[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
				s4 += In1[(l1+1)*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[      (W_In2-1)] = s0;
		       	pOut[W_Out+(W_In2-1)] = s4;;
		       	pIn2 += H_In2;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	F16_DSP *pIn2 = In2;
        	int l1 = Last-1;
        	F16V_DSP *VIn1 = (F16V_DSP *) (&In1[(l1  )*W_In1 + 0]);
        	for (Col=0; Col<W_In2/4; Col++) {
			F16V_DSP *VBuff0 = (F16V_DSP *) (pIn2);
			F16V_DSP *VBuff1 = (F16V_DSP *) (pIn2+H_In2);
			F16V_DSP *VBuff2 = (F16V_DSP *) (pIn2+2*H_In2);
			F16V_DSP *VBuff3 = (F16V_DSP *) (pIn2+3*H_In2);
			F16V_DSP S0 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S1 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S2 = (F16V_DSP) {0.0, 0.0};
			F16V_DSP S3 = (F16V_DSP) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
				S1 += VIn1[i] * VBuff1[i];
				S2 += VIn1[i] * VBuff2[i];
				S3 += VIn1[i] * VBuff3[i];
			}
			F16_DSP s0 = S0[0]+S0[1];
			F16_DSP s1 = S1[0]+S1[1];
			F16_DSP s2 = S2[0]+S2[1];
			F16_DSP s3 = S3[0]+S3[1];
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
			F16V_DSP *VBuff0 = (F16V_DSP *) (pIn2);
			F16V_DSP S0 = (F16V_DSP) {0.0, 0.0};
			for (i=0; i<W_In1/2; i++) {
				S0 += VIn1[i] * VBuff0[i];
			}
			F16_DSP s0 = S0[0]+S0[1];
			if (W_In1&0x1) {
				s0 += In1[(l1  )*W_In1 + W_In1-1] * pIn2[W_In1-1];
			}
		       	pOut[(W_In2-1)] = s0;;
		       	pIn2 += H_In2;
		}
        }
	gap_waitbarrier(0);
}


void KerParMatMulDSP_fp32(MatMul_DSP_T *Arg)

{
	float * __restrict__ In1 = (float *__restrict__) Arg->In1;
	float * __restrict__ In2 = (float *__restrict__) Arg->In2;
	float * __restrict__ Out = (float *__restrict__) Arg->Out;
	float *BufferColIn2      = (float *) Arg->BufferColIn2;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
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
			float S1 = 0.0, S5 = 0.0;
			float S2 = 0.0, S6 = 0.0;
			float S3 = 0.0, S7 = 0.0;
			float S4 = 0.0, S8 = 0.0;
			for (i=0; i<W_In1; i++) {
				S1 += In1[(l1  )*W_In1+i] * BufferColIn2[i        ];
				S2 += In1[(l1  )*W_In1+i] * BufferColIn2[i+1*H_In2];
				S3 += In1[(l1  )*W_In1+i] * BufferColIn2[i+2*H_In2];
				S4 += In1[(l1  )*W_In1+i] * BufferColIn2[i+3*H_In2];

				S5 += In1[(l1+1)*W_In1+i] * BufferColIn2[i        ];
				S6 += In1[(l1+1)*W_In1+i] * BufferColIn2[i+1*H_In2];
				S7 += In1[(l1+1)*W_In1+i] * BufferColIn2[i+2*H_In2];
				S8 += In1[(l1+1)*W_In1+i] * BufferColIn2[i+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = S1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = S2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = S3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = S4;
		       	Out[(l1+1+OffLine)*W_Out+4*Col  +OffCol] = S5;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+1+OffCol] = S6;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+2+OffCol] = S7;
		       	Out[(l1+1+OffLine)*W_Out+4*Col+3+OffCol] = S8;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
			float S1 = 0.0;
			float S2 = 0.0;
			float S3 = 0.0;
			float S4 = 0.0;
			for (i=0; i<W_In1; i++) {
				S1 += In1[(l1  )*W_In1+i] * BufferColIn2[i        ];
				S2 += In1[(l1  )*W_In1+i] * BufferColIn2[i+1*H_In2];
				S3 += In1[(l1  )*W_In1+i] * BufferColIn2[i+2*H_In2];
				S4 += In1[(l1  )*W_In1+i] * BufferColIn2[i+3*H_In2];
			}
		       	Out[(l1  +OffLine)*W_Out+4*Col  +OffCol] = S1;
		       	Out[(l1  +OffLine)*W_Out+4*Col+1+OffCol] = S2;
		       	Out[(l1  +OffLine)*W_Out+4*Col+2+OffCol] = S3;
		       	Out[(l1  +OffLine)*W_Out+4*Col+3+OffCol] = S4;
		}
		gap_waitbarrier(0);
	}
	for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
		for (i=F; i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
		gap_waitbarrier(0);
		for (Line=0; Line<Iter/2; Line++) {
	        	int l1 = 2*Line + First;
			float S1 = 0.0, S5 = 0.0;
			for (i=0; i<W_In1; i++) {
				S1 += In1[(l1  )*W_In1+i] * BufferColIn2[i];
				S5 += In1[(l1+1)*W_In1+i] * BufferColIn2[i];
			}
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = S1;
		       	Out[(l1+1+OffLine)*W_Out+Col  +OffCol] = S5;
		}
		if (Iter&0x1) {
			int l1 = Last-1;
			float S1 = 0.0;
			for (i=0; i<W_In1; i++) S1 += In1[l1*W_In1+i] * BufferColIn2[i];
		       	Out[(l1  +OffLine)*W_Out+Col  +OffCol] = S1;
		}
		gap_waitbarrier(0);
	}
}

void KerParMatMulDSPT_fp32(MatMul_DSP_T *Arg)

{
	float * __restrict__ In1 = (float *__restrict__) Arg->In1;
	float * __restrict__ In2 = (float *__restrict__) Arg->In2;
	float * __restrict__ Out = (float *__restrict__) Arg->Out;
	unsigned int W_In1 = Arg->W_In1;
	unsigned int H_In1 = Arg->H_In1;
	unsigned int W_In2 = Arg->W_In2; 	/* H_In2 = W_In1 by construction */
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
	float * pOut = Out + (OffLine+First)*W_Out + OffCol;
	for (Line=0; Line<Iter/2; Line++) {
        	float *pIn2 = In2;
        	int l1 = 2*Line + First;
		for (Col=0; Col<W_In2/4; Col++) {
			float S0 = 0.0, S4 = 0.0;
			float S1 = 0.0, S5 = 0.0;
			float S2 = 0.0, S6 = 0.0;
			float S3 = 0.0, S7 = 0.0;
			for (i=0; i<W_In1; i++) {
				S0 += In1[(l1  )*W_In1] * pIn2[i        ];
				S1 += In1[(l1  )*W_In1] * pIn2[i+  H_In2];
				S2 += In1[(l1  )*W_In1] * pIn2[i+2*H_In2];
				S3 += In1[(l1  )*W_In1] * pIn2[i+3*H_In2];

				S4 += In1[(l1+1)*W_In1] * pIn2[i        ];
				S5 += In1[(l1+1)*W_In1] * pIn2[i+  H_In2];
				S6 += In1[(l1+1)*W_In1] * pIn2[i+2*H_In2];
				S7 += In1[(l1+1)*W_In1] * pIn2[i+3*H_In2];
			}
		       	pOut[      (4*Col  )] = S0;
		       	pOut[      (4*Col+1)] = S1;
		       	pOut[      (4*Col+2)] = S2;
		       	pOut[      (4*Col+3)] = S3;
		       	pOut[W_Out+(4*Col  )] = S4;
		       	pOut[W_Out+(4*Col+1)] = S5;
		       	pOut[W_Out+(4*Col+2)] = S6;
		       	pOut[W_Out+(4*Col+3)] = S7;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			float S0 = 0.0, S4 = 0.0;
			for (i=0; i<W_In1; i++) {
				S0 += In1[(l1  )*W_In1] * pIn2[i];
				S4 += In1[(l1+1)*W_In1] * pIn2[i];
			}
		       	pOut[      (W_In2-1)] = S0;
		       	pOut[W_Out+(W_In2-1)] = S4;
		       	pIn2 += H_In2;
		}
		pOut += 2*W_Out;
	}
	if (Iter&0x1) {
        	float *pIn2 = In2;
        	int l1 = Last-1;
        	for (Col=0; Col<W_In2/4; Col++) {
			float S0 = 0.0;
			float S1 = 0.0;
			float S2 = 0.0;
			float S3 = 0.0;
			for (i=0; i<W_In1; i++) {
				S0 += In1[(l1  )*W_In1] * pIn2[i        ];
				S1 += In1[(l1  )*W_In1] * pIn2[i+  H_In2];
				S2 += In1[(l1  )*W_In1] * pIn2[i+2*H_In2];
				S3 += In1[(l1  )*W_In1] * pIn2[i+3*H_In2];
			}
		       	pOut[      (4*Col  )] = S0;
		       	pOut[      (4*Col+1)] = S1;
		       	pOut[      (4*Col+2)] = S2;
		       	pOut[      (4*Col+3)] = S3;
		       	pIn2 += 4*H_In2;
		}
		for (Col=(W_In2/4)*4; Col<W_In2; Col++) {
			float S0 = 0.0;
			for (i=0; i<W_In1; i++) {
				S0 += In1[(l1  )*W_In1] * pIn2[i];
			}
		       	pOut[      (W_In2-1)] = S0;
		       	pIn2 += H_In2;
		}
        }
	gap_waitbarrier(0);
}
