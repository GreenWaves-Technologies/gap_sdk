#
# preload_mobilenet.py
# Manuele Rusci <manuele.rusci@unibo.it>
#
# Copyright (C) 2019 University of Bologna
# All rights reserved.
#
# Adapted from: https://github.com/ruotianluo/pytorch-mobilenet-from-tf
#

import torch
import torch.nn as nn
import torch.utils.model_zoo as model_zoo
import os
import sys

import tensorflow as tf
from tensorflow.python import pywrap_tensorflow
from collections import OrderedDict
import re
import torch
import numpy as np

dummy_replace = OrderedDict([\
('features/Logits/Conv2d_1c_1x1/biases','fc.bias' ),\
('features/Logits/Conv2d_1c_1x1/weights','fc.weight'),\
('features/Conv2d_0/weights','model.0.0.weight'         ),  \
('features/Conv2d_0/BatchNorm/beta','model.0.1.bias'      ),\
('features/Conv2d_0/BatchNorm/moving_mean','model.0.1.running_mean'   ),  \
('features/Conv2d_0/BatchNorm/moving_variance','model.0.1.running_var'    ),  \
('features/Conv2d_0/BatchNorm/gamma','model.0.1.weight'        ),\
('features/Conv2d_1_depthwise/depthwise_weights','model.1.0.weight' ),\
('features/Conv2d_1_depthwise/BatchNorm/beta','model.1.1.bias' ),\
('features/Conv2d_1_depthwise/BatchNorm/moving_mean','model.1.1.running_mean'   ),  \
('features/Conv2d_1_depthwise/BatchNorm/moving_variance','model.1.1.running_var'    ),  \
('features/Conv2d_1_depthwise/BatchNorm/gamma','model.1.1.weight'         ),  \
('features/Conv2d_1_pointwise/weights','model.2.0.weight'         ),  \
('features/Conv2d_1_pointwise/BatchNorm/beta','model.2.1.bias'           ),  \
('features/Conv2d_1_pointwise/BatchNorm/moving_mean','model.2.1.running_mean'   ),  \
('features/Conv2d_1_pointwise/BatchNorm/moving_variance','model.2.1.running_var'    ),  \
('features/Conv2d_1_pointwise/BatchNorm/gamma','model.2.1.weight'         ),  \
('features/Conv2d_2_depthwise/depthwise_weights','model.3.0.weight'         ),  \
('features/Conv2d_2_depthwise/BatchNorm/beta','model.3.1.bias'           ),  \
('features/Conv2d_2_depthwise/BatchNorm/moving_mean','model.3.1.running_mean'   ),  \
('features/Conv2d_2_depthwise/BatchNorm/moving_variance','model.3.1.running_var'    ),  \
('features/Conv2d_2_depthwise/BatchNorm/gamma','model.3.1.weight'         ),  \
('features/Conv2d_2_pointwise/weights','model.4.0.weight'         ),  \
('features/Conv2d_2_pointwise/BatchNorm/beta','model.4.1.bias'           ),  \
('features/Conv2d_2_pointwise/BatchNorm/moving_mean','model.4.1.running_mean'   ),  \
('features/Conv2d_2_pointwise/BatchNorm/moving_variance','model.4.1.running_var'    ),  \
('features/Conv2d_2_pointwise/BatchNorm/gamma','model.4.1.weight'         ),  \
('features/Conv2d_3_depthwise/depthwise_weights','model.5.0.weight'         ),  \
('features/Conv2d_3_depthwise/BatchNorm/beta','model.5.1.bias'           ),  \
('features/Conv2d_3_depthwise/BatchNorm/moving_mean','model.5.1.running_mean'   ),  \
('features/Conv2d_3_depthwise/BatchNorm/moving_variance','model.5.1.running_var'    ),  \
('features/Conv2d_3_depthwise/BatchNorm/gamma','model.5.1.weight'         ),  \
('features/Conv2d_3_pointwise/weights','model.6.0.weight'         ),  \
('features/Conv2d_3_pointwise/BatchNorm/beta','model.6.1.bias'           ),  \
('features/Conv2d_3_pointwise/BatchNorm/moving_mean','model.6.1.running_mean'   ),  \
('features/Conv2d_3_pointwise/BatchNorm/moving_variance','model.6.1.running_var'    ),  \
('features/Conv2d_3_pointwise/BatchNorm/gamma','model.6.1.weight'         ),  \
('features/Conv2d_4_depthwise/depthwise_weights','model.7.0.weight'         ),  \
('features/Conv2d_4_depthwise/BatchNorm/beta','model.7.1.bias'           ),  \
('features/Conv2d_4_depthwise/BatchNorm/moving_mean','model.7.1.running_mean'   ),  \
('features/Conv2d_4_depthwise/BatchNorm/moving_variance','model.7.1.running_var'    ),  \
('features/Conv2d_4_depthwise/BatchNorm/gamma','model.7.1.weight'         ),  \
('features/Conv2d_4_pointwise/weights','model.8.0.weight'         ),  \
('features/Conv2d_4_pointwise/BatchNorm/beta','model.8.1.bias'           ),  \
('features/Conv2d_4_pointwise/BatchNorm/moving_mean','model.8.1.running_mean'   ),  \
('features/Conv2d_4_pointwise/BatchNorm/moving_variance','model.8.1.running_var'    ),  \
('features/Conv2d_4_pointwise/BatchNorm/gamma','model.8.1.weight'         ),  \
('features/Conv2d_5_depthwise/depthwise_weights','model.9.0.weight'         ),  \
('features/Conv2d_5_depthwise/BatchNorm/beta','model.9.1.bias'           ),  \
('features/Conv2d_5_depthwise/BatchNorm/moving_mean','model.9.1.running_mean'   ),  \
('features/Conv2d_5_depthwise/BatchNorm/moving_variance','model.9.1.running_var'    ),  \
('features/Conv2d_5_depthwise/BatchNorm/gamma','model.9.1.weight'         ),  \
('features/Conv2d_5_pointwise/weights','model.10.0.weight'        ),  \
('features/Conv2d_5_pointwise/BatchNorm/beta','model.10.1.bias'          ),  \
('features/Conv2d_5_pointwise/BatchNorm/moving_mean','model.10.1.running_mean'  ),  \
('features/Conv2d_5_pointwise/BatchNorm/moving_variance','model.10.1.running_var'   ),  \
('features/Conv2d_5_pointwise/BatchNorm/gamma','model.10.1.weight'        ),  \
('features/Conv2d_6_depthwise/depthwise_weights','model.11.0.weight'        ),  \
('features/Conv2d_6_depthwise/BatchNorm/beta','model.11.1.bias'          ),  \
('features/Conv2d_6_depthwise/BatchNorm/moving_mean','model.11.1.running_mean'  ),  \
('features/Conv2d_6_depthwise/BatchNorm/moving_variance','model.11.1.running_var'   ),  \
('features/Conv2d_6_depthwise/BatchNorm/gamma','model.11.1.weight'        ),  \
('features/Conv2d_6_pointwise/weights','model.12.0.weight'        ),  \
('features/Conv2d_6_pointwise/BatchNorm/beta','model.12.1.bias'          ),  \
('features/Conv2d_6_pointwise/BatchNorm/moving_mean','model.12.1.running_mean'  ),  \
('features/Conv2d_6_pointwise/BatchNorm/moving_variance','model.12.1.running_var'   ),  \
('features/Conv2d_6_pointwise/BatchNorm/gamma','model.12.1.weight'        ),  \
('features/Conv2d_7_depthwise/depthwise_weights','model.13.0.weight'        ),  \
('features/Conv2d_7_depthwise/BatchNorm/beta','model.13.1.bias'          ),  \
('features/Conv2d_7_depthwise/BatchNorm/moving_mean','model.13.1.running_mean'  ),  \
('features/Conv2d_7_depthwise/BatchNorm/moving_variance','model.13.1.running_var'   ),  \
('features/Conv2d_7_depthwise/BatchNorm/gamma','model.13.1.weight'        ),  \
('features/Conv2d_7_pointwise/weights','model.14.0.weight'        ),  \
('features/Conv2d_7_pointwise/BatchNorm/beta','model.14.1.bias'          ),  \
('features/Conv2d_7_pointwise/BatchNorm/moving_mean','model.14.1.running_mean'  ),  \
('features/Conv2d_7_pointwise/BatchNorm/moving_variance','model.14.1.running_var'   ),  \
('features/Conv2d_7_pointwise/BatchNorm/gamma','model.14.1.weight'        ),  \
('features/Conv2d_8_depthwise/depthwise_weights','model.15.0.weight'        ),  \
('features/Conv2d_8_depthwise/BatchNorm/beta','model.15.1.bias'          ),  \
('features/Conv2d_8_depthwise/BatchNorm/moving_mean','model.15.1.running_mean'  ),  \
('features/Conv2d_8_depthwise/BatchNorm/moving_variance','model.15.1.running_var'   ),  \
('features/Conv2d_8_depthwise/BatchNorm/gamma','model.15.1.weight'        ),  \
('features/Conv2d_8_pointwise/weights','model.16.0.weight'        ),  \
('features/Conv2d_8_pointwise/BatchNorm/beta','model.16.1.bias'          ),  \
('features/Conv2d_8_pointwise/BatchNorm/moving_mean','model.16.1.running_mean'  ),  \
('features/Conv2d_8_pointwise/BatchNorm/moving_variance','model.16.1.running_var'   ),  \
('features/Conv2d_8_pointwise/BatchNorm/gamma','model.16.1.weight'        ),  \
('features/Conv2d_9_depthwise/depthwise_weights','model.17.0.weight'        ),  \
('features/Conv2d_9_depthwise/BatchNorm/beta','model.17.1.bias'          ),  \
('features/Conv2d_9_depthwise/BatchNorm/moving_mean','model.17.1.running_mean'  ),  \
('features/Conv2d_9_depthwise/BatchNorm/moving_variance','model.17.1.running_var'   ),  \
('features/Conv2d_9_depthwise/BatchNorm/gamma','model.17.1.weight'        ),  \
('features/Conv2d_9_pointwise/weights','model.18.0.weight'        ),  \
('features/Conv2d_9_pointwise/BatchNorm/beta','model.18.1.bias'          ),  \
('features/Conv2d_9_pointwise/BatchNorm/moving_mean','model.18.1.running_mean'  ),  \
('features/Conv2d_9_pointwise/BatchNorm/moving_variance','model.18.1.running_var'   ),  \
('features/Conv2d_9_pointwise/BatchNorm/gamma','model.18.1.weight'        ),  \
('features/Conv2d_10_depthwise/depthwise_weights','model.19.0.weight'        ),  \
('features/Conv2d_10_depthwise/BatchNorm/beta','model.19.1.bias'          ),  \
('features/Conv2d_10_depthwise/BatchNorm/moving_mean','model.19.1.running_mean'  ),  \
('features/Conv2d_10_depthwise/BatchNorm/moving_variance','model.19.1.running_var'   ),  \
('features/Conv2d_10_depthwise/BatchNorm/gamma','model.19.1.weight'        ),  \
('features/Conv2d_10_pointwise/weights','model.20.0.weight'        ),  \
('features/Conv2d_10_pointwise/BatchNorm/beta','model.20.1.bias'          ),  \
('features/Conv2d_10_pointwise/BatchNorm/moving_mean','model.20.1.running_mean'  ),  \
('features/Conv2d_10_pointwise/BatchNorm/moving_variance','model.20.1.running_var'   ),  \
('features/Conv2d_10_pointwise/BatchNorm/gamma','model.20.1.weight'        ),  \
('features/Conv2d_11_depthwise/depthwise_weights','model.21.0.weight'        ),  \
('features/Conv2d_11_depthwise/BatchNorm/beta','model.21.1.bias'          ),  \
('features/Conv2d_11_depthwise/BatchNorm/moving_mean','model.21.1.running_mean'  ),  \
('features/Conv2d_11_depthwise/BatchNorm/moving_variance','model.21.1.running_var'   ),  \
('features/Conv2d_11_depthwise/BatchNorm/gamma','model.21.1.weight'        ),  \
('features/Conv2d_11_pointwise/weights','model.22.0.weight'        ),  \
('features/Conv2d_11_pointwise/BatchNorm/beta','model.22.1.bias'          ),  \
('features/Conv2d_11_pointwise/BatchNorm/moving_mean','model.22.1.running_mean'  ),  \
('features/Conv2d_11_pointwise/BatchNorm/moving_variance','model.22.1.running_var'   ),  \
('features/Conv2d_11_pointwise/BatchNorm/gamma','model.22.1.weight'        ),  \
('features/Conv2d_12_depthwise/depthwise_weights','model.23.0.weight'        ),  \
('features/Conv2d_12_depthwise/BatchNorm/beta','model.23.1.bias'          ),  \
('features/Conv2d_12_depthwise/BatchNorm/moving_mean','model.23.1.running_mean'  ),  \
('features/Conv2d_12_depthwise/BatchNorm/moving_variance','model.23.1.running_var'   ),  \
('features/Conv2d_12_depthwise/BatchNorm/gamma','model.23.1.weight'        ),  \
('features/Conv2d_12_pointwise/weights','model.24.0.weight'        ),  \
('features/Conv2d_12_pointwise/BatchNorm/beta','model.24.1.bias'          ),  \
('features/Conv2d_12_pointwise/BatchNorm/moving_mean','model.24.1.running_mean'  ),  \
('features/Conv2d_12_pointwise/BatchNorm/moving_variance','model.24.1.running_var'   ),  \
('features/Conv2d_12_pointwise/BatchNorm/gamma','model.24.1.weight'        ),  \
('features/Conv2d_13_depthwise/depthwise_weights','model.25.0.weight'        ),  \
('features/Conv2d_13_depthwise/BatchNorm/beta','model.25.1.bias'          ),  \
('features/Conv2d_13_depthwise/BatchNorm/moving_mean','model.25.1.running_mean'  ),  \
('features/Conv2d_13_depthwise/BatchNorm/moving_variance','model.25.1.running_var'   ),  \
('features/Conv2d_13_depthwise/BatchNorm/gamma','model.25.1.weight'        ),  \
('features/Conv2d_13_pointwise/weights','model.26.0.weight'        ),  \
('features/Conv2d_13_pointwise/BatchNorm/beta','model.26.1.bias'          ),  \
('features/Conv2d_13_pointwise/BatchNorm/moving_mean','model.26.1.running_mean'  ),  \
('features/Conv2d_13_pointwise/BatchNorm/moving_variance','model.26.1.running_var'   ),  \
('features/Conv2d_13_pointwise/BatchNorm/gamma','model.26.1.weight'        ),  \
])


def path_pretrained_tf(input_size, depth_multiplier):

	folder_path = '/home/rusci/Work/quantized_neural_networks/models/imagenet/mobilenet_tf/'

	if input_size == 224:
	    if depth_multiplier == 1.0:
	        tensorflow_model='224_1.0/mobilenet_v1_1.0_224.ckpt'
	    elif  depth_multiplier == 0.75:
	        tensorflow_model='224_0.75/mobilenet_v1_0.75_224.ckpt'
	    elif depth_multiplier == 0.5:
	        tensorflow_model='224_0.5/mobilenet_v1_0.5_224.ckpt'
	    elif depth_multiplier == 0.25:
	        tensorflow_model='224_0.25/mobilenet_v1_0.25_224.ckpt'
	elif input_size == 192:
	    if depth_multiplier == 1.0:
	        tensorflow_model='192_1.0/mobilenet_v1_1.0_192.ckpt'
	    elif  depth_multiplier == 0.75:
	        tensorflow_model='192_0.75/mobilenet_v1_0.75_192.ckpt'
	    elif depth_multiplier == 0.5:
	        tensorflow_model='192_0.5/mobilenet_v1_0.5_192.ckpt'
	    elif depth_multiplier == 0.25:
	        tensorflow_model='192_0.25/mobilenet_v1_0.25_192.ckpt'
	elif input_size == 160:
	    if depth_multiplier == 1.0:
	        tensorflow_model='160_1.0/mobilenet_v1_1.0_160.ckpt'
	    elif  depth_multiplier == 0.75:
	        tensorflow_model='160_0.75/mobilenet_v1_0.75_160.ckpt'
	    elif depth_multiplier == 0.5:
	        tensorflow_model='160_0.5/mobilenet_v1_0.5_160.ckpt'
	    elif depth_multiplier == 0.25:
	        tensorflow_model='160_0.25/mobilenet_v1_0.25_160.ckpt'
	elif input_size == 128:
	    if depth_multiplier == 1.0:
	        tensorflow_model='128_1.0/mobilenet_v1_1.0_128.ckpt'
	    elif  depth_multiplier == 0.75:
	        tensorflow_model='128_0.75/mobilenet_v1_0.75_128.ckpt'
	    elif depth_multiplier == 0.5:
	        tensorflow_model='128_0.5/mobilenet_v1_0.5_128.ckpt'
	    elif depth_multiplier == 0.25:
	        tensorflow_model='128_0.25/mobilenet_v1_0.25_128.ckpt'

	return folder_path+tensorflow_model



