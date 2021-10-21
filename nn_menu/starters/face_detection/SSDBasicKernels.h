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
	short * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	short * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned char Q;
	unsigned short n_classes;

} KerSDD3Dto2DShort_ArgT;

void KerSDD3Dto2DShort(KerSDD3Dto2DShort_ArgT *Arg);

typedef struct {
	short * __restrict__ Classes;
	short * __restrict__ Boxes;
	unsigned int Classes_W;
	unsigned int Classes_H;
	unsigned int Classes_TileIndex;
	unsigned int Classes_Std_H;
	void * Ancor_layer;
	void * BoundingBoxes;
	int Boxes_Q;
	unsigned int n_classes;
} KerPredecoderShort_ArgT;

void KerPredecoderShort(KerPredecoderShort_ArgT *Arg);



#endif //__IIBASICKERNELS_H__
