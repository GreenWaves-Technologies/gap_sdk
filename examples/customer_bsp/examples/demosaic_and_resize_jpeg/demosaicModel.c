/*
 * Copyright (C) 2021 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdio.h>

// AutoTiler Libraries
#include "AutoTilerLib.h"
// Resize generator
#include "DeMosaicGenerator.h"

// Types of kernels are based on bayer filter first 4 pixel block:
// 
// IE for KerDeMosaicGRBG:
// *************************
// * Green *  Red  *  ...  *
// *************************
// * Blue  * Green *  ...  *
// *************************
// *  ...  *  ...  *  ...  *

int main(int argc, char **argv)
{
	//BayerOrder_t bt = GRBG;
	BayerOrder_t bt = GBRG;
	//BayerOrder_t bt = BGGR;
	// This will parse AutoTiler options and perform various initializations
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	DeMosaicConfiguration(40000,300000);
	// Load the Resize basic kernels template library
	LoadDeMosaicLibrary();
	// Call Resize generator, here image is [320x240]
	unsigned int W = 320, H = 240;
	Simple_DeMosaic_Resize("DeMosaicing", W, H, 0,0, bt);
	// Now that we are done with model parsing we generate the code
	GenerateTilingCode();
	return 0;
}
