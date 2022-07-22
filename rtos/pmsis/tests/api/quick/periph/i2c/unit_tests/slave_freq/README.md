# GAP9 I2C Slave frequency test

## Introduction

The goal of this test is to verify that I2C works with 100kHz and 400 kHz frequencies.

The probe reads data from the GAP9 slave and reports the number of I2C errors.

## Setup

- GAP9 FPGA with GAP9 FMC (mezzanine board)
- Aardvark probe

Connect GAP9 FMC J3 I2C pins (SDA, SCL, GND) to the probe corresponding pins.

## Running the test

Using two terminals:

1. launch GAP9 slave with `make clean all run platform=fpga PMSIS_OS=freertos`
2. launch the master probe with `./probe/BUILD/aai2c_master 0 20 write 0xA 0 1`
