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
#include "AutoTilerLib.h"
#include "CNN_Generators.h"

void Cifar10Model(unsigned int L1Memory)
{
    SetInlineMode(ALWAYS_INLINE);
    SetSymbolDynamics();

    SetSymbolNames("Cifar10_L1_Memory", "Cifar10_L2_Memory");

    SetUsedFilesNames(0, 2, "pmsis.h", "CNN_BasicKernels.h");
    SetGeneratedFilesNames("Cifar10Kernels.c", "Cifar10Kernels.h");

    SetL1MemorySize(L1Memory);
    LoadCNNLibrary();

    // 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
    // 1 input plane [32x32], 8 output planes [14x14]
    CNN_ConvolutionPoolReLU("Conv5x5MaxPool2x2_SW_0", 0, 2, 2, 2, 2, 0, 0, 0, 0, 1, 8, 32, 32, KOP_CONV, 5, 5, 1, 1, 1, 1, 0, KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 1, KOP_NONE);
    // 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
    // 8 input planes [14x14], 12 output planes [5x5]
    CNN_ConvolutionPoolReLU("Conv5x5MaxPool2x2_SW_1", 0, 2, 2, 2, 2, 0, 0, 0, 0, 8, 12, 14, 14, KOP_CONV, 5, 5, 1, 1, 1, 1, 0, KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 1, KOP_NONE);
    // Linear Layer
    // Input 12 x [5x5], Output 10
    CNN_LinearReLU("LinearLayerReLU_1", 0, 2, 2, 2, 2, 0, 0, 0, 0, 12*5*5, 10, KOP_LINEAR, KOP_NONE);
}

int main(int argc, char **argv)
{
    // Parse AutoTiler options
    if (TilerParseOptions(argc, argv))
    {
        printf("Failed to initialize or incorrect output arguments directory.\n");
        return 1;
    }
    // Set Auto Tiler configuration, given shared L1 memory is 51200
    Cifar10Model(51200);
    // Generate code
    GenerateTilingCode();
    return 0;
}


