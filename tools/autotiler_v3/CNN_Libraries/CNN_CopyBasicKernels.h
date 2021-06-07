/*
 * Copyright (C) 2021 GreenWaves Technologies
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


#ifndef __CNN_COPY_BASIC_KERNELS_H__
#define __CNN_COPY_BASIC_KERNELS_H__

#include "Gap.h"
#include "CNN_Defines.h"

typedef struct {
	short int *__restrict__ In;		/**< Input matrix */
	short int *__restrict__ Out;		/**< Output matrix */
	unsigned int Feat;			/**< Number of matrices */
	unsigned int W;				/**< Matrix width */
	unsigned int H;				/**< Matrix height */
	unsigned char Sx;			/**< Stride for W dimension */
	unsigned char Sy;			/**< Stride for H dimension */
} KerMatTranspose_fp_T;

typedef struct {
	signed char *__restrict__ In;		/**< Input matrix */
	signed char *__restrict__ Out;		/**< Output matrix */
	unsigned int Feat;			/**< Number of matrices */
	unsigned int W;				/**< Matrix width */
	unsigned int H;				/**< Matrix height */
	unsigned char Sx;			/**< Stride for W dimension */
	unsigned char Sy;			/**< Stride for H dimension */
} KerMatTranspose_fps_T;

typedef struct {
	void *__restrict__ In;           /**< Input matrix */
	void *__restrict__ Out;          /**< Output matrix */
	unsigned int W;                         /**< Tile width */
	unsigned int H;                         /**< Tile height */
} KerCopy_void_T;

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

/*************************************************************************************************************************************************
	List of Matrix Transposition
*************************************************************************************************************************************************/

extern void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg);

extern void CNN_ParTranspose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_ParTransposeSxSy_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_Transpose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_TransposeSxSy_fp(KerMatTranspose_fp_T *Arg);

/*************************************************************************************************************************************************
	3D Tensor dimension permutations
*************************************************************************************************************************************************/

extern void CNN_MatPermCHW2CWH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HWC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WHC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WCH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HCW_fp(KerMatTranspose_fp_T *Arg);

extern void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg);

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
void CNN_Copy_void(KerCopy_void_T * Arg);

#endif

