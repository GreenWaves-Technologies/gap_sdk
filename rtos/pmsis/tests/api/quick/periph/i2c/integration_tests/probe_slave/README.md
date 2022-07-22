# I2C Probe Slave test

## Introduction

The goal of this test is to verify that the I2C slave works with an external probe.

The probe writes and reads data to/from the GAP9 slave and fails if there is any error.
It repeats this operations many times.

## Setup

- GAP9 FPGA with GAP9 FMC (mezzanine board)
- Aardvark probe

Connect GAP9 FMC J3 I2C pins (SDA, SCL, GND) to the probe corresponding pins.

## Running the test

Using two terminals:

1. launch GAP9 slave with `make clean all run platform=fpga PMSIS_OS=freertos`
2. launch the master probe with `./probe/BUILD/aai2c_master 0 400 0xA`
