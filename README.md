[![GreenWaves Technologies][GWT-logo]][GWT-link]

[GWT-logo]: logo.png
[GWT-link]: https://greenwaves-technologies.com/

# Setting up the GAP8 SDK

## About

The GAP8 SDK allows you to compile and execute applications on the GAP8 IoT Application Processor. This SDK is an extract of the necessary elements from the pulp-sdk (https://github.com/pulp-platform/pulp-sdk) produced by the PULP project, to provide a development environment for the GAP8 series processors.

We provide you with all the necessary tools and two different operating systems for GAP8:

*   Tools
    -   GAP8 RISCV GNU toolchain: a pre-compiled toolchain inherited from RISC V project with support for our extensions to the RISC-V Instruction Set Architecture.
        * Program / control GAP8
        * Debug your application using GDB
        * Program the GAPuino flash memory with applications

*   Operating Systems
    -   PULP OS - The open source embedded RTOS produced by the PULP project
    -   FreeRTOS - FreeRTOS is an open source real time operating system. GreenWaves Technologies has ported it to GAP8
    -   PMSIS - PMSIS is an open-source system layer which any operating system can implement to provide a common API to applications. We currently provide it for PULP OS and FreeRTOS, and it is used by our applications to be portable.

##  Getting started with the GAP SDK

### Ubuntu 18.04

These instructions were developed using a fresh Ubuntu 18.04 Bionic Beaver 64-Bit virtual machine from https://www.osboxes.org/ubuntu/#ubuntu-1804-info

The following packages needed to be installed:

~~~~~shell
sudo apt-get install -y build-essential git libftdi-dev libftdi1 doxygen python3-pip libsdl2-dev curl cmake libusb-1.0-0-dev scons gtkwave libsndfile1-dev rsync autoconf automake texinfo libtool pkg-config libsdl2-ttf-dev
~~~~~

An openocd build for gap8 should be cloned and installed:

~~~~~shell
git clone https://github.com/GreenWaves-Technologies/gap8_openocd.git
cd gap8_openocd
./bootstrap
./configure --program-prefix=gap8- --prefix=/usr --datarootdir=/usr/share/gap8-openocd
make -j
sudo make -j install

#Finally, copy openocd udev rules and reload udev rules
sudo cp /usr/share/gap8-openocd/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload-rules && sudo udevadm trigger
~~~~~

Now, add your user to dialout group.
~~~~~shell
sudo usermod -a -G dialout <username>
# This will require a logout / login to take effect
~~~~~

Finally, logout of your session and log back in.

If you are using a Virtual Machine make sure that you give control of the FTDI device to your virtual machine. Plug the GAPuino into your USB port and then allow the virtual machine to access it. For example, for VirtualBox go to Devices->USB and select the device.

Please also make sure that your Virtual Machine USB emulation matches your PC USB version. A mismatch causes the USB interface to be very slow.

The following instructions assume that you install the GAP SDK into your home directory. If you want to put it somewhere else then please modify them accordingly.

### Ubuntu 16.04

You can follow the steps for Ubuntu 18.04 except for the following instructions.

After you have installed the system packages with apt-get,  you need to also create this symbolic link:

~~~~~shell
sudo ln -s /usr/bin/libftdi-config /usr/bin/libftdi1-config
~~~~~

Also, you may need to install git lfs

~~~~~shell
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt-get install git-lfs
git lfs install
~~~~~

## Download and install the toolchain:

Now clone the GAP8 SDK and the GAP8/RISC-V toolchain:

~~~~~shell
git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
~~~~~

In case you use an old git version, you may need to use these commands instead:

~~~~~shell
git lfs clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
~~~~~

Install the toolchain (this may require to launch the script through sudo):

~~~~~shell
cd ~/gap_riscv_toolchain_ubuntu_18
./install.sh
~~~~~

Finally, clone the sdk (adapt gap_sdk path according to your needs)
~~~~~shell
git clone https://github.com/GreenWaves-Technologies/gap_sdk.git
cd ~/gap_sdk
~~~~~

## Configure the SDK:

You can either source sourceme.sh in the root sdk folder and then select the right board from the list, or directly source the board config.

~~~~~shell
source sourceme.sh
~~~~~

or

~~~~~shell
# replace gapuino_v2.sh by the board you want
source config/gapuino_v2.sh
~~~~~

If you directly source the board config, you need to source the appropriate config file for the board that you have. The SDK supports 2 boards (gapuino and gapoc) and each of them can use version 1 or version 2 of the GAP8 chip. Boards bought before 10/2019 contains GAP8 version 1 and use a USB B plug for JTAG while the ones bought after contains version 2 and use a USB micro B for JTAG.

Hereafter you can find a summary of the available boards and their configuration file.

Board     | Chip       | Config file
|:-------:|:----------:|:----------------------:|
Gapuino   | GAP8 v1    | configs/gapuino.sh
Gapuino   | GAP8 v2    | configs/gapuino_v2.sh
Gapoc     | GAP8 v1    | configs/gapoc_a.sh
Gapoc     | GAP8 v2    | configs/gapoc_a_v2.sh

Once the proper config file is sourced, you can proceed with the SDK build.

Note that after the SDK has been built, you can source another board config file to change the board configuration, in case you want to use a different board. In this case the SDK will have to be built again. As soon as the SDK has been built once for a board configuration, it does not need to be built again for this configuration, unless the SDK is cleaned.

## Minimal install (FreeRTOS only, no neural network tools)

We will first make a minimal install to check whether previous steps were successful.
If you are only doing board bringup or peripheral testing, this install will also be a sufficient.

### Python requirements
Our modules (gapy runner) require a few additional Python packages that you can install with this command from Gap SDK root folder:

~~~~~shell
pip3 install -r requirements.txt
~~~~~

### SDK install

Initialize and download all sub projects required to run pmsis_examples on a board (freertos, pmsis_api, gapy and bsp):

First, use the following command to configure the shell environment correctly for the GAP SDK.
It must be done for each terminal session**:

~~~~~shell
cd ~/gap_sdk
# Choose which board
source sourceme.sh
~~~~~

Tip: You can add an "alias" command as follows in your .bashrc file:
~~~~~shell
alias GAP_SDK='cd ~/gap_sdk && source sourceme.sh'
~~~~~

Typing GAP_SDK will now change to the gap_sdk directory and execute the source command.

Then, compile the minimal set of dependencies to run examples:

~~~~~shell
make minimal_sdk
~~~~~

### Helloworld


Finally try a test project. First connect your GAPuino to your PCs USB port.
Now, you should be able to run your first helloworld on the board.

~~~~~shell
cd examples/pmsis/helloworld
make clean && make PMSIS_OS=freertos platform=board io=host all -j && make platform=board io=host run
~~~~~

In details: PMSIS_OS allows us to choose an OS (freertos/pulpos), platform allows to choose the runner
(board/gvsoc) and io choose the default output for printf (host/uart).

After the build you should see an output resembling:
~~~~~
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
~~~~~

If this fails, ensure that you followed previous steps correctly (openocd install, udev rules).
If libusb fails with a permission error, you might need to reboot to apply all changes.

If you need Gap tools for neural networks (nntool) or the Autotiler, please follow the next section

If you just wish to also have access to pulp-os simply type:

~~~~~shell
# checkout all needed submodules
make pulpos.all
# compile pulp-os and its librairies
make pulp-os
~~~~~

And replace PMSIS_OS=freertos by PMSIS_OS=pulpos on your run command line.

## Full Install

### Python requirements

In order to use the Gap tools for neural networks (nntool), we strongly encourage to install the Anaconda distribution ( Python3 ). You can find more information here: https://www.anaconda.com/.

Note that this is needed only if you want to use nntool, you can skip this step otherwise. Once Anaconda is installed, you need to activate it and install python modules for this tool with this command:

~~~~~shell
pip install -r tools/nntool/requirements.txt
pip install -r requirements.txt
~~~~~

### Pull and compile the full tool suite

Finally, we install the full tool suite of the sdk (including nntool and autotiler).

~~~~~shell
git submodule update --init --recursive
make sdk
~~~~~

Note that if you only need autotiler (and not nntool) you can instead use:
~~~~~shell
git submodule update --init --recursive
make all && make autotiler
~~~~~

## OpenOCD

OpenOCD for Gap8 is now used instead of plpbridge. There are a few applications which require OpenOCD, as they are using OpenOCD semi-hosting to transfer files with the workstation.

You have to install the system dependencies required by OpenOCD that you can find here: http://openocd.org/doc-release/README

There are different cables setup by default for each board. In case you want to use a different cable, you can define this environment variable:

~~~~~shell
export GAPY_OPENOCD_CABLE=interface/ftdi/olimex-arm-usb-ocd-h.cfg
~~~~~

## Using the virtual platform

If you only followed [Minimal installation](#minimal-install-freertos-only-no-neural-network-tools) process, begin by compiling gvsoc:

~~~~~shell
make gvsoc
~~~~~

You can also run this example on the Gap virtual platform with this command:

~~~~~shell
make clean all run platform=gvsoc PMSIS_OS=freertos/pulpos
~~~~~

You can also generate VCD traces to see more details about the execution:

~~~~~shell
make clean all run platform=gvsoc runner_args=--vcd
~~~~~

You should see a message from the platform telling how to open the profiler.

## Using the flasher (Hyperflash)

As soon as at least one file for a file-system is specified, the command "make all" will
also build a flash image containing the file systems and upload it to the flash.

For example, you can include files for the readfs file-system with these flags in your Makefile:

~~~~~shell
READFS_FILES += <file1> <file2> <file3> ......
~~~~~

In case you don't have any file but you still want to upload the flash image, for example for booting
from flash, you can execute after you compiled your application:

~~~~~shell
make flash
~~~~~

In case you specified files, the command "make all" will not only build the application but also build
the flash image and upload it to the flash. In case you just want to build your application, you can do:

~~~~~shell
make build
~~~~~

Then after that if you want to produce the flash image and upload it, you can do:

~~~~~shell
make image flash
~~~~~


## Boot from flash

The board is by default configured to boot through JTAG. If you want to boot from flash, you need to
first program a few efuses to tell the ROM to boot from flash. Be careful that this is a permanent operation, even though
it will still be possible to boot from JTAG. This will just always boot from flash when you power-up the board or reset it.
To program the efuses, execute the following command and follow the instructions:

~~~~~shell
# if using hyperflash:
openocd-fuser-hyperflash
# if using spiflash:
openocd-fuser-spiflash
~~~~~

If you choose to boot your application from Flash, and you want to view the output of printf's in your code then you can first compile your application
with the printf redirected on the UART with this command:

~~~~~shell
make clean all platform=board PMSIS_OS=your_os io=uart
~~~~~

You can also use a terminal program, like "cutecom":

~~~~~shell
sudo apt-get install -y cutecom
cutecom&
~~~~~

Then please configure your terminal program to use /dev/ttyUSB1 with a 115200 baud rate, 8 data bits and 1 stop bit.

## Documentation

Build the documentation:

~~~~~shell
cd gap_sdk
make docs
~~~~~

If you haven't download and install the autotiler, you will probably have some warnings when you build the docs.
All the documentations are available on our website: https://greenwaves-technologies.com/en/sdk/

You can read the documentation by opening gap_doc.html in the docs folder in your browser:

~~~~~shell
firefox docs/gap_doc.html
~~~~~

If you would like PDF versions of the reference manuals you can do:

~~~~~shell
cd docs
make pdf
~~~~~

## Upgrading/Downgrading the SDK

If you want to upgrade/downgrade your SDK to a new/old version:

~~~~~shell
cd gap_sdk
git checkout master && git pull
git checkout <release tag name>
git submodule sync --recursive
# For minimal install
make clean minimal_sdk
# for full install
git submodule update --init --recursive
make clean sdk
~~~~~

Please check our release tags here to ensure the version:
https://github.com/GreenWaves-Technologies/gap_sdk/releases


## Getting help

Please log any issues you have with the SDK in the github project.

We have also created a GAP8 developers’ forum for you to ask questions and find out more about the GAP8 SDK. You can join this group at https://greenwaves-technologies.com/gap8_developers_forum/
