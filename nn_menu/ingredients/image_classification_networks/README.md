# Image Classification Models on GAP

This repository showcases the GAP *flow* for Image Classification models inference. 
Specifically, the GAP *flow* code generation applies on quantized state-of-the-art deep models in TFLite format for [visual object classification](https://www.tensorflow.org/lite/guide/hosted_models).
Besides the *nntool* scripts needed to import TFLite graph, we include the application code to run inference on [gvsoc](https://greenwaves-technologies.com/gvsoc-the-full-system-simulator-for-profiling-gap-applications/) or [GAP8 boards](https://greenwaves-technologies.com/store/).

The code is intended as a fast-access point to Greenwaves Technologies tools for model bechmarking and as a starting point for development of edge intelligence on GAP8.


## Supported Models
Below the models tested with the GAP *flow*. More will come shortly...

| MODEL ID | Quantized TFLite Graph | MACs (M) | Parameters (M) | Top 1 Accuracy | FPS | Energy (mJ) |
|-----|------------------------|----------|----------------|----------------|----------|----------|
| 0        |[MobileNet V1 224 1.0](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz)  | 569 | 4.24 | 70.1 | 2.1	 | 49.7 |	
| 1        |[MobileNet V1 192 1.0](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_192_quant.tgz)  | 418 | 4.24 | 69.2 | 3.1	 | 35.2 |	
| 2        |[MobileNet V1 160 1.0](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_160_quant.tgz)  | 291 | 4.24 | 67.2 | 4.4	 | 25.1 |	
| 3        |[MobileNet V1 128 1.0](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_128_quant.tgz)  | 186 | 4.24 | 63.4 | 7.2	 | 15.2 |
| 4        |[MobileNet V1 224 0.75](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_224_quant.tgz)| 317 | 2.59 | 66.8 | 4.0	 | 27.6 |	
| 5        |[MobileNet V1 192 0.75](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_192_quant.tgz)| 233 | 2.59 | 66.1 | 5.3	 | 20.9 |	
| 6        |[MobileNet V1 160 0.75](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_160_quant.tgz)| 162 | 2.59 | 62.3 | 8.5	 | 13.3 |	
| 7        |[MobileNet V1 128 0.75](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_128_quant.tgz)| 104 | 2.59 | 55.8 | 12.9 |  8.5 |	
| 8        |[MobileNet V1 224 0.5](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_224_quant.tgz)  | 150 | 1.34 | 60.7 | 8.2	 | 13.6 |	
| 9        |[MobileNet V1 192 0.5](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_192_quant.tgz)  | 110 | 1.34 | 60.0 | 11.5 |  9.6 |	
| 10       |[MobileNet V1 160 0.5](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_160_quant.tgz)  | 77  | 1.34 | 57.7 | 16.1 |  6.8 |	
| 11       |[MobileNet V1 128 0.5](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_128_quant.tgz)  | 49  | 1.34 | 54.5 | 23.8 |  4.4 |	
| 12       |[MobileNet V1 224 0.25](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_224_quant.tgz)| 41  | 0.47 | 48.0 | 25.2 |  4.4 |	
| 13       |[MobileNet V1 192 0.25](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_192_quant.tgz)| 34  | 0.47 | 46.0 | 34.1 |  3.2 |	
| 14       |[MobileNet V1 160 0.25](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_160_quant.tgz)| 21  | 0.47 | 43.4 | 47.4 |  2.3 |	
| 15       |[MobileNet V1 128 0.25](http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_128_quant.tgz)| 14  | 0.47 | 39.5 | 66.3 |  1.5 |	
| 16       |[MobileNet V2 224 1.4](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_140.tgz)              | 582 | 6.06 | 75.0 | 1.4	 | 67.4 |	
| 17       |[MobileNet V2 224 1.0](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_100.tgz)              | 300 | 3.47 | 71.8 | 3.4	 | 32.2 |	
| 18       |[MobileNet V2 192 1.0](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_100.tgz)              | 221 | 3.47 | 70.7 | 4.7	 | 23.7 |	
| 19       |[MobileNet V2 160 1.0](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_100.tgz)              | 154 | 3.47 | 68.8 | 6.9	 | 16.1 |	
| 20       |[MobileNet V2 128 1.0](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_100.tgz)              | 56  | 3.47 | 60.3 | 10.3 | 10.4 |	
| 21       |[MobileNet V2 96 1.0](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_100.tgz)                | 99  | 3.47 | 65.3 | 16.3 |  6.3 |	
| 22       |[MobileNet V2 224 0.75](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_75.tgz)              | 209 | 2.61 | 71.8 | 4.6	 | 23.8 |	
| 23       |[MobileNet V2 192 0.75](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_75.tgz)              | 153 | 2.61 | 70.7 | 6.1	 | 17.9 |	
| 24       |[MobileNet V2 160 0.75](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_75.tgz)              | 107 | 2.61 | 68.8 | 9.3	 | 12.0 |	
| 25       |[MobileNet V2 128 0.75](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_75.tgz)              | 69  | 2.61 | 65.3 | 12.9 |  8.2 |	
| 26       |[MobileNet V2 96 0.75](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_75.tgz)                | 39  | 2.61 | 58.8 | 21.5 |  4.7 |	
| 27       |[MobileNet V2 224 0.5](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_50.tgz)               | 97  | 1.95 | 71.8 | 8.9	 | 12.6 |	
| 28       |[MobileNet V2 192 0.5](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_50.tgz)               | 71  | 1.95 | 70.7 | 11.7 |  9.2 |	
| 29       |[MobileNet V2 160 0.5](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_50.tgz)               | 50  | 1.95 | 68.8 | 15.5 |  6.8 |	
| 30       |[MobileNet V2 128 0.5](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_50.tgz)               | 32  | 1.95 | 65.3 | 22.5 |  4.5 |	
| 31       |[MobileNet V2 96 0.5](https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_50.tgz)                 | 18  | 1.95 | 75.8 | 32.8 |  2.9 |	
| 32	   |[MobileNet V3 Large Minimalistic](https://storage.googleapis.com/mobilenet_v3/checkpoints/v3-large-minimalistic_224_1.0_uint8.tgz)| 209 | 3.9 | 71.3 | 4.2	 | 23.7 |


Supported models have been tested on GAP8 RevC by setting: FC clk=250MHz, CL Freq=175MHz, Vdd=1.2V, L2 Memory Used = 350kB






## Download the models
A Mobilenet TFLite model can be found in the models/tflite_models folder. 
To download other tflite hosted models you can run the script:

	./models/download_models.sh

# Requirements

* [GAPsdk](https://github.com/GreenWaves-Technologies/gap_sdk) 3.5+ 

# Inference On GAP8

Once you downloaded your model and sourced the GAPsdk, you can run the application on GAP8 (or platform simulator *gvsoc*):

	make clean all run [platform=gvsoc] MODEL_ID=XX [FREQ_FC=YY FREQ_CL=ZZ HAVE_CAMERA=1 HAVE_LCD=1]

MODEL_ID  select the target model (as detailed in the table). 
The FREQ_FC and FREQ_CL are, respectively, the Fabric Controllers and Clusters frequencies expressed in MHz.

If HAVE_CAMERA is not set, the application loads a [sample image](images) from file to feed the network.


# Project structure
- [main.c](main.c): application code running on GAP8
- [common](common):
	- headers: contains the header files needed by the functions generated by the AutoTiler for each model;
	- model_rules.mk: contains the common rules to control the GAP *flow* with makefiles;
	- model_decl.mk: defines the common paths needed for the model and GAP *flow* compilation and run;
- [images](images): contains sample images for each input_size model.
- [models](models):
	- nntool_scripts: here you can find the nntool scripts used for the AT model generation. For MobileNet V1 and V2 we developed specific nntool scripts which leverage the Autotiler features to optimize the performances;
	- tflite_models: the makefile structure suppose the tflite models files in this directory;
	- download_models.sh: the script to automatically download all the tflite quantized hosted models.
- accuracy_on_validation_data: this folder contains everything related to the accuracy check of the model on the target GAP chip. It uses the \_\_EMUL\_\_ mode supported by the GAP*flow*. The usage of this tool is explained in its relative directory [documentation](accuracy_on_validation_data/README.md).
- [common.mk](common.mk): defines the topology parameters which will be passed as macro defines to your application code.

