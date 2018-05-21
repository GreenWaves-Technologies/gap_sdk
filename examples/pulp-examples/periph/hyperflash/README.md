 Hyperflash read example

## Introduction

## What is needed
- gapuino board and hyperflash previously flashed

## What in this folder:

| Name          |         Description                                              |
|---------------|------------------------------------------------------------------|
|Makefile       |  Makefile of this example                                        |
|test.c         |  Source code of this example                                     |
|               |             |
|  |                                         |
|  |  header file                                                     |
|     |          |
|README.md      |  This readme file                                                |

##Board configuration

###Gapuino configuration
There is 1 specific jumper to add on the gapuino:
JP 14 to drive hyperram chip select

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example using the simulator type:

~~~~~sh
make clean all run
~~~~~

## Result

~~~~~c
Entering main controller
pad config done
End of RX for id 0
Detected end of application, exiting with status: 0
~~~~~

