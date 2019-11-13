/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

// AutoTiler Libraries
#include "AutoTilerLib.h"
// FIR generator
#include "FirGenerator.h"

/** @addtogroup groupFIR
@{ */

/** @defgroup FIRExa FIRExample

@code
// AutoTiler Libraries
#include "AutoTilerLib.h"
// FIR generator
#include "FirGenerator.h"

int main(int argc, char **argv)

{
	// Parse AutoTiler options
        if (TilerParseOptions(argc, argv)) {
                printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
        }
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
        return 0;
}
// Running on 8 cores:
// Core 8 10 Taps 49149 Samples GenericScalar Cycles   376829. Cycles per Output Sample  7
// Core 8 10 Taps 49149 Samples GenericVect   Cycles   249925. Cycles per Output Sample 5
// Core 8 10 Taps 49149 Samples OptVect       Cycles   132479. Cycles per Output Sample  2
// Core 8 20 Taps 49149 Samples GenericScalar Cycles   626760. Cycles per Output Sample 12
// Core 8 20 Taps 49149 Samples GenericVect   Cycles   394910. Cycles per Output Sample  8
// Core 8 20 Taps 49149 Samples OptVect       Cycles   203772. Cycles per Output Sample  4
// Core 8 40 Taps 49149 Samples GenericScalar Cycles  1131093. Cycles per Output Sample 23
// Core 8 40 Taps 49149 Samples GenericVect   Cycles   688846. Cycles per Output Sample 14

@endcode

@{ */


int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    FIRConfiguration(51200);
    // Load FIR basic kernels
    LoadFIRLibrary();

    // Number of Samples
    unsigned int Samples = 32768;
   // 40 Taps, Generic and scalar basic kernel 
    GeneratorFIR("TiledFir40GenericScalar", Samples, 40, 1, 1);
    // 40 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version
    GeneratorFIR("TiledFir40Generic", Samples, 40, 0, 0);

    // 10 Taps, Generic and scalar basic kernel 
    GeneratorFIR("TiledFir10GenericScalar", Samples, 10, 1, 1);
    // 10 Taps, Generic and vectorial basic kernel => 2x16b vectorized version
    GeneratorFIR("TiledFir10Generic", Samples, 10, 1, 0);
    // 10 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version, and using  GAP8 buitins for multiply accumulate, and dot product
    GeneratorFIR("TiledFir10Opt", Samples, 10, 0, 0);

    // 20 Taps, Generic and scalar basic kernel
    GeneratorFIR("TiledFir20GenericScalar", Samples, 20, 1, 1);
    // 20 Taps, Generic and vectorial basic kernel=> 2x16b vectorized version
    GeneratorFIR("TiledFir20Generic", Samples, 20, 1, 0);
    // 20 Taps, Specialized and vectorial basic kernel 10 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version, and using GAP8 buitins for multiply accumulate, and dot product
    GeneratorFIR("TiledFir20Opt", Samples, 20, 0, 0);

    // Generate code
    GenerateTilingCode();
}

/** @} */ // End of FIRExample
/** @} */
