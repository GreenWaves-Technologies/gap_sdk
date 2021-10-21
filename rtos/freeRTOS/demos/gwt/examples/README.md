# FreeRTOS

## FreeRTOS examples :

- kernel : FreeRTOS API examples
- periph : Periph examples

To compile and run on board or GVSOC :
```bash
user@computer:gap_sdk/rtos/freeRTOS$ cd demos/gwt/examples/hello_world
user@computer:gap_sdk/rtos/freeRTOS/demos/gwt/examples/hello_world$ make [clean] all run platform=[board,gvsoc]
```

## API Reference
https://www.freertos.org/a00106.html

# How to run
```bash
## GVSOC simulation
[chip=GAP8] PMSIS_OS=freertos platform=gvsoc make clean all run

## Board simulation
[chip=GAP8] PMSIS_OS=freertos platform=board io=host make clean all run

## Board simulation (Uart Printf)
[chip=GAP8] PMSIS_OS=freertos platform=board io=uart make clean all run

## GVSOC simulation
[chip=GAP9] PMSIS_OS=freertos platform=gvsoc make clean all run

## RTL simulation Terminal with XCELIUM
[chip=GAP9] PMSIS_OS=freertos platform=rtl sim=xcelium make clean all run

## RTL simulation with GUI, XCELIUM
[chip=GAP9] PMSIS_OS=freertos platform=rtl sim=xcelium make clean all GUI=1

## FPGA simulation
[chip=GAP9] PMSIS_OS=freertos platform=fpga make clean all run

chip     : GAP8 GAP9 (according to sourceme.sh)
platform : board gvsoc rtl fpga (defult if not set is gvsoc)
io       : disable host(semihosting) uart rtl (defult if not set is semihosting)
sim      : xcelium vsim  (defult if not set is vsim)
