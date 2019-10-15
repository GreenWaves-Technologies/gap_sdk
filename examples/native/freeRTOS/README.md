# FreeRTOS

## FreeRTOS examples :

- test_os : FreeRTOS API examples
- periph : Periph examples

To test on gapuino :
```bash
user@computer:gap_sdk/example/native/freeRTOS$ cd PATH/TO/TEST
user@computer:gap_sdk/examples/freeRTOS-examples/PATH/TO/TEST$ make [clean] all run
```
For Autotiler tests :
```bash
user@computer:gap_sdk/examples/freeRTOS-examples/test_autotiler/Cifar10$ make clean all run
```

Jenkins tests :
```bash
user@computer:gap_sdk/examples/freeRTOS-examples$ make clean gapuino_compile_and_run_test
```

## API Reference
https://www.freertos.org/a00106.html

# How to run

## RTL simulation Terminal
chip=GAP9 platform=rtl make clean all run

## RTL simulation with GUI
chip=GAP9 platform=rtl make clean all gui

## RTL simulation Terminal (No Printf)
chip=GAP9 platform=rtl io=disable make clean all run

## RTL simulation with GUI (No Printf)
chip=GAP9 platform=rtl io=disable make clean all gui

## RTL simulation Terminal (Uart Printf)
chip=GAP9 platform=rtl io=uart make clean all run

## RTL simulation with GUI (Uart Printf)
chip=GAP9 platform=rtl io=uart make clean all gui

## RTL simulation with XCELIUM
chip=GAP9 platform=rtl sim=xcelium make clean all gui

## FPGA simulation
chip=GAP9 fpga=1 make clean all run

chip     : GAP8 GAP9 (according to sourceme.sh)
platform : rtl fpga gapuino (defult if not set is gapuino)
io       : disable uart rtl (defult if not set is rtl)
sim      : xcelium vsim  (defult if not set is vsim)
