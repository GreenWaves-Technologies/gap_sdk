# Copyright 2019 GreenWaves Technologies, SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#include paths
import argparse
import os, imp, sys
import time
import logging
import torch
import torch.nn as nn
import torch.nn.parallel
import torch.backends.cudnn as cudnn
import torch.optim
import torch.utils.data
from datetime import datetime
from ast import literal_eval
from torchvision.utils import save_image
from torch.autograd import Variable
import copy, math
import numpy as np
import struct
import cv2 # for image loading

# local includes
sys.path.append('training/')
from data import get_dataset, get_num_classes
from preprocess import get_transform
from utils import *
import models
import quantization


parser = argparse.ArgumentParser(description='GAP8 Parser')

parser.add_argument('--ckp_file', metavar='CHECKPOINT_FILE', default='',
                    help='Pythorch checkpoint file of the fake-quantized network')
parser.add_argument('-e', '--evaluate', action='store_true',
                    help='run integer-only model on validation set')
parser.add_argument('--img_file', default='',
                    help='Input image file')
parser.add_argument('--bin_path', default='',
                    help='Folder for file generation')
parser.add_argument('--dump', action='store_true',
                    help='Activate dumping process of binary file')
parser.add_argument('--network_name', default='MobileNetCNN',
                    help='Name of the dumped network')

model_names = sorted(name for name in models.__dict__
                     if name.islower() and not name.startswith("__")
                     and callable(models.__dict__[name]))


# bit parameter for model dumping
MULTBIAS_BITS = 8
BIAS_BITS = 16


