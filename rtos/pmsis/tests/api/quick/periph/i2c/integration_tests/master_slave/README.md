# GAP9 I2C Stress Test

## Introduction

The goal of this test is to use 2 I2C interfaces: one as a master, the other as a slave.

The master writes data in the slave, and try to read it back.
If read data and written data are different, then the test fails.

It repeats the operation for a certain number of iterations.

## Setup

- GAP9 FPGA with GAP9 FMC (mezzanine board)
  - First 2 I2Cs are on J3 and J2

Connect corresponding pins of GAP9 I2C interfaces and probe together.
You may need a breadboard to do so.

## Running the test

Launch the GAP9 test with `make clean all run platform=fpga PMSIS_OS=freertos`
