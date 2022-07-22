Hello World
===========

Requirements
------------

No specific requirement. This example should run without issue on all chips/boards/OSes.

Description
-----------

This example is a classic Hello World.
It prints an hello world string on all available cores.

You can run this on GVSoC or your board:

.. code-block:: bash

    # Run on GVSoC
    make clean all run platform=gvsoc

    # Run on real board
    make clean all run platform=board

You should have an output looking like this (order may vary):

.. code-block::

         *** PMSIS HelloWorld ***

    Entering main controller
    [32 0] Hello World!
    Cluster master core entry
    [0 2] Hello World!
    [0 0] Hello World!
    [0 1] Hello World!
    [0 3] Hello World!
    [0 4] Hello World!
    [0 5] Hello World!
    [0 6] Hello World!
    [0 7] Hello World!
    Cluster master core exit
    Test success !

