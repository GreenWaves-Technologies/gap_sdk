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
import copy

import numpy as np
import scipy.special
from graph.types import (ConstantInputParameters, LogOpParameters, NNEdge,
                         SoftMaxParameters)
from importer.common.constant_mixin import ConstantMixin
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op


def softmax_func(v, axis=None):
    return scipy.special.softmax(v, axis=axis)

@onnx_op("LogSoftmax")
class LogSoftmax(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, v13=False, **kwargs):
        all_nodes = kwargs['all_nodes']
        valid_name = kwargs['valid_name']
        G = kwargs['G']
        inputs = [all_nodes[inp] for inp in node.input]
        axis = node.attrs.get('axis', None)
        # may have more than one input i.e. clip
        x = inputs[0]
        x_shape = x[2].shape
        if axis and axis < 0:
            axis += len(x_shape)
        axis = cls._trim_axis(axis, x_shape)
        if axis != 0 and not v13:
            ValueError('LogSoftmax does not support ONNX version < 13 with axis not first')
        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(valid_name, value=np.log(softmax_func(cls.get_constant(x), axis=axis)))
        else:
            softmax_params = SoftMaxParameters(f'{valid_name}_softmax', axis=axis)
            G.add_edge(NNEdge(from_node=x[0], to_node=softmax_params, from_idx=x[1], to_idx=0))
            params = LogOpParameters(f'{valid_name}_log')
            G.add_edge(NNEdge(from_node=softmax_params, to_node=params))
        all_nodes[node.output[0]] = (params, 0, copy.deepcopy(x[2]), None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, v13=True, **kwargs)
