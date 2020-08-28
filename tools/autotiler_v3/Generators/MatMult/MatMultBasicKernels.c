/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap.h"
#include "MatMultBasicKernels.h"


#define Min(a, b)               (((a)<(b))?(a):(b))

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

void KerMatMultParallelf32(KerMatMultParallelf32_ArgT *Arg)

{
        float * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        float * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        float * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W_In2);
        unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

        // for (Col=0; Col<W_In2; Col++) {
        for (Col=First; Col<Last; Col++) {
                for (Line=0; Line<H_In1; Line++) {
                        float S = 0.0;
            for (i=0; i<W_In1/2; i++) {
                S = S + In1[Line*W_In1 + (2*i  )] * In2[(2*i  )*W_In2+Col];
                S = S + In1[Line*W_In1 + (2*i+1)] * In2[(2*i+1)*W_In2+Col];
            }
            if (W_In1&0x1) S += In1[Line*W_In1 + W_In1-1]*In2[(W_In1 - 1)*W_In2];
                        Out[Line*W_Out+Col+OutFirstCol] = S;
        }
        }
        gap_waitbarrier(0);
}
#ifdef __gap9__
void KerMatMultParallelf16(KerMatMultParallelf16_ArgT *Arg)

{
        f16 * __restrict__ In1 = Arg->In1;
        unsigned int W_In1 = Arg->W_In1;
        unsigned int H_In1 = Arg->H_In1;
        f16 * __restrict__ In2 = Arg->In2;
        unsigned int W_In2 = Arg->W_In2;
        f16 * __restrict__ Out = Arg->Out;
        unsigned int W_Out = Arg->W_Out;
        unsigned int OutFirstCol = Arg->OutFirstCol;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W_In2);
        unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

        // for (Col=0; Col<W_In2; Col++) {
        for (Col=First; Col<Last; Col++) {
                for (Line=0; Line<H_In1; Line++) {
                        f16 S = 0.0;
                        for (i=0; i<W_In1; i++) S = S + In1[Line*W_In1 + i] * In2[(i)*W_In2+Col];
                        Out[Line*W_Out+Col+OutFirstCol] = S;
                }
        }
        gap_waitbarrier(0);
}


void KerMatMultParallelVectorialf16(KerMatMultParallelf16_ArgT *Arg)

{
    f16 * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    f16 * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    f16 * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    f16 *BufferColIn2 = Arg->BufferColIn2;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;
    v2h *VBuff = (v2h *) BufferColIn2;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
    unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);

    for (Col=0; Col<W_In2; Col++) {
        for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
            gap_waitbarrier(0);
        for (Line=First; Line<Last; Line++) {
            v2h *VIn1 = (v2h *) (&In1[Line*W_In1 + 0]);
            v2h VS = (v2h) {0.0, 0.0};
            for (i=0; i<(W_In1/4); i++) {
                VS += VIn1[2*i  ] * VBuff[2*i  ];
                VS += VIn1[2*i+1] * VBuff[2*i+1];
            }
            f16 S = VS[0] + VS[1];
            for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
            Out[Line*W_Out+Col+OutFirstCol] = S;
        }
            gap_waitbarrier(0);
    }
}

#endif

void KerMatMultParallel32(KerMatMultParallel32_ArgT *Arg)

{
    int * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    int * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    int * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    unsigned int Norm = Arg->Norm;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W_In2);
        unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

    // for (Col=0; Col<W_In2; Col++) {
    for (Col=First; Col<Last; Col++) {
        for (Line=0; Line<H_In1; Line++) {
            int S = 0;
            for (i=0; i<W_In1/2; i++) {
                S = S + In1[Line*W_In1 + (2*i  )] * In2[(2*i  )*W_In2+Col];
                S = S + In1[Line*W_In1 + (2*i+1)] * In2[(2*i+1)*W_In2+Col];
            }
            if (W_In1&0x1) S += In1[Line*W_In1 + W_In1-1]*In2[(W_In1 - 1)*W_In2];
            Out[Line*W_Out+Col+OutFirstCol] = gap_roundnorm_reg(S, Norm);
        }
    }
        gap_waitbarrier(0);
}

