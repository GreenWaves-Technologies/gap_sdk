 Hyperflash write example

## Introduction

## What is needed
- gapuino board

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
test start
pad config done
conf finished.
Erase finished.
128 bytes of data, 1 Page would be loaded, 128 bytes in last page
checkSum: 1FC0
Write finished, 1 pages should be writen, 1 pages has been done, with i = 1
Test SUCCESS with check: 1FC0
Test success
~~~~~

