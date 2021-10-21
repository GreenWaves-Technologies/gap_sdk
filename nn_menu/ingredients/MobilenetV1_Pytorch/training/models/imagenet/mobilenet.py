#
# mobilenet.py
# Manuele Rusci <manuele.rusci@unibo.it>
#
# Copyright (C) 2019 University of Bologna
# All rights reserved.
#
# This is an implementation of the quantized mobilenet built from 
# https://github.com/marvis/pytorch-mobilenet/blob/master/main.py
#

import torch.nn as nn
import torch.utils.model_zoo as model_zoo
import math
import torchvision.transforms as transforms
import torch.nn.functional as F

from ..linear_quantized_modules import ClippedLinearQuantization, LearnedClippedLinearQuantization, Conv2d_SAME
from .preload_mobilenet import preload_mobilenet_tf


###### Full Precision Blocks #############
def conv_dw(inp, oup, stride, pad1=0, bias_ena=False):
    if pad1 == 1:
        return nn.Sequential(
            nn.ConstantPad2d(1, -1), 
            nn.Conv2d(inp, inp, 3, stride, 0, groups=inp, bias=bias_ena),
            nn.BatchNorm2d(inp),
            nn.ReLU6(inplace=False)
        )
    else:
        return nn.Sequential(
            nn.Conv2d(inp, inp, 3, stride, 1, groups=inp, bias=bias_ena),
            nn.BatchNorm2d(inp),
            nn.ReLU6(inplace=False)
        )

def conv_pw(inp, oup, stride,bias_ena=False):
    return nn.Sequential(
        nn.Conv2d(inp, oup, 1, 1, 0, bias=bias_ena),
        nn.BatchNorm2d(oup),
        nn.ReLU6(inplace=False)
    )

def conv_bn(inp, oup, stride):
    return nn.Sequential(
        nn.Conv2d(inp, oup, 3, stride, 1, bias=False),
        nn.BatchNorm2d(oup),
        nn.ReLU6(inplace=False)
    )

###### Quantized Blocks ##############################
# this should be removed and automatized
def get_quant_activ_layer(activ_type=None, act_bits=8):
    if activ_type == 'clipped':
        act_layer = ClippedLinearQuantization(num_bits=act_bits, clip_val=6)
    elif activ_type == 'learned':
        act_layer = LearnedClippedLinearQuantization(num_bits=act_bits, init_act_clip_val=6)
    elif activ_type == 'fixed1':
        act_layer = QuantActive(num_bits=act_bits)
    elif activ_type == 'binary':
        act_layer = BinActive()
    else:
        act_layer = nn.ReLU6(inplace=True)

    return act_layer


def conv_dw_quant(inp, oup, stride, activ_type=None, act_bits=8, bias_ena=False):
    return nn.Sequential(
        Conv2d_SAME(inp, inp, 3, stride, 1, groups=inp, bias=bias_ena),
        nn.BatchNorm2d(inp),
        get_quant_activ_layer(activ_type, act_bits)
    )

def conv_pw_quant(inp, oup, stride, activ_type=None, act_bits=8, bias_ena=False):
    return nn.Sequential(
        Conv2d_SAME(inp, oup, 1, 1, 0, bias=bias_ena),
        nn.BatchNorm2d(oup),
        get_quant_activ_layer(activ_type, act_bits)
    )

def conv_bn_quant(inp, oup, stride, activ_type=None, act_bits=8):
    return nn.Sequential(
        Conv2d_SAME(inp, oup, 3, stride, 1, bias=False),
        nn.BatchNorm2d(oup),
        get_quant_activ_layer(activ_type, act_bits)
    )


class mobilenet_real(nn.Module):
    def __init__(self, width_mult=1.0, input_dim = 224):
        super(mobilenet_real, self).__init__()
        print(width_mult, input_dim)

        if input_dim == 224:
            avg_size = 7
            crop_size = 256
        elif input_dim == 192:
            avg_size = 6
            crop_size = 220
        elif input_dim == 160:
            avg_size = 5
            crop_size = 180
        elif input_dim == 128:
            avg_size = 4
            crop_size = 146
        else:
            return -1
        self.width_mult = width_mult
        self.model = nn.Sequential(
            conv_bn(                  3,  int(width_mult* 32), 2), 
            conv_dw( int(width_mult* 32), int(width_mult* 64), 1),
            conv_pw( int(width_mult* 32), int(width_mult* 64), 1),
            conv_dw( int(width_mult* 64), int(width_mult*128), 2),
            conv_pw( int(width_mult* 64), int(width_mult*128), 2),
            conv_dw( int(width_mult*128), int(width_mult*128), 1),           
            conv_pw( int(width_mult*128), int(width_mult*128), 1),       
            conv_dw( int(width_mult*128), int(width_mult*256), 2),
            conv_pw( int(width_mult*128), int(width_mult*256), 2),
            conv_dw( int(width_mult*256), int(width_mult*256), 1),           
            conv_pw( int(width_mult*256), int(width_mult*256), 1),
            conv_dw( int(width_mult*256), int(width_mult*512), 2),
            conv_pw( int(width_mult*256), int(width_mult*512), 2),
            conv_dw( int(width_mult*512), int(width_mult*512), 1),
            conv_pw( int(width_mult*512), int(width_mult*512), 1),           
            conv_dw( int(width_mult*512), int(width_mult*512), 1),
            conv_pw( int(width_mult*512), int(width_mult*512), 1),           
            conv_dw( int(width_mult*512), int(width_mult*512), 1),           
            conv_pw( int(width_mult*512), int(width_mult*512), 1),           
            conv_dw( int(width_mult*512), int(width_mult*512), 1),           
            conv_pw( int(width_mult*512), int(width_mult*512), 1),           
            conv_dw( int(width_mult*512), int(width_mult*512), 1),           
            conv_pw( int(width_mult*512), int(width_mult*512), 1),            
            conv_dw( int(width_mult*512), int(width_mult*1024), 2),            
            conv_pw( int(width_mult*512), int(width_mult*1024), 2),
            conv_dw( int(width_mult*1024), int(width_mult*1024), 1),            
            conv_pw( int(width_mult*1024), int(width_mult*1024), 1),
            nn.AvgPool2d(avg_size),
        )
        self.fc = nn.Linear( int(width_mult*1024), 1000) 

        self.regime = {
            0: {'optimizer': 'SGD', 'lr': 1e-1,
                'weight_decay': 1e-4, 'momentum': 0.9},
            10: {'lr': 5e-2},
            20: {'lr': 1e-2}, #, 'weight_decay': 0},
            30: {'lr': 5e-3},
            40: {'lr': 1e-3},
            50: {'lr': 5e-4},
            60: {'lr': 1e-4}
        }


        #prepocessing
        normalize = transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])

        self.input_transform = {
            'train': transforms.Compose([
                transforms.Scale(crop_size),
                transforms.RandomCrop(input_dim),
                transforms.RandomHorizontalFlip(),
                transforms.ToTensor(),
                normalize
            ]),
            'eval': transforms.Compose([
                transforms.Scale(crop_size),
                transforms.CenterCrop(input_dim),
                transforms.ToTensor(),
                normalize
            ])
        }

    def forward(self, x):
        x = self.model(x)
        x = x.view(-1, int(self.width_mult*1024))
        x = self.fc(x)
        return x

class mobilenet_quant_devel(nn.Module):
    def __init__(self, width_mult=1.0, input_dim = 224, activ_bits =None, weight_bits= None, activ_type='relu'):
        super(mobilenet_quant_devel, self).__init__()

        print('This is a quantized Mobilenet with alpha= ', width_mult, \
            ' input_size: ', input_dim, \
            ' activation bits: ', activ_bits, \
            ' weight bits: ', weight_bits, \
            ' activation type: ', activ_type )

        if input_dim == 224:
            avg_size = 7
            crop_size = 256
        elif input_dim == 192:
            avg_size = 6
            crop_size = 220
        elif input_dim == 160:
            avg_size = 5
            crop_size = 180
        elif input_dim == 128:
            avg_size = 4
            crop_size = 146
        else:
            return -1


        self.width_mult = width_mult
        self.model = nn.Sequential(
            conv_bn_quant(                   3, int(width_mult* 32), 2 , activ_type, activ_bits ), 
            conv_dw_quant( int(width_mult* 32), int(width_mult* 64), 1 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult* 32), int(width_mult* 64), 1 , activ_type, activ_bits ),
            conv_dw_quant( int(width_mult* 64), int(width_mult*128), 2 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult* 64), int(width_mult*128), 2 , activ_type, activ_bits ),
            conv_dw_quant( int(width_mult*128), int(width_mult*128), 1 , activ_type, activ_bits ),           
            conv_pw_quant( int(width_mult*128), int(width_mult*128), 1 , activ_type, activ_bits ),       
            conv_dw_quant( int(width_mult*128), int(width_mult*256), 2 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult*128), int(width_mult*256), 2 , activ_type, activ_bits ),
            conv_dw_quant( int(width_mult*256), int(width_mult*256), 1 , activ_type, activ_bits ),           
            conv_pw_quant( int(width_mult*256), int(width_mult*256), 1 , activ_type, activ_bits ),
            conv_dw_quant( int(width_mult*256), int(width_mult*512), 2 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult*256), int(width_mult*512), 2 , activ_type, activ_bits ),
            conv_dw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_dw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),
            conv_pw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_dw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_pw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_dw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_pw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_dw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),           
            conv_pw_quant( int(width_mult*512), int(width_mult*512), 1 , activ_type, activ_bits ),            
            conv_dw_quant( int(width_mult*512), int(width_mult*1024), 2, activ_type, activ_bits ),            
            conv_pw_quant( int(width_mult*512), int(width_mult*1024), 2, activ_type, activ_bits ),
            conv_dw_quant( int(width_mult*1024), int(width_mult*1024),1, activ_type, activ_bits ),            
            conv_pw_quant( int(width_mult*1024), int(width_mult*1024),1, activ_type, activ_bits ),
            nn.AvgPool2d(avg_size),
        )
        self.fc = nn.Linear( int(width_mult*1024), 1000) 

        for m in self.model.modules():
            if 'BatchNorm' in m.__class__.__name__:
                m.eps = 0.001
                m.momentum = 0.003        


        self.regime = {
            0: {'optimizer': 'Adam', 'lr': 1e-4 },
            5: {'lr': 5e-5},
            8: {'lr': 1e-5 }
        }
        #prepocess as BNN
        normalize = transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])

        self.input_transform = {
            'train': transforms.Compose([
                transforms.Scale(crop_size),
                transforms.RandomCrop(input_dim),
                transforms.RandomHorizontalFlip(),
                #transforms.Grayscale(num_output_channels=3),
                transforms.ToTensor(),
                normalize
            ]),
            'eval': transforms.Compose([
                transforms.Scale(crop_size),
                transforms.CenterCrop(input_dim),
                #transforms.Grayscale(num_output_channels=3),
                transforms.ToTensor(),
                normalize
            ])
        }

    def forward(self, x):
        x = self.model(x)
        x = x.view(-1, int(self.width_mult*1024))
        x = F.dropout(x, 0.2, self.training)
        x = self.fc(x)
        return x





def mobilenet(type_quant= None, activ_bits =None, weight_bits= None, activ_type='hardtanh',  width_mult=1.0, input_dim = 224, pretrained=True, **kwargs):
    print(','.join('{0}={1!r}'.format(k,v) for k,v in kwargs.items()))

    print(activ_bits, weight_bits, type_quant)


    if type_quant in ['PerLayerAsymMinMax','PerLayerSymPACT','PerLayerAsymPACT','PerChannelsAsymMinMax']:
      model = mobilenet_quant_devel(width_mult, input_dim, activ_bits, weight_bits, 'real')
      if pretrained:
        preload_mobilenet_tf( model , input_dim, width_mult)
      return model

    else:
      model = mobilenet_real(width_mult, input_dim)
      if pretrained:
        preload_mobilenet_tf( model , input_dim, width_mult)
      return model

