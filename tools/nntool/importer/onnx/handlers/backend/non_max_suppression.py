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

import numpy as np
from graph.types import NNEdge
from graph.types.ssd import NMSParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("NonMaxSuppression")
class NonMaxSuppression(ConstantMixin, BackendHandler):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]

        boxes = inputs[0]
        scores = inputs[1]
        scores_shape = scores[2].shape
        num_classes = scores_shape[1]
        max_output_boxes_per_class = cls.optional_constant_scalar(inputs, 2, 0, dtype=np.int64)
        iou_threshold = cls.optional_constant_scalar(inputs, 3, 0)
        score_threshold = cls.optional_constant_scalar(inputs, 4, 0)
        center_point_box = node.attrs.get('center_point_box', 0)

        parameters = {
            'nms_score_threshold': score_threshold,
            'nms_iou_threshold': iou_threshold,
            'max_output_boxes_per_class': max_output_boxes_per_class,
            'num_classes': num_classes,
            'center_point_box': center_point_box
        }

        pshape = ProvisionalDim([num_classes*max_output_boxes_per_class, 3])

        params = NMSParameters(valid_name, parameters=parameters,
                               in_dims_hint=[['batch', 'spatial_dim', 'box'], ['batch', 'class', 'spatial_dim']],
                               out_dims_hint=[['spatial_dim', 'index']])
        G.add_edge(NNEdge(from_node=boxes[0], to_node=params, from_idx=boxes[1], to_idx=0))
        G.add_edge(NNEdge(from_node=scores[0], to_node=params, from_idx=scores[1], to_idx=1))
        all_nodes[node.output[0]] = (params, 0, pshape, None)
        return params


    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls._common(node, **kwargs)
