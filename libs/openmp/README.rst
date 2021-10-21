.. _openmp_lib:

OpenMP
======

This repository contains the source code for GAPs OpenMP API.

It only supports GCC.

Supported features
------------------

GAP OpenMP only support static scheduling and the following directives (pragma):

- :code:`parallel`
- :code:`critical`
- :code:`for`
- :code:`barrier`
- :code:`atomic`
- :code:`single`

How to use
----------

To use OpenMP in your application, simply add :code:`CONFIG_OPENMP=1` to your Makefile.
