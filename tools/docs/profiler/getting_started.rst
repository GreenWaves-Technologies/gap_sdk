.. _profiler_getting_started:

Getting Started
===============

Follow this guide to:

- Install the profler and its dependencies
- run fork application


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
    source configs/gap9_v2.sh
    #Now build the profiler
    make profiler_v2
    

