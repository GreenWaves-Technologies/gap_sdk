#
# quant_auto.py
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

import sys
sys.path.append('../')
from models.linear_quantized_modules import Conv2d_SAME


def memory_driven_quant(model, x, ONLY_READ_MEM = 512*1024, 	READWRITE_MEM = 64*1024, cfg_quant='MixPL' ):

	####################################################################################
	###################### compute activation footprint ################################
	####################################################################################
	def compute_activation_footprint(param_list):
		i_params_mem = []
		i_bits_mem = []
		o_params_mem = []
		o_bits_mem = []

		for i,item in enumerate(param_list):
			o_size = activation_vector_o[i]
			i_size = activation_vector_i[i]
			o_params = 1
			for v in range(len(o_size)):
				o_params *= o_size[v]
			i_params = 1
			for v in range(len(i_size)):
				i_params *= i_size[v] 
			
			i_bits = param_list[i]['act_i_bits']
			o_bits = param_list[i]['act_o_bits']
				
			i_params_mem.append(i_params)
			i_bits_mem.append(i_bits)
			o_params_mem.append(o_params)
			o_bits_mem.append(o_bits)
			
			#print('Input = ', i_params,'(bits = ', i_bits, ') | Output Params = ', o_params,'(bits = ', o_bits,')')            

		return i_params_mem, i_bits_mem, o_params_mem, o_bits_mem

	def cut_activation_footprint(param_list, act_mem, MIN_ACT_BITS=2 ):
		
		i_params_mem, i_bits_mem, o_params_mem, o_bits_mem = compute_activation_footprint(param_list)
		LAST = len(o_bits_mem)-1

		errQuant = False

		completeQuant = False
		
		#for i in range(len(i_params_mem)):
		#start from layer 0 in forward direction
		i = 0 
		forward = True
		okQuantNet = False

		print('Init: ', i_bits_mem, o_bits_mem)

		while completeQuant is False:

			okQuant = True	# flag is set to False if one of the layer quantization results uncomplete during forward or backward

			print('*********** Layer ',i, 'Start ************** | completeQuant is ', completeQuant,'| okQuantNet is ',okQuantNet)
			tot_layer_mem = ((i_params_mem[i] * i_bits_mem[i]) + (o_params_mem[i] * o_bits_mem[i]) )/8 # to bytes
			n_iter = 1

			while tot_layer_mem > act_mem and okQuant == True:
				
				print('Iteration ',n_iter, 'with bits: ',i_bits_mem[i],o_bits_mem[i],'with forward as ', forward)
				
				i_mem = i_params_mem[i] * i_bits_mem[i]
				o_mem = o_params_mem[i] * o_bits_mem[i]

				if i == 0: # first layer

					if forward == True:
						if o_bits_mem[i] > MIN_ACT_BITS:										#forward
							param_list[i]['act_o_bits'] = int(o_bits_mem[i] / 2)
							param_list[i+1]['act_i_bits'] = param_list[i]['act_o_bits']
						else:																	#error
							errQuant = True
							print('No way to find a solution because of the first layer!')
							break
					else:

						if o_bits_mem[i] > MIN_ACT_BITS:										#forward
							okQuant = False

						else:																	#error
							errQuant = True
							print('No way to find a solution because of the first layer!')
							break


				elif i == LAST: # last layer

					if forward == True:

						if i_bits_mem[i] > MIN_ACT_BITS:										#backward
							okQuant = False

						else:																	#error
							errQuant = True
							print('No way to find a solution because of the last layer!')
							break        
					else:

						if i_bits_mem[i] > MIN_ACT_BITS:										#backward
							param_list[i-1]['act_o_bits'] = int(i_bits_mem[i] / 2)
							param_list[i]['act_i_bits'] = param_list[i-1]['act_o_bits']

						else:																	#error
							errQuant = True
							print('No way to find a solution because of the last layer!')
							break        
				else:

					if forward == True:

						print('We are in forward!')

						if i_bits_mem[i] == MIN_ACT_BITS and o_bits_mem[i] == MIN_ACT_BITS :	#error
							errQuant = True
							print('No way to find a solution: layer ',i,'with i_bits,o_bits =',MIN_ACT_BITS)
							break

						elif i_bits_mem[i] > o_bits_mem[i] and i_bits_mem[i] > MIN_ACT_BITS:	#backward
							okQuant = False
	   

						elif i_bits_mem[i] < o_bits_mem[i] and o_bits_mem[i] > MIN_ACT_BITS:	#forward
							param_list[i]['act_o_bits'] = int(o_bits_mem[i] / 2)
							param_list[i+1]['act_i_bits'] = param_list[i]['act_o_bits']
	  

						elif i_bits_mem[i] == o_bits_mem[i]:
							if i_mem > o_mem and i_bits_mem[i] > MIN_ACT_BITS :					#backward
								okQuant = False

							elif o_bits_mem[i] > MIN_ACT_BITS:									#forward
								param_list[i]['act_o_bits']  = int(o_bits_mem[i] / 2)
								param_list[i+1]['act_i_bits'] = param_list[i]['act_o_bits']

							else:																#error
								errQuant = True
								print('Corner case!')
								break

						elif o_bits_mem[i] > MIN_ACT_BITS :										#forward
							param_list[i]['act_o_bits']   = int(o_bits_mem[i] / 2)
							param_list[i+1]['act_i_bits'] = param_list[i]['act_o_bits']

						elif i_bits_mem[i] > MIN_ACT_BITS :										#backward
							okQuant = False

						else: 																	#error
							errQuant = True
							print('No way to find a solution!')
							break    

					else:
						print('We are in backward!')

						if i_bits_mem[i] == MIN_ACT_BITS and o_bits_mem[i] == MIN_ACT_BITS :	#error
							errQuant = True
							print('No way to find a solution: layer ',i,'with i_bits,o_bits =',MIN_ACT_BITS)
							break

						elif i_bits_mem[i] > o_bits_mem[i] and i_bits_mem[i] > MIN_ACT_BITS:	#backward
							param_list[i-1]['act_o_bits'] = int(i_bits_mem[i] / 2)  
							param_list[i]['act_i_bits'] = param_list[i-1]['act_o_bits']
	   

						elif i_bits_mem[i] < o_bits_mem[i] and o_bits_mem[i] > MIN_ACT_BITS:	#forward
							okQuant = False
	  

						elif i_bits_mem[i] == o_bits_mem[i]:
							if i_mem > o_mem and i_bits_mem[i] > MIN_ACT_BITS :					#backward
								param_list[i-1]['act_o_bits'] = int(i_bits_mem[i] / 2)
								param_list[i]['act_i_bits'] = param_list[i-1]['act_o_bits']

							elif o_bits_mem[i] > MIN_ACT_BITS:									#forward
								okQuant = False

							else:																#error
								errQuant = True
								print('Corner case!')
								break

						elif o_bits_mem[i] > MIN_ACT_BITS :										#forward
							okQuant = False

						elif i_bits_mem[i] > MIN_ACT_BITS :										#backward
							param_list[i-1]['act_o_bits'] = int(i_bits_mem[i] / 2)
							param_list[i]['act_i_bits']   = param_list[i-1]['act_o_bits']

						else: 																	#error
							errQuant = True
							print('No way to find a solution!')
							break    


				i_params_mem, i_bits_mem, o_params_mem, o_bits_mem = compute_activation_footprint(param_list)
				tot_layer_mem = ((i_params_mem[i] * i_bits_mem[i]) + (o_params_mem[i] * o_bits_mem[i]) )/ 8 # to bytes
				n_iter +=1

			else:

				if okQuant == True:
					print('Layer ',i,' is OK - \t Total Mem:', tot_layer_mem, 'with bits: ',i_bits_mem[i], o_bits_mem[i],i_params_mem[i],o_params_mem[i]  )
				else:
					print('Layer ',i,' is NOK - \t')
					okQuantNet = False

				if forward == True:
					if i == LAST:
						if okQuantNet == True:
							completeQuant = True # stop iteration
						else:
							forward = False	# continue in opposite direction
							okQuantNet = True	# reset of okQuantNet
					else:
						i += 1
				else:
					if i == 0:
						if okQuantNet == True:
							completeQuant = True # stop iteration
						else:
							forward = True	# continue in opposite direction
							okQuantNet = True	# reset of okQuantNet
					else:
						i -= 1

		
		# final print
		i_params_mem, i_bits_mem, o_params_mem, o_bits_mem = compute_activation_footprint(param_list)
		print('Input: ',i_bits_mem,'\n  Output:', o_bits_mem)
		if errQuant is True:
			print('No way for quantization')
			return -1
  

	####################################################################################
	###################### compute params footprint ####################################
	####################################################################################
	def compute_params_footprint(param_list):
		LAST = len(param_list)
		weight_footprint = [0 for x in range(LAST)]
		weight_bits = [0 for x in range(LAST)]
		for i,item in enumerate(param_list):
			#number of bits
			w_bits = item['w_bits']
					
			# size of convolutional parameters
			len_w = len(item['weight'].size())
			size_param = 1
			for v in range(len_w):
				size_param *= item['weight'].size(v)
			w_footprint = (size_param * w_bits) / 8 #measure in bytes
			weight_footprint[i] = w_footprint
			weight_bits[i] = w_bits
			
		return weight_footprint,sum(weight_footprint), weight_bits


	def compute_bias_footprint(param_list, cfg_quant='MixPL'):
		BIAS_PerLayerAsymPACT_BITS = 32
		BIAS_CH_PerLayerAsymPACT_BITS = 16
		BIAS_CH_ZW_BITS = 8
		BIAS_M0_BITS = 32
		BIAS_N0_BITS = 8

		LAST = len(param_list)
		weight_footprint = [0 for x in range(LAST)]
		for i,item in enumerate(param_list):

			#number of bits
			w_bits = item['w_bits']
			n_out_ch = item['weight'].size(0)
			a_o_bits = item['act_o_bits']
			# size of extra parametes
			quant_type = item['quant_type']
			fold_type = item['fold_type']

			if cfg_quant == 'MixPL':
				if quant_type is None:
					quant_type = 'PerLayerAsymPACT'
					fold_type = 'folding_weights'
				elif w_bits < 8:
					quant_type = 'PerLayerAsymPACT'
					fold_type = 'ICN'
				elif a_o_bits < 8:
					fold_type = 'ICN'
					quant_type = 'PerLayerAsymPACT'
			else:
				if quant_type is None:
					quant_type = 'PerChannelsAsymMinMax'
					fold_type = 'ICN'
				elif w_bits < 8:
					quant_type = 'PerChannelsAsymMinMax'
					fold_type = 'ICN'
				elif a_o_bits < 8:
					fold_type = 'ICN'
					quant_type = 'PerChannelsAsymMinMax'

			item['quant_type'] = quant_type
			item['fold_type'] = fold_type
			
			# compute number of bias
			if i == LAST-1:
				if quant_type == 'PerLayerAsymPACT':	
					bias_size  = int(n_out_ch * BIAS_PerLayerAsymPACT_BITS / 8 )		# bias
					bias_size += int(5 * BIAS_PerLayerAsymPACT_BITS / 8 )				# Zi,Zo,Zw,M0,N0 per layer
				elif quant_type == 'PerChannelsAsymMinMax':
					bias_size  = int(n_out_ch * BIAS_PerLayerAsymPACT_BITS / 8 ) 		# bias 
					bias_size += int(n_out_ch * BIAS_CH_ZW_BITS / 8 ) 		# Zw per channel
					bias_size += int(1 * BIAS_PerLayerAsymPACT_BITS / 8 ) 				# Zi,Zo per layer	
					bias_size += int(2*n_out_ch * BIAS_CH_ZW_BITS / 8 ) 	# M0,N0 last layer				
			
			elif i < LAST-1:
				if quant_type == 'PerLayerAsymPACT':
					if fold_type == 'folding_weights':
						bias_size  = int(n_out_ch * BIAS_PerLayerAsymPACT_BITS / 8 )		# bias
						bias_size += int(5 * BIAS_PerLayerAsymPACT_BITS / 8 )				# Zi,Zo,Zw,M0,N0 per layer
					elif fold_type == 'ICN':
						bias_size  = int(n_out_ch * BIAS_M0_BITS / 8 ) 			# M0 per channel
						bias_size += int(n_out_ch * BIAS_N0_BITS / 8 ) 			# N0 per channel
						bias_size += int(n_out_ch * BIAS_PerLayerAsymPACT_BITS / 8 ) 		# bias 
						bias_size += int(3 * BIAS_PerLayerAsymPACT_BITS / 8 ) 				# Zw,Zi,Zo per layer
					else:
						print('Error!')
						exit(0)

				elif quant_type == 'PerChannelsAsymMinMax':
					bias_size  = int(n_out_ch * BIAS_M0_BITS / 8 ) 			# M0 per channel
					bias_size += int(n_out_ch * BIAS_N0_BITS / 8 ) 			# N0 per channel
					bias_size += int(n_out_ch * BIAS_PerLayerAsymPACT_BITS / 8 ) 		# bias 
					bias_size += int(n_out_ch * BIAS_CH_ZW_BITS / 8 ) 		# Zw per channel
					bias_size += int(2 * BIAS_PerLayerAsymPACT_BITS / 8 ) 				# Zi,Zo per layer
 

			else:
				print('ERROR!!!')
			
			weight_footprint[i] = bias_size
			
		return weight_footprint

	def compute_footprint(param_list, cfg_quant='MixPL'):
		weight_footprint, _, _ = compute_params_footprint(param_list)
		bias_footprint = compute_bias_footprint(param_list, cfg_quant)
		ratio = np.array(bias_footprint) / np.array(weight_footprint)
		weight_footprint = sum(weight_footprint)
		bias_footprint = sum(bias_footprint)
		print('Total Weights: ',int(weight_footprint/1024),'kbytes | bias =' ,int(bias_footprint/1024),'kbytes ('\
			  ,100*bias_footprint/weight_footprint ,'%)')
		return bias_footprint+weight_footprint
			
			
	def compute_next_cut(param_list, MAX = 0.05, MIN_BIT=2):
		
		f,_,w_b = compute_params_footprint(param_list)
		weight = np.array(f)
		total = weight.sum()
		perc = weight / total
		arr = np.sort(perc)[::-1]
		ind = np.argsort(perc)[::-1]
		
		# remove w_bit = 1 from search
		rm = []
		for i,item in enumerate(w_b):
			if item == MIN_BIT:
				for i_x,x in enumerate(ind):
					if x == i:
						rm.append(i_x)
						continue

		arr = np.delete(arr, rm)
		ind = np.delete(ind, rm)
		
		if len(arr) == 0:
			return -1
			
		#print(arr, ind)
		LAST = len(arr)

		cut_we = arr[0]
		cut_i = ind[0]
		print('most footprint on layer: ',cut_i, 'equal to', cut_we, 'ratio' )
		thr = cut_we - MAX
		for i in range(1, LAST):
			if arr[i] > thr and ind[i] < cut_i:
				cut_i = ind[i]
				print(ind[i],arr[i])
		
		return cut_i            

		
	def weight_quantization(param_list, read_only_mem, cfg_quant='MixPL'):

		s = compute_footprint(param_list, cfg_quant)
		print('Total Footprint: ', s )

		while (s > read_only_mem):
		
			print('***************************************************************')
			c = compute_next_cut(param_list)
			if c == -1:
				print('No way for weight quantization')
				return -1
			
			nb = int(param_list[c]['w_bits'] / 2)
			print('Layer to cut: ', c, 'to ', nb  ,'bits')
			param_list[c]['w_bits'] = nb
			s = compute_footprint(param_list, cfg_quant)

		return(param_list)

	####################################################################################
	############ parse full precision network to map convolutional layers ##############
	####################################################################################
	def print_size(model, input, output): 
		global si, so
		si = input[0].size()
		so = output[0].size()

	activation_vector_i = []
	activation_vector_o = []
	param_list = []
	model.eval()

	for i,module in enumerate(model.modules()):
		if type(module) in [ Conv2d_SAME, nn.Conv2d]:
			print('Conv2d (Same) =: ',i)
			hook = module.register_forward_hook(print_size)
			model(x)
			hook.remove() 
			activation_vector_i.append(si)
			activation_vector_o.append(so)
			param_list.append({'act_o_bits':8, 'act_i_bits':8, 'w_bits':8 , 'weight': module.weight.data, 'quant_type': None, 'fold_type': None})
			
		elif type(module) is nn.Linear:
			print('Linear =: ',i)
			hook = module.register_forward_hook(print_size)
			model(x)
			hook.remove() 
			activation_vector_i.append(si)
			activation_vector_o.append(so)
			param_list.append({'act_o_bits':8, 'act_i_bits':8, 'w_bits':8 , 'weight': module.weight.data, 'quant_type': None, 'fold_type': None})

	LAST = len(activation_vector_i)-1

	for i,item in enumerate(activation_vector_i):
		print('Size of layer ', i , ': ', item)
				

	####################################################################################
	###################### cut weight & activation bits ################################
	####################################################################################	
	ret = cut_activation_footprint(param_list, READWRITE_MEM)
	if ret == -1:
		return -1	
	ret = weight_quantization(param_list, ONLY_READ_MEM, cfg_quant = cfg_quant)
	if ret == -1:
		return -1


	add_config = []
	LAST = len(param_list)-1
	for i,item in enumerate(param_list):
		print('***** Layer ', i , '**********')
		print('| i_bits : ',item['act_i_bits'], '\t| o_bits : ',item['act_o_bits'],'\t| w_bits' ,item['w_bits'],'\t| quant_type : ',item['quant_type'],'\t| fold_type : ',item['fold_type'])
		if i == LAST:
			add_config.append({'layer':i, 'w_bits': item['w_bits'], 'quant_type': item['quant_type'] })
		else:
			add_config.append({'layer':i, 'w_bits': item['w_bits'], 'quant_type': item['quant_type'] , 'fold_type': item['fold_type'], 'a_bits': item['act_o_bits'] })


	return add_config 