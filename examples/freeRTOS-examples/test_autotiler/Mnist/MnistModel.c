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
#include "GenTiling.h"
#include "ModelCreate.h"
#include "TilingGenCode.h"
#include "StdTypes.h"
#include "StdModel.h"

void CnnModel(unsigned int L1Memory, int UseHWCE)

{

	SetInlineMode(ALWAYS_INLINE);
	// SetSymbolDynamics();

	SetUsedFilesNames("KernelLibStdTypes.h", "BasicKernelLib.h");
	SetGeneratedFilesNames("MnistKernelsInit.c", "MnistKernelsInit.h", "MnistKernels.c", "MnistKernels.h");

	SetL1MemorySize(L1Memory);

	LoadSoftwareKernelLibrary();
	LoadHWCEKernelLibrary();

	if (UseHWCE) {
		TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_0", 5,  1, 32, 28, 28, 1);
		TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_1", 5, 32, 64, 12, 12, 1);
	} else {
		TiledConvNxNReLUPool2x2_SW_fp	("Conv5x5ReLUMaxPool2x2_0",      5,  1, 32, 28, 28, 1);
		TiledConvNxNReLUPool2x2_SW_fp	("Conv5x5ReLUMaxPool2x2_1",      5, 32, 64, 12, 12, 1);
	}
	TiledLinearLayer     			("LinearLayerReLU_2", 		64,  4,  4, 10,  1, 0, 0);
}

int main(int argc, char **argv)

{
	if (TilerParseOptions(argc, argv)) return 1;
	if (GlobalInit()==0) {
		printf("Failed to initialize or incorrect output directory.\n"); return 0;
	}
	CnnModel(51200, 1);
	GenerateTilingCode();
	return 0;
}

