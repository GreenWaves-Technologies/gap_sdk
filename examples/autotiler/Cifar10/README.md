# CIFAR10 image recognition

## Introduction

This document describes a CIFAR10 image recognition application based on a convolutional neural network (CNN) on the GAP8 processor.
The example can be compiled and executed on the virtual model of GAP8 available in the SDK.

In the example, 32x32 pixels images are stored in the L2 memory of the fabric controller.
The images are then processed by the CNN on the 8 cores of the cluster. The CNN classifies the images in 10 categories, by selecting the maximum confidence value at the output as the inferred category.

The network topology (number of layers, number of inputs and outputs for the layers) has been taken from an open source project on the Internet. In this example, we demonstrate the implementation 
of this network on GAP8 using adapted data management through DMA and optimized parallelized filter kernels taking benefits of the 8 cores of the cluster.

The CNN has been trained off-line using the CIFAR10 dataset. The recognition ratio reached after training is 68%, which is very close to the asymptotic ratio announced by the authors.
Actually more sophisticated topologies can achieve a better rate, but this example is given more as a simpler tutorial for CNN implementation principles on GAP8 architecture.
Specifically, it demonstrates how to perform data management between L2 memory in the fabric controller and the L1 cluster shared memory so that no memory overflow occurs while keeping all the compute resources busy.
It also shows how the basic filter kernels (2D convolution filter and linear filters) can be described to use the 8 cores of the cluster efficiently so as to reduce computation time and hence power consumption of the application.

In the next sections, we will describe the structure of the example directory, the structure of the CNN and its software components and finally how to run the application on the GAP8 simulator.

## Automatic layer code generation

### Cifar10 with automatic tiling generation

For this cifar10 example, the low level code for the CNN layers (convolutional and dense linear layers) has been generated using the auto-tiling tool available in this SDK.

The generator assumes that the data and weights are stored on the L2 memory (in the Fabric Controller). The available size of L1 shared memory for processing is specified to the tiler.
The data and weights are split into tiles, forming  data sets that will be loaded through DMA to the L1 shared memory of the cluster. The cores will process the data and when finished, the results are stored back into the L2.
The tool takes the "high level" parameters of the layers as input and generates the low level functions describing the data management and the sync run time functions to execute the layers in parallel on the eight cores of the cluster.

Data transfers and processing are pipelined. Doing so, the DMA processes and the data processing can be executed in parallel on different data sets to optimize the execution time.

The following files are automatically generated:
- Cifar10Kernels.c	    : contains the generated code for the layers, with the calls to run time DMA process and processor synchronization
- Cifar10Kernel.h	      : contains the header for Convlayer and dense layer functions

### How to automatically generate the CIFAR10 CNN code with tiling 

The input file containing the "high level" description of the layers used in cifar10 is Model_kernel.c.

Following is the description of the different sections in the input file:

~~~~~c
  SetInlineMode(SINGLE_INLINE);
~~~~~

This instruction specifies that a single function will be generated if several occurrences of it are generated.

To Generate one function per call the argument to pass to the SetInlineMode function is "ALWAYS_INLINE".

The following instructions specify the name of the library header files (SetUsedFIlesNames), the names of the generated files (SetGeneratedFilesNames), and the declaration of the amount of L1 memory to be used by the auto-tiler generator.

~~~~~c
SetSymbolNames("Cifar10_L1_Memory", "Cifar10_L2_Memory");
SetUsedFilesNames(0, 2, "pmsis.h", "CNN_BasicKernels.h");
  
SetL1MemorySize(L1Memory);
~~~~~

The kernel libraries are loaded for usage by the tiler:

~~~~~c
LoadCNNLibrary();
~~~~~

The layer parameters are defined by the code below:

~~~~~c
// cifar10 config 
// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
// 1 input plane [32x32], 8 output planes [14x14]
    CNN_ConvolutionPoolReLU("Conv5x5MaxPool2x2_SW_0", 0, 2, 2, 2, 2, 0, 0, 0, 0, 1, 8, 32, 32, KOP_CONV, 5, 5, 1, 1, 1, 1, 0, KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 1, KOP_NONE);
// 5x5 Convolution followed by 2x2 Max pooling. Pure SW.
// 8 input planes [14x14], 12 output planes [5x5]
    CNN_ConvolutionPoolReLU("Conv5x5MaxPool2x2_SW_1", 0, 2, 2, 2, 2, 0, 0, 0, 0, 8, 12, 14, 14, KOP_CONV, 5, 5, 1, 1, 1, 1, 0, KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 1, KOP_NONE);
// Linear Layer
// Input 12 x [5x5], Output 10
    CNN_LinearReLU("LinearLayerReLU_1", 0, 2, 2, 2, 2, 0, 0, 0, 0, 12*5*5, 10, KOP_LINEAR, KOP_NONE);

~~~~~

## Compile and run the application

### Single execution

To execute the application on the specific image contained in RGB.img, just type the following in the cifar10 directory:

~~~~~sh
	make clean all run
~~~~~
