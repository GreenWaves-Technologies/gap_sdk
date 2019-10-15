# Face Detection

## Introduction

The face detection example included in the GAP8 SDK is based on Viola-Jones face detection algorithm.

Algorithms kernels:

1. Image Resize
2. Integral Image
3. Squared Integral Image
4. Cascade Detector

## Algorithm configuration:

The detector is configured to detect faces from 64x48 px size to 40x30 px size. This is achieved using 3 pyramid layers.


## How to Build and Execute the Application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example type:

~~~~~sh
make clean all run
~~~~~

### Change Input-Output Configuration

The example can run with two input-output configurations:

1.  **Input and Output from file on host PC (default configuration)**
2.  **Input from Camera and output to LCD screen**

There are no additional requirements for mode 1, while for mode two the Himax camera must be connected to cpi camera interface and an additional LCD screen must be mounted on the arduino connector. The driver included in this example supports [2.8 TFT LCD Shield based on ili9341 controller](https://www.adafruit.com/products/1947).

To select between confiration 1 and 2 a define must be changed within the file main.c.
To select configuration 1 (deafult) set macro:

~~~~~c
#define FROM_FILE 1
~~~~~
while to select configuration 2:

~~~~~c
#define FROM_FILE 0
~~~~~

After changing configuration the project must be rebuilt and run:

~~~~~sh
make clean all run
~~~~~

### Change Input Image

To change the input image the following two steps are required.

In the main.c file change the name of the input file:

~~~~c
char *Imagefile = "MyNewImage.ppm";
~~~~

So far only images of 324x244 size are supported.
