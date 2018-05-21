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
// Import AutoTiler lib
#include "AutoTilerLib.h"
// Import CNN generators
#include "CNN_Generator.h"

/** @addtogroup groupCNN
@{ */

/** @defgroup Cifar10Exa Cifar10Example

This code will build the whole CNN structure for Cifar10.

        Cifar10 takes small input images, 32x32, of natural scene and figures out to which class this image belongs to.

        To finalize a fully functional implementation of Cifar10 you need to allocate and deallocate the dynamic memory each CNN layer
        might need in L2 memory and of course pass the trained coefficients. The code bellow is a possible fully functional
        implementation. Filter_Layer0, Bias_Layer0, Filter_Layer1, Bias_Layer1, Filter_Layer2, Bias_Layer2 are the trained coefficients.



@code
void Cifar10(short int *Image_In, short int *Cifar10_Out)

{
	Allocate(Out_Layer0, 8*14*14*sizeof(short));
        Conv5x5MaxPool2x2_SW_0(Image_In, Filter_Layer0, Out_Layer0, 14, Bias_Layer0, 0);
	Allocate(Out_Layer1, 12*5*5*sizeof(short));
        Conv5x5MaxPool2x2_SW_1(Out_Layer0, Filter_Layer1, Out_Layer1, 14, Bias_Layer1, 0);
	DeAllocate(Out_Layer0);
	Allocate(Cifar10_Out, 10*sizeof(short));
        LinearLayerReLU_1(Out_Layer1, Filter_Layer2, 16, Bias_Layer2, 10, Cifar10_Out, 0, 0)
	DeAllocate(Out_Layer1);
}
@endcode


	Here is the Cifar10 generator itself:

@code
// Import AutoTiler lib
#include "AutoTilerLib.h"
// Import CNN generators
#include "CNN_Generator.h"

void Cifar10Configuration(unsigned int L1Memory)

{
	// Always inline user kernels
	SetInlineMode(ALWAYS_INLINE);
	// C symbols used by the AutoTiler
	SetSymbolNames("Cifar10_L1_Memory", "Cifar10_L2_Memory", "Cifar10_KernelDescr", "Cifar10_KernelArgs");
	// L1 and L2 symbols are dynamic
	SetSymbolDynamics();
	// Standard data types are used, we import CNN basic kernels
	SetUsedFilesNames("KernelLibStdTypes.h", 1, "CNN_BasicKernels.h");
	// Auto Tiler generated files
	SetGeneratedFilesNames("Cifar10KernelsInit.c", "Cifar10KernelsInit.h", "Cifar10Kernels.c", "Cifar10Kernels.h");
	// L1 shared memory given to Auto Tiler
	SetL1MemorySize(L1Memory);
}

void Cifar10Generator(int UseHWCE)

{
	if (UseHWCE) {
		// 5x5 Convolution followed by 2x2 Max pooling. HWCE based.
		// 1 input plane [32x32], 8 output planes [14x14]
		CNN_TiledConvNxNReLUPool2x2_HWCE_fp("Conv5x5MaxPool2x2_HWCE_0", 5, 1,  8, 32, 32, 3);
		// 5x5 Convolution followed by 2x2 Max pooling. HWCE based.
		// 8 input planes [14x14], 12 output planes [5x5]
		CNN_TiledConvNxNReLUPool2x2_HWCE_fp("Conv5x5MaxPool2x2_HWCE_1", 5, 8, 12, 14, 14, 3);
	} else {
		// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
		// 1 input plane [32x32], 8 output planes [14x14]
		CNN_TiledConvNxNReLUPool2x2_SW_fp  ("Conv5x5MaxPool2x2_SW_0",   5, 1,  8, 32, 32, 3);
		// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
		// 8 input planes [14x14], 12 output planes [5x5]
		CNN_TiledConvNxNReLUPool2x2_SW_fp  ("Conv5x5MaxPool2x2_SW_1",   5, 8, 12, 14, 14, 3);
	}
	// Linear Layer
	// Input 12 x [5x5], Output 10
	CNN_TiledLinearLayer("LinearLayerReLU_1", 12, 5, 5, 10, 1, 0, 0);
}

int main(int argc, char **argv)

{
	// Parse AutoTiler options
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Set Auto Tiler configuration, given shared L1 memory is 51200
	Cifar10Configuration(51200);
	// Load SW CNN basic kernels
	CNN_LoadSoftwareKernelLibrary();
	// Load HWCE enabled CNN basic kernels
	CNN_LoadHWCEKernelLibrary();
	// Generate Cifar10, here without using HWCE
	Cifar10Generator(0);
	// Generate code
	GenerateTilingCode();
	return 0;
}
@endcode
@{ */

void Cifar10Configuration(unsigned int L1Memory)

{
	// Always inline user kernels
	SetInlineMode(ALWAYS_INLINE);
	// C symbols used by the AutoTiler
	SetSymbolNames("Cifar10_L1_Memory", "Cifar10_L2_Memory", "Cifar10_KernelDescr", "Cifar10_KernelArgs");
	// L1 and L2 symbols are dynamic
	SetSymbolDynamics();
	// Standard data types are used, we import CNN basic kernels
	SetUsedFilesNames("KernelLibStdTypes.h", 1, "CNN_BasicKernels.h");
	// Auto Tiler generated files
	SetGeneratedFilesNames("Cifar10KernelsInit.c", "Cifar10KernelsInit.h", "Cifar10Kernels.c", "Cifar10Kernels.h");
	// L1 shared memory given to Auto Tiler
	SetL1MemorySize(L1Memory);
}

void Cifar10Generator()

{
	#if RT_HAS_HWCE
		// 5x5 Convolution followed by 2x2 Max pooling. HWCE based.
		// 1 input plane [32x32], 8 output planes [14x14]
		CNN_TiledConvNxNReLUPool2x2_HWCE_fp("Conv5x5MaxPool2x2_HWCE_0", 5, 1,  8, 32, 32, 3);
		// 5x5 Convolution followed by 2x2 Max pooling. HWCE based.
		// 8 input planes [14x14], 12 output planes [5x5]
		CNN_TiledConvNxNReLUPool2x2_HWCE_fp("Conv5x5MaxPool2x2_HWCE_1", 5, 8, 12, 14, 14, 3);
	#else
		// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
		// 1 input plane [32x32], 8 output planes [14x14]
		CNN_TiledConvNxNReLUPool2x2_SW_fp  ("Conv5x5MaxPool2x2_SW_0",   5, 1,  8, 32, 32, 3);
		// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
		// 8 input planes [14x14], 12 output planes [5x5]
		CNN_TiledConvNxNReLUPool2x2_SW_fp  ("Conv5x5MaxPool2x2_SW_1",   5, 8, 12, 14, 14, 3);
	#endif
	// Linear Layer
	// Input 12 x [5x5], Output 10
	CNN_TiledLinearLayer("LinearLayerReLU_1", 12, 5, 5, 10, 1, 0, 0);
}

int main(int argc, char **argv)

{
	// Parse AutoTiler options
	if (TilerParseOptions(argc, argv)) {
		printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
	}
	// Set Auto Tiler configuration, given shared L1 memory is 51200
	Cifar10Configuration(51200);
	// Load SW CNN basic kernels
	CNN_LoadSoftwareKernelLibrary();
	// Load HWCE enabled CNN basic kernels
	CNN_LoadHWCEKernelLibrary();
	// Generate Cifar10, here without using HWCE
	Cifar10Generator();
	// Generate code
	GenerateTilingCode();
	return 0;
}

/** @} */ // End of Cifar10Exa
/** @} */

