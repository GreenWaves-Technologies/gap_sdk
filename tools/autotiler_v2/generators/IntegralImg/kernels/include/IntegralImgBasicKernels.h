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

typedef struct {
	unsigned int * __restrict__ KerBuffer;
	unsigned int W;
} KerPrimeImage_ArgT;

void KerPrime(KerPrimeImage_ArgT *KerArg);

typedef struct {
	unsigned char * __restrict__ In;
	unsigned int W;
	unsigned int H;
	unsigned int * __restrict__ IntegralImage;
	unsigned int * __restrict__ KerBuffer;
} KerProcessImage_ArgT;

void KerProcess(KerProcessImage_ArgT *KerArg);


#endif //__IIBASICKERNELS_H__
