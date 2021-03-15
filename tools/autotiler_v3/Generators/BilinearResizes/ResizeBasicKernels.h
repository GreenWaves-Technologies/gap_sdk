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

#ifndef Max
	#define Max(a, b)               (((a)>(b))?(a):(b))
#endif
#ifndef Min
	#define Min(a, b)               (((a)<(b))?(a):(b))
#endif

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

typedef struct {
	unsigned char * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	unsigned char * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned int HTileOut;
	unsigned int FirstLineIndex;
} KerResizeNearestNeighbor_ArgT;

typedef struct {
	signed char * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	signed char * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned int HTileOut;
	unsigned int FirstLineIndex;
} KerResizeBilinearSigned_ArgT;

typedef struct {
	signed char * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	signed char * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned int HTileOut;
	unsigned int FirstLineIndex;
} KerResizeNearestNeighborSigned_ArgT;

typedef struct {
	signed short * __restrict__ In;
	unsigned int Win;
	unsigned int Hin;
	signed short * __restrict__ Out;
	unsigned int Wout;
	unsigned int Hout;
	unsigned int HTileOut;
	unsigned int FirstLineIndex;
} KerResizeSigned16_ArgT;

void KerResizeBilinear(KerResizeBilinear_ArgT *Arg);
void KerResizeNearestNeighbor(KerResizeNearestNeighbor_ArgT *Arg);
void KerResizeBilinearSigned(KerResizeBilinearSigned_ArgT *Arg);
void KerResizeNearestNeighborSigned(KerResizeNearestNeighborSigned_ArgT *Arg);
void KerResizeBilinearSigned_Q16(KerResizeSigned16_ArgT *Arg);
void KerResizeNearestNeighborSigned_Q16(KerResizeSigned16_ArgT *Arg);
#endif //__IIBASICKERNELS_H__
