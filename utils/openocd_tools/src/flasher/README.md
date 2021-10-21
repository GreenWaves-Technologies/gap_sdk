# GAP Flasher Filmware

## Introduction

This flasher will be load and executed on GAP, it will synchronize with openOCD to:

* Read flashimage/files from your host section by section (256kB for Hyper, 64kB for SPI)
* Write each section to your HyperFlash or SPI Flash

## Build:

### Hyper version

~~~~~shell
make clean all 
~~~~~

### SPI version

~~~~~shell
make clean all spiflash=1 
~~~~~
