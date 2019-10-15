# Finite Impulse Response (FIR) filter  

## Introduction

This document describes FIR filter examples using the kernels designed to run in parallel on the 8 cores of the GAP8 cluster.
The example can be compiled and executed on the virtual model of GAP8 available in the SDK, or on the GAPUINO board with the actual GAP8 chip.

In this examples, 3 filters are implemented, with 10, 20 and 40 taps. A stream of 32768 samples is processed, and the result is compared to a numpy golden output.

The distorsion with respect to the reference, and the performances (cycles) are estimated for each filter execution.

In the example, 32768 16 bits data are stored in the L2 memory of the fabric controller.
The stream is then processed by the FIR on the 8 cores of the cluster. 

In the next sections, we will describe the structure of the example directory, the structure of the filter code and its software components and finally how to run the application on the GAP8 simulator.

## Code structure

The program is comprised of the FC code, contained in main.c, the generated code (FirKernels.c) running on the processor 0 of the cluster, and the kernel code (FirBasicKernels.c in the tools/autotiler_v2/generators/fir/kernels) , executing in parallel on the 8 cores of the cluster.

## Automatic  code generation

### FIR  with automatic tiling generation

For this FIR example, the low level code splitting the input data in tiles that fit on the L1 memory of the cluster has been generated using the auto-tiling tool available in this SDK.
This process includes the transfer of tiles from the L2 memory in the fabric controler to the L1 in the cluster using DMA Processes.

The generator assumes that the input data are stored on the L2 memory (in the Fabric Controller). The available size of L1 shared memory for processing is specified to the tiler) (here 51200B.
The data are split into tiles, forming  data sets that will be loaded through DMA to the L1 shared memory of the cluster. The cores will process the data and when finished, the results are stored back into the L2.
The tool takes the "high level" parameters of the filters as input and generates the low level functions describing the data management and the sync run time functions to execute the filter in parallel on the eight cores of the cluster, each core processing a chunk of the preloaded tile.

Data transfers and processing are pipelined. Doing so, the DMA processes and the data processing can be executed in parallel on different data sets to optimize the execution time.

The following files are automatically generated:
- FirKernels.c	    : contains the generated code for the filter, with the calls to run time DMA process and processor synchronization
- FirKernel.h	      : contains the header for the filter functions

### How to automatically generate the FIR  code with tiling 

The input file containing the "high level" description of the layers used in the FIR is FirModel.c.

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
LoadFIRLibrary();
~~~~~

The filters are defined by the code below:

~~~~~c

   // 40 Taps, Generic and scalar basic kernel 
    GeneratorFIR("TiledFir40GenericScalar", Samples, 40, 1, 1);
    // 40 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version
    GeneratorFIR("TiledFir40Generic", Samples, 40, 0, 0);

    // 10 Taps, Generic and scalar basic kernel 
    GeneratorFIR("TiledFir10GenericScalar", Samples, 10, 1, 1);
    // 10 Taps, Generic and vectorial basic kernel => 2x16b vectorized version
    GeneratorFIR("TiledFir10Generic", Samples, 10, 1, 0);
    // 10 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version, and using 
     GAP8 buitins for multiply accumulate, and dot product
    GeneratorFIR("TiledFir10Opt", Samples, 10, 0, 0);

    // 20 Taps, Generic and scalar basic kernel
    GeneratorFIR("TiledFir20GenericScalar", Samples, 20, 1, 1);
    // 20 Taps, Generic and vectorial basic kernel=> 2x16b vectorized version
    GeneratorFIR("TiledFir20Generic", Samples, 20, 1, 0);
    // 20 Taps, Specialized and vectorial basic kernel 10 Taps, Specialized and vectorial basic kernel => 2x16b vectorized version, and using GAP8 buitins for multiply accumulate, and dot product
    GeneratorFIR("TiledFir20Opt", Samples, 20, 0, 0);

~~~~~

## Compile and run the application

### Single execution

To generate the kernels and execute the application on the stream contained in fir_in32768.h, just type the following in the Fir directory:


~~~~~sh
	make clean all run
~~~~~

The output looks like the following, giving the total number of cycles to process the full stream (Cycles), the average number of cycles to compute one sample (PerOut). It estimates the distortion withe the refrence (diff) and the ratio diff/total (per thausand).
Ratio is expected to 

cluster master start
Core 8 10 Taps 32768 Samples GenericScalar Cycles   232230 PerOut  7
 total  234270857 diff 57686 ratio 0
Core 8 10 Taps 32768 Samples GenericVect   Cycles   172259 PerOut  5
 total  234270857 diff 57686 ratio 0
Core 8 10 Taps 32768 Samples OptVect       Cycles    82544 PerOut  2
 total  234270857 diff 57686 ratio 0
Core 8 20 Taps 32768 Samples GenericScalar Cycles   398252 PerOut 12
 total  198877915 diff 85510 ratio 0
Core 8 20 Taps 32768 Samples GenericVect   Cycles   269736 PerOut  8
 total  198877915 diff 85510 ratio 0
Core 8 20 Taps 32768 Samples OptVect       Cycles   143390 PerOut  4
 total  198877915 diff 85510 ratio 0
Core 8 40 Taps 32768 Samples GenericScalar Cycles   736098 PerOut 22
 total  179317280 diff 198961 ratio 1
Core 8 40 Taps 32768 Samples GenericVect   Cycles   469491 PerOut 14
 total  179317280 diff 198961 ratio 1
End of application
Test success


## Generate the inputs and golden outputs

   in the ref_np directory, run the python script ref_fir.py to generate a random sequence of length 32768, 3 sets of coefficients (10,20 and 40 taps) and the reference outpus for each coefficient set. By modifying the script it is possible to generate a custom sequence and coefficient set.

