#
# quantop.py
# Manuele Rusci <manuele.rusci@unibo.it>
#
# Copyright (C) 2019 University of Bologna
# All rights reserved.
#

import torch, math
import torch.nn as nn
import numpy as np
from scipy.linalg import lstsq
from torch.autograd import Variable
import copy
import torch.utils.tensorboard
from collections import OrderedDict

from .quant_auto import memory_driven_quant

import sys
sys.path.append('../')


import models

from models.linear_quantized_modules import ClippedLinearQuantization, LearnedClippedLinearQuantization, ScaledClippedLinearQuantization, ScaledThresholdsQuantization4d, \
                                            Conv2d_SAME, ScaledClippedLinearQuantizationChannel, ScaledClippedLinearQuantizationChannelBias, Round, \
                                            LearnedClippedSymLinearQuantization, LearnedClippedSymLinearQuantizationADD

_OP_LIST = ['onnx::Gemm','onnx::BatchNormalization','onnx::Conv','onnx::Clip','onnx::AveragePool','onnx::Reshape']
_IO_LIST = ['IO Node']

############################################################################
####        Utils
############################################################################

def has_children(module):
    try:
        next(module.children())
        return True
    except StopIteration:
        return False


def get_activ_stats(layer_int):
    a_bits = layer_int['act_o_bits']
    n_levels_a = (2**a_bits)-1
    if type(layer_int['act']) == ClippedLinearQuantization:
        a_min = 0
        a_max = layer_int['act'].clip_val
        S_a =  a_max / n_levels_a
        Z_a = 0
    elif type(layer_int['act']) ==  LearnedClippedLinearQuantization:
        a_min = 0
        a_max = layer_int['act'].clip_val.data.item()
        S_a = a_max / n_levels_a
        Z_a = 0
    elif type(layer_int['act']) ==  QuantActive:
        a_min = -1
        a_max = 1-2**(-a_bits)
        S_a = (a_max-a_min) / n_levels_a
        Z_a = 0
    return S_a, Z_a, n_levels_a, a_min, a_max


def get_first_layer_stats():
# corresponds to the 8 bit quatized inputs of the imagenet dataset
    a_min, a_max = -1, 1
    n_levels_a = (2**8)-1
    S_a = (a_max - a_min) / n_levels_a
    Z_a = 0.0
    return S_a, Z_a, n_levels_a, a_min, a_max


def find_conv_node(node,g):
    ret = 0
    for j, x in enumerate(node.input):
        id = get_model_id(x,g)
        n = g.node[id]
        print('Input', j, 'is:', n.op)

        if n.op in _IO_LIST:
            ret = -1
            break
        elif 'onnx::' in n.op:
            if n.op in ['onnx::Gemm','onnx::Conv']:
                ret = id
                break
            else:
                ret = find_conv_node(n,g)
                if ret is not 0:
                    break     
        elif 'aten::' in n.op:
            ret = find_conv_node(n,g)
            if ret is not 0:
                break         
    return ret

def get_model_name(a):
    s = ''
    while a.find('[') > 0:
        s += a[a.find('[')+1:a.find(']')]+'.'
        a = a[a.find(']')+1:]
    return s[:-1]

def get_model_id(a, g):
    for i, node in enumerate(g.node):
        if node.name == a:
            return i
    print('ID  not found!')
    return -1

############################################################################
####   		Quantization and Binarization Library
############################################################################


'''
    The QuantOp operator handles the quantization process of model's parameters.
    This is inspired from the code in https://github.com/jiecaoyu/XNOR-Net-PyTorch

    Implemented quantization approaches:
    - 'PerLayerAsymMinMax', apply per-layer asymmetric weight quantization w/ min-max range
    - 'PerLayerAsymPACT', apply per-layer asymmetric weight quantization w/ PACT strategy
    - 'PerChannelsAsymMinMax', apply per-channel asymmetric weight quantization w/ min-max range
'''

_Availble_Quant_Type = ['PerLayerAsymMinMax', 'PerLayerSymPACT', 'PerLayerAsymPACT', 'PerChannelsAsymMinMax', None]
_Supported_Activ_Funct = [nn.ReLU6, nn.ReLU, ClippedLinearQuantization, LearnedClippedLinearQuantization, LearnedClippedSymLinearQuantization, LearnedClippedSymLinearQuantizationADD]



class QuantOp():



    def __init__(self, 
        model, 
    	quant_type = None, 
        weight_bits= 8,
        bias_bits = 15,
        batch_fold_type = 'folding_weights', 
        batch_fold_delay = 0, 
        act_bits = 8,
        add_config = [], 
        dummy_input = None,
        graph_mode=True
    ):
        ''' 
        Description: Wraps the full-precision model to apply weight quantization and build the deployments graph
		Arguments:
		- model:		        model to quantize
		- quant_type:	        'None' default type of per-layer quantization for the weights
		- weight_bits:	        default number of bits for the weights
        - batch_fold_delay:     number of epochs before folding batch normalization layers
		- act_bits:	            default number of bits for the activations
		'''

        # generate quantized model
        self.fake_model = model
        self.deployment_model = copy.deepcopy(model)
        self.param_to_quantize = []
        self.batch_fold = False
        self.batch_fold_delay = batch_fold_delay
        self.batch_fold_type = batch_fold_type

        print('Batch Folding: ', self.batch_fold, 'Batch Folding Delay: ', batch_fold_delay, 'Type', batch_fold_type )


        if graph_mode:

            # produce graph
            g,v = torch.utils.tensorboard._pytorch_graph.graph(model,dummy_input)
            #start
            ###################################################
            ##### Mapping the graph ###########################
            ###################################################
            last_layer = None
            replace_graph = {}

            m = OrderedDict(model.named_modules())


            for i,node in enumerate(g.node):

                if node.op in _IO_LIST:
                    print('************')
                    print(node.name)
                    print(node.op)
                    
                if node.op in _OP_LIST:
                    print('************')
                    name = get_model_name(node.name)
                    module = m[name]
                    idx = get_model_id(node.name,g)
                    print(node.name,node.op)
                    
                    if node.op in ['onnx::Gemm','onnx::Conv']:
                        layer_quant_descr = {}
                        # saving per-node characteristics
                        layer_quant_descr['w_bits'] = weight_bits
                        layer_quant_descr['fold_type'] = batch_fold_type

                        if quant_type in _Availble_Quant_Type :
                            layer_quant_descr['quant_type'] = quant_type
                        else:
                            print('Type of quantization not recognized')

                        layer_quant_descr['conv'] = module
                        layer_quant_descr['weight'] = module.weight.data.clone()
                        layer_quant_descr['w_clip'] = None
                        layer_quant_descr['id'] = idx
                        layer_quant_descr['leaf'] = True

                        # import per-layer config from external config file
                        idx_layer = len(self.param_to_quantize)
                        for item_dict in add_config:
                            if item_dict['layer'] == idx_layer:
                                if 'w_bits' in item_dict.keys():
                                    layer_quant_descr['w_bits'] = item_dict['w_bits'] 

                                if 'fold_type' in item_dict.keys():
                                    layer_quant_descr['fold_type'] = item_dict['fold_type'] 

                                if 'quant_type' in item_dict.keys():
                                    layer_quant_descr['quant_type'] = item_dict['quant_type'] 

                        if module.bias is None:
                            layer_quant_descr['bias'] = False
                        else:
                            layer_quant_descr['bias'] = module.bias.data.clone()
                        layer_quant_descr['bias_bits'] = bias_bits
                        layer_quant_descr['batch_norm'] = None
                        layer_quant_descr['pool'] = None
                        layer_quant_descr['act'] = None
                        layer_quant_descr['quant_act'] = None

                        # append into deployment graph
                        quant_layer = copy.deepcopy(module)
                        layer_quant_descr['quant_conv'] = quant_layer

                        #PACT needs extra parameters for learning quantization range
                        if layer_quant_descr['quant_type'] == 'PerLayerAsymPACT':
                            layer_quant_descr['w_max_thr'] = nn.Parameter( module.weight.data.max().cuda(),requires_grad=True )
                            layer_quant_descr['w_min_thr'] = nn.Parameter( module.weight.data.min().cuda(),requires_grad=True )
                            layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad
                            layer_quant_descr['w_min_thr'].add(1).sum().backward() #fake to create grad

                        elif layer_quant_descr['quant_type'] == 'PerLayerSymPACT':
                            layer_quant_descr['w_max_thr'] = nn.Parameter( module.weight.data.max().cuda(),requires_grad=True )
                            layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad

                        elif layer_quant_descr['quant_type'] == 'PerChannelsAsymMinMax':
                            weight = module.weight.data
                            max_weight,_ = weight.reshape(weight.size(0), -1).max(1)
                            min_weight,_ = weight.reshape(weight.size(0), -1).min(1)
                            layer_quant_descr['w_max_thr'] = nn.Parameter( max_weight.cuda(),requires_grad=True )
                            layer_quant_descr['w_min_thr'] = nn.Parameter( min_weight.cuda(),requires_grad=True )
                            layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad
                            layer_quant_descr['w_min_thr'].add(1).sum().backward() #fake to create grad
                            layer_quant_descr['w_min_mat'] = None
                            layer_quant_descr['w_max_mat'] = None

                            #FIXME: this part is not supported with graph yet
                            if type(module) == nn.Linear:
                                print('PerLayerAsymPACT chennel with fixed batch')
                                out_ch = weight.size(0)
                                quant_act = ScaledClippedLinearQuantizationChannelBias(out_ch, clip_val=False, bias=False)
                                layer_quant_descr['quant_act'] = quant_act

                        # append the latest node
                        self.param_to_quantize.append(layer_quant_descr)
                        replace_graph[name] = [module, quant_layer]

                        id_layer = find_conv_node(node,g)
                        print('The previos layer ID is: ', id_layer)
                        if id_layer == -1:
                            layer_quant_descr['input'] = -1
                        else:
                            for j, item in enumerate(self.param_to_quantize):
                                if item['id'] == id_layer:
                                    layer_quant_descr['input'] = j
                                    self.param_to_quantize[j]['leaf'] = False
                                    break
                        print('The previos layer is: ', id_layer, layer_quant_descr['input'])

                    elif node.op in ['onnx::BatchNormalization']:
                        id_layer = find_conv_node(node,g)
                        for item in self.param_to_quantize:
                            if item['id'] == id_layer:
                                item['batch_norm'] = module
                                break
                                
                        replace_graph[name] = [module, None]

                    elif node.op in ['onnx::AveragePool']:
                        roundAvg = nn.Sequential(copy.deepcopy(module), Round() )
                        replace_graph[name] = [module, roundAvg]

                        id_layer = find_conv_node(node,g)
                        for item in self.param_to_quantize:
                            if item['id'] == id_layer:
                                item['pool'] = roundAvg
                                break

                    elif node.op in ['onnx::Clip']:
                        
                        id_layer = find_conv_node(node,g)
                        for item in self.param_to_quantize:
                            if item['id'] == id_layer:
                                last_layer = item
                                break

                        # quantized activations
                        if type(module) in [nn.ReLU6, nn.ReLU]:
                            act_bits = act_bits
                            #check if number of bits need to be changed
                            for item_dict in add_config:
                                if item_dict['layer'] == idx_layer:
                                    if 'a_bits' in item_dict.keys():
                                        act_bits = item_dict['a_bits'] 
                                        module.num_bits = act_bits                                 

                            if last_layer['fold_type'] == 'folding_thresh':
                                quant_act = ScaledThresholdsQuantization4d()
                                quant_act.num_bits = act_bits
                                quant_act.n_thresholds = 2**act_bits -1

                            elif last_layer['fold_type'] == 'ICN':
                                out_ch = last_layer['conv'].out_channels
                                quant_act = ScaledClippedLinearQuantizationChannel(out_ch,clip_val=2**act_bits -1)

                            else:
                                quant_act = ScaledClippedLinearQuantization(clip_val=2**act_bits -1)
                            
                            fake_quant_act = LearnedClippedLinearQuantization(init_act_clip_val=6,num_bits=act_bits)
                            last_layer['act'] = fake_quant_act
                            last_layer['act_o_bits'] = act_bits

                        else:
                            print('Supported activation layer but no method is here yet!')
                        
                        last_layer['quant_act'] = quant_act
                        replace_graph[name] = [fake_quant_act, quant_act]
                        
                    else:
                        print('Else name = ', name)
                        replace_graph[name] = [module, copy.deepcopy(module)]


            ###################################################
            ##### Generate Quantized Models ###################
            ###################################################
            for name_sub,submodel in model.named_children():
                # map the sub-graph 
                modules_quant = [] 
                modules_fake = []
                for name, module in submodel.named_modules():
                    if has_children(module) is False:
                        if name is not '':
                            n = name_sub+'.'+name
                        else:
                            n = name_sub
                        v = replace_graph[n]
                        fake = v[0]
                        other = v[1]
                        if fake is not None:
                            modules_fake.append(fake)
                        
                        if other is not None:
                            modules_quant.append(other)
                        
                
                self.fake_model._modules[name_sub] = nn.Sequential(*modules_fake)
                self.deployment_model._modules[name_sub] = nn.Sequential(*modules_quant)

            
            self.n_quantize_layers = len(self.param_to_quantize)

            print('This is the fake quantized model')
            print(self.fake_model)

            print('This is the quantized network: ')
            print(self.deployment_model)

        else:
            ###################################################
            ##### Not Graph mode ##############################
            ###################################################
            inputFound = False
            for name_sub,submodel in model.named_children():

                # map the sub-graph 
                for name, module in submodel.named_modules():
                    if type(module) is models.imagenet.mobilenetv2.InvertedResidual:
                        print(type(module))
                    elif has_children(module) is False:
                        
                        layer_quant_descr = {}

                        if type(module) in [nn.Conv2d, nn.Linear, Conv2d_SAME]:
                            
                            # saving per-node characteristics
                            layer_quant_descr['name'] = name
                            layer_quant_descr['w_bits'] = weight_bits
                            layer_quant_descr['fold_type'] = self.batch_fold_type

                            temp = getattr(submodel, 'quant_type', quant_type)
                            if temp in _Availble_Quant_Type :
                                layer_quant_descr['quant_type'] = temp
                            else:
                                print('Type of quantization not recognized')
                                return -1
                            layer_quant_descr['conv'] = module
                            layer_quant_descr['weight'] = module.weight.data.clone()
                            layer_quant_descr['w_clip'] = None
                            if not inputFound:
                                inputFound = True
                                layer_quant_descr['input'] = -1
                            else:
                                layer_quant_descr['input'] = 0

                            # import per-layer config from external config file
                            idx_layer = len(self.param_to_quantize)
                            for item_dict in add_config:
                                if item_dict['layer'] == idx_layer:
                                    if 'w_bits' in item_dict.keys():
                                        layer_quant_descr['w_bits'] = item_dict['w_bits'] 

                                    if 'fold_type' in item_dict.keys():
                                        layer_quant_descr['fold_type'] = item_dict['fold_type'] 

                                    if 'quant_type' in item_dict.keys():
                                        layer_quant_descr['quant_type'] = item_dict['quant_type'] 

                            if module.bias is None:
                                layer_quant_descr['bias'] = False
                            else:
                                layer_quant_descr['bias'] = module.bias.data.clone()
                            layer_quant_descr['bias_bits'] = bias_bits
                            layer_quant_descr['batch_norm'] = None
                            layer_quant_descr['act'] = None
                            layer_quant_descr['quant_act'] = None

                            # append into deployment graph
                            quant_layer = module
                            layer_quant_descr['quant_conv'] = quant_layer

                            #PACT needs extra parameters for learning quantization range
                            if layer_quant_descr['quant_type'] == 'PerLayerAsymPACT':
                                layer_quant_descr['w_max_thr'] = nn.Parameter( module.weight.data.max().cuda(),requires_grad=True )
                                layer_quant_descr['w_min_thr'] = nn.Parameter( module.weight.data.min().cuda(),requires_grad=True )
                                layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad
                                layer_quant_descr['w_min_thr'].add(1).sum().backward() #fake to create grad

                            elif layer_quant_descr['quant_type'] == 'PerLayerSymPACT':
                                layer_quant_descr['w_max_thr'] = nn.Parameter( module.weight.data.max().cuda(),requires_grad=True )
                                layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad

                            elif layer_quant_descr['quant_type'] == 'PerChannelsAsymMinMax':
                                weight = module.weight.data
                                max_weight,_ = weight.reshape(weight.size(0), -1).max(1)
                                min_weight,_ = weight.reshape(weight.size(0), -1).min(1)
                                layer_quant_descr['w_max_thr'] = nn.Parameter( max_weight.cuda(),requires_grad=True )
                                layer_quant_descr['w_min_thr'] = nn.Parameter( min_weight.cuda(),requires_grad=True )
                                layer_quant_descr['w_max_thr'].add(1).sum().backward() #fake to create grad
                                layer_quant_descr['w_min_thr'].add(1).sum().backward() #fake to create grad
                                layer_quant_descr['w_min_mat'] = None
                                layer_quant_descr['w_max_mat'] = None
                                if type(module) == nn.Linear:
                                    print('PerLayerAsymPACT chennel with fixed batch')
                                    out_ch = weight.size(0)
                                    quant_act = ScaledClippedLinearQuantizationChannelBias(out_ch, clip_val=False, bias=False)
                                    layer_quant_descr['quant_act'] = quant_act

                            # append the latest node
                            self.param_to_quantize.append(layer_quant_descr)
                            last_layer = self.param_to_quantize[-1]

                        elif (type(module) in [nn.BatchNorm2d]):
                            if last_layer is not None:
                                last_layer['batch_norm'] = module

                        elif type(module) in [nn.AvgPool2d]:
                            pass

                        elif type(module) in _Supported_Activ_Funct:

                            # check if a quantized features an activation function
                            if last_layer is not None:
                                
                                last_layer['act'] = module
                                
                                # quantized activations
                                if type(module) in [ClippedLinearQuantization, LearnedClippedLinearQuantization, \
                                                    LearnedClippedSymLinearQuantization, LearnedClippedSymLinearQuantizationADD,QuantActive]:

                                    act_bits = module.num_bits
                                    #check if number of bits need to be changed
                                    for item_dict in add_config:
                                        if item_dict['layer'] == idx_layer:
                                            if 'a_bits' in item_dict.keys():
                                                act_bits = item_dict['a_bits'] 
                                                module.num_bits = act_bits                                 

                                    if last_layer['fold_type'] == 'folding_thresh':
                                        quant_act = ScaledThresholdsQuantization4d()
                                        quant_act.num_bits = act_bits
                                        quant_act.n_thresholds = 2**act_bits -1

                                    elif last_layer['fold_type'] == 'ICN':
                                        out_ch = last_layer['conv'].out_channels
                                        quant_act = ScaledClippedLinearQuantizationChannel(out_ch,clip_val=2**act_bits -1)

                                    else:
                                        quant_act = ScaledClippedLinearQuantization(clip_val=2**act_bits -1)
                                    
                                    last_layer['act_o_bits'] = act_bits

                                # full-precision activations
                                elif  type(module) is nn.ReLU6:
                                    quant_act = nn.ReLU6(inplace=True)

                                # binary acivation
                                elif  type(module) is BinActive :
                                    act_bits = 1
                                    quant_act = BinActive()
                                
                                else:
                                    print('Supported activation layer but no method is here yet!')
                                    return 0
                                

                                last_layer['quant_act'] = quant_act


                        else:
                            print(type(module), 'not supported yet!')

                    else:
                        #if type(module) is not nn.Sequential:
                        last_layer = None

                

            
            self.n_quantize_layers = len(self.param_to_quantize)
            self.deployment_model = model



    # FIXME used only for Xnornet- remove me
    def __clamp_real_params(self, min_value, max_value):    
    # in-place clamp of real values parameters
        for index in range(self.num_of_params):
            self.target_modules[index].data.clamp_(min_value, max_value)

    # FIXME used only for Xnornet- remove me
    def __save_params(self):
    #store parameters weights in the saved_params tensor
        for index in range(self.num_of_params):
            self.saved_params[index].copy_(self.target_modules[index].data)


    def _get_m0_nexp_vect (self, data_vec):
        dim = data_vec.size(0)
        M0 = torch.Tensor(dim).type_as(data_vec)
        N0 = torch.Tensor(dim).type_as(data_vec)
        for i in range(dim):
            M0[i],N0[i] = self._get_m0_nexp(data_vec[i])

        return M0, N0

    def _get_m0_nexp(self, data_s):
        ''' Decompose data_s as data*2**n_exp '''
        n_exp = 0
        data = abs(data_s)
        if data == 0.0:
            print('Data is 0')
            return data, n_exp

        while data >= 1 or data < 0.5:
            if data >= 1:
                data = data / 2
                n_exp += 1
            else:
                data = data * 2
                n_exp -= 1

        if data_s < 0: 
            data = -data

        return data, n_exp


    def _linear_asymmetric_quant_args(self, data_tensor, n_bits ):
        ''' 
        Quantize data_tensor in min/max quantization range 
        and return quantized tensor
        '''
        max_value = data_tensor.max()
        min_value = data_tensor.min()
        range_values = max_value - min_value

        # to prevent division by 0 impose a quantization range of 1
        if range_values == 0:
            range_values = 1

        n_steps = (2**n_bits)-1
        eps = range_values / n_steps
        data_quant = data_tensor.div(eps).round().mul(eps)
        
        return data_quant, min_value, max_value

    def _linear_symmetric_quant_args(self, data_tensor, n_bits ):
        ''' 
        Quantize data_tensor in min/max quantization range 
        and return quantized tensor
        '''
        max_value = max(data_tensor.abs().max(),0)
        range_values = 2 * max_value

        # to prevent division by 0 impose a quantization range of 1
        if range_values == 0:
            range_values = 1

        n_steps = (2**n_bits)-1
        eps = range_values / n_steps
        data_quant = data_tensor.div(eps).round().clamp(-2**(n_bits-1),2**(n_bits-1)-1).mul(eps)
        
        return data_quant, -max_value, max_value

    def _clipped_linear_asymmetric_quant_args(self, data_tensor, n_bits, min_value, max_value):
        ''' Applying clipping before linear quantization     '''
        data_tensor.clamp_(min_value, max_value)
        return self._linear_asymmetric_quant_args(data_tensor, n_bits) 

    def compute_thresholds(self, conv_layer, batch_layer, a_bits, a_min, a_max, S_a_i, S_w, first=False ):
        ''' 
        Compute integer activation thresholds 
        '''
        # compute thresholds
        print('A bits: ', a_bits, ' | a_min: ',a_min, ' | a_max: ', a_max)
        bias = conv_layer.bias
        eps = batch_layer.eps
        mean = batch_layer.running_mean
        if bias is not None:
            mean += bias.data
        std = batch_layer.running_var.add(eps).sqrt()
        gamma = batch_layer.weight.data
        beta = batch_layer.bias.data
        
        #dim of channels
        dim = gamma.size(0)
        n_thresholds = 2**a_bits -1
        thresholds = torch.arange(n_thresholds).unsqueeze(0).expand(dim, n_thresholds).type_as(gamma)
        signs = gamma.new_empty(dim)

        #quantizer factors
        step_thr = (a_max-a_min)/n_thresholds
        offset_thr = a_min+step_thr/2
        scale = S_a_i * S_w

        #compute thresholds
        thresholds = thresholds.mul(step_thr).add(offset_thr)

        for i in range(dim):
            if  abs(std[i]/gamma[i]) > 1e30:
                #sign += '2, '
                print('Ratio = ', abs(std[i]/gamma[i]), 'layer=', i )
                pass
            elif gamma[i] > 0:
                signs[i] = 1
            else:
                signs[i] = 0

            thr_array = []
            for j in range(n_thresholds):
                thr = ( mean[i]+((thresholds[i,j]-beta[i])*(std[i]/gamma[i])) ) / scale
                if not first:
                    thresholds[i,j] = math.floor( thr )
                else:
                    thresholds[i,j] = thr

        return signs, thresholds


    def _get_BN_scale_factors(self, batch_layer):

        eps = batch_layer.eps
        gamma_tensor = batch_layer.weight.data 
        beta_tensor = batch_layer.bias.data 
        mu_tensor = batch_layer.running_mean
        var_tensor = batch_layer.running_var
        
        # check for instabilities
        if var_tensor.le(0).sum() >0:
            print('erorr')
            return

        sigma_tensor = var_tensor.add(eps).sqrt()
        gamma_over_sigma = gamma_tensor / sigma_tensor

        return gamma_over_sigma, mu_tensor, beta_tensor



    def _batch_fold(self,conv_layer, batch_layer ):
        ''' 
            Folding of batch normalization parameters into conv_layer params
            Return folded conv_layer tensors
        '''
        weight_tensor = conv_layer.weight.data.clone()
        weight_tensor_size = weight_tensor.size()
        eps = batch_layer.eps
        gamma_tensor = batch_layer.weight.data 
        beta_tensor = batch_layer.bias.data 
        mu_tensor = batch_layer.running_mean
        var_tensor = batch_layer.running_var
        
        #assuming convolution bias == False
        bias_tensor = mu_tensor.clone() 
        bias_tensor = bias_tensor.mul(-1)

        # check for instabilities
        if var_tensor.le(0).sum() >0:
            print('erorr')
            return

        sigma_tensor = var_tensor.add(eps).sqrt()
        gamma_over_sigma = gamma_tensor / sigma_tensor
        gamma_over_sigma_tensor = gamma_over_sigma.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(weight_tensor_size )
        weight_tensor = weight_tensor * gamma_over_sigma_tensor
        bias_tensor = (bias_tensor*gamma_over_sigma)+beta_tensor

        return weight_tensor, bias_tensor

    def _batch_defold(self, weight_tensor, bias_tensor, batch_layer):
        ''' 
            De-Foldingconv_layer params from batch-norm parameters
            This is needed for fake-quantized graph
        '''
        weight_tensor_size = weight_tensor.size()
        eps = batch_layer.eps
        gamma_tensor = batch_layer.weight.data 
        beta_tensor = batch_layer.bias.data 
        mu_tensor = batch_layer.running_mean
        var_tensor = batch_layer.running_var

        # check for instabilities
        if var_tensor.le(0).sum() >0:
            print('erorr')
            return

        sigma_tensor = var_tensor.add(eps).sqrt()
        gamma_over_sigma = gamma_tensor / sigma_tensor
        gamma_over_sigma_tensor = gamma_over_sigma.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(weight_tensor_size )

        weight_tensor = torch.where(gamma_over_sigma_tensor.eq(0.0), gamma_over_sigma_tensor, weight_tensor / gamma_over_sigma_tensor)  
        if bias_tensor is not False:
            bias_tensor = torch.where(gamma_over_sigma.eq(0.0), bias_tensor, ((bias_tensor-beta_tensor)/ gamma_over_sigma)+mu_tensor)

        return weight_tensor, bias_tensor


    def add_params_to_optimizer(self, optimizer):
        ''' 
            Adding PACT weights parameters to the optimizer
        '''
        #for key, value in dict(self.fake_model.named_parameters()).items():
        #    if 'clip_val' in key:
        #        optimizer.add_param_group({"params": value, 'weight_decay': 5e-4})

        for layer in self.param_to_quantize:
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']
            if quant_type in ['PerLayerAsymPACT']:
                optimizer.add_param_group({"params": layer['w_min_thr'], 'weight_decay': 5e-4})
                optimizer.add_param_group({"params": layer['w_max_thr'], 'weight_decay': 5e-4})
            elif quant_type in ['PerLayerSymPACT']:
                optimizer.add_param_group({"params": layer['w_max_thr'], 'weight_decay': 5e-4})



    def init_parameters(self ):
        ''' 
            Init PACT parameters
        '''
        for layer in self.param_to_quantize:
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']

            if quant_type == 'PerLayerAsymPACT':

                if layer['batch_norm'] is not None and layer['fold_type'] == 'folding_weights':
                    batch_layer = layer['batch_norm']
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    layer['w_max_thr'].data =  weight_tensor.max().cuda()
                    layer['w_min_thr'].data =  weight_tensor.min().cuda() 

                ## PPQ
                elif layer['fold_type'] == 'ICN':
                    weights = conv_layer.weight.data
                    w_levels = 2**w_bits -1
                    mult = (w_levels/2)
                    mean_w = weights.mean()
                    gamma= weights.add(-mean_w).abs().max().cuda().mul(2/w_levels)
                    for i in range(10):
                        gamma_zero = gamma

                        mult = (w_levels/2)
                        max_v = max(mean_w + gamma_zero*mult, 0)
                        min_v = min(mean_w - gamma_zero*mult ,0)
                        quant_weights = weights.clamp(min_v,max_v).div(gamma_zero).round()                        
                        sum_quant = quant_weights.mul(quant_weights).sum()
                        sum_proj = quant_weights.mul(weights).sum()
                        gamma = sum_proj /  sum_quant

                    layer['w_max_thr'].data.fill_(max(mean_w + gamma_zero*mult, 0))
                    layer['w_min_thr'].data.fill_(min(mean_w - gamma_zero*mult ,0))                    

                else:
                    layer['w_max_thr'].data =  conv_layer.weight.data.max().cuda()
                    layer['w_min_thr'].data =  conv_layer.weight.data.min().cuda()

            elif quant_type == 'PerLayerSymPACT':

                if layer['batch_norm'] is not None and layer['fold_type'] == 'folding_weights':
                    batch_layer = layer['batch_norm']
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    layer['w_max_thr'].data =  weight_tensor.abs().max().cuda()
                else:
                    layer['w_max_thr'].data =  conv_layer.weight.data.abs().max().cuda()

            # FIXME: this must be moved from min/max to PACT
            elif quant_type == 'PerChannelsAsymMinMax':

                if layer['batch_norm'] is not None and layer['fold_type'] == 'folding_weights':

                    batch_layer = layer['batch_norm']
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    max_weight,_ = weight_tensor.reshape(weight_tensor.size(0), -1).max(1)
                    min_weight,_ = weight_tensor.reshape(weight_tensor.size(0), -1).min(1)
                    layer['w_max_thr'] = nn.Parameter( max_weight.cuda(), requires_grad=True )
                    layer['w_min_thr'] = nn.Parameter( min_weight.cuda(), requires_grad=True )

                ## PPQ
                elif layer['fold_type'] == 'ICN':

                    weights = conv_layer.weight.data
                    w_levels = 2**w_bits -1
                    mult = (w_levels/2)

                    for v in range(weights.size(0)):
                        mean_w = weights[v].mean()

                        gamma= weights[v].add(-mean_w).abs().max().cuda().mul(2/w_levels)

                        for i in range(10):
                            gamma_zero = gamma

                            mult = (w_levels/2)
                            max_v = max(mean_w + gamma_zero*mult, 0)
                            min_v = min(mean_w - gamma_zero*mult ,0)
                            quant_weights = weights[v].clamp(min_v,max_v).div(gamma_zero).round()                        
                            sum_quant = quant_weights.mul(quant_weights).sum()
                            sum_proj = quant_weights.mul(weights[v]).sum()
                            gamma = sum_proj /  sum_quant

                        layer['w_max_thr'].data[v].fill_(max(mean_w + gamma_zero*mult, 0))
                        layer['w_min_thr'].data[v].fill_(min(mean_w - gamma_zero*mult ,0))                    

                else:

                    weight = conv_layer.weight.data
                    max_weight,_ = weight.reshape(weight.size(0), -1).max(1)
                    min_weight,_ = weight.reshape(weight.size(0), -1).min(1)
                    layer['w_max_thr'].data = max_weight.cuda() 
                    layer['w_min_thr'].data = min_weight.cuda() 




    def freeze_BN_and_fold(self, epoch):
        ''' 
            Freeze BN layer params after batch_fold_delay epochs
            When batch_fold is True, BN params are frozen and folding is applied if requested
        '''

        old_batch_fold = self.batch_fold

        #enable batch-folding
        if epoch >= self.batch_fold_delay:
            self.batch_fold  = True
            print("Frozen Batch Normalization Layers statistics!")

