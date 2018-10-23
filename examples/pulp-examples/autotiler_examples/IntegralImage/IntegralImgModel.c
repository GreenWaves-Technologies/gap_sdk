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

#include "AutoTilerLib.h"
#include "IntegralImgGenerator.h"

int main(int argc, char **argv)

{
	// This will parse AutoTiler options and perform various initializations
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	IntegralImageConfiguration(40000);
	// Load basic kernels template library
	LoadIntegralImageLibrary();
	// Call generator, with the size of the input image
	unsigned int W = 322, H = 242;
	GenerateIntegralImage("IntegralImage", W, H);
	// Now that we are done with model parsing we generate the code
	GenerateTilingCode();
	return 0;
}
