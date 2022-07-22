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

from onnx import numpy_helper
import numpy as np
from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode
from nntool.importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import onnx_op, ps_description, partial_support


@onnx_op("Constant")
@partial_support(True)
@ps_description('sparse and string constants are not supported')
class Constant(BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        if node.attrs.get('value'):
            value = numpy_helper.to_array(node.attrs['value'])
        elif node.attrs.get('value_float'):
            value = np.atleast_1d(node.attrs['value_float'], dtype=np.float32)
        elif node.attrs.get('value_floats'):
            value = np.array(node.attrs['value_floats'], dtype=np.float32)
        elif node.attrs.get('value_int'):
            value = np.atleast_1d(node.attrs['value_int'], dtype=np.int32)
        elif node.attrs.get('value_ints'):
            value = np.array(node.attrs['value_ints'], dtype=np.int32)
        elif node.attrs.get('value_string') or node.attrs.get('value_strings'):
            raise NotImplementedError('NNTOOL does not support string constants')
        elif node.attrs.get('sparse_value'):
            raise NotImplementedError('NNTOOL does not support sparse constants')
        else:
            raise ValueError('ONNX constant has no value')

        params = ConstantInputNode(valid_name, dims=Dim.unnamed(value.shape), value=value)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(value.shape), None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_12(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
