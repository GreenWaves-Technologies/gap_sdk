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

void Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory)

{
    SetSymbolDynamics();

    SetUsedFilesNames(0, 1, "MatMultBasicKernels.h");
    SetGeneratedFilesNames("MatMultKernels.c", "MatMultKernels.h");
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

    SetMemoryDeviceInfos(4,
		AT_MEM_L1, L1Memory, "L1_Memory", 0, 0,
		AT_MEM_L2, L2Memory, "L2_Memory", 0, 0,
		AT_MEM_L3_HRAM, L3Memory, "L3_Memory", 0, 0,
		AT_MEM_L3_HFLASH, 20*1024*1024, "0", "L3_Flash_Const.dat", 0
	);

	LoadMatMultLibrary();
	
	ParMatMultGenerator    ("ParMatMult",     200, 70, 70, 150, 1); //Short int
    ParVectMatMultGenerator("ParVectMatMult", 200, 70, 70, 150, 1); //Short int

}


int main(int argc, char **argv)

{
        if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
        Model(45000, 360*1024, 8*1024*1024);
        GenerateTilingCode();
        return 0;
}
