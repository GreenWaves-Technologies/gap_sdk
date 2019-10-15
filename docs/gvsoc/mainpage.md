# Introduction

GVSOC is a lightweight and flexible instruction set simulator which can simulate GreenWaves' GAP8 IoT application processor. GVSOC allows execution of programs on a virtual platform without any hardware limits. Thanks to device models, full application with real device drivers can be simulated. Currently, we provide simulations of devices such as cameras, microphones, LCDs, etc.

Hereafter you will find a very brief introduction of GVSOC. For further information, please visit: https://gvsoc.readthedocs.io/en/latest/

# GVSOC installation

The installation of gvsoc has been fully integrated into the GAP SDK install process. Once you have folowed the GAP SDK install process, the command "make all" in gap_sdk, will install GVSOC for GAP8 in your SDK. You could of cause install it alternatively by executing the following commands:

~~~~~shell
cd ~/gap_sdk
make gvsoc
~~~~~

# How to start compiling and running applications on GVSOC

Before running the applications, please make sure:

 - The toolchain has been installed successfully.
 - The SDK has been built without error.
 - The shell environment has been configured correctly for the GAP8 SDK.

## How to compile and run the "Hello World” example on GVSOC

The hello world example gets each core to print out a hello message. We will go through this example in detail so that you can learn how to schedule tasks on the cluster.

Like other examples and applications, this example is located in the "examples" folder:

~~~~~shell
	cd examples/pulp-examples/hello_world
~~~~~

Then execute this command for compiling and running the example on GVSOC:

~~~~~shell
	make clean all run platform=gvsoc
~~~~~

Of course, you can also compile and execute the example separately.

~~~~~shell
	make clean all
	make run platform=gvsoc
~~~~~


In the result, you will see the Hello from all the cores in FC and in Cluster, with their Cluster IDs and Core IDs.
All the examples can be built and run with the same procedure.

# How to run the application with instruction traces

The virtual platform provides the ability to dump instruction traces to aid in debugging.

To activate this function, you just need to add 1 parameter in your execution command:

~~~~~shell
	make run platform=gvsoc  runner_args="--trace=insn"
~~~~~

# How to run an application with VCD trace

The virtual platform can also dump VCD traces which show the state of several components over time, like the core’s PC (program counter), DMA transfers, etc. This gives a better overview than instruction traces.

VCD tracing can be activated through the option vcd:

~~~~shell
make clean all run platform=gvsoc runner_args="--vcd"
~~~~

Once the platform is run, this will generate a VCD file called all.vcd in the build folder. This file is a raw file containing all the signal values.

Another file called view.gtkw is generated and can be opened using GTKwave. This is a script file which will setup the view with the most interesting signals. The command to be executed is displayed at the beginning of the simulation when VCD traces are enabled.

# How to do more

You will find much more options on the GVSOC documentation here: https://gvsoc.readthedocs.io/en/latest/

The documentation is giving information about the options which can be given to the GVSOC runner. All these options can be passed from the makefile to the runner with the following command, like for instruction or vcd traces:

~~~~shell
make clean all run platform=gvsoc runner_args="<GVSOC options>"
~~~~