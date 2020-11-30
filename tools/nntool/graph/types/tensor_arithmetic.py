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

from ..dim import Dim
from .base import SameNumberOfDimensionsForInputs, Transposable

LOG = logging.getLogger("nntool." + __name__)

class MatrixBroadcastedLinearOpParameters(Transposable, SameNumberOfDimensionsForInputs):
    def __init__(self, name, *args, **kwargs):
        super(MatrixBroadcastedLinearOpParameters, self).__init__(name, *args, **kwargs)
        self.at_options.valid_options['PARALLELFEATURES'] = int
        self.at_options.valid_options['TILEORIENTATION'] = int

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def clone(self, name, groupn=None):
        return None

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim for dim, trans in zip(self.in_dims, self.transpose_in)]
        else:
            in_dims = self.in_dims
        out_dim = Dim.broadcast(in_dims)
        if self.transpose_out and self.transpose_out[0]:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def __str__(self):
        return "{} {} {}".format(self.op_name, Transposable.__str__(self), self.at_options)


class MatrixAddParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "add"


class MatrixMulParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "mul"


class MatrixSubParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "sub"


class MatrixDivParameters(MatrixBroadcastedLinearOpParameters):
    op_name = "div"

class MatMulOpParameters(Transposable):
    op_name = "matmul"

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

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim for dim, trans in zip(self.in_dims, self.transpose_in)]
        else:
            in_dims = self.in_dims
        x_shape = list(in_dims[0].shape).copy()
        y_shape = list(in_dims[1].shape).copy()

        if len(x_shape) == 1:
            x_shape = [1] + x_shape
            remove_first = True
        else:
            remove_first = False

        if len(y_shape) == 1:
            y_shape = y_shape + [1]
            remove_last  = True
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
