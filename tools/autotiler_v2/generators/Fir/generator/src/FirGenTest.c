/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "AutoTilerLib.h"
// Fir generator
#include "FirGenerator.h"

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
 
    // Set Auto Tiler configuration, given shared L1 memory is 51200
    FIRConfiguration(51200);
    // Load FIR basic kernels
    LoadFIRLibrary();

    // Number of Samples
    unsigned int Samples = 49149;
    // 40 Taps, Generic and scalar basic kernel
    GeneratorFIR("TiledFir40GenericScalar", Samples, 40, 1, 1);
    // 40 Taps, Specialized and vectorial basic kernel
    GeneratorFIR("TiledFir40Generic", Samples, 40, 0, 0);

    // 10 Taps, Generic and scalar basic kernel
    GeneratorFIR("TiledFir10GenericScalar", Samples, 10, 1, 1);
    // 10 Taps, Generic and vectorial basic kernel
    GeneratorFIR("TiledFir10Generic", Samples, 10, 1, 0);
    // 10 Taps, Specialized and vectorial basic kernel
    GeneratorFIR("TiledFir10Opt", Samples, 10, 0, 0);

    // 20 Taps, Generic and scalar basic kernel
    GeneratorFIR("TiledFir20GenericScalar", Samples, 20, 1, 1);
    // 20 Taps, Generic and vectorial basic kernel
    GeneratorFIR("TiledFir20Generic", Samples, 20, 1, 0);
    // 20 Taps, Specialized and vectorial basic kernel
    GeneratorFIR("TiledFir20Opt", Samples, 20, 0, 0);

    // Generate code
    GenerateTilingCode();
}