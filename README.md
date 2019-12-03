[![GreenWaves Technologies][GWT-logo]][GWT-link]

[GWT-logo]: logo.png
[GWT-link]: https://greenwaves-technologies.com/

# Setting up the GAP8 SDK

## About

The GAP8 SDK allows you to compile and execute applications on the GAP8 IoT Application Processor. This SDK is an extract of the necessary elements from the pulp-sdk (https://github.com/pulp-platform/pulp-sdk) produced by the PULP project, to provide a development environment for the GAP8 series processors.

We provide you with all the necessary tools and two different operating systems for GAP8:

*   Tools
    -   GAP8 RISCV GNU toolchain: a pre-compiled toolchain inherited from RISC V project with support for our extensions to the RISC-V Instruction Set Architecture.
    -   PLPBRIDGE: a tool from the PULP project which allows communication between your PC and a GAP8 processor. Using this tool you can:
        * Program / control GAP8
        * Debug your application using GDB
        * Program the GAPuino flash memory with applications

*   Operating Systems
    -   PULP OS - The open source embedded RTOS produced by the PULP project
    -   Arm® Mbed™ OS - Arm Mbed OS is an open source embedded operating system. GreenWaves Technologies has ported it to GAP8 and VEGA.
    -   FreeRTOS - FreeRTOS is an open source real time operating system. GreenWaves Technologies has ported it to GAP8
    -   PMSIS - PMSIS is an open-source system layer which any operating system can implement to provide a common API to applications. We currently provide it fro PULP OS and FreeRTOS, and it is used by our applications to be portable.

##  Getting started with the GAP SDK

### Ubuntu 18.04

These instructions were developed using a fresh Ubuntu 18.04 Bionic Beaver 64-Bit virtual machine from https://www.osboxes.org/ubuntu/#ubuntu-1804-info

The following packages needed to be installed:

~~~~~shell
sudo apt-get install -y build-essential git libftdi-dev libftdi1 doxygen python3-pip libsdl2-dev curl cmake libusb-1.0-0-dev scons gtkwave libsndfile1-dev rsync autoconf automake texinfo libtool pkg-config
~~~~~

The precompiled toolchain should be clone by using git lfs, this should be installed by using the following command:

~~~~~shell
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt-get install git-lfs
git lfs install
~~~~~

## Configure USB Port for GAPuino Evaluation Board and Gapoc board

For the USB serial connection our GAPuino and Gapoc boards use an FDDI 2 port serial to USB controller. This needs to be set up (the driver is installed in the apt-get install step above).

Add your username to the dialout group:

~~~~~shell
sudo usermod -a -G dialout <username>
~~~~~

Logout from your session and login again. Now create a udev rule for gapuino ftdi2232 usb chip

~~~~~shell
touch 90-ftdi_gapuino.rules
echo 'ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6010", MODE="0666", GROUP="dialout"'> 90-ftdi_gapuino.rules
echo 'ATTRS{idVendor}=="15ba", ATTRS{idProduct}=="002b", MODE="0666", GROUP="dialout"'>> 90-ftdi_gapuino.rules
sudo mv 90-ftdi_gapuino.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
~~~~~

If you are using a Virtual Machine make sure that you give control of the FTDI device to your virtual machine. Plug the GAPuino into your USB port and then allow the virtual machine to access it. For example, for VirtualBox go to Devices->USB and select the device.

Please also make sure that your Virtual Machine USB emulation matches your PC USB version. A mismatch causes the USB interface to be very slow.

The following instructions assume that you install the GAP SDK into your home directory. If you want to put it somewhere else then please modify them accordingly.

### Ubuntu 16.04

You can follow the steps for Ubuntu 18.04 except for the following instructions.

After yu have install the system packages with apt-get, also create this symbolic link:

~~~~~shell
sudo ln -s /usr/bin/libftdi-config /usr/bin/libftdi1-config
~~~~~

## Download and install the toolchain:

Now clone the GAP8 SDK and the GAP8/RISC-V toolchain:

~~~~~shell
git clone https://github.com/GreenWaves-Technologies/gap_sdk.git
git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
~~~~~

In case you use an old git version, you may need to use these commands instead:

~~~~~shell
git clone https://github.com/GreenWaves-Technologies/gap_sdk.git
git lfs clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
~~~~~

Install the toolchain:

~~~~~shell
cd ~/gap_riscv_toolchain_ubuntu_18
./install.sh
~~~~~

Initialize and download all the sub projects of the SDK:

~~~~~shell
cd ~/gap_sdk
git submodule update --init --recursive
~~~~~

## Install Python requirements

Our modules requires a few additional Python packages that you can install with this command from Gap SDK root folder:

~~~~~shell
pip3 install -r requirements.txt
~~~~~

In order to use the Gap tools for neural networks (nntool), we strongly encourage to install the Anaconda distribution ( Python3 ). You can find more information here: https://www.anaconda.com/.

Note that this is needed only if you want to use nntool, you can skip this steap otherwise. Once Anaconda is installed, you need to activate it and install python modules for this tool with this command:

~~~~~shell
pip install -r tools/nntool/requirements.txt
pip install -r requirements.txt
~~~~~

## Configure the SDK:

You can either source sourceme.sh in the root sdk folder and then select the right board from the list, or directly source the board config.

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


## Build the SDK:

You can build the SDK with this command:

~~~~~shell
make all
~~~~~

GVSOC virtual platform will be installed during the previous build.

## Getting the Autotiler

The autotiler code generation library needs to be downloaded by this following commands:

~~~~~shell
cd ~/gap_sdk/
make autotiler
~~~~~

During the download, you will be asked several simple questions and then you will receive an email with an URL.
Please copy the url in the mail to the last question, then the download and installation would be started automatically.
If you have already answered these questions, this enquiry will be skipped.

For offering you the best support, please answer the questions.

## Getting nntool

You first need to install Anaconda and Python dependencies (see above).

Then execute:

~~~~~shell
cd ~/gap_sdk/
make nntool
~~~~~

## Getting OpenOCD

OpenOCD for Gap8 has been introduced. Plpbridge is still used by default to load binaries until the next SDK, in which OpenOCD will become the default bridge. There are a few applications which require OpenOCD, as they are using OpenOCD semi-hosting to transfer files with the workstation.

If you want to install it, execute this command from the gap SDK folder:

~~~~~shell
cd ~/gap_sdk/
make openocd
~~~~~

Applications using it are already configured in their Makefile to use it. If you also want to force it for other examples and applications, you have to define this environment variable:

~~~~~shell
export GAP_USE_OPENOCD=1
~~~~~

# Compiling, running and debugging programs

The following command configures the shell environment correctly for the GAP8 SDK. **It must be done for each terminal session**:

~~~~~shell
source ~/gap_sdk/configs/gapuino.sh
~~~~~

The configuration file you should sourced may be different, depending on the board you are using (see above).

Tip: You can add an "alias" command as following in your .bashrc file:

~~~~~shell
alias GAP_SDK='cd ~/gap_sdk && source sourceme.sh'
~~~~~

Typing GAP_SDK will now change to the gap_sdk directory and execute the source command.

Finally try a test project. First connect your GAPuino to your PCs USB port and then type:

~~~~~shell
cd ~/gap_sdk/examples/pmsis/helloworld
make clean all run
~~~~~

After the build you should see:
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

## Debugging Programs

The JTAG bridge allows for printf's in your code to generate messages on your PC. GAP8 also has support for gdb server over JTAG for both pulp-os and mbed-os programs.

To enable gdbserver run mode execute the make target gdbserver:

~~~~~shell
make clean all gdbserver
~~~~~

The program will load and gdbserver will be started. Connect to the target with gdb from another shell (remember to source the sourceme.sh file in the root of the SDK in your new shell). Change directory to the directory containing your test program and then:

~~~~~shell
riscv32-unknown-elf-gdb BUILD/GAP8/GCC_RISCV/test
~~~~~

Once gdb has loaded connect to the gdbserver on the target.

~~~~~
(gdb) target remote localhost:1234
Remote debugging using localhost:1234
~~~~~

## Using the virtual platform

You can also run this example on the Gap virtual platform with this command:

~~~~~shell
make clean all run platform=gvsoc
~~~~~

You can also generate VCD traces to see more details about the execution:

~~~~~shell
make clean all run platform=gvsoc runner_args=--vcd
~~~~~

You should see a message from the platform telling how to open the profiler.

## Using the flasher (Hyperflash)

Add the following line into your application Makefile:

~~~~~shell
PLPBRIDGE_FLAGS += -f <file1> <file2> <file3> ......
~~~~~

Without any further options the files are flashed but your application is not executed.

If you want to execute your application after flashing use the following options:

If you want to boot your application from jtag:

~~~~~shell
PLPBRIDGE_FLAGS += -jtag
~~~~~

If you wan to boot your application from hyperflash:
~~~~~shell
PLPBRIDGE_FLAGS += -hyper
~~~~~

These two flags cannot be used at the same time.

If you choose to boot your application from HyperFlash, and you want to view the output of printf's in your code then you can use a terminal program, like "cutecom":

~~~~~shell
sudo apt-get install -y cutecom
cutecom&
~~~~~

Then please configure your terminal program to use /dev/ttyUSB1 with a 115200 baud rate, 8 data bits and 1 stop bit.

## Using the file IO

Through our new bridge tool (pulp-rt-bridge), you could load/store from/to in your PC.
For example, if you want to use an image locally, but run the algorithm with this image on GAP8. You could use this functionality to do it.

To trigger this function in bridge, you need to add this flag in your Makefile:

~~~~~shell
PLPBRIDGE_FLAGS += -fileIO <option>
~~~~~

The option should be a number, which would be used for let the bridge wait for \<option\> seconds. Because of the synchronization between this bridge and our debug bridge, we need to wait until the program has been load and executed on GAP8. If you don't put a number, the default value is 2 (seconds).

Otherwise, you could run this service manually:

In another terminal:

Configure the environment, and then:

~~~~~shell
plpbridge-rt --binary=<path of your binary file> --chip=gap
~~~~~

Please read the example for more information: examples/pulp-examples/kernel/bridge

For your information, this new bridge is quite slow, please be patient when you are using it. We will resolve this problem as soon as possible.

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
git submodule update --init --recursive
make clean all
~~~~~

Please check our release tags here to ensure the version:
https://github.com/GreenWaves-Technologies/gap_sdk/releases



## Getting help

Please log any issues you have with the SDK in the github project.

We have also created a GAP8 developers’ forum for you to ask questions and find out more about the GAP8 SDK. You can join this group at https://greenwaves-technologies.com/gap8_developers_forum/

## Trademarks

Arm® Mbed™ OS are registered trademarks or trademarks of Arm Limited (or its subsidiaries) in the US and/or elsewhere.
