#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jan 31 16:14:35 2020

@author: ahmad
"""

import os.path
import pandas as pd
from tqdm import tqdm
import random
from prettytable import PrettyTable
class Input_Exception(Exception):
    pass

class data_division:
    

    def __init__(self, annotations_path, percentages, shuffling_info):
        
        
        '''
        Inputs:
            
            
            - annotations_path: the path to annotations csv file path 

            
            - percentages: samples are being divided into train, validatin, test, and fine_tuning divisions according to assigned percentages.
            
                e.g., percentages = {'train': 70, 'val': 10, 'test': 10, 'train_fine_tuning':10, 'val_fine_tuning':10, 'test_fine_tuning':10}
        
            - shuffling_info: to shuffle sample before division.
            
                e.g., shuffling_info   = {'INCLUDE': True, 'random_seed': 1000, 'num_shuffles': 1000}
        
        
        Outputs:
            
            exported dataframes in csv format including selected samples for each task (train, val, etc.)
        
        
        
        '''
        ########################################
        # check valididty of the annotation path 
        if not (isinstance(annotations_path, str)):
            raise Input_Exception('\n\n Data division process: the given file path for annotations is not <str> type.\n\n')
        if not os.path.isfile(annotations_path):
            raise Input_Exception('\n\n Data division process: the given file path for annotations does not exist.\n\n')
        self.annotations_path = annotations_path

        ########################################
        # percentages
        required_keys_for_percentages_dict= ('train', 'val', 'test', 'train_fine_tuning', 'val_fine_tuning', 'test_fine_tuning')
        if not(isinstance(percentages, dict)):
            raise Input_Exception('\n\n Data division process: Percentages info must be provided in a dict type structure with keys: \
                                  {valid_keys}.\n\n'.format(valid_keys=required_keys_for_percentages_dict))
        if not(set(required_keys_for_percentages_dict).issubset(percentages)):
            raise Input_Exception('\n\n Data division process: Percentages info dict does not include valid keys: \
                                  {valid_keys}.\n\n'.format(valid_keys=required_keys_for_percentages_dict))
        if not all((isinstance(val,(int,float)) and val>0) for val in percentages.values()):
            raise Input_Exception('\n\n Data division process: Percentages info dict does not include valid numbers:\
                                  positive ints/floats.\n\n')
        if sum(percentages.values())>100.0:
            raise Input_Exception('\n\n Data division process: Percentages info dict values has a summation of more than 100%.\n\n')
        # percentages info is valid
        self.percentages = percentages
        
        ########################################
        # shuffeling info
        required_keys_for_shuffling_info_dict= ('INCLUDE', 'random_seed', 'num_shuffles')
        if not(isinstance(shuffling_info, dict)):
            raise Input_Exception('\n\n Data division process: shuffling info must be provided in a dict type structure with keys: \
                                  {valid_keys}.\n\n'.format(valid_keys=required_keys_for_shuffling_info_dict))
        if not(set(required_keys_for_shuffling_info_dict).issubset(shuffling_info)):
            raise Input_Exception('\n\n Data division process: shuffling info dict does not include valid keys: \
                                  {valid_keys}.\n\n'.format(valid_keys=required_keys_for_shuffling_info_dict))
        if not isinstance(shuffling_info['INCLUDE'],bool):
            raise Input_Exception('\n\n Data division process: shuffling info dict [INCLUDE] must be boolean.\n\n')
        if not ((shuffling_info['random_seed'] is None) or (isinstance(shuffling_info['random_seed'], int) and shuffling_info['random_seed']>0)):
            raise Input_Exception('\n\n Data division process: shuffling info, random_seed must be initialize to None or positive int.\n\n')
        if not (isinstance(shuffling_info['num_shuffles'], int) and shuffling_info['num_shuffles']>0):
            raise Input_Exception('\n\n Data division process: shuffling info, positive integers are required for num_shuffles.\n\n')
        # percentages info is valid
        self.shuffling_info = shuffling_info
        ########################################
        
    
    
    
        ########################################    
        # annotations, dataframe
        annotations_df = pd.read_csv(self.annotations_path)
        # annotations_df = pd.read_csv(self.annotations_path, names=['image_name','xmin', 'xmax','ymin', 'ymax', 'class_id'])
        
        # suffling
        if not (shuffling_info['random_seed'] is None):random.seed(shuffling_info['random_seed'])
        uframe = list(annotations_df.image_name.unique())
        num_images = len(uframe)
        if shuffling_info['INCLUDE']:
            for i in range(shuffling_info['num_shuffles']): random.shuffle(uframe)
    
    
        ########################################
        # selections
        
        train_per    = percentages['train']
        val_per      = percentages['val']
        test_per     = percentages['test']
        train_ft_per = percentages['train_fine_tuning']
        val_ft_per   = percentages['val_fine_tuning']
        test_ft_per  = percentages['test_fine_tuning']
       
        
        
        
        
        x = PrettyTable()
        print_width = 10
                
        x.field_names = ['set', 'percentages (%)', 'starting', 'ending', 'num_samples', 'selection precentages']
        
        # train
        take_from, take_to = (0, int(train_per * num_images/100))
        train_frames = uframe[take_from:take_to]       
        x.add_row( ['{0: <{width}}'.format('train', width=print_width), train_per,\
                    take_from, take_to-1, (take_to-take_from),\
                    round((take_to-take_from)/num_images,2)])

        # validation
        take_from, take_to = (take_to, take_to+int(val_per * num_images/100))
        val_frames   = uframe[take_from:take_to]
        x.add_row( ['{0: <{width}}'.format('val', width=print_width), val_per,\
            take_from, take_to-1, (take_to-take_from),\
            round((take_to-take_from)/num_images,2)])

        # test
        take_from, take_to = (take_to, take_to+int(test_per * num_images/100))
        test_frames   = uframe[take_from:take_to]
        x.add_row( ['{0: <{width}}'.format('test', width=print_width), test_per,\
            take_from, take_to-1, (take_to-take_from),\
            round((take_to-take_from)/num_images,2)])

        # fine-tuning train 
        take_from, take_to = (take_to, take_to+int(train_ft_per * num_images/100))
        train_ft_frames   = uframe[take_from:take_to]
        x.add_row( ['{0: <{width}}'.format('train fine-tuning', width=print_width), train_ft_per,\
            take_from, take_to-1, (take_to-take_from),\
            round((take_to-take_from)/num_images,2)])
        
        # fine-tuning val
        take_from, take_to = (take_to, take_to+int(val_ft_per * num_images/100))
        val_ft_frames   = uframe[take_from:take_to]
        x.add_row( ['{0: <{width}}'.format('val fine-tuning', width=print_width), val_ft_per,\
            take_from, take_to-1, (take_to-take_from),\
            round((take_to-take_from)/num_images,2)])

    
        # fine-tuning test
        take_from, take_to = (take_to, take_to+int(test_ft_per * num_images/100))
        test_ft_frames   = uframe[take_from:take_to]
        x.add_row( ['{0: <{width}}'.format('test fine-tuning', width=print_width), test_ft_per,\
            take_from, take_to-1, (take_to-take_from),\
            round((take_to-take_from)/num_images,2)])
        
    
        print(x)
    
        # correct order of columns
        columnsOrder = ['image_name', 'xmin', 'xmax', 'ymin', 'ymax', 'class_id']
        
        ########################################
        # take samples 
        #
        # train
        train_indices = annotations_df.image_name.infer_objects().isin(train_frames)
        train_data = annotations_df[train_indices]
        self.train_data = train_data[columnsOrder]
        # 
        # validation
        val_indices = annotations_df.image_name.infer_objects().isin(val_frames)
        val_data = annotations_df[val_indices]
        self.val_data = val_data[columnsOrder]
        #
        # test
        test_indices = annotations_df.image_name.infer_objects().isin(test_frames)
        test_data = annotations_df[test_indices]
        self.test_data = test_data[columnsOrder]
        #
        # fine tuning samples for train
        train_fine_tuning_indices = annotations_df.image_name.infer_objects().isin(train_ft_frames)
        train_fine_tuning_data = annotations_df[train_fine_tuning_indices]
        self.train_fine_tuning_data = train_fine_tuning_data[columnsOrder]

        #
        # fine tuning samples for validation
        val_fine_tuning_indices = annotations_df.image_name.infer_objects().isin(val_ft_frames)
        val_fine_tuning_data = annotations_df[val_fine_tuning_indices]
        self.val_fine_tuning_data = val_fine_tuning_data[columnsOrder]
        
        
        
        # fine tuning samples for test
        test_fine_tuning_indices = annotations_df.image_name.infer_objects().isin(test_ft_frames)
        test_fine_tuning_data    = annotations_df[test_fine_tuning_indices]
        self.test_fine_tuning_data = test_fine_tuning_data[columnsOrder]
        
        
        ########################################
        # export the sample information
        export_path = os.path.dirname(self.annotations_path)
        self.train_data.to_csv(os.path.join(export_path,'train_data.csv'), index = False) 
        self.val_data.to_csv(os.path.join(export_path,'val_data.csv'), index = False) 
        self.test_data.to_csv(os.path.join(export_path,'test_data.csv'), index = False) 
        self.train_fine_tuning_data.to_csv(os.path.join(export_path,'train_fine_tuning_data.csv'), index = False) 
        self.val_fine_tuning_data.to_csv(os.path.join(export_path,'val_fine_tuning_data.csv'), index = False) 
        self.test_fine_tuning_data.to_csv(os.path.join(export_path,'test_fine_tuning_data.csv'), index = False) 
