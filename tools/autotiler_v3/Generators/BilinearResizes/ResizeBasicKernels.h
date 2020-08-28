/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __IIBASICKERNELS_H__
#define __IIBASICKERNELS_H__

#include "Gap.h"

#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

typedef struct {
	unsigned char * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	unsigned char * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned int HTileOut;
	unsigned int FirstLineIndex;
} KerResizeBilinear_ArgT;

void KerResizeBilinear(KerResizeBilinear_ArgT *Arg);
#endif //__IIBASICKERNELS_H__
