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

import math

import numpy as np
from importer.common.constant_mixin import ConstantMixin
from quantization.qtype import QType
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import domain, onnx_op


@domain('org.openvinotoolkit')
@onnx_op("FakeQuantize")
class NNCFFakeQuantize(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        levels = node.attrs['levels']
        auto_broadcast = node.attrs.get('auto_broadcast', 'numpy')
        if auto_broadcast != 'numpy':
            raise ValueError(f'{valid_name} - only numpy is supported for auto_broadcast')

        qopts = kwargs.get('qopts', {})
        x = inputs[0]
        # input_low = inputs[1]
        # input_high = inputs[2]
        output_low = cls.get_constant(inputs[3])
        output_high = cls.get_constant(inputs[4])

        if levels <= math.pow(2, 8):
            dtype = np.int8
        elif levels <= math.pow(2, 16):
            dtype = np.int16
        else:
            raise ValueError(f"{valid_name} - don't know how to handle more than {math.pow(2, 16)} levels")

        bits = int(math.log2(levels))
        qopts.setdefault(NodeId(x[0]), {'output_size': [None] * (x[1] + 1)})['output_size'][x[1]] = bits
        low_shape = output_low.shape
        high_shape = output_high.shape
        bc_dims_low = sum(1 for dim in high_shape if dim > 1)
        bc_dims_high = sum(1 for dim in high_shape if dim > 1)
        if bc_dims_low:
            if not bc_dims_high:
                raise ValueError(f"{valid_name} low output shape is broadcasted but high output shape is scalar")
            if bc_dims_high > 1 or bc_dims_low > 1:
                # could do a reduction here to axis 0
                raise ValueError(f"{valid_name} broadcast of output statistic on more than one dimension")
            low_axis = next(idx for idx, dim in enumerate(low_shape) if dim > 1)
            high_axis = next(idx for idx, dim in enumerate(high_shape) if dim > 1)
            if low_axis != high_axis:
                raise ValueError(f"{valid_name} low and high are not broadcasted on the same axis")
            min_val = output_low.flatten()
            max_val = output_high.flatten()
            channel = low_axis
        else:
            if bc_dims_high:
                raise ValueError(f"{valid_name} low output shape is scalar but high output shape is broadcasted")
            channel = None
            min_val = output_low.flatten()[0]
            max_val = output_high.flatten()[0]

        qtype = QType.from_min_max_sq(min_val, max_val, dtype=dtype, bits=bits, quantized_dimension=channel)

        all_nodes[node.output[0]] = tuple(list(x)[0:3] + [qtype])
        return None

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
