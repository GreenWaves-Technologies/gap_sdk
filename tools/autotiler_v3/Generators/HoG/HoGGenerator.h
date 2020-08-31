/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __HOG_GENERATOR_H__
#define __HOG_GENERATOR_H__

/** @addtogroup groupHOG
@{ */

/** @defgroup HOGGene HOGGenerator
@{ */

/**
@brief Load HOG basic kernels

Load HOG basic kernels
*/
void LoadHOGLibrary();

/**
@brief Setup AutoTiler configuration for HOG

Setup AutoTiler configuration
*/
void HOGConfiguration(
	unsigned int L1Memory	/**< Maximum amount of memory in byte usable by the AutoTiler in shared L1 */
	);

/**
@brief Generate HOG for a given HOG set of parameters and for a given image size

Generate HOG for a given HOG set of parameters and for a given image size
*/
void GenerateHOG(char *Name,			/**< C Name of the function AutoTiler will generate */
		unsigned int W,			/**< Image width, width%4==0 should hold */
		unsigned int H, 		/**< Image Height */
		unsigned int CellSize, 		/**< Cell is [CellSize x CellSize] pixels */
		unsigned int BlockSize, 	/**< Block is [BlockSize x BlockSize] Cells */
		unsigned int BlockOverlap, 	/**< Number Cells in common between 2 adjacent blocks */
		unsigned int BinsPerCell	/**< Number of bins for a cell */
		);

/** @} */ // End of HOGGene

/** @} */
#endif
