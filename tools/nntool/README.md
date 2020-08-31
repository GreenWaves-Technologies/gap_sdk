# NNTOOL

## Table of contents
- [Overview](#overview)
- [Installation](#installation)
- [Model Conversion](#model-conversion)
- [Quantization](#quantization)
- [Nntool Execution](#nntool-execution)
- [Model Save](#model-save)
- [Autotiler Model Generation](#autotiler-model-generation)
- [Image Formatter](#image-formatter)
- [Input Options](#input-options)

## Overview

NNTOOL helps to port NN graphs from various NN training packages to GAP8. It helps with:

- Post training graph quantization
- Graph evaluation for activation and parameter sizing
- AutoTiler CNN model generation

NNTOOL helps in the development phase to select appropriate quantization settings and in the project build phase to
automate the build process using any software build system (such as Make, CMake, etc.)

The tool has a command interpreter mode where it provides an interface to:

- Visualize the graph
- Calculate parameter and activation dynamic statistics
- Test quantization settings
- Transform the graph to fit AutoTiler generators by fusing operators together
- Equalize cross channel variations in weights to improve post quantization accuracy
- Generate AutoTiler model file
- Save a state file with all the transformations done to the graph

The tool also has a command line mode which takes a saved state file and directly genenerates the model and
saves a parameters file; the two elements necessary for a GAP project build.

## Installation

To set up the tool install the packages in the requirements file

	python -m pip install -r requirements.txt

if python3 is your default interpreter or

	python3 -m pip install -r requirements.txt

if it is not and type make

	make

When everything is installed, the nntool command line can be accessed with:

	nntool

These steps will automatically be executed by the GAP SDK setup procedure


## Model Conversion

The nntool takes as input network a float or quantized .tflite model. You can find bunch of trained tflite models online in both integer-only and floating-point version online ([TF hosted models](https://www.tensorflow.org/lite/guide/hosted_models)). To start nntool and open the target model:
	
	nntool
	open /path/to/model.tflite [-q]

The -q is required if the target model has already been quantized in the tflite conversion process (i.e. inference_type=QUANTIZED_UINT8).

TFLite execution kernels use HxWxC order for the activations and C<sub>out</sub>xHxWxC<sub>in</sub> order for filters. On the other hand, the Autotiler, and therefore GAP execution kernels, use CxHxW activations and C<sub>out</sub>xC<sub>in</sub>xHxW filters. Moreover to increase the efficiency of the kernels, the Autotiler uses fused layers (e.g. Convolution followed by a pooling and a ReLU can be performed by a single AT layer: ConvPoolRelu). For these reasons, to generate the compatible AT model, the nntool has to apply graph transformations and match the Autotiler features:

	adjust
	fusions [--scale8 | --pow2]

IMPORTANT: the _fusions_ type (scale8 vs pow2) must match the quantization scheme which will be performed (see next session).

To see the current nntool model topology use:

	show

## Quantization

To run on GAP platforms the model must be quantized with one of the Autotiler supported scheme:
- 8-bits quantization: similar to [tensorflow lite quantization](https://www.tensorflow.org/lite/performance/quantization_spec) but targets symmetric computational kernels (Autotiler). This scheme is automatically applied if the input tflite graph is already quantized: the quantization specs are simply translated to match the symmetric kernels.
- 16-bits quantization: this scheme targets a PowerOf2 quantization, i.e. each tensor is interpreted as a signed 16bits Qm.n fixed-point vector with m integer bits and n decimal bits. This approach can lead to better accuracy results but is paid with 2x memory footprint and almost 2x latency increase.

NOTE: the _fusions_ command above must meet the quantization scheme that you want to apply (--scale8 in case of 8-bits quantization and --pow2 in case of 16-bits quantization). If you want to change the scheme for any reason, you will need to reopen the original graph and perform the _fusions_ step again.
NOTE2: the 16-bits quantization scheme is supported only if the input graph is not already quantized.

If the imported tflite graph targets a floating point execution, nntool can perform the post-training quantization step providing the network a set of calibration data on which it can collect the min/max ranges statistic for the activations:

	aquant -f [8 | 16] /path/to/images/direcotry/* [input-options: -T, -D, -O, ...]
	***IMPORTANT*** If the adjust command has been used and the input has more than one channel, the -T (transpose) flag is necessary to match the new activations order (input image with HxWxC to CxHxW)

nntool can execute the graph in both floating point and quantized precision, hence we can evaluate the signal to noise ratio (QSNR) after the quantization is set one or more input data by comparing the two results:

	qerror /path/to/the/image.ppm [input-options: -T, -D, -O, -W, -H, ...] [-s]

With the -s flag the comparison is done individually for each layer: their output is evaluated from the FP32 input quantized instead of the output result of the quantized computation.

## Nntool Execution 

The nntool provides utilities to inspect the output activation tensors which come from a specific input execution in details:

	dump ./image.ppm -S tensors_1
	dump ./image.ppm -S tensors_2 -q -d

	Usage:
	-S: store the tensors in the workspace with the given name
	[-q]: compute the inference with the quantized graph (if not specified the network is run in floating point)
	[-d]: beside -q export the dequantized version of the tensors to have the same format of the one computed with the FP32 graph
	[-P file.npy]: save the list of activations tensors in a file

To compare them side by side or with QSNR (in this case tensors_1 come from the float execution while tensors_2 from the quantized one with dequantized (real numbers) values instead of the integer ones):
	
	tensors -t tensors_1 tensors_2 -s 2 -c 0 [-Q]

	Usage:
	-t: tensors to compare
	-s: graph step (layer) to compare
	-c: layer channel
	[-Q]: if present outputs the QSNR between the tensors, otherwise the tensors elements are displayed side by side

## Model Save

To save the nntool graph with all the quantization information and constant parameters tensors in the .json format:

	save_state /path/to/write/to/

To load back the saved model:

	open /path/to/nntool_model_state.json 

## Autotiler Model Generation

At this point the nntool graph is ready to be translated in an Autotiler Model format:
	
	nntool -g path/to/nntool_model_state.json -M /path/to/model/dir -m Autotiler_model_file.c -T path/to/tensors/dir 

## Image Formatter

To handle different type of input images format you can add to your graph an input formatter which will generate the Autotiler optimized code for the proper conversion. It supports:
- rgb565 HxWxC input to rgb888 CxHxW (*rgb565_rgb888*)
- rgb888 HxWxC input to rgb888 CxHxW (*rgb888*)
- rgb888 HxWxC input to rgb16 CxHxW (*rgb16*)
- grayscale8 input to grayscale8 (*bw8*)
- grayscale8 input to grayscale16 (*bw16*)

It also handle the conversion between uint8 [0:255] values to int8 [-128:127] supported in the AT convolutional kernels. You will need to speify the desired technique:
- *shift_int8*: will apply elemnt-wise a right shift of 1 bit (>> 1) so that the values do not overflow the max int8 [0:128] (more efficient)
- *offset_int8*: will apply element-wise a -128 addition to output [-128:127] values ready for AT Convolutional kernels (more accurate)
- (for 16 bits graphs only) *out_int16*: takes the uint8 input and converts to int16 output by applying a left shift of 7 bits (<< 7)

The command to run to introduce the formatter into your graph is:
	imageformat input_x [bw8 | bw16 | rgb888 | rgb565_rgb888 | rgb16] [shift_int8 | offset_int8 | out_int16]

NOTE: in case of multichannel input the image formmatter will automatically handle also the automatic transposition when you run the network in nntool (i.e. the -T option is no more needed)

## Input Options

Whenever one or several images are given to the nntool (i.e. with commands like dump, aquant, qerror, ...) there are options you can set to perform preprocessing on them:

	-T: transposes the input images from (HxWxC) order to (CxHxW), useful when multichannel models have already been adjusted
	-H [value]: resize the input images height to value
	-W [value]: resize the input images width to value
	-D [value]: divides each inputs pixel by value
	-O [value]: sums to each inpts pixel value (offset)
	-F [nptype]: interpret pixels as this numpy type
	-N [lambda function]: lambda function to apply on input in the form x: fn(x)

Another option is to set some of this parameters by default using the set command:

	set input_transpose [True/False]	
	set input_width  [value]
	set input_height [value]
	set input_divisor
	set input_offset
	set input_norm_func [lambda function]: is equivalent to set each time an image is provide -N [lambda function]

NOTE: if an input_norm_func is set the other operational settings (input_divisor or input_offset) will be ignored