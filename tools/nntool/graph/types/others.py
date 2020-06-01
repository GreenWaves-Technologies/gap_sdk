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
import sys

import numpy as np

from graph.dim import Dim
from utils.formatters import FORMAT_CHANGES, NORMALIZATIONS

from .base import (NoSizeChangeParameters, Parameters,
                   SameNumberOfDimensionsForInputs, SensitiveToOrder,
                   SingleInputAndOutput, Transposable)

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


class InputBaseParameters(InputOutputParameters):

    @property
    def in_dims(self):
        dim = self.dims.clone()
        if self.in_dims_hint:
            dim.apply_naming_hints(self.in_dims_hint[0])
        return [dim]

    @in_dims.setter
    def in_dims(self, val):
        pass

    def __str__(self):
        return "I {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        if self.transpose_out:
            out_dim.transpose(self.transpose_out)
        if self.out_dims_hint:
            out_dim.apply_naming_hints(self.out_dims_hint[0])
        return [out_dim]


class InputParameters(InputBaseParameters):
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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    # @property
    # def can_promoteq(self):
    #     return self.out_q.bits < STATS_BITS[-1]

    # def promoteq(self):
    #     if self.out_q.bits == STATS_BITS[-1]:
    #         raise ValueError("can't step further")
    #     self.out_q = get_quantization(self.activation_stats, None, self.out_q.bits * 2)
    #     return True

class ImageFormatParameters(Parameters, SingleInputAndOutput, SensitiveToOrder):
    op_name = "image_format"
    NORMALIZATIONS = NORMALIZATIONS
    FORMAT_CHANGES = FORMAT_CHANGES

    def __init__(self, *args, norm_func=None, format_change=None, **kwargs):
        self._norm_func = None
        self._format_change = None
        super(ImageFormatParameters, self).__init__(*args, **kwargs)
        self.norm_func = norm_func
        self.format_change = format_change

    @property
    def input_channels(self):
        if self.format_change in ("RGB565_RGB888", "BW8", "BW16"):
            return 1
        if self.format_change in ("RGB888", "RGB16"):
            return 3
        return None

    @property
    def input_dtype(self):
        if self.format_change == "RGB565_RGB888":
            return np.uint16
        if self.format_change in ("RGB888", "BW8", "BW16", "RGB16"):
            return np.uint8
        return None

    @property
    def output_channels(self):
        if self.format_change in ("RGB565_RGB888", "RGB888", "RGB16"):
            return 3
        if self.format_change in ("BW8", "BW16"):
            return 1
        return None

    @property
    def output_dtype(self):
        if self.norm_func in ("SHIFT_INT8", "OFFSET_INT8"):
            return np.int8
        if self.norm_func in "OUT_INT16":
            return np.int16
        return None

    @property
    def format_change(self):
        # RGB565_RGB888
        return self._format_change

    @format_change.setter
    def format_change(self, val):
        val = val and val.upper()
        if val is not None and val not in self.FORMAT_CHANGES:
            raise ValueError("format change is not valid")
        self._format_change = val

    @property
    def norm_func(self):
        # None, "shift", "offset"
        return self._norm_func

    @norm_func.setter
    def norm_func(self, val):
        val = val and val.upper()
        if val is not None and val not in self.NORMALIZATIONS:
            raise ValueError("normalization is not valid")
        self._norm_func = val

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = self.clone_dim_with_hints(in_dims, hint_dir='in')
        out_dim = self.clone_dim_with_hints(in_dims, hint_dir='out')[0]
        if self.format_change == "RGB565_RGB888":
            assert out_dim.is_named and out_dim.c == 1
            out_dim.impose_order(self.out_dims_hint[0])
            out_dim.c = 3
        elif self.format_change in ("BW8", "BW16"):
            assert out_dim.is_named and out_dim.c == 1
            out_dim.impose_order(self.out_dims_hint[0])
        elif self.format_change in ("RGB888", "RGB16"):
            assert out_dim.is_named and out_dim.c == 3
            out_dim.impose_order(self.out_dims_hint[0])
        else:
            raise ValueError("unknow format change")

        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "FORMAT_CHANGE Fmt: {} Norm: {}".format(self.format_change, self.norm_func)

