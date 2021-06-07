Introduction
============

The GAP SDK allows you to compile and execute applications on GAP processors.

It provides:

- An extensive set of tools including:

   - NNTool: a set of tools to easily port NN graphs from various training packages to GAP chips.
   - :ref:`autotiler`: A code generator which can generate a user algorithm (CNN, MatrixAdd, MatrixMult,
     FFT, MFCC, etc) with optimized memory management.
   - :ref:`gvsoc`: a lightweight and flexible instruction set simulator which can simulate GAP processors,
     allowing execution of programs on a virtual platform without any hardware limit.

- Two different operating systems: FreeRTOS and PulpOS
- a common API to create applications that can run on both OSes and interacts easily with other devices
- :ref:`examples` and :ref:`applications` to help you create your own application.

GAP Chips
---------

To learn more about GAP chips check these pages:

.. toctree::
   :maxdepth: 1

   gap8.rst

Getting Started
---------------

Welcome to GAP SDK! See the documentation's :ref:`getting_started` to start developing
and :ref:`user_dev_guides` for more advanced subjects.

Resources
---------

- **Website:** https://greenwaves-technologies.com
- **Source code:** https://github.com/GreenWaves-Technologies/gap_sdk
- **Releases:** https://github.com/GreenWaves-Technologies/gap_sdk/releases
- **Issues:** https://github.com/GreenWaves-Technologies/gap_sdk/issues

