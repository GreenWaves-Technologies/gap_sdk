/* * Copyright (C) 2017 GreenWaves Technologies * All rights reserved.
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
#include "CNN_Generators.h"


void MnistModel(unsigned int L1Memory)

{
    // Always inline user kernels
    SetInlineMode(ALWAYS_INLINE);
    // L1 and L2 symbols are dynamic
    SetSymbolDynamics();
    // C symbols used by the AutoTiler
    SetSymbolNames("Mnist_L1_Memory", "Mnist_L2_Memory", "Mnist_KernelDescr", "Mnist_KernelArgs");
    // Standard data types are used, we import CNN basic kernels
    SetUsedFilesNames(0, 1, "CNN_BasicKernels.h");
    // Auto Tiler generated files
    SetGeneratedFilesNames("MnistKernelsInit.c", "MnistKernelsInit.h", "MnistKernels.c", "MnistKernels.h");
    // L1 shared memory given to Auto Tiler
    SetL1MemorySize(L1Memory);

    LoadCNNLibrary();

#if RT_HAS_HWCE
    CNN_LoadHWCEKernelLibrary();
    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
    // 1 input plane [28x28], 32 output planes [12x12]
    CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_0", 5,  1, 32, 28, 28, 1);
    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
    // 32 input planes [12x12], 64 output planes [4x4]
    CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_1", 5, 32, 64, 12, 12, 1);
#else
    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
    // 1 input plane [28x28], 32 output planes [12x12]
	CNN_SmallParOutFeatConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_0",     HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2, FROM_L2, FROM_L2, TO_L2,   1, 32, 28, 28, 5, 5, 1, 1, 1, 1, NO_PADDING, NO_RELU, 2, 2, 2, 2, NO_PADDING, RELU, MAX_POOLING);
    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
    // 32 input planes [12x12], 64 output planes [4x4]
	CNN_SmallParOutFeatConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_1",     HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2, FROM_L2, FROM_L2, TO_L2,  32, 64, 12, 12, 5, 5, 1, 1, 1, 1, NO_PADDING, NO_RELU, 2, 2, 2, 2, NO_PADDING, RELU, MAX_POOLING);
#endif
    // Linear Layer, Input, Output and Coeffs on 16bits.
    // Input 64 x [4x4], Output 10
	//CNN_ParOutFeatLinearLayerReLU ("LinearLayerReLU_1",		HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2,FROM_L2,FROM_L2,TO_L2, 64, 10, NO_RELU);
	CNN_LinearLayerReLU ("LinearLayerReLU_1",		HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2,FROM_L2,FROM_L2,TO_L2, 64*4*4, 10, NO_RELU);
}


int main(int argc, char **argv)

{
    // Parse AutoTiler options
    if (TilerParseOptions(argc, argv)) {
        printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
    }
    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MnistModel(51200);
    // Generate code
    GenerateTilingCode();
    return 0;
}


