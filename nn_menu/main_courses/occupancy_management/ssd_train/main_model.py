### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##############################################################################

# 1. General requirements 
import os
import keras
from keras.optimizers import Adam, SGD
from keras.callbacks import ModelCheckpoint, EarlyStopping, ReduceLROnPlateau, TerminateOnNaN, CSVLogger
from keras import backend as K
from keras.models import load_model
from math import ceil
import numpy as np
from matplotlib import pyplot as plt
from models.ssd_model import build_models
from keras_loss_function.keras_ssd_loss import SSDLoss
from keras_layers.keras_layer_AnchorBoxes import AnchorBoxes
from keras_layers.keras_layer_DecodeDetections import DecodeDetections
from keras_layers.keras_layer_DecodeDetectionsFast import DecodeDetectionsFast
from ssd_encoder_decoder.ssd_input_encoder import SSDInputEncoder
from ssd_encoder_decoder.ssd_output_decoder import decode_detections, decode_detections_fast
from data_generator.object_detection_2d_data_generator import DataGenerator
from data_generator.data_augmentation_chain_constant_input_size import DataAugmentationConstantInputSize

##############################################################################
# 2. Set model configuration parameters 
img_height = 80 # Height of the input images
img_width = 80 # Width of the input images
img_channels = 1 # Number of color channels of the input images
intensity_mean = 0 # Set this to your preference (maybe `None`). The current settings transform the input pixel values to the interval `[-1,1]`.
intensity_range = 256 # Set this to your preference (maybe `None`). The current settings transform the input pixel values to the interval `[-1,1]`.
n_classes = 1 # Number of positive classes
scales = [0.05, 0.15, 0.3, 0.4, 0.6] # An explicit list of anchor box scaling factors. If this is passed, it will override `min_scale` and `max_scale`.
batch_size = 8
# The list of aspect ratios for the anchor boxes
aspect_ratios_global = None
aspect_ratios_per_layer = [[1./4., 1./3., 1./2., 1., 2., 3., 4.],
                           [1./4., 1./3., 1./2., 1., 2., 3., 4.],
                           [1./4., 1./3., 1./2., 1., 2., 3., 4.],
                           [1./4., 1./3., 1./2., 1., 2., 3., 4.]]
two_boxes_for_ar1 = True # Whether or not you want to generate two anchor boxes for aspect ratio 1
steps = None # In case you'd like to set the step sizes for the anchor box grids manually; not recommended
offsets = None # In case you'd like to set the offsets for the anchor box grids manually; not recommended
clip_boxes = False # Whether or not to clip the anchor boxes to lie entirely within the image boundaries
variances = [.1, .1, .1, .1] # The list of variances by which the encoded target coordinates are scaled
normalize_coords = True # Whether or not the model is supposed to use coordinates relative to the image size
n_predictor_layers = 4
training_info_path = './main_model_training_info'
if not os.path.isdir(training_info_path):
    os.mkdir(training_info_path)


##############################################################################
# 3. Models instantiation
    
# pass model configuration parameters to the model builder

# step1: clear previous models from memory.
K.clear_session() 

image_size=(img_height, img_width, img_channels)
model_constructor_params = {'image_size': image_size, 'n_classes': n_classes, 'mode':'training', 'training_info_path':training_info_path, \
                            'l2_regularization':0.0005, 'min_scale': 0.1, 'max_scale':0.9, 'scales':scales, 'n_predictor_layers':n_predictor_layers,\
                            'aspect_ratios_global':aspect_ratios_global,'aspect_ratios_per_layer':aspect_ratios_per_layer, 'two_boxes_for_ar1':True,\
                            'steps':None, 'offsets':None, 'clip_boxes':False, 'variances':variances, 'coords':'centroids',\
                            'normalize_coords':True, 'subtract_mean':intensity_mean,'divide_by_stddev':intensity_range,'swap_channels':False,\
                            'confidence_thresh':0.5,'iou_threshold':0.3, 'top_k':40, 'nms_max_output_size':400,\
                            'return_predictor_sizes':True,'build_base_model':True}

# step 2: Pass model configuration parameters to the model builder
constructed_models = build_models(model_constructor_params)
main_model =constructed_models.main_model
predictor_sizes = constructed_models.predictor_sizes

##############################################################################
# 4. Model training requirements

# construct the ssd loss function 
ssd_loss = SSDLoss(neg_pos_ratio=3, alpha=1.)

# Instantiate an Adam optimizer and the SSD loss function and compile the model
adam = Adam(lr=0.001, beta_1=0.9, beta_2=0.999, epsilon=1e-08, decay=0.0)

# compile the model with ssd loss and selected optimizer
main_model.compile(optimizer=adam, loss=ssd_loss.compute_loss)


##############################################################################
# 5. Data generators



###### 5.1. Instantiate generators and parse the datasets


# 1: Instantiate two `DataGenerator` objects: One for training, one for validation.

# Optional: If you have enough memory, consider loading the images into memory for the reasons explained above.
train_dataset = DataGenerator(load_images_into_memory=False, hdf5_dataset_path=None)
val_dataset   = DataGenerator(load_images_into_memory=False, hdf5_dataset_path=None)
test_dataset  = DataGenerator(load_images_into_memory=False, hdf5_dataset_path=None)

# 2: Parse the image and label lists for the training and validation datasets.

# Images
data_path   = './data'
all_images_path = os.path.join(data_path, 'images')

# Ground truth
train_labels_filename = os.path.join(data_path, 'train_data.csv')
val_labels_filename   = os.path.join(data_path, 'val_data.csv')
test_labels_filename   = os.path.join(data_path, 'test_data.csv')

# parce the csv files
train_dataset.parse_csv(images_dir=all_images_path,
                        labels_filename=train_labels_filename,
                        input_format=['image_name', 'xmin', 'xmax', 'ymin', 'ymax', 'class_id'], # This is the order of the first six columns in the CSV file that contains the labels for your dataset. If your labels are in XML format, maybe the XML parser will be helpful, check the documentation.
                        include_classes='all')

val_dataset.parse_csv(images_dir=all_images_path,
                      labels_filename=val_labels_filename,
                      input_format=['image_name', 'xmin', 'xmax', 'ymin', 'ymax', 'class_id'],
                      include_classes='all')

test_dataset.parse_csv(images_dir=all_images_path,
                      labels_filename=val_labels_filename,
                      input_format=['image_name', 'xmin', 'xmax', 'ymin', 'ymax', 'class_id'],
                      include_classes='all')

# Get the number of samples in the training and validations datasets.
train_dataset_size = train_dataset.get_dataset_size()
val_dataset_size   = val_dataset.get_dataset_size()
test_dataset_size  = test_dataset.get_dataset_size()
print("Number of images in the training dataset:\t{:>6}".format(train_dataset_size))
print("Number of images in the validation dataset:\t{:>6}".format(val_dataset_size))
print("Number of images in the test dataset:\t{:>6}".format(test_dataset_size))


###### 5.2. Image processing chain

# Define the image processing chain.
train_data_augmentation_chain = DataAugmentationConstantInputSize(random_brightness=(-48, 48, 0.5),
                                                                random_contrast=(0.5, 1.8, 0.5),
                                                                random_saturation=(0.5, 1.8, 0.5),
                                                                random_hue=(18, 0.5),
                                                                random_flip=0.5,
                                                                random_gaussian_noise = (0.5, 0., 10), # gaussine noise
                                                                random_poisson_noise  = (0.5, 20),     # poisson noise
                                                                random_salt_pepper_noise = (0.5, 0.5, 0.005), # salt&pepper or impalse noise 
                                                                random_row_defect = (0.5, 1), # row defect
                                                                random_col_defect = (0.5, 1), # col defect
                                                                random_translate=((0.03,0.5), (0.03,0.5), 0.5),
                                                                random_scale=(0.5, 2.0, 0.5),
                                                                n_trials_max=3,
                                                                clip_boxes=True,
                                                                overlap_criterion='area',
                                                                bounds_box_filter=(0.3, 1.0),
                                                                bounds_validator=(0.5, 1.0),
                                                                n_boxes_min=1,
                                                                background=(127,127,127))



