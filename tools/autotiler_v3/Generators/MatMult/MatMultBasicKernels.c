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


#ifndef Min
  #define Min(a, b)               (((a)<(b))?(a):(b))
#endif
#ifndef Max
  #define Max(a, b)               (((a)>(b))?(a):(b))
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

void KerMatMultParallelVectorialf16_v2(KerMatMultParallelf16_ArgT *Arg)

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
        v2h *VBuff1 = (v2h *) BufferColIn2;
        v2h *VBuff2 = (v2h *) BufferColIn2+H_In2;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);

        for (Col=0; Col<W_In2; Col++) {
                for (i=F;i<L; i++) {
                        BufferColIn2[i]       = In2[i*W_In2+2*Col];
                        BufferColIn2[i+H_In2] = In2[i*W_In2+2*Col+1];
                }
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v2h *VIn1_1 = (v2h *) (&In1[Line*W_In1 + 0]);
                        v2h *VIn1_2 = (v2h *) (&In1[(Line+1)*W_In1 + 0]);
                        v2h VS1 = (v2h) {0.0, 0.0};
                        v2h VS2 = (v2h) {0.0, 0.0};
                        for (i=0; i<(W_In1/4); i++) {
                                VS1 += VIn1_1[2*i  ] * VBuff1[2*i  ];
                                VS1 += VIn1_1[2*i+1] * VBuff1[2*i+1];
                                VS2 += VIn1_2[2*i  ] * VBuff2[2*i  ];
                                VS2 += VIn1_2[2*i+1] * VBuff2[2*i+1];
                        }
                        f16 S1 = VS1[0] + VS1[1];
                        f16 S2 = VS2[0] + VS2[1];
                        for (i=(W_In1/4)*4; i<W_In1; i++) {
                                S1 += In1[Line*W_In1 + i] * BufferColIn2[i];
                                S2 += In1[(Line+1)*W_In1 + i] * BufferColIn2[i+H_In2];
                        }
                        Out[Line*W_Out+Col+OutFirstCol] = S1;
                        Out[(Line+1)*W_Out+Col+OutFirstCol] = S2;
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

void KerMatMultParallelVectorial16_v2(KerMatMultParallel16_ArgT *Arg)

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
        v2s * __restrict__ VBuff0 = (v2s *) BufferColIn2;
        v2s * __restrict__ VBuff1 = (v2s *) (BufferColIn2+H_In2);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(H_In1);
        unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);

        for (Col=0; Col<W_In2/2; Col++) {
                for (i=F;i<L; i++) {
                        int X0 = In2[i*W_In2+2*Col+0];
                        int X1 = In2[i*W_In2+2*Col+1];
                        BufferColIn2[i+0*H_In2] = X0;
                        BufferColIn2[i+1*H_In2] = X1;
                }
                gap_waitbarrier(0);
                for (Line=0; Line<Iter/2; Line++) {
                        int l1 = 2*Line + First;
                        v2s *VIn1 = (v2s *) (&In1[(l1  )*W_In1 + 0]);
                        v2s *VIn2 = (v2s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = 0, S1=0, S2=0, S3=0;
                        for (i=0; i<(W_In1/2); i++) {
                                v2s V0 = VIn1[i];
                                v2s V1 = VIn2[i];
                                v2s A = VBuff0[i];
                                v2s B = VBuff1[i];
                                S0 = gap_sumdotp2(V0, A, S0);
                                S1 = gap_sumdotp2(V0, B, S1);
                                S2 = gap_sumdotp2(V1, A, S2);
                                S3 = gap_sumdotp2(V1, B, S3);
                        }
                        if (W_In1&0x1) {
                                int V0 = In1[l1*W_In1 + W_In1-1];
                                int V1 = In1[(l1+1)*W_In1 + W_In1-1];
                                int X0 = BufferColIn2[W_In1-1]; 
                                int X1 = BufferColIn2[W_In1-1+1*H_In2];
                                S0 += V0 * X0;
                                S1 += V0 * X1;
                                S2 += V1 * X0;
                                S3 += V1 * X1;
                        }
                        Out[(l1  )*W_Out+2*Col+OutFirstCol  ] = gap_clip(gap_roundnorm_reg(S0, Norm), 15);
                        Out[(l1  )*W_Out+2*Col+OutFirstCol+1] = gap_clip(gap_roundnorm_reg(S1, Norm), 15);
                        Out[(l1+1)*W_Out+2*Col+OutFirstCol  ] = gap_clip(gap_roundnorm_reg(S2, Norm), 15);
                        Out[(l1+1)*W_Out+2*Col+OutFirstCol+1] = gap_clip(gap_roundnorm_reg(S3, Norm), 15);
                }
                if (Iter&0x1){
                        int l1 = Last-1;
                        v2s *VIn1 = (v2s *) (&In1[l1*W_In1 + 0]);
                        int S0 =0, S1 = 0;
                        for (i=0; i<W_In1/2; i++) {
                                v2s V0 = VIn1[i];
                                v2s A = VBuff0[i];
                                v2s B = VBuff1[i];
                                S0 = gap_sumdotp2(V0, A, S0);
                                S1 = gap_sumdotp2(V0, B, S1);
                        }
                        if (W_In1&0x1) {
                                int V0 = In1[l1*W_In1 + W_In1-1];
                                int X0 = BufferColIn2[W_In1-1]; 
                                int X1 = BufferColIn2[W_In1-1+1*H_In2];
                                S0 += V0 * X0;
                                S1 += V0 * X1;
                        }
                        Out[l1*W_Out+2*Col+OutFirstCol  ] = gap_clip(gap_roundnorm_reg(S0, Norm), 15);
                        Out[l1*W_Out+2*Col+OutFirstCol+1] = gap_clip(gap_roundnorm_reg(S1, Norm), 15);
                }
                gap_waitbarrier(0);
        }
        if (W_In2&0x1) {
                Col = W_In2-1;
                for (i=F;i<L; i++) {
                        BufferColIn2[i] = In2[i*W_In2+Col];
                }
                gap_waitbarrier(0);
                for (Line=First; Line<Last; Line++) {
                        v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
                        int S1 = 0;
                        for (i=0; i<W_In1/2; i++) {
                                S1 = gap_sumdotp2(VIn1[i], VBuff1[i], S1);
                        }
                        if (W_In1&0x1) {
                                S1 += In1[Line*W_In1 + W_In1-1] * BufferColIn2[W_In1-1];
                        }
                        Out[Line*W_Out+Col+OutFirstCol] = gap_clip(gap_roundnorm_reg(S1, Norm), 15);
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

void KerMatMultParallelVectorial8_4Buff(KerMatMultParallel8_ArgT *Arg)

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
        int OffLine = 0, OffCol = OutFirstCol;

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
                        int S0 = 0, S1=0, S2=0, S3=0;
                        int S4 = 0, S5=0, S6=0, S7=0;
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
                        v4s R1 = gap_pack4(gap_clip(gap_roundnorm_reg(S0, Norm), 7), gap_clip(gap_roundnorm_reg(S1, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S2, Norm), 7), gap_clip(gap_roundnorm_reg(S3, Norm), 7));
                        v4s R2 = gap_pack4(gap_clip(gap_roundnorm_reg(S4, Norm), 7), gap_clip(gap_roundnorm_reg(S5, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S6, Norm), 7), gap_clip(gap_roundnorm_reg(S7, Norm), 7));
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
                        v4s R1 = gap_pack4(gap_clip(gap_roundnorm_reg(S0, Norm), 7), gap_clip(gap_roundnorm_reg(S1, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S2, Norm), 7), gap_clip(gap_roundnorm_reg(S3, Norm), 7));
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
                        Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(gap_roundnorm_reg(S0, Norm), 7);
                        Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(gap_roundnorm_reg(S1, Norm), 7);
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
                        Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(gap_roundnorm_reg(S0, Norm), 7);
                }
                gap_waitbarrier(0);
        }
}

void KerMatMultParallelVectorial8_HWxWH(KerMatMultParallel8_ArgT *Arg)

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
        unsigned int Norm = Arg->Norm;

        unsigned int H_In2 = W_In1;
        unsigned int H_Out = H_In1;
        unsigned int Line, Col, i;
        v4s * __restrict__ VBuff0 = (v4s *) (In2);
        v4s * __restrict__ VBuff1 = (v4s *) (In2+H_In2);
        v4s * __restrict__ VBuff2 = (v4s *) (In2+2*H_In2);
        v4s * __restrict__ VBuff3 = (v4s *) (In2+3*H_In2);

        unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(H_In1), First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
        unsigned int Iter = (Last>First)?(Last-First):0;
        unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);
        int OffLine = 0, OffCol = OutFirstCol;

        for (Col=0; Col<W_In2/4; Col++) {
                for (Line=0; Line<Iter/2; Line++) {
                        int l1 = 2*Line + First;
                        v4s *VIn1 = (v4s *) (&In1[(l1)*W_In1 + 0]);
                        v4s *VIn2 = (v4s *) (&In1[(l1+1)*W_In1 + 0]);
                        int S0 = 0, S1=0, S2=0, S3=0;
                        int S4 = 0, S5=0, S6=0, S7=0;
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
                                S0 += V0 * In2[i];
                                S1 += V0 * In2[i+1*H_In2];
                                S2 += V0 * In2[i+2*H_In2];
                                S3 += V0 * In2[i+3*H_In2];
                                int V1 = In1[(l1+1)*W_In1 + i];
                                S4 += V1 * In2[i];
                                S5 += V1 * In2[i+1*H_In2];
                                S6 += V1 * In2[i+2*H_In2];
                                S7 += V1 * In2[i+3*H_In2];
                        }
                        v4s R1 = gap_pack4(gap_clip(gap_roundnorm_reg(S0, Norm), 7), gap_clip(gap_roundnorm_reg(S1, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S2, Norm), 7), gap_clip(gap_roundnorm_reg(S3, Norm), 7));
                        v4s R2 = gap_pack4(gap_clip(gap_roundnorm_reg(S4, Norm), 7), gap_clip(gap_roundnorm_reg(S5, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S6, Norm), 7), gap_clip(gap_roundnorm_reg(S7, Norm), 7));
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
                                S0 += V0 * In2[i];
                                S1 += V0 * In2[i+1*H_In2];
                                S2 += V0 * In2[i+2*H_In2];
                                S3 += V0 * In2[i+3*H_In2];
                        }
                        v4s R1 = gap_pack4(gap_clip(gap_roundnorm_reg(S0, Norm), 7), gap_clip(gap_roundnorm_reg(S1, Norm), 7),
                                           gap_clip(gap_roundnorm_reg(S2, Norm), 7), gap_clip(gap_roundnorm_reg(S3, Norm), 7));
                        *((v4s *) (Out+(l1+OffLine)*W_Out+4*Col+0+OffCol)) = R1;
                }
                gap_waitbarrier(0);
        }
        if (W_In2&0x2) {
                Col = W_In2/2 - 1;
                for (i=F;i<L; i++) {
                        int X0 = In2[i*W_In2+2*Col+0];
                        int X1 = In2[i*W_In2+2*Col+1];
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
                                S0 += V0 * In2[i];
                                S1 += V0 * In2[i+1*H_In2];
                        }
                        Out[(Line+OffLine)*W_Out+2*Col+0+OffCol] = gap_clip(gap_roundnorm_reg(S0, Norm), 7);
                        Out[(Line+OffLine)*W_Out+2*Col+1+OffCol] = gap_clip(gap_roundnorm_reg(S1, Norm), 7);
                }
                gap_waitbarrier(0);
        }
        if (W_In2&0x1) {
                Col = W_In2-1;
                for (i=F;i<L; i++) {
                        int X0 = In2[i*W_In2+1*Col+0];
                        In2[i+0*H_In2] = X0; // In2[i*W_In2+4*Col+0];
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
                                S0 += V0 * In2[i];
                        }
                        Out[(Line+OffLine)*W_Out+1*Col+0+OffCol] = gap_clip(gap_roundnorm_reg(S0, Norm), 7);
                }
                gap_waitbarrier(0);
        }
}
