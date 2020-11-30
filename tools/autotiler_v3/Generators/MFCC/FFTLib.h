/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef _FFTLIB_H
#define _FFTLIB_H

#include <stdint.h>
#include "Gap.h"

#if (FP==2) 
typedef   float16    v2h  __attribute__((vector_size (4)));
typedef   float16alt v2ah __attribute__((vector_size (4)));
void SetupTwiddlesLUT_f16(float16 *Twiddles, float *Twiddlesf, int N);
void Dmp_twid(float16 *Twid, int N, int size);
#endif

char *__FP(int X, int Prec, char *Out);

typedef struct {
  float r;
  float i;
} complex_fp;


typedef int cmplx __attribute__((vector_size (8)));

/** @brief Template for FFT/IFFT 1D on a  complex input
****
Template for FFT/IFFT 1D on a complex input
*/
typedef struct {
  void * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  signed char * __restrict__ shift_fft;	/**< Pointer to shift table */
  unsigned int N_fft; /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;  /**< FFT direction, direct or inverse */
} FFT_Arg_T;

typedef struct {
  void * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  signed char *__restrict__ shift_fft;
  unsigned int N_fft; /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;  /**< FFT direction, direct or inverse */
} FFT_scal_Arg_T;


/** @brief Template for I,Q samples reordering (bit reverse to/from natural order)

Template for I,Q samples reordering (bit reverse to/from natural order)
*/
typedef struct {
        void *__restrict__ Data;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        short *__restrict__ SwapTable;		/**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;					/**< FFT dimension */
} SwapSamples_Arg_T;

typedef struct {
        void *__restrict__ Data;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        signed char *__restrict__ shift_BF;
        short *__restrict__ SwapTable;		/**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;					/**< FFT dimension */
} SwapSamples_scal_Arg_T;

/** @brief Template for Samples to I,Q expansion to prepare for FFT, 1D and 2D.

Template for Samples to I,Q expansion to prepare for FFT, 1D and 2D.
*/
typedef struct {
        void * __restrict__ Samples;            /**< Pointer to Samples */
        int SampleSize;                         /**< Size of one Sample in byte, if SampleSize<0 Sample is considered as signed, if >0 considered as unsigned */
        v2s * __restrict__ Out;                 /**< Pointer to the (I, Q) expansion of In, I=0, Q=In */
        unsigned int Precision;                 /**< Fixed point format for (I,Q) samples, should me max Q12 for Radix4 FFTs and max Q13 for Radix2 FFTs */
        unsigned short int W;                   /**< Image width */
        unsigned short int H;                   /**< Image height */
} Samples2Complex_Arg_T;

/** @brief Template for twiddles and swap coefficients copy from L2 to shared L1.

Template for twiddles and swap coefficients copy from L2 to shared L1.

Should always be called before using one of the FFT routines when using it on the cluster, it will speedup operations.
All FFTs routines will also work safely if Twiddles and swap LUT are accessed directly in L2 memory.
*/
typedef struct {
        short int Nfft;                         /**< Size of the fft */
        short int Radix;                        /**< Radix of the fft, 2 or 4 */
        void * __restrict__ Twiddles;      /**< Pointer to Twiddles in L2 */
        short int * __restrict__ SwapLUT;       /**< Pointer to Swap LUT table in L2 */
        void * __restrict__ L1_Twiddles;   /**< Pointer to Twiddles copy in shared L1 */
        short int * __restrict__ L1_SwapLUT;    /**< Pointer to Swap LUT table copy in shared L1 */
} FFT_InstallArg_T;



//static void Radix4FFTKernelDIF_FP(complex_fp *InOutA, complex_fp *InOutB, complex_fp *InOutC, complex_fp *InOutD,    complex_fp W1, complex_fp W2, complex_fp W3, unsigned int Inverse);


void SwapSamples_scal( SwapSamples_scal_Arg_T *Arg);
void SwapSamples_Par_FP(SwapSamples_Arg_T *Arg);
void Radix4FFT_DIF_Par_Internal_FP(float *__restrict__ Data,
				     float *__restrict__ Twiddles,int
				     N_fft,unsigned int Inverse);

/*
void SwapSamples_FP_FC(float *Datai, int Ni, short int *SwapTable);
void Radix4FFT_DIF_FP_FC(FFT_Arg_T *Arg);
void Radix2FFT_DIF_FP_FC(FFT_Arg_T *Arg);
void RadixMixedFFT_DIF_FP_FC(FFT_Arg_T *Arg);
*/

void Radix2FFT_DIF_Par_FP(FFT_Arg_T *Arg);
void RadixMixedFFT_DIF_Par_FP(FFT_Arg_T *Arg);
void RadixMixedFFT_DIF_Par_FP16(FFT_Arg_T *Arg);
void RadixMixedFFT1D_FP16_Args(FFT_Arg_T *Arg);
void Radix2FFT_DIF_Par_FP16(FFT_Arg_T *Arg);
void RadixMixedFFT_DIF_Inv_Par(FFT_Arg_T *Arg);
void Radix2FFT_DIF_Inv_Par(FFT_Arg_T *Arg);
void Radix2FFT_DIF(FFT_Arg_T *Arg);

void Radix4FFT1D_FP_Args(FFT_Arg_T *Arg);
void Radix2FFT1D_FP_Args(FFT_Arg_T *Arg);
void Radix4FFT1D_Args(FFT_Arg_T *Arg);
void Radix2FFT1D_Args(FFT_Arg_T *Arg);
void RadixMixedFFT1D_FP_Args(FFT_Arg_T *Arg);
void RadixMixedFFT1D_Args(FFT_Arg_T *Arg);



void Radix4FFT_DIF_Scalar        (FFT_Arg_T *Arg);		/* OK */
void Radix4FFT_DIF_Scalar_Inverse(FFT_Arg_T *Arg);		/* OK */


void Radix4FFT_DIF_Par        (FFT_Arg_T *Arg);		/* OK */
void RadixMixedFFT_DIF_Par        (FFT_Arg_T *Arg);		/* OK */

void Radix2FFT_DIF_Par(FFT_Arg_T *Arg);


void SwapSamples    (SwapSamples_Arg_T *Arg);			/* OK */
void SwapSamples_Par(SwapSamples_Arg_T *Arg);			/* OK */


void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format);

void printout(void *Data, int N);
 
void SwapSamples_args(        v2s *__restrict__ Data,
                              short int *__restrict__ SwapTable ,
                              int Ni);
void Radix2FFT_DIF_args(short int *__restrict__ Data,short int *__restrict__ Twiddles, int N_FFT2);

void Radix2FFT_DIF_INT_Scal_Par(FFT_scal_Arg_T *Arg);

#ifdef BUILD_LUT
void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);
void SetupScalarTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);

void SetupR4SwapTable (short int *SwapTable, int Ni);
void SetupR2SwapTable (short int *SwapTable, int Ni);
#endif

#define FFT4_SAMPLE_DYN 12
#define FFT2_SAMPLE_DYN 13
#define FFT_TWIDDLE_DYN 15

#define FFT4_SCALEDOWN 2
#define FFT2_SCALEDOWN 1


#endif
