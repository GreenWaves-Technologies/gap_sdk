[![GreenWaves Technologies][GWT-logo]][GWT-link]

[GWT-logo]: logo.png
[GWT-link]: https://greenwaves-technologies.com/

# GAP SDK

## About

The GAP SDK allows you to compile and execute applications on the GAP IoT
Application Processor.
This SDK provides a development environment for the GAP series processors.

We provide you with a set of tools and two different operating systems for GAP:

* [Tools](https://greenwaves-technologies.com/tools-and-software/)
  * GAP RISCV GNU toolchain: a pre-compiled toolchain inherited from RISC-V
    project with support for our extensions to the RISC-V Instruction Set
    Architecture.
     * Program / control GAP
     * Debug your application using GDB
     * Program the GAPuino flash memory with applications
  * [NNTOOL](https://github.com/GreenWaves-Technologies/gap_sdk/blob/master/tools/nntool/README.md):
    a set of tools based on Python helps to port NN graphs from various NN
    training packages to GAP.
  * [GVSOC](https://greenwaves-technologies.com/gvsoc-the-full-system-simulator-for-profiling-gap-applications/)
    is a lightweight and flexible instruction set simulator which can
    simulate GreenWaves' GAP series processors. GVSOC allows execution of
    programs on a virtual platform without any hardware limits.
    Thanks to device models, full application with real device drivers can be
    simulated. Currently, we provide simulations of devices such as cameras,
    microphones, LCDs, etc.
  * [Profiler](https://greenwaves-technologies.com/profiler/) is a part of
    GAP SDK and is used with GVSOC.
    Profiler gives a visual view of what is happening inside the chip and
    allows to control the simulator through a graphic interface. Profiler is
    an extremely useful tool for developing and debugging applications on GAP
    processors.
  * [Autotiler](https://greenwaves-technologies.com/manuals/BUILD/AUTOTILER/html/index.html):
    a code generator for GAP, which can generate a user algorithm (CNN,
    MatrixAdd, MatrixMult, FFT, MFCC, etc) with optimized memory management.
  * Gapy: a Python utility for building the flashimage, creating
    partitions and filesystems, executing OpenOCD, etc.
* Operating Systems
  * PULP OS - The open source embedded RTOS produced by the PULP project
  * FreeRTOS - FreeRTOS is an open source real-time operating system.
    GreenWaves Technologies has ported it to GAP.
  * PMSIS - PMSIS is an open-source system layer which any operating system
    can implement to provide a common API to applications. We currently provide
    it for PULP OS and FreeRTOS, and it is used by our applications to be
    portable.

## Getting started with the GAP SDK

### Ubuntu 18.04/20.04

#### OS Requirements installation

These instructions were developed using a fresh Ubuntu 18.04 Bionic Beaver
64-Bit virtual machine from
[OS-Boxes](https://www.osboxes.org/ubuntu/#ubuntu-1804-info).

The following packages need to be installed:

```bash
sudo apt-get install -y \
    autoconf \
    automake \
    bison \
    build-essential \
    cmake \
    curl \
    doxygen \
    flex \
    git \
    gtkwave \
    libftdi-dev \
    libftdi1 \
    libjpeg-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsndfile1-dev \
    libtool \
    libusb-1.0-0-dev \
    pkg-config \
    python3-pip \
    rsync \
    scons \
    texinfo \
    wget
```

For Ubuntu 20.04 only:

```bash
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10
```

This will setup a "python" binary pointing at python3.

##### GAP8 OpenOCD
Clone, build and install OpenOCD for GAP8:

```bash
git clone https://github.com/GreenWaves-Technologies/gap8_openocd.git
cd gap8_openocd
./bootstrap
./configure --program-prefix=gap8- --prefix=/usr --datarootdir=/usr/share/gap8-openocd
make -j
sudo make -j install

#Finally, copy openocd udev rules and reload udev rules
sudo cp /usr/share/gap8-openocd/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Now, add your user to dialout group.

```bash
sudo usermod -a -G dialout <username>
# This will require a logout / login to take effect
```

Finally, logout of your session and log back in.

If you are using a Virtual Machine make sure that you give control of the FTDI
device to your virtual machine. Plug the GAPuino into your USB port and then
allow the virtual machine to access it.
For example, for VirtualBox go to Devices->USB and select the device.

Please also make sure that your Virtual Machine USB emulation matches your PC
USB version. A mismatch causes the USB interface to be very slow.

The following instructions assume that you install the GAP SDK into your home
directory. If you want to put it somewhere else then please modify them
accordingly.

## Download and install the toolchain

Now clone the GAP SDK and the GAP/RISC-V toolchain:

```bash
git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
```

Install the toolchain (this may require to launch the script through sudo):

```bash
cd gap_riscv_toolchain_ubuntu_18
./install.sh
```

Finally, clone the sdk (adapt gap_sdk path according to your needs)

```bash
git clone https://github.com/GreenWaves-Technologies/gap_sdk.git
cd gap_sdk
```

## Configure the SDK

You can either source sourceme.sh in the root sdk folder and then select the
right board from the list, or directly source the board config.

```bash
source sourceme.sh
```

or

```bash
# replace gapuino_v3.sh by the board you want
source config/gapuino_v3.sh
```

If you directly source the board config, you need to source the appropriate
config file for the board that you have. The SDK supports 3 boards (gapuino,
gapoc_a and gapoc_b) and each of them can use version 1/2/3 of the GAP8 chip.
Boards bought before 10/2019 contains GAP8 version 1 and use a USB B plug for
JTAG while the ones bought after contains version 2/3 and use a USB micro B
for JTAG.

Once the proper config file is sourced, you can proceed with the SDK build.

Note that after the SDK has been built, you can source another board config
file to change the board configuration, in case you want to use a different
board. In this case the SDK will have to be built again. As soon as the SDK
has been built once for a board configuration, it does not need to be built
again for this configuration, unless the SDK is cleaned.

## Minimal install (FreeRTOS only, no neural network tools)

We will first make a minimal install to check whether previous steps were
successful.
If you are only doing board bringup or peripheral testing, this install will
also be a sufficient.

### Python requirements

Our modules (gapy runner) require a few additional Python packages that you
can install with this command from GAP SDK root folder:

```bash
pip3 install -r requirements.txt
pip3 install -r doc/requirements.txt
```

### SDK install

First, use the following command to configure the shell environment correctly
for the GAP SDK.
It must be done for each terminal session:

```bash
cd path/to/gap_sdk
# Choose which board
source sourceme.sh
```

Tip: You can add an "alias" command as follows in your .bashrc file:

```bash
alias GAP_SDK='cd path/to/gap_sdk && source sourceme.sh'
```

Typing `GAP_SDK` will now change to the gap_sdk directory and execute the source
command.

Once in the SDK, run `make help` to get commands and get SDK ready to use.

```
$ make help
=================== GAP SDK ===================

Main targets:
 - clean       : clean the SDK
 - all         : build the whole SDK with all tools
 - minimal     : get latest sources for all rtos and libs
 - gvsoc       : build GVSOC simulation platform
 - openocd.all : build OpenOCD tools to run simulation on boards
```

Then, compile the minimal set of dependencies to run examples on either GVSOC or boards:

```bash
make minimal
# Build GVSOC simulation platform
make gvsoc
# Build openocd tools to flash and run simulation on boards
make openocd.all
```

### Helloworld

Finally try a test project. First connect your GAPuino to your PCs USB port.
Now, you should be able to run your first helloworld on the board.

```bash
cd examples/pmsis/helloworld
make clean all run PMSIS_OS=freertos platform=board
```

In details, `PMSIS_OS`, `platform`, `io` are used to configure the RTOS to run
the example on, specify the runner, and select the output for printf.

* PMSIS_OS : RTOS (freertos/pulpos)
* platform : board gvsoc rtl fpga (defult if not set is gvsoc)
* io       : disable host(semihosting) uart rtl (defult if not set is semihosting)

After the build you should see an output resembling:

```
     *** PMSIS HelloWorld ***

     Entering main controller
     [32 0] Hello World!
     Cluster master core entry
     [0 7] Hello World!
     [0 0] Hello World!
     [0 4] Hello World!
     [0 5] Hello World!
     [0 3] Hello World!
     [0 1] Hello World!
     [0 2] Hello World!
     [0 6] Hello World!
     Cluster master core exit
     Test success !
     Detected end of application, exiting with status: 0
     Loop exited
     commands completed
```

If this fails, ensure that you followed previous steps correctly (openocd
install, udev rules).
If libusb fails with a permission error, you might need to reboot to apply all
changes.

If you need GAP tools for neural networks (nntool) or the Autotiler, please
follow the next section

If you just wish to also have access to pulp-os simply type:

```bash
# compile pulp-os and its librairies
make pulp-os
```

And replace `PMSIS_OS=freertos` by `PMSIS_OS=pulpos` on your run command line.

## Full Install

### Python requirements

In order to use the GAP tools for neural networks (nntool), we strongly
encourage to install the Anaconda distribution (Python3). You can find more
information on [Anaconda website](https://www.anaconda.com/).

Note that this is needed only if you want to use nntool, you can skip this step
otherwise. Once Anaconda is installed, you need to activate it and install
Python modules for this tool with this command:

```bash
pip install -r tools/nntool/requirements.txt
pip install -r requirements.txt
```

### Pull and compile the full tool suite

Finally, we install the full tool suite of the sdk (including nntool and
autotiler).

```bash
make sdk
```

Note that if you only need autotiler (and not nntool) you can instead use:

```bash
make all && make autotiler
```

## OpenOCD

OpenOCD for GAP8 is now used instead of plpbridge. There are a few applications
which require OpenOCD, as they are using OpenOCD semi-hosting to transfer files
with the workstation.

You have to install the system dependencies required by OpenOCD.
You can find them in [OpenOCD README](http://openocd.org/doc-release/README).

There are different cables setup by default for each board. In case you want to
use a different cable, you can define this environment variable:

```bash
export GAPY_OPENOCD_CABLE=interface/ftdi/olimex-arm-usb-ocd-h.cfg
```

## Using the virtual platform

If you only followed
[Minimal installation](#minimal-install-freertos-only-no-neural-network-tools)
process, begin by compiling gvsoc:

```bash
make gvsoc
```

You can also run this example on the GAP virtual platform with this command:

```bash
make clean all run platform=gvsoc PMSIS_OS=freertos/pulpos
```

### Using the virtual platform with profiler

#### Using GAP Profiler

Install the profiler:

```bash
make profiler
```

For further information, you can open the doc : gap_sdk/doc/_build/html/index.html
and find Tools -> Profiler

In the doc, we will show you how to install and use the profiler step by step.

Note:
1. If you are using Anaconda in Ubuntu 20.04, Anaconda has default Qt version: 5.9.7,
   which is not aligned to the default version in Ubuntu 20.04 (5.12.8).
   The solution is to use only the one from system (5.12.8):
   ```bash
   export PATH=/usr/lib/qt5/bin:$PATH
   ```

2. Profiler build process doesn't support GCC-10+ for now.
   The current solution is switch to gcc-9.

#### Using VCD traces with GTKWave

You can also generate VCD traces to see more details about the execution:

```bash
make clean all run platform=gvsoc runner_args=--vcd
```

You should see a message from the platform telling how to open the profiler.

## Using the flasher (Hyperflash)

As soon as at least one file for a file-system is specified, the command
`make all` will also build a flash image containing the file systems and upload
it to the flash.

For example, you can include files for the readfs file-system with these flags
in your Makefile:

```make
READFS_FILES += <file1> <file2> <file3> ......
```

In case you don't have any file but you still want to upload the flash image,
for example for booting from flash, you can execute after you compiled your
application:

```bash
make flash
```

In case you specified files, the command `make all` will not only build the
application but also build the flash image and upload it to the flash.
In case you just want to build your application, you can do:

```bash
make build
```

Then after that if you want to produce the flash image and upload it,
you can do:

```bash
make image flash
```

## Boot from flash

The board is by default configured to boot through JTAG. If you want to boot
from flash, you need to first program a few efuses to tell the ROM to boot from
flash. Be careful that this is a permanent operation, even though it will still
be possible to boot from JTAG. This will just always boot from flash when you
power-up the board or reset it.

To program the efuses, please read the [README](./tools/gap_fuser/README.md) and
use the fuser tool to program your efuse.

## Console IO via uart

If you choose to boot your application from Flash, and/or you want to view the
output of printf's in your code then you can first compile your application
with the printf redirected on the UART with this command:

```bash
make clean all platform=board PMSIS_OS=your_os io=uart
```

You can also use a terminal program, like "cutecom":

```bash
sudo apt-get install -y cutecom
cutecom&
```

Then please configure your terminal program to use /dev/ttyUSB1 with a 115200
baud rate, 8 data bits and 1 stop bit.

## Upgrading/Downgrading the SDK

If you want to upgrade/downgrade your SDK to a new/old version:

```bash
cd gap_sdk
git checkout master && git pull
git checkout <release tag name>
# For minimal install
make clean minimal_sdk
# for full install
make clean sdk
```

You can find a list of [releases](https://github.com/GreenWaves-Technologies/gap_sdk/releases)
on Github.

## Getting help

Please [log any issue](https://github.com/GreenWaves-Technologies/gap_sdk/issues)
you have with the SDK in the Github project.
Include all the information you can to help us reproduce your issue, including
SDK version, logs, steps to reproduce and board.
