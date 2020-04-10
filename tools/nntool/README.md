# NNTOOL

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

To set up the tool install the packages in the requirements file

	python -m pip install -r requirements.txt

if python3 is your default interpreter or

	python3 -m pip install -r requirements.txt

if it is not and type make

	make

When everything is installed, the nntool command line can be accessed with:

	nntool

These staeps will automatically be executed by the GAP SDK setup procedure

## Visual-Wake-Words Example

To show the **nntool** usage, we provide this detailed example of the 2019 Visual Wakeup Words Challenge Winner model porting on GAP8. 

### Model Loading

The nntool takes as input network a non-quantized .tflite model. After downloading the model from the github repository (https://github.com/mit-han-lab/VWW) we can open the model into the nntool:
	
	nntool
	open /path/to/model.tflite

TFLite format uses HxWxC activations and C<sub>out</sub>xHxWxC<sub>in</sub> weights while Autotiler uses CxHxW activations and C<sub>out</sub>xC<sub>in</sub>xHxW weights. Moreover to increase the efficiency of the kernels, the Autotiler uses fused layers (e.g. ConvReLUPool). To generate the compatible AT model, the nntool has to apply graph transformations and match the Autotiler features:

	adjust
	fusions

To see the current nntool model topology use:

	show

### Quantization

Now we quantize the model to either 8 or 16 bit. The aquant command quantizes the original floating point model to a fixed point one. For the constant parameters, i.e. weights and biases, the number of integer and decimal bits is computed from their actual values distributions. On the other hand, non-constant values, i.e. activations, need a representative dataset to collect their distributions. Whenever you feed the nntool model with some inputs data you can do manipulation on them:

	aquant -f <NUM_BITS> /path/to/images/direcotry/* [input-options: -T, -D, -O, ...]
	***IMPORTANT*** If the adjust command has been used and the input has more than one channel, the -T flag is necessary to match the new activations order 


We can evaluate the signal to noise ratio (QSNR) after the quantization step by processing one or more input data:

	qerror /path/to/the/image.ppm [input-options: -T, -D, -O, -W, -H, ...] [-s]

It computes the model in the original FP32 version and then in the quantized version and compare the outputs of each layer. With the -s flag the comparison is done individually for each layer. Their output is evaluated from the FP32 input quantized instead of the output result of the quantized computation.

If for some layer a very low QSNR is reported, the user can change the bit precision with the qtune command.

#### Quantization Inspection 

The nntool provides utilities to inspect the quantization performance in details for specific tensors by comparing them side by side, for a given sample data like below:

	dump ./image.ppm -S tensors_1
	dump ./image.ppm -S tensors_2 -q -d

	Usage:
	-S: store the tensors in the workspace with the given name
	-q: compute the inference with the quantized graph
	-d: export the dequantized version of the tensors to have the same format of the one computed with the FP32 graph

To compare them:
	
	tensors -t tensors_1 tensors_2 -s 2 -c 0 [-Q]

	Usage:
	-t: tensors to compare
	-s: graph step (layer) to compare
	-c: layer channel
	[-Q]: if present outputs the QSNR between the tensors, otherwise the tensors elements are displayed side by side

### Saving the model

To save the nntool graph with all the quantization information and constant parameters tensors in the .json format:

	save_state /path/to/write/to/

To load back the saved model:

	open /path/to/nntool_model_state.json 

### Autotiler Model Generation

At this point the nntool graph is ready to be translated in an Autotiler Model format:
	
	nntool -g path/to/nntool_model_state.json -M /path/to/model/dir -m Autotiler_model_file.c -T path/to/tensors/dir 


## Input Images Options

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