# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging
import sys
from functools import reduce

from generation.kernel_parameters import GenCtrl

from ..dim import Dim
from .base import (FilterParameters, NoSizeChangeParameters, Parameters,
                   SensitiveToOrder, SingleInputAndOutput, Transposable, NodeOptions)

LOG = logging.getLogger("nntool." + __name__)

class InputOutputParameters(Transposable):

    def __init__(self, *args, dims=None, fixed_order=False, **kwargs):
        super().__init__(*args, **kwargs)
        self._output_value = None
        self._index = None
        self.dims = dims
        self.fixed_order = fixed_order
        self.at_options.valid_options['ALLOCATE'] = int
        self.at_options.valid_options['FIXED_ORDER'] = int
        self.at_options.fixed_order = 0

    @property
    def fixed_order(self):
        return self.at_options.fixed_order == 1

    @fixed_order.setter
    def fixed_order(self, val):
        self.at_options.fixed_order = 1 if val else 0

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

    def clone(self, name, groupn=None):
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
        return "I {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )

    @property
    def in_dims(self):
        dim = self.dims.clone()
        if self.in_dims_hint:
            dim.apply_naming_hints(self.in_dims_hint[0])
        return [dim]

    @in_dims.setter
    def in_dims(self, val):
        pass

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        if self.transpose_out:
            out_dim.transpose(self.transpose_out)
        if self.out_dims_hint:
            out_dim.apply_naming_hints(self.out_dims_hint[0])
        return [out_dim]

    def clone(self, name, groupn=None):
        raise NotImplementedError()

class OutputParameters(InputOutputParameters):
    op_name = "output"

    def __init__(self, *args, **kwargs):
        super(OutputParameters, self).__init__(*args, **kwargs)

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        if self.transpose_in:
            out_dim.transpose(self.transpose_in)
        return [out_dim]

    @property
    def out_dims(self):
        return [self.dims]

    @out_dims.setter
    def out_dims(self, val):
        self.dims = val[0]

    def __str__(self):
        return "O {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )

    def clone(self, name, groupn=None):
        raise NotImplementedError()

class ActivationParameters(NoSizeChangeParameters, SingleInputAndOutput):

    def __init__(self, name, activation="relu", activation_params=None):
        super(ActivationParameters, self).__init__(name)
        self.activation = activation
        self.activation_params = activation_params

    @property
    def op_name(self):
        return self.activation

    def get_parameter_size(self):
        return 0

    def clone(self, name, groupn=None):
        return ActivationParameters(name, self.activation, self.activation_params)

    @property
    def can_equalize(self):
        # Is leaky usable? Looks like it.
        return self.activation == "relu" or self.activation == "leaky"\
            or self.activation == "relu6" or self.activation == "relun"

    def compute_load(self):
        # TODO - Be more accurate with different activation types
        return self.out_dims[0].size()

    def __str__(self):
        return "Activation {} {}".format(
            self.activation,
            self.at_options
        )

class TransposeParameters(Transposable, SingleInputAndOutput):
    op_name = "transpose"

    def __init__(self, *args, transpose=None, **kwargs):
        super(TransposeParameters, self).__init__(*args, **kwargs)
        self.transpose_in = transpose

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def transpose_elements(self):
        tin = self.transpose_in
        elems = []
        cur = []
        for i in tin:
            if len(cur) == 0 or cur[-1] + 1 == i:
                cur.append(i)
            else:
                elems.append(cur)
                cur = [i]
        if len(cur) > 0:
            elems.append(cur)
        return elems

    @property
    def transpose_size(self):
        dim = self.in_dims[0].shape
        return [reduce(lambda x, y: x * dim[y], telem, 1) for telem in self.transpose_elements()]

    @property
    def transpose_dimension(self):
        if self._transpose_in is None:
            return 1
        return len(self.transpose_elements())

    @property
    def transpose_out(self):
        return self._transpose_in

    @transpose_out.setter
    def transpose_out(self, val):
        self._transpose_in = val

    def get_output_size(self, in_dims):
        self.in_dims = in_dims
        out_dim = in_dims[0].clone()
        if self.transpose_in:
            out_dim = out_dim.transpose(self.transpose_in)
        return [out_dim]

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "T {} {}".format(
            self.transpose_in and ','.join([str(i) for i in self.transpose_in]) or "None",
            self.at_options
        )

class ConcatParameters(Transposable):
    op_name = "concat"

    def __init__(self, *args, axis=None, **kwargs):
        super(ConcatParameters, self).__init__(*args, **kwargs)
        self.axis = axis

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        self.in_dims = in_dims
        if self.transpose_in:
            in_dims = [in_dim.clone().transpose(self.transpose_in) for in_dim in in_dims]
        out_dim = Dim.combine([in_dim for in_dim in in_dims], self.axis)
        if self.transpose_out:
            out_dim.transpose(self.transpose_out)
        return [out_dim]

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "A {} {} {}".format(
            self.axis,
            Transposable.__str__(self),
            self.at_options
        )

class FusionParameters(Parameters, SingleInputAndOutput):
    '''Fusion of operators. At present restricted to single input and output but
    this could be removed perhaps'''

    op_name = "fusion"

    def __init__(self, name, fusion_type, subgraph):
        super(FusionParameters, self).__init__(name)
        self._subgraph = subgraph
        nodes = self.contained_nodes()
        self.in_dims_hint = nodes[0].in_dims_hint
        self.out_dims_hint = nodes[-1].out_dims_hint
        self.fusion_type = fusion_type

    def _init_at_options(self):
        if self._at_options is None:
            self._at_options = NodeOptions(None)
        self._at_options.extend(*[node.at_options for node in self.contained_nodes()])

    @property
    def at_options(self):
        self._init_at_options()
        return self._at_options

    @at_options.setter
    def gen_ctrl(self, val):
        self._init_at_options()
        self._at_options = val

    def contained_filters(self):
        return [x for x in self.contained_nodes() if isinstance(x, FilterParameters)]

    def get_parameter_size(self):
        return sum([node.get_parameter_size() for node in self.contained_nodes()])

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

    # # Needs to be refactored out
    # @property
    # def params(self):
    #     return self._nodes

    @property
    def subgraph(self):
        return self._subgraph

    def contained_nodes(self):
        return [node for node in self.subgraph.dfs()]

    def get_contained_node(self, name):
        return next((n for n in self.contained_nodes() if n.name == name), None)

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        out_dims = in_dims

        for node in self.contained_nodes():
            out_dims = node.get_output_size(out_dims)

        return out_dims

    @property
    def can_equalize(self):
        return all([param.can_equalize for param in self.contained_nodes()])

    def clone(self, name, groupn=None):
        return FusionParameters(name, self.fusion_type, self._subgraph)

    def compute_load(self):
        return sum([load if load else 0 for load in [node.compute_load()\
            for node in self.contained_nodes()]])

    def __str__(self):
        return "{} {}".format(",".join([str(node) for node in self.contained_nodes()]), self.gen_ctrl or "")

class GroupParameters(Parameters, SensitiveToOrder):

    op_name = "group"

    def __init__(self, name, groups, in_dims_hint=None, out_dims_hint=None):

        super(GroupParameters, self).__init__(name,
                                              in_dims_hint=in_dims_hint,
                                              out_dims_hint=out_dims_hint)
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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "GRPS {}".format(
            self.groups
        )

class PadParameters(Parameters, SingleInputAndOutput, SensitiveToOrder):
    op_name = "pad"

    def __init__(self, name, padding, in_dims_hint=None, out_dims_hint=None):

        super(PadParameters, self).__init__(name,
                                            in_dims_hint=in_dims_hint,
                                            out_dims_hint=out_dims_hint)
        self.padding = padding

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = self.clone_dim_with_hints(in_dims)
        out_dim = self.in_dims[0].clone()
        out_dim.w += self.padding.w
        out_dim.h += self.padding.h
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "PAD {}".format(self.padding)

class UpsampleParameters(Parameters, SingleInputAndOutput, SensitiveToOrder):

    op_name = "upsample"

    def __init__(self, name, algo, factor, in_dims_hint=None, out_dims_hint=None):

        super(UpsampleParameters, self).__init__(name,
                                                 in_dims_hint=in_dims_hint,
                                                 out_dims_hint=out_dims_hint)
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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "A {} factor {}".format(
            self.algo,
            self.factor
        )

class ReshapeParameters(Transposable, SingleInputAndOutput):
    '''This class covers reshapes and transposes'''

    op_name = "reshape"

    def __init__(self, *args, old_shape=None, shape=None, **kwargs):
        super(ReshapeParameters, self).__init__(*args, **kwargs)
        if not isinstance(shape, Dim):
            shape = Dim.unnamed(shape)
        self._shape = shape
        self._old_shape = old_shape

    def does_nothing(self):
        return self.shape.shape == list(filter(lambda x: x != 1, self.old_shape.shape))

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]
        assert in_dims.size() == self.shape.size()
        out = self.shape.clone()
        if self.transpose_out:
            out.transpose(self.transpose_out)
        return [out]

    @property
    def shape(self):
        return self._shape

    @shape.setter
    def shape(self, val):
        self._shape = val

    @property
    def old_shape(self):
        return self._old_shape

    @old_shape.setter
    def old_shape(self, val):
        self._old_shape = val

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "SHAPE {} {}".format(
            self.shape,
            Transposable.__str__(self)
        )

class YoloParameters(NoSizeChangeParameters, SingleInputAndOutput, SensitiveToOrder):

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

    def clone(self, name, groupn=None):
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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def __str__(self):
        return "{}".format(self.at_options)

class SoftMaxParameters(NoSizeChangeParameters, SingleInputAndOutput, SensitiveToOrder):

    op_name = "softmax"

    def __init__(self, name, beta):
        super(SoftMaxParameters, self).__init__(name)
        self.beta = beta

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def __str__(self):
        return "BETA {} {}".format(
            self.beta,
            self.at_options
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
            return self.indicated_outputs
        if len(in_dims) == 1:
            return [in_dims[0]]
        return [Dim.unknown()]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, name, groupn=None):
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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "Unknown"
