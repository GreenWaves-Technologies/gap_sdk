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
#include "Gap8.h"


/** @brief Template for FFT/IFFT 1D on a complex input

Template for FFT/IFFT 1D on a complex input
*/
typedef struct {
        signed short * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        signed short * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
        unsigned int N_fft;			/**< FFT dimension, has to be a power of 2 */
} FFT_Arg_T;

/** @brief Template for FFT 2D on a complex input

Template for FFT 2D on a complex input
*/
typedef struct {
        signed short * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        signed short * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
        unsigned short int N_fft;
        unsigned short int Nb;
} FFT_2D_Arg_T;

/** @brief Template for I,Q samples reordering (bit reverse to/from natural order)

Template for I,Q samples reordering (bit reverse to/from natural order)
*/
typedef struct {
        v2s *__restrict__ Data;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        short *__restrict__ SwapTable;		/**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;					/**< FFT dimension */
} SwapSamples_Arg_T;

/** @brief Template for I,Q samples reordering (bit reverse to/from natural order), 2D samples

Template for I,Q samples reordering (bit reverse to/from natural order), 2D samples
*/
typedef struct {
        v2s *__restrict__ Data;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        short *__restrict__ SwapTable;		/**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        unsigned short int Ni;			/**< FFT dimension */
        unsigned short int Nb;			/**< Number of FFT rows or column */
} SwapSamples_2D_Arg_T;

/** @brief Template for byte image to I,Q expansion to prepare for 2D FFT

Template for byte image to I,Q expansion to prepare for 2D FFT
*/
typedef struct {
	unsigned char * __restrict__ In;	/**< Pointer to a byte image (or image tile) */
	int * __restrict__ Out;			/**< Pointer to the (I, Q) expansion of In, I=0, Q=In */
	unsigned short int W;			/**< Image width */
	unsigned short int H;			/**< Image height */
} Image2Complex_Arg_T;

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
        short int * __restrict__ Twiddles;      /**< Pointer to Twiddles in L2 */
        short int * __restrict__ SwapLUT;       /**< Pointer to Swap LUT table in L2 */
        short int * __restrict__ L1_Twiddles;   /**< Pointer to Twiddles copy in shared L1 */
        short int * __restrict__ L1_SwapLUT;    /**< Pointer to Swap LUT table copy in shared L1 */
} FFT_InstallArg_T;

void Image2Complex		 (Image2Complex_Arg_T *Arg);
void Samples2Complex             (Samples2Complex_Arg_T *Arg);
void Samples2Complex_Par         (Samples2Complex_Arg_T *Arg);

void Radix4FFT_DIT_Scalar        (FFT_Arg_T *Arg);		/* OK */
void Radix4FFT_DIT_Scalar_Inverse(FFT_Arg_T *Arg);		/* OK */

void Radix4FFT_DIF_Scalar        (FFT_Arg_T *Arg);		/* OK */
void Radix4FFT_DIF_Scalar_Inverse(FFT_Arg_T *Arg);		/* OK */

void Radix2FFT_DIT_Scalar(FFT_Arg_T *Arg);			/* OK */
void Radix2FFT_DIF_Scalar(FFT_Arg_T *Arg);			/* OK */

void Radix4FFT_DIT        (FFT_Arg_T *Arg);			/* OK */
void Radix4FFT_DIT_Inverse(FFT_Arg_T *Arg);			/* OK */

void Radix4FFT_DIF            (FFT_Arg_T *Arg);			/* OK */
void Radix4FFT_DIF_Par        (FFT_Arg_T *Arg);			/* OK */

void Radix4FFT_DIF_Inverse    (FFT_Arg_T *Arg);			/* OK */
void Radix4FFT_DIF_Par_Inverse(FFT_Arg_T *Arg);			/* OK */

void Radix2FFT_DIT    (FFT_Arg_T *Arg);				/* OK */
void Radix2FFT_DIF    (FFT_Arg_T *Arg);				/* OK */
void Radix2FFT_DIF_Par(FFT_Arg_T *Arg);				/* OK */


void SwapSamples    (SwapSamples_Arg_T *Arg);			/* OK */
void SwapSamples_Par(SwapSamples_Arg_T *Arg);			/* OK */

void Radix4FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg);		/* OK */
void Radix4FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg);		/* OK */

void Radix4FFT_DIF_2D_Scalar_Horizontal(FFT_2D_Arg_T *Arg);
void Radix4FFT_DIF_2D_Scalar_Vertical(FFT_2D_Arg_T *Arg);

void Radix2FFT_DIF_2D_Horizontal(FFT_2D_Arg_T *Arg);		/* OK */
void Radix2FFT_DIF_2D_Vertical(FFT_2D_Arg_T *Arg);		/* OK */

void Radix2FFT_DIF_2D_Scalar_Horizontal(FFT_2D_Arg_T *Arg);
void Radix2FFT_DIF_2D_Scalar_Vertical(FFT_2D_Arg_T *Arg);

void SwapSamples_2D_Horizontal_Par(SwapSamples_2D_Arg_T *Arg);	/* OK */
void SwapSamples_2D_Vertical_Par(SwapSamples_2D_Arg_T *Arg);	/* OK */

void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg);


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
