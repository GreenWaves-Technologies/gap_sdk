Introduction
------------

The NNTool is a graph manipulation tool capable of reading a high-level
computational graph description mixing DSP and NN operators and mapping it
to a set of Autotiler nodes (named Generators, provided in the GAP SDK,
look at the autotiler docs for more details) that it will use to
generate optimized GAP C code.

NNTool is a Python library with a numpy backend to run the provided
computational graph in both floating and fixed point arithmetic,
bit accurate wrt GAP fixed point inference.

The process that the NNTool applies to a provided Neural Network is the
following:

    1. Load the model description
    2. Quantize the graph (if not already, i.e. load tflite quantization)
       through tensor statistics collection with floating point inference
       over a calibration dataset
    3. Map the original operators into Autotiler operators, i.e. match tensor
       order, insert reshape/transposes where needed, fuse nodes if possible
    4. Generate the Autotiler Model

From a user perspective, NNTool can be used both interactively in command
line or directly importing it as a package in a Python script.
The graphs provided to NNTool must be TFLite or ONNX models.

NOTE: Not all the operands from the two formats are supported, for a complete
list open the nntool command line and type:
