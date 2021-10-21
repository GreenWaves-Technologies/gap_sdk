# NN Menu

The **Neural Network Menu\*** is a collection of software that implements Neural Networks on Greenwaves Application Processors (GAP). This repository contains common mobile and edge NN archtecture examples, NN sample applications and full flagged reference designs. Our tools map a TFLITE model (quantized or unquantized) onto gap. There is also a flow in the ingredients directory showing how to hand map from a Pytorch Model onto GAP. 

For a detailed description of the content of each project please refer to the readme inside the project folder. 

## Getting Started 

To run the code in this repository you need to install and source the [gap sdk](https://github.com/GreenWaves-Technologies/gap_sdk).

Once the sdk is installed source the sourceme.sh in the root sdk folder and retrieve this repository with a:

```
git clone --recurse-submodules -j4 git@github.com:GreenWaves-Technologies/nn_menu.git
```

## Content of the repository 

The repository is divided into 4 different folders:

#### **ingredients**
Optimized models for common mobile and edge use cases. This is a playground to start with, it shows how well known networks topologies are mapped onto gap.

Content of the folder:
- Image Classification Networks (several versions of Mobilenet V1, V2, V3 minimalistic, full V3 to come)
- kws (Google Keyword Spotting)
- Mobilenet V1 from Pytorch Model

These examples take image from file with semihosting in input and output the results through the shell.

#### **starters**
Use Case specific examples that use specific datasets and shows nn running for a specific task. 

Content of the folder:
- Body Detection (custom CNN) 
- Face Detection (custom CNN) 
- People Spotting (NN from [MIT Visual Wakeup Words](https://github.com/mit-han-lab/VWW))
- Vehicle Spotting (Customization and embedding of a deep learning pipeline for visual object spotting)
- Licence Plate Recognition (Mobilenet SSDLite + LPRNet)

These applications take an input from file with semihosting and output the results trough shell, they also run on our boards to be tested with input from drivers. For specific cameras configrations please check the readme in within each projects folder.  

#### **main courses**
Full flagged applications (aka reference designs) running on [GAPoC series boards](https://greenwaves-technologies.com/store/).

- ReID (on GAPoC A)
- Occupancy Management (on GAPoC B)

These applications take image from file with semihosting in input and output the results trough shell, these mode run in gvsoc and all boards. Then they have flags in makefile to enable input/output with driver. Each of them is designed for a specific board as a reference design. 


#### **sides**

Other tools, this is empty for the moment.


## Futures Releases

We are actively working in the area of RNN, LSTM and GRU. Next releases will contain new repository that will be demoing it. If you have any suggestion or willing please contact us at <support@greenwaves-technologies.com>


\* We are a french Company, so we care about food. The team is composed from people all over the world, that's why we can laugh about it :-) 
