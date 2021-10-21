from __future__ import division
import numpy as np
from keras.models import Model
from keras.layers import Input, Lambda, Conv2D, MaxPooling2D, BatchNormalization, ELU, Reshape, Concatenate, Activation
from keras.regularizers import l2
import keras.backend as K
from keras import layers
from keras_layers.keras_layer_AnchorBoxes import AnchorBoxes
from keras_layers.keras_layer_DecodeDetections import DecodeDetections
from keras_layers.keras_layer_DecodeDetectionsFast import DecodeDetectionsFast


class construct_input_layer(layers.Layer):
    
    # put any user-define layers here
    def __init__(self, params, name='input_layer', **kwargs):
        super(construct_input_layer, self).__init__(name=name)
        
        required_keys = set(['subtract_mean', 'divide_by_stddev', 'swap_channels','img_width', 'img_height', 'img_channels' ])
        self.__dict__.update((key, None) for key in required_keys)
        self.__dict__.update((key, value) for key, value in params.items() if key in required_keys)

    def identity_layer(self, tensor):
        return tensor
        
    def input_mean_normalization(self, tensor):
        return tensor - np.array(self.subtract_mean)
    
    def input_stddev_normalization(self, tensor):
        return tensor / np.array(self.divide_by_stddev)
        
    def input_channel_swap(self, tensor):
        if len(self.swap_channels) == 3:
            return K.stack([tensor[...,self.swap_channels[0]], tensor[...,self.swap_channels[1]], tensor[...,self.swap_channels[2]]], axis=-1)
        elif len(self.swap_channels) == 4:
            return K.stack([tensor[...,self.swap_channels[0]], tensor[...,self.swap_channels[1]], tensor[...,self.swap_channels[2]], tensor[...,self.swap_channels[3]]], axis=-1)
    
    def into_2d_layer(self, tensor):
        return K.stack([tensor[...,0]], axis=-1)    
          
    def call(self, inputs):
        # The following identity layer is only needed so that the subsequent lambda layers can be optional.
        x1 = Lambda(self.identity_layer, output_shape=(self.img_height, self.img_width, self.img_channels), name='identity_layer')(inputs)
        if not (self.subtract_mean is None):
            x1 = Lambda(self.input_mean_normalization, output_shape=(self.img_height, self.img_width, self.img_channels), name='input_mean_normalization')(x1)
        if not (self.divide_by_stddev is None):
            x1 = Lambda(self.input_stddev_normalization, output_shape=(self.img_height, self.img_width, self.img_channels), name='input_stddev_normalization')(x1)
        if self.swap_channels:
            x1 = Lambda(self.input_channel_swap, output_shape=(self.img_height, self.img_width, self.img_channels), name='input_channel_swap')(x1)
        return x1
    
         
     
class construct_feature_maps:
    
    
    def __init__(self, params):
        
        required_keys = set(['l2_regularization'])
        self.__dict__.update((key, None) for key in required_keys)
        self.__dict__.update((key, value) for key, value in params.items() if key in required_keys)
        
    def __call__(self, x):
        
#        print('')
#        print('')
#        print('')
#        print(x)
#        print('')
#        print('')
#        print('')
#        n_filters = 16 
#        kernel_size =5
#        conv1 = Conv2D(n_filters, (kernel_size, kernel_size), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv1')(x)
#        conv1_1 = BatchNormalization(axis=3, momentum=0.99, name='bn1')(conv1) 
#        conv1_2 = Activation('relu', name='relu1')(conv1_1)
#        pool1 = MaxPooling2D(pool_size=(2, 2), name='pool1')(conv1_2)
#    
#        conv2 = Conv2D(n_filters, (kernel_size, kernel_size), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv2')(pool1)
#        conv2_1 = BatchNormalization(axis=3, momentum=0.99, name='bn2')(conv2)
#        conv2_2 = Activation('relu', name='relu2')(conv2_1) 
#        pool2 = MaxPooling2D(pool_size=(2, 2), name='pool2')(conv2_2)
#        
#        conv3 = Conv2D(n_filters, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv3')(pool2)
#        conv3_1 = BatchNormalization(axis=3, momentum=0.99, name='bn3')(conv3)
#        conv3_2 = Activation('relu',name='relu3')(conv3_1)
#        pool3 = MaxPooling2D(pool_size=(2, 2), name='pool3')(conv3_2)
#    
#        n_filters = 16
#        conv4 = Conv2D(n_filters, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv4')(pool3)
#        conv4_1 = BatchNormalization(axis=3, momentum=0.99, name='bn4')(conv4)
#        conv4_2 = Activation('relu', name='relu4')(conv4_1) #===> 10*10 ===> scale[2]
#        pool4 = MaxPooling2D(pool_size=(2, 2), name='pool4')(conv4_2)
#    
#        conv5 = Conv2D(n_filters, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv5')(pool4)
#        conv5_1 = BatchNormalization(axis=3, momentum=0.99, name='bn5')(conv5)
#        conv5_2 = Activation('relu', name='relu5')(conv5_1)
        
