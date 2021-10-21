# Occupancy Management on Gap8 with Lynred Thermeye B120

This repository contains a reference platform for indoor occupancy management that runs on Greenwaves [GAPoC_B evaluation board](https://greenwaves-technologies.com/product/gappoc-b-occupancy-management-reference-platform/).

To see how to install and use our reference platform you can refer to this [video](https://www.youtube.com/watch?v=NDgnNjm8TaI)

## Repository Content Description:

1. **3D Case**       : CAD STL files for 3D case printing
2. **apk**           : Android apk that connects to GAPoC_B Ble to shows detections.
3. **dataset_tools** : This project is meant to retrieve an offset image for you Thermeye camera sensor. Tools to collect a dataset with the board and to filter images.
4. **gap8_project**  : Gap8 Application source code
5. **ssd_train**  : CNN training and fine tuning

The dataset used to train the Neural network algorithm can be found [here](https://gwt-website-files.s3.eu-central-1.amazonaws.com/occupancy_management_dataset.zip).

## Getting Started

### Prerequisites

To compile and run the code you need to install the [GAP SDK](https://github.com/GreenWaves-Technologies/gap_sdk). Follow the readme at this repository install it:

https://github.com/GreenWaves-Technologies/gap_sdk

If you do not have a GAPoC_B board and you want to run the demo you can try it with input and output images from the host pc using the GVSoC simulator that comes within the sdk.

Before running the code you have to source the sourceme file contained in the sdk. If you have a GAPoC B board (or run it on GVSoC):

```
 $ source [root_sdk_folder]/configs/gapoc_b_v2.sh
```

To download the precompiled shutterless library (only needed to run demo using input from Thermal Camera) you have to run make in the root folder of this repository:

```
 $ make all
```

You will be asked to enter your details and will receive a link to copy/paste in your shell. 

### Simple run of the application

To run the application:

```
cd gap8_project
make clean all run
```

If you do not have a board and want to test it on GVSoC you can simply add this flag to the make command:

```
cd gap8_project
make clean all run platform=gvsoc
```

Once the application has executed in the folder dump_out_imgs you can see the result of the execution and the detected bounding boxes

## Full Application Deploy

### Offset image for your Lynred Thermeye

The Lynred Thermeye needs an offset image to properly filter the input images to feed the NN algorithm. Every Thermeye sensor has his own offset, which is different due to fabrication process of the bolometers. The folder dataset_tools/gap8_capture_raw_images contains a sample project to collect this offset and some sample images to test the Calibration Image

Before running the project find a black, flat, cold object, for example the back of a smartphone. You need to use this object to cover the Thermeye lens.

To run the example
```
cd dataset_tools/gap8_capture_raw_images
make clean all run
```

The project will tell you when to cover the Thermeye lens. After the execution the offset image is saved in the offset_img folder under the name Calibration.bin.

To copy it to the gap8 project folder:

```
cp Calibration.bin ../../gap8_project/
```

### Android Application

The folder apk contains an android application that shows the output in real time of the people detected with the GAPoC board. To install on you android smartphone/tablet, you have to enable unknown sources apk installation, copy the application to the device and install it.

### Gap8 Application

The folder gap8_project contains the source code for the gap8 application.At the beginning of the Makefile there are few variables that permits to configure the demo in different modalities. You can comment/uncomment them to enable/disable different application features.

If you want to run the standalone application you have to copy the Calibration.bin file (see step: "Offset image for your Lynred Thermeye"). If instead you want that the offset image is taken at every boot you can enable it uncommenting this flag:

```
##  To take an image as reference offset at every boot you can use the following flags:
GAP_FLAGS += -DOFFSET_IMAGE_EVERY_BOOT=1
```

To enable the Bluetooth Low Energy on GAPoC board that communicates with the android application uncomment this line:

```
##  To enable Ble communication (with the demo android app):
GAP_FLAGS += -DUSE_BLE
```

If you want to flash the board to boot from the embedded flash, you have to follow these 2 steps. This permits you to flash the board once and turn it on and off without having to reload the code from your PC.

1. Burn the efuse to boot from flash.

Move the selector one on the board to position close (some leds should turn on). This enables the 2.5 V to properly power supply the gap8 for efuse burning. Then run the following script:

```
cd gap8_project
./burn_boot_hyperflash_efuse.sh
```

2. Configure the Makefile as follows:

```
## Comment these two lines
#GAP_FLAGS += -DINPUT_FILE
#SAVE_TO_PC=1

## Uncomment these two lines:
GAP_FLAGS += -DSILENT=1
GAP_FLAGS += -DUSE_BLE
```



## Dataset Collection

In the folder dataset_collection you have two subfolders that contain:

1. A tool to collect offset and dataset images in subfolder gap8_capture_raw_images
2. A filtering library to analyze and filter the collected dataset

## 3D Case Printing

In the folder 3D_case you have two STL files that you can use to 3d print a case for the GAPoC B board.


