LCD with camera
===============

Requirements
------------

- Gapuino board with HM01B0 or MT9V034 camera, and ili9341 LCD display.

Description
-----------

This example shows how to capture images from the camera and display them on the LCD on the Gapuino board.

It can be run on the board with command :code:`make all run` or on GVSoC with command :code:`make all run platform=gvsoc`.
On GVSoC, the Makefile is giving a PGM file to the camera model which is using it to generate the input CPI stream to the chip.