#        #freeze batch norm stas and parameters
#        if epoch >= self.batch_fold_delay: #fixme
#            self.batch_freeze  = True
#        else:
#            self.batch_freeze  = False

        change_fold_mode = self.batch_fold and (not old_batch_fold)  
        print('Changing Folding Mode: ', change_fold_mode)


        for i_l,layer in enumerate(self.param_to_quantize):
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']

            # freeze BN parametes
            if quant_type in ['PerLayerAsymMinMax','PerLayerAsymPACT','PerLayerSymPACT','PerChannelsAsymMinMax'] and layer['batch_norm'] is not None and self.batch_fold:
                batch_layer = layer['batch_norm']
                batch_layer.eval()
                batch_layer.weight.requires_grad = False
                batch_layer.bias.requires_grad = False

                if torch.isnan(batch_layer.running_mean).sum() > 0:
                    print('Runnin Mean of layer ',i_l)
                    print(batch_layer.running_mean)

                if torch.isnan(batch_layer.running_var).sum() > 0:
                    print('Runnin Var of layer ',i_l)
                    print(batch_layer.running_var)


            # manage min/max params when activate batch folding into weights
            if quant_type == 'PerLayerAsymPACT' and change_fold_mode and layer['fold_type'] == 'folding_weights':
                if self.batch_fold and layer['batch_norm'] is not None:
                    batch_layer = layer['batch_norm']
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    layer['w_max_thr'].data =  weight_tensor.max().cuda()
                    layer['w_min_thr'].data =  weight_tensor.min().cuda()

            # FIXME this must be moved to the case of PC + PACT 
            elif quant_type == 'PerChannelsAsymMinMax' and change_fold_mode and layer['fold_type'] == 'folding_weights':
                if self.batch_fold and layer['batch_norm'] is not None:
                    batch_layer = layer['batch_norm']
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    max_weight,_ = weight_tensor.reshape(weight_tensor.size(0), -1).max(1)
                    min_weight,_ = weight_tensor.reshape(weight_tensor.size(0), -1).min(1)
                    layer['w_max_thr'].data = max_weight.cuda()
                    layer['w_min_thr'].data = min_weight.cuda()

            elif quant_type == 'PerLayerSymPACT' and change_fold_mode and layer['fold_type'] == 'folding_weights':
                if self.batch_fold and layer['batch_norm'] is not None:
                    batch_layer = layer['batch_norm']
                    max_value = conv_layer.weight.data.abs().max().item()
                    weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
                    layer['w_max_thr'].data =  weight_tensor.abs().max().cuda() 
