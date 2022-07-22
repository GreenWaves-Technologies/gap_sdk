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



#ifndef __CNN_COPY_H__
#define __CNN_COPY_H__
#include "Gap.h"
#include "CNN_Defines.h"
#include "CNN_FloatType.h"
#include "CNN_Defines_fp16.h"

#define gap_pack2f16(x, y)                 ((F16V) {(F16)   (x), (F16)   (y)})

typedef struct {
	void *__restrict__ In;           /**< Input matrix */
	void *__restrict__ Out;          /**< Output matrix */
	unsigned int N;                         /**< Number of elements */
} KerCopy_void_T;

typedef struct {
	void *__restrict__ In;           /**< Input matrix */
	void *__restrict__ Out;          /**< Output matrix */
	unsigned int NTile;              /**< Number of input elements in this tile */
	unsigned int NRepeat;            /**< Number of times to repeat the input */
} KerRepeat_void_T;

typedef struct {
    char *__restrict__ In1;
    char *__restrict__ In2;
    char *__restrict__ In3;
    char *__restrict__ In4;
    char *__restrict__ Out;
    unsigned short int H;
    unsigned short int W1;
    unsigned short int W2;
    unsigned short int W3;
    unsigned short int W4;
    unsigned char DataSize;
} CNN_Concat_Width_Arg_T;

typedef struct {
    char *__restrict__ In;
    char *__restrict__ Out1;
    char *__restrict__ Out2;
    char *__restrict__ Out3;
    char *__restrict__ Out4;
    unsigned short int H;
    unsigned short int W1;
    unsigned short int W2;
    unsigned short int W3;
    unsigned short int W4;
    unsigned char DataSize;
} CNN_Split_Width_Arg_T;

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



