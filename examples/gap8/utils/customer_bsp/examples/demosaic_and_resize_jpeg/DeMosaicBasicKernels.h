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
    unsigned char * __restrict__ Out0;
    unsigned char * __restrict__ Out1;
    unsigned char * __restrict__ Out2;
    unsigned int W;
    unsigned int H;
    unsigned int isTileFirst;
	unsigned int isTileLast;
} KerDeMosaic_ArgT;


typedef struct {
    unsigned char * __restrict__ In;
    unsigned int W;
    unsigned int H;
    unsigned char * __restrict__ Out;
} KerDeMosaicAndResize_ArgT;





void KerDeMosaicGRBG(KerDeMosaic_ArgT *Arg);
void KerDeMosaicAndResizeGRBG(KerDeMosaicAndResize_ArgT *Arg);
void KerDeMosaicAndResizeGBRG(KerDeMosaicAndResize_ArgT *Arg);
void KerDeMosaicAndResizeBGGR(KerDeMosaicAndResize_ArgT *Arg);



#endif //__IIBASICKERNELS_H__