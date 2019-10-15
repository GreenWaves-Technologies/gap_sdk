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
#include "Gap8.h"

typedef struct {
        short int * __restrict__ In1;
        unsigned int W_In1;
        unsigned int H_In1;
        short int * __restrict__ In2;
        unsigned int W_In2;
        short int * __restrict__ Out;
        unsigned int W_Out;
        unsigned int OutFirstCol;       // Equal M2FirstCol
        short int *BufferColIn2;        // In case vectorization is used will be used to copy a column of In2 into a line
} KerMatMultParallel_ArgT;

void KerMatMultParallel(
	KerMatMultParallel_ArgT *Arg
	);

void KerMatMultParallelVectorial(
	KerMatMultParallel_ArgT *Arg
	);

#endif
