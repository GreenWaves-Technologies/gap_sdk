/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "Gap8.h"

typedef struct MatrixAdd_Arg{
	int *In1;
	int *In2;
	int *Out;
	unsigned int W;
	unsigned int H;
} MatrixAdd_Arg_T;

void MatSumPar(MatrixAdd_Arg_T *KerArg0 );
