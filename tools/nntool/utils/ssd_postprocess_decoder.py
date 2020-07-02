# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# from . import get_ModelInfo_FromPipeline
import copy
import os
import json
import numpy as np

class ValException(Exception):
    pass

class DecodeBboxes(object):
    '''
    decode anchor boxes using the offsets estimated by ssd model. The class
    requires 4 parameters as x_scale, y_scale, w_scale, and h_scale. These
    four parameters are realated to the variances in the original ssd paper:

        _var = 1./_scale

    inputs : offsets and default anchors in one of the following format
        decoder_configuration = {'using_json_config':{'INCLUDE':False, 'json_config_path':post_processing_json_path},\
                    'using_pipeline_config':{'INCLUDE':False, 'pipeline_config_path':pipeline_config_path},\
                    'using_params':{'INCLUDE':True, 'params':{'x_scale':10.,'y_scale':10.,
                                                              'height_scale':5., 'width_scale':5.}}}
    outputs: decoded anchors
    '''
    def __init__(self, decoder_configuration):
        # pylint: disable=no-member
        # all allowed keys will be initialized as class attributes
        allowed_keys = set(['using_json_config', 'using_pipeline_config', 'using_params'])
        # initialize all allowed keys to false
        self.__dict__.update((key, None) for key in allowed_keys)
        # and update the given keys by their given values
        self.__dict__.update((key, value) for key, value in decoder_configuration.items() if key in allowed_keys)

        if self.using_json_config['INCLUDE']:
            json_config_path = self.using_json_config['json_config_path']
            if not os.path.isfile(json_config_path):
                raise ValException('\n\n the path to the json file is not valid.')
            with open(json_config_path) as json_file:
                json_data = json.load(json_file)
            # x_scale, y_scale, h_scale, w_scale
            if isinstance(json_data['x_scale'], (float, int)) and json_data['x_scale'] > 0:
                self.x_scale = json_data['x_scale']
            else:
                raise ValException('\n\n no valid x_scale is json file, shoulde be postive float/integer ...')
            if isinstance(json_data['y_scale'], (float, int)) and json_data['y_scale'] > 0:
                self.y_scale = json_data['y_scale']
            else:
                raise ValException('\n\n no valid y_scale is json file, shoulde be postive float/integer ...')
            if isinstance(json_data['h_scale'], (float, int)) and json_data['h_scale'] > 0:
                self.height_scale = json_data['h_scale']
            else:
                raise ValException('\n\n no valid h_scale is json file, shoulde be postive float/integer ...')
            if isinstance(json_data['w_scale'], (float, int)) and json_data['w_scale'] > 0:
                self.width_scale = json_data['w_scale']
            else:
                raise ValException('\n\n no valid w_scale is json file, shoulde be postive float/integer ...')
        elif self.using_pipeline_config['INCLUDE']:
            raise ValException('Need to import object_detection from tf')
            # pipeline_config_path = self.using_pipeline_config['pipeline_config_path']
            # if not os.path.isfile(pipeline_config_path):
            #     raise ValException('\n\n the path for pipeline configuration dose not exist...')
            # pipeline_info = get_ModelInfo_FromPipeline(pipeline_config_path).pipeline_config
            # box_coder = pipeline_info.model.ssd.box_coder.faster_rcnn_box_coder
            # # x_scale, y_scale, h_scale, w_scale
            # self.x_scale = box_coder.x_scale
            # self.y_scale = box_coder.y_scale
            # self.height_scale = box_coder.height_scale
            # self.width_scale = box_coder.width_scale
        elif self.using_params['INCLUDE']:
            params = self.using_params['params']
            if isinstance(params['x_scale'], (float, int)) and params['x_scale'] > 0:
                self.x_scale = params['x_scale']
            else:
                raise ValException('\n\n no valid x_scale is assigned, shoulde be postive float/integer ...')
            if isinstance(params['y_scale'], (float, int)) and params['y_scale'] > 0:
                self.y_scale = params['y_scale']
            else:
                raise ValException('\n\n no valid y_scale is assigned, shoulde be postive float/integer ...')
            if isinstance(params['h_scale'], (float, int)) and params['h_scale'] > 0:
                self.height_scale = params['h_scale']
            else:
                raise ValException('\n\n no valid height_scale is assigned, shoulde be postive float/integer ...')
            if isinstance(params['w_scale'], (float, int)) and params['w_scale'] > 0:
                self.width_scale = params['w_scale']
            else:
                raise ValException('\n\n no valid width_scale is assigned, shoulde be postive float/integer ...')
        # indices
        self.cnty, self.cntx, self.h, self.w = 0, 1, 2, 3
        self.ymin, self.xmin, self.ymax, self.xmax = 0, 1, 2, 3

    def convert_cors2cnts(self, bboxes_cors):
        bboxes_cors = copy.deepcopy(bboxes_cors)
        bboxes_cnts = np.zeros_like(bboxes_cors)
        bboxes_cors[:, self.h] = bboxes_cnts[:, self.ymax] - bboxes_cnts[:, self.ymin]
        bboxes_cors[:, self.w] = bboxes_cnts[:, self.xmax] - bboxes_cnts[:, self.xmin]
        bboxes_cors[:, self.cnty] = bboxes_cnts[:, self.ymin] + 0.5 * bboxes_cors[:, self.h]
        bboxes_cors[:, self.cntx] = bboxes_cnts[:, self.xmin] + 0.5 * bboxes_cors[:, self.w]
        return bboxes_cors

    def __call__(self, offsets_ref, anchors_ref, anchors_type='centers'):
        '''
        offsets: [batch_size, num_anchors, 4]
        anchors: [num_anchors, 4]: centers
        anchors_type: 'centers' | 'boxes'
        '''
        anchors = copy.deepcopy(anchors_ref)
        offsets = copy.deepcopy(offsets_ref)

        if anchors_type == 'centers':
            anchors_cnts = anchors
        else:
            anchors_cnts = self.convert_cors2cnts(anchors)

        # add batch dimension, if not included
        if offsets.ndim == 2:
            offsets = offsets.reshape([1, -1, offsets.shape[1]])
            anchors_cnts = anchors_cnts.reshape([1, -1, anchors_cnts.shape[1]])
        else: # offsets.ndim===3
            anchors_cnts = np.tile(anchors_cnts, (offsets.shape[0], 1, 1))

        # apply offsets
        ycenter = (offsets[:, :, self.cnty]/self.y_scale) * anchors_cnts[:, :, self.h] + anchors_cnts[:, :, self.cnty]
        xcenter = (offsets[:, :, self.cntx]/self.x_scale) * anchors_cnts[:, :, self.w] + anchors_cnts[:, :, self.cntx]
        half_h = 0.5 * np.exp(offsets[:, :, self.h]/self.height_scale) * anchors_cnts[:, :, self.h]
        half_w = 0.5 * np.exp(offsets[:, :, self.w]/self.width_scale)  * anchors_cnts[:, :, self.w]

        # min-max or corners format: required for nms
        decoded_anchors = np.zeros_like(anchors_cnts)
        decoded_anchors[:, :, self.ymin] = ycenter - half_h
        decoded_anchors[:, :, self.ymax] = ycenter + half_h
        decoded_anchors[:, :, self.xmin] = xcenter - half_w
        decoded_anchors[:, :, self.xmax] = xcenter + half_w
        return decoded_anchors
