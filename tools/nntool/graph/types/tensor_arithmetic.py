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

from expressions.symbolic.basic import Add, Div, Mul, SquaredDifference, Sub

from ..dim import Dim
from .base import (CanFuseToExpression, Parameters, SameNumberOfDimensionsForInputs, SensitiveToOrder,
                   expression_op, cls_op_name, nargs, ComparableParameters)

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method
class Broadcastable(Parameters):
    def __init__(self, *args, **kwargs) -> None:
        super(Broadcastable, self).__init__(*args, **kwargs)
        self.broadcast = None
        self.axis_masks = None
        self._is_broadcasted = False

    def set_broadcast(self, shapes):
        max_len = max(len(shape) for shape in shapes)
        self.broadcast = [([1] * max(max_len - len(shape), 0)) + shape for shape in shapes]
        self.axis_masks = [([None] * max(max_len - len(shape), 0)) + list(range(len(shape))) for shape in shapes]
        self._is_broadcasted = not all(
            self.broadcast[idx] == shape for idx, shape in enumerate(shapes))

    @property
    def is_broadcasted(self):
        return self._is_broadcasted

    @staticmethod
    def strip_ones(shape):
        return [dim for dim in shape if dim != 1]

    def verify_broadcast(self, shape, broadcast):
        if self.strip_ones(shape) != self.strip_ones(broadcast):
            raise ValueError(f'invalid broadcast {shape} -> {broadcast} on {self.name}')

    def broadcast_inputs(self, input_tensors):
        shapes = [tensor.shape for tensor in input_tensors]
        max_len = max(len(shape) for shape in shapes)
        broadcasted_shapes = [([1] * (max_len - len(shape)))+list(shape) for shape in shapes]
        max_dim_shape = [max(dims) for dims in zip(*broadcasted_shapes)]
        for in_idx, shape in enumerate(broadcasted_shapes):
            for idx, dim in enumerate(shape):
                if dim != max_dim_shape[idx] and dim != 1:
                    raise ValueError(f"invalid broadcast {self.name} input {in_idx} {shape} -> {max_dim_shape}")
        res = []
        for idx, input_tensor in enumerate(input_tensors):
            res.append(np.reshape(input_tensor.copy(), broadcasted_shapes[idx]))
        return res

    @staticmethod
    def transpose_mask(mask, trans):
        out_idx = 0
        res = []
        for axis in trans:
            if mask[axis] is None:
                res.append(None)
            else:
                res.append(out_idx)
                out_idx += 1
        return tuple(res)

    def transpose_broadcast(self, transpose):
        # # broadcasted input could have included batch dimensions
        # # just expand the transpose not moving those axes
        # while len(transpose) < max(len(shape) for shape in self.broadcast):
        #     transpose = [0] + [dim + 1 for dim in transpose]
        self.broadcast = [tuple([shape[idx] for idx in transpose]) for shape in self.broadcast]
        self.axis_masks = [self.transpose_mask(shape, transpose) for shape in self.axis_masks]

    def derive_transpose(self, full_transpose, in_idx):
        mask = self.axis_masks[in_idx]
        return tuple([mask[idx] for idx in full_transpose if mask[idx] is not None])

@nargs({2})
class MatrixBroadcastedLinearOpParameters(CanFuseToExpression,
                                          SameNumberOfDimensionsForInputs, Broadcastable):
    def __init__(self, name, *args, **kwargs):
        super(MatrixBroadcastedLinearOpParameters, self).__init__(name, *args, **kwargs)
        self.at_options.valid_options['PARALLELFEATURES'] = int
        self.at_options.valid_options['TILEORIENTATION'] = int

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        if self.broadcast is None:
            self.set_broadcast([dim.shape for dim in in_dims])
        out_dim = Dim.broadcast(in_dims)
        return [out_dim]

    def should_fuse(self, node_set, qrec=None):
        return True

    def __str__(self):
        return f"{self.op_name} {self.at_options}"
        
@expression_op(Add)
@cls_op_name('add')
class MatrixAddParameters(MatrixBroadcastedLinearOpParameters, ComparableParameters):
    TEST_MODE = False

    def __init__(self, name, *args, **kwargs):
        super(MatrixAddParameters, self).__init__(name, *args, **kwargs)
        self._force_quantized_index = None

    # This property is used in the padded add where the input scaled must always be the idx=0
    @property
    def force_quantized_index(self):
        return self._force_quantized_index

    @force_quantized_index.setter
    def force_quantized_index(self, val):
        self._force_quantized_index = val

    def is_same_operation_as(self, G, other):
        if self.__class__ != other.__class__:
            return False
        my_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(self)]
        other_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(other)]
        return all(edge in other_in_edges for edge in my_in_edges)


    def should_fuse(self, node_set, qrec=None):
        # add should fuse into an expression if there are several adds or the input
        # shapes don't match since we don't have broadcasted kernels in the AT gens
        if self.TEST_MODE:
            return True
        return super().should_fuse(node_set, qrec=qrec) and (len(node_set) > 1 or self.in_dims[0].layout_shape != self.in_dims[1].layout_shape)


