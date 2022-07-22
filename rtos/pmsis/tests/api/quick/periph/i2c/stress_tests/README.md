# GAP9 I2C Stress Test

## Introduction

The goal of this test is to stress I2C interfaces.

GAP9 has 3 available I2C interfaces on FPGA.
Test sets up:

- 3 I2C Slaves (a virtual eeprom on each interface)
- 2 I2C Masters

An external Aardvark probe can be used to stress the system even more.

## Setup

- GAP9 FPGA with GAP9 FMC (mezzanine board)
  - First 2 I2Cs are on J3 and J2
  - The third I2C interface is not mapped on the GAP9 FMC,
    but on ETH FMC. Test might hang if you don't have this FMC.
- Aardvark probe

Connect corresponding pins of GAP9 I2C interfaces and probe together.
You may need a breadboard to do so.

## Running the test

Using two terminals:

1. launch the Aardvark aai2c_master with
   `./probe/BUILD/aai2c_master 0 400 write <addr> 0 1` where `<addr>` is the
   address of the third I2C interface slave.
2. launch the GAP9 test with `make clean all run platform=fpga PMSIS_OS=freertos`
