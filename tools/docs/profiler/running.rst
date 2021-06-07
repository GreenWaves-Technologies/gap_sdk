.. _running:

Running the Profiler
====================

To run the profiler, first go to an example. Let’s try the mnist  example: 

>cd ~/gap_sdk/examples/nntool/mnist

Then run the profiler:

>make all run profiler PMSIS_OS=pulpos platform=gvsoc

This launches the profiler graphics application and the profiler window is displayed with an empty timeline window that will later on display all the GAP9 signals transferred from Gvsoc. 

.. image:: /source/tools/docs/profiler/images/image_0.png


**Fig 1: Profiler Window**
