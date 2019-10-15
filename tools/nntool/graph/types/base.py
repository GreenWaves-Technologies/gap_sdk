# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
from typing import Union
from abc import abstractmethod

from utils.graph import Edge, Node

LOG = logging.getLogger("nntool." + __name__)

class ParameterError(Exception):
    pass

class CantPromoteQError(ParameterError):
    pass

# pylint: disable=too-many-instance-attributes
class Parameters(Node):
    op_name = "unknown"

    def __init__(self, name, *args, **kwargs):
        super().__init__(name)
        del args, kwargs
        self._in_dims = None
        self._out_dims = None
        self._in_dims_hint = None
        self._out_dims_hint = None
        self._step_idx = -1

    def get_parameters(self):
        return {}

    def set_parameters(self, val):
        pass

    @property
    def step_idx(self):
        return self._step_idx

    @step_idx.setter
    def step_idx(self, val):
        self._step_idx = val

    @property
    def in_dims_hint(self) -> list:
        return self._in_dims_hint

    @in_dims_hint.setter
    def in_dims_hint(self, val: list):
        self._in_dims_hint = val

    @property
    def out_dims_hint(self) -> list:
        return self._out_dims_hint

    @out_dims_hint.setter
    def out_dims_hint(self, val: list):
        self._out_dims_hint = val

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, val):
        self._value = val

    @property
    def can_promoteq(self):
        return False

    def promoteq(self):
        raise CantPromoteQError()

    @property
    def in_dims(self):
        return self._in_dims

    @in_dims.setter
    def in_dims(self, value):
        assert isinstance(value, list), "in_dims should always be a list"
        LOG.debug("%s in dims set to %s", self.__class__.__name__, [str(val) for val in value])
        self._in_dims = value

    @property
    def out_dims(self):
        return self._out_dims

    @out_dims.setter
    def out_dims(self, value):
        LOG.debug("%s out dims set to %s", self.__class__.__name__, str(value))
        self._out_dims = value

    @abstractmethod
    def get_parameter_size(self):
        pass

    @property
    @abstractmethod
    def can_equalize(self):
        pass

    @abstractmethod
    def clone(self, groupn=None):
        pass

    @abstractmethod
    def __str__(self):
        pass

class SingleInputAndOutput():
    '''Mixin that indicates that node has a single input and output'''

class NoSizeChangeParameters(Parameters):

    def get_output_size(self, in_dims):
        return in_dims

    @abstractmethod
    def clone(self, groupn=None):
        pass

    @abstractmethod
    def __str__(self):
        pass

#pylint: disable=abstract-method
class FilterLikeParameters(Parameters, SingleInputAndOutput):

    def __init__(self, name, *args, stride=None, padding=None,
                 pad_type="zero", **kwargs):
        assert stride and padding
        super(FilterLikeParameters, self).__init__(name, stride=None, padding=None,
                                                   pad_type="zero", *args, **kwargs)
        self.stride = stride
        self.padding = padding
        self.pad_type = pad_type

    def has_at_zero_pad(self):
        if self.padding.has_at_pad():
            if self.pad_type != "zero":
                raise AttributeError("Padding is probably not compatible with AutoTiler")
            return True
        return False

    @property
    def can_equalize(self):
        return True

#pylint: disable=abstract-method
class FilterParameters(Parameters, SingleInputAndOutput):

    def __init__(self, name, *args, filt=None, has_bias=False, **kwargs):
        assert filt
        super(FilterParameters, self).__init__(name, filt=None, has_bias=False, *args, **kwargs)
        self.has_bias = has_bias
        self.filter = filt
        self.stats = None
        self.weights = None
        self.biases = None
        self.details = None

    def get_parameters(self):
        return {'weights': self.weights, 'biases': self.biases}

    def set_parameters(self, val):
        self.weights = val['weights']
        self.biases = val['biases']

    # @property
    # def can_promoteq(self):
    #     return self.out_q.bits < 16

    # def step_filter_q(self, in_q):
    #     '''Step up the precision of the filter. Returns True if the output
    #     quantization has changed'''
    #     if self.out_q.bits == 16:
    #         raise ValueError("can't step further")

    #     if self.calc_q.bits <= in_q.bits and self.calc_q.bits < STATS_BITS[-1]:
    #         self.calc_q = QType(bits=self.calc_q.bits * 2,
    #                             q=in_q.q + self.weights_q.q,
    #                             signed=True)
    #         return False

    #     if self.acc_q.bits < self.calc_q.bits:
    #         self.acc_q = self.calc_q.clone()
    #         return False

    #     if self.out_q.bits < self.weights_q.bits:
    #         self.out_q = QType(bits=self.weights_q.bits,
    #                            q=self.weights_q.bits - (self.out_q.bits - self.out_q.q),
    #                            signed=True)
    #         return True

    #     fstats = self.stats

    #     w_qsnr = get_current_qsnr(fstats['weights'], self.weights_q.bits)
    #     if 'biases' in fstats:
    #         b_qsnr = get_current_qsnr(fstats['biases'], self.weights_q.bits)
    #     else:
    #         b_qsnr = float('Infinite')

    #     if b_qsnr < w_qsnr and self.biases_q.bits < self.calc_q.bits:
    #         self.biases_q = get_quantization(fstats['biases'], None, self.calc_q.bits)
    #         return False
    #     self.weights_q = get_quantization(fstats['weights'], None, self.calc_q.bits)
    #     assert self.calc_q.bits < 32
    #     # TODO - Calculate if we really need to expand calc_q
    #     self.calc_q = QType(bits=self.calc_q.bits * 2,
    #                         q=self.weights_q.q + in_q.q, signed=True)
    #     return False

    # def promoteq(self):
    #     return self.step_filter_q(self.in_qs[0])

    # @abstractmethod
    # def scale_input(self, scale, weights=None):
    #     pass

    # @abstractmethod
    # def scale_output(self, scale, weights=None, biases=None):
    #     pass

class EdgeParameters():
    # edge types are in, out or in_out
    def __init__(self, name, dims, creating_node, creating_node_idx, creating_step,
                 edge_type="in_out", edge_order=None):
        self._name = name
        self._dims = dims
        self._creating_node = creating_node
        self._creating_node_idx = creating_node_idx
        self._creating_step = creating_step
        self._edge_type = edge_type
        self._edge_order = edge_order

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def dims(self):
        return self._dims

    @dims.setter
    def dims(self, val):
        self._dims = val

    @property
    def creating_node(self):
        return self._creating_node

    @creating_node.setter
    def creating_node(self, val):
        self._creating_node = val

    @property
    def creating_node_idx(self):
        return self._creating_node_idx

    @creating_node_idx.setter
    def creating_node_idx(self, val):
        self._creating_node_idx = val

    @property
    def creating_step(self):
        return self._creating_step

    @creating_step.setter
    def creating_step(self, val):
        self._creating_step = val

    @property
    def edge_type(self):
        return self._edge_type

    @edge_type.setter
    def edge_type(self, val):
        self._edge_type = val

    @property
    def edge_order(self):
        return self._edge_order

    @edge_order.setter
    def edge_order(self, val):
        self._edge_order = val

class NNEdge(Edge):
    def __init__(self, from_node: Union[str, Node], to_node: Union[str, Node],
                 params: Parameters = None,
                 from_idx: int = 0, to_idx: int = 0):
        super().__init__(from_node, to_node, from_idx, to_idx)
        self.params = params
