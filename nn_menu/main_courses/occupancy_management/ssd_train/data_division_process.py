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

from data_preparation.preparation_utils import data_division
import os

data_path = './data'
annotations_path = os.path.join(data_path, 'annotations.csv')

percentages = {'train': 70, 'val': 10, 'test': 5, 'train_fine_tuning':5, 'val_fine_tuning':5, 'test_fine_tuning':5}

shuffling_info = {'INCLUDE': True, 'random_seed': 1000, 'num_shuffles': 5}

'''
Samples taken for each task are exported in the given annotation_path, and the user can access to each one 
through the <datasets> object's attributes.' 

they are accecible in datasets object whithin the attributes below (in dataframe format):
    
    train             : datasets.train_data
    validation        : datasets.val_data
    test              : datasets.test_data
    train fine-tuning : datasets.train_fine_tuning_data
    val fine-tuning   : datasets.val_fine_tuning_data
    test fine-tuning  : datasets.test_fine_tuning_data
    
'''
datasets  = data_division(annotations_path, percentages, shuffling_info)