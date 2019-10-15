# Pulp debug bridge

This is a tool which can be used to interact with a pulp target, like doing read and write or loading a binary.
It also provide an RSP server so that it can be used to interface GDB with pulp targets.

###  Getting the sources

You can get the sources with this command:

    $ git clone git@github.com:pulp-platform/pulp-builder.git --recursive

### Prerequisites

In case the FTDI cables are needed, the libftdi must be installed. Here is the command on Ubuntu to install it:

    $ sudo apt-get install libftdi1-dev
    
Once the FTDI lib is installed, USB access rights must be updated.
Most of the time a rule like the following must be added under /etc/udev/rules.d/, for example in /etc/udev/rules.d/10-ftdi.rules:

        ATTR{idVendor}=="15ba", ATTR{idProduct}=="002b", MODE="0666", GROUP="dialout"

The following command can be used to restart udev after the rule has been added:

        sudo udevadm control --reload-rules && sudo udevadm trigger

The user should also need to be in the *dialout* group.

The following python package is also needed:

    $ sudo pip3 install pyelftools


### Installation

To build this tool, you have to clone the following repository:

    $ git clone git@github.com:pulp-platform/pulp-builder.git --recursive

Then go inside pulp-builder and execute the following command:

    $ ./scripts/build-debug-bridge

This will checkout all sources inluding other modules needed by the bridge, build everything and install that under the `install` directory.
    
All what is needed to use the tool is then inside the directory `install`. You can define the following paths in order to use it:

    export PATH=$PWD/install/ws/bin:$PATH
    export PYTHONPATH=$PWD/install/ws/python:$PYTHONPATH
    export LD_LIBRARY_PATH=$PWD/install/ws/lib:$LD_LIBRARY_PATH
    export PULP_CONFIGS_PATH=$PWD/install/ws/configs

### Usage

You can execute the following to display the help:

    $ plpbridge --help
    
You need at least to configure the cable and the target with these options:

    # plpbridge --chip=pulpissimo --cable=ftdi
    
You can for example read from the target with this command:

    # plpbridge --chip=pulpissimo --cable=ftdi read --addr=0x1c000000 --size=32
    
Or write:

    # plpbridge --chip=pulpissimo --cable=ftdi write --addr=0x1c000000 --size=32 --value=0x12345678
    
A binary can also be loaded with this command:

    # plpbridge --chip=pulpissimo --cable=ftdi load --binary=<binary path>

The RSP server for the GDB connection can be started with this command:

    # plpbridge --chip=pulpissimo --cable=ftdi gdb wait --rsp-port=1234
    
### Supported cables

2 FTDI cables are supported: --cable=ftdi@olimex and --cable=ftdi@digilent.
However the bridge may need few modifications depending on the ftdi chip which is used.

It is also possible to connect the bridge to a remote server, like an RTL platform (using a DPI model): --cable=jtag-proxy.
More information for this cable will be provided soon.

### Supported targets

Only pulp and pulpissimo are supported for now.

### Customizing existing targets

The bridge is getting architecture information about the selected chip using a JSON description of the architecture.
Such descriptions can be found under install/configs/systems (once the bridge is built). For example if the selected chip is pulpissimo, the bridge will get the description from install/configs/systems/pulpissimo.json. These configs comes from the pulp-configs module which is downloaded as a dependency and are found by the bridge using the environment variable PULP_CONFIGS_PATH.

The configs contains a lot of hardware details and are generated from high-level descriptions that can be found under install/configs/templates/chips. The idea is that this high-level config describes the most important architecture properties, which are then used by generator scripts to generate the final detailed description.

If the targetted architecture is slightly different from an existing one, it is possible to take one of these templates and customize it to reflect the differences, and then use the generated configuration to configure the bridge.

For that first copy and modify an existing template:

    $ cp install/configs/templates/chips/pulpissimo.json my_template.json

The memory map can for example be modified and will be taken into account by the generators.
Then generate the configuration from your template:

    $  pulp_config_gen --template=$PWD/my_template.json --output=my_config.json

Then instead of specifying a chip, you can specify your configuration on the bridge command-line like this:

    $ plpbridge --config=my_config.json --cable=ftdi load --binary=<binary path>
