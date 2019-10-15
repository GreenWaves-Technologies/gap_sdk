/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "AutoTilerLib.h"
#include "MatMultGenerator.h"

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
    MatMultConfiguration(45000);
    LoadMatMultLibrary();
    GenerateMatMultPar    ("ParMatMult",     200, 70, 70, 150);
    GenerateMatMultParVect("ParVectMatMult", 200, 70, 70, 150);
    GenerateTilingCode();
    return 0;
}