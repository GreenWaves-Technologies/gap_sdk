Jpeg Encoder
============

This is an example of JPEG encoding.
It reads the input PGM image from the flash, encodes it to JPEG, and saves it to the workstation using semi-hosting.

You can run this example with "make clean all run" or "make clean all run platform=gvsoc".
Once it has been run, the output JPEG image can be opened in the build folder with

.. code-block:: c

	xview imgTest0.jpg

Versions
--------

You can choose to run the monchrome or the color example changing this flag ``COLOR_JPG?=1`` in the Makefile.

You can run the encoder on the Gap Cluster or Fabric Controller. This flag ``RUN_ON_CLUSTER?=1`` in the makefile control the execution.


Jpeg Quality and Compression
----------------------------

There are different level of jpeg compression available. The default value (without defining any flag in the Makefile) is JPEG_Q_50 which representes 50% compression.

To change the Jpeg quality you can add one of these flags in the Makefile:

.. code-block:: c

	APP_CFLAGS += -DJPEG_Q_0
	APP_CFLAGS += -DJPEG_Q_10
	APP_CFLAGS += -DJPEG_Q_20
	APP_CFLAGS += -DJPEG_Q_90
	APP_CFLAGS += -DJPEG_Q_90
	APP_CFLAGS += -DJPEG_Q_95
