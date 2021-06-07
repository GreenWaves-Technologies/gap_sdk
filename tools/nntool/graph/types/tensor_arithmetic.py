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

from expressions.symbolic.basic import Add, Div, Mul, Sub

from ..dim import Dim
from .base import (CanFuseToExpression, Parameters, SameNumberOfDimensionsForInputs,
                   Transposable, expression_op, cls_op_name)

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
        if self.broadcast is None:
            raise ValueError(f'broadcast has not been set on {self.name}')
        res = []
        for idx, input_tensor in enumerate(input_tensors):
            self.verify_broadcast(input_tensor.shape, self.broadcast[idx])
            res.append(np.reshape(input_tensor.copy(), self.broadcast[idx]))
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

    def delete_transpose(self, in_idx, trans):
        #TODO - Implement
        pass


class MatrixBroadcastedLinearOpParameters(CanFuseToExpression, Transposable,
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
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim
                       for dim, trans in zip(in_dims, self.transpose_in)]
        if self.broadcast is None:
            self.set_broadcast([dim.shape for dim in in_dims])
        out_dim = Dim.broadcast(in_dims)
        if self.transpose_out and self.transpose_out[0]:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def should_fuse(self, node_set, qrec=None):
        for transpose in [self.transpose_in, self.transpose_out]:
            if transpose is None:
                continue
            if any(trans is not None for trans in transpose):
                return False
        return True

    def __str__(self):
        return "{} {} {}".format(self.op_name, Transposable.__str__(self), self.at_options)

@expression_op(Add)
@cls_op_name('add')
class MatrixAddParameters(MatrixBroadcastedLinearOpParameters):
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

    def should_fuse(self, node_set, qrec=None):
        # add should fuse into an expression if there are several adds or the input
        # shapes don't match since we don't have broadcasted kernels in the AT gens
        if self.TEST_MODE:
            return True
        return super().should_fuse(node_set, qrec=qrec) and (len(node_set) > 1 or self.in_dims[0].layout_shape != self.in_dims[1].layout_shape)


@expression_op(Mul)
@cls_op_name('mul')
class MatrixMulParameters(MatrixBroadcastedLinearOpParameters):
    pass


@cls_op_name('sub')
@expression_op(Sub)
class MatrixSubParameters(MatrixBroadcastedLinearOpParameters):
    pass


@cls_op_name('div')
@expression_op(Div)
class MatrixDivParameters(MatrixBroadcastedLinearOpParameters):
    pass


@cls_op_name('matmul')
class MatMulOpParameters(Transposable):

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
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim
                       for dim, trans in zip(in_dims, self.transpose_in)]
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
        if self.transpose_out and self.transpose_out[0]:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def __str__(self):
        return "{} {} {}".format(self.op_name, Transposable.__str__(self), self.at_options)
