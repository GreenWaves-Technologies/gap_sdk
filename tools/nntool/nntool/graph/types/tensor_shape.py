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
from functools import reduce

import numpy as np

from nntool.graph.dim import Dim
from nntool.graph.manipulations.eliminate_transposes.transpose_helpers import \
    transpose_does_nothing, leading_real
from nntool.graph.types import (InsensitiveToQuantizationMixin, NNNodeBase,
                         SensitiveToOrderMixin, SingleInputAndOutputMixin)
from nntool.graph.types.base import ComparableNodeMixin, cls_op_name, not_generated, nargs
from nntool.utils.batch_to_space import batch_to_space_shape_nd, space_to_batch_shape_nd
from nntool.utils.real_transpose import real_transpose


@cls_op_name('transpose')
class TransposeNode(NNNodeBase, SingleInputAndOutputMixin, InsensitiveToQuantizationMixin, ComparableNodeMixin):

    def __init__(self, *args, transpose=None, **kwargs):
        super(TransposeNode, self).__init__(*args, **kwargs)
        try:
            self._transpose = tuple(transpose)
        except TypeError as ex:
            raise ValueError(f'{transpose} is not a valid transpose') from ex

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs['transpose'] = self.transpose
        return super_attrs

    @property
    def transpose(self):
        return self._transpose

    @transpose.setter
    def transpose(self, val):
        self._transpose = val

    @property
    def graph_anon_label(self):
        return ['Transpose', f'{self.transpose}']

    @property
    def graph_label(self):
        return [f'{self.CLS_OP_NAME}({self.name})', f'{self.transpose}']

    def get_parameter_size(self):
        return 0

    def permute(self, val):
        return [val[i] for i in self.transpose]

    @property
    def no_model_code(self):
        if not self.transpose:
            return True
        if not self.in_dims or not self.in_dims[0]:
            return False
        return transpose_does_nothing(self.transpose, self.in_dims[0].shape)

    @property
    def does_nothing(self) -> bool:
        return self._transpose is None

    def is_same_operation_as(self, G, other):
        if not isinstance(other, TransposeNode):
            return False
        if self.transpose is None:
            return other.transpose is None
        if other.transpose is None:
            return self.transpose is None
        return tuple(self.transpose) == tuple(other.transpose)

    @property
    def can_equalize(self):
        return False

    def real_shape(self):
        return real_transpose(self.in_dims[0].shape, self.transpose)

    @property
    def transpose_dimension(self):
        if self._transpose is None:
            return 1
        return len(self.transpose)

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        if self.transpose:
            out_dim = out_dim.transpose(self.transpose)
        return [out_dim]

    def __str__(self):
        return "T {} {}".format(
            self.transpose and ','.join(
                [str(i) for i in self.transpose]) or "None",
            self.at_options
        )


@cls_op_name('expand')
class ExpandNode(NNNodeBase, SensitiveToOrderMixin, InsensitiveToQuantizationMixin):
    def __init__(self, *args, shape=None, **kwargs):
        super(ExpandNode, self).__init__(*args, **kwargs)
        self.shape = shape

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "shape",
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        in_shape = list(in_dims[0].shape)
        exp_shape = list(self.shape)
        if len(in_shape) > len(exp_shape):
            exp_shape = in_shape[:(
                len(in_shape) - len(exp_shape)):] + exp_shape
        elif len(exp_shape) > len(in_shape):
            in_shape = exp_shape[:(len(exp_shape) - len(in_shape)):] + in_shape
        out_shape = []
        for exp_dim, in_dim in zip(exp_shape, in_shape):
            if in_dim == 1:
                out_shape.append(exp_dim)
            elif exp_dim == 1:
                out_shape.append(in_dim)
            elif in_dim != exp_dim:
                raise ValueError(
                    f'{self.name} invalid expand {in_dims[0]} {self.shape}')
            else:
                out_shape.append(in_dim)
        return [Dim.unnamed(out_shape)]

    def __str__(self):
        return f"{self.shape}"


@cls_op_name('reverse')
class ReverseNode(NNNodeBase, InsensitiveToQuantizationMixin):

    def __init__(self, *args, axis=0, **kwargs):
        super(ReverseNode, self).__init__(*args, **kwargs)
        self.axis = axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "axis",
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def __str__(self):
        return "A {}".format(self.axis)


