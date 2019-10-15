
## Support to DRAMSys SystemC TLM-2.0 Memory Models

[DRAMSys](https://www.jstage.jst.go.jp/article/ipsjtsldm/8/0/8_63/_article) is
a flexible DRAM subsystem design space exploration framework that consists of
models reflecting the DRAM functionality, power consumption, retention time
errors, etc.

### DRAMSys Installation

This documentation assumes that access to DRAMSys repository is granted.
Open-sourcing process in progress.

A convenience script [get_dramsys.sh](./get_dramsys.sh) to clone the
repository and install dram.sys dependencies is provided.

After running the script make sure you have the following environment
variables in your ~/.bashrc. 

Some segments extracted from a functional ~/.bashrc file are presented below
to be used as reference. Note that you may have to adapt it, for example,
changing paths to point to the place you installed some of the libraries.

```bash
# SystemC home and target architecture
export SYSTEMC_HOME=$HOME/systemc-2.3.1a
export SYSTEMC_TARGET_ARCH=linux64
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${SYSTEMC_HOME}/lib-$SYSTEMC_TARGET_ARCH

# Qwt lib
export LIBQWT_HOME=$HOME/qwt-6.1/lib
export LIBQWT_HEADERS=$HOME/qwt-6.1/src
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH:+${LD_LIBRARY_PATH}:}$LIBQWT_HOME

# Python lib
export LIBPYTHON_VERSION="3.5m"
export PYTHON_HOME=/usr/lib/python3.5
export PYTHON_HEADERS=/usr/include/python3.5m
```

Alternatively, clone the repository manually and follow the steps described in
its documentation.

```bash
$ cd models/memory
$ git clone --recursive https://git.eit.uni-kl.de/ems/astdm/dram.sys.git
```

Check DRAMSys project documentation, follow the steps on README.md in order to
get all the dependencies installed in your system and learn about the features
provided.

### Building gvsoc with DRAMSys

Two environment variables influence the build process and should be defined in
order to use DRAMSys. They are:

```bash
export VP_USE_SYSTEMC=1
export VP_USE_SYSTEMC_DRAMSYS=1
```

Optionally, these variables can be added to an initialization file such as
~/.bashrc.

The desired DRAMSys configuration file can be passed as a configuration option.

See [ddr_sc_impl.cpp](./ddr_sc_impl.cpp) for more details. You may want to
double check or change the values below:

```
define BYTES_PER_ACCESS            64
define ACCEPT_DELAY_PS             1000
```

## References

[1] TLM Modelling of 3D Stacked Wide I/O DRAM Subsystems, A Virtual Platform
for Memory Controller Design Space Exploration
M. Jung, C. Weis, N. Wehn, K. Chandrasekar. International Conference on
High-Performance and Embedded Architectures and Compilers 2013 (HiPEAC),
Workshop on: Rapid Simulation and Performance Evaluation: Methods and Tools
(RAPIDO), January, 2013, Berlin.

[2] DRAMSys: A flexible DRAM Subsystem Design Space Exploration Framework
M. Jung, C. Weis, N. Wehn. Accepted for publication, IPSJ Transactions on
System LSI Design Methodology (T-SLDM), October, 2015.

