'''
A small 7-layer Keras model with SSD architecture. Also serves as a template to build arbitrary network architectures.

Copyright (C) 2018 Pierluigi Ferrari

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
'''

from __future__ import division
import numpy as np
from keras.models import Model
from keras.layers import Input, Lambda, Conv2D, MaxPooling2D, BatchNormalization, ELU, Reshape, Concatenate, Activation
from keras.regularizers import l2
import keras.backend as K
from keras import layers
import os
from keras_layers.keras_layer_AnchorBoxes import AnchorBoxes
from keras_layers.keras_layer_DecodeDetections import DecodeDetections
from keras_layers.keras_layer_DecodeDetectionsFast import DecodeDetectionsFast
from models.ssd_model_utils import *
import tensorflow as tf
from tensorflow import lite

class build_models:
    
    def __init__(self,kwargs):
        
        # all allowed keys will be initialized as class attributes
        required_keys = set(['image_size', 'n_classes', 'mode','l2_regularization', 'min_scale', 'max_scale', 'scales','n_predictor_layers','training_info_path',\
                         'aspect_ratios_global','aspect_ratios_per_layer', 'two_boxes_for_ar1', 'steps', 'offsets', 'clip_boxes',\
                         'variances', 'coords','normalize_coords', 'subtract_mean','divide_by_stddev','swap_channels','confidence_thresh',\
                         'iou_threshold', 'top_k', 'nms_max_output_size', 'return_predictor_sizes','build_base_model'])
        
        # initialize all allowed keys to false
        self.__dict__.update((key, None) for key in required_keys)
        
        # and update the given keys by their given values
        self.__dict__.update((key, value) for key, value in kwargs.items() if key in required_keys)


        self.params = kwargs


        'some evaluation on input values and re-calculations if required'
        self.init_params()

        'At the end, all build information will be stored/provided in model_info_dict'
        self.models_info = {'training':None, 'inference':None, 'inference_fast':None,\
                            'simple_model':None, 'predictor_sizes':None}

        
        'the input placeholder'
        x = Input(shape=(self.img_height, self.img_width, self.img_channels))
        
        '''
        
        model constroctures:
            
            - input_layer_constructor  : normalize the input or swap the channels, if required.
            - feature maps constructor : build the network structue throgh which the features are extracted. 
            - predictors constructor   : the network structure throgh which the predictions tensors are cretead.
            - anchors_constructor      : creats all defaults anchors according to the network structure
            
        '''
        input_layer_constructor   = construct_input_layer(self.params)      
        feature_maps_constructor  = construct_feature_maps(self.params)
        predictors_constructor    = construct_predictors(self.params)
        anchors_constructor       = construct_default_anchors(self.params)
        output_constructor        = construct_model_output(self.params)

        
        # costumes
        self.costume_input_layer  = input_layer_constructor
        self.costume_feature_maps = feature_maps_constructor
        self.costume_predictors   = predictors_constructor
        self.costume_anchors      = anchors_constructor
        self.costume_output       = output_constructor
        
            
            
        
        
        

        # main model construction
        x1 = input_layer_constructor(x)
        features = feature_maps_constructor(x1)
        classes, bboxes = predictors_constructor(features)
        anchors = anchors_constructor(bboxes)
        predictions      = output_constructor(classes, bboxes, anchors)
        

        if self.mode == 'training':
            self.main_model =  Model(inputs=x, outputs=predictions)
        

        if self.mode == 'inference':
            decoded_predictions = DecodeDetections(confidence_thresh=self.confidence_thresh,
                                                   iou_threshold=self.iou_threshold,
                                                   top_k=self.top_k,
                                                   nms_max_output_size=self.nms_max_output_size,
                                                   coords=self.coords,
                                                   normalize_coords=self.normalize_coords,
                                                   img_height=self.img_height,
                                                   img_width=self.img_width,
                                                   name='decoded_predictions')(predictions)
            self.main_model = Model(inputs=x, outputs=decoded_predictions)
        
        if self.mode == 'inference_fast':
            decoded_predictions = DecodeDetectionsFast(confidence_thresh=self.confidence_thresh,
                                                       iou_threshold=self.iou_threshold,
                                                       top_k=self.top_k,
                                                       nms_max_output_size=self.nms_max_output_size,
                                                       coords=self.coords,
                                                       normalize_coords=self.normalize_coords,
                                                       img_height=self.img_height,
                                                       img_width=self.img_width,
                                                       name='decoded_predictions')(predictions)
            self.main_model = Model(inputs=x, outputs=decoded_predictions)
            
        
        
        # build model
        if self.build_base_model:
            #######################################################################
            # construct the simple model for gap model
            base_model_input = Input(shape=(self.img_height, self.img_width, self.img_channels))
            base_features = feature_maps_constructor(base_model_input)
            base_classes, base_bboxes = predictors_constructor(base_features)
            self.base_model = Model(inputs= base_model_input, outputs =base_classes+base_bboxes)
    
        # build predictor sizes
        if self.return_predictor_sizes:
            # The spatial dimensions are the same for the `classes` and `boxes` predictor layers.
            predictors_sizes  = [class_predictor._keras_shape[1:3] for class_predictor in classes]
            self.predictor_sizes  = np.array(predictors_sizes)
            

    def init_params(self):
        self.n_classes += 1 # Account for the background class.
