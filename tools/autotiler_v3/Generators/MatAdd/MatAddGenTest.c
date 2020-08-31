/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "AutoTilerLib.h"
#include "MatAddGenerator.h"

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
    MatAddConfiguration(L1_MEMORY_SIZE);
    LoadMatAddLibrary();
    GenerateMatAddPar("MatADD", MAT_W, MAT_H);
    GenerateTilingCode();
    return 0;
}
