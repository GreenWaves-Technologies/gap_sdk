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
#include "cascade.h"

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

void KerResizeBilinear(KerResizeBilinear_ArgT *KerArg);

typedef struct {
	unsigned int * __restrict__ KerBuffer;
	unsigned int W;
} KerPrimeImage_ArgT;

void KerIntegralImagePrime(KerPrimeImage_ArgT *KerArg);

typedef struct {
	unsigned char * __restrict__ In;
	unsigned int W;
	unsigned int H;
	unsigned int * __restrict__ IntegralImage;
	unsigned int * __restrict__ KerBuffer;
} KerProcessImage_ArgT;

void KerIntegralImageProcess(KerProcessImage_ArgT *KerArg);


void KerSquaredIntegralImageProcess(KerProcessImage_ArgT *KerArg);

typedef struct {
	unsigned int * __restrict__ IntegralImage;
	unsigned int * __restrict__ SquaredIntegralImage;
	unsigned int W;
	unsigned int H;
	void * cascade_model;
	unsigned char WinW;
	unsigned char WinH;
	unsigned char * __restrict__ CascadeReponse;
} KerEvaluateCascade_ArgT;



typedef struct{
    int stage_index;
    int* stage_sum;
    unsigned int *integralImage;
    int img_w;
    single_cascade_t *cascade_stage;
    int std;
    int off_x;
    int off_y;

} eval_weak_classifier_Arg_T;

void KerEvaluateCascade(
	unsigned int * __restrict__ IntegralImage,
	unsigned int * __restrict__ SquaredIntegralImage,
	unsigned int W,
	unsigned int H,
	void * cascade_model,
	unsigned char WinW,
	unsigned char WinH,
	int * __restrict__ CascadeReponse);

#endif //__IIBASICKERNELS_H__
