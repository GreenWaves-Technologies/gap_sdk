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
from expressions.symbolic.basic import (Abs, Cos, Exp, Log, Max, Min, Neg, Pow,
                                        Sin, Sqrt)
from graph.dim import Dim

from .base import (CanFuseToExpression, ComparableParameters,
                   InsensitiveToQuantization, NoSizeChangeParameters,
                   Parameters, SensitiveToOrder, SingleInputAndOutput,
                   Transposable, cls_op_name, expression_op)

LOG = logging.getLogger("nntool." + __name__)


@cls_op_name('transpose')
class TransposeParameters(Transposable, SingleInputAndOutput, InsensitiveToQuantization, ComparableParameters):

    def __init__(self, *args, transpose=None, block_search_up=False, block_search_down=False, **kwargs):
        super(TransposeParameters, self).__init__(*args, **kwargs)
        self.transpose_in = [transpose]
        self.block_search_up = block_search_up
        self.block_search_down = block_search_down

    @property
    def graph_anon_label(self):
        return ['Transpose']

    def get_parameter_size(self):
        return 0

    def permute(self, val):
        return [val[i] for i in self.transpose_in[0]]

    def does_nothing(self):
        if not self.transpose_in or not self.transpose_in[0]:
            return True
        if not self.in_dims or not self.in_dims[0]:
            return False
        shape = self.in_dims[0].shape
        trans = self.transpose_in[0]
        shape_idx = [idx if dim > 1 else None for idx, dim in enumerate(shape)]
        shape_trans = [shape_idx[idx]
                       for idx in trans if shape_idx[idx] is not None]
        return shape_trans == sorted(shape_trans)

    def is_same_operation_as(self, other):
        if self.transpose_in is None:
            return other.transpose_in is None
        if other.transpose_in is None:
            return self.transpose_in is None
        return tuple(self.transpose_in) == tuple(other.transpose_in)

    @property
    def can_equalize(self):
        return False

    def real_shape(self):
        input_shape = self.in_dims[0].shape
        cond_input_idx = [i for i, sz in enumerate(
            self.in_dims[0].shape) if sz != 1]
        real_transpose = [
            i for i in self.transpose_in[0] if i in cond_input_idx]
        cond_input_shape = [input_shape[i] for i in cond_input_idx]
        cond_transpose = [cond_input_idx.index(i) for i in real_transpose]
        return tuple(cond_input_shape), tuple(cond_transpose)

    @property
    def transpose_dimension(self):
        if self._transpose_in is None:
            return 1
        return len(self.transpose_in[0])

    @property
    def transpose_out(self):
        return None

    @transpose_out.setter
    def transpose_out(self, val):
        self._transpose_in = val

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        if self.transpose_in:
            out_dim = out_dim.transpose(self.transpose_in[0])
        return [out_dim]

    def __str__(self):
        return "T {} {}".format(
            self.transpose_in and ','.join(
                [str(i) for i in self.transpose_in[0]]) or "None",
            self.at_options
        )


@cls_op_name('copy')
class CopyParameters(Parameters, InsensitiveToQuantization):

    def __init__(self, *args, **kwargs):
        super(CopyParameters, self).__init__(*args, **kwargs)

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def __str__(self):
        return ""


@cls_op_name('quantize')
class QuantizeParameters(Parameters):

    def __init__(self, *args, from_qtype=None, to_qtype=None,
                 inserted_by_quantizer=False, **kwargs):
        super(QuantizeParameters, self).__init__(*args, **kwargs)

        self.from_qtype = from_qtype
        self.to_qtype = to_qtype
        self.inserted_by_quantizer = inserted_by_quantizer

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def __str__(self):
        return f"{self.from_qtype} --> {self.to_qtype}"


@cls_op_name('reverse')
class ReverseParameters(Parameters, InsensitiveToQuantization):

    def __init__(self, *args, axis=0, **kwargs):
        super(ReverseParameters, self).__init__(*args, **kwargs)
        self.axis = axis

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def __str__(self):
        return "A {}".format(self.axis)


@cls_op_name('concat')
class ConcatParameters(Transposable):

    def __init__(self, *args, axis=None, axis_hint=None, **kwargs):
        super(ConcatParameters, self).__init__(*args, **kwargs)
        self._axis = axis
        self._axis_hint = axis_hint

    @property
    def graph_label(self):
        return [self.name, f'Axis {self.axis}']

    @property
    def graph_anon_label(self):
        return ['Concat', f'Axis {self.axis}']

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
        if self.transpose_in:
            in_dims = [(in_dim.clone() if self.transpose_in[idx] is None
                        else in_dim.clone().transpose(self.transpose_in[idx]))
                       for idx, in_dim in enumerate(in_dims)]
        if in_dims[0].is_named and self._axis_hint:
            self._axis = in_dims[0].get_order_idx(self._axis_hint)
        out_dim = Dim.combine([in_dim for in_dim in in_dims], self.axis)
        if self.transpose_out:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def __str__(self):
        return "A {} {} {}".format(
            self.axis,
            Transposable.__str__(self),
            self.at_options
        )


@cls_op_name('split')
class SplitParameters(Transposable):

    def __init__(self, *args,
                 act_slices=None,
                 out_shapes=None,
                 axis=None,
                 **kwargs):

        super(SplitParameters, self).__init__(*args, **kwargs)
        self.act_slices = act_slices
        self.out_shapes = out_shapes
        self.axis = axis

    @property
    def graph_label(self):
        return [self.name, f'Axis {self.axis}']

    @property
    def graph_anon_label(self):
        return ['Split', f'Axis {self.axis}']

    def numpy_split(self, arr: np.ndarray):
        slice_specs = [tuple([slice(elem[0], elem[1], elem[2])
                              for elem in act_slice])
                       for act_slice in self.act_slices]
        return [arr[spec] for spec in slice_specs]

    @staticmethod
    def get_splits(in_shape, axis, splits=None, num_splits=None):
        assert splits or num_splits, "no split parameters provided"
        assert in_shape[axis] is not None, "split on undefined axis"
        in_idx = 0
        act_slices = []
        out_shapes = []
        if splits:
            for sz in splits:
                act_slices.append([(in_idx, in_idx + sz, 1) if idx == axis else (0, shape, 1)
                                   for idx, shape in enumerate(in_shape)
                                   if shape is not None])
                out_shapes.append([sz if shape is not None and idx == axis else shape
                                   for idx, shape in enumerate(in_shape)])
                in_idx += sz
        elif num_splits:
            assert in_shape[axis] % num_splits == 0, "dimension of split is not divisible by number of splits"
            sz = in_shape[axis] // num_splits
            while in_idx < in_shape[axis]:
                act_slices.append([(in_idx, in_idx + sz, 1) if idx == axis else (0, shape, 1)
                                   for idx, shape in enumerate(in_shape)
                                   if shape is not None])
                out_shapes.append([sz if shape is not None and idx == axis else shape
                                   for idx, shape in enumerate(in_shape)])
                in_idx += sz
        count_nones = sum(1 if dim is None else 0 for dim in in_shape[:axis:])
        axis -= count_nones
        return act_slices, out_shapes, axis

    @property
    def num_splits(self):
        return len(self.act_slices)

    def transpose_params(self, order):
        self.act_slices = [
            [act_slice[idx] for idx in order] for act_slice in self.act_slices
        ]
        self.out_shapes = [
            [shape[idx] for idx in order] for shape in self.out_shapes
        ]

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        out_size = [Dim.unnamed(shape) for shape in self.out_shapes]
        if self.transpose_out:
            out_size = [dim if self.transpose_out[idx] is None else dim.transpose(self.transpose_out[idx])
                        for idx, dim in enumerate(out_size)]
        return out_size

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "A {} {} {}".format(
            self.axis,
            Transposable.__str__(self),
            self.at_options
        )