#                    scale_factor = weight_tensor.abs().max().item() / max_value




    def _quant_PerLayerAsymMinMax(self, conv_layer, batch_norm_layer, batch_fold_type, w_bits, idx_layer, \
        training=False, get_quantized=True):

        # 1- folding of batch normalization layers before quantization if enabled and for validation
        if (self.batch_fold ) and ( batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            #folding batch_norm into convolution and quantize
            batch_layer = batch_norm_layer
            weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)

        else: # weights and bias as-it-is
            weight_tensor = conv_layer.weight.data.clone()
            if conv_layer.bias is None:
                bias_tensor = False
            else:
                bias_tensor = conv_layer.bias.data.clone()

        # return tensor data before quantization
        if not get_quantized:
            return weight_tensor, bias_tensor
        
        # 2- quantization
        weight_quant_tensor, w_min_value, w_max_value = self._linear_asymmetric_quant_args(weight_tensor, w_bits)

        if bias_tensor is not False:
            S_w = ( weight_tensor.max()-weight_tensor.min() ) / ((2**w_bits)-1)

            inp_layer = self.param_to_quantize[idx_layer]['input']
            if inp_layer == -1 :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_first_layer_stats()                       
            else :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])

            eps = S_a_i * S_w
            bias_quant_tensor = bias_tensor.div(eps).round().mul(eps)
        else:
            bias_quant_tensor = False


        # 3- batch norm defolding
        if (self.batch_fold ) and (batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            weight_quant_tensor, bias_quant_tensor = self._batch_defold(weight_quant_tensor, bias_quant_tensor, batch_layer)

        return weight_quant_tensor, bias_quant_tensor

    def _quant_PerLayerSymPACT(self, conv_layer, batch_norm_layer, batch_fold_type, w_bits, bias_bits, idx_layer, \
        w_min_ext, w_max_ext, training=False, get_quantized=True):

        # 1- folding of batch normalization layers before quantization if enabled and for validation
        if self.batch_fold  and ( batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            #folding batch_norm into convolution and quantize
            batch_layer = batch_norm_layer
            weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
        else: # weights and bias as-it-is
            weight_tensor = conv_layer.weight.data.clone()
            if conv_layer.bias is None:
                bias_tensor = False
            else:
                bias_tensor = conv_layer.bias.data.clone()

        # return tensor data before quantization
        if self.batch_fold and (batch_fold_type == 'folding_weights') and ( batch_norm_layer is not None):
            weight_tensor = weight_tensor.clamp_(w_min_ext, w_max_ext)

        if not get_quantized:
            return weight_tensor, bias_tensor
        
        # 2- quantization
        weight_quant_tensor, w_min_value, w_max_value = self._linear_symmetric_quant_args(weight_tensor, w_bits)

        range_w = w_max_value-w_min_value
        if range_w == 0:
            range_w = 1
        S_w = range_w  / ((2**w_bits)-1)

        if bias_tensor is not False:

            inp_layer = self.param_to_quantize[idx_layer]['input']
            if inp_layer == -1 :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_first_layer_stats()
            else :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])
            
            eps = S_a_i * S_w
            bias_quant_tensor = bias_tensor.div(eps).round().clamp(-2**(bias_bits-1),2**(bias_bits-1)-1).mul(eps)
        else:
            bias_quant_tensor = False


        # 3- batch norm defolding
        if self.batch_fold and (batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            weight_quant_tensor, bias_quant_tensor = self._batch_defold(weight_quant_tensor, bias_quant_tensor, batch_layer)

        return weight_quant_tensor, bias_quant_tensor

    def _quant_PerLayerAsymPACT(self, conv_layer, batch_norm_layer, batch_fold_type, w_bits, idx_layer, \
        w_min_ext, w_max_ext, training=False, get_quantized=True):

        # 1- folding of batch normalization layers before quantization if enabled and for validation
        if self.batch_fold  and ( batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            #folding batch_norm into convolution and quantize
            batch_layer = batch_norm_layer
            weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)
        else: # weights and bias as-it-is
            weight_tensor = conv_layer.weight.data.clone()
            if conv_layer.bias is None:
                bias_tensor = False
            else:
                bias_tensor = conv_layer.bias.data.clone()

        # return tensor data before quantization
        if self.batch_fold and (batch_fold_type == 'folding_weights') and ( batch_norm_layer is not None):
            weight_tensor = weight_tensor.clamp_(w_min_ext, w_max_ext)

        if not get_quantized:
            return weight_tensor, bias_tensor
        
        # 2- quantization
        weight_quant_tensor, w_min_value, w_max_value = self._linear_asymmetric_quant_args(weight_tensor, w_bits)

        range_w = w_max_value-w_min_value
        if range_w == 0:
            range_w = 1
        S_w = range_w  / ((2**w_bits)-1)

        if bias_tensor is not False:

            inp_layer = self.param_to_quantize[idx_layer]['input']
            if inp_layer == -1 :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_first_layer_stats()
            else :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])
            eps = S_a_i * S_w
            bias_quant_tensor = bias_tensor.div(eps).round().mul(eps)
        else:
            bias_quant_tensor = False


        # 3- batch norm defolding
        if self.batch_fold and (batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            weight_quant_tensor, bias_quant_tensor = self._batch_defold(weight_quant_tensor, bias_quant_tensor, batch_layer)

        return weight_quant_tensor, bias_quant_tensor


    def _quant_PerChannelsAsymMinMax(self, conv_layer, batch_norm_layer, batch_fold_type, w_bits, idx_layer, \
        w_min_ext, w_max_ext, training=False, get_quantized=True):

        # 0 -clamping value
        w_max,_ = conv_layer.weight.data.reshape(conv_layer.weight.data.size(0), -1).max(1)
        w_min,_ = conv_layer.weight.data.reshape(conv_layer.weight.data.size(0), -1).min(1)
        #w_max = w_max_ext
        #w_min = w_min_ext
        if len(conv_layer.weight.data.size()) == 4:
            w_min_value_mat = w_min.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(conv_layer.weight.data.size())
            w_max_value_mat = w_max.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(conv_layer.weight.data.size())
        elif len(conv_layer.weight.data.size()) == 2:
            w_min_value_mat = w_min.unsqueeze(1).expand(conv_layer.weight.data.size())
            w_max_value_mat = w_max.unsqueeze(1).expand(conv_layer.weight.data.size())                
        else:
            return 1

        if batch_fold_type != 'folding_weights' :
            conv_layer.weight.data = torch.max(conv_layer.weight.data, w_min_value_mat)
            conv_layer.weight.data = torch.min(conv_layer.weight.data, w_max_value_mat)

        # 1- folding of batch normalization layers before quantization if enabled and for validation
        if (self.batch_fold ) and ( batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            #folding batch_norm into convolution and quantize
            batch_layer = batch_norm_layer
            weight_tensor, bias_tensor = self._batch_fold(conv_layer, batch_layer)

            w_max,_ = weight_tensor.reshape(weight_tensor.size(0), -1).max(1)
            w_min,_ = weight_tensor.reshape(weight_tensor.size(0), -1).min(1)
            if len(weight_tensor.size()) == 4:
                w_min_value_mat = w_min.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(weight_tensor.size())
                w_max_value_mat = w_max.unsqueeze(1).unsqueeze(2).unsqueeze(3).expand(weight_tensor.size())
            elif len(weight_tensor.size()) == 2:
                w_min_value_mat = w_min.unsqueeze(1).expand(weight_tensor.size())
                w_max_value_mat = w_max.unsqueeze(1).expand(weight_tensor.size())                
            else:
                return 1
        else: # weights and bias as-it-is
            weight_tensor = conv_layer.weight.data.clone()
            if conv_layer.bias is None:
                bias_tensor = False
            else:
                bias_tensor = conv_layer.bias.data.clone()

        # return tensor data before quantization
        if self.batch_fold and (batch_fold_type == 'folding_weights') and  batch_norm_layer is not None :
            for v in range(weight_tensor.size(0) ):
                weight_tensor[v] = weight_tensor[v].clamp(w_min_ext[v].item(), w_max_ext[v].item() )

        if not get_quantized:
            return weight_tensor, bias_tensor, w_min_value_mat, w_max_value_mat
        
        # 2- quantization
        range_mat = w_max_value_mat - w_min_value_mat
        range_mat.masked_fill_(range_mat.eq(0), 1)
        n_steps = (2**w_bits)-1
        S_w = range_mat.div(n_steps)

        weight_quant_tensor = weight_tensor.div(S_w).round().clone()
        weight_quant_tensor = weight_quant_tensor.mul(S_w)
        

        if bias_tensor is not False:
            range_ext = w_max - w_min
            S_w =  range_ext.masked_fill_(range_ext.eq(0), 1) / n_steps

            inp_layer = self.param_to_quantize[idx_layer]['input']
            if inp_layer == -1 :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_first_layer_stats()    
            else :
                S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])

            eps = S_a_i * S_w
            bias_quant_tensor = bias_tensor.div(eps).round().mul(eps)
            if eps.abs().min() == 0:
                print('Eps = 0 in layer ', idx_layer, '| bias is ', bias_quant_tensor)
        else:
            bias_quant_tensor = False


        # 3- batch norm defolding
        if (self.batch_fold ) and (batch_norm_layer is not None) and (batch_fold_type == 'folding_weights'):
            weight_quant_tensor, bias_quant_tensor = self._batch_defold(weight_quant_tensor, bias_quant_tensor, batch_layer)

        return weight_quant_tensor, bias_quant_tensor, w_min_value_mat, w_max_value_mat



    def generate_deployment_model(self):
        '''
            Generate integer-only parameters from the fake-quantized model
        '''

        for i_l,layer in enumerate(self.param_to_quantize):
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']
            bias_bits = layer['bias_bits']

            if quant_type == 'PerLayerAsymMinMax':
                weight_tensor, bias_tensor = self._quant_PerLayerAsymMinMax( \
                        conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, training=False, get_quantized=False)

            elif quant_type == 'PerLayerAsymPACT':
                
                weight_org = conv_layer.weight.data.clone()

                layer['w_min_thr'].data.clamp_(max= 0)
                layer['w_max_thr'].data.clamp_(min= 0)
                w_min_value, w_max_value = layer['w_min_thr'].data.item() , layer['w_max_thr'].data.item()

                if layer['fold_type'] != 'folding_weights' or layer['batch_norm'] is None:
                    conv_layer.weight.data.clamp_(w_min_value, w_max_value)

                if layer['fold_type'] == 'ICN':
                    weight_tensor = conv_layer.weight.data.clone()
                    if conv_layer.bias is None:
                        bias_tensor = False
                    else:
                        bias_tensor = conv_layer.bias.data.clone()
                else:
                    weight_tensor, bias_tensor = self._quant_PerLayerAsymPACT( \
                            conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, w_min_value, w_max_value, training=False, get_quantized=False)

                conv_layer.weight.data.copy_(weight_org)

            elif quant_type == 'PerLayerSymPACT':
                
                weight_org = conv_layer.weight.data.clone()

                layer['w_max_thr'].data.clamp_(min= 0)
                w_max_value = layer['w_max_thr'].data.item()
                w_min_value = -w_max_value

                if layer['fold_type'] != 'folding_weights' or layer['batch_norm'] is None:
                    conv_layer.weight.data.clamp_(-w_max_value, w_max_value)

                if layer['fold_type'] == 'ICN':
                    weight_tensor = conv_layer.weight.data.clone()
                    if conv_layer.bias is None:
                        bias_tensor = False
                    else:
                        bias_tensor = conv_layer.bias.data.clone()
                else:
                    weight_tensor, bias_tensor = self._quant_PerLayerSymPACT( \
                            conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, bias_bits, i_l, -w_max_value, w_max_value, training=False, get_quantized=False)

                conv_layer.weight.data.copy_(weight_org)

            elif quant_type == 'PerChannelsAsymMinMax':
                weight_org = conv_layer.weight.data.clone()

                layer['w_min_thr'].data.clamp_(max= 0)
                layer['w_max_thr'].data.clamp_(min= 0)
                w_min_tensor, w_max_tensor = layer['w_min_thr'].data , layer['w_max_thr'].data

                weight_tensor, bias_tensor, w_min_mat, w_max_mat = self._quant_PerChannelsAsymMinMax( \
                            conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, w_min_tensor, w_max_tensor, training=False, get_quantized=False )

                conv_layer.weight.data.copy_(weight_org)

                
            else:
                print('Not recognized quantization scheme!')
                exit() 

            # compute S, Z parameters of input and outpt layers

        ###################################          PARAMS BITS         #############################################
            BIAS_BITS = 32  #INT32
            M0_BITS = 32
            N0_BITS = 8
            BIAS_CH_BITS = 16
            M0_BITS_LAST = 8
            BIAS_FIXED_BITS = 0


        ###################################          LEAF layers          #############################################
            if layer['leaf']:
                n_levels_w = (2**w_bits)-1

                if quant_type == 'PerChannelsAsymMinMax':    # per channel quantization

                    weight_quant_tensor = weight_tensor.clone()
                    if bias_tensor is not False:
                        bias_quant_tensor = bias_tensor
                    else:
                        bias_quant_tensor = False

                    inp_layer = layer['input']
                    S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])

                    for v in range(weight_tensor.size(0)) :
                        #w_min_value,w_max_value= layer['w_min_thr'].data[v], layer['w_max_thr'].data[v]
                        w_min_value,w_max_value= weight_tensor[v].min(), weight_tensor[v].max()
                        range_w = w_max_value-w_min_value
                        if range_w == 0:
                            range_w = 1
                        S_w = (range_w) / n_levels_w

                        weight_quant_tensor[v] = weight_tensor[v].div(S_w).round()

                        if bias_tensor is not False:
                            bias_quant_tensor[v] = bias_tensor[v].div( S_w * S_a_i ).mul(2**BIAS_FIXED_BITS).round().clamp(-2**(BIAS_BITS-1), 2**(BIAS_BITS-1)-1 ).div(2**BIAS_FIXED_BITS)

                    w_max_value , _ = weight_tensor.reshape(weight_tensor.size(0), -1).max(1)
                    w_min_value , _ = weight_tensor.reshape(weight_tensor.size(0), -1).min(1)

                    range_w = w_max_value-w_min_value
                    range_w.masked_fill_(range_w.eq(0), 1)
                    S_w = (range_w) / n_levels_w
                    M0, n_exp = self._get_m0_nexp_vect(S_w)
                    M0 = M0.mul(2**(M0_BITS_LAST-1)).round().clamp(-2**(M0_BITS_LAST-1),2**(M0_BITS_LAST-1)-1).div(2**(M0_BITS_LAST-1))   #assume Q.1.31
                    n_exp = n_exp.clamp(-2**(N0_BITS-1),2**(N0_BITS-1)-1  )
                    layer['quant_act'].M_ZERO = M0
                    layer['quant_act'].N_ZERO = 2**n_exp    

                    layer['quant_conv'].weight.data.copy_( weight_quant_tensor )
                    if bias_quant_tensor is not False:
                        layer['quant_conv'].bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
                    
        
                elif quant_type == 'PerLayerAsymPACT': 

                    w_min_value,w_max_value= weight_tensor.min(), weight_tensor.max()
                    #w_min_value, w_max_value=  layer['w_min_thr'].data.item() , layer['w_max_thr'].data.item()
                    range_w = w_max_value-w_min_value
                    if range_w == 0:
                        range_w = 1
                    S_w = range_w / n_levels_w

                    print('Layer: ',i_l,' | Sw: ', S_w )

                    weight_quant_tensor = weight_tensor.div(S_w).round()
                    layer['quant_conv'].weight.data.copy_( weight_quant_tensor )

                    inp_layer = layer['input']
                    S_a_i, Z_a_i, nl_a_i, a_min, a_max= get_activ_stats(self.param_to_quantize[inp_layer])
                    if bias_tensor is not False:
                        bias_tensor = bias_tensor.div(S_w*S_a_i)
                        bias_quant_tensor = bias_tensor.round().clamp(-2**(BIAS_BITS-1),2**(BIAS_BITS-1)-1)

                        layer['quant_conv'].bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
                        print('Layer: ',i_l,' | Quant Bias Tensor - Error: ', (bias_quant_tensor - bias_tensor).abs().sum() )


                    #folding scaling factors into thresholds
                    if layer['fold_type'] == 'folding_thresh':
           
                        if layer['batch_norm'] is not None:
                            layer['quant_act'].load_state_dict(layer['act'].state_dict())
                    
                    #folding scaling factors into weights
                    else:
                        pass

                elif quant_type == 'PerLayerSymPACT': 

                    w_max_value = weight_tensor.abs().max()
                    w_min_value = -w_max_value
                    range_w = w_max_value-w_min_value
                    if range_w == 0:
                        range_w = 1
                    S_w = range_w / n_levels_w

                    print('Layer: ',i_l,' | Sw: ', S_w )

                    weight_quant_tensor = weight_tensor.div(S_w).round().clamp(-2**(w_bits-1),2**(w_bits-1)-1)
                    layer['quant_conv'].weight.data.copy_( weight_quant_tensor )

                    inp_layer = layer['input']
                    S_a_i, Z_a_i, nl_a_i, a_min, a_max= get_activ_stats(self.param_to_quantize[inp_layer])
                    if bias_tensor is not False:
                        bias_tensor = bias_tensor.div(S_w*S_a_i)
                        bias_quant_tensor = bias_tensor.round().clamp(-2**(bias_bits-1),2**(bias_bits-1)-1)

                        layer['quant_conv'].bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