#        self.n_predictor_layers = 5 # The number of predictor conv layers in the network
        self.l2_reg = self.l2_regularization # Make the internal name shorter.
        self.img_height, self.img_width, self.img_channels = self.image_size[0], self.image_size[1], self.image_size[2]
    
        ############################################################################
        # Get a few exceptions out of the way.
        ############################################################################
    
        if self.aspect_ratios_global is None and self.aspect_ratios_per_layer is None:
            raise ValueError("`aspect_ratios_global` and `aspect_ratios_per_layer` cannot both be None. At least one needs to be specified.")
        if self.aspect_ratios_per_layer:
            if len(self.aspect_ratios_per_layer) != self.n_predictor_layers:
                raise ValueError("It must be either aspect_ratios_per_layer is None or len(aspect_ratios_per_layer) == {}, but len(aspect_ratios_per_layer) == {}.".format(self.n_predictor_layers, len(self.aspect_ratios_per_layer)))
    
        if (self.min_scale is None or self.max_scale is None) and self.scales is None:
            raise ValueError("Either `min_scale` and `max_scale` or `scales` need to be specified.")
        if self.scales:
            if len(self.scales) != self.n_predictor_layers+1:
                raise ValueError("It must be either scales is None or len(scales) == {}, but len(scales) == {}.".format(self.n_predictor_layers+1, len(self.scales)))
        else: # If no explicit list of scaling factors was passed, compute the list of scaling factors from `min_scale` and `max_scale`
            self.scales = np.linspace(self.min_scale, self.max_scale, self.n_predictor_layers+1)
    
        if len(self.variances) != 4: # We need one variance value for each of the four box coordinates
            raise ValueError("4 variance values must be pased, but {} values were received.".format(len(self.variances)))
        self.variances = np.array(self.variances)
        if np.any(self.variances <= 0):
            raise ValueError("All variances must be >0, but the variances given are {}".format(self.variances))
    
        if (not (self.steps is None)) and (len(self.steps) != self.n_predictor_layers):
            raise ValueError("You must provide at least one step value per predictor layer.")
    
        if (not (self.offsets is None)) and (len(self.offsets) != self.n_predictor_layers):
            raise ValueError("You must provide at least one offset value per predictor layer.")
    
        ############################################################################
        # Compute the anchor box parameters.
        ############################################################################
    
        # Set the aspect ratios for each predictor layer. These are only needed for the anchor box layers.
        if self.aspect_ratios_per_layer:
            self.aspect_ratios = self.aspect_ratios_per_layer
        else:
            self.aspect_ratios = [self.aspect_ratios_global] * self.n_predictor_layers
    
        # Compute the number of boxes to be predicted per cell for each predictor layer.
        # We need this so that we know how many channels the predictor layers need to have.
        if self.aspect_ratios_per_layer:
            n_boxes = []
            for ar in self.aspect_ratios_per_layer:
                if (1 in ar) & self.two_boxes_for_ar1:
                    n_boxes.append(len(ar) + 1) # +1 for the second box for aspect ratio 1
                else:
                    n_boxes.append(len(ar))
        else: # If only a global aspect ratio list was passed, then the number of boxes is the same for each predictor layer
            if (1 in self.aspect_ratios_global) & self.two_boxes_for_ar1:
                n_boxes = len(self.aspect_ratios_global) + 1
            else:
                n_boxes = len(self.aspect_ratios_global)
            n_boxes = [n_boxes] * self.n_predictor_layers
            
        self.n_boxes = n_boxes
        if self.steps is None:
            self.steps = [None] * self.n_predictor_layers
        if self.offsets is None:
            self.offsets = [None] * self.n_predictor_layers
    
        
        # information to create the anchors
        self.params['img_height']=self.img_height
        self.params['img_width']=self.img_width
        self.params['img_channels']=self.img_channels
        self.params['scales']=self.scales
        self.params['aspect_ratios']=self.aspect_ratios
        self.params['two_boxes_for_ar1']=self.two_boxes_for_ar1
        self.params['steps']=self.steps
        self.params['offsets']=self.offsets
        self.params['n_boxes']=self.n_boxes
        self.params['n_classes']=self.n_classes
        self.params['n_predictor_layers']=self.n_predictor_layers
    
    def create_base_model(self):
        layer_names=[layer.name for layer in self.base_model.layers]
        for layer_name in layer_names:
            if len(self.base_model.get_layer(layer_name).get_weights())>0:
                layer   = self.main_model.get_layer(layer_name)
                weights = layer.get_weights()
                self.base_model.get_layer(layer_name).set_weights(weights)          
        
        # create h5
        self.base_model.save(os.path.join(self.training_info_path,'base_model.h5'))
        
        
        # create a frozen graph
        frozen_graph = self.freeze_session(K.get_session(), output_names=[out.op.name for out in self.base_model.outputs])
        tf.train.write_graph(frozen_graph, self.training_info_path, 'base_model.pb', as_text=False)
        
                
        # create tflite
        lite_converter = lite.TFLiteConverter.from_keras_model_file(os.path.join(self.training_info_path,'base_model.h5'))
        lite_converter.target_ops = [lite.OpsSet.TFLITE_BUILTINS, lite.OpsSet.SELECT_TF_OPS]
        tflite_base_model = lite_converter.convert()
        open(os.path.join(self.training_info_path,'base_model.tflite'), "wb").write(tflite_base_model)
        

    def freeze_session(self, session, keep_var_names=None, output_names=None, clear_devices=True):
        """
        Freezes the state of a session into a pruned computation graph.
        
        Creates a new computation graph where variable nodes are replaced by
        constants taking their current value in the session. The new graph will be
        pruned so subgraphs that are not necessary to compute the requested
        outputs are removed.
        @param session The TensorFlow session to be frozen.
        @param keep_var_names A list of variable names that should not be frozen,
                              or None to freeze all the variables in the graph.
        @param output_names Names of the relevant graph outputs.
        @param clear_devices Remove the device directives from the graph for better portability.
        @return The frozen graph definition.
        """
        graph = session.graph
        with graph.as_default():
            freeze_var_names = list(set(v.op.name for v in tf.global_variables()).difference(keep_var_names or []))
            output_names = output_names or []
            output_names += [v.op.name for v in tf.global_variables()]
            input_graph_def = graph.as_graph_def()
            if clear_devices:
                for node in input_graph_def.node:
                    node.device = ""
            frozen_graph = tf.graph_util.convert_variables_to_constants(
                session, input_graph_def, output_names, freeze_var_names)
            return frozen_graph
        