/*
 * Copyright 2020 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
