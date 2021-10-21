# Accuracy Validation tool

This section describes the tool which evaluates the deployed networks accuracy over the Imagenet dataset. In the accuracy_on_validation_data you will find all you need to do so:

	cd accuracy_on_validation_data

The \_\_EMUL\_\_ mode is a particular Autotiler setting in which the generated code is compiled to target x86 architectures instead of the GAP RISCV one. In this way you can run the same code which runs on the platform and check the accuracy on a large set of saved images in a very short time (you do not have to flash one by one to the board).

After compiling the main_emul.c with all its dependences with the usage of [emul.mk](./emul.mk) Makefile, an executable file named as your target model followed by "\_emul" is generated. To run it you need to specify the path to the dataset directory on which you want to validate your model accuracy, e.g.:

    ./mobilenet_v1_1_0_224_quant_emul /path/to/your/dataset

## Dataset Requirements

To work properly the application needs the dataset to be divided in folders, one for each class, named with the corresponding class Imagenet [synset](dataset_conversion/tf_synset_name.txt):

```
    dataset
        ├── n01440764
            ├── images for this class
        ├── n01443537
        ├── n01484850
        ├── ...
        ├── n15075141
```

Since the [main file](./main_emul.c) uses the GAP functionalities, the images needs to be in the raw format (.ppm) and already resized for the target network architecture. With the default Imagenet validation dataset downloaded you can convert to be used in this application with:

    cd dataset_conversion
    python3 ppm_dataset_generator.py -D -H 224 -W 224 --original_dataset_path path/to/imagenet/val --converted_dataset_path imagenet/converted/


## Computing Top1 Accuracy on TFLite models
To compute the accyuracy of a tflite model on the converted dataset:

    python3 test_tflite.py ../path/to/folder/tflite/models/ tflite_model_name path/to/dataset

e.g. python3 test_tflite.py ../models/tflite_models/ mobilenet_v1_1_0_224_quant dataset_conversion/imagenet224/