@cls_op_name('reshape')
@not_generated
class ReshapeNode(NNNodeBase, SingleInputAndOutputMixin, InsensitiveToQuantizationMixin, ComparableNodeMixin):

    def __init__(self, *args, old_shape=None, shape=None, **kwargs):
        super(ReshapeNode, self).__init__(
            *args, **kwargs)
        if not isinstance(shape, Dim):
            shape = Dim.unnamed(shape)
        if old_shape is not None and not isinstance(old_shape, Dim):
            old_shape = Dim.unnamed(old_shape)
        assert shape.is_ordered and (old_shape is None or old_shape.is_ordered)
        self._shape = shape
        self._old_shape = old_shape

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "old_shape", "shape"
        ] if getattr(self, k)})
        return super_attrs

    @property
    def graph_label(self):
        return [f'Reshape({self.name})', f'{self.old_shape} to {self.shape}']

    @property
    def graph_anon_label(self):
        return ['Reshape', f'{self.old_shape} to {self.shape}']

    @property
    def does_nothing(self):
        return tuple(self.shape.shape) == tuple(self.old_shape.shape)

    @property
    def no_model_code(self) -> bool:
        return True

    def get_parameter_size(self):
        return 0

    def exp_red_pattern(self):
        """ If the reshape is an expand or reduce dim i.e. adds or removes 1 size axes then
        return a pattern with True indicating an added axis, False a removed axis and None
        an unchanged axis"""
        if not self.does_nothing:
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

    def is_same_operation_as(self, G, other):
        if not isinstance(other, ReshapeNode):
            return False
        if tuple(self.old_shape.shape) != tuple(other.old_shape.shape):
            return False
        if tuple(self.shape.shape) != tuple(other.shape.shape):
            return False
        return True

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        in_dim = in_dims[0]
        self.old_shape = in_dim
        if in_dim.size() != self.shape.size():
            raise NotImplementedError("bad reshape %s: in dim %s does not match reshape %s" %
                                      (self.name, in_dim, self.shape))
        out = self.shape.clone()
        return [out]

    @property
    def shape(self):
        return self._shape

    @shape.setter
    def shape(self, val):
        assert val.is_ordered
        self._shape = val

    @property
    def old_shape(self):
        return self._old_shape

    @old_shape.setter
    def old_shape(self, val):
        assert val.is_ordered
        self._old_shape = val

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return f"{self.old_shape}->{self.shape}"


@cls_op_name('scatternd')
class ScatterNdNode(NNNodeBase, SensitiveToOrderMixin):
    def __init__(self, *args, indices=None, updates=None, reduction=None, **kwargs):
        super(ScatterNdNode, self).__init__(*args, **kwargs)
        self.indices = indices
        self.updates = updates
        self.reduction = reduction

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "indices", "updates", "reduction"
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [Dim.unnamed(in_dims[0].shape)]

    def __str__(self):
        return ""

@cls_op_name('repeat')
class RepeatNode(NNNodeBase, SensitiveToOrderMixin):
    def __init__(self, *args, n_repeats=None, repeat_axis=None, **kwargs):
        super(RepeatNode, self).__init__(*args, **kwargs)
        self.n_repeats = n_repeats
        self.repeat_axis = repeat_axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "n_repeats", "repeat_axis"
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        new_shape = [s * self.n_repeats if i == self.repeat_axis else s for i, s in enumerate(in_dims[0].shape)]
        return [Dim.unnamed(new_shape)]

    def __str__(self):
        return f"A:{self.repeat_axis} N:{self.n_repeats}"

@cls_op_name('concat')
@nargs({'*'})
class ConcatNode(NNNodeBase):

    def __init__(self, *args, axis=None, **kwargs):
        super(ConcatNode, self).__init__(*args, **kwargs)
        if axis is None:
            raise ValueError("axis must be set")
        self._axis = axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs['axis'] = self.axis
        return super_attrs

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

    @property
    def does_nothing(self) -> bool:
        return self.in_dims and len(self.in_dims) == 1

    @property
    def no_model_code(self) -> bool:
        if not self.out_dims or not self.out_dims[0]:
            return False
        return self._axis in leading_real(self.out_dims[0].shape)

    @property
    def exclude_from_generation(self) -> bool:
        return self.does_nothing

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    @property
    def offsets(self):
        return reduce(
            lambda state, in_dim: (
                state[0] + [state[1]], state[1] + in_dim.shape[self.axis]),
            self.in_dims,
            ([], 0)
        )[0]

    def get_output_size(self, in_dims):
        out_dim = Dim.combine([in_dim for in_dim in in_dims], self.axis)
        return [out_dim]

    def __str__(self):
        return "A {} {}".format(
            self.axis,
            self.at_options
        )


@cls_op_name('split')
@not_generated
class SplitNode(NNNodeBase):

    def __init__(self, *args,
                 act_slices=None,
                 out_shapes=None,
                 axis=None,
                 **kwargs):

        super(SplitNode, self).__init__(*args, **kwargs)
        self.act_slices = act_slices
        self.out_shapes = out_shapes
        self.axis = axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "act_slices", "out_shapes", "axis"
        ]})
        return super_attrs

    def __call__(self, *args, **kwargs):
        return super().__call__(*args, num_outputs=len(self.act_slices), **kwargs)

    @property
    def does_nothing(self) -> bool:
        return self.out_dims and len(self.out_dims) == 1

    @property
    def exclude_from_generation(self) -> bool:
        return self.does_nothing

    @property
    def in_shape(self):
        sum_split = sum(shape[self.axis] for shape in self.out_shapes)
        return tuple(list(self.out_shapes[0][:self.axis]) + [sum_split] + list(self.out_shapes[0][self.axis+1:]))

    @property
    def no_model_code(self) -> bool:
        return self.axis in leading_real(self.in_shape)

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
            if in_shape[axis] is not None and any(split == -1 for split in splits):
                rest_sz = sum(split for split in splits if split > 0)
                splits = (split if split >
                          0 else in_shape[axis] - rest_sz for split in splits)
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
        return out_size

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "A {} {}".format(
            self.axis,
            self.at_options
        )


