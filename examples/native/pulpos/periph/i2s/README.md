# I2S microphone example

## Introduction
This test does an audio record through the I2S interface. For this, sensor daugther board equiped with microphone has to be plugged to gapuino board.
A python script executed in parallel of the c code and both are synchronized to start the record when pressing PC keyboard "return" key. At the end of the record the python script dump GAP memory and launch VLC to hear the record.
The recorded audio has a bandwith of 16kHz, is recorded in mono.

## What is needed
- gapuino board
- sensor board
- VLC installed to listen to the record

## What in this folder:

| Name          |         Description                                              |
|---------------|------------------------------------------------------------------|
|Makefile       |  Makefile of this example                                        |
|test.c         |  Source code of this example                                     |
|test.py        |  A script to synchronize record start and download it            |
|wav_synchro.c  |  Creation of .wav header                                         |
|wav_synchro.h  |  header file                                                     |
|wavOut.wav     |  Symbolic link to the record (only after a 1st execution)        |
|README.md      |  This readme file                                                |

##Board configuration

###Gapuino configuration
There is a specific jumper to add on the gapuino to make available IS1 interface.
JP 16 on pin 1-2.

###Sensor board configuration
4 jumpers are needed on the sensor board.
JP1 on 3.3V so pin 2-3
JP6 for CLK
JP12 for D0
JP13 for D1

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example using the simulator type:

~~~~~sh
make clean all run
~~~~~

## Result

What is displayed describe what is on going on the python script side and on the embedded side.
As informed, you need to press return and almost at the same time having a sound source near the microphone for the record.
vlc is launched to play the record by the python

~~~~~c
Executing script: i2s@i2s.py
py: wait c code to start
c: Ready to record audio from microphones
py: Press return to start sound acquisition:
py: waiting recReady
py: waiting recReady
py: waiting recReady
py: now download 65580 bytes
~~~~~

