[![GreenWaves Technologies][GWT-logo]][GWT-link]

[GWT-logo]: logo.png
[GWT-link]: https://greenwaves-technologies.com/

# GAP SDK

## About

The GAP SDK allows you to compile and execute applications on the GAP Series Processors.
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
  * [Autotiler](https://greenwaves-technologies.com/manuals/BUILD/AUTOTILER/html/index.html):
    a code generator for GAP, which can generate a user algorithm (CNN,
    MatrixAdd, MatrixMult, FFT, MFCC, etc) with optimized memory management.
  * Gapy: a Python utility for building the flashimage, creating
    partitions and filesystems, executing OpenOCD, etc.
  * [GVSOC](https://greenwaves-technologies.com/gvsoc-the-full-system-simulator-for-profiling-gap-applications/)
    is a lightweight and flexible instruction set simulator which can
    simulate GreenWaves' GAP series processors. GVSOC allows execution of
    programs on a virtual platform without any hardware limits.
    Thanks to device models, full application with real device drivers can be
    simulated. Currently, we provide simulations of devices such as cameras,
    microphones, LCDs, etc.
  * Profiler: Profiler gives a visual view of what is happening inside the chip and allows to control the simulator through a graphic interface. 


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

These instructions were developed using a fresh Ubuntu 20.04 (Focal Fossa)
64-Bit virtual machine from
[OS-Boxes](https://www.osboxes.org/ubuntu/#ubuntu-2004-info).

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
    graphicsmagick-libmagick-dev-compat \
    libtool \
    libusb-1.0-0-dev \
    pkg-config \
    python3-pip \
    rsync \
    scons \
    texinfo \
    wget
```

#### Python Package Management

SDK and some tools are all based on Python3 (version > 3.8), you can use following command to set your default python to python3.  

```bash
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10
```

This will setup a "python" binary pointing at python3.

We strongly recommend to use [Anaconda3](https://www.anaconda.com/) to manage python packages

## Download and install the toolchain

Now clone the GAP/RISC-V toolchain:

```bash
git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu.git
```

Install the toolchain (this may require to launch the script through sudo):

```bash
cd gap_riscv_toolchain_ubuntu
./install.sh
```

Clone the actual gap_sdk repository

- gitlab: you can find the url on the top, blue button which written "Clone"
- github: you can find the url on the top, green button which written "Code"

Then run:

```bash
git clone <the url of this repository>
```

If you are using ssh to clone, don't forget to put your ssh-key in your account. 

## Configure the SDK

You can either source sourceme.sh in the root sdk folder and then select the
right board from the list, or directly source the board config.

```bash
source sourceme.sh
```

or

```bash
source config/<the target you want to use>.sh
```

For GAP8, if you directly source the board config, you need to source the appropriate
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

### Python requirements

Our modules (gapy runner) require a few additional Python packages that you
can install with this command from GAP SDK root folder:

```bash
pip3 install -r requirements.txt
pip3 install -r doc/requirements.txt
```

### SDK installation

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
 - nntool      : build nntool
```

Then, depends on what you need, build the SDK accordingly

```bash
make <target>
```
#### Install OpenOCD Rules 

* Copy openocd udev rules and reload udev rules

```bash
sudo cp <your openocd path>/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload-rules && sudo udevadm trigger
```

* Now, add your user to dialout group.

```bash
sudo usermod -a -G dialout <username>
```

This will require a logout / login to take effect

### Build SDK Doc

SDK Doc is build and generated based on SPHINX [https://www.sphinx-doc.org/en/master/], the SDK will have installed all the necessary packages for you.  
You just need to run:

```bash
cd doc
make html
```

This will generate the doc in HTML in 
```
doc/_build/html/  
```
and open the file index.html with your browser

**Within this doc, you can find all the api descriptions, how to use our tools (like nntool, gvsoc, profiler, etc), and some useful application notes.**

### Helloworld

Finally try a test project. First connect your GAPuino to your PCs USB port.
Now, you should be able to run your first helloworld on the board.

```bash
cd examples/<target name>/basic/helloworld
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

You can find a list of releases in this repository.

## Getting help

Please [log any issue](https://github.com/GreenWaves-Technologies/gap_sdk/issues)
you have with the SDK in the Github project.
Include all the information you can to help us reproduce your issue, including
SDK version, logs, steps to reproduce and board.
