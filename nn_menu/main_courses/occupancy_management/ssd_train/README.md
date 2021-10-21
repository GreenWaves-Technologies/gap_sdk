# Neural Network Model: dataset, training and fine tuning

The repository contains 3 Jupiter Notebooks:

1. **data_division_process** : to divide dataset into subsets (i.e., train, validation, etc.).
2. **main_model**: to train a sample SSD model.
3. **main_model_fine_tuning**: to load weights of a pre-trained model and follow training process.


## Prerequisites

Download the dataset: 
```
$ wget https://gwt-website-files.s3.eu-central-1.amazonaws.com/occupancy_management_dataset.zip
$ unzip data.zip
```

Install Python Dependencies

```
$ pip3 install -r requirements.txt
```


## Data division

To open the jupiter notebook type:

```
$ jupiter-notebook data_division_process.ipynb
```


This notbook divides the dataset into the following subsets:


|        set        | percentages (%) | starting | ending | num_samples | selection precentages |
| --- | --- | --- | --- | --- | --- | --- |
|     train         |        70       |    0     |  4948  |     4949    |          0.7          |
|     val           |        10       |   4949   |  5655  |     707     |          0.1          |
|     test          |        5        |   5656   |  6008  |     353     |          0.05         |
| train fine-tuning |        5        |   6009   |  6361  |     353     |          0.05         |
|  val fine-tuning  |        5        |   6362   |  6714  |     353     |          0.05         |
|  test fine-tuning |        5        |   6715   |  7067  |     353     |          0.05         |


## Model Training

To open the jupiter notebook type:

```
$ jupiter-notebook main_model.ipynb
```


This notebook contains all the datails for model training and export to a "base_model" that can be deployied into the gap8 Project.

## Model Fine Tuning

To open the jupiter notebook type:

```
$ jupiter-notebook main_model_fine_tuning.ipynb
```


This is a brief tutorial on how to fine tune a pre-trained main-model. This assumes that the new model parameters are in correspondence with main-model configurations. The fine tuning process is similar to the main-model training, but starts from an already trained model.