@expression_op(Mul)
@cls_op_name('mul')
class MatrixMulParameters(MatrixBroadcastedLinearOpParameters, ComparableParameters):
    def is_same_operation_as(self, G, other):
        if self.__class__ != other.__class__:
            return False
        my_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(self)]
        other_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(other)]
        return all(edge in other_in_edges for edge in my_in_edges)


@expression_op(Sub)
@cls_op_name('sub')
class MatrixSubParameters(MatrixBroadcastedLinearOpParameters, ComparableParameters):
    def is_same_operation_as(self, G, other):
        if self.__class__ != other.__class__:
            return False
        my_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(self)]
        other_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(other)]
        return all(edge == other_edge for edge, other_edge in zip(my_in_edges, other_in_edges))


@expression_op(Div)
@cls_op_name('div')
class MatrixDivParameters(MatrixBroadcastedLinearOpParameters, ComparableParameters):
    def is_same_operation_as(self, G, other):
        if self.__class__ != other.__class__:
            return False
        my_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(self)]
        other_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(other)]
        return all(edge == other_edge for edge, other_edge in zip(my_in_edges, other_in_edges))

@expression_op(SquaredDifference)
@cls_op_name('squared_difference')
class SquaredDifferenceParameters(MatrixBroadcastedLinearOpParameters, ComparableParameters):
    def is_same_operation_as(self, G, other):
        if self.__class__ != other.__class__:
            return False
        my_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(self)]
        other_in_edges = [(edge.from_node, edge.from_idx) for edge in G.indexed_in_edges(other)]
        return all(edge == other_edge for edge, other_edge in zip(my_in_edges, other_in_edges))


@cls_op_name('matmul')
@nargs({2, 3})
class MatMulOpParameters(Parameters, SensitiveToOrder):

    def __init__(self, name, *args, **kwargs):
        super(MatMulOpParameters, self).__init__(name, *args, **kwargs)
        self.at_options.valid_options['PARALLELFEATURES'] = int
        self.at_options.valid_options['TILEORIENTATION'] = int
        # axes of extra channel dimension on either operand

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def compute_load(self):
        line_m1 = self.in_dims[0].shape[-2]
        col_m1 = self.in_dims[0].shape[-1]
        col_m2 = self.in_dims[1].shape[-2] if isinstance(self, MatMulTransposedParameters) else self.in_dims[1].shape[-1]
        n_mat = np.prod(self.in_dims[1].shape[:-2])
        return n_mat * (line_m1 * col_m1 * col_m2)

    def get_output_size(self, in_dims):
        x_shape = list(in_dims[0].shape).copy()
        y_shape = list(in_dims[1].shape).copy()

        if len(x_shape) == 1:
            x_shape = [1] + x_shape
            remove_first = True
        else:
            remove_first = False

        if len(y_shape) == 1:
            y_shape = y_shape + [1]
            remove_last = True
        else:
            remove_last = False
        x_chans = x_shape[:-2:]
        y_chans = y_shape[:-2:]
        out_chans = Dim.npbroadcast([x_chans, y_chans])
        x = [] if remove_first else [x_shape[-2]]
        y = [] if remove_last else [y_shape[-1]]
        out_dim = Dim.unnamed(out_chans + x + y)
        return [out_dim]

    def __str__(self):
        return f"{self.op_name} {self.at_options}"

@cls_op_name('matmulT')
class MatMulTransposedParameters(MatMulOpParameters):
    def get_output_size(self, in_dims):
        x_shape = list(in_dims[0].shape).copy()
        y_shape = list(in_dims[1].shape).copy()

        if len(x_shape) == 1:
            x_shape = [1] + x_shape
            remove_first = True
        else:
            remove_first = False

        if len(y_shape) == 1:
            y_shape = y_shape + [1]
            remove_last = True
        else:
            remove_last = False
        y_shape = y_shape[:-2] + y_shape[-2:][::-1]
        x_chans = x_shape[:-2:]
        y_chans = y_shape[:-2:]
        out_chans = Dim.npbroadcast([x_chans, y_chans])
        x = [] if remove_first else [x_shape[-2]]
        y = [] if remove_last else [y_shape[-1]]
        out_dim = Dim.unnamed(out_chans + x + y)
        return [out_dim]

    def __str__(self):
        return f"Transposed {self.op_name} {self.at_options}"
