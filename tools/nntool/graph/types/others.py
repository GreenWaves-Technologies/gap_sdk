# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
import sys

from ..dim import Dim
from .base import (FilterParameters, NoSizeChangeParameters, Parameters,
                   SingleInputAndOutput)

LOG = logging.getLogger("nntool." + __name__)

class InputOutputParameters(Parameters):

    def __init__(self, name, dims):
        super().__init__(name)
        self._output_value = None
        self._index = None
        self.dims = dims

    @property
    def output_value(self):
        return self._output_value

    @output_value.setter
    def output_value(self, value):
        self._output_value = value

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, value):
        self._index = value

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, groupn=None):
        raise NotImplementedError()

class InputParameters(InputOutputParameters):
    op_name = "input"

    def set_input(self, value):
        try:
            value = value.reshape(self.dims.shape)
        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s}".format(ex)
            ).with_traceback(trace_back)
        self.output_value = value

    # @property
    # def can_promoteq(self):
    #     return self.out_q.bits < STATS_BITS[-1]

    # def promoteq(self):
    #     if self.out_q.bits == STATS_BITS[-1]:
    #         raise ValueError("can't step further")
    #     self.out_q = get_quantization(self.activation_stats, None, self.out_q.bits * 2)
    #     return True

    def __str__(self):
        return "I {}".format(
            self.dims
        )

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        return [out_dim]

    def clone(self, groupn=None):
        raise NotImplementedError()

class OutputParameters(InputOutputParameters):
    op_name = "output"

    def __init__(self, name):
        super(OutputParameters, self).__init__(name, None)

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        return [out_dim]

    @property
    def out_dims(self):
        return [self.dims]

    @out_dims.setter
    def out_dims(self, val):
        self.dims = val[0]

    def __str__(self):
        return "O {}".format(
            self.dims
        )

    def clone(self, groupn=None):
        raise NotImplementedError()

class ActivationParameters(NoSizeChangeParameters, SingleInputAndOutput):

    def __init__(self, name, activation="relu"):
        super(ActivationParameters, self).__init__(name)
        self.activation = activation

    @property
    def op_name(self):
        return self.activation

    def get_parameter_size(self):
        return 0

    def clone(self, groupn=None):
        return ActivationParameters(self.activation)

    @property
    def can_equalize(self):
        # Is leaky usable? Looks like it.
        return self.activation == "relu" or self.activation == "leaky"

    def __str__(self):
        return "Activation {}".format(
            self.activation
        )

class ConcatParameters(Parameters):
    op_name = "concat"

    def __init__(self, name, axis=None):
        super(ConcatParameters, self).__init__(name)
        self.axis = axis

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        self.in_dims = in_dims
        out_dim = Dim.combine([in_dim for in_dim in in_dims], self.axis)
        return [out_dim]

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "A {}".format(
            self.axis
        )

class FusionParameters(Parameters, SingleInputAndOutput):
    '''Fusion of operators. At present restricted to single input and output but
    this could be removed perhaps'''

    op_name = "fusion"

    def __init__(self, name, fusion_type, nodes):
        super(FusionParameters, self).__init__(name)
        self.in_dims_hint = nodes[0].in_dims_hint
        self.out_dims_hint = nodes[-1].out_dims_hint
        self.fusion_type = fusion_type
        self._nodes = nodes

    def contained_filters(self):
        return [x for x in self._nodes if isinstance(x, FilterParameters)]

    def get_parameter_size(self):
        return sum([node.get_parameter_size() for node in self._nodes])

    @property
    def quantized_weights(self):
        filters = self.contained_filters()
        if filters:
            return filters[0].quantized_weights

    @property
    def quantized_biases(self):
        filters = self.contained_filters()
        if filters:
            return filters[0].quantized_biases

    # Needs to be refactored out
    @property
    def params(self):
        return self._nodes

    def contained_nodes(self):
        return self._nodes

    def get_contained_node(self, name):
        return next((n for n in  self._nodes if n.name == name), None)

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        out_dims = in_dims

        for node in self._nodes:
            out_dims = node.get_output_size(out_dims)

        return out_dims

    @property
    def can_equalize(self):
        return all([param.can_equalize for param in self._nodes])

    def clone(self, groupn=None):
        new_params = []
        for node in self._nodes:
            new_params.append(node.clone(groupn=groupn))

        return FusionParameters(self.name, self.fusion_type, new_params)

    def __str__(self):
        return ",".join([str(node) for node in self._nodes])