// signed short -> unsigned short
typedef struct {
	signed short *__restrict__ In;
	unsigned short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpUFp_T;

// unsigned short -> signed short
typedef struct {
	unsigned short *__restrict__ In;
	signed short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpFp_T;

// signed short -> signed char
typedef struct {
	signed short *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpFps_T;

// signed short -> unsigned char
typedef struct {
	unsigned short *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpUFps_T;

// unsigned short -> signed char
typedef struct {
	unsigned short *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpFps_T;

// unsigned short -> unsigned char
typedef struct {
	unsigned short *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpUFps_T;

// signed char -> unsigned char
typedef struct {
	signed char *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsUFps_T;

// signed char -> signed char
typedef struct {
	signed char *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsFps_T;

// unsigned char -> unsigned char
typedef struct {
	unsigned char *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsUFps_T;

// unsigned char -> signed char
typedef struct {
	unsigned char *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsFps_T;

// signed char -> signed short
typedef struct {
	signed char *__restrict__ In;
	signed short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsFp_T;

// unsigned char -> signed short
typedef struct {
	unsigned char *__restrict__ In;
	signed short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsFp_T;

// signed char -> unsigned short
typedef struct {
	signed char *__restrict__ In;
	unsigned short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsUFp_T;

// unsigned char -> unsigned short
typedef struct {
	unsigned char *__restrict__ In;
	unsigned short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsUFp_T;

// signed short -> signed short
typedef struct {
	signed short *__restrict__ In;
	signed short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpFp_T;

// unsigned short -> unsigned short
typedef struct {
	unsigned short *__restrict__ In;
	unsigned short *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpUFp_T;

// float16 -> signed short
typedef struct {
	F16 *__restrict__ In;
	short int *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float16Fp_T;

// float16 -> unsigned short
typedef struct {
	F16 *__restrict__ In;
	unsigned short int *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float16UFp_T;

// float16 -> signed char
typedef struct {
	F16 *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float16Fps_T;

// float16 -> unsigned char
typedef struct {
	F16 *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float16UFps_T;

// signed short -> float16
typedef struct {
	short int *__restrict__ In;
	F16 *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpFloat16_T;

// unsigned short -> float16
typedef struct {
	unsigned short int *__restrict__ In;
	F16 *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpFloat16_T;

// signed char -> float16
typedef struct {
	signed char *__restrict__ In;
	F16 *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsFloat16_T;

// unsigned char -> float16
typedef struct {
	unsigned char *__restrict__ In;
	F16 *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsFloat16_T;

// float32 -> signed short
typedef struct {
	float *__restrict__ In;
	short int *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float32Fp_T;

// float32 -> unsigned short
typedef struct {
	float *__restrict__ In;
	unsigned short int *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float32UFp_T;

// float32 -> signed char
typedef struct {
	float *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float32Fps_T;

// float32 -> unsigned char
typedef struct {
	float *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_Float32UFps_T;

// signed short -> float32
typedef struct {
	short int *__restrict__ In;
	float *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpFloat32_T;

// unsigned short -> float32
typedef struct {
	unsigned short int *__restrict__ In;
	float *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpFloat32_T;

// signed char -> float32
typedef struct {
	signed char *__restrict__ In;
	float *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_FpsFloat32_T;

// unsigned char -> float32
typedef struct {
	unsigned char *__restrict__ In;
	float *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
    signed char *__restrict__ Infos;
} CNN_UFpsFloat32_T;

#define AT_INF_QUANT_ZERO_DIFF		0
#define AT_INF_QUANT_SCALE			4
#define AT_INF_QUANT_NORM			6

extern void CNN_Copy_void(KerCopy_void_T *Arg);
extern void CNN_Repeat_void(KerRepeat_void_T *Arg);
extern void CNN_Concat_Width(CNN_Concat_Width_Arg_T *Arg);
extern void CNN_ParConcat_Width(CNN_Concat_Width_Arg_T *Arg);
extern void CNN_Split_Width(CNN_Split_Width_Arg_T *Arg);
extern void CNN_ParSplit_Width(CNN_Split_Width_Arg_T *Arg);

/* Transpose byte */
extern void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg);

/* Transpose short */
extern void CNN_ParTranspose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_ParTransposeSxSy_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_Transpose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_TransposeSxSy_fp(KerMatTranspose_fp_T *Arg);

/* CHW -> *, byte */
extern void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg);

/* CHW -> *, short */
extern void CNN_MatPermCHW2CWH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HWC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WHC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WCH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HCW_fp(KerMatTranspose_fp_T *Arg);

/* HWC -> *, byte */
extern void CNN_MatPermHWC2HCW_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermHWC2WCH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermHWC2WHC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermHWC2CHW_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermHWC2CWH_fps(KerMatTranspose_fps_T *Arg);

/* HWC -> *, short */
extern void CNN_MatPermHWC2HCW_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermHWC2WCH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermHWC2WHC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermHWC2CHW_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermHWC2CWH_fp(KerMatTranspose_fp_T *Arg);

extern void CNN_NormRGB565_offset_fps(KerNormRGB565_fps_T *Arg);
extern void CNN_NormRGB565_shift_fps(KerNormRGB565_fps_T *Arg);
extern void CNN_NormRGB888_offset_fps(KerNormRGB888_fps_T *Arg);
extern void CNN_NormRGB888_shift_fps(KerNormRGB888_fps_T *Arg);
extern void CNN_NormRGB16_fp(KerNormRGB16_fp_T *Arg);
extern void CNN_NormBW_offset_fps(KerNormBW_fps_T *Arg);
extern void CNN_NormBW_shift_fps(KerNormBW_fps_T *Arg);
extern void CNN_NormBW_fp(KerNormBW_fp_T *Arg);

extern void CNN_FpsUFps(CNN_FpsUFps_T *Args);
extern void CNN_FpsFp(CNN_FpsFp_T * Arg);
extern void CNN_FpsFpScaled(CNN_FpsFp_T * Arg);
extern void CNN_FpsUFpScaled(CNN_FpsUFp_T * Arg);
extern void CNN_FpsFpScaledPosNorm(CNN_FpsFp_T * Arg);
extern void CNN_FpsUFpsScaled(CNN_FpsUFps_T * Arg);
extern void CNN_FpsFpsScaled(CNN_FpsFps_T * Arg);

extern void CNN_UFpsFps(CNN_UFpsFps_T *Args);
extern void CNN_UFpFp(CNN_UFpFp_T * Arg);
extern void CNN_UFpsFpScaled(CNN_UFpsFp_T * Arg);
extern void CNN_UFpsUFpScaled(CNN_UFpsUFp_T * Arg);
extern void CNN_UFpsUFp(CNN_UFpsUFp_T * Arg);
extern void CNN_UFpsFpsScaled(CNN_UFpsFps_T * Arg);
extern void CNN_UFpsUFpsScaled(CNN_UFpsUFps_T * Arg);

extern void CNN_FpUFp(CNN_FpUFp_T * Arg);
extern void CNN_FpFps(CNN_FpFps_T * Arg);
extern void CNN_FpFpsScaled(CNN_FpFps_T * Arg);
extern void CNN_FpUFpsScaled(CNN_FpUFps_T * Arg);
extern void CNN_FpUFpScaled(CNN_FpUFp_T * Arg);
extern void CNN_FpFpScaled(CNN_FpFp_T * Arg);
extern void CNN_UFpUFpScaled(CNN_UFpUFp_T * Arg);

extern void CNN_UFpUFpsScaled(CNN_UFpUFps_T * Arg);
extern void CNN_UFpFpScaled(CNN_UFpFp_T * Arg);

extern void CNN_UFpFpsScaled(CNN_UFpFps_T * Arg);
extern void CNN_UFpUFps(CNN_UFpUFps_T * Arg);

extern void CNN_Float16Fp(CNN_Float16Fp_T * Arg);
extern void CNN_Float16UFp(CNN_Float16UFp_T * Arg);
extern void CNN_Float16Fps(CNN_Float16Fps_T * Arg);
extern void CNN_Float16UFps(CNN_Float16UFps_T * Arg);

extern void CNN_FpFloat16(CNN_FpFloat16_T * Arg);
extern void CNN_UFpFloat16(CNN_UFpFloat16_T * Arg);
extern void CNN_FpsFloat16(CNN_FpsFloat16_T * Arg);
extern void CNN_UFpsFloat16(CNN_UFpsFloat16_T * Arg);

extern void CNN_FpFloat32(CNN_FpFloat32_T * Arg);
extern void CNN_UFpFloat32(CNN_UFpFloat32_T * Arg);
extern void CNN_FpsFloat32(CNN_FpsFloat32_T * Arg);
extern void CNN_UFpsFloat32(CNN_UFpsFloat32_T * Arg);

extern void CNN_Float32Fp(CNN_Float32Fp_T * Arg);
extern void CNN_Float32UFp(CNN_Float32UFp_T * Arg);
extern void CNN_Float32Fps(CNN_Float32Fps_T * Arg);
extern void CNN_Float32UFps(CNN_Float32UFps_T * Arg);

#endif

