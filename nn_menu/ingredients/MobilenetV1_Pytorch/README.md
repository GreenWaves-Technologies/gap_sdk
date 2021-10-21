# MobilenetV1 Imagenet Classification on GAP8 

This reference design aims at demonstrating the end-to-end deployment of a state-of-the-art 
deep netowork, such as a quantized INT8 MobilenetV1, on the GAP8 platfom. 
Throuought this project we show:
* How to generate C code for the infernce task with quantized network network
* How to run a NN inference task on GAP8 chip
* How to train and dump a INT8 quantized network from Pytorch

## Getting Started

### Requirements
To run this example code, the SDK tag > 3.1 must be installed from [here](https://github.com/GreenWaves-Technologies/gap_sdk),
optionally including the [Autotiler] (https://github.com/GreenWaves-Technologies/gap_sdk#getting-the-autotiler) tool.
For training or dumping the network model, Pytorch==1.1 with GPU support must be installed.

### Running the inference task
You can run the compile the code and run the inference task on a GAPUINO board ith the command:
~~~~~shell
make clean all run
~~~~~
To avoid reloading data on the FLASH memory at every run:
~~~~~shell
make clean all run ALREADY_FLASHED=1
~~~~~
Alternatively, the code can run on the [virtual platform]
(https://github.com/GreenWaves-Technologies/gap_sdk#using-the-virtual-platform):
~~~~~shell
make run platform=gvsoc
~~~~~
If the Autotiler tool is installed, the MobileNetV1 C code can be regenereted by:
~~~~~shell
make clean model
~~~~~
### Dumping MobilenetV1 graph description from Pytorch
~~~~~shell
python3 dump_mobilenet_to_GAP8.py --ckp_file './fake_quant_models/imagenet/mobilenetv1_224_1.0/checkpoint.pth.tar' --img_file './image144.jpeg'
~~~~~

### Quantization-aware retraining
To replicate the quantization-aware retraining flow, you can type:
~~~~~shell
cd training/
python3 main.py -a mobilenet --mobilenet_width 1.0 --mobilenet_input 224 --save Imagenet/mobilenet_224_1.0 --dataset imagenet --type_quant 'PerLayerSymPACT' --weight_bits 8 --activ_bits 7 --activ_type learned --gpus 0,1,2,3 -j 32 --epochs 12 -b 128 --save_check --quantizer --batch_fold_delay 1 --batch_fold_type folding_weights
~~~~~
More details can be found [here].

## Performance
The table below details the performance when running a MobilenetV1 model scoring 70% on Imagenet on a GAPUINO board.


| #Layer  | Cycles    | Operation | Operation/Cycle |
|---------|-----------|-----------|-----------------|
| Layer0  |   6141809 |  11239424 | 1.829986        |
| Layer1  |   5075473 |   4014080 | 0.790878        |
| Layer2  |   9202907 |  25690112 | 2.791521        |
| Layer3  |   4558109 |   2007040 | 0.440323        |
| Layer4  |   6022202 |  25690112 | 4.265900        |
| Layer5  |   4900303 |   4014080 | 0.819149        |
| Layer6  |  20289308 |  51380224 | 2.532379        |
| Layer7  |   2276100 |   1003520 | 0.440895        |
| Layer8  |   6693301 |  25690112 | 3.838183        |
| Layer9  |   1932933 |   2007040 | 1.038339        |
| Layer10 |   8886530 |  51380224 | 5.781809        |
| Layer11 |   1723533 |    501760 | 0.291123        |
| Layer12 |   4125530 |  25690112 | 6.227106        |
| Layer13 |    304888 |   1003520 | 3.291438        |
| Layer14 |   8568675 |  51380224 | 5.996286        |
| Layer15 |    286597 |   1003520 | 3.501502        |
| Layer16 |   8573150 |  51380224 | 5.993156        |
| Layer17 |    268446 |   1003520 | 3.738256        |
| Layer18 |   8529251 |  51380224 | 6.024002        |
| Layer19 |    280675 |   1003520 | 3.575381        |
| Layer20 |   8554171 |  51380224 | 6.006453        |
| Layer21 |    299234 |   1003520 | 3.353630        |
| Layer22 |   8554028 |  51380224 | 6.006553        |
| Layer23 |    108525 |    250880 | 2.311725        |
| Layer24 |   4446018 |  25690112 | 5.778229        |
| Layer25 |    199611 |    501760 | 2.513689        |
| Layer26 |  11182318 |  51380224 | 4.594774        |
| Layer27 |     61225 |     50176 | 0.819534        |
| Layer28 |   4661205 |   1024000 | 0.219686        |
|         |   		  |           |                 |
| Total   | 146706055 | 571123712 | 3.892980        |
