# mbed-examples:
 - test_os              : ARM Mbed OS C APIs examples
 - test_driver_gap8     : ARM Mbed OS C Driver examples for GAP8
 - test_driver_gap9     : ARM Mbed OS C Driver examples for GAP9
 - test_os_c++          : ARM Mbed OS C++ RTOS examples
 - test_driver_c++      : ARM Mbed OS C++ Driver examples
 - test_event           : ARM Mbed OS Event Queue C++ examples
 - test_features        : ARM Mbed OS Features C and C++ examples
 - test_application     : ARM Mbed OS Apllications examples
 - test_autotiler       : ARM Mbed OS Autotiler examples, need install AutoTiler library firstly

# How to run

## RTL simulation Terminal
chip=GAP8/GAP9 platform=rtl make clean all run

## RTL simulation with GUI
chip=GAP8/GAP9 platform=rtl make clean all gui

## RTL simulation Terminal (No Printf)
chip=GAP8/GAP9 platform=rtl io=disable make clean all run

## RTL simulation with GUI (No Printf)
chip=GAP8/GAP9 platform=rtl io=disable make clean all gui

## RTL simulation Terminal (Uart Printf)
chip=GAP8/GAP9 platform=rtl io=uart make clean all run

## RTL simulation with GUI (Uart Printf)
chip=GAP8/GAP9 platform=rtl io=uart make clean all gui

## RTL simulation with XCELIUM
chip=GAP8/GAP9 platform=rtl sim=xcelium make clean all gui

## FPGA simulation
chip=GAP8/GAP9 platform=fpga make clean all run


chip     : GAP8 / GAP9          (according to sourceme.sh)
platform : rtl / fpga / gapuino (defult if not set is gapuino)
io       : disable / uart / rtl (defult if not set is rtl)
sim      : xcelium / vsim       (defult if not set is vsim)

# Configuration in Makefile
## Choose Boot Mode
### Boot from Jtag
Add load=-l JTAG_DEV into Makefile, then use above command to run test

### Boot from QSPI FLASH
vsimBootTypeMode=TB_BOOT_FROM_SPI_FLASH

### Boot from HYPER FLASH (default)
vsimBootTypeMode=TB_BOOT_FROM_HYPER_FLASH

## Add RTL simulation trace file
### Questasim
recordWlf=YES
vsimDo="-do /absolute/path/of/your/file.do"

### XCELIUM
recordWlf=YES
vsimDo="/absolute/path/of/your/file.svcf"

## Add RTL simulation special control for padmux
### For GAP8
vsimPadMuxMode=TB_PADMUX_ALT3_HYPERBUS / TB_PADMUX_SPIM_SPIS_LOOPBACK / TB_PADMUX_GPIO_LOOPBACK
### For GAP9
vsimPadMuxMode=TB_PADMUX_CSI2 / TB_PADMUX_UART2 / TB_PADMUX_OCTO_PSRAM / TB_PADMUX_OCTO_SPI_FLASH / TB_PADMUX_OCTO_SPI / TB_PADMUX_OCTO_TWO_HYPERFLASH / TB_PADMUX_SDIO / TB_PADMUX_GPIO_LOOPBACK
