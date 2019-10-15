
## Support to Gem5 SystemC TLM-2.0 Binding

Requirements to use this feature:

Two environment variables influence the build process and should be defined in
order to use gem5 SystemC TLM-2.0 binding. They are:

```bash
export VP_USE_SYSTEMC=1
export VP_USE_SYSTEMC_GEM5=1
```

Optionally, these variables can be added to an initialization file such as
~/.bashrc.


One can use the scripts from [gem5.TnT](https://github.com/tukl-msd/gem5.TnT)
to install gem5, build it as a library to be linked to this project, get
benchmark programs, etc.

A convenience script [gem5.tnt.sh](gem5.tnt.sh) that executes essential steps
to build gem5 as a library is provided.

Executing the script:

```bash
$ ./gem5.tnt.sh
```

The script will,

Clone [gem5.TnT](https://github.com/tukl-msd/gem5.TnT) repository and change to its directory.

```bash
$ git clone https://github.com/tukl-msd/gem5.TnT.git
$ cd gem5.TnT
```

Install known dependencies. Note that this step requires **sudo**.

```bash
$ sudo ./dep_install.sh
```

Get essential repositories.

```bash
$ ./get_essential_repos.sh
```

Build gem5 as a library (and as standalone simulator).

```bash
$ ./build_gem5.sh
```

Other environment variables that influence the build are shown below. In the
example the respective default values are assigned.

```bash
export VP_USE_SYSTEMC_GEM5_HOME=${HOME}/gem5_tnt/gem5
export VP_USE_SYSTEMC_GEM5_LIB=${VP_USE_SYSTEMC_GEM5_HOME}/build/ARM/libgem5_opt.so
```


## References

[1] [https://github.com/tukl-msd/gem5.TnT](https://github.com/tukl-msd/gem5.TnT)

[2] The gem5 Simulator. Nathan Binkert, Bradford Beckmann, Gabriel Black,
Steven K. Reinhardt, Ali Saidi, Arkaprava Basu, Joel Hestness, Derek R. Hower,
Tushar Krishna, Somayeh Sardashti, Rathijit Sen, Korey Sewell, Muhammad
Shoaib, Nilay Vaish, Mark D. Hill, and David A. Wood. May 2011, ACM SIGARCH
Computer Architecture News.

[3] [http://www.gem5.org/Main_Page](http://www.gem5.org/Main_Page)

[4] System Simulation with gem5 and SystemC: The Keystone for Full
Interoperability C. Menard, M. Jung, J. Castrillon, N. Wehn. IEEE International
Conference on Embedded Computer Systems Architectures Modeling and Simulation
(SAMOS), July, 2017, Samos Island, Greece.