#        return [conv2, conv3, conv4, conv5_2]
        kernel_size = 5
        conv1 = Conv2D(16, (kernel_size, kernel_size), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv1')(x)
        conv1 = BatchNormalization(axis=3, momentum=0.99, name='bn1')(conv1) # Tensorflow uses filter format [filter_height, filter_width, in_channels, out_channels], hence axis = 3
        conv1 = Activation('relu', name='relu1')(conv1)
        pool1 = MaxPooling2D(pool_size=(2, 2), name='pool1')(conv1)
        # out: 40x40
    
        # in: 40x40
        conv2 = Conv2D(16, (kernel_size, kernel_size), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv2')(pool1)
        conv2 = BatchNormalization(axis=3, momentum=0.99, name='bn2')(conv2)
        conv2 = Activation('relu', name='relu2')(conv2) #===> 40*40: scale[0]
        pool2 = MaxPooling2D(pool_size=(2, 2), name='pool2')(conv2)
        # out: 20x20
    
    
        # in: 20x20
        conv3 = Conv2D(16, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv3')(pool2)
        conv3 = BatchNormalization(axis=3, momentum=0.99, name='bn3')(conv3)
        conv3 = Activation('relu',name='relu3')(conv3) #===> 20*20 ===> scale[1]
        pool3 = MaxPooling2D(pool_size=(2, 2), name='pool3')(conv3)
        # out: 10x10: 0.08
    
    
        # in: 10x10
        conv4 = Conv2D(16, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv4')(pool3)
        conv4 = BatchNormalization(axis=3, momentum=0.99, name='bn4')(conv4)
        conv4 = Activation('relu', name='relu4')(conv4) #===> 10*10 ===> scale[2]
        pool4 = MaxPooling2D(pool_size=(2, 2), name='pool4')(conv4)
        # out: 5x5: 0.16
    
    
        # in: 5x5
        conv5 = Conv2D(16, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal', kernel_regularizer=l2(self.l2_regularization), name='conv5')(pool4)
        conv5 = BatchNormalization(axis=3, momentum=0.99, name='bn5')(conv5)
        conv5 = Activation('relu', name='relu5')(conv5) #===> 5*5 ===> scale[3]
                    
        return [conv2, conv3, conv4, conv5]


class construct_predictors:
    def __init__(self, params):
        # initialize all allowed keys to false
        
        required_keys = set(['n_boxes', 'n_classes', 'l2_regularization'])

        self.__dict__.update((key, None) for key in required_keys)
        
        # and update the given keys by their given values
        self.__dict__.update((key, value) for key, value in params.items() if key in required_keys)
    
    def __call__(self, feature_maps):

        classes , bboxes = [], []
        for i, feature_map in enumerate(feature_maps):
            # classes
            temp_classes = Conv2D(self.n_boxes[i] * self.n_classes, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal',\
                                  kernel_regularizer=l2(self.l2_regularization), name='classes'+str(i))(feature_map)
            classes.append(temp_classes)
            # bboxes
            temp_boxes   = Conv2D(self.n_boxes[i] * 4, (3, 3), strides=(1, 1), padding="same", kernel_initializer='he_normal',\
                                  kernel_regularizer=l2(self.l2_regularization), name='boxes'+str(i))(feature_map)
            bboxes.append(temp_boxes)
        return classes, bboxes

    
class construct_default_anchors:
    def __init__(self, params):
        # initialize all allowed keys to false
        self.__dict__.update((key, None) for key in params)
        # and update the given keys by their given values
        self.__dict__.update((key, value) for key, value in params.items() if key in params)
            
    def __call__(self, bboxes):

        default_anchors = []
        for i, bbox in enumerate(bboxes):
            temp_anchors = AnchorBoxes(self.img_height, self.img_width, this_scale=self.scales[i], next_scale=self.scales[i+1], aspect_ratios=self.aspect_ratios[i],
                           two_boxes_for_ar1=self.two_boxes_for_ar1, this_steps=self.steps[i], this_offsets=self.offsets[i],
                           clip_boxes=self.clip_boxes, variances=self.variances, coords=self.coords, normalize_coords=self.normalize_coords, name='anchors'+str(i))(bbox)
            default_anchors.append(temp_anchors)
        return default_anchors

class construct_model_output:
    def __init__(self, params):
        
        # initialize all allowed keys to false
        self.__dict__.update((key, None) for key in params)
        
        # and update the given keys by their given values
        self.__dict__.update((key, value) for key, value in params.items() if key in params)
            
        
    def __call__(self, classes, bboxes, acnhors):
        
        classes_reshaped, boxes_reshaped, anchors_reshaped = [],[],[]
        for i in range(self.n_predictor_layers):
            classes_reshaped.append(Reshape((-1, self.n_classes), name='classes'+str(i)+'_reshape')(classes[i]))
            boxes_reshaped.append(Reshape((-1, 4), name='boxes'+str(i)+'_reshape')(bboxes[i]))
            anchors_reshaped.append(Reshape((-1, 8), name='anchors'+str(i)+'_reshape')(acnhors[i]))

        # Concatenate the predictions from the different layers and the assosciated anchor box tensors
        # Axis 0 (batch) and axis 2 (n_classes or 4, respectively) are identical for all layer predictions,
        # so we want to concatenate along axis 1
        # Output shape of `classes_concat`: (batch, n_boxes_total, n_classes)
        classes_concat = Concatenate(axis=1, name='classes_concat')(classes_reshaped)
    
        # Output shape of `boxes_concat`: (batch, n_boxes_total, 4)
        boxes_concat = Concatenate(axis=1, name='boxes_concat')(boxes_reshaped)
    
        # Output shape of `anchors_concat`: (batch, n_boxes_total, 8)
        anchors_concat = Concatenate(axis=1, name='anchors_concat')(anchors_reshaped)
    
        # The box coordinate predictions will go into the loss function just the way they are,
        # but for the class predictions, we'll apply a softmax activation layer first
        classes_softmax = Activation('softmax', name='classes_softmax')(classes_concat)
    
        # Concatenate the class and box coordinate predictions and the anchors to one large predictions tensor
        # Output shape of `predictions`: (batch, n_boxes_total, n_classes + 4 + 8)
        predictions = Concatenate(axis=2, name='predictions')([classes_softmax, boxes_concat, anchors_concat])
        return predictions
