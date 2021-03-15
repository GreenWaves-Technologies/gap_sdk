Keyword Spotting Application
============================

#About
This directory contains the files for keyword spotting application test and the performance measurements on GAP for the keyword spotting application (kws).
THis applications requires a GAPUINO evaluation board connected to a PC, and the installation of the standard GAP SDK.

For testing the kws application a reducced set of the test dataset V0.2 from Pete Warden (described in the paper  arXiv:1804.03209v1) is used.

The complete test set is stored in http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz


1/ KWS application description

The Keyword spotting application ported on GAP uses  a feature extractor based on MFCC (Mel Cepstrum Coefficients  Computation), followed by a three layers convolutive neural network.

It has been ported from the floating point python version of the speech_command (https://github.com/tensorflow/tensorflow/tree/master/tensorflow/examples/speech_commands ), after quantization of the MFCC part, and  translation of the CNN part using the TF2GAP8 bridge.

THe MFCC generates 40x98 features from a 1 second  speech frame sampled at 16khz.
The CNN has 2 convolutive layers and one fully connected layer.
the first layer takes one feature input of size 40x98 and generates 32 output features of size 16x39.
The second layer generates 32 features of size 13x30. The last layer generates 12 outputs.
THe decision is the maximum value of the 12 outputs.

The kws recognizes 10 words (yes, no, up, down, left, right, on,  off, stop, go) plus "silence" and "none" (when the word is not in the list of 10).

The convolution kernels are of size 8x20 for the first layer and 6x10 for the second.

The amount of coefficients to store in the GAP L2 memory is (32*8*20 + 32*32*6*14 + 32*13*30*12 ) = 240896 .

The amount of fixed point computations is (8*20*40*98 + 32*32*6*14*16*39 + 32*13*30*12) = 54Million multiply accumulate operations.

The application available in this repository performs the CNN in 2.7Million cycles using the hardware convolution engine (HWCE)  accelerator available in GAP.
It consumes 300uW to process one frame of 1sec.


2/ measurement of the CNN performances on GAP8

The default configuration is set to use the hardware convolutional engine (HWCE) accelerator available on GAP.
To run a keyword spotting test on a single  waveform (DataTest.h) using the HWCE, do:

~~~~~shell
> make clean all run
~~~~~

The log file of the execution shows the consumed number of cycles and the produced results is the following:

~~~~~log
=========================================================
Loading binary through jtag
Notifying to boot code that we are doing a JTAG boot
Found ftdi device i:0x403:0x6010:0
Connecting to ftdi device i:0x403:0x6010:0
Loading binaries
Loading section (base: 0x1b000000, size: 0x23c)
Loading section (base: 0x1b00023c, size: 0x924)
Loading section (base: 0x1c000000, size: 0x5450)
Loading section (base: 0x1c005450, size: 0x44)
Loading section (base: 0x1c005494, size: 0xc8)
Loading section (base: 0x1c00555c, size: 0x3cd18)
Starting execution
MFCC done, features generated.


CNN launched on the HWCE
Layer0:  831290 Cycles,   53053 instruments
Layer1: 1740001 Cycles,  230244 instruments
Layer2:   90221 Cycles,   62806 instruments
 ============> CNN power consumption at 1.2V : 353 µWatt (at 1V 296 µWatt)

	ffffff88
 feat 0: -120
	3b
 feat 1: 59
	ffffffca
 feat 2: -54
	48
 feat 3: 72
	fffffffb
 feat 4: -5
	57
 feat 5: 87
	ffffffac
 feat 6: -84
	fffffffc
 feat 7: -4
	0
 feat 8: 0
	ffffffcf
 feat 9: -49
	d
 feat 10: 13
	55
 feat 11: 85

found max 6

found word down
Detected end of application, exiting with status: 0

==============================================================
~~~~~

THe log gives (in addition to the performance figures) the soft values of the 12 outputs of the cnn, of which the maximum is taken as the found word.

To deactivate the hardware CE (and perform the computation in the 8 cores of the cluster),
comment the following line in the Makefile:

~~~~~shell
#USE_HARDWARE_CE = -DRT_HAS_HWCE=1
~~~~~

and run the test again:
~~~~~shell
> make clean all run
~~~~~

The cycle counts will display:

cluster master start
CNN  launched on cluster (8 cores)
Layer0: 3664012 Cycles, 3336229 instruments
Layer1: 3614180 Cycles, 3299926 instruments
Layer2:   84446 Cycles,   58486 instruments
 ============> CNN power consumption at 1.2V : 4123 µWatt (at 1V 2473 µWatt)

The soft outputs are equivalent the HWCE version (with +/- 1 approximation)

3/ measurement of the correct detection ratio on the reference test set

An iterative test is available. It loads .wav waveforms from the dataset converts to an .h file and runs the kws program on GAP8.A partial dataset is available in "reduced_test_set" directory

It measures the number of correct detections over a reduced set of data.

to run the test, do:

~~~~~shell
> source ./test.sh  ./reduced_test_set/testing_list_small_small.txt 0
~~~~~

first argument is the path to the testset, and the other is the number of test that must be skipped


4/ coefficients quantization

The floating point weights and biases are located in the tf2gap8_weights directory.

3 quantization modes are available (quantization layer1:layer2:layer3)

~~~~~shell
-DFULL_PREC : 16:16:16
-DW_CHAR  16:8:8
-DW_HALFCHAR 16:8:4
~~~~~

The corresponding set of coefficients are located in the directory weights_bias.

to regenerate the quantized coefficients: do

~~~~~shell
> gcc -DW_HALFCHAR ../read_float_weights.c -o read_float -lm
> ./read_float
~~~~~

Set the quantization parameter in the Makefile before running the test in the line (in this line, DW_HALFCHAR mode is used):

~~~~~shell
PULP_CFLAGS += -O3 -Wno-unused-but-set-variable  -Wno-overflow -Wno-sign-compare -DW_HALFCHAR -DTILED -DHWCE_ACTIVE  -DDOMFCC
~~~~~

The software mode execution supports the 3 quantization modes

The result on the reduced test in W_HALFCHAR is 86%
THe size of the binary file is 169KB

In the HWCE enabled execution mode, the quantization is 16:16:4, and it cannot be modified.


5/ Repository description

CNN_kernels : contains the computation kernels for the HWCE (CnnKernels_hwce) and for the software execution (CnnKernels).

libs : contains the library of functions used for the MFCC (MFCC.c) and the CNN (CNN_HWCE and CNN_BasicKernels).

include : contains the header files for the CNN_kernels functions

include_libs : contains the header files and constant tables used in the libs functions

reduced_test_set contains the waves dataset from the speech command dataset used in the iterative test for recognition ratio measurement

tf2gap8_data:  contain the data generated by the TF2GAP8 bridge, including the floating point coefficients.

weights_bias: contains the quantized weights and biases for the 3 available software quatization modes, and for the HWCE mode.

in the keyword_spotting_del repo:
main.c is the main file
Makefile is the makefile
test.sh is the script for iterative testing using the .wav files in the reduced dataset (reduced_test_set)

read_weightd_hwce.c is the source code to quantize the floating point coefficients for the hwce.
DataTest.h is the input speech waveform ( table of constants loaded in the L2 memory)

