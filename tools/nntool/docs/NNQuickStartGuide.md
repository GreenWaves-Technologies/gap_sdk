# NN Quick Start Guide

In this post we will guide you through the GAP*flow*, a set of easy-to-use tools which allow you to easily and automatically port a trained neural network algorithm to the Greenwaves-Technologies chip family (GAP) and run inference operations using it. To install the GAP SDK and all the needed tools follow the guide [here](https://github.com/GreenWaves-Technologies/gap_sdk).

## GAP*flow*

Before starting with the tutorial let’s have a look at the components of GAP*flow*. GAP*flow* takes a component rather than black box approach to neural network conversion. This has all the benefits of a fully automated process however it allows you to see and understand each step so that if you have a problem you can quickly resolve it.

![GAP*flow*](https://greenwaves-technologies.com/wp-content/uploads/2020/06/img_gapflow.png)

The green boxes represent the tools that we use in our flow, the blue boxes represent the files generated automatically by the different tools and the orange boxes the input files that you need to provide.

The key element in the flow is the GAP AutoTiler: it optimizes the execution of convolutional layers, minimizing memory access, reducing the memory bottleneck that characterizes most NN architectures. For the actual computation, it provides parallelized convolutional kernels which leverage the GAP8 multi-core cluster and its ISA to execute the operation on integer only values (int8/int16 to speed up the computation of a matrix multiply by 4x/2x). To do its job the AutoTiler needs a model description called “AutoTiler Model”. This .c file contains the description of the neural network topology with all the layer types and the connections between them and also includes the quantization policy that must be applied to each NN parameter.

To ease production of the AutoTiler Model we provide NNTool which, through an interactive command line interface, takes an high level NN description from TFLite and translates all the layers and parameters to an AT model description. It can be fed with already quantized TFLite models and translate the quantization info to the AutoTiler “language” or with an unquantized float graph and some sample input data with which it can compute the quantization calibration. NNTool can also provide an easy way to test your model before porting to the platform thanks to internal python implementations of the AutoTiler kernels. 

## Visual Wake Words example

To walk through the flow let’s look at a practical example of porting a neural network to GAP. The (visual wake words)[https://arxiv.org/abs/1906.05721] problem is a typical example of an embedded AI application. We need to detect whether a person is present in an image or not. To address the problem we choose the 2019 Visual Wake Words challenge winner released by Song Han MIT group in (this)[https://github.com/mit-han-lab/VWW] github repository. Here you can find the float and quantized .tflite models. For the purpose of this tutorial we will use the quantized model. In this way we will use the quantization parameters learned during the quantization aware training step done by the authors. The advantage of using these parameters is that they are based on the full data set used during training rather than a sample set of inputs. The following steps are implemented in the examples/nntool/visual_wake folder inside the GAP sdk.

### NNTool

After all the sdk has been properly installed and sourced (source gap_sdk/sourceme.sh) you will be able to run the nntool:

    nntool path/to/model.tflite [-q]

The nntool will start and translate the flatbuffer (.tflite) file into an internal network description. The -q flag will tell the framework to also load the quantization info stored in the flatbuffer. From now on, we can use the following commands to see the network structure with number of parameters/operations for each layer, and the quantization schemes applied to them:

    show
    qshow

Once the model has been imported, we need to start the process of translating into something compatible with the AutoTiler. First thing to do is to adjust the tensors dimensions: the Autotiler deals with CxHxW activation tensors and CoutxCinxHxW weights whereas the imported TFlite tensors are in HxWxC and CoutxHxWxCin order:

    adjust

Next thing is to find substructures in the graph which can be implemented with optimized kernels by the Autotiler (e.g. convolution followed by a pool followed by a activation becomes a single AutoTiler layer):
    
    fusions [--scale8 | --pow2]

The flag indicates the type of quantization scheme which the user wants to apply. TFlite quantization requires the --scale8 flag. --pow2 is needed when we want to quantize the network in 16bits (not covered in this post).

The network is now ready and the AT model file can be generated besides the constant tensors file for the layers parameters:
    
    gen ATModel.c -t -T path/to/tensors/dir

At this point the user can save the nntool state file before exiting the shell with:
    
    save_state

In this way all the model information (adjusted tensors, fused layers and quantization info) will be stored in a .json dictionary besides the network parameters (.nnparam file) and can be accessed with:
    
    open file.json

Or from the linux shell with:

    nntool file.json

To automate the steps described above, the user can provide a .txt file with all the commands he needs to run in nntool and execute them in scripting mode:

    nntool -s nntool_script.txt path/to/model.tflite [-q]

### Autotiler

Now that we have the AutoTiler Model we have to compile and run it from the linux shell:
    
    gcc ATModel.c -g -o Gentile -I. -I${TILER_INC} -I${TILER_EMU_INC} -I${TILER_CNN_GENERATOR_PATH} -I${TILER_CNN_GENERATOR_PATH_SQ8} -I${NNTOOL_GENERATOR_PATH} -I${TILER_CNN_KERNEL_PATH} -I${TILER_CNN_KERNEL_PATH_SQ8} -I${NNTOOL_KERNELS_PATH} ${TILER_LIB} ${TILER_CNN_GENERATOR_PATH}/CNN_Generator_Util.c ${TILER_CNN_GENERATOR_PATH_SQ8}/CNN_Generators_SQ8.c ${NNTOOL_GENERATOR_PATH}/nntool_extra_generators.c

All the symbols are defined when the sdk is sourced. You can look at their values with:
    
    env | grep TILER 
    env | grep NNTOOL

Now the AT executable can be run:
    
    export L1_MEMORY=48804
    export L2_MEMORY=200000
    export L3_MEMORY=83888608
    ./Gentile -o output/path/dir -c output/path/dir --L1 ${L1_MEMORY} --L2 ${L2_MEMORY} --L3 ${L3_MEMORY}

We suggest that you automate this process with the usage of a build tool such as make, to see some templates you can look in the examples/nntool folder inside the GAP_SDK_HOME.

### Application Code

The Autotiler will generate 3 files: vwwKernels.c, vwwKernels.h and vww_L3_Flash_Const.dat. The first two define the functions that will be used in the application code, the latter is the set of constant parameters wrapped in an AutoTiler readable format. The functions defined in the C code files are 3:

- Model constructor: allocates the memory needed during the network execution
- Model run: sequence of network layers execution
- Model destructor: deallocates the memory allocated in the constructor

After switching on the cluster in the main application, you will need to construct the network, then send the Model run to the cluster and use the output and finally destroy the network. 

To compile and run the model you will need all the PMSIS rules for the cluster execution etc, we suggest to use a Makefile and automate this steps following the numerous examples in the sdk folder.

### Emulation mode

The AutoTiler functions also support another mode of execution called \_\_EMUL\_\_. In this flow all the GAP specific functions generated during the AutoTiler Model run are substituted with x86 sequential functions and the generated code can be run on a Linux machine. 

![Emulation Mode](https://greenwaves-technologies.com/wp-content/uploads/2020/06/img_emul.png)

In the nntool examples you can find the emul.mk and the main_emul.c which shows the basic usage of this mode. Following them you will end up with an executable file which can be simply run from the linux shell:
    
    ./main_emul

### NNTool evaluation and debugging

Thanks to the python description of the AT kernels, the nntool can execute the graph’s operation just like the AutoTiler generated model run function would:
    
    dump path/to/input.png -S nntool [-q -d] [-P file.npy]

This command will save into an internal variable (called in this case nntool) the output tensors from each layer and, optionally save them into a file. -q flag is specified if the execution has to be run with the quantized values, in this case the -d flag will dequantize the integer values to real numbers so that we can compare them to float execution of the graph.
    
    tensors -t tensors_1 tensors_2 [-Q] [-s n]

Will compare two sets of tensors saved in two different dump stages, -Q will compute the QSNR between the two (an estimation of how much they are similar) while the optional -s will compare only the output of the layer specified (step n - use the show command to find the step numbers).

From the nntool you can set also more AutoTiler options, in particular you can specify to the AutoTiler that you want to dump all the intermediate tensors during the network execution via printf:
    
    set graph_dump_tensor 7

In this way all the weights, biases and output activations will be printed out (we suggest to mostly use this feature in AutoTiler emulation mode otherwise the printf will slow down the execution on chip). If you redirect this output to a file (run the network > file.dat), this can be read in nntool later on and compared to the actual nntool execution:
    
    tensors --gap_load file.dat 
    tensors -t file.dat nntool -Q
