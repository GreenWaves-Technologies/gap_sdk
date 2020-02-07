# Visual Wakewords

This is an implementation of the visual wakewords challenge winner at https://github.com/mit-han-lab/VWW.

The tflite file is a converted version of the trained float model and quantization is carried out in 8 bit activations and weights using sample data from the visual wake words dataset converted to ppm format.

Currently it is running in emulation mode only. We will add a Makefile to build for GAP8 shortly.
