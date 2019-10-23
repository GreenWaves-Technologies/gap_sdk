# I2S one shot audio stereo 4 microphones record to be plugged with vocal IP

## Introduction
This test does an audio record through the I2S interface. For this, sensor daugther board equiped with microphone has to be plugged to gapuino board.
Voice capture is done using  4 microphones. The application uses a front end IP to perform VAD, beamforming and noise reduction. The IP beamforms the 4 mics signal to compute a single dimension sound signal.

==> PLEASE NOTE that the IP has a time limitation implemented. After 2 minutes, the IP will cease functioning, and the signal acquisition is blocked.

Tke word recognition is done  by computing  time frequency image of the input signal with  MFCC (Mel Frequency Cepstrum COefficient) . Then a 3 layers neural network is applied to recognize the learned words.

The application uses the MBED operating system.

Two applications can be built:
The KWS (KeyWord Spotting), where a list of 10 words can be recignized.
The "alexa" application, where alexa wakeup word is recognized.


The test used mbed API

## What is needed
- gapuino board
- sensor board

## What in this folder:

| Name          |         Description                                              |
|---------------|------------------------------------------------------------------|
|Makefile       |  Makefile of this example                                        |
|main.c     |  Source code of this test                                        |
|README.md      |  This readme file                                                |

##Board configuration

###Gapuino configuration
none

###Sensor board configuration
4 jumpers are needed on the sensor board.
JP1 on 3.3V so pin 2-3
JP6 for CLK
JP12 for D0
JP13 for D1

## How to build and execute the application
## int the gap_sdk installation directory
source  sourceme.sh

To run the example using the board:

~~~~~sh
make clean all run
~~~~~

If it does not compile, you may need to update your gap installation.

If you want to build the KWS or ALEXA  application, just uncomment the relevant MBED_FLAG line in the Makefile.



~~~~~