#                        print('Layer: ',i_l,' | Quant Bias Tensor - Error: ', (bias_quant_tensor - bias_tensor).abs().sum() )


                    #folding scaling factors into thresholds
                    if layer['fold_type'] == 'folding_thresh':
           
                        if layer['batch_norm'] is not None:
                            layer['quant_act'].load_state_dict(layer['act'].state_dict())
                    
                    #folding scaling factors into weights
                    else:
                        pass
        
        ###################################          OTHERS layers       #############################################
            elif type(layer['act']) in [ClippedLinearQuantization, LearnedClippedLinearQuantization] :

                # get S_a parameters
                inp_layer = layer['input']
                if inp_layer == -1 :
                    S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_first_layer_stats()  
                else:
                    S_a_i, Z_a_i, nl_a_i, a_min, a_max = get_activ_stats(self.param_to_quantize[inp_layer])

                S_a_o, Z_a_o, nl_a_o, a_min, a_max= get_activ_stats(layer)

                n_levels_w = (2**w_bits)-1

                if quant_type == 'PerChannelsAsymMinMax':    # per channel quantization
                       
                    if layer['fold_type'] == 'ICN':

                        batch_layer = layer['batch_norm']


                        gamma_over_sigma, mu_tensor, beta_tensor = self._get_BN_scale_factors(batch_layer)
                        if bias_tensor is not False:
                            bias_tensor = bias_tensor.add(-mu_tensor)
                            print('hereeee')
                        else:
                            bias_tensor = -mu_tensor.clone()
                        
                        #w_min_value,w_max_value= layer['w_min_thr'].data, layer['w_max_thr'].data
                        w_max_value , _ = weight_tensor.reshape(weight_tensor.size(0), -1).max(1)
                        w_min_value , _ = weight_tensor.reshape(weight_tensor.size(0), -1).min(1)
                        range_w = w_max_value-w_min_value
                        range_w.masked_fill_(range_w.eq(0), 1)
                        S_w = (range_w) / n_levels_w
                        bias_tensor = bias_tensor.add(beta_tensor/gamma_over_sigma).div(S_a_i*S_w).mul(2**BIAS_FIXED_BITS).round().clamp(-2**(BIAS_BITS-1),2**(BIAS_BITS-1)-1).div(2**BIAS_FIXED_BITS)  #assume INT8
                        layer['quant_conv'].bias = nn.Parameter(bias_tensor, requires_grad = False)


                        weight_quant_tensor = weight_tensor.clone()
                        for v in range(weight_tensor.size(0)) :
                            #w_min_value, w_max_value= layer['w_min_thr'].data[v], layer['w_max_thr'].data[v]
                            w_min_value,w_max_value= weight_tensor[v].min(), weight_tensor[v].max()
                            range_w = w_max_value-w_min_value
                            if range_w == 0:
                                range_w = 1
                                print('hhhhh')

                            S_w = range_w / n_levels_w
                            weight_quant_tensor[v] = weight_tensor[v].div(S_w).round()
                            gamma_over_sigma[v] = gamma_over_sigma[v].mul((S_a_i*S_w)/S_a_o)

                        #print('min:', weight_quant_tensor.min(),'max: ', weight_quant_tensor.max())
                        M0, n_exp = self._get_m0_nexp_vect(gamma_over_sigma)
                        M0 = M0.mul(2**(M0_BITS-1)).round().clamp(-2**(M0_BITS-1),2**(M0_BITS-1)-1).div(2**(M0_BITS-1))   #assume Q.1.31
                        n_exp = n_exp.clamp(-2**(N0_BITS-1),2**(N0_BITS-1)-1  )
                        layer['quant_act'].M_ZERO = M0
                        layer['quant_act'].N_ZERO = 2**n_exp                    
                        layer['quant_act'].clip_val = nl_a_o
                        layer['quant_conv'].weight.data.copy_( weight_quant_tensor )

                    else:
                        print('Not supported yet!!')
                        return 1

                else:   #### PerLayerAsymPACT ################
                    if quant_type == 'PerLayerAsymPACT':    # per channel quantization
                        w_min_value,w_max_value= weight_tensor.min(), weight_tensor.max()
                    else:
                        w_max_value = weight_tensor.abs().max()
                        w_min_value = -w_max_value

                    range_w = w_max_value-w_min_value
                    if range_w == 0:
                        range_w = 1
                    S_w = range_w / n_levels_w
                    print('Layer: ',i_l,' | Sw: ', S_w , 'S_a_i', S_a_i)
                    if quant_type == 'PerLayerAsymPACT':    # per channel quantization
                        weight_quant_tensor = weight_tensor.div(S_w).round()
                    else:
                        weight_quant_tensor = weight_tensor.div(S_w).round().clamp(-2**(w_bits-1),2**(w_bits-1)-1)
                    print('[WEIGHT] Min: ',weight_quant_tensor.min().item(),'| Max: ',weight_quant_tensor.max().item())
                    layer['quant_conv'].weight.data.copy_( weight_quant_tensor )


                    if layer['fold_type'] == 'folding_thresh':
                        layer['quant_conv'].bias = None

                        signs,thresholds = self.compute_thresholds(conv_layer,layer['batch_norm'], layer['act'].num_bits, a_min, a_max, S_a_i, S_w, i_l==0)
                        layer['quant_act'].thresholds = thresholds
                        layer['quant_act'].signs = signs

                    elif layer['fold_type'] == 'ICN':

                        batch_layer = layer['batch_norm']

                        gamma_over_sigma, mu_tensor, beta_tensor = self._get_BN_scale_factors(batch_layer)
                        if bias_tensor is not False:
                            bias_tensor = bias_tensor.add(-mu_tensor)
                            print('hereeee')
                        else:
                            bias_tensor = -mu_tensor


                        bias_tensor = bias_tensor.add(beta_tensor/gamma_over_sigma).div(S_a_i*S_w)  #assume INT8
                        print('Layer: ',i_l,' | Quant Bias Tensor - min: ', bias_tensor.min(),'- max: ', bias_tensor.max())
                        print('Layer: ',i_l,' | Gamma over sigma: ', gamma_over_sigma)

                        bias_quant_tensor = bias_tensor.round().clamp(-2**(BIAS_BITS-1),2**(BIAS_BITS-1)-1)
                        print('Layer: ',i_l,' | Quant Bias Tensor - Error: ', (bias_quant_tensor - bias_tensor).abs().sum() )

                        layer['quant_conv'].bias = nn.Parameter(bias_quant_tensor, requires_grad = False)

                        gamma_over_sigma = gamma_over_sigma.mul((S_a_i*S_w)/S_a_o)
                        M0, n_exp = self._get_m0_nexp_vect(gamma_over_sigma )
                        print('Layer: ',i_l,' | M0: ', M0.min(), M0.max(),'- n_exp: ', n_exp.min(), n_exp.max())

                        M0 = M0.mul(2**(M0_BITS-1)).round().clamp(-2**(M0_BITS-1),2**(M0_BITS-1)-1).div(2**(M0_BITS-1))   #assume Q.1.31
                        n_exp = n_exp.clamp(-2**(N0_BITS-1),2**(N0_BITS-1)-1  )
                        print('Layer: ',i_l,' | Error: ', ((M0*2**n_exp)-gamma_over_sigma).abs().sum()  )


                        layer['quant_act'].M_ZERO = M0
                        layer['quant_act'].N_ZERO = 2**n_exp                    
                        layer['quant_act'].clip_val = nl_a_o

                    else:
                        if quant_type == 'PerLayerSymPACT':    # per channel quantization
                            BIAS_BITS = bias_bits

                        ######### Folding weights #######################
                        if bias_tensor is not False:
                            bias_tensor = bias_tensor.div(S_w*S_a_i)
                            bias_quant_tensor = bias_tensor.round().clamp(-2**(BIAS_BITS-1),2**(BIAS_BITS-1)-1)
                            layer['quant_conv'].bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
