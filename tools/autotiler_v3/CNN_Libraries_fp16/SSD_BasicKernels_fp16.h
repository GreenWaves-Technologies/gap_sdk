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

#ifndef __SSD_BASICKERNELS_H__
#define __SSD_BASICKERNELS_H__

#include "Gap.h"
#include <stdint.h>

#ifndef Abs
#define Abs(a)      (((int)(a)<0)?(-(a)):(a))
#endif
#ifndef Min
#define Min(a, b)               (((a)<(b))?(a):(b))
#endif
#ifndef Max
#define Max(a, b)               (((a)>(b))?(a):(b))
#endif

typedef struct{
	F16 x;
	F16 y;
	F16 w;
	F16 h;
	F16 score;
	signed char  class;
	signed char  alive;
}bbox_f16_t;

typedef struct {
    short int *bbox_idx;
    bbox_f16_t * bbox_buf;
    short int n_max_bb;
}Ker_SSD_Init_Arg_f16_T;

void Ker_SSD_Init_f16(Ker_SSD_Init_Arg_f16_T *Arg);


typedef struct {
	F16 * __restrict__ boxes_in;
    F16 * __restrict__ classes_in;
    F16 * __restrict__ anchors_in;
    bbox_f16_t * bbox_buf;
    short int N_Classes;
    short int N_Anchors;
    short int n_max_bb;
    F16 ScoreThr;
    short int *bbox_idx;
} Ker_SSD_Decoder_Arg_f16_T;

void Ker_SSD_Decoder_f16(Ker_SSD_Decoder_Arg_f16_T *Arg);

typedef struct {
    bbox_f16_t * bbox_buf;
    short int n_out_box;
    F16 * bbox_out;
    F16 * class_out;
    F16 * scores_out;
    F16 NMSThr;
    short int n_max_bb;
    short int *bbox_idx;
} Ker_SSD_NMS_ArgT;

void Ker_SSD_NMS(Ker_SSD_NMS_ArgT *Arg);



#endif //__SSD_BASICKERNELS_H__
