# I2C sensorboard example

## Introduction
This test read/write registers of each following sensor of the multisensors board.

Here is the list of sensors:
BMP280 DIGITAL PRESSURE SENSOR
HTS221 Capacitive digital sensor for relative humidity and temperature
LIS3MDL Digital output magnetic sensor
LSM6DSL 3D accelerometer and 3D gyroscope
VL53L0X Time-of-Flight ranging and gesture detection sensor
VEML6030 Ambient Light Sensor

## What is needed
- gapuino board
- sensor board

## What in this folder:

| Name          |         Description                                              |
|---------------|------------------------------------------------------------------|
|Makefile       |  Makefile of this example                                        |
|test.c         |  Source code of this example                                     |
|README.md      |  This readme file                                                |

##Board configuration

###Gapuino configuration
Nothing specific.

###Sensor board configuration
1 jumper is needed on the sensor board.
JP1 on 3.3V so pin 2-3

## How to build and execute the application

Please configure your shell for the SDK by running the `GAP` command as indicated in the GAP8 SDK Getting Started Guide.

To run the example using the simulator type:

~~~~~sh
make clean all run
~~~~~

## Result

~~~~~c
*** Test BMP280 ***
BMP280 chip Id and Read/Write in control register ok
	chipID=0x58 expected 0x58
	Written in register 0x55 read 0x55
*** Test HTS221 ***
HTS221 chip Id and Read/Write in control register ok
	chipID=0xbc expected 0xbc
	Written in register 0x87 read 0x87
*** Test LIS3MDL ***
LIS3MDL chip Id and Read/Write in control register ok
	chipID=0x3d expected 0x3d
	Written in register 0x87 read 0x87
*** Test LSM6DSL ***
LSM6DSL chip Id and Read/Write in control register ok
	chipID=0x6a expected 0x6a
	Written in register 0xfe read 0xfe
*** Test VL53L0X ***
VL53L0X reference register read ok
	0xee vs 0xee
	0xaa vs 0xaa
	0x10 vs 0x10
	0x99 vs 0x99
	 0x0 vs 0x0
*** Test VEML6030 ***
read1 0x3 0xd
read 0x3 0x9
VEML6030 test ok:
	Written in register 0x309 read 0x309
///
Test Successful, all sensors I2C link ok
///
Detected end of application, exiting with status: 0
~~~~~

