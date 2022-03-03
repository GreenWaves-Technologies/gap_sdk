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

import logging

import numpy as np
from graph.dim import Dim
from quantization.qtype import QType
from quantization.compression.compress import CompressionError, compress

from .base import NNNodeRef, cls_op_name
from .input_output import InputBaseParameters

LOG = logging.getLogger("nntool." + __name__)


@cls_op_name('constant')
class ConstantInputParameters(InputBaseParameters):

    def __init__(self, *args, adjust_transpose=None, is_mutated=False,
                 is_intermediate=False, always_copy=False, value: np.ndarray = None,
                 qtype: QType = None, dims: Dim = None, **kwargs):
        if dims is None:
            dims = Dim.unnamed(value.shape)
        super(ConstantInputParameters, self).__init__(
            *args, dims=dims, **kwargs)
        self._value = value
        del self.at_options.valid_options['FIXED_ORDER']
        self.at_options.valid_options['RESET_NAME'] = str
        self._adjust_transpose = adjust_transpose
        self._is_mutated = is_mutated
        self._is_intermediate = is_intermediate
        self._is_constant = True
        self._is_global = True
        self._always_copy = always_copy
        self._use_fake = False
        self._use_compressed = False
        self._compressed_value = None
        self._qtype = qtype

    def __call__(self, graph):
        if graph.__class__.__name__ != 'NNGraph':
            raise ValueError('expecting NNGraph as parameter')
        return NNNodeRef(graph, self, 0)

    @classmethod
    def fake(cls, G, val):
        for node in G.nodes(node_classes=ConstantInputParameters):
            node.use_fake = val

    @classmethod
    def save_compression_state(cls, G):
        res = []
        for node in G.nodes(node_classes=ConstantInputParameters):
            if node.use_compressed:
                res.append(node)
                node.use_compressed = False
        return res

    @classmethod
    def restore_compression_state(cls, G, state):
        for node in state:
            node.use_compressed = True

    @classmethod
    def clear_compression_state(cls, G):
        for node in G.nodes(node_classes=ConstantInputParameters):
            node.clear_compression()

    def verify(self, G):
        problems = []
        for edge in G.in_edges(self.name):
            problems.append(
                f'constant input node {self.name} has input edge from {edge.from_node.name}:{edge.from_idx}')
        for edge in G.out_edges(self.name):
            if edge.from_idx > 0:
                problems.append(f'constant input node {self.name} has output edge on idx {edge.from_idx} '
                                f'to {edge.to_node.name}:{edge.to_idx}')
        if self.value is None:
            problems.append(f'constant node {self.name} has no value')
        return problems

    @property
    def use_fake(self):
        return self._use_fake

    @use_fake.setter
    def use_fake(self, val):
        self._use_fake = val

    @property
    def graph_anon_label(self):
        return ['Constant']

    @property
    def qtype(self):
        return self._qtype

    @qtype.setter
    def qtype(self, val):
        self._qtype = val

    @property
    def dqvalue(self):
        if self._use_compressed and self._compressed_value is not None:
            return self.value
        if self._qtype:
            return self._qtype.dequantize(self.value)
        return self.value

    @dqvalue.setter
    def dqvalue(self, val):
        self._qtype = None
        self.value = val

    @property
    def value(self):
        if self._use_fake:
            return np.random.normal(loc=0, scale=0.2, size=self.out_dims[0].shape)
        if self._use_compressed and self._compressed_value is not None:
            value = self._compressed_value.value
        else:
            value = self._value

        if self._always_copy and isinstance(value, np.ndarray):
            return value.copy()
        return value

    @value.setter
    def value(self, val):
        self._value = val

    def value_as(self, qtype, generation=False):
        # handles both None or both equal
        if self._qtype == qtype:
            return self.value
        # need to quantize
        if self._qtype is None or (self._use_compressed and self._compressed_value is not None):
            return qtype.quantize(self.value)
        # need to dequantize maybe need to quantize
        return qtype.quantize(self.dqvalue) if qtype else self.dqvalue

    def compress_value(self, qtype=None, bits=None, min_qsnr=None, force_sparse=False,
                       allow_sparse=True, threshold=None):
        if qtype:
            if (qtype.attr.no_compression or qtype.offset or qtype.attr.interleaved_values):
                raise CompressionError(f'{self.name} quantization parameters prevent compression')
            qbits = qtype.bits
        else:
            qbits = 8
        value = self._qtype.dequantize(self._value) if self._qtype else self._value
        self._compressed_value = compress(value, bits=bits, min_qsnr=min_qsnr, force_sparse=force_sparse,
                                          allow_sparse=allow_sparse, qbits=qbits, threshold=threshold)

    @property
    def always_copy(self):
        return self._always_copy

    @always_copy.setter
    def always_copy(self, val):
        self._always_copy = val

    @property
    def reset_name(self):
        return self.at_options.reset_name

    @reset_name.setter
    def reset_name(self, val):
        self.at_options.reset_name = val

    @property
    def adjust_transpose(self):
        return self._adjust_transpose

    @adjust_transpose.setter
    def adjust_transpose(self, val):
        self._adjust_transpose = val

    @property
    def compressed_value(self):
        return self._compressed_value

    @compressed_value.setter
    def compressed_value(self, val):
        self._compressed_value = val

    @property
    def is_compressed(self):
        return self._use_compressed

    @property
    def use_compressed(self):
        return self._use_compressed

    def clear_compression(self):
        self._compressed_value = None
        self._use_compressed = False

    @use_compressed.setter
    def use_compressed(self, val):
        self._use_compressed = val

    @property
    def is_constant(self):
        return self._is_constant

    @is_constant.setter
    def is_constant(self, val):
        self._is_constant = val

    @property
    def is_global(self):
        return self._is_global

    @is_global.setter
    def is_global(self, val):
        self._is_global = val

    @property
    def is_mutated(self):
        return self._is_mutated

    @is_mutated.setter
    def is_mutated(self, val):
        self._is_mutated = val

    @property
    def is_intermediate(self):
        return self._is_intermediate

    @is_intermediate.setter
    def is_intermediate(self, val):
        self._is_intermediate = val

    def get_parameter_size(self):
        if self._use_compressed and self._compressed_value:
            return self._compressed_value.size
        return self.dims.size()

    def get_parameters(self):
        return {'value': self.value}

    def set_parameters(self, val):
        self.value = val['value']

    def __str__(self):
        props = [param for param in ["is_mutated", "is_intermediate",
                                     "is_compressed"] if getattr(self, param)]
        return "Const {} {} {}".format(
            self.dims,
            " ".join(props),
            self.at_options
        )