# convert the images into the 3d arrays: (width, height, 1)
from data_generator.object_detection_2d_photometric_ops import ConvertTo1Channel
convertor = ConvertTo1Channel() 


###### 5.3. Instantiate SSD encoder
# Instantiate an encoder that can encode ground truth labels into the format needed by the SSD loss function.
# The encoder constructor needs the spatial dimensions of the model's predictor layers to create the anchor boxes.
ssd_input_encoder = SSDInputEncoder(img_height=img_height,
                                    img_width=img_width,
                                    n_classes=n_classes,
                                    predictor_sizes=predictor_sizes,
                                    scales=scales,
                                    aspect_ratios_global=aspect_ratios_global,
                                    aspect_ratios_per_layer= aspect_ratios_per_layer,
                                    two_boxes_for_ar1=two_boxes_for_ar1,
                                    steps=steps,
                                    offsets=offsets,
                                    clip_boxes=clip_boxes,
                                    variances=variances,
                                    matching_type='multi',
                                    #matching_type='bipartite',
                                    pos_iou_threshold=0.5,
                                    neg_iou_limit=0.3,
                                    normalize_coords=normalize_coords)

##### 5.4. Create generators 
train_generator = train_dataset.generate(batch_size=batch_size,
                                         shuffle=True,
                                         transformations=[train_data_augmentation_chain],
                                         label_encoder=ssd_input_encoder,
                                         returns={'processed_images',
                                                  'encoded_labels'},
                                         keep_images_without_gt=False)

val_generator = val_dataset.generate(batch_size=batch_size,
                                     shuffle=False,
                                     transformations=[train_data_augmentation_chain],
                                     label_encoder=ssd_input_encoder,
                                     returns={'processed_images',
                                              'encoded_labels'},
                                     keep_images_without_gt=False)

test_generator = test_dataset.generate(batch_size=batch_size,
                                     shuffle=False,
                                     transformations=[convertor],
                                     label_encoder=ssd_input_encoder,
                                     returns={'processed_images',
                                              'encoded_labels'},
                                     keep_images_without_gt=False)

##############################################################################
# 6. CAllbacks
# To save the model after every epoch.
model_checkpoint = ModelCheckpoint(filepath=os.path.join(training_info_path,'main_model_epoch-{epoch:02d}_loss-{loss:.4f}_val_loss-{val_loss:.4f}.h5'),
                                   monitor='val_loss',
                                   verbose=1,
                                   save_best_only=True,
                                   save_weights_only=False,
                                   mode='auto',
                                   period=1)

# Callback that streams epoch results to a csv file.
csv_logger = CSVLogger(filename=os.path.join(training_info_path, 'training_log.csv'),
                       separator=',',
                       append=True)

# Stop training when a monitored quantity has stopped improving.
early_stopping = EarlyStopping(monitor='val_loss',
                               min_delta=0.0,
                               patience=41,
                               verbose=1)

# Reduce learning rate when a metric has stopped improving.
reduce_learning_rate = ReduceLROnPlateau(monitor='val_loss',
                                         factor=0.2,
                                         patience=20,
                                         verbose=1,
                                         epsilon=0.001,
                                         cooldown=0,
                                         min_lr=0.000001)
# list of callbacks 
callbacks = [model_checkpoint,
             csv_logger,
             early_stopping,
             reduce_learning_rate]

##############################################################################
# 7. Train the main-model
# epoch numbers
initial_epoch   = 0
final_epoch     = 200

# number of steps per epoch
train_steps_per_epoch = int(train_dataset_size/batch_size)
val_steps_per_epoch   = int(val_dataset_size/batch_size)


# training
history = main_model.fit_generator(generator=train_generator,
                                  steps_per_epoch=train_steps_per_epoch,
                                  epochs=final_epoch,
                                  callbacks=callbacks,
                                  validation_data=val_generator,
                                  validation_steps=val_steps_per_epoch,
                                  initial_epoch=initial_epoch)
main_model._make_predict_function()


# plot training and validation losses
plt.figure(figsize=(10,10))
plt.plot(history.history['loss'], label='loss')
plt.plot(history.history['val_loss'], label='val_loss')
plt.legend(loc='upper right', prop={'size': 24});


