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

#define FFT_TWIDDLE_DYN 15
#define FFT4_SCALEDOWN 2
#define FFT2_SCALEDOWN 1

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
  unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;         /**< FFT direction, direct or inverse */
} FFT_Arg_T;


typedef struct {
  void  * __restrict__ Data; /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void  * __restrict__ RFFT_Out; /**< Pointer to output */
  void  * __restrict__ Twiddles; /**< Pointer to fft twiddles */
  void  * __restrict__ RTwiddles; /**< Pointer to rfft twiddles */
  short * __restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
  unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;         /**< FFT direction, direct or inverse */
} RFFT_Arg_T;

typedef struct {
  void * __restrict__ Data;	    /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void * __restrict__ Twiddles;	    /**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  unsigned int N_fft;               /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;             /**< FFT direction, direct or inverse */
  signed char *__restrict__ shift_fft;
} FFT_scal_Arg_T;

typedef struct {
  void  * __restrict__ Data; /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void  * __restrict__ RFFT_Out; /**< Pointer to output */
  void  * __restrict__ Twiddles; /**< Pointer to fft twiddles */
  void  * __restrict__ RTwiddles; /**< Pointer to rfft twiddles */
  short * __restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
  unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
  unsigned int Inverse;         /**< FFT direction, direct or inverse */
  signed char *__restrict__ shift_fft;
  signed char *__restrict__ shift_rfft;
} RFFT_scal_Arg_T;

/** @brief Template for I,Q samples reordering (bit reverse to/from natural order)

Template for I,Q samples reordering (bit reverse to/from natural order)
*/
typedef struct {
        void *__restrict__ Data;        /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        short *__restrict__ SwapTable;  /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;                         /**< FFT dimension */
} SwapSamples_Arg_T;

typedef struct {
        void        *__restrict__ Data;         /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        signed char *__restrict__ shift_fft;
        short       *__restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;                                 /**< FFT dimension */
} SwapSamples_scal_Arg_T;

/** @brief Template for twiddles and swap coefficients copy from L2 to shared L1.

Template for twiddles and swap coefficients copy from L2 to shared L1.

Should always be called before using one of the FFT routines when using it on the cluster, it will speedup operations.
All FFTs routines will also work safely if Twiddles and swap LUT are accessed directly in L2 memory.
*/
typedef struct {
        short int Nfft;                         /**< Size of the fft */
        short int Radix;                        /**< Radix of the fft, 2 or 4 */
        void      * __restrict__ Twiddles;      /**< Pointer to Twiddles in L2 */
        void      * __restrict__ RTwiddles;     /**< Pointer to RTwiddles in L2 */
        short int * __restrict__ SwapLUT;       /**< Pointer to Swap LUT table in L2 */
        void      * __restrict__ L1_Twiddles;   /**< Pointer to Twiddles copy in shared L1 */
        void      * __restrict__ L1_RTwiddles;  /**< Pointer to Twiddles copy in shared L1 */
        short int * __restrict__ L1_SwapLUT;    /**< Pointer to Swap LUT table copy in shared L1 */
} FFT_InstallArg_T;

extern void Radix4FFT_DIF_Seq   (signed short *__restrict__ Data, signed short *__restrict__ Twiddles, unsigned int N_fft, int Inverse);
extern void Radix2FFT_DIF_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft);
extern void Radix2FFT_DIF_Seq   (signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft);

extern void Radix4FFT_DIF_Par_Fix16(FFT_Arg_T *Arg);
extern void Radix4FFT_DIF_Par_Fix32(FFT_Arg_T *Arg);
extern void Radix4FFT_DIF_Par_f32(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix16_Fast(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix16(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix32(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix32_Scal(FFT_scal_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_f32  (FFT_Arg_T *Arg);
#ifdef __gap9__
extern void Radix4FFT_DIF_Par_f16(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_f16(FFT_Arg_T *Arg);
extern void RFFT_DIF_Par_f16(RFFT_Arg_T *Arg);
extern void IRFFT_DIF_Par_f16(RFFT_Arg_T *Arg);
extern void SwapSamples_Par_f16(SwapSamples_Arg_T *Arg);
#endif

extern void SwapSamples_Par(SwapSamples_Arg_T *Arg);
extern void SwapSamples_Par_f32(SwapSamples_Arg_T *Arg);
extern void SwapSamples_Par_Fix32(SwapSamples_Arg_T *Arg);
extern void SwapSamples_scal(SwapSamples_scal_Arg_T *Arg);
extern void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format);
extern void RFFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format);

extern void RFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg);
extern void RFFT_DIF_Par_Fix32_Scal(RFFT_scal_Arg_T *Arg);
extern void RFFT_DIF_Par_f32(RFFT_Arg_T *Arg);

extern void IRFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg);
extern void IRFFT_DIF_Par_f32(RFFT_Arg_T *Arg);

#endif
