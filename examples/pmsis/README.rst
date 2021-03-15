.. _examples_pmsis:

PMSIS
=====

The SDK contains various examples demonstrating the capabilities of GAP chips and boards
using the PMSIS API and PMSIS BSP.

Examples are divided in folders corresponding to categories.
Main categories are:

- :code:`helloworld`: classic helloworld program
- :code:`features`: examples showing GAP features
- :code:`periph`: examples showing how to use GAP internal peripherals
- :code:`bsp`: examples showing how to use the PMSIS BSP to control external devices

.. toctree::
   :maxdepth: 2
   :titlesonly:
   :glob:

   helloworld/*
   features/*
   periph/*
   bsp/*


How to contribute
-----------------

An example should:

- be in the right category
- have a :code:`README.rst` file that explains what it is doing, its requirements and limitations.
- be as simple as possible
- present only one feature
- contain useful code comments

To add an example, write it in a branch and then submit a merge/pull request.

