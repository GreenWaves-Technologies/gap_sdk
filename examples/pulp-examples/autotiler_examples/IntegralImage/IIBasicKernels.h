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

#include "Gap8.h"
#include "StdTypes.h"

typedef struct {
	Wordu32 * __restrict__ KerBuffer;
	Wordu32 W;
} KerPrimeImage_ArgT;

void KerPrime(KerPrimeImage_ArgT *KerArg);

typedef struct {
	Wordu8 * __restrict__ In;
	Wordu32 W;
	Wordu32 H;
	Wordu32 * __restrict__ IntegralImage;
	Wordu32 * __restrict__ KerBuffer;
} KerProcessImage_ArgT;

void KerProcess(KerProcessImage_ArgT *KerArg);


#endif //__IIBASICKERNELS_H__