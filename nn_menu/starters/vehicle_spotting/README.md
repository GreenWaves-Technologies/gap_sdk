# Developing a Custom Visual Object Spotting system with the GAP *flow*


Deep Learning (DL) frameworks (e.g. TensorFlow) provide a fast way to develop robust image classifiers. 
The desing process leverages the collected data to train personalized detection pipelines, 
which can be embedded into portable low-power devices, such as GAP8.
In this context, the GAP *flow* is the missing piece between high-level DL frameworks and the processing engine GAP8.

To showcase the customization and embedding of a deep learning pipeline for visual object spotting into a GAP8-based smart sensor,
this project demonstrates the design steps in the figure below in case of image vehicle spotting. 
Nevertheless, you can follow the same steps to easily and fastly build your own smart camera system!


![image](./gapflow.png)

# Getting Started with Vehicles Spotting on a GAP8-based camera system
The repository includes a trained TF model for image vehicle spotting. 
To covert the model into C code with the GAP *flow* and run the application-code on a [GAP8-based camera system](https://greenwaves-technologies.com/store/): 
```
make clean all run [RGB=1 or 0]
```
The application works either with greyscale (default, Himax HM01B0) or RGB camera sensors (GalaxyCore GC0308); 
TF models are trained accrodingly on greyscale or RGB augumented data.

The application code runs also on the GAP8 software simulator [GVSOC](https://greenwaves-technologies.com/gvsoc-the-full-system-simulator-for-profiling-gap-applications/):
```
make clean all run platform=gvsoc [RGB=1 or 0]
```
where the visual pipeline is fed with a sample image loaded through JTAG (_images/COCO_val2014_000000000641.ppm_).


In the following, we detail the design steps to train and deploy a custom visual spotting model for vehicle detectecion.

## Requirements

### Model Training

> Tensorflow 1.13 
>
> Tensorflow Slim 

Concerning the TF Slim scripts, we refer to the committed version added as a submodule to the repository. 

### Model Deployment
> GapSDK 3.5+

## Table of contents:
  - [Dataset Preparation and DL Model training](#dataset-preparing-and-training)
  - [Deep Model Deployment on GAP8 with the GAP *flow*](#nn-on-platform-with-gapflow)
  - [Accuracy Validation with Platform Emulator](#validation-with-platform-emulator)
  
## Dataset Preparation and DL Model training

We refer to the open-source [Tensorflow Slim] framework (TF1.x) to build a custom image dataset and to train the DL model for visual image spotting.
With this tutorial, we will guide you through the usage of the TF framework to reproduce our results for vehicles spotting.
Specifically we: 

1. Build a dataset
2. Train a DL model, leveraging on quantization-aware training
3. Freeze the graph and convert to TFlite format

Afterwars, the GAP *flow* is fed with the tflite model.


### Download COCO and distill a custom dataset

The custom dataset for image vehicle spotting is distilled from the [COCO dataset](https://github.com/amikelive/coco-labels/blob/master/coco-labels-2014_2017.txt):
the presence of the target object - *vehiles* -  determines the new label of every image of the source dataset. 
In our case, the target object consists of a list of classes: 'bicycle', 'car', 'motorcycle', 'airplane', 'bus', 'train', 'truck', 'boat'.
The TF framework provides a script to automatically download (if not already done) 
and convert the COCO dataset into the new  dataset for image visual spotting, according to the TF-Record format:
```
python3 slim/download_and_convert.py --dataset_name=visualwakewords  \
                                     --dataset_dir=visualwakewords_vehicle  \
                                     --foreground_class_of_interest='bicycle','car','motorcycle','airplane','bus','train','truck','boat'\
                                     --small_object_area_threshold=0.05  \
                                     --download  \
                                     --coco_dir=coco  \
```
Arguments details:
- _dataset_name_: name of the dataset to download (one of "flowers", "cifar10", "mnist", "visualwakewords")
- _dataset\_dir_: where to store the dataset
in case of "visualwakewords":
- _foreground\_class\_of\_interest_: list of COCO object which you want to detect in images
- _small\_object\_area\_threshold_: minimum percentage of area of the interested object to promote the image to the visualwakewords label of true
- _download_: whether to download the entire coco dataset or not if already did it
- _coco\_dir_: if download=False where to store the coco dataset, if download=True where it is stored

Now, the dataset is ready to feed the training process. 

### Model Training

We select a MobileNetV2 to solve the image classification problem, 
with input resolution 224x224 input dimensions and width multiplier of 1 in case of grayscale images and 0.35 in case of RGB images. 
However, different models included in the TF SLim framework can be used as well.

A quantization-aware finetuning process takes places during the training phase. 
The model training is launched with:

```
python3 train_image_classifier.py \
	      --train_dir='vww_vehicle_train_grayscale' \
	      --dataset_name='visualwakewords' \
	      --dataset_split_name=train \
	      --dataset_dir='./visualwakewords_vehicle/' \
	      --log_every_n_steps=100 \
	      --model_name='mobilenet_v2' \
	      --checkpoint_path='./vww_vehicle_train_grayscale/' \
	      --max_number_of_steps=100000   \  
	      --num_clones=1   \
        --quantize_delay=90000  \
	      --use_grayscale
```
- _use\_grayscale_: (optional): to apply grayscale conversion to the dataset samples
- _train\_dir_: path to store checkpoints and training info
- _dataset\_name_: training dataset name
- _dataset\_split_: dataset partition to use for training
- _dataset\_dir_: path of dataset (TF-Records)
- _model\_name_: netowork architecture to train (_slim/nets/nets\_factory.py_ for a complete list of supported networks)
- _checkpoint\_path_: path of the folder contrianing the checkpoint files, if not defined the network is trained from scratch
- _max\_number\_of\_steps_: number of training steps
- _num\_clones_: number of GPU to use for training
- _quantize\_delay_: after how many steps the model is quantized

Since the target platform features optimized INT8 computational kernels, 
the quantization API of TensorFlow contrib has been replaced with its experimental version 
(_contrib\_quantize.experimental\_create\_training\_graph\_(symmetric=True)_). 
On the contrary, you can force: _--quantize\_sym=False_ .

### Model Evaluation

The model can now be evaluated the validation dataset:
```
python3 eval_image_classifier.py   \   
	      --checkpoint_path='vww_train_vehicle_grayscale/'   \   
	      --eval_dir='vww_eval_vehicle_grayscale/' \  
      	--dataset_split_name=val   \   
	      --dataset_dir='visualwakewords_vehicle/'  \  
      	--dataset_name='visualwakewords' \  
	      --model_name='mobilenet_v2' \
        --quantize  \ #if the model has been trained with quantization
	      --use_grayscale
```
The script computes the ratio of correct predictions, 
the number of false positives and false negatives. 
All these metrics can be inspected through tensorboard:
```
tensorboard --logdir='vww_eval_vehicle_grayscale'
```

To investigate the validation score at training time, 
we have added a bash script which evaluates the model validation accuracy at every epoch:
```
./train_eval_loop.sh -m ${MODEL_NAME} -b ${BATCH_SIZE} -e ${NUM_EPOCHS} -l ${LEARNING_RATE} -q ${QUANT_DELAY} -i ${IMAGE_SIZE} -g [to use grayscale] 
```
NOTE: dataset name and directory are hardcoded in the script. Change them accordingly in your project.

### Model export, freeze and conversion to tflite

To export the inference graph, i.e. the tensorflow graphdef file for the inference:
```
python3 slim/export_inference_graph.py
        --model_name=mobilenet_v2 \
        --image_size=224 \
        --output_file=./mobilenet_v2_224_grayscale.pb  \
        --quantize  \
        --use_grayscale
```
To obtain a frozen graph:
```
freeze_graph \
  --input_graph=./mobilenet_v2_224_grayscale.pb \
  --output_graph=./frozen_mbv2_224_grayscale.pb \
  --input_checkpoint=./vww_train_vehicle_grayscale/model.ckpt-100000 \
  --input_binary=true \
  --output_node_names=MobilenetV2/Predictions/Reshape_1
```

To inspect the graph and get the output_node_names you can use [Netron](https://lutzroeder.github.io/netron/)
Lastly, the TFLite is generated by means of:
```
tflite_converter --graph_def=./frozen_mbv2_224_grayscale.pb  \
                 --output_file=mbv2_grayscale.tflite  \
                 --input_arrays=input  \
                 --output_arrays=MobilenetV2/Predictions/Reshape_1  \
                 # if the model has been trained with quantization (suggested)
                 --inference_type=QUANTIZED_UINT8   \
                 --std_dev_val=128  \
                 --mean_val=128
```



## Deep Model Deployment on GAP8 with the GAP *flow*

The GAP *flow* converts a frozen TFlite model into a GAP-optimized C code. 

The GAP *flow* consists of a two-steps procedure.
Firstly, we use the __nntool__ to convert the high level tflite model description 
into an special graph description, which will be used by the __Autotiler__ tool. This 
latter leverages on the predictable memory access pattern 
of convolutional neural network to generate optimized C-code for the GAP platforms.


### nntool
This tool, besides producing the graph description needed by the __Autotiler__ 
to optimize the memory accesses, 
includes debug features to inspect the target DL model.

Initially, the tflite model is opened with nntool:
```
nntool mbv2_grayscale.tflite [-q to load also the quantization information]
show
```
_show_ will display the list of layers and the network topology. 
The model graph is now modified to match the __Autotiler__ execution model:
```
adjust
fusions --scale8
```
This two commands tranlsate the graph tensors from a HxWxC data layout (TF) to CxHxW data layout (GAP8 computational kernels). 
Moreover, fusions finds and replaces all the subgraph portions that the Autotiler can handle with a 
single optimized layer (e.g. Conv+Pool+Relu into one single layer).

__Only if the model was not quantized:__

The model runs inferences on several images, i.e. the calibration dataset, to estimate the activation quantization ranges (post-training quantization). 
Weights and bias quantization ranges are computed statically.
```
aquant -f 8 -s calibatrion_images/*
```
The nntool can also validate the accuracy of the model over a validation dataset:
```
validation dataset/* [-q to run with quantized kernels]
```
the default behaviour (see _--help_ for more information) interpretes the last character of 
the filename as the label (e.g. COCO_0000000_1.png means that the true value of the prediction is 1).
If the validation scores are poor, you can inspect the quantization log and see if the quantization error 
measured in terms of layer-wise QSNR (quantization signal-to-noise-ratio) wrt float values is too high: 
```
qerror image.png
```
NOTE: wnen running inference in nntool, the input must be preprocessed accordingly 
to what it is done in the training process. For example, in our case the model expects a [-1:1] input but 
input data belongs to the range [0:255], a normalization input function can be configured or selected among the ones available:
```
imageformat input_1 bw8 offset_int8
```

Lastly, the nntool state can be saved:
```
save_state
```
This will save the adjusted+fused+quantized nntool graph in a .json file beside all its parameters .nnparam. 
At this point you can generate the Autotiler Model code from the nntool saved state and the constants tensors 
files for weights and biases:
```
nntool -g model.json -m ATModel.c -T path/to/tensors/folder
```
### autotiler code generation

All the above steps are automatized in the common/model_rules.mk procedure, which also includes
the Autotiler model compilation and code generation. 
The Autotiler will generate 3 functions which you can place into your application code:
- __Graph Constructor__: when it is called allocates all the tensors needed by the graph in memory
- __Graph Run__: run the series of layers with optimal memory movements
- __Graph Destructor__: deallocate all the structures allocated by the Constructor

### Run the code on GAP

Now it is time to run visual infecences on a GAP8-based camera platform. 
Don't worry if you do not have one, 
Greenwaves provides a platform simulator [gvsoc](https://greenwaves-technologies.com/manuals/BUILD/GVSOC/html/index.html). 
You can take a loog to the application code in _main.c_ and run it 
```
make clean all run [RGB=0 or 1]
```
![Alt Text](demo_vehicle.gif)

## Accuracy validation in _\_\_EMUL\_\__ mode (Optional)

The Autotiler features an optional, yet very useful, run mode: the _\_\_EMUL\_\__ mode.  
If the flag is enabled, the Autotiler replaces all the GAP parallel code and built-in 
functions with x86 instructions, which can be executed by the host x86 PC. 
This speed-up the simulation time for functional testing on the host PC, with respect then using the GVSOC platform.
_main\_accuracy.c_ and _emul.mk_ leverages on this feature. 
_main\_accuracy.c_ gets a folder path and run infernce over the data samples (images) to return the total accuracy: 
```
make -f emul.mk clean all TEST_ACC=1 RGB=0 or 1 
./mobv2_vwwvehicle_quant_asym_emul /path/to/dataset/
```
NOTE: only non-compressed __.ppm__ images format is supported.
