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

// AutoTiler Libraries
#include "AutoTilerLib.h"
// Resize generator
#include "ResizeGenerator.h"

int main(int argc, char **argv)
{
	// This will parse AutoTiler options and perform various initializations
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	ResizeConfiguration(40000);
	// Load the Resize basic kernels template library
	LoadResizeLibrary();
	// Call Resize generator, here image is [322x242]
	unsigned int W = 322, H = 242;
	// GenerateResize("ResizeImage", W, H, W/2, H/2);
	// GenerateResizeMultiChannel("S155_Op_Resize_432", 256, 1, 1024, 1, 16, 1, KOP_NEAREST_NEIGHBOR_RESIZE);
	// void GenerateResizeNew(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout, resize_kop_t Type)

	GenerateResizeNew("S155_Op_Resize_432", 256, 2, 1024, 1, KOP_NEAREST_NEIGHBOR_RESIZE);
	// Now that we are done with model parsing we generate the code
	GenerateTilingCode();
	return 0;
}