@cls_op_name('gather')
class GatherNode(NNNodeBase, SingleInputAndOutputMixin, SensitiveToOrderMixin, InsensitiveToQuantizationMixin):

    def __init__(self, *args,
                 axis=None,
                 indices=None,
                 **kwargs):

        super(GatherNode, self).__init__(*args, **kwargs)
        self.axis = axis
        self.indices = np.array(indices)

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "indices", "axis"
        ]})
        return super_attrs

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
class StridedSliceNode(NNNodeBase, SingleInputAndOutputMixin, ComparableNodeMixin, InsensitiveToQuantizationMixin):

    def __init__(self, *args,
                 act_slice=None,
                 out_shape=None,
                 **kwargs):
        super(StridedSliceNode, self).__init__(*args, **kwargs)
        self.act_slice = act_slice
        self.out_shape = tuple(out_shape)

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "act_slice", "out_shape"
        ]})
        return super_attrs

    @property
    def graph_label(self):
        return [self.name] + ["(%s,%s,%s)" % elem for elem in self.act_slice]

    @property
    def graph_anon_label(self):
        return ['Slice'] + ["(%s,%s,%s)" % elem for elem in self.act_slice]

    @property
    def slice_shape(self):
        return tuple(
            int(abs(math.ceil((max(sl[1], -1) - max(sl[0], -1))/sl[2]))) for sl in self.act_slice)

    @property
    def slices_axes(self):
        in_shape = self.in_dims[0].shape
        return tuple(idx for idx, shapes in enumerate(zip(self.slice_shape, in_shape)) if shapes[0] != shapes[1])

    @property
    def changes_shape(self):
        return self.slice_shape != self.out_shape

    @property
    def can_equalize(self):
        return False

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

    def is_same_operation_as(self, G, other):
        if not isinstance(other, StridedSliceNode):
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
    def does_nothing(self) -> bool:
        return self.no_model_code and not self.changes_shape

    @property
    def has_neg_stride(self) -> bool:
        # if the slice is negative but the size is one treat as positive
        return any(sl[2] < 0 and self.slice_shape[axis] > 1 for axis, sl in enumerate(self.act_slice))

    @property
    def no_model_code(self) -> bool:
        if not self.in_dims:
            return False
        return self.slice_shape == tuple(self.in_dims[0].shape) and not self.has_neg_stride

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        return [Dim.unnamed(self.out_shape)]

    def __str__(self):
        return ",".join("(%s,%s,%s)" % elem for elem in self.act_slice)

@cls_op_name('batch_to_space')
class BatchToSpaceNode(NNNodeBase, SensitiveToOrderMixin):

    def __init__(self, *args,
                 block_shape=None,
                 crops=None,
                 **kwargs):
        super().__init__(*args, **kwargs)
        self.block_shape = block_shape
        self.crops = crops

    @property
    def graph_label(self):
        return [self.name, f'blocks {self.block_shape!r}', f'crops {self.crops!r}']

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        shape = batch_to_space_shape_nd(in_dims[0].shape, self.block_shape, self.crops)
        return [Dim.unnamed(shape)]

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return ""

@cls_op_name('space_to_batch')
class SpaceToBatchNode(NNNodeBase, SensitiveToOrderMixin):

    def __init__(self, *args,
                 block_shape=None,
                 pads=None,
                 **kwargs):
        super().__init__(*args, **kwargs)
        self.block_shape = block_shape
        self.pads = pads

    @property
    def graph_label(self):
        return [self.name, f'blocks {self.block_shape!r}', f'pads {self.pads!r}']

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        shape = space_to_batch_shape_nd(in_dims[0].shape, self.block_shape, self.pads)
        return [Dim.unnamed(shape)]

    def __str__(self):
        return ""

@cls_op_name('tile')
class TileNode(NNNodeBase, SensitiveToOrderMixin):

    def __init__(self, *args,
                 reps=None,
                 pads=None,
                 **kwargs):
        super().__init__(*args, **kwargs)
        self.reps = reps

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        out_shape = tuple([
            dim * rep
            for dim, rep in zip(in_dims[0].shape, self.reps)])
        return [Dim.unnamed(out_shape)]

    def __str__(self):
        return ""
