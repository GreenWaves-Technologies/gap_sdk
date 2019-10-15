/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __FFT2D_GENERATOR_H__
#define __FFT2D_GENERATOR_H__

/** @addtogroup groupFFT2D
@{ */

/** @defgroup FFT2DGene FFT2DGenerator
@{ */

/**
@brief Load FFT2D basic kernels

Load FFT2D basic kernels
*/
void LoadFFT2DLibrary();

/**
@brief Setup AutoTiler configuration for FFT2D

Setup AutoTiler configuration
*/
void FFT2DConfiguration(unsigned int L1Memory);


/**
@brief Generate FFT2D for a given set of parameters

Generate FFT2D for a given set of parameters
*/
void GeneratorFFT2D(
    char *Name,             /**< C Name of the function AutoTiler will generate */
    unsigned int Dim,       /**< Document me! */
    int ForceRadix2,        /**< Document me! */
    int Scalar              /**< Document me! */
    );

/** @} */ // End of FFT2DGene
/** @} */

#endif