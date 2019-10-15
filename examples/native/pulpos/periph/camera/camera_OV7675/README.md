# Example Camera OmniVision 7675

## Introduction

This test shows you how to use the OmniVision 7675 camera to capture an image, then download the image captured by using a script of python. The file test.py is executed from the runtime to transfer an image from the the GAPUINO OmniVision 7675 camera to the host pc.

## What in this folder:

| Name          |         Description                                              |
|---------------|------------------------------------------------------------------|
|Makefile       |  Makefile of this example                                        |
|test.c         |  Source code of this example                                     |
|test.py        |  A script allows the user download the image from GAPUINO BOARD  |
|imgOut0.pgm    |  The Output image taken from Omnivision camera  (normal version) |
|README.md      |  This readme file                                                |

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example using the simulator type:

~~~~~sh
make clean all run
~~~~~


## Result

The result shows you the result of check.

An examle of the output is:

~~~~~c
Starting execution
Executing script: camera@test.py
Entering main controller
ready to take a photo
Py: Press Enter to acquire an image:
taking picture
Picture taken!
Py: Waiting image to be written
Py: Saving image to file ... (  79071  bytes)
ready to take a photo
Py: Press Enter to acquire an image:
Py: Saving image to file ... (  24015  bytes)
taking picture
~~~~~
