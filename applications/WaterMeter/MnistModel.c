/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>
// Import AutoTiler lib
#include "AutoTilerLib.h"
// Import CNN generators
#include "CNN_Generators.h"

/** @addtogroup groupCNN
  @{ */

/** @defgroup MnistExa MnistExample

  This code will build the whole CNN structure for Mnist.

  Mnist takes small input images, 28x28, of natural scene and figures out to which class, among 10, this image belongs to.

  To finalize a fully functional implementation of Mnist you need to allocate and deallocate the dynamic memory each CNN layer
  might need in L2 memory and of course pass the trained coefficients. The code bellow is a possible fully functional
  implementation. Filter_Layer0, Bias_Layer0, Filter_Layer1, Bias_Layer1, Filter_Layer2, Bias_Layer2 are the trained coefficients.


  @code
  void Mnist(short int *Image_In, short int *Mnist_Out)

  {

  Allocate(Out_Layer0, 32*12*12*sizeof(short));
  Conv5x5ReLUMaxPool2x2_0(Image_In, Filter_Layer0, Out_Layer0, 14, Bias_Layer0, 0);
  Allocate(Out_Layer1, 64*4*4*sizeof(short));
  Conv5x5ReLUMaxPool2x2_1(Out_Layer0, Filter_Layer1, Out_Layer1, 14, Bias_Layer1, 0);
  DeAllocate(Out_Layer0);
  Allocate(Mnist_Out, 10*sizeof(short));
  LinearLayerReLU_2(Out_Layer1, Filter_Layer2, 16, Bias_Layer2, 10, Mnist_Out, 10, 0);
  }
  @endcode


  Here is the Mnist generator code itself:

  @code
// Import AutoTiler lib
#include "AutoTilerLib.h"
// Import CNN generators
#include "CNN_Generator.h"

void MnistConfiguration(unsigned int L1Memory)

{
// Always inline user kernels
SetInlineMode(ALWAYS_INLINE);
// C symbols used by the AutoTiler
SetSymbolNames("Mnist_L1_Memory", "Mnist_L2_Memory", "Mnist_KernelDescr", "Mnist_KernelArgs");
// L1 and L2 symbols are dynamic
SetSymbolDynamics();
// Standard data types are used, we import CNN basic kernels
SetUsedFilesNames("KernelLibStdTypes.h", 1, "CNN_BasicKernels.h");
// Auto Tiler generated files
SetGeneratedFilesNames("MnistKernelsInit.c", "MnistKernelsInit.h", "MnistKernels.c", "MnistKernels.h");
// L1 shared memory given to Auto Tiler
SetL1MemorySize(L1Memory);
}

void MnistGenerator(int UseHWCE)

{
if (UseHWCE) {
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
// 1 input plane [28x28], 32 output planes [12x12]
CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_0", 5,  1, 32, 28, 28, 1);
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
// 32 input planes [12x12], 64 output planes [4x4]
CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_1", 5, 32, 64, 12, 12, 1);
} else {
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
// 1 input plane [28x28], 32 output planes [12x12]
CNN_TiledConvNxNReLUPool2x2_SW_fp	("Conv5x5ReLUMaxPool2x2_0",      5,  1, 32, 28, 28, 1);
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
// 32 input planes [12x12], 64 output planes [4x4]
CNN_TiledConvNxNReLUPool2x2_SW_fp	("Conv5x5ReLUMaxPool2x2_1",      5, 32, 64, 12, 12, 1);
}
// Linear Layer, Input, Output and Coeffs on 16bits.
// Input 64 x [4x4], Output 10
CNN_TiledLinearLayer     			("LinearLayerReLU_2", 		64,  4,  4, 10,  1, 0, 0);
}


int main(int argc, char **argv)

{
    // Parse AutoTiler options
    if (TilerParseOptions(argc, argv)) {
        printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
    }
    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MnistConfiguration(51200);
    // Load SW CNN basic kernels
    CNN_LoadSoftwareKernelLibrary();
    // Load HWCE enabled CNN basic kernels
    CNN_LoadHWCEKernelLibrary();
    // Generate Mnist, here without using HWCE
    MnistGenerator(0);
    // Generate code
    GenerateTilingCode();
    return 0;
}
@endcode
@{ */

    void MnistConfiguration(unsigned int L1Memory)

    {
        // Always inline user kernels
        SetInlineMode(ALWAYS_INLINE);
        // C symbols used by the AutoTiler
        SetSymbolNames("Mnist_L1_Memory", "Mnist_L2_Memory", "Mnist_KernelDescr", "Mnist_KernelArgs");
        // L1 and L2 symbols are dynamic
        SetSymbolDynamics();
        // Standard data types are used, we import CNN basic kernels
        SetUsedFilesNames(0, 1, "CNN_BasicKernels.h");
        // Auto Tiler generated files
        SetGeneratedFilesNames("MnistKernelsInit.c", "MnistKernelsInit.h", "MnistKernels.c", "MnistKernels.h");
        // L1 shared memory given to Auto Tiler
        SetL1MemorySize(L1Memory);
    }

    void MnistGenerator()

    {
#if RT_HAS_HWCE
        // Load HWCE enabled CNN basic kernels
        CNN_LoadHWCEKernelLibrary();

        // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
        // 1 input plane [28x28], 32 output planes [12x12]
        CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_0", 5,  1, 32, 28, 28, 1,0,0);
        // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. HWCE based.
        // 32 input planes [12x12], 64 output planes [4x4]
        CNN_TiledConvNxNReLUPool2x2_HWCE_fp	("Conv5x5ReLUMaxPool2x2_HWCE_1", 5, 32, 64, 12, 12, 1,0,0);
#else
        // Load SW CNN basic kernels
        LoadCNNLibrary();
        // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
        // 1 input plane [28x28], 32 output planes [12x12]
        CNN_SmallParOutFeatConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_0", HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2, FROM_L2, FROM_L2, TO_L2,  1, 32, 28, 28, 5, 1, NO_PADDING, NO_RELU, 2, 2, NO_PADDING, RELU, MAX_POOLING);
        // 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
        // 32 input planes [12x12], 64 output planes [4x4]
        CNN_SmallParOutFeatConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_1", HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2, FROM_L2, FROM_L2, TO_L2,  32, 64, 12, 12, 5, 1, NO_PADDING, NO_RELU, 2, 2, NO_PADDING, RELU, MAX_POOLING);
#endif
        // Linear Layer, Input, Output and Coeffs on 16bits.
        // Input 64 x [4x4], Output 10
        CNN_LinearLayerReLU ("LinearLayerReLU_2",		HALF_WORD,HALF_WORD,HALF_WORD,HALF_WORD, FROM_L2,FROM_L2,FROM_L2,TO_L2, 64*4*4, 10, NO_RELU);
    }


    int main(int argc, char **argv)

    {
        // Parse AutoTiler options
        if (TilerParseOptions(argc, argv)) {
            printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
        }
        // Set Auto Tiler configuration, given shared L1 memory is 51200
        MnistConfiguration(51200);
        // Generate Mnist, here without using HWCE
        MnistGenerator();
        // Generate code
        GenerateTilingCode();
        return 0;
    }

    /** @} */ // End of MnistExa
/** @} */

