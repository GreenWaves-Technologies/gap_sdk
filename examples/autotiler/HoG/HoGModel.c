/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdio.h>

/** @addtogroup groupHOG
@{ */

/** @defgroup HOGExa HOGExample

This section shows how to use the HOG Generator on 644x482 input image

@code
// AutoTiler Libraries
#include "AutoTilerLib.h"
// HOG parameters definitions
#include "HoGParameters.h"
// HOG generator
#include "HoGGenerator.h"

int main(int argc, char **argv)

{
	// This will parse AutoTiler options and perform various initializations
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	HOGConfiguration(40000);
	// Load the HOG basic kernels template library
	LoadHOGLibrary();
	// Call HOG generator, here image is [644x482], the HOG parameters come from HOGParameters.h
	unsigned int W = 322, H = 242;
	GenerateHOG("MyHOG", W, H, HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	// Now that we are done with model parsing we generate the code
	GenerateTilingCode();
	return 0;
}

@endcode

@{ */

// AutoTiler Libraries
#include "AutoTilerLib.h"
// HOG parameters definitions
#include "HoGParameters.h"
// HOG generator
#include "HoGGenerator.h"

int main(int argc, char **argv)

{
	// This will parse AutoTiler options and perform various initializations
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	HOGConfiguration(40000);
	// Load the HOG basic kernels template library
	LoadHOGLibrary();
	// Call HOG generator, here image is [644x482], the HOG parameters come from HOGParameters.h
	unsigned int W = 322, H = 242;
	GenerateHOG("MyHOG", W, H, HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	// Now that we are done with model parsing we generate the code
	GenerateTilingCode();
	return 0;
}
/** @} */ // End of HOGExample
/** @} */