def main():
	# parsing the input
	args = parser.parse_args()

	# read checkpoint file
	checkpoint_file = args.ckp_file
	if checkpoint_file == '':
		print('No checkpoint file provided!')
		exit(1)

	if os.path.isfile(checkpoint_file):
		checkpoint_loaded = torch.load(checkpoint_file)
		print('Model pretrained at ', checkpoint_file)
	else:
		print('Model pretrainined not Loaded')
		exit(1)

	# Load configuration from checkpoint
	model_name = checkpoint_loaded['model']
	model_config = checkpoint_loaded['config']
	activ_bits = model_config['activ_bits']
	activ_type = model_config['activ_type']
	dataset = model_config['dataset']
	input_dim = model_config['input_dim']
	input_size = input_dim
	num_classes = model_config['num_classes']
	type_quant = model_config['type_quant']
	weight_bits = model_config['weight_bits']
	width_mult = model_config['width_mult']
	additional_config = ''
	quant_add_config = checkpoint_loaded['add_config']
	fold_type = checkpoint_loaded['fold_type']

	# Load quantizer & model state
	quantizer_load = checkpoint_loaded['quantizer']
	model_state = checkpoint_loaded['state_dict']

	# Create model with same characteristics
	model = models.__dict__[model_name]
	nClasses = get_num_classes(dataset)
	model_config = {'input_size': input_size, 'dataset': dataset, 'num_classes': nClasses, \
	                'type_quant': type_quant, 'weight_bits': weight_bits, 'activ_bits': activ_bits,\
	                'activ_type': activ_type, 'width_mult': width_mult, 'input_dim': input_size }
	if additional_config is not '':
	    model_config = dict(model_config, **literal_eval(additional_config))	
	model = models.__dict__[model_name]
	model = model(**model_config, pretrained=False)
	if model is None :
	    print('ERROR: model is none')
	    exit(1)


	# wrap the model with quantop operator
	dummy_input = torch.Tensor(1,3,int(input_dim),int(input_dim))
	quantizer = quantization.QuantOp(model, quant_type = type_quant, weight_bits = weight_bits,bias_bits =32, \
                                 batch_fold_delay=0,batch_fold_type=fold_type, act_bits=activ_bits, \
                                 add_config=quant_add_config, dummy_input = dummy_input  )	

	# load features from quantizer_load into quantizer and model state
	for i,item in enumerate(quantizer_load.param_to_quantize):
	    item2 = quantizer.param_to_quantize[i]
	    item2['w_max_thr'] = item['w_max_thr']
	    item2['bias_bits'] = item['bias_bits']
	    if 'w_min_thr' in item2.keys():
	        item2['w_min_thr'] = item['w_min_thr']
	    if item2['conv'] is not None:
	        item2['conv'].load_state_dict(item['conv'].state_dict())
	    if item2['batch_norm'] is not None:
	        item2['batch_norm'].load_state_dict(item['batch_norm'].state_dict())    
	    if item2['act'] is not None:
	        item2['act'].load_state_dict(item['act'].state_dict()) 


	# enable folding of batch norm in to convolutional layers
	quantizer.batch_fold = True

	# generate model (with cuda)
	quantizer.generate_deployment_model()
	quantizer.deployment_model.cuda()
	quantizer.deployment_model.eval()

	# adjust bias and scaling factor
	for i, item in enumerate(quantizer.param_to_quantize):
		# clamping bias of last layer
		if type(item['quant_conv']) is nn.Linear:
			bias_bits = item['bias_bits']
			if item['quant_conv'].bias is not None:
				item['quant_conv'].bias.data.clamp_(-2**(bias_bits-1),2**(bias_bits-1) -1 )

		# clamping bias of other layers
		else:
			#mult bias rounding 
			if item['quant_act'] is not None:
				M0 = item['quant_act'].M_ZERO
				N0 = item['quant_act'].N_ZERO
				M0_new = np.floor(M0* 2**(MULTBIAS_BITS-1)) / 2**(MULTBIAS_BITS-1)
				item['quant_act'].M_ZERO = M0_new
				item['quant_act'].N_ZERO = N0

			#bias clamping and extraction of normbias
			if item['quant_conv'].bias is not None:
				bias_bits = item['bias_bits']
				bias = item['quant_conv'].bias.data
				bias = bias.clamp_(-2**(bias_bits-1),2**(bias_bits-1) -1 )
				N0_bias = - max( bias.abs().max().log2().ceil().item()-7,0)
				item['quant_conv'].bias.data = bias.mul(2**N0_bias).round().clamp(-2**7,(2**7) -1 ).div(2**N0_bias)
				item['quant_conv'].bias.N0_bias = N0_bias

        
	# run validation
	if args.evaluate:
		import torchvision.transforms as transforms

		normalize = transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])

		input_eval_transform = transforms.Compose([
		        transforms.Scale(int(input_size) ),
		        transforms.ToTensor(),
		        normalize
		    ])
		input_transform = getattr(model, 'input_transform', input_eval_transform)

		val_data = get_dataset(dataset, 'val', input_transform['eval'])
		val_loader = torch.utils.data.DataLoader(
		    val_data,
		    batch_size=32, shuffle=False,
		    num_workers=8, pin_memory=True)

		def forward(data_loader, model,  epoch=0, training=False, quantizer=None):
		    batch_time = AverageMeter()
		    data_time = AverageMeter()
		    losses = AverageMeter()
		    top1 = AverageMeter()
		    top5 = AverageMeter()

		    end = time.time()
		    model.eval()
		    
		    max_i = 1
		    min_i = -1
		    bit_i = 8
		    n_steps = (2**bit_i)-1
		    eps = (max_i-min_i) / n_steps
		    
		    for i, (inputs, target) in enumerate(data_loader):
		        # measure data loading time
		        data_time.update(time.time() - end)
		        target = target.cuda(async=True)
		        input_var = Variable(inputs.cuda(), volatile=not training)
		        target_var = Variable(target)
		        input_var = input_var.clamp(min_i,max_i).div(eps).round()
		        if quantizer is not None:
		            input_var = input_var.mul(eps)
		            quantizer.store_and_quantize(training=False )

		        # compute output
		        output = model(input_var)
		        if type(output) is list:
		            output = output[0]
		        values, indices = output.max(1)
		        # measure accuracy and record loss
		        prec1, prec5 = accuracy(output.data, target, topk=(1, 5))
		        top1.update(prec1.item(), inputs.size(0))
		        top5.update(prec5.item(), inputs.size(0))

		        # measure elapsed time
		        batch_time.update(time.time() - end)
		        end = time.time()

		        if quantizer is not None:
		            quantizer.restore_real_value()

		        if i % 100 == 0:
		            print('{phase} - Epoch: [{0}][{1}/{2}]\t'
		                         'Time {batch_time.val:.3f} ({batch_time.avg:.3f})\t'
		                         'Data {data_time.val:.3f} ({data_time.avg:.3f})\t'
		                         'Prec@1 {top1.val:.3f} ({top1.avg:.3f})\t'
		                         'Prec@5 {top5.val:.3f} ({top5.avg:.3f})'.format(
		                             epoch, i, len(data_loader),
		                             phase='TRAINING' if training else 'EVALUATING',
		                             batch_time=batch_time,
		                             data_time=data_time, top1=top1, top5=top5))

		    print('Top1: ', top1.avg)
		    return top1.avg, top5.avg

		forward(val_loader, quantizer.deployment_model.cuda() )


	# dumping of the input image
	input_dim = int(input_dim)
	if args.img_file == '':
		print('No input image provide. Going to generate a rondom matrix!')
		x = torch.Tensor(1,3,int(input_dim),int(input_dim)).cuda().random_(-4,4)

	else:
		image = cv2.imread(args.img_file)
		scale_dim = 256
		off = int((scale_dim - input_dim)/2)
		image = cv2.resize(image,(scale_dim,scale_dim))
		image = image[off:input_dim+off,off:off+input_dim,:]
		x = torch.Tensor(image).permute(2,0,1).unsqueeze(0).cuda().add(-128)

	y=quantizer.deployment_model(x)
	print('This is the inference result: ', y.argmax().item() )

	#file generation
	if args.bin_path == '':
		root_folder = './'
	else:
		root_folder = args.bin_path
	folder_bindump = root_folder+'binFiles/'

	if args.dump:
		os.makedirs(root_folder, exist_ok=True)
		os.makedirs(folder_bindump, exist_ok=True)

	def get_4D_act_tensor_to_list_HWC(x):
	    tensor_list = []
	    n_b, in_ch, in_dim_w, in_dim_h = x.size()
	    for c in range(in_ch):
	        for i in range(in_dim_w):
	            for j in range(in_dim_h):
	                tensor_list.append(int(x[0][c][i][j].item()))
	    return tensor_list

	def get_4D_wgt_tensor_to_list_HWC(w):
	    tensor_list = []
	    out_ch, in_ch, ker_w, ker_h = w.size()
	    for co in range(out_ch):
	        for ci in range(in_ch):
	            for i in range(ker_w):
	                for j in range(ker_h):
	                    tensor_list.append(int(w[co][ci][i][j].item()))
	    return tensor_list

	def get_2D_wgt_tensor_to_list_HWC(w):
	    tensor_list = []
	    out_ch, in_ch = w.size()
	    for co in range(out_ch):
	        for ci in range(in_ch):
	            tensor_list.append(int(w[co][ci].item()))
	    return tensor_list

	def get_1D_bias_tensor_to_list_HWC(b):
	    tensor_list = []    
	    out_ch , = b.size()
	    for co in range(out_ch):
	        tensor_list.append(int(b[co].item()))
	    return tensor_list   	

	def dump_int16_tensor_to_file(file_name, data):    
	    newFile = open(file_name, "wb")
	    for item in data:
	        newFile.write(struct.pack('h', item)) # 1byte
	    newFile.close()


	def dump_int8_tensor_to_file(file_name, data):    
	    newFile = open(file_name, "wb")
	    for item in data:
	        newFile.write(struct.pack('b', item)) # 1byte
	    newFile.close()

	def print_size(model, input, output): 
	    global si, so
	    si = input[0].size()
	    so = output[0].size()

	# graph buffers
	Layers = []
	WeightEdges = []
	ActivEdges = []
	GraphNodes = []

	# dump input data
	input_tensor = "In" # this must match the input tensor of the network inference function
	if args.dump:
		data = get_4D_act_tensor_to_list_HWC(x)
		dump_int8_tensor_to_file(folder_bindump+'L0_INPUT.bin',data)
	txt = 'TCArgInfo("signed char *__restrict", "{}", ARG_SCOPE_ARG, ARG_DIR_IN, 1,AT_MEM_L3_HRAM, 0)'.format(input_tensor)
	WeightEdges.append(txt) 

	# dump the network layer-by-layer
	i_l = 0
	for i,item in enumerate(quantizer.param_to_quantize):

	    #convolution parameters
	    conv = item['quant_conv']
	    hook = conv.register_forward_hook(print_size)
	    quantizer.deployment_model(x)
	    hook.remove() 
	    input_size = si

	    if item['quant_act'] is not None:
	        M0 = int(item['quant_act'].M_ZERO * 2**(MULTBIAS_BITS-1))
	        N0 = MULTBIAS_BITS - 1 - item['quant_act'].N_ZERO
	    
	    if type(conv) in [ models.linear_quantized_modules.Conv2d_SAME, nn.Conv2d]:
	        out_ch = conv.out_channels
	        in_ch = conv.in_channels
	        if in_ch == conv.groups:
	            is_dw = True
	        else:
	            is_dw = False
	        ker_size = conv.kernel_size
	        ker_stride = conv.stride
	        ker_dilation = conv.dilation
	        if is_dw:
	            num_params = out_ch*ker_size[0]*ker_size[0]
	        else:
	            num_params = out_ch*in_ch*ker_size[0]*ker_size[0]
	        ker_padding = conv.padding
	        
	        #weight parameters
	        file_txt = 'L{}_weight_L3.bin'.format(i_l)
	        if args.dump:
	        	data = get_4D_wgt_tensor_to_list_HWC(conv.weight.data)
	        	dump_int8_tensor_to_file(folder_bindump+file_txt,data)
	        txt = 'TCArgInfo ("signed char *__restrict", "FL{}",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("{}", 1, 1, 1, 0))'.format( \
	            i_l,folder_bindump+file_txt)
	        WeightEdges.append(txt)
	        
	        #bias parameters
	        N0_bias =  int(-conv.bias.N0_bias)
	        bias_int = conv.bias.div(2**N0_bias).round().clamp(-2**7,(2**7) -1 )
	        file_txt = 'L{}_bias_L3.bin'.format(i_l)
	        if args.dump:
	        	data = get_1D_bias_tensor_to_list_HWC(bias_int)
	        	dump_int8_tensor_to_file(folder_bindump+file_txt,data)
	        txt = 'TCArgInfo ( "signed char * __restrict__", "BL{}",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "{}", 1, 1, 1, 0))'\
	            .format(i_l,folder_bindump+file_txt)
	        WeightEdges.append(txt)          

	        # scaling factor parameters
	        file_txt = 'L{}_M0_L3.bin'.format(i_l)
	        if args.dump:
	        	data = [M0 for x in range(out_ch)]
	        	dump_int8_tensor_to_file(folder_bindump+file_txt,data)
	        txt = 'TCArgInfo ("signed char *__restrict", "ML{}",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("{}", 1, 1, 1, 0))'.format( \
	            i_l,folder_bindump+file_txt)
	        WeightEdges.append(txt)    

	        # add node to the graph   
	        NormMul = 7	# fixed
	        Norm = N0 - NormMul
	        NormBias = N0_bias
	        #if ker_size[0]==1 and ker_size[1]==1:          
	        #	
	        #else:                  
	        #	NormBias = 2*Norm - N0_bias

	        # convolution layer
	        txt = 'CNN_ConvolutionMulBiasPoolReLU("Layer{}", &CtrlH, 1,1,1,1,1,{},{},{},{},{},1,1,1,0,1,{},{},{},{},{},{},{},{},{},{},{}, 1, KOP_NONE, 3,3, 1,1, 2,2, 1, KOP_RELU)'\
	            .format(i_l,0,0,-NormBias,N0,0,\
	            		in_ch,out_ch,input_size[2],input_size[3],'KOP_CONV_DWDP' if is_dw else 'KOP_CONV_DP', \
	                   ker_size[0], ker_size[1], ker_dilation[0], ker_dilation[1], ker_stride[0], ker_stride[1] )        
	        Layers.append(txt)
	        
	        # temporary tensors                              
	        output_tensor = "OutL{}".format(i)
	        txt = 'TCArgInfo ("signed char *__restrict", "{}", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0)'\
	            .format(output_tensor)
	        ActivEdges.append(txt)


	        txt = 'AddNode("Layer{}",Bindings(5,GNodeArg(GNA_IN, "{}", 0),GNodeArg(GNA_IN, "FL{}", 0),GNodeArg(GNA_IN, "BL{}", 0),GNodeArg(GNA_IN, "ML{}", 0),GNodeArg(GNA_OUT, "{}", 0) ))'\
	            .format(i_l,input_tensor,i_l,i_l,i_l,output_tensor)
	        GraphNodes.append(txt)
	        
	        # 
	        input_tensor = output_tensor
	        i_l +=1
	    

	    elif type(conv) in [ nn.Linear]:
	        
	        out_features = conv.out_features
	        in_features = conv.in_features
	        
	        file_txt = 'L{}_weight_L3.bin'.format(i_l)
	        data = get_2D_wgt_tensor_to_list_HWC(conv.weight.data)
	        if args.dump:
	            dump_int8_tensor_to_file(folder_bindump+file_txt,data)    
	        txt = 'TCArgInfo ("signed char *__restrict", "FL{}",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo("{}", 1, 1, 1, 0))'.format( \
	            i_l,folder_bindump+file_txt)
	        WeightEdges.append(txt)
	        
	        data = get_1D_bias_tensor_to_list_HWC(conv.bias.data)
	        if args.dump:
	            dump_int16_tensor_to_file(folder_bindump+file_txt,data)
	        txt = 'TCArgInfo ( "short int * __restrict__", "BL{}",  ARG_SCOPE_GLOBAL, ARG_DIR_CONSTIN, 0,  AT_MEM_UNDEF, ConstInfo(  "{}", 1, 1, 1, 0))'\
	            .format(i_l,folder_bindump+file_txt)
	        WeightEdges.append(txt)    
	        
	        txt = 'CNN_LinearReLU("Layer{}",&CtrlH, 1,1,2,2,0,0,0,0,1,1,1,1,{},{},KOP_LINEAR, KOP_NONE)'\
	            .format(i_l,in_features,out_features )        
	        Layers.append(txt)

	        output_tensor = "Out"
	        txt = 'TCArgInfoA("short int *__restrict", "{}",     ARG_SCOPE_ARG,   ARG_DIR_OUT,    AT_MEM_L2,  AT_MEM_L2, 0)'\
	            .format(output_tensor)
	        WeightEdges.append(txt)

	        txt = 'AddNode("Layer{}",Bindings(4,GNodeArg(GNA_IN, "{}", 0),GNodeArg(GNA_IN, "FL{}", 0),GNodeArg(GNA_IN, "BL{}", 0),GNodeArg(GNA_OUT, "{}", 0)))'\
	            .format(i_l,input_tensor,i_l,i_l,output_tensor)
	        GraphNodes.append(txt)
	        
	        input_tensor = output_tensor
	        i_l += 1


	    # pooling layer append to the last convolution layer
	    if item['pool'] is not None:
	        pool = item['pool'][0]
	        if type(pool) is nn.AvgPool2d:
	            txt = 'CNN_PoolReLU("Layer{}",&CtrlH, 1,1,0,0, 1,1, {},{}, {},{},KOP_AVGPOOL,{},{}, 1,1,{},{}, 1, KOP_NONE)'\
	                .format(i_l,input_size[1],input_size[1],input_size[2],input_size[3],pool.kernel_size, pool.kernel_size,\
	                       pool.stride, pool.stride)        
	            Layers.append(txt)  

	            output_tensor = "OutL{}".format(i_l)
	            txt = 'TCArgInfo ("signed char *__restrict", "{}", ARG_SCOPE_LOCAL,  ARG_DIR_INOUT, 0, AT_MEM_UNDEF, 0)'\
	                .format(output_tensor)
	            ActivEdges.append(txt)

	            txt = 'AddNode("Layer{}",Bindings(2,GNodeArg(GNA_IN, "{}", 0),GNodeArg(GNA_OUT, "{}", 0)))'\
	                .format(i_l,input_tensor,output_tensor)
	            GraphNodes.append(txt)
	        
	            input_tensor = output_tensor
	            i_l += 1



	# print Graph Model
	f_txt = ''
	f_txt += 'void {}()'.format(args.network_name)+'{\n'
	f_txt += '\tCNN_GenControl_T CtrlH;\n'
	f_txt += '\tCNN_InitGenCtrl(&CtrlH);\n'
	f_txt += '\tCNN_SetGenCtrl(&CtrlH, "EnableIm2Col", AT_OPT_ON);\n'
	f_txt += '\tCNN_SetGenCtrl(&CtrlH, "PADTYPE", PAD_BALANCED_RIGHT);\n\n'
	for item in Layers:
	    f_txt += '\t'+item+';\n'

	f_txt += '\n\tCreateGraph("{}",\n'.format(args.network_name)
	f_txt += '\t\tCArgs({},\n'.format(len(WeightEdges))
	for i,item in enumerate(WeightEdges):
	    f_txt += '\t\t\t'+item
	    f_txt += '\n' if i==len(WeightEdges)-1 else ',\n'

	f_txt += '\t\t),\n\t\tCArgs({},\n'.format(len(ActivEdges)) 
	for i, item in enumerate(ActivEdges):
	    f_txt += '\t\t\t'+item
	    f_txt += '\n' if i==len(ActivEdges)-1 else ',\n'
	f_txt += '\t\t)\n\t);\n'

	for i, item in enumerate(GraphNodes):
	    f_txt += '\t'+item+';\n'

	f_txt += '\tCloseGraph();\n}' 

	#if args.dump:
	file_name = args.network_name + '.c'
	newFile = open(file_name, "w")
	newFile.write(f_txt)
	newFile.close()




if __name__ == '__main__':
    main()