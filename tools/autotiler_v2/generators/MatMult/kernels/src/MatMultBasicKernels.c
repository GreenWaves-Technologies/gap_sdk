/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap8.h"
#include "MatMultBasicKernels.h"


#define Min(a, b)               (((a)<(b))?(a):(b))

#ifdef OLD_RUNTIME
#define wait_synch_barrier()          eu_bar_trig_wait_clr(eu_bar_addr(0))
#else
#define wait_synch_barrier()          rt_team_barrier()
#endif

static int CoreCountDynamic = 1;
static int ActiveCore = gap8_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
    unsigned int NCore;
    unsigned int Log2Core;
    unsigned int Chunk;

    if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
    Log2Core = gap8_fl1(NCore);
    Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
    return Chunk;
}

void KerMatMultParallel(KerMatMultParallel_ArgT *Arg)

{
    short int * __restrict__ In1 = Arg->In1;
    unsigned int W_In1 = Arg->W_In1;
    unsigned int H_In1 = Arg->H_In1;
    short int * __restrict__ In2 = Arg->In2;
    unsigned int W_In2 = Arg->W_In2;
    short int * __restrict__ Out = Arg->Out;
    unsigned int W_Out = Arg->W_Out;
    unsigned int OutFirstCol = Arg->OutFirstCol;

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;

    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize(W_In2);
    unsigned int First = CoreId*ChunkCell, Last  = Min(W_In2, First+ChunkCell);

    for (Col=First; Col<Last; Col++) {
        for (Line=0; Line<H_In1; Line++) {
            int S = 0;
            for (i=0; i<W_In1; i++) S = S + In1[Line*W_In1 + i] * In2[i*W_In2+Col];
            Out[Line*W_Out+Col+OutFirstCol] = S;
        }
    }
    wait_synch_barrier();
}

void KerMatMultParallelVectorial(KerMatMultParallel_ArgT *Arg)

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

    unsigned int H_In2 = W_In1;
    unsigned int H_Out = H_In1;
    unsigned int Line, Col, i;
    v2s *VBuff = (v2s *) BufferColIn2;

    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize(H_In1);
    unsigned int First = CoreId*ChunkCell, Last  = Min(H_In1, First+ChunkCell);
    unsigned int C = ChunkSize(H_In2), F = CoreId*C, L  = Min(H_In2, F+C);


    for (Col=0; Col<W_In2; Col++) {

        for (i=F;i<L; i++) BufferColIn2[i] = In2[i*W_In2+Col];
            wait_synch_barrier();

        for (Line=First; Line<Last; Line++) {
            v2s *VIn1 = (v2s *) (&In1[Line*W_In1 + 0]);
            int S = 0;
            for (i=0; i<W_In1/2; i++) S = gap8_sumdotp2(VIn1[i], VBuff[i], S);
            if (W_In1&0x1) S += In1[Line*W_In1 + W_In1-1]*BufferColIn2[W_In1 - 1];
            Out[Line*W_Out+Col+OutFirstCol] = S;
        }
        wait_synch_barrier();
    }
}
