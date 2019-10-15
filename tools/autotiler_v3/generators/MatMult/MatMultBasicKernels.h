/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __MATMULTBASICKERNELS_H__
#define __MATMULTBASICKERNELS_H__
#include "Gap.h"

typedef struct {
        float * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        float * __restrict__ In2;
        unsigned int W_In2;
        float * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        float * __restrict__ BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
} KerMatMultParallelf32_ArgT;
void KerMatMultParallelf32(KerMatMultParallelf32_ArgT *Arg);

#ifdef __riscv__
typedef struct {
        f16 * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        f16 * __restrict__ In2;
        unsigned int W_In2;
        f16 * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        f16 * __restrict__ BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
} KerMatMultParallelf16_ArgT;
void KerMatMultParallelf16(KerMatMultParallelf16_ArgT *Arg);
void KerMatMultParallelVectorialf16(KerMatMultParallelf16_ArgT *Arg);

#endif

typedef struct {
        int * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        int * __restrict__ In2;
        unsigned int W_In2;
        int * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        int * __restrict__ BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
        unsigned int Norm;
} KerMatMultParallel32_ArgT;
void KerMatMultParallel32(KerMatMultParallel32_ArgT *Arg);

typedef struct {
        short int * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        short int * __restrict__ In2;
        unsigned int W_In2;
        short int * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        short int * __restrict__ BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
        unsigned int Norm;
} KerMatMultParallel16_ArgT;

void KerMatMultParallel16(
        KerMatMultParallel16_ArgT *Arg
        );

void KerMatMultParallelVectorial16(
        KerMatMultParallel16_ArgT *Arg
        );

typedef struct {
        signed char * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        signed char * __restrict__ In2;
        unsigned int W_In2;
        signed char * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        signed char * __restrict__ BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
        unsigned int Norm;
} KerMatMultParallel8_ArgT;

void KerMatMultParallel8(
        KerMatMultParallel8_ArgT *Arg
        );

void KerMatMultParallelVectorial8(
        KerMatMultParallel8_ArgT *Arg
        );
#endif
