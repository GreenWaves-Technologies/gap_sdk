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


void MnistModel(unsigned int L1Memory, unsigned int L2Memory)

{
    // Always inline user kernels
    SetInlineMode(ALWAYS_INLINE);
    // L1 and L2 symbols are dynamic
    SetSymbolDynamics();
    // Standard data types are used, we import CNN basic kernels
    SetUsedFilesNames(0, 1, "CNN_BasicKernels.h");
    // Auto Tiler generated files
    SetGeneratedFilesNames("MnistKernels.c", "MnistKernels.h");
    // L1 shared memory given to Auto Tiler
    SetMemoryDeviceInfos(4,
            AT_MEM_L1, L1Memory, "Mnist_L1_Memory", 0, 0,
            AT_MEM_L2, L2Memory, "Mnist_L2_Memory", 0, 0,
            AT_MEM_L3_DEFAULTRAM, 8*1024*1024, "Mnist_L3_Memory", 0, 1,
            AT_MEM_L3_DEFAULTFLASH, 20*1024*1024, "0", "Cnn_Flash_Const.dat", 0
            );

    // To use the Reordering you need to define a graph
    AT_SetGraphCtrl(AT_GRAPH_REORDER_CONSTANT_IN, AT_OPT_OFF);

    LoadCNNLibrary();

    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
    // 1 input plane [28x28], 32 output planes [12x12]
	CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_0",   0, 2,2,2,2, 12,14,14,12, 0,0,0,0,   1,32,28,28,  KOP_CONV, 5,5, 1,1, 1,1, 0,  KOP_MAXPOOL,   2,2, 1,1, 2,2, 1, KOP_RELU);
    // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
    // 32 input planes [12x12], 64 output planes [4x4]
    CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_1",   0, 2,2,2,2, 12,14,14,12, 0,0,0,0,   32,64,12,12,  KOP_CONV, 5,5, 1,1, 1,1, 0,  KOP_MAXPOOL,   2,2, 1,1, 2,2, 1, KOP_RELU);
    // Linear Layer, Input, Output and Coeffs on 16bits.
    // Input 64 x [4x4], Output 10
	CNN_LinearReLU         ("LinearLayerReLU_1",        0,  2,2,2,2, 12,12,8,11, 0,0,0,0,   64*4*4,10, KOP_LINEAR, KOP_NONE);
}


int main(int argc, char **argv)

{
    // Parse AutoTiler options
    if (TilerParseOptions(argc, argv)) {
        printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
    }
    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MnistModel(45000, 300000);
    // Generate code
    GenerateTilingCode();
    return 0;
}


