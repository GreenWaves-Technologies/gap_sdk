/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __FFT2D_GENERATOR_H__
#define __FFT2D_GENERATOR_H__

/** @addtogroup groupFFT2D
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
void FFT2DConfiguration(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory);

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

/** @} */

#endif
