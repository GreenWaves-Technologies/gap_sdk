# Application Canny Edge Detection

## Introduction

Canny Edge Detection is a popular edge detection algorithm. It is a technique to extract useful structural information from different vision objects and dramatically reduce the amount of data to be processed. It has been widely applied in various computer vision systems.

With this application example, we show how this algorithm can be implemented on GAP8. We compare the performance of the algorithm when run on 1, 2, 4 and 8 cores with the same image. Of course, you can deactivate this test mode, and run the application only on 8 cores by changing the value of variable "CoreCountDynamic" to 0.

Thanks to GAP8's extended instruction set, we can finish a MAC operation (Multiply-Accumulate) of 2 four-vectors in only one cycle. To achieve maximum performance we use this by vectorizing the main operations of algorithm.

For instance, in the first step of the algorithm, which applies a Gaussian Filter (5 * 5) to smooth the image we have used seven 4 element vectors to represent the 5 * 5 filter. Please read the comments in the source code for further information.

## What in this folder:

| Name          |         Description             |
|---------------|---------------------------------|
|CannyDetect.c  |  Source code of the application |
|Makefile       |  Makefile of this application   |
|Mills.h        |  A pre-defined image buffer      |
|README.md      |  This readme file               |

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example using the simulator type:

~~~~~sh
make clean all run
~~~~~

## Result

The result shows you the number of cycles used in each part of the algorithm when running 1, 2, 4 and 8 cores.

An example of the output is:

~~~~~sh
Start of application
cluster master start
Canny Edge Detector running on 1 cores, Source Mono image[W=360, H=202]
Entering Master, W=360, H=202, 15 iterations, read stripes: 21 lines, wrote stripe: 13 lines
Conv5x5_Byte_Vectorial                :    1984659 Cycles
EdgeIntensityAndOrientation_Vectorial :    3280079 Cycles
CannyRemoveNonMax                     :    2723484 Cycles
CannyBlobAnalysis                     :    1293926 Cycles
CannyRemoveWeakEdges                  :     116464 Cycles
Total                                 :    9398612 Cycles
Total with Master                     :    9403548 Cycles
Canny Edge Detector running on 2 cores, Source Mono image[W=360, H=202]
Entering Master, W=360, H=202, 15 iterations, read stripes: 21 lines, wrote stripe: 13 lines
Conv5x5_Byte_Vectorial                :     994220 Cycles
EdgeIntensityAndOrientation_Vectorial :    1643662 Cycles
CannyRemoveNonMax                     :    1365630 Cycles
CannyBlobAnalysis                     :     656683 Cycles
CannyRemoveWeakEdges                  :      58784 Cycles
Total                                 :    4718979 Cycles
Total with Master                     :    4723521 Cycles
Canny Edge Detector running on 4 cores, Source Mono image[W=360, H=202]
Entering Master, W=360, H=202, 15 iterations, read stripes: 21 lines, wrote stripe: 13 lines
Conv5x5_Byte_Vectorial                :     500909 Cycles
EdgeIntensityAndOrientation_Vectorial :     829353 Cycles
CannyRemoveNonMax                     :     689137 Cycles
CannyBlobAnalysis                     :     338531 Cycles
CannyRemoveWeakEdges                  :      29796 Cycles
Total                                 :    2387726 Cycles
Total with Master                     :    2392387 Cycles
Canny Edge Detector running on 8 cores, Source Mono image[W=360, H=202]
Entering Master, W=360, H=202, 15 iterations, read stripes: 21 lines, wrote stripe: 13 lines
Conv5x5_Byte_Vectorial                :     256015 Cycles
EdgeIntensityAndOrientation_Vectorial :     417930 Cycles
CannyRemoveNonMax                     :     348603 Cycles
CannyBlobAnalysis                     :     175752 Cycles
CannyRemoveWeakEdges                  :      15290 Cycles
Total                                 :    1213590 Cycles
Total with Master                     :    1218337 Cycles
~~~~~

With a core frequency of 100MHz this would execute in ~12ms if all 8 cores are used.
