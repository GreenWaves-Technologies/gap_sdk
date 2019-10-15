/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __MATMULT_GENERATOR_H__
#define __MATMULT_GENERATOR_H__

/** @addtogroup groupMATMULT
@{ */

/** @defgroup MATMULTGene MATMULTGenerator
@{ */

/**
@brief Load matrix multiplication basic kernels

Load matrix multiplication basic kernels
*/
void LoadMatMultLibrary();

/**
@brief Setup AutoTiler configuration for matrix multiplication

Setup AutoTiler configuration
*/
void MatMultConfiguration(unsigned int L1Memory);


/**
@brief Generate matrix multiplication for a given set of parameters for integer input and output data

Generate matrix multiplication for a given set of parameters
*/
void GenerateMatMultPar(
    char *Name,                 /**< C Name of the function AutoTiler will generate */
    unsigned int LineM1,        /**< Number of lines in first argument */
    unsigned int ColM1,         /**< Number of columns in first argument */
    unsigned int LineM2,        /**< Number of lines in second argument */
    unsigned int ColM2          /**< Number of columns in second argument */
);


/**
@brief Generate matrix multiplication for a given set of parameters for integer input and output data

Generate matrix multiplication for a given set of parameters. Vectorial basic kernel implementation.
*/
void GenerateMatMultParVect(
    char *Name,                 /**< C Name of the function AutoTiler will generate */
    unsigned int LineM1,        /**< Number of lines in first argument */
    unsigned int ColM1,         /**< Number of columns in first argument */
    unsigned int LineM2,        /**< Number of lines in second argument */
    unsigned int ColM2          /**< Number of columns in second argument */
);

/** @} */ // End of MATMULTGene
/** @} */

#endif