##############################################################################
# 8. Create the base-model

# export the base model at the same path for the training info
constructed_models.create_base_model()



#%%############################################################################ 
# Evaluation


# make predictions for some test samples 

# generate samples
batch_images, batch_labels = next(test_generator)

# make a prediction
y_pred = main_model.predict(batch_images)

# Decode the raw prediction `y_pred`
y_pred_decoded = decode_detections(y_pred,
                                   confidence_thresh=0.5,
                                   iou_threshold=0.3,
                                   top_k=40,
                                   normalize_coords=normalize_coords,
                                   img_height=img_height,
                                   img_width=img_width)

# image number in the taken batch
i = 0

print("Ground truth boxes:\n")
print(batch_labels[i])

np.set_printoptions(precision=2, suppress=True, linewidth=90)
print("Predicted boxes:\n")
print('   class   conf xmin   ymin   xmax   ymax')
print(y_pred_decoded[i])

# Draw the predicted boxes onto the image
plt.figure(figsize=(6,6))
plt.imshow(batch_images[i][:,:,0], cmap ='gray')

current_axis = plt.gca()
colors = plt.cm.hsv(np.linspace(0, 1, n_classes+1)).tolist() # Set the colors for the bounding boxes
classes = ['non-human', 'human'] # Just so we can print class names onto the image instead of IDs


# Draw the ground truth boxes in green (omit the label for more clarity)
for box in batch_labels[i]:
    xmin = box[1]
    ymin = box[2]
    xmax = box[3]
    ymax = box[4]
    label = '{}'.format(classes[int(box[0])])
    current_axis.add_patch(plt.Rectangle((xmin, ymin), xmax-xmin, ymax-ymin, color='green', fill=False, linewidth=5))  

# Draw the predicted boxes in blue
for box in y_pred_decoded[i]:
    xmin = box[-4]
    ymin = box[-3]
    xmax = box[-2]
    ymax = box[-1]
    color = colors[int(box[0])]
    label = '{}: {:.2f}'.format(classes[int(box[0])], box[1])
    current_axis.add_patch(plt.Rectangle((xmin, ymin), xmax-xmin, ymax-ymin, color=color, fill=False, linewidth=2))  
    current_axis.text(xmin, ymin, label, size='x-large', color='white', bbox={'facecolor':color, 'alpha':0.5})



#%%############################################################################ 
# Performance report

# Instantiate a convertor, 2d images to the 3d arrays: (width, height, 1)
from data_generator.object_detection_2d_photometric_ops import ConvertTo1Channel
convertor = ConvertTo1Channel() 

# instantiate an evaluator using the test data set
from eval_utils.average_precision_evaluator import Evaluator
evaluator = Evaluator(model=main_model,
                      n_classes=n_classes,
                      data_generator=test_dataset,
                      model_mode='training')

# perform the evaluation
results = evaluator(img_height=80,
                    img_width=80,
                    batch_size=32,
                    data_generator_mode='resize',
                    convertor = convertor,
                    round_confidences=False,
                    matching_iou_threshold=0.5,
                    border_pixels='include',
                    sorting_algorithm='quicksort',
                    average_precision_mode='sample',
                    num_recall_points=11,
                    ignore_neutral_boxes=True,
                    return_precisions=True,
                    return_recalls=True,
                    return_average_precisions=True,
                    verbose=True)


# print the evaluation results
mean_average_precision, average_precisions, precisions, recalls = results
classes = ['non-human', 'human']
for i in range(1, len(average_precisions)):
    print("{:<14}{:<6}{}".format(classes[i], 'AP', round(average_precisions[i], 3)))
    print()
    print("{:<14}{:<6}{}".format('','mAP', round(mean_average_precision, 3)))
    
plt.plot(recalls[n_classes], precisions[n_classes], color='blue', linewidth=1.0)
plt.xlabel('recall', fontsize=14)
plt.ylabel('precision', fontsize=14)
plt.grid(True)
plt.xticks(np.linspace(0,1,11))
plt.yticks(np.linspace(0,1,11))
plt.title("{}, AP: {:.3f}".format(classes[n_classes], average_precisions[n_classes]), fontsize=16) 
