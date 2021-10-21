# MCUNet on GAP

## Original repository

ONNX model is converted from PyTorch model which is both with quantized TFLite version are taken from the original
[MCUNet repo](https://github.com/mit-han-lab/tinyml/tree/master/mcunet).
We are using [mcunet-512kb-2mb_imagenet.pth](https://hanlab.mit.edu/projects/tinyml/mcunet/release/mcunet-512kb-2mb_imagenet.pth)
and [mcunet-512kb-2mb_imagenet.pth](https://hanlab.mit.edu/projects/tinyml/mcunet/release/mcunet-512kb-2mb_imagenet.tflite) respectively.

## Supported Models

Depending on your needs, onnx model (which requires further quantization) and quantized tflite models are supported.

## Quantization Data

If you'd like to run onnx model on GAP, you first need to download data to quantize the model on.
To do so simply run:
```
./download_quant_data.sh
```

## Inference On GAP8

You can run the application on GAP8 (or platform simulator gvsoc).
If you want to use onnx model, run:

```
make clean all run platform=gvsoc MODEL_FORMAT=onnx
```
And for tflite model the command will change to:

```
make clean all run platform=gvsoc MODEL_FORMAT=tflite
```

## Evaluation

For evaluation process we suggest to use emulator as it tremendously increase its speed.
To rebuild the project with it, navigate to `accuracy_on_validation` folder:
```
cd accuracy_on_validation
```
Then, you will need to create a symbolic link to quantization data by running:
```
ln -s ../quant_data_ppm/ quant_data_ppm
```
and finally run:
```
make all -f emul.mk MODEL_FORMAT=onnx
```
or
```
make all -f emul.mk MODEL_FORMAT=tflite
```
depending on chosen model.

To evaluate the model you will need to dump the results on the properly preprocessed data which is transformed the same way it was for the original model.
Download evaluation data by running the appropriate script:
```
./download_eval_data.sh
```
And then to preprocess it, run:
```
python3 preprocess_gap_images.py --val-dir imagenet_gap/val  --out-dir preprocessed_data
```
After that you can dump the results with:
```
python3 get_gap_results.py --in_dir preprocessed_data --out_dir gap_results 
```
Next step is to evaluate the obtained output for each image with the command below:
```
python3 eval_gap_results.py --val-dir imagenet_gap/val --gap-dir gap_results
```

The comparison metrics table for all of the models is the following:
| MODEL | Top-1 Accuracy, % | Top-5 Accuracy, % |
|-------|------------------------|------------------------|
| MCUNet 512kB-2MB PyTorch Float32  |  68.2 |  88 |
| MCUNet 512kB-2MB ONNX Float32  |  68.09 |  87.97 |
| MCUNet 512kB-2MB TFlite Int8  |  67.68 |  87.64  |
| MCUNet 512kB-2MB ONNX GAP Int8  |  67.04 |  87.20 |
| MCUNet 512kB-2MB TFlite GAP Int8  |  67.07 |  87.25 |
