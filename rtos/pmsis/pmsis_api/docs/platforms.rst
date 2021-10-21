Platforms
---------

GVSOC
.....

The GVSOC API provides some features specific to GVSOC through semi-hosting to ease the debug and profiling
of the simulated software.

Engine control
=============

This provides a set of functions for controlling GVSOC execution.

.. doxygengroup:: ENGINE
    :members:
    :private-members:
    :protected-members:

Trace control
=============

This provides a set of functions for controlling system traces from the simulated software.

.. doxygengroup:: TRACES
    :members:
    :private-members:
    :protected-members:


PCER control
=============

This provides a set of functions for reading and writing core performance counters.
These counters are usually accessed with CSR instructions but this API can also be used to get more convenient ways of
accessing them, like for example dumping them all to a file.

.. doxygengroup:: PERF
    :members:
    :private-members:
    :protected-members:


VCD control
===========

This provides a set of functions for controlling VCD traces from the simulated software, in particular to modify
VCD traces value in order to give some context to the VCD view.

.. doxygengroup:: VCD
    :members:
    :private-members:
    :protected-members:
