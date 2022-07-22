Command Line Usage
------------------

To open NNTool in a command line interactive interface, after sourcing the
GAP SDK:

.. code-block:: bash

    nntool

Open a Model
^^^^^^^^^^^^

As mentioned above, the model can be .tflite or .onnx, the -q option allows
the user to load the quantization information already present in some tflite
model (only if the tflite flatbuffer has been converted with quantization
oprions)

.. code-block:: bash

    open path/to/model.tflite [-q]

Visualize the Graph
^^^^^^^^^^^^^^^^^^^

At any time, the graph can be visualized with:

.. code-block:: bash

    draw
        # Render the graph in a single image
    show
        # Display the structure of the graph in a literal way
    qshow
        # Display the quantization structure of the graph in a literal way

Adjust and Fusions
^^^^^^^^^^^^^^^^^^
To match the Autotiler Generators the structure of the graph might need a
restructure, for example the convolutions by default are handled in CHW
tensor order by the backend library of Autotiler, hence the adjust reorders
the tensors to match the Autotiler backend. Also, some layer sequences can
be merged together to be more efficient in the finel target, i.e.
Conv+Pool+ReLU can be a single node: fusions spots those structures.
According to the type of quantization you want to apply, the set of fusions
can change:

    - scale8: for 8 bits scaled quantization (both SW or NE16) and Float16
    - pow2: for 16 bits POW2 quantization

.. code-block:: bash

    adjust
    fusions [--scale8 or --pow2]

If the graph contains standalone piecewise DSP operations such as
div/mul/add/pow/sqrt/log NNTool can also wrap them in fused structures
and will generate specific layers for them. This last step is not
automatically run, if required it will be prompted in further steps:

.. code-block:: bash

    fusions -a expression_matcher

NNTool Inference
^^^^^^^^^^^^^^^^

In the following sections will be presented several commands that use the
NNtool inference engine. Any time the user run inference on NNTool over
inputs coming from files, he/she must be sure that the input shapes match
the graph inputs, i.e. if the network has been adjusted to CHW input should
match this order. Inputs can be numpy tensor, images (opened with Pillow),
binary (.dat) or audio files (.wav).

NOTE: Regardless the type of inference (quantized or float), NNTool expects
data in their floating point real representation, i.e. if a network has been
trained with images normalized to [-1:1] the data must be normalized as well
before feeding NNTool.

To make this input preprocessing more useful from user perspective (especially
with images where they natively come in [0:255] format) there are these
options:

.. code-block:: bash

    INPUT_OPTIONS:
        -B, --bit_shift BIT_SHIFT
            # shift input before offset and divisor. Negative for right shift.
        -D, --divisor DIVISOR
            # divide all input data by this value
        -O, --offset OFFSET
            # offset all input data by this value
        -H, --height HEIGHT
            # resieze image height to this value
        -W, --width WIDTH
            # resieze image width this value
        -T, --transpose
            # Used only on images, it swap the channels position (from CHW to HWC or viceversa)
        -F, --nptype
            # Interpret pixels as this numpy type
        -M, --mode {1, L, P, RGB, RGBA, CMYK, YCbCr, LAB, HSV, I, F}
            # mode to import image in
        -N, --norm_func "x:x/128"
            # lambda function to apply on input in the form x: fn(x)
        --rgb888_rgb565
            # convert 3 channel 8bits input into 1 channel 16bit rgb565

These options can be also set on the entire shell, thus they will be
applied without having to specify every time:

.. code-block:: bash

    set image_height HEIGHT
        # resieze image height to this value
    set image_mode {1, L, P, RGB, RGBA, CMYK, YCbCr, LAB, HSV, I, F}
        # mode to import image in
    set image_width WIDTH
        # resieze image width this value
    set input_divisor DIVISOR
        # divide all input data by this value
    set input_norm_func "x:x/128"
        # lambda function to apply on input in the form x: fn(x)
    set input_offset OFFSET
        # offset all input data by this value

