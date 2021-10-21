# Licence Plate Detection and Recognition

The project implements an image-based Deep Learning pipeline to detect license plates and read the registration number. 
The algorithm is based on a 2 steps inference model:
* Mobilenet SSD-Lite to detect License plates within greyscale 320x240 images
* LPRNet to read the regristration number from 94x24 Licese Plates crops 

Models are invoked in sequence to run the full pipleine. 

## Getting Started

To generate the code for the two models:

	make -f ssd.mk clean model
	make -f lprnet.mk clean model

After both models are generated, to run the full-application:

	make clean all run

## AT Optimization

The utils/call_at.py script launches the Autotiler engine with different L2 memory configurations and plots the trend of several metrics to explore the optimal configuration:

	python3 utils/call_at.py SSD
and/or

	python3 utils/call_at.py LPR 

## Test nntool models

Once you generated the models you can test the results in NNtool.

For the LPRnet:

	nntool BUILD_MODEL_LPR/lprnet.json
	set input_norm_func 'x:x/128-1' [there is not a formatter so you need to specify a normalization]
	dump images/china_1_cropped.ppm -q -S nntool --mode RGB -T
	run_pyscript utils/get_char_from_nntool.py [to see the predicted characters]

For the SSD model:
	
	nntool BUILD_MODEL_LPR/lprnet.json
	set input_norm_func 'x:x/128-1' [there is not a formatter so you need to specify a normalization]
	dump images/china_1.ppm -q -S nntool --mode RGB -T -v

NOTE: 
- -q: quantized inference like GAP
- --mode RGB: convert to RGB (model wants 3 channels images)
- -T: transpose the RGB images to meet the CHW execution order of nntool/Autotiler
- -v: allows the user to visualize the detected bounding boxes

## Test tflite models

The utils folder contains two Python scripts (lprnet_tflite_inference.py and ssd_tflite_inference.py) to test the tflite quantized models:

	python3 lprnet_tflite_inference.py [path/to/image]

if not specified the test images will be taken from the images folder.

## Debug

To test the two models separately:

	make -f ssd.mk clean all run
	make -f lprnet.mk clean all run

The result from the ssd model can be copied and pasted in the utils/DrawBB.py script to see the detected bounding box.

Both models can be run in \_\_EMUL\_\_ mode for debugging purpose with:

	make -f emul.mk clean all MODEL=1
	./lprnet_emul images/image.ppm > log.txt
or

	make -f emul.mk clean all MODEL=2
	./ssdlite_ocr_emul images/image.ppm > log.txt
