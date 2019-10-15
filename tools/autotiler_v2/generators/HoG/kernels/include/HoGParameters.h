/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __HOG_PARAMETERS_H__
#define __HOG_PARAMETERS_H__

/** @addtogroup groupHOG
@{
*/

/** @defgroup HOGPar HOGParameters

Hog General parameters
@{ */

/**
@brief  Size of the side of a square cell of pixels, total pixels in cells is HOG_CELL_SIZE^2 
*/
#define HOG_CELL_SIZE           8

/**
@brief Size of the side of a square block of pixel cells, Total size in pixels is (HOG_BLOCK_SIZE^2*HOG_CELL_SIZE^2)
*/
#define HOG_BLOCK_SIZE          2

/**
@brief Overlapping factor between 2 adjacent blocks
*/
#define HOG_BLOCK_OVERLAP       1

/**
@brief Number of degrees between 2 bins of the gradient histograms
*/
#define HOG_ANGLE_STEP          20

/**
@brief Total number of bins for 0..180 degree, we use unsigned orientations
*/
#define HOG_NBINS               (180/HOG_ANGLE_STEP)

/** @} */ // End of HOGPar

/** @} */

#endif

