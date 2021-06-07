/*
 * Copyright 2021 GreenWaves Technologies, SAS
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


#ifndef __NNTOOL_EXTRA_KERNELS__
#define __NNTOOL_EXTRA_KERNELS__
#include "Gap.h"

#ifdef __pulp__
#ifndef Min
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#endif
#ifndef Max
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#endif
#else
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

#ifdef GENASM
#ifdef __EMUL__
#define gap_ncore()     8
#define gap_coreid()    __builtin_pulp_CoreId()
#endif
#endif

// typedef struct {
// 	signed char *__restrict__ In;           /**< Input matrix */
// 	signed char *__restrict__ Out;          /**< Output matrix */
// 	unsigned int W;                         /**< Tile width */
// 	unsigned int H;                         /**< Tile height */
// } KerCopy_fps_T;

typedef struct {
	unsigned short *__restrict__ In;           /**< Input matrix */
	signed char *__restrict__ Out0;          /**< Output matrix */
	signed char *__restrict__ Out1;          /**< Output matrix */
	signed char *__restrict__ Out2;          /**< Output matrix */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
} KerNormRGB565_fps_T;

typedef struct {
	unsigned char *__restrict__ In;           /**< Input matrix */
	signed char *__restrict__ Out0;          /**< Output matrix */
	signed char *__restrict__ Out1;          /**< Output matrix */
	signed char *__restrict__ Out2;          /**< Output matrix */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
} KerNormRGB888_fps_T;

typedef struct {
	unsigned char *__restrict__ In;         /**< Input matrix */
	signed short int *__restrict__ Out0;    /**< Output matrix */
	signed short int *__restrict__ Out1;    /**< Output matrix */
	signed short int *__restrict__ Out2;    /**< Output matrix */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
} KerNormRGB16_fp_T;

typedef struct {
	unsigned char *__restrict__ In;           /**< Input matrix */
	signed char *__restrict__ Out;          /**< Output matrix */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
} KerNormBW_fps_T;

typedef struct {
	unsigned char *__restrict__ In;         /**< Input matrix */
	signed short int *__restrict__ Out;     /**< Output matrix */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
} KerNormBW_fp_T;


typedef struct {
	signed char *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsFpu_T;

typedef struct {
	unsigned char *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpuFps_T;

void CNN_NormRGB565_offset_fps(KerNormRGB565_fps_T *Arg);
void CNN_NormRGB565_shift_fps(KerNormRGB565_fps_T *Arg);
void CNN_NormRGB888_offset_fps(KerNormRGB888_fps_T *Arg);
void CNN_NormRGB888_shift_fps(KerNormRGB888_fps_T *Arg);
void CNN_NormRGB16_fp(KerNormRGB16_fp_T *Arg);
void CNN_NormBW_offset_fps(KerNormBW_fps_T *Arg);
void CNN_NormBW_shift_fps(KerNormBW_fps_T *Arg);
void CNN_NormBW_fp(KerNormBW_fp_T *Arg);
void CNN_FpuFps(CNN_FpuFps_T *Args);
void CNN_FpsFpu(CNN_FpsFpu_T *Args);
// void CNN_Copy_fps(KerCopy_fps_T * args);

#endif