@cls_op_name('gather')
class GatherParameters(Parameters, SingleInputAndOutput, SensitiveToOrder, InsensitiveToQuantization):

    def __init__(self, *args,
                 axis=None,
                 indices=None,
                 **kwargs):

        super(GatherParameters, self).__init__(*args, **kwargs)
        self.axis = axis
        self.indices = np.array(indices)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        in_dim = in_dims[0]
        new_shape = in_dim.shape[:self.axis:] + \
            list(self.indices.shape) + in_dim.shape[self.axis + 1:]
        return [Dim.unnamed(new_shape)]

    @property
    def rank(self):
        return len(self.in_dims[0].shape) + len(self.indices.shape) - 1

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "A %s I %s" % (self.axis, self.indices)

@cls_op_name('strided_slice')
class StridedSliceParameters(Parameters, SingleInputAndOutput, ComparableParameters, InsensitiveToQuantization):

    def __init__(self, *args,
                 act_slice=None,
                 out_shape=None,
                 **kwargs):

        super(StridedSliceParameters, self).__init__(*args, **kwargs)
        self.act_slice = act_slice
        self.out_shape = out_shape

    @property
    def graph_label(self):
        return [self.name] + ["(%s,%s,%s)" % elem for elem in self.act_slice]

    @property
    def graph_anon_label(self):
        return ['Slice'] + ["(%s,%s,%s)" % elem for elem in self.act_slice]

    def numpy_slice(self, arr: np.ndarray):
        slice_spec = [slice(elem[0], elem[1], elem[2])
                      for elem in self.act_slice if len(elem) == 3]
        return arr[tuple(slice_spec)].reshape(self.out_shape)

    def only_slices_axis(self, axis):
        """check if there is a slice on only one axis"""
        in_shape = self.in_dims[0].shape
        return all(sl[0] == 0 and sl[1] == in_shape[idx] and sl[2] == 1
                   for idx, sl in enumerate(self.act_slice) if idx != axis)

    def is_unit_slice(self, axis):
        """check if the slice on one axis returns shape of 1"""
        slce = self.act_slice[axis]
        if slce[1] > slce[0]:
            return slce[1] - slce[0] == 1 and slce[2] == 1
        else:
            return slce[0] - slce[1] == 2 and slce[2] == -1

    def is_same_operation_as(self, other):
        if not isinstance(other, StridedSliceParameters):
            return False
        if tuple(self.out_shape) != tuple(other.out_shape):
            return False
        if len(self.act_slice) != len(other.act_slice):
            return False
        return all(tuple(elem) == tuple(oelem) for elem, oelem in zip(self.act_slice, other.act_slice))

    def only_slices(self, axis):
        return all(dim == self.act_slice[idx][1] and self.act_slice[idx][0] == 0 and self.act_slice[idx][2] == 1
                   for idx, dim in enumerate(self.in_dims[0].shape) if axis != idx)

    @property
    def post_slice_shape(self):
        return [(sl[1] - sl[0])//sl[2] for sl in self.act_slice]

    @property
    def changes_shape(self):
        return len(self.post_slice_shape) > len(self.out_shape)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        return [Dim.unnamed(self.out_shape)]

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return ",".join("(%s,%s,%s)" % elem for elem in self.act_slice)


@cls_op_name('pad')
class PadParameters(Parameters, SingleInputAndOutput):

    def __init__(self, name, padding=None, pad_vals=None, in_dims_hint=None, out_dims_hint=None):

        super(PadParameters, self).__init__(name,
                                            in_dims_hint=in_dims_hint,
                                            out_dims_hint=out_dims_hint)
        self.padding = padding
        self.pad_vals = pad_vals

    @property
    def graph_label(self):
        return [self.name, f'Pad {self.padding}']

    @property
    def graph_anon_label(self):
        return ['Pad', f'{self.padding}']

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        out_dim = in_dims[0].clone()
        for idx, vals in enumerate(self.padding):
            out_dim[idx] += sum(vals)
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def __str__(self):
        return "PAD {}".format(self.padding)


class BinaryOpParameters(CanFuseToExpression, Parameters):

    def __new__(cls, *args, op_type="maximum", **kwargs):
        if cls is BinaryOpParameters:
            for subcls in BinaryOpParameters.__subclasses__():
                if op_type == subcls.CLS_OP_NAME:
                    return super(BinaryOpParameters, cls).__new__(subcls)
            raise ValueError(f'binary op {op_type} not found')

    def __init__(self, *args, op_type="maximum", **kwargs):
        super(BinaryOpParameters, self).__init__(*args, **kwargs)
        self._op_type = op_type

    @property
    def op_type(self):
        return self._op_type

    def get_output_size(self, in_dims):
        assert len(in_dims) == 2
        out_dim = in_dims[0].clone()
        return [out_dim]

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "{} {}".format(
            self._op_type,
            self.at_options
        )


@cls_op_name('maximum')
@expression_op(Max)
class MaxOpParameters(BinaryOpParameters, InsensitiveToQuantization):
    pass


@cls_op_name('minimum')
@expression_op(Min)
class MinOpParameters(BinaryOpParameters, InsensitiveToQuantization):
    pass


@cls_op_name('pow')
@expression_op(Pow)
class PowOpParameters(BinaryOpParameters):
    pass


class UnaryOpParameters(CanFuseToExpression, Parameters):

    def __new__(cls, *args, op_type="sqrt", **kwargs):
        if cls == UnaryOpParameters:
            for subcls in UnaryOpParameters.__subclasses__():
                if op_type == subcls.CLS_OP_NAME:
                    return super(UnaryOpParameters, cls).__new__(subcls)
            raise ValueError(f'unary op {op_type} not found')
        return super(UnaryOpParameters, cls).__new__(cls)

    def __init__(self, *args, op_type=None, **kwargs):
        super(UnaryOpParameters, self).__init__(*args, **kwargs)
        self._op_type = op_type

    @property
    def op_type(self):
        return self._op_type

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        out_dim = in_dims[0].clone()
        return [out_dim]

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "{} {}".format(
            self._op_type,
            self.at_options
        )


@cls_op_name('sqrt')
@expression_op(Sqrt)
class SqrtOpParameters(UnaryOpParameters):
    pass


@cls_op_name('exp')
@expression_op(Exp)
class ExpOpParameters(UnaryOpParameters):
    pass


@cls_op_name('log')
@expression_op(Log)
class LogOpParameters(UnaryOpParameters):
    pass


@cls_op_name('sin')
@expression_op(Sin)
class SinOpParameters(UnaryOpParameters):
    pass


@cls_op_name('cos')
@expression_op(Cos)
class CosOpParameters(UnaryOpParameters):
    pass


@cls_op_name('abs')
@expression_op(Abs)
class AbsOpParameters(UnaryOpParameters, InsensitiveToQuantization):
    pass


@cls_op_name('neg')
@expression_op(Neg)
class NegOpParameters(UnaryOpParameters, InsensitiveToQuantization):
    pass


# @cls_op_name('global')
# class GlobalPoolingParameters(Transposable, SingleInputAndOutput):

#     def __init__(self, *args, pool_type="max", axis=None, keep_dims=None, **kwargs):
#         super(GlobalPoolingParameters, self).__init__(*args, **kwargs)
#         self._pool_type = pool_type
#         self._axis = axis
#         self._keep_dims = keep_dims

#     @property
#     def axis(self):
#         return self._axis

#     @property
#     def keep_dims(self):
#         return self._keep_dims

#     @property
#     def pool_type(self):
#         return self._pool_type

#     @keep_dims.setter
#     def keep_dims(self, val):
#         self._keep_dims = val

#     @axis.setter
#     def axis(self, val):
#         self._axis = val

#     def get_parameter_size(self):
#         return 0

#     def get_output_size(self, in_dims):
#         out_dim = in_dims[0].clone()
#         if self.transpose_in:
#             out_dim.transpose(self.transpose_in[0])

#         if self.keep_dims:
#             names = out_dim.keys if out_dim.is_named else None
#             out_dim = Dim(shape=[1 if idx in self._axis else dim
#                                  for idx, dim in enumerate(out_dim.shape)],
#                           names=names, is_ordered=True)
#             if self.transpose_out:
#                 out_dim.transpose(self.transpose_out[0])
#         else:
#             out_dim = Dim(shape=[dim for idx, dim in enumerate(out_dim.shape)
#                                  if idx not in self._axis],
#                           is_ordered=True)

#         return [out_dim]

#     @property
#     def can_equalize(self):
#         return False

#     def __str__(self):
#         return "{} A {}{} {} {}".format(
#             self._pool_type,
#             self._axis,
#             " keep_dims " if self._keep_dims else "",
#             Transposable.__str__(self),
#             self.at_options
#         )


@cls_op_name('reshape')
class ReshapeParameters(Transposable, SingleInputAndOutput, InsensitiveToQuantization, ComparableParameters):

    def __init__(self, *args, old_shape=None, shape=None, **kwargs):
        super(ReshapeParameters, self).__init__(
            *args, eliminate_transposes_pass_down=True, eliminate_transposes_pass_up=True, **kwargs)
        if not isinstance(shape, Dim):
            shape = Dim.unnamed(shape)
        self._shape = shape
        self._old_shape = old_shape

    @property
    def graph_label(self):
        return [self.name, f'{self.old_shape} to {self.shape}']

    @property
    def graph_anon_label(self):
        return ['Reshape', f'{self.old_shape} to {self.shape}']

    def does_nothing(self):
        return self.shape.layout_shape == self.old_shape.layout_shape

    def get_parameter_size(self):
        return 0

    def exp_red_pattern(self):
        """ If the reshape is an expand or reduce dim i.e. adds or removes 1 size axes then
        return a pattern with True indicating an added axis, False a removed axis and None
        an unchanged axis"""
        if not self.does_nothing():
            return None
        res = []
        s1 = self._old_shape.shape.copy()
        s2 = self._shape.shape.copy()
        while s1 and s2:
            if not s1:
                top = s2.pop(0)
                assert top == 1
                res.append(True)
            elif not s2:
                top = s1.pop(0)
                assert top == 1
                res.append(False)
            else:
                if s1[0] == s2[0]:
                    s1.pop(0)
                    s2.pop(0)
                    res.append(None)
                elif s1[0] == 1:
                    s1.pop(0)
                    res.append(False)
                elif s2[0] == 1:
                    s2.pop(0)
                    res.append(True)
                else:
                    raise ValueError('shape issue in exp_red_pattern')
        return res

    def is_same_operation_as(self, other):
        if not isinstance(other, ReshapeParameters):
            return False
        if tuple(self.old_shape.shape) != tuple(other.old_shape.shape):
            return False
        if tuple(self.shape.shape) != tuple(other.shape.shape):
            return False
        return True

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        in_dim = in_dims[0]
        self._old_shape = in_dim
        if in_dim.size() != self.shape.size():
            raise NotImplementedError("bad reshape %s: in dim %s does not match reshape %s" %
                                      (self.name, in_dim, self.shape))
        out = self.shape.clone()
        if self.transpose_out:
            out.transpose(self.transpose_out[0])
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

    def __str__(self):
        return "SHAPE {} {}".format(
            self.shape,
            Transposable.__str__(self)
        )


# pylint: disable=abstract-method

@cls_op_name('noop')
class NoOPParameters(NoSizeChangeParameters, SingleInputAndOutput, InsensitiveToQuantization):

    def __init__(self, name, desc=""):
        super(NoOPParameters, self).__init__(name)
        self._desc = desc

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        return 0

    def __str__(self):
        return "NOOP {}".format(
            self._desc
        )


class UnexecutableOpParameters(Parameters):
    pass


@cls_op_name('UNSUPPORTED')
class UnconvertedOpParameters(UnexecutableOpParameters):

    def __init__(self, name, indicated_op_name=None, expected_inputs=None,
                 indicated_outputs=None, info=None, **kwargs):
        super(UnconvertedOpParameters, self).__init__(name, **kwargs)
        self.info = info
        self.expected_inputs = expected_inputs
        self.indicated_outputs = indicated_outputs
        self.indicated_op_name = indicated_op_name

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

    def __str__(self):
        return "UNSUPPORTED OP: %s" % self.indicated_op_name


@cls_op_name('UNKNOWN')
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

    def __str__(self):
        return "Unknown"
