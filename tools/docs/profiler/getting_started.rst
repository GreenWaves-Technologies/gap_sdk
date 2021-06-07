.. _getting_started:

Getting Started
===============

Follow this guide to:

- Install the profler and its dependencies
- run mnist application


Install dependencies
--------------------

You'll need to install some dependencies

- GAP SDK : see https://github.com/GreenWaves-Technologies/gap_sdk
- Qt >= 5.13: see https://doc.qt.io/qt-5/gettingstarted.html

Requirements on Ubuntu 18
-------------------------

.. code-block:: bash

    sudo apt install qt5-default libqt5charts5-dev

How to build
------------

.. code-block:: bash

    # First initialize gap_sdk
    cd ~/gap_sdk
    # source the initialization file and choose GAP9_v2
    source sourceme.sh
    #Simply run `make` in profiler directory
    cd ~/profiler
    make

How to initialize the profiler
------------------------------

.. code-block:: bash

    #execute initialization files
    cd ~/profiler
    source init.sh
    