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

void FIRConfiguration(unsigned int L1Memory)

{
        SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
        SetSymbolNames("FIR_L1_Memory", "FIR_L2_Memory");
        SetSymbolDynamics();

        SetUsedFilesNames(0, 2, "FirBasicKernels.h","DSP_FloatType.h");
        SetGeneratedFilesNames("FirKernels.c", "FirKernels.h");

        SetL1MemorySize(L1Memory);
}


int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    FIRConfiguration(3000);
    // Load FIR basic kernels
    LoadFIRLibrary();

    // Number of Samples
    unsigned int Samples = 400;
   
#   if FIR_FLOAT32
    GeneratorFIR_fp32("TiledFir", Samples, 74);
#   else
    GeneratorFIR_fp16("TiledFir", Samples, 74);
#   endif
    
    
    // Generate code
    GenerateTilingCode();
}

/** @} */ // End of FIRExample
/** @} */