#                            print('Layer: ',i_l,' | Quant Bias Tensor - Error: ', (bias_quant_tensor - bias_tensor).abs().sum() )

                        else:
                            layer['quant_conv'].bias = None

                        Z_w = -weight_quant_tensor.min().item()
                        
                        M0, n_exp = self._get_m0_nexp((S_a_i*S_w)/S_a_o )
                        print('Layer PerLayerSymPACT: ',i_l,' | M0: ', M0,'- n_exp: ', n_exp)
                        M0 = np.clip(np.round(M0.item()*2**(M0_BITS-1)),-2**(M0_BITS-1),2**(M0_BITS-1)-1) / 2**(M0_BITS-1)
                        n_exp = np.clip(n_exp,-2**(N0_BITS-1),2**(N0_BITS-1)-1  )
 #                       print('Layer PerLayerAsymPACT: ',i_l,' | Error M: ', ((M0.tolist()*2**n_exp.tolist())-((S_a_i*S_w)/S_a_o) )  )

                        layer['quant_act'].M_ZERO = M0.tolist()
                        layer['quant_act'].N_ZERO = n_exp.tolist()
                        layer['quant_act'].clip_val = nl_a_o



            else: # relu for instance
                print('Herrrrrreeeeeeee')

                layer['quant_conv'].weight.data.copy_( weight_quant_tensor )
                layer['quant_conv'].bias = nn.Parameter(bias_tensor, requires_grad = False)      


    def store_and_quantize(self, training=False):
    # store actual weights and replace with quantized versione before the forward pass

        for i_l,layer in enumerate(self.param_to_quantize):
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']

            
            if quant_type == 'PerLayerAsymMinMax':

                # 1. store data floating value
                layer['weight'] = conv_layer.weight.data.clone()
                if conv_layer.bias is None:
                    layer['bias'] = False
                else:
                    layer['bias'] = conv_layer.bias.data.clone()
                bias_bits = layer['bias_bits']

                #2. quantization
                weight_quant_tensor, bias_quant_tensor = self._quant_PerLayerAsymMinMax( \
                    conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, training=training )

                # 3. push quantized values into fake-quantized network
                conv_layer.weight.data.copy_( weight_quant_tensor )
                if bias_quant_tensor is not False:
                    if conv_layer.bias is not None:
                        conv_layer.bias.data.copy_( bias_quant_tensor )
                    else:
                        conv_layer.bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
                elif conv_layer.bias is not None:
                    print('Error! something wrong with bias')
                    return(-1)

            elif quant_type in ['PerLayerSymPACT','PerLayerAsymPACT','PerChannelsAsymMinMax']:

                # 1. store data floating value
                layer['weight'] = conv_layer.weight.data.clone()
                if conv_layer.bias is None:
                    layer['bias'] = False
                else:
                    layer['bias'] = conv_layer.bias.data.clone()
                bias_bits = layer['bias_bits']

                # 2. quantization 
                if quant_type == 'PerLayerAsymPACT':

                    layer['w_min_thr'].data.clamp_(max= 0)
                    layer['w_max_thr'].data.clamp_(min= 0)
                    w_min_value, w_max_value = layer['w_min_thr'].data.item() , layer['w_max_thr'].data.item()

                    if layer['fold_type'] != 'folding_weights' or layer['batch_norm'] is None:
                        conv_layer.weight.data.clamp_(w_min_value, w_max_value)

                    weight_quant_tensor, bias_quant_tensor = self._quant_PerLayerAsymPACT( \
                        conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, w_min_value, w_max_value, training=training )

                if quant_type == 'PerLayerSymPACT':

                    layer['w_max_thr'].data.clamp_(min= 0)
                    w_max_value = layer['w_max_thr'].data.item()

                    if layer['fold_type'] != 'folding_weights' or layer['batch_norm'] is None:
                        conv_layer.weight.data.clamp_(-w_max_value, w_max_value)

                    weight_quant_tensor, bias_quant_tensor = self._quant_PerLayerSymPACT( \
                        conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, bias_bits,i_l, -w_max_value, w_max_value, training=training )
                           
                elif quant_type == 'PerChannelsAsymMinMax':

                    layer['w_min_thr'].data.clamp_(max= 0)
                    layer['w_max_thr'].data.clamp_(min= 0)
                    w_min_tensor, w_max_tensor = layer['w_min_thr'].data , layer['w_max_thr'].data
                            
                    weight_quant_tensor, bias_quant_tensor, layer['w_min_mat'], layer['w_max_mat'] = self._quant_PerChannelsAsymMinMax( \
                        conv_layer, layer['batch_norm'], layer['fold_type'], w_bits, i_l, w_min_tensor, w_max_tensor, training=training )

                # 3. push quantized values into fake-quantized network
                conv_layer.weight.data.copy_( weight_quant_tensor )

                if bias_quant_tensor is not False:
                    if conv_layer.bias is not None:
                        conv_layer.bias.data.copy_( bias_quant_tensor )
                    else:
                        conv_layer.bias = nn.Parameter(bias_quant_tensor, requires_grad = False)
                elif conv_layer.bias is not None:
                    print('Error something wrong with bias')
                    return(-1)

            else:
                print('Not recognized quantization scheme!')
                exit() 
    

    def restore_real_value(self):
    # restore real value into the fake quantized model before the weight update

        for layer in self.param_to_quantize:
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']


            if quant_type in ['PerLayerSymPACT','PerLayerAsymMinMax', 'PerLayerAsymPACT', 'PerChannelsAsymMinMax']:

                #if layer['fold_type'] == 'folding_weights':
                # restore data 
                conv_layer.weight.data.copy_(layer['weight'])
                if layer['w_clip'] is not None:
                    [ w_min, w_max ]= layer['w_clip']
                    conv_layer.weight.data = conv_layer.weight.data.clamp(w_min, w_max)

                if layer['bias'] is not False:
                    conv_layer.bias.data.copy_(layer['bias'])
                else:
                    conv_layer.bias = None


            elif quant_type == None:
                return None

    def backprop_quant_gradients(self):
    # apply backward pass from quantized weights to real value weights    


        for i_l, layer in enumerate(self.param_to_quantize):
            quant_type = layer['quant_type']
            w_bits = layer['w_bits']
            conv_layer = layer['conv']

            if quant_type == 'PerLayerAsymMinMax':
                self.__ste_correction(conv_layer, clip=False)

            elif quant_type == 'PerLayerAsymPACT':
                w_min_param, w_max_param = layer['w_min_thr'],layer['w_max_thr']
                w_min_value, w_max_value = w_min_param.data.item(), w_max_param.data.item()

                grad_a_max = conv_layer.weight.grad.data.clone()
                grad_a_min = conv_layer.weight.grad.data.clone()

                weight_tensor = conv_layer.weight.data
                dLdw = conv_layer.weight.grad.data
                dLdw.masked_fill_(weight_tensor.le(w_min_value), 0)
                dLdw.masked_fill_(weight_tensor.ge(w_max_value), 0)

                grad_a_max.masked_fill_(layer['weight'].lt(w_max_value), 0)
                grad_a_max = grad_a_max.sum().expand_as(w_max_param.data)
                w_max_param.grad.data = grad_a_max

                grad_a_min.masked_fill_(layer['weight'].gt(w_min_value), 0)
                grad_a_min = grad_a_min.sum().expand_as(w_min_param.data)
                w_min_param.grad.data = grad_a_min


            elif quant_type == 'PerLayerSymPACT':
                w_max_param = layer['w_max_thr']
                w_max_value = w_max_param.data.item()

                grad_a_max = conv_layer.weight.grad.data.clone()

                weight_tensor = conv_layer.weight.data
                dLdw = conv_layer.weight.grad.data
                dLdw.masked_fill_(weight_tensor.abs().ge(w_max_value), 0)

                grad_a_max.masked_fill_(layer['weight'].abs().lt(w_max_value), 0)
                grad_a_max = grad_a_max.sum().expand_as(w_max_param.data)
                w_max_param.grad.data = grad_a_max

            
            else:
            	return None


    def __ste_correction(self, conv_layer, clip=True):
        # gradient correction according to straight trough estimator ( STE)      
        #update convolution weights
        weight = conv_layer.weight.data
        m = conv_layer.weight.grad.data
        if clip:
            m.masked_fill_(weight.lt(-1.0), 0)
            m.masked_fill_(weight.gt(1.0), 0)


