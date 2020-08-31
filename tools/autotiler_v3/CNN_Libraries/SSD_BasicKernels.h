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

#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

typedef struct{
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
	int16_t  score;
	uint16_t class;
	uint8_t  alive;
}bbox_t;

typedef struct {
    int16_t *bbox_idx;
}Ker_SSD_Init_ArgT;

void Ker_SSD_Init(Ker_SSD_Init_ArgT *Arg);


typedef struct {
	int8_t * __restrict__ boxes_in;
    int8_t * __restrict__ classes_in;
    int8_t * __restrict__ anchors_in;
    bbox_t * bbox_out;
    int16_t Box_W;
    int16_t Class_W;
    int16_t H;
    uint8_t* in_scales;
    uint8_t* in_norms;
    int8_t * infos;
    int16_t n_max_bb;
    int16_t *bbox_idx;
} Ker_SSD_Decoder_ArgT;

void Ker_SSD_Decoder(Ker_SSD_Decoder_ArgT *Arg);

typedef struct {
    bbox_t * bbox_out;
    int8_t * infos;
    int16_t n_max_bb;
    int16_t *bbox_idx;
} Ker_SSD_NMS_ArgT;

void Ker_SSD_NMS(Ker_SSD_NMS_ArgT *Arg);



#endif //__SSD_BASICKERNELS_H__
