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

typedef struct {
	signed char *__restrict__ In;           /**< Input matrix */
	signed char *__restrict__ Out;          /**< Output matrix */
	unsigned int W;                         /**< Tile width */
	unsigned int H;                         /**< Tile height */
} KerCopy_fps_T;

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

void CNN_NormRGB565_offset_fps(KerNormRGB565_fps_T *Arg);
void CNN_NormRGB565_shift_fps(KerNormRGB565_fps_T *Arg);
void CNN_NormRGB888_offset_fps(KerNormRGB888_fps_T *Arg);
void CNN_NormRGB888_shift_fps(KerNormRGB888_fps_T *Arg);
void CNN_NormRGB16_fp(KerNormRGB16_fp_T *Arg);
void CNN_NormBW_offset_fps(KerNormBW_fps_T *Arg);
void CNN_NormBW_shift_fps(KerNormBW_fps_T *Arg);
void CNN_NormBW_fp(KerNormBW_fp_T *Arg);
void CNN_Copy_fps(KerCopy_fps_T * args);

#endif