class ConstantInputParameters(InputBaseParameters):
    op_name = "constant"

    def __init__(self, *args, **kwargs):
        self.value = None
        super(ConstantInputParameters, self).__init__(*args, **kwargs)

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def get_parameter_size(self):
        return self.dims.size()

    def get_parameters(self):
        return {'value': self.value}

    def set_parameters(self, val):
        self.value = val['value']

    def __str__(self):
        return "Const {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )


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


class TransposeParameters(Transposable, SingleInputAndOutput):
    op_name = "transpose"

    def __init__(self, *args, transpose=None, **kwargs):
        super(TransposeParameters, self).__init__(*args, **kwargs)
        self.transpose_in = transpose

    def get_parameter_size(self):
        return 0

    def permute(self, val):
        return [val[i] for i in self.transpose_in]

    @property
    def can_equalize(self):
        return False

    def real_shape(self):
        input_shape = self.in_dims[0].shape
        cond_input_idx = [i for i, sz in enumerate(self.in_dims[0].shape) if sz != 1]
        real_transpose = [i for i in self.transpose_in if i in cond_input_idx]
        cond_input_shape = [input_shape[i] for i in cond_input_idx]
        cond_transpose = [cond_input_idx.index(i) for i in real_transpose]
        return tuple(cond_input_shape), tuple(cond_transpose)

    @property
    def transpose_dimension(self):
        if self._transpose_in is None:
            return 1
        return len(self.transpose_in)

    @property
    def transpose_out(self):
        return self._transpose_in

    @transpose_out.setter
    def transpose_out(self, val):
        self._transpose_in = val

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
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

    def __init__(self, *args, axis=None, axis_hint=None, **kwargs):
        super(ConcatParameters, self).__init__(*args, **kwargs)
        self._axis = axis
        self._axis_hint = axis_hint

    @property
    def axis(self):
        return self._axis

    @axis.setter
    def axis(self, val):
        self._axis = val

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        if in_dims[0].is_named and self._axis_hint:
            self._axis = in_dims[0].get_order_idx(self._axis_hint)
        self.in_dims = self.clone_dim_with_hints(in_dims)
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
        self.in_dims = self.clone_dim_with_hints(in_dims)
        in_dims = self.in_dims[0]
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
        self.pad_type = "zero"

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


class GlobalPoolParameters(Parameters, SingleInputAndOutput, SensitiveToOrder):
    op_name = "global"

    def __init__(self, name, pool_type="average", in_dims_hint=None, out_dims_hint=None):

        super(GlobalPoolParameters, self).__init__(name,
                                                   in_dims_hint=in_dims_hint,
                                                   out_dims_hint=out_dims_hint)
        self.pool_type = pool_type

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = self.clone_dim_with_hints(in_dims)
        out_dim = self.in_dims[0].clone()
        out_dim.w = 1
        out_dim.h = 1
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "GLOBAL {}".format(self.pool_type)


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
        self.in_dims = self.clone_dim_with_hints(in_dims)
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
        return self.shape.layout_shape == self.old_shape.layout_shape

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = self.clone_dim_with_hints(in_dims)
        in_dim = in_dims[0]
        self._old_shape = in_dim
        assert in_dim.size() == self.shape.size(), "in shape does not match in size"
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


class MatrixBroadcastedLinearOpParameters(Parameters, SameNumberOfDimensionsForInputs):
    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
        max_idx, _ = max(enumerate(self.in_dims), key=lambda x: x[1].size())
        return [self.in_dims[max_idx]]

    def __str__(self):
        return "{} {}".format(self.op_name, self.at_options)


class MatrixAddParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "add"


class MatrixMulParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "mul"


class MatrixSubParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "sub"


class MatrixDivParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "div"


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


class NoOPParameters(NoSizeChangeParameters, SingleInputAndOutput):
    op_name = "noop"

    def __init__(self, name, desc=""):
        super(NoOPParameters, self).__init__(name)
        self._desc = desc

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return 0

    def __str__(self):
        return "NOOP {}".format(
            self._desc
        )


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
        self.in_dims = self.clone_dim_with_hints(in_dims)
        if len(self.in_dims) == 1:
            return [self.in_dims[0]]
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
        self.in_dims = self.clone_dim_with_hints(in_dims)
        if len(self.in_dims) == 1:
            return [self.in_dims[0]]
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