void KerMatMultParallel16(KerMatMultParallel16_ArgT *Arg)

{
    short int * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    short int * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    short int * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    unsigned int Norm = Arg->Norm;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W_In2);
        unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

    // for (Col=0; Col<W_In2; Col++) {
    for (Col=First; Col<Last; Col++) {
        for (Line=0; Line<H_In1; Line++) {
            int S = 0;
            for (i=0; i<W_In1/2; i++) {
                S = S + In1[Line*W_In1 + (2*i  )] * In2[(2*i  )*W_In2+Col];
                S = S + In1[Line*W_In1 + (2*i+1)] * In2[(2*i+1)*W_In2+Col];
            }
            if (W_In1&0x1) S += In1[Line*W_In1 + W_In1-1]*In2[(W_In1 - 1)*W_In2];
            Out[Line*W_Out+Col+OutFirstCol] = gap_clip(gap_roundnorm_reg(S, Norm), 15);
        }
    }
        gap_waitbarrier(0);
}

void KerMatMultParallelVectorial16(KerMatMultParallel16_ArgT *Arg)

{
    short int * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    short int * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    short int * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    short int *BufferColIn2 = Arg->BufferColIn2;
    unsigned int Norm = Arg->Norm;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;
    v2s *VBuff = (v2s *) BufferColIn2;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
    unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);

    for (Col=0; Col<W_In2; Col++) {
        // for (i=0;i<H_In2; i++) BufferColIn2[i] = In2[i*W_In2+Col];
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
            Out[Line*W_Out+Col+OutFirstCol] = gap_clip(gap_roundnorm_reg(S, Norm), 15);
        }
            gap_waitbarrier(0);
    }
}


void KerMatMultParallel8(KerMatMultParallel8_ArgT *Arg)

{
    signed char * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    signed char * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    signed char * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    unsigned int Norm = Arg->Norm;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(W_In2);
        unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

    // for (Col=0; Col<W_In2; Col++) {
    for (Col=First; Col<Last; Col++) {
        for (Line=0; Line<H_In1; Line++) {
            int S = 0;
            for (i=0; i<W_In1; i++) S = S + In1[Line*W_In1 + (i  )] * In2[(i  )*W_In2+Col];
            Out[Line*W_Out+Col+OutFirstCol] = gap_clip(gap_roundnorm_reg(S, Norm), 7);
        }
    }
        gap_waitbarrier(0);
}

void KerMatMultParallelVectorial8(KerMatMultParallel8_ArgT *Arg)

{
    signed char * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    signed char * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    signed char * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;
    signed char *BufferColIn2 = Arg->BufferColIn2;
    unsigned int Norm = Arg->Norm;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;
    v4s *VBuff = (v4s *) BufferColIn2;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
    unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);

    for (Col=0; Col<W_In2; Col++) {
        // for (i=0;i<H_In2; i++) BufferColIn2[i] = In2[i*W_In2+Col];
        for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
            gap_waitbarrier(0);
        // for (Line=0; Line<H_In1; Line++) {
        for (Line=First; Line<Last; Line++) {
            v4s *VIn1 = (v4s *) (&In1[Line*W_In1 + 0]);
            int S = 0;
            for (i=0; i<(W_In1/4); i++) S = gap_sumdotp4(VIn1[i], VBuff[i], S);
            for (i=(W_In1/4)*4; i<W_In1; i++) S += In1[Line*W_In1 + i] * BufferColIn2[i];
            Out[Line*W_Out+Col+OutFirstCol] = gap_clip(gap_roundnorm_reg(S, Norm), 7);
        }
            gap_waitbarrier(0);
    }
}
