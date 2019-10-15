# Canny Edge Detection

## Introduction

Canny Edge Detection is a popular edge detection algorithm. It is a technique to extract useful structural information from different vision objects and dramatically reduce the amount of data to be processed. It has been widely applied in various computer vision systems.

With this application example, we show how this algorithm can be implemented on GAP8.

Thanks to GAP8's extended instruction set, we can finish a MAC operation (Multiply-Accumulate) of 2 four-vectors in only one cycle. To achieve maximum performance we use this by vectorizing the main operations of algorithm.

For instance, in the first step of the algorithm, which applies a Gaussian Filter (5 * 5) to smooth the image we have used seven 4 element vectors to represent the 5 * 5 filter. Please read the comments in the source code for further information.

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example type:

~~~~~sh
make clean all run
~~~~~

## Result

The result shows you the number of cycles used in each part of the algorithm when running the 8 cluster cores. The example operate on Pedestrian.pgm image and produces img_OUT.ppm which contains the image after canny edge elaboration.

An example of the output is:

~~~~~sh
Start execution on GAP8
Start of application
bridge connecting
Tcp_listener: client connected (fd 12)
client connected
Bridge connected, waiting for load image
Image ../../../Pedestrian.pgm, [W: 320, H: 240], Gray, Size: 76800 bytes, Loaded sucessfully
cluster master start
Canny Edge Detector running on 8 cores, Source Mono image[W=320, H=240]
Entering Master, W=320, H=240, 17 iterations, read stripes: 21 lines, wrote stripe: 13 lines
Conv5x5_Byte_Vectorial                :     473669 Cycles
EdgeIntensityAndOrientation_Vectorial :     738829 Cycles
CannyRemoveNonMax                     :     530711 Cycles
CannyBlobAnalysis                     :     380492 Cycles
CannyRemoveWeakEdges                  :      27190 Cycles
Total                                 :    2150891 Cycles
Total with Master                     :    1197378 Cycles
imgName: ../../../img_OUT.ppm
Writing image  [#                             ]
Writing image  [####                          ]
Writing image  [#######                       ]
Writing image  [###########                   ]
Writing image  [##############                ]
Writing image  [#################             ]
Writing image  [#####################         ]
Writing image  [########################      ]
Writing image  [###########################   ]
Detected end of application, exiting with status: 0
Loop exited
commands completed
~~~~~

With a core frequency of 100MHz this would execute in \~12ms if all 8 cores are used.