Quantization
^^^^^^^^^^^^
To quantize the Model, NNTool uses a numpy float32 backed to run inference
on the graph and collect the tensors statistic (min/max). If the original
graph was a TFLite quantized model, these information have been collected
at loading time.

With these information NNTool uses an algorithm to propagate the quantization
information through the graph, for example it handles the fact that a concat
layer needs to have the same quantization in all its inputs.

At this stage the user can also choose different types of executions, this
might affect the quantization types, i.e. NE16 requires asymmetric unsigned
8 bits while SW symmetric signed 8 bits. The type of execution can be selected
graph-wise or layer-wise giving the graph step numbers related to the nodes
the user wants to quantize in a certain way.

.. code-block:: bash

    QUANT_OPTIONS:
            --scheme  {SQ8, POW2, FLOAT}
                # quantize with scaling factors
                # (TFlite quantization-like) [default] or POW2
            --float_type {bfloat16, float16, ieee16, float32, ieee32}
                # If scheme=FLOAT it selects the type of datatype
            --hwc
                # Use HWC SW Kernel instead of CHW (only with SQ8)
            --use_ne16
                # Enable use of NE16 kernels (if supported)
            --weight_bits {2, 3, 4, 5, 6, 7, 8}
                # (Only available on NE16 layers)
            --force_external_size {8, 16}
                # number of bits to use for outputs features and RNN states
                # (only available in NE16)
            --force_input_size {8, 16}
                # number of bits to use for input features
                # (only available in NE16)
            --softmax_out_8bits
                # make the output scale8 8 bits (default 16bits)
            --clip_type
                # Clipping method for any node that modifies its input:
                #    none (default) - the minimum and maximum observed values are used.
                #    laplace, gaus - Values chosen based on laplace or gaussian distribution
                #    mix - MSE is used to estimate if distribution is laplace or gaussian
                #    whiskers - 1.5 * inter quartile range
                #    std3, std5 - 3 or 5 times standard deviation from mean

.. code-block:: bash

    aquant path/to/calibration/samples* [INPUT_OPTIONS] [QUANT_OPTIONS]
            --json JSON_PATH
                # Quant options can be stored in a json dictionary and provided here
            --stats STATS_PATH
                # Instead of running calibration in nntool, statistics can be
                # passed as a pickle

    # If no calibration data are available or the target quantization is float (no statistic needed)
    fquant [QUANT_OPTIONS] --json JSON_PATH

If the model is already quantized, i.e. the statistics are already in
the graph, the user can change the quantization options with:

.. code-block:: bash

    qtune --step STEP KEY1=VALUE1 KEY2=VALUE2 [...]
                # STEP: single step, a set of steps or a range to which apply
                # following QUANT_OPTIONS:
                # --step 1 use_ne16=True force_external_size=16
                # --step 2,3 hwc=True
                # --step 4:-1 use_ne16=True
                # or to apply to the whole graph:
                # --step * use_ne16=True
            --json JSON_PATH
                # Quant options can be stored in a json dictionary and provided here

To save the current quantization options in a dictionary and later
manually edit and pass to qtune or aquant --json:

.. code-block:: bash

    qtunesave --json JSON_PATH

NOTE: after running qtune/aquant might be necessary to run adjust/fusions
again (the tensor order might be changed)

Run Inference and Compare
^^^^^^^^^^^^^^^^^^^^^^^^^

Once the model has been quantized the user can run inference with a bit
accurate quantized backend with respect to the platform execution.
In this way the user can assert any accuracy drop in the quantization
process.

.. code-block:: bash

    dump path/to/files* [INPUT_OPTIONS]
            -q
            # Quantized mode (without -q it runs float32)
            -d
            # if quantized mode: dequantize the results such that
            # they are represented in the real space
            -S TENSOR_NAME
            # save the results in the workspace with TENSOR_NAME
            -P TENSOR_PICKLE
            # save the results in a pickle file

    qerror path/to/files* [INPUT_OPTIONS]
            # Automatically run in float and quantized and
            # compares layer by layer statistics

    # To compare two set of dump runs manually (i.e. float vs quantized)
    tensors -t TENSOR_NAME1 TENSOR_NAME2
            -s 10
            # To compare outputs of step 10, otherwise all
            -Q
            # QSNR
            -E
            # Absolute error

If the model has been generated with the Autotiler graph option DUMP_TENSOR,
the log of the run will contain the outputs of the layers.
NNTool can load the so dumped tensors with:

.. code-block:: bash

    tensors --gap_load path/to/logfile.txt

.. _nntool_section_attach_special_nodes:

Attach Special Nodes in front of the Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes you want to apply standard operations to the input of a network
which prepare the data coming from a sensor to the input of the NN. NNTool
supports few of these possible input preprocessors:
imageformat: attach a layer that can do image transposition HWC -> CHW on the cluster
with offset -128 in int8 or automatically output 16bits, useful in POW2 16bits models
resize: attach a resizer to the input of the network, useful for example if the
network is trained with HxW images but the sensor uses IN_HxIN_W

.. code-block:: bash

    imageformat INPUT_NODE
                    # Node to attach the formatter
                IN_FORMAT {bw16, bw8, rgb16, rgb565_rgb888, rgb888}
                    # Data Type coming in
                NORMALIZATION {offset_int8, out_int16, shift_int8}
                    # Operation applied to each op:
                    #    - offset_int8: subtracts 128 to every pixel [0:255]->[-128:127]
                    #    - out_int16: outputs int16 data
                    #    - shift_int8: shift >>1 every pixel: [0:255]->[0:127]

    input_resizer INPUT_NODE
                    # Node to attach the resizer
                  RESIZER_TYPE {bilinear, nearest}
                    # Algo used by the resizer
                  --from_w IN_W
                  --from_h IN_H
                    # Width and Height of the original input resized to the INPUT_NODE shape

AUDIO DSP:
dsp_preprocessing: attach a DSP preprocessing which can be of type Mfcc or RFFT. It computes
the operation to overlapped windows of the inputs.
NOTE: The Mfcc can do melspectrogram/logmelspectrogram/mfcc but no normalization is applied to
the output of the DSP node. If needed it should be included as part of the model after the input.

.. code-block:: bash

    logmel_config.json:
    {
        "window": "hanning",        # Window numpy function
        "pad_type": "center",       # How to apply padding if frame_size < n_fft
        "n_fft": 512,               # Number of FFT bins
        "n_frames": 401,            # Number of frames (it must match the graph input size)
        "frame_size": 512,          # Number of samples in each frame
        "window_size": 400,         # Number of samples for the window function (if < frame_size the window will be center padded)
        "frame_step": 160,          # Number of samples between two consecutive frames
        "sample_rate": 16000,       # Samplerate
        "magsquared": True,         # If True, the spectrogram is calculated as np.abs(rfft) ** 2, else np.abs(rfft)
        "n_fbanks": 128,            # Number of Mel Filterbanks (ignored when dsp_node_type=="fft")
        "fbank_type": "librosa",    # Algorithm used to generate the Mel Filters
        "mel_filterbanks_file": None, # Path to melfilters .npy array provided by user, in this case fbank_type will be ignored
        "n_dct": 0,                 # Number of DCT bins in case of Mfcc preprocessing
        "dct_type": 2,              # DCT algorithm type
        "fmin": 0.0,                # Fmin used to calculate the Mel Filters
        "fmax": None,               # Fmax used to calculate the Mel Filters
        "log_type": "natural",       # Logarithm type ("None", "db" or "natural"), None will output melspectrogram
        "log_offset": 1e-6          # Offset applied before log function
    }

    dsp_preprocessing INPUT_NODE
                        # Node to Attach the DSP node
                      {MFCCPreprocessingParameters, RFFT2DPreprocessingParameters}
                        # Type of preprocessing
                      --config_json logmel_config.json
                        # Config file with all the parameters

The code above emulates the librosa equivalent of:

.. code-block:: python

    import librosa
    melspect = librosa.feature.melspectrogram(y=audio,
                                              sr=16000,
                                              n_fft=512,
                                              hop_length=160,
                                              win_length=400,
                                              window="hann",
                                              center=False # This is important to not pad the whole audio input
                                              )
    logmelspect = np.log(melspect + 1e-6)

HINT: If using GAP9 the preprocessing in float16 is equally fast in
terms execution time as the fixed point, hence we suggest to qtune
this layer to be float16 to get better accuracy:

.. code-block:: bash

    qtune --step MfccPreprocessing scheme=float float_type=float32


NOTE: This feature is advanced and might not work in every possible
configuration. To have a more flexible feature we suggest to use
directly the autotiler Generator for MFCC/RFFT. You can find
examples in the sdk/examples

Save NNTool State
^^^^^^^^^^^^^^^^^

After several commands are run, you can save the nntool state in a json file.
Save the state of the transforms and quantization of the graph. This state file
can be used to generate the model file as part of a build script.
If no argument is given then the state files will be saved
in the same directory as the graph. If a directory is given then the
state files will be saved in it with the graph basename. If
a filename is given, its basename will be used to save the state
files.

.. code-block:: bash

    save_state FILE

It basically saves the sequence of commands run and the quantization
statistics.

Generate Autotiler Code
^^^^^^^^^^^^^^^^^^^^^^^

Once the model has been quantized and fused, the Autotiler model can be
generated:

.. code-block:: bash

    gen AT_MODEL
            -t, --output_tensors
            # write constants (weights, biases)
            -T, --tensor_directory TENSOR_DIRECTORY
            # path to tensor directory. full path will be created
            # if it doesn't exist. If this parameter is given it
            # willupdate the settings saved with the graph state.
            -M, --model_directory MODEL_DIRECTORY
            # path to model directory. full path will be created
            # if it doesn't exist. If this parameter is given it
            # will update the settings saved with the graph state.
            --basic_kernel_source_file BASIC_KERNEL_SOURCE_FILE
            # file to write to, otherwise output to terminal
            --basic_kernel_header_file BASIC_KERNEL_HEADER_FILE
            # file to write to, otherwise output to terminal

Scripting Mode
^^^^^^^^^^^^^^

All the commands to prepare a graph for Autotiler can be stored in a script
file and run with:

.. code-block:: bash

    nntool -s nntool_script

To generate the code directly from an nntool state:

.. code-block:: bash

    nntool -g AT_MODEL_PATH
            -M, --model_directory MODEL_DIRECTORY
            # path to model directory. full path will be created
            # if it doesn't exist. If this parameter is given it
            # will update the settings saved with the graph state.
            -m AT_MODEL_NAME
            # file name for autotiler model
            -T TENSOR_DIRECTORY
            # path to tensor directory. full path will be created
            # if it doesn't exist.

After having prepared the graph with some commands, the NNTool can
also generate a sample project, with all files dependencies and a
simple main application which runs on the platform the model with
unset input data. The model used in the project generated will
be prepared with a script written from the history of the current
NNTool shell:

.. code-block:: bash

    gen_profect PROJECT_FOLDER
                -o, --overwrite
                # overwrite existing files

Similarly the performance command will generate a project and
run the application from the python interface, getting performance
results out of gvsoc run:

.. code-block:: bash

    performance


Autotiler Optimization options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

NOTE: All the following options can be manually overwritten in the
generated Autotiler Model. They don't affect anyhow the NNTool
execution.

Node Options:

.. code-block:: bash

    nodeoption STEP OPTION=VAL

OPTION:

    - PARALLELFEATURES: In CHW mode convolution can be selected in
      with parallelization done in the features (default) or spatial dimension
    - ALLOCATE: input/output can be allocated in memory by the autotiler.
      This will spare L2 memory for other data in the application
    - TILEORIENTATION: tile computed vertically or horizontally (default)

Graph Options:

.. code-block:: bash

    set graph_const_exec_from_flash true/false
        # If on, constant symbols executes from home location
    set graph_dump_one_node NODE_NAME
        # Trace one specific graph node (name of the autotiler not NNTool)
    set graph_dump_tensor NUM
        # {4: output, 6: input and output, 7: input, output, constants}
        # Trace selected tensors arguments at inference time, either all nodes or selected node
    set graph_monitor_cvar_name NAME
        # When monitor cycles is on name of the C var array to receive results
    set graph_monitor_cycles true/false
        # Enable automatic cycle capture for each node of the graph
    set graph_name NAME
        # name of the graph used for code generation
    set graph_noinline_node true/false
        # If on, all user kernel function are marked as noinline
    set graph_pref_l3_exec {AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM}
        # In case a symbol must be allocated in L3 for execution this is the prefered memory
    set graph_pref_l3_home {AT_MEM_L3_HRAM, AT_MEM_L3_OSPIFLASH, AT_MEM_L3_QSPIFLASH, AT_MEM_L3_HFLASH, AT_MEM_L3_MRAMFLASH, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM,AT_MEM_L3_DEFAULTFLASH}
        # For constant symbols which L3 flash prefered memory
    set graph_produce_node_cvar_name true/false
        # When producing node names is on name of the C array receiving the names as strings
    set graph_produce_node_names true/false
        # Enable production of an array containing the name of each graph node
    set graph_produce_operinfos true/false
        # Enable production of number of macs for each layer
    set graph_produce_operinfos_cvar_name true/false
        # When Number of oper Infos is on name of the C array receiving mac infos for each node
    set graph_reorder_constant_in true/false
        # Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses
    set graph_size_opt OPT
        # 0: Default make opt, 1: O2 for all layers, Os for xtor,dxtor,runner, 2: Os for layers and  xtor,dxtor,runner
    set graph_trace_exec true/false
        # Enable trace of activity
    set graph_warm_construct true/false
        # Generate construct/destruct functions with the Warm option to only allocate/deallocate L1 buffer

Memory Options:

.. code-block:: bash

    set l1_size BYTE_SIZE
    set l2_size BYTE_SIZE
    set l3_size BYTE_SIZE
    set l3_flash_mb MBYTE_SIZE
        # Set the different memory levels size, used to generate the default Autotiler model Memories.
        # All of them can be overriden when calling the Autotiler
    set l2_ram_ext_managed true/false
        # Wether to let the autotiler allocate the L2 buffer by itself, in the model constructor (default: true)
    set l3_flash_device {AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM}
        # Which type of device will be used for the read/write memory level 3 device in the model. It will change the used driver to open the L3 RAM (default: AT_MEM_L3_HRAM)
    set l3_flash_ext_managed true/false
        # Wether to let the autotiler open the external L3 Flash and allocate the L3 Flash buffer by itself or the user will have to do it in the application, in the model constructor (default: true)
    set l3_ram_device {AT_MEM_L3_HFLASH, AT_MEM_L3_OSPIFLASH, AT_MEM_L3_QSPIFLASH, AT_MEM_L3_MRAMFLASH,AT_MEM_L3_DEFAULTRAM}
        # Which type of device will be used for the read-only memory level 3 device in the model. It will change the used driver to open the L3 FLASH (default: AT_MEM_L3_HFLASH)
    set l3_ram_ext_managed true/false
        # Wether to let the autotiler open the external L3 and allocate the L3 buffer by itself or the user will have to do it in the application, in the model constructor (default: true)
    set default_input_exec_location {AT_MEM_L2, AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM}
        # Where the autotiler should assume the input to be executed from (default: AT_MEM_L2)
    set default_input_home_location {AT_MEM_L2, AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM}
        # Where the autotiler should assume the input to be located to (default: AT_MEM_L2)
    set default_output_exec_location {AT_MEM_L2, AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM}
        # Where the autotiler should assume the output to be executed from (default: AT_MEM_L2)
    set default_output_home_location {AT_MEM_L2, AT_MEM_L3_HRAM, AT_MEM_L3_OSPIRAM, AT_MEM_L3_QSPIRAM,AT_MEM_L3_DEFAULTRAM}
        # Where the autotiler should assume the output to be located to (default: AT_MEM_L2)
