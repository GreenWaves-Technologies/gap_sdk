/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "AutoTilerLib.h"
// HOG parameters definitions
#include "HoGParameters.h"
// HOG generator
#include "HoGGenerator.h"

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
    // Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
    // compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
    HOGConfiguration(51200);
    // Load the HOG basic kernels template library
    LoadHOGLibrary();
    // Call HOG generator, here image is [644x482], the HOG parameters come from HOGParameters.h
    unsigned int W = 322, H = 242;
    GenerateHOG("MyHOG", W, H, HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
    GenerateTilingCode();
    return 0;
}