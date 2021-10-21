# Keyword spotting for GAP 

This repository consists of the tensorflow models and training scripts used 
in the paper: 
[Hello Edge: Keyword spotting on Microcontrollers](https://arxiv.org/pdf/1711.07128.pdf) adapted for Greenwaves-Technologies Toolset (GAPflow). 
The scripts come from [Tensorflow examples](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/examples/speech_commands) 
and some are repeated here for the sake of making these scripts self-contained.

We focused on the ds_cnn models family which achieved the higher accuracy in the KWS task.
The training scripts are adapted to support quantization aware training for such models targeting the GAPflow,
for instance, a symmetric quantization scheme is used to target the deployed convolutional kernels.
Also, for the preprocessing we used MFCC features. From version 3.8 the gap sdk supports this kind of preprocessing
in both schemes:
    - STFT -> Spectrogram -> MelFilterBank -> Log -> (optional) DCT
    - STFT -> Power Spectrogram -> MelFilterBank -> Log -> (optional) DCT

Since the second is more efficient from a computational point of view, we extended the input_data.py script
to support also this (USE_POWER variable line 40).

## How to train your model

To train a DNN with 3 fully-connected layers with 128 neurons in each layer, run:

```
python train.py --model_architecture dnn --model_size_info 128 128 128 
```
The command line argument *--model_size_info* is used to pass the neural network layer
dimensions such as number of layers, convolution filter size/stride as a list to models.py, 
which builds the tensorflow graph based on the provided model architecture 
and layer dimensions. 
For more info on *model_size_info* for each network architecture see 
[models.py](models.py).
The training commands with all the hyperparameters to reproduce the models shown in the 
[paper](https://arxiv.org/pdf/1711.07128.pdf) are given [here](train_commands.txt).

To run inference on the trained model from a checkpoint on train/val/test set, run:
```
python test.py --model_architecture dnn --model_size_info 128 128 128 --checkpoint 
<checkpoint path>
```

To freeze the trained model checkpoint into a .pb file, run:
```
python freeze.py --model_architecture dnn --model_size_info 128 128 128 --checkpoint 
<checkpoint path> --output_file dnn.pb
```

Once you froze your model you can convert it to tflite and feed the GAPflow, read below for the tflite converter commands.

## Reproduce our results

Here we present the accuracy achieved by the ds_cnn family with both power and spectrogram mfcc features in their different size settings and the commands that reproduce them:

## Power MFCC

### SMALL

    python train.py  --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize 5000 --learning_rate 0.0005,0.0001,0.00002 --how_many_training_steps 10000,10000,10000 --summaries_dir checkpoints/work_q_sym/DS_CNN_S_104020/retrain_logs --train_dir checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/training
    python freeze.py  --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize True --start_checkpoint checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/training/best/ds_cnn_q_9329.ckpt-12800 --output_file checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/ds_cnn_q_frozen_quant.pb --output_file  checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    python test_tflite.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --tflite_model checkpoints/work_q_sym_new_mfcc/DS_CNN_S_104020/ds_cnn_q_frozen_quant.tflite

    PB
     Test:  93.35%                                            Validation: 93.03%
      [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
       [  0 350   5   5   6  12   5   3   6   2   7   7]       [  1 322   4   7   2   6   9   4   3   2   7   4]
       [  0  10 399   2   0   0   6   0   0   0   0   2]       [  0   3 385   1   0   1   2   1   0   0   0   4]
       [  0   5   2 389   0   2   3   0   0   0   0   4]       [  1  11   2 375   2   2   1   1   0   1   2   8]
       [  0   3   0   0 390   1   2   0   3  17   6   3]       [  0   5   0   0 317   2   1   0   1  12  10   2]
       [  0  10   1   7   0 374   0   0   0   0   1  13]       [  0   4   0  10   0 353   2   0   0   0   2   6]
       [  0   7  10   0   0   1 388   5   0   0   1   0]       [  0   8  11   1   1   0 323   6   0   1   0   1]
       [  0  14   0   0   1   1   7 370   0   0   1   2]       [  0   7   0   0   0   0   1 352   0   0   0   3]
       [  1   9   0   0   2   4   0   1 360  13   2   4]       [  1   8   0   0   6   0   0   0 337   7   0   4]
       [  1   3   0   2   8   0   2   0   6 375   0   5]       [  0   4   0   0  23   0   1   0   6 330   7   2]
       [  0   3   0   1   4   3   0   0   0   1 394   5]       [  2   1   0   0   7   0   1   0   1   1 335   2]
       [  0   9   0  13   0   6   2   0   0   1   3 368]]      [  0   8   2   9   2   6   1   0   1   3   3 337]]

    TFLITE
    Test:  93.45%                                             Validation:  92.91%
    [[408   0   0   0   0   0   0   0   0   0   0   0]        [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  0 356   3   6   6   6   5   7   3   2   7   7]         [  0 325   0   7   0   9   6   8   4   2   5   5]
     [  0  13 392   3   0   0  10   0   0   0   0   1]         [  0   7 376   4   0   0   5   1   0   0   0   4]
     [  0   6   1 387   1   2   3   0   0   0   0   5]         [  0   9   2 369   1   3   2   4   0   0   0  16]
     [  0   7   0   0 392   0   1   0   4  11   6   4]         [  0   5   1   0 320   3   1   1   0   8   7   4]
     [  0  10   2  11   1 369   0   0   0   0   2  11]         [  0   4   1   9   0 354   0   2   0   0   2   5]
     [  0   5   7   1   2   0 389   7   0   0   1   0]         [  0  10   7   0   0   0 326   7   0   0   2   0]
     [  0  10   0   0   2   1   1 380   0   0   1   1]         [  0   5   0   1   0   0   2 354   0   0   0   1]
     [  0  10   0   0   4   3   0   0 371   2   3   3]         [  1   6   0   0   6   0   1   1 340   6   0   2]
     [  1   9   0   1  10   0   3   0   8 361   0   9]         [  0   2   0   0  23   0   0   0  11 325   7   5]
     [  0   2   0   1   5   3   0   0   0   0 394   6]         [  1   3   0   0   8   0   0   0   1   1 334   2]
     [  0   6   0  14   1   4   3   1   0   0   3 370]]        [  0  11   1   8   3   7   0   0   2   1   4 335]]


### MEDIUM

    python train.py  --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize 5000 --learning_rate 0.0005,0.0001,0.00002 --how_many_training_steps 10000,10000,10000 --summaries_dir work_q_sym/DS_CNN_M_104020/retrain_logs --train_dir work_q_sym_new_mfcc/DS_CNN_M_104020/training 
    python freeze.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize True --start_checkpoint checkpoints/work_q_sym_new_mfcc/DS_CNN_M_104020/training/best/ds_cnn_q_9442.ckpt-19600 --output_file checkpoints/work_q_sym_new_mfcc/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file checkpoints/work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb --output_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    python test_tflite.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --tflite_model checkpoints/work_q_sym_new_mfcc/DS_CNN_M_104020/ds_cnn_q_frozen_quant.tflite

    PB
     Test:  94.01%                                          Validation:  94.42%
     [[408   0   0   0   0   0   0   0   0   0   0   0]     [[408   0   0   0   0   0   0   0   0   0   0   0]
      [  0 365   7   3   3   3   6   6   3   2   3   7]      [  0 365   7   3   3   3   6   6   3   2   3   7]
      [  0   3 409   3   1   1   2   0   0   0   0   0]      [  0   3 409   3   1   1   2   0   0   0   0   0]
      [  0   6   1 387   1   3   2   0   0   0   0   5]      [  0   6   1 387   1   3   2   0   0   0   0   5]
      [  0   9   0   0 385   4   0   0   6  11  10   0]      [  0   9   0   0 385   4   0   0   6  11  10   0]
      [  0   5   2  13   0 369   2   0   0   0   3  12]      [  0   5   2  13   0 369   2   0   0   0   3  12]
      [  0   8   3   2   0   0 397   2   0   0   0   0]      [  0   8   3   2   0   0 397   2   0   0   0   0]
      [  0  12   0   0   0   3   1 377   1   0   2   0]      [  0  12   0   0   0   3   1 377   1   0   2   0]
      [  0   6   0   0   2   7   0   0 363  12   2   4]      [  0   6   0   0   2   7   0   0 363  12   2   4]
      [  0   3   0   0   6   1   1   0   8 374   3   6]      [  0   3   0   0   6   1   1   0   8 374   3   6]
      [  0   2   1   0   3   3   0   0   1   0 398   3]      [  0   2   1   0   3   3   0   0   1   0 398   3]
      [  0   3   1  21   0   3   1   1   3   2   2 365]]     [  0   3   1  21   0   3   1   1   3   2   2 365]]

    TFLITE
     Test:  94.01%                                          Validation:  94.17%
     [[408   0   0   0   0   0   0   0   0   0   0   0]     [[371   0   0   0   0   0   0   0   0   0   0   0]
      [  0 364   4   8   2   4   5   7   6   1   3   4]      [  1 338   0   4   3   8   4   6   0   2   4   1]
      [  0   6 407   4   0   0   1   0   0   0   1   0]      [  0   4 387   0   0   2   2   0   0   0   0   2]
      [  0   5   0 388   1   5   2   0   0   0   0   4]      [  0  10   0 373   0   9   1   2   0   0   3   8]
      [  0   7   0   0 392   4   0   0   5  10   6   1]      [  0   4   1   0 321   0   2   0   2  10   9   1]
      [  0   5   4  13   0 374   0   0   2   0   1   7]      [  0   3   0   8   0 359   0   0   0   0   2   5]
      [  0   8   3   3   0   0 396   2   0   0   0   0]      [  0   1   7   2   0   0 339   3   0   0   0   0]
      [  0  13   0   0   0   2   4 375   1   0   1   0]      [  0   6   1   1   0   2   0 353   0   0   0   0]
      [  0   9   0   0   3   9   0   0 361  11   2   1]      [  1   5   0   2   4   1   0   1 342   6   1   0]
      [  0   6   0   0   9   1   0   0   7 373   1   5]      [  0   5   0   1  19   1   1   1   3 337   2   3]
      [  0   3   1   0   3   4   0   1   1   2 396   0]      [  1   2   0   0   8   1   0   0   0   2 336   0]
      [  0   5   0  23   0   5   2   0   2   2   1 362]]      [  0   9   0  18   2   6   0   0   2   2   4 329]]

### LARGE

    python train.py  --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize 5000 --learning_rate 0.0005,0.0001,0.00002 --how_many_training_steps 10000,10000,10000 --summary_dir checkpoints/work_q_sym_new_mfcc/DS_CNN_403010/retrain_log --train_dir checkpoints/work_q_sym_new_mfcc/DS_CNN_403010/training/ 
    python freeze.py --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --start_checkpoint checkpoints/work_q_sym_new_mfcc/DS_CNN_403010/training/best/ds_cnn_q_9563.ckpt-26400 --output_file checkpoints/work_q_sym_new_mfcc/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file checkpoints/work_q_sym_new_mfcc/DS_CNN_403010/ds_cnn_q_frozen_quant.pb --output_file checkpoints/work_q_sym_new_mfcc/DS_CNN_403010//ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619


    PB
     Test:  95.99%                                           Validation:  95.43%
     [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
      [  0 367   1   4   3   4   8   7   4   1   4   5]       [  0 341   3   1   3   4   4   6   1   2   4   2]
      [  0   1 411   3   0   0   1   0   0   1   0   2]       [  0   4 388   1   0   2   2   0   0   0   0   0]
      [  0   1   3 395   0   2   2   0   0   0   1   1]       [  0   7   0 385   3   1   2   2   1   0   1   4]
      [  0   6   0   0 397   1   0   0   3  14   3   1]       [  0   3   1   1 324   0   6   0   0   9   5   1]
      [  0   3   0   5   0 389   0   0   3   1   0   5]       [  0   1   1   6   0 364   1   0   0   0   1   3]
      [  0   3   4   0   0   1 404   0   0   0   0   0]       [  0   1   2   1   0   0 346   2   0   0   0   0]
      [  0   8   0   1   0   1   0 384   0   1   1   0]       [  0   3   0   2   0   0   1 356   0   0   0   1]
      [  0   8   0   0   2   2   0   0 370  11   2   1]       [  0   5   0   3   4   2   0   0 340   7   0   2]
      [  0   1   0   1  10   0   0   0   2 384   0   4]       [  0   2   0   2  15   0   2   0   6 341   3   2]
      [  0   0   0   0   1   2   0   0   1   1 403   3]       [  0   1   2   1   5   2   0   0   1   2 336   0]
      [  0   1   0   9   0   3   1   1   1   2   2 382]]      [  0   6   1   5   0   6   0   0   0   2   2 350]]

    TFLITE
     Test:  95.92%                                           Validation:  95.25%
     [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
      [  0 368   1   3   2   4   9   8   3   1   4   5]       [  0 338   5   1   0   5   4   5   2   4   4   3]
      [  0   2 414   0   0   0   1   0   0   1   0   1]       [  0   3 388   1   0   1   3   0   0   0   0   1]
      [  0   4   2 387   0   3   3   0   0   0   1   5]       [  0   8   1 384   2   3   2   1   0   1   0   4]
      [  0   6   0   0 400   0   0   0   4  11   3   1]       [  0   4   0   1 325   0   2   0   1   8   8   1]
      [  0   5   1   5   0 387   1   0   0   1   1   5]       [  0   2   3   7   0 360   1   0   0   0   0   4]
      [  0   3   3   0   0   0 405   1   0   0   0   0]       [  0   2   2   1   0   0 346   0   0   0   1   0]
      [  0   7   0   1   0   0   0 386   0   1   1   0]       [  0   1   0   3   0   0   1 357   0   1   0   0]
      [  0   7   0   0   1   3   0   0 368  15   2   0]       [  0   6   1   1   4   2   0   1 341   6   0   1]
      [  0   2   0   0  15   0   0   0   1 380   0   4]       [  0   2   0   1  18   0   3   1   7 338   2   1]
      [  0   0   0   0   0   2   0   0   1   0 407   1]       [  0   1   1   0   6   3   0   0   2   2 334   1]
      [  0   3   0   8   2   4   1   1   0   0   3 380]]      [  0   7   1   4   1   3   0   0   1   1   3 351]]



## Spectrogram MFCC
### SMALL

    python train.py  --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize 2000 --summary_dir work_q_sym/DS_CNN_104020/retrain_log --train_dir work_q_sym/DS_CNN_104020/training/ --learning_rate 0.0005,0.0002,0.0001 --how_many_training_steps 2000,1000,1000 
    python freeze.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize True --start_checkpoint work_q_sym/DS_CNN_104020/training/best/ds_cnn_8949.ckpt-5000 --output_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.pb --output_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    # TEST
    python test_pb.py     --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize True --graph work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.pb
    python test_tflite.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --tflite_model work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.tflite

    PB
    Test:  Accuracy: 90.27%                                 Validation:  Accuracy: 90.71%
    [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  2 323   6  11   6  12  12   7   7   2   7  13]        [  1 299   4   7   6  13  12   6   8   1   6   8]
     [  1  12 385   7   0   1  10   1   0   0   1   1]        [  2   6 371   6   0   1   8   1   0   0   0   2]
     [  1   5   2 370   0  16   4   1   0   0   0   6]        [  2  16   2 364   0   5   1   0   3   0   2  11]
     [  2  12   0   0 384   1   2   0   6  12   3   3]        [  0   7   0   0 317   1   2   1   2  11   7   2]
     [  1  14   2  14   0 354   5   0   3   1   1  11]        [  2   5   1  17   0 341   2   0   0   0   2   7]
     [  2   4  10   1   1   0 386   6   0   1   1   0]        [  0  13  11   1   2   1 316   4   0   0   4   0] 
     [  2  15   0   0   1   1   5 371   1   0   0   0]        [  0  11   3   1   0   0   3 343   0   1   0   1]
     [  3  11   2   0   5   5   0   0 356  12   0   2]        [  3  11   0   0   5   1   0   0 333   9   0   1]
     [  2   7   0   0  24   1   1   1   5 349   2  10]        [  1   5   0   0  16   0   0   1  13 332   2   3]
     [  2   4   0   0  12   5   0   0   1   2 382   3]        [  5   7   0   0  10   2   0   0   1   4 320   1]
     [  3  13   0  30   0   5   1   1   0   3   0 346]]       [  1  15   0  13   1  10   0   1   1   4   1 325]]


    TFLITE
    Test: Accuracy: 89.22%                                  Validation: Accuracy: 89.24%
    [[408   0   0   0   0   0   0   0   0   0   0   0]       [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  2 321   1   9   6  13  17   6   8   1  10  14]        [  1 287   3   8   6  14  13  13  10   1   6   9]
     [  1  12 375   7   0   1  21   1   0   0   0   1]        [  1  10 360   9   0   6  10   0   0   0   0   1]
     [  0   7   3 353   1  22   4   1   0   0   1  13]        [  2  18   2 340   3  16   1   4   3   0   1  16]
     [  2  16   0   1 383   1   2   0   4   7   8   1]        [  0   4   0   0 319   1   5   2   1  10   6   2]
     [  3  20   0  11   0 353   4   0   3   0   3   9]        [  1   9   0  16   0 330   3   1   0   0   5  12]
     [  2   3   9   3   2   0 384   7   1   0   1   0]        [  0  10   8   2   1   1 322   5   0   0   3   0]
     [  1  14   1   0   1   1   7 369   0   0   1   1]        [  0  11   2   0   0   0   3 343   0   1   0   3]
     [  3  13   1   0   5   4   0   0 352  14   1   3]        [  2  13   0   0   5   2   0   0 332   8   1   0]
     [  2   8   0   0  28   1   4   2   8 335   5   9]        [  0   6   0   1  25   0   2   1  12 314   6   6]
     [  2   4   0   0  10   8   0   0   1   0 383   3]        [  3   6   0   0  10   2   0   0   1   3 322   3]
     [  3  12   0  19   1  11   2   0   0   4   4 346]]       [  1  13   0  11   1  11   3   1   3   1   1 326]]


### MEDIUM

    python train.py  --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize 5000 --learning_rate 0.0005,0.0001,0.00002 --how_many_training_steps 10000,10000,10000 --summaries_dir work_q_sym/DS_CNN_M_104020/retrain_logs --train_dir work_q_sym/DS_CNN_M_104020/training 
    python freeze.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize True --start_checkpoint work_q_sym/DS_CNN_M_104020/training/best/ds_cnn_q_?????? --output_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb --output_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    python test_pb.py     --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize True --graph work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb
    python test_tflite.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --tflite_model work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.tflite

    TFLITE
    Test: Accuracy: 92.69%                                  Validation: Accuracy: 92.7%
    [[408   0   0   0   0   0   0   0   0   0   0   0]     [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  0 355   3   7   6   6   8   4   4   0   7   8]      [  1 326   4   4   2   5   4   5   3   2   6   9]  
     [  0  10 401   0   0   0   8   0   0   0   0   0]      [  0   3 381   2   1   0   9   0   0   0   0   1]  
     [  0   3   0 382   0  12   3   0   0   0   0   5]      [  1   9   3 378   1   5   1   0   0   0   1   7]  
     [  0  14   0   0 392   1   1   0   2   5  10   0]      [  0   2   1   0 321   2   3   0   0  10   9   2]  
     [  0   7   2  15   1 367   1   0   2   0   1  10]      [  1   3   1  11   0 351   1   0   0   0   1   8]  
     [  1   5   3   1   0   0 395   4   0   1   2   0]      [  0   2  11   1   1   0 332   2   0   0   1   2]  
     [  0  17   0   0   1   1   4 370   1   0   1   1]      [  0   8   1   0   0   1   4 346   0   0   1   2]  
     [  0  11   0   0   8   6   0   0 359   6   2   4]      [  2   8   0   1   4   1   0   1 338   8   0   0]  
     [  0   9   0   1  25   0   1   0   5 354   2   5]      [  1   2   0   1  22   1   2   0   7 325   9   3]  
     [  0   5   0   0   3   3   1   0   0   0 394   5]      [  2   2   0   0  11   0   1   1   1   0 330   2]  
     [  0   8   1  22   2   9   1   0   0   0   4 355]]     [  0   7   1  19   2  14   0   1   0   3   4 321]]   

### LARGE

    python train.py  --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --summary_dir work_q/DS_CNN_403010/retrain_log --train_dir work_q/DS_CNN_403010/training/ --learning_rate 0.0005 --how_many_training_steps 3000 
    python freeze.py --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --start_checkpoint /tmp/speech_commands_train/best/ds_cnn_q_8980.ckpt-800 --output_file work_q/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q/ds_cnn_q_frozen_quant.pb --output_file work_q/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    # TEST
    python test_pb.py     --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --graph work_q/DS_CNN_403010/ds_cnn_L_403010_frozen.pb 
    python test_tflite.py --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --tflite_model work_q/DS_CNN_403010/ds_cnn_L_403010_quant.tflite 

    PB
    Test:  Accuracy: 93.87%                                 Validation:  Accuracy: 93.97%
    [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  1 361   2   8   1   4   6  11   4   1   6   3]       [  1 324   2   7   2   6   8  10   3   0   6   2]
     [  1   8 404   2   1   0   2   0   0   0   1   0]       [  1   5 386   3   0   0   2   0   0   0   0   0]
     [  0   5   0 397   0   3   0   0   0   0   0   0]       [  2   6   2 390   0   2   1   0   0   0   0   3]
     [  2  11   0   0 392   0   0   1   1  10   7   1]       [  0   7   1   1 322   1   3   1   0   7   6   1]
     [  1   5   1  16   0 379   1   1   0   0   0   2]       [  1   2   1  11   0 359   1   0   0   0   1   1]
     [  1   6   2   0   0   0 401   2   0   0   0   0]       [  0   6   5   3   0   0 334   4   0   0   0   0]
     [  0  11   0   0   0   0   1 383   0   0   1   0]       [  0   6   1   0   0   0   1 355   0   0   0   0]
     [  1  14   0   0   5   5   1   0 354  16   0   0]       [  2   7   1   0   4   2   0   0 340   7   0   0]
     [  1  11   0   3  11   1   0   0   5 367   0   3]       [  1   5   0   0  19   0   0   0   6 337   4   1]
     [  0   7   0   0   1   2   2   0   0   1 398   0]       [  1   6   0   0   6   2   0   0   1   1 333   0]
      [  3  17   0  26   1   8   0   1   0   1   1 344]]      [  0  12   0  16   1  13   1   0   0   1   2 326]]
    
    TFLITE
    Test:  Accuracy: 92.77%                                 Validation:  Accuracy: 92.75%
    [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  1 337   0  12   3   5  15  16   5   2   7   5]       [  0 300   3   9   1   4  13  23   3   1  13   1]
     [  1   3 405   0   1   0   7   0   0   0   2   0]       [  2   4 384   1   0   0   6   0   0   0   0   0]
     [  0   5   0 390   0   5   1   0   0   0   3   1]       [  2   5   3 379   0   5   5   1   0   0   1   5]
     [  1   5   0   0 389   0   3   1   1  11  14   0]       [  1   5   1   1 315   1   4   1   0   8  12   1]
     [  1   9   1  24   0 358   5   1   0   0   5   2]       [  1   1   2  10   0 350   2   0   0   0  10   1]
     [  1   2   2   1   0   0 405   1   0   0   0   0]       [  0   3   2   2   0   0 340   5   0   0   0   0]
     [  1   5   0   0   0   1   6 381   0   0   2   0]       [  0   5   3   0   0   0   1 353   0   0   0   1]
     [  2  12   0   0   3   4   1   0 346  23   3   2]       [  2   7   2   0   4   0   3   0 329  15   0   1]
     [  1   6   0   3  16   1   3   0   1 364   3   4]       [  1   1   1   0  20   0   3   2   2 334   7   2]
     [  0   2   0   0   0   0   3   0   0   0 406   0]       [  3   0   0   0   6   0   0   2   0   2 337   0]
     [  2  16   1  18   1   6   2   1   0   1   7 347]]       [  0   7   1  14   2  10   1   0   0   2   5 330]]


