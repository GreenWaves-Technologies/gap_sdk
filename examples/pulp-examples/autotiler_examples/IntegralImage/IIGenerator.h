/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __II_GENERATOR_H__
#define __II_GENERATOR_H__


void LoadIntegralImageLibrary();

void IntegralImageConfiguration(
	unsigned int L1Memory	/**< Maximum amount of memory in byte usable by the AutoTiler in shared L1 */
	);

void GenerateIntegralImage(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H 		/* Image Height */
	);

#endif //__II_GENERATOR_H__