def preload_mobilenet_tf(model, input_size, depth_multiplier):

	#Read TensorFlow Model
	tensorflow_model = path_pretrained_tf(input_size, depth_multiplier)
	reader = pywrap_tensorflow.NewCheckpointReader(tensorflow_model)
	var_to_shape_map = reader.get_variable_to_shape_map()
	var_dict = {k:reader.get_tensor(k) for k in var_to_shape_map.keys()}

	# Take PyTorch Model
	x = model.state_dict()

	# Filtering Tensorflow params from the model
	for k in list(var_dict.keys()):
	    if var_dict[k].ndim == 4:
	        if 'depthwise' in k:
	            var_dict[k] = var_dict[k].transpose((2, 3, 0, 1)).copy(order='C')
	        else:
	            var_dict[k] = var_dict[k].transpose((3, 2, 0, 1)).copy(order='C')
	    if var_dict[k].ndim == 2:
	        var_dict[k] = var_dict[k].transpose((1, 0)).copy(order='C')

	for k in list(var_dict.keys()):
	    if 'Momentum' in k or 'ExponentialMovingAverage' in k or 'RMSProp' in k or 'global_step' in k :
	        del var_dict[k]

	for k in list(var_dict.keys()):
	    if k.find('/') >= 0:
	        var_dict['features'+k[k.find('/'):]] = var_dict[k]
	        del var_dict[k]

	# Adapt from tensorflow to pytorch 
	for a, b in dummy_replace.items():
	    for k in list(var_dict.keys()):
	        if a in k:
	            var_dict[k.replace(a,b)] = var_dict[k]
	            del var_dict[k]
            
#	print('In var_dict but not in x_dict')
#	print(set(var_dict.keys()) - set(x.keys()))
#	print('In x_dict but not in var_dict')
#	print(set(x.keys()) - set(var_dict.keys()))
	for k in set(x.keys()) - set(var_dict.keys()):
	    del x[k]

	assert len(set(x.keys()) - set(var_dict.keys())) == 0
	for k in set(var_dict.keys()) - set(x.keys()):
	    del var_dict[k]

	for k in list(var_dict.keys()):
	    if x[k].shape != var_dict[k].shape:
	       print(k, 'Error')

	for k in list(var_dict.keys()):
	    var_dict[k] = torch.from_numpy(var_dict[k])

	# remove 1001-th class
	org_weight = var_dict['fc.weight'].clone()
	org_bias = var_dict['fc.bias'].clone()
	START = 1
	var_dict['fc.weight'] = org_weight.narrow(0,START,1000).squeeze()
	var_dict['fc.bias'] = org_bias.narrow(0,START,1000)

	# load model
	model.load_state_dict(var_dict, strict=False)


	print('Model Pretrained Loaded')