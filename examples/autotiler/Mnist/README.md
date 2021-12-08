# MNIST image recognition

## Introduction

This document describes a MNINST hand written digit recognition application based on a convolutional neural network (CNN) on GAP8 processor.

In the example, 28x28 pixels images are loaded from host PC in the L2 memory of the fabric controller.
The images are then processed by the CNN on the 8 cores of the cluster. The CNN classifies the images in 10 categories (digit from 0 to 9), by selecting the maximum confidence value at the output as the inferred category.

The network topology (number of layers, number of inputs and outputs for the layers) has been taken from an open source project on the Internet. In this example, we demonstrate the implementation 
of this network on GAP8 using adapted data management through DMA and optimized parallelized filter kernels taking benefits of the 8 cores of the cluster.

The CNN has been trained off-line using the MNIST dataset. The recognition ratio reached after training is 96%, which is very close to the asymptotic ratio announced by the authors.
Actually more sophisticated topologies can achieve a better rate, but this example is given more as a simpler tutorial for CNN implementation principles on GAP8 architecture.
Specifically, it demonstrates how to perform data management between L2 memory in the fabric controller and the L1 cluster shared memory so that no memory overflow occurs while keeping all the compute resources busy.
It also shows how the basic filter kernels (2D convolution filter and linear filters) can be described to use the 8 cores of the cluster efficiently so as to reduce computation time and hence power consumption of the application.

In the next sections, we will describe the structure of the example directory, the structure of the CNN and its software components and finally how to run the application on the GAP8 simulator.

## Automatic Layer Code Generation

The low level code for the CNN layers has been generated using the auto-tiler tool available in the SDK.
The generator assumes that the data and weights are stored in the L2 memory (in the Fabric Controller). The available size of L1 shared memory for processing is specified to the auto-tiler.

The data and weights are split into tiles, forming  data sets that will be loaded through DMA to the L1 shared memory of the cluster. The cores will process the data and when finished, the results are stored back into the L2.
The tool takes the "high level" parameters of the layers as input and generates the low level functions describing the data management and the run-time functions to execute the layers in parallel on the eight cores of the cluster.

In MNIST example, a different version is generated for each instance, and the parameters are hard coded in the function.

The following files are automatically generated:
- MnistKernels.c	    : contains the generated code for the layers, with the calls to run time DMA process and processor synchronization
- MnistKernel.h	    : contains the header for Convlayer and dense layer functions

### How to automatically generate the MNIST CNN code with tiling 

The input file containing the "high level" description of the layers used in MNIST is MnistModel.c.
Following is the description of the different sections in the input file:

~~~~~c
  SetInlineMode(ALWAYS_INLINE);
~~~~~

This instruction specifies that a  function will be generated for each occurrence of the layer.
To generate a single parameterized function, the SetInlineMode function must be called with "SINGLE_INLINE" parameter.

The following instructions specify the name of the library header files (SetUsedFilesNames), the names of the generated files (SetGeneratedFilesNames), and the declaration of the amount of L1 memory to be used by the tiler generator.

~~~~~c
SetSymbolNames("Mnist_L1_Memory", "Mnist_L2_Memory");
// Standard data types are used, we import CNN basic kernels
SetUsedFilesNames(0, 1, "CNN_BasicKernels.h");
// Auto Tiler generated files
SetGeneratedFilesNames("MnistKernels.c", "MnistKernels.h");
// L1 shared memory given to Auto Tiler
SetL1MemorySize(L1Memory);
~~~~~

The kernel libraries are loaded for usage by the tiler:

~~~~~c
  LoadCNNLibrary();
~~~~~

The layer parameters are defined by the code below: 

~~~~~c
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
// 1 input plane [28x28], 32 output planes [12x12]
CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_0",   0, 2,2,2,2, 0,0,0,0,   1,32,28,28,  KOP_CONV, 5,5, 1,1, 1,1, 0,  KOP_MAXPOOL,   2,2, 1,1, 2,2, 1, KOP_RELU);
// 5x5 Convolution followed by ReLu and then by 2x2 Max pooling. Pure SW.
// 32 input planes [12x12], 64 output planes [4x4]
CNN_ConvolutionPoolReLU("Conv5x5ReLUMaxPool2x2_1",   0, 2,2,2,2, 0,0,0,0,   32,64,12,12,  KOP_CONV, 5,5, 1,1, 1,1, 0,  KOP_MAXPOOL,   2,2, 1,1, 2,2, 1, KOP_RELU);
// Linear Layer, Input, Output and Coeffs on 16bits.
// Input 64 x [4x4], Output 10
CNN_LinearReLU         ("LinearLayerReLU_1",        0, 2,2,2,2, 0,0,0,0,   64*4*4,10, KOP_LINEAR, KOP_NONE);
~~~~~

The generated files using the actual MnistModel present in the MNIST directory are generated in the example root directory.

## Compile and run the application

To execute the application on the specific image contained in RGB.img, just type the following in the MNIST directory:

~~~~~sh
	make clean all run
~~~~~

## Compile with cmake build alternative (freertos only)

~~~~~sh
  mkdir CMBUILD
  cd CMBUILD
  cmake -DCONFIG_GAP_SDK_HOME=$GAP_SDK_HOME .. -DCMAKE_MODULE_PATH=$GAP_SDK_HOME/utils/cmake -DCONFIG_CHIP=GAP9 -DCONFIG_CHIP_VERSION=2 -DCONFIG_PMSIS_OS=freertos
  cmake --build .
~~~~~