class GroupParameters(Parameters):

    op_name = "group"

    def __init__(self, name, groups, in_dims_hint=None, out_dims_hint=None):

        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

        super(GroupParameters, self).__init__(name)
        self.groups = groups

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]
        assert in_dims.c % self.groups == 0
        out_edges = in_dims.c // self.groups
        out_c = in_dims.c // out_edges
        out_dim = in_dims.clone(['c', 'h', 'w'])
        out_dim.c = out_c
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "GRPS {}".format(
            self.groups
        )

class PadParameters(Parameters, SingleInputAndOutput):
    op_name = "pad"

    def __init__(self, name, padding, in_dims_hint=None, out_dims_hint=None):
        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

        super(PadParameters, self).__init__(name)
        self.padding = padding

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = in_dims
        out_dim = in_dims[0].clone()
        out_dim.w += self.padding.w
        out_dim.h += self.padding.h
        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "PAD {}".format(self.padding)

class UpsampleParameters(Parameters, SingleInputAndOutput):

    op_name = "upsample"

    def __init__(self, name, algo, factor, in_dims_hint=None, out_dims_hint=None):

        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

        super(UpsampleParameters, self).__init__(name)
        self.algo = algo
        self.factor = factor

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]

        out_dim = in_dims.clone()
        out_dim = out_dim * self.factor
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "A {} factor {}".format(
            self.algo,
            self.factor
        )

class ReshapeParameters(Parameters, SingleInputAndOutput):

    op_name = "reshape"


    def __init__(self, name, shape=None, transpose=None):
        super(ReshapeParameters, self).__init__(name)
        if not isinstance(shape, Dim):
            shape = Dim.unnamed(shape)
        self.shape = shape
        self.transpose = transpose

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]
        assert in_dims.size() == self.shape.size()
        if self.transpose:
            in_dims.transpose(self.transpose)
        return [self.shape.clone()]

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "SHAPE {}".format(
            self.shape
        )

class YoloParameters(NoSizeChangeParameters, SingleInputAndOutput):

    op_name = "yolo"

    def __init__(self, name, classes, total, mask, max_boxes):
        super(YoloParameters, self).__init__(name)
        self.classes = classes
        self.total = total
        self.mask = mask
        self.max_boxes = max_boxes

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "C {} T {} MSK {} MAX {}".format(
            self.classes, self.total, self.mask, self.max_boxes
        )

class MatrixAddParameters(NoSizeChangeParameters):

    op_name = "add"

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return ""

class SoftMaxParameters(NoSizeChangeParameters, SingleInputAndOutput):

    op_name = "softmax"

    def __init__(self, name, beta):
        super(SoftMaxParameters, self).__init__(name)
        self.beta = beta

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "BETA {}".format(
            self.beta
        )

# pylint: disable=abstract-method
class UnexecutableOpParameters(Parameters):
    pass

class UnconvertedOpParameters(UnexecutableOpParameters):

    def __init__(self, name, indicated_op_name, expected_inputs, indicated_outputs, info):
        super(UnconvertedOpParameters, self).__init__(name)
        self.info = info
        self.expected_inputs = expected_inputs
        self.indicated_outputs = indicated_outputs
        self.indicated_op_name = indicated_op_name

    @property
    def op_name(self):
        return self.indicated_op_name

    def get_output_size(self, in_dims):
        if self.indicated_outputs:
            return [Dim.unnamed(out) for out in self.indicated_outputs]
        if len(in_dims) == 1:
            return [in_dims[0]]
        return [Dim.unknown()]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return self.indicated_op_name

class UnknownOpParameters(UnexecutableOpParameters):

    def __init__(self, name, info):
        super(UnknownOpParameters, self).__init__(name)
        self.info = info

    def get_output_size(self, in_dims):
        if len(in_dims) == 1:
            return [in_dims[0]]
        return [Dim.unknown()]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "Unknown"
