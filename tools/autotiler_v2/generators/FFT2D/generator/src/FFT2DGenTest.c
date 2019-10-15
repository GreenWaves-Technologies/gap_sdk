/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "AutoTilerLib.h"
// FFT2D generator
#include "FFT2DGenerator.h"

int main(int argc, char **argv)

{
    if (TilerParseOptions(argc, argv)) {
        printf("Failed to initialize or incorrect output directory.\n"); return 0;
    }
    FFT2DConfiguration(51200);
    LoadFFT2DLibrary();
    GeneratorFFT2D("FFT2D_256", 256, 0, 0);
    GeneratorFFT2D("FFT2D_128", 128, 0, 0);
    GenerateTilingCode();
    return 0;
}