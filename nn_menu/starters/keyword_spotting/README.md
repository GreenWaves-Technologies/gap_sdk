# Keyword Spotting

In this project you will find the deployment of a Keyword spotting application on Greenwaves-Technologies GAP processors. The algorithms are inspired by the [Google Speech commands example](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/examples/speech_commands) and [ARM KWS project](https://github.com/ARM-software/ML-KWS-for-MCU) adapted for GAP chips.

## Requirements

HARDWARE:
	
	- GAPUINO V2/V3 BOARD
	- Sensorboard (optional: if not available the application can run on standard .wav files)

SOFTWARE:
	
	- GAP sdk 3.8+

## Install the SDK

To run the application you have to install the GAP sdk before. Download it from (here)[https://github.com/GreenWaves-Technologies/gap_sdk] and follow the instruction in the README.md of the SDK to properly install and source.

## Training

The models topology are the DS-CNN architecture proposed in [Keyword spotting for Microcontrollers](https://github.com/ARM-software/ML-KWS-for-MCU), a CNN based on depth separable convolutional blocks. All the three versions of it are proposed: SMALL, MEDIUM and LARGE. They differ in number of parameters and number of operations but have the same topology. The models have been trained for 30k steps introducing quantization after 5k steps. In the (ML-KWS-for-GAP-training)[ML-KWS-for-GAP-training] you will find the scripts and the commands to reproduce the neural networks training.

The networks operate on Mel-frequency ceptral coefficients. Given the lack of FPU on Greenwaves GAP8 processor, a fully quantized algorithm for MFCC has been introduced and can be found (here)[https://github.com/GreenWaves-Technologies/gap_sdk/tree/master/tools/autotiler_v3/Generators/MFCC ]. All the models have been trained on the MFCC features derived from the module of the FFT of each window (|STFT|) and the power (|STFT|^2, have the suffix *_pow.tflite*). The latter result in better performance on the deployed model because do not rely on the sqrtfor the calculation of complex numbers modules (|a + ib| = sqrt(a^2 + b^2)) achieving the same accuracy. For this reason the power models are by default chosen for the deployment scripts. After the models have been trained and frozen, the tflite converter converts them to the tflite format and they are ready to be ported on the platform through the GAP flow.


## Deployment

The application is deployed through the usage of the GAP flow, a set of tools which allows the user to port a trained CNN graph into Greenwaves platforms. In particular this project uses nntool+Autotiler to convert a tflite graph into C code to run the quantized network on GAP processors. _Makefile_ contains all the rules necessary to convert the tflite into GAP code. To compile and launch the application (IMPORTANT: you have to source the sdk with the proper target - GAP8_V2 or GAP8_V3):

	source /path/to/gap_sdk/sourceme.sh
	make clean all run MEDIUM=1 (platform=gvsoc if you want to run on the SoC simulator)

By default the model will run on the .wav file specified in Makefile with WAV_PATH variable. If you have a Sensorboard you can run directly on real time data coming from the microphones specfying:

	make clean all run MEDIUM=1 USE_MIC_SENSORBOARD=1 (not available on virtual platform gvsoc)

With these settings the application will execute runtime with your voice applying the MFCC+Network every 0.5 seconds over 1 second frame lenght, i.e. 1 second windows with 50% overlap.

## Test accuracy

In this project we provide also the scripts to test the deployed model accuracy on the dataset of speech commands by google. To make this kind of testing fast and reliable, GAPflow provides the _EMUL_ mode. In this modality the generated code is compiled to run on x86 architectures and can run on standard PCs. To compile the program in this modality:

	make -f emul.mk clean all MEDIUM=1

To test the inference on a specific wav file:

	./kws_ds_cnn_emul path/to/file.wav

The inference is numerically equal to the one which run on the final target. You can use this feature to test the deployed model on the complete dataset:

	python3 utils/test_accuracy_emul.py --tflite_model $(TRAINED_TFLITE_MODEL) --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms) --test_with_wav $(WITH_MFCC) --use_power_spectrogram $(USE_POWER) --use_high_prec $(USE_HIGH_PREC)

## Results

In this section are presented the results of the deployed models in term of accuracy and power consumption.

Accuracy calculated with the testing partition of the (Google Speech Commands dataset)[https://www.google.com/search?client=opera&q=google+speech+commands+v2&sourceid=opera&ie=UTF-8&oe=UTF-8] with a testing percentage of 10%:

| Inference Engine <br /> Preprocessing | TFLite <br /> TF MFCC (float) | GAP <br /> GWT MFCC (int16)  | GAP <br /> GWT MFCC (High Precision int32) |
|------------------|-----------------|------------------|---------------------------------|
| DS_CNN SMALL     | 93.45%          | 92.86%           | 93.50%                          |
| DS_CNN MEDIUM    | 94.10%          | 93.99%           | 94.05%                          |
| DS_CNN LARGE     | 94.62%          | 94.09%           | 94.72%                          |

The timings and power measurements are calculated executing the MFCC+CNN over 1s windows with 50% overlap between consecutive windows:


| Model           | Preprocessing                   | Freq FC <br /> [MHz] | Freq CL <br /> [MHz] | Voltage <br /> [V] | T<sub>MFCC</sub> <br /> [ms] | T<sub>CNN</sub> <br /> [ms] | P<sub>MFCC</sub> <br /> [mW] | P<sub>CNN</sub> <br /> [mW] | P<sub>avg</sub> <br /> [mW] | 
|-----------------|---------------------------------|---------|---------|---------|-----------------|----------------|------|------|------|
| DS_CNN SMALL    | GWT MFCC (int16)                | 250     | 175     | 1.2     | 6.8             | 3.76           | 1.14 | 0.80 | 25.8 |
| DS_CNN SMALL    | GWT MFCC (int16)                | 10      | 10      | 1.0     | 119             | 68             | 1.41 | 0.81 | 4.8  |
| DS_CNN SMALL    | GWT MFCC (High Precision int32) | 250     | 175     | 1.2     | 18.5            | 3.76           | 4.33 | 0.80 | 28.4 |
| DS_CNN SMALL    | GWT MFCC (High Precision int32) | 10      | 10      | 1.0     | 327             | 68             | 4.33 | 0.84 | 6.9  |
| DS_CNN MEDIUM   | GWT MFCC (int16)                | 250     | 175     | 1.2     | 6.8             | 14.4           | 1.26 | 3.25 | 27.5 |
| DS_CNN MEDIUM   | GWT MFCC (int16)                | 10      | 10      | 1.0     | 119             | 253            | 1.33 | 3.29 | 6.5  |
| DS_CNN MEDIUM   | GWT MFCC (High Precision int32) | 250     | 175     | 1.2     | 18.5            | 14.4           | 4.31 | 3.33 | 30.2 |
| DS_CNN MEDIUM   | GWT MFCC (High Precision int32) | 10      | 15      | 1.0     | 220             | 176            | 3.69 | 2.89 | 8.3  |
| DS_CNN LARGE    | GWT MFCC (int16)                | 250     | 175     | 1.2     | 6.8             | 38             | 1.26 | 7.9  | 28.2 |
| DS_CNN LARGE    | GWT MFCC (int16)                | 10      | 20      | 1.0     | 62              | 361            | 1.04 | 6.74 | 9.2  |
| DS_CNN LARGE    | GWT MFCC (High Precision int32) | 250     | 175     | 1.2     | 18.5            | 38             | 4.31 | 7.84 | 33.8 |
| DS_CNN LARGE    | GWT MFCC (High Precision int32) | 10      | 25      | 1.0     | 134             | 301            | 3.31 | 6.42 | 11.2 |
