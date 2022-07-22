# GAP9 I2C Master frequency test

## Introduction

The goal of this test is to verify that I2C works with 100kHz and 400 kHz frequencies.

GAP9 master writes to the probe and read data from it. It verifies that data read
is what was expected.

## Setup

- GAP9 FPGA with GAP9 FMC (mezzanine board)
- Aardvark probe

Connect GAP9 FMC J3 I2C pins (SDA, SCL, GND) to the probe corresponding pins.

## Running the test

Using two terminals:

1. launch the aai2c_eeprom with `./probe/BUILD/aai2c_eepromslave 0 20 0`
2. launch the GAP9 test with `make clean all run platform=fpga PMSIS_OS=freertos`
