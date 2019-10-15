Usage
-----

The virtual platform must be launched through the common runner called *pulp-run*.

The specified platform must be *gvsoc* (through option *\-\-platform*) and the only mandatory option is either *\-\-config-file* or *\-\-config* in order to give the system configuration to simulate (see next section).

As the virtual platform will generate several temporary files, it is also good to launch it from a specific folder or to specify it through the *\-\-dir* option.

All the other options for the platform must be given through the configuration file (see next sections). The most common one is the binary to be simulated. A few common options has a shortcut as a direct option to *pulp-run*, you can execute this command to get them: ::

  $ pulp-run --platform=gvsoc --config=gap_rev1 --help

After the options, the set of commands to execute must be specified. The usual ones are *prepare* for generating the platform stimuli (e.g. to prepare the flash image containing the test binary), and *run* for running the simulation.

Here is a full example: ::

  $ pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run

