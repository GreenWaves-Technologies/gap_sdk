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

from .base import SingleInputAndOutput, Transposable

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method
class ResizerParameters(SingleInputAndOutput, Transposable):

    SUPPORTED_OP_TYPES = ['BILINEAR', 'NEAREST']

    def __init__(self, name, new_shape=None, align_corners=False, halfpixel_centers=False, **kargs):
        super(ResizerParameters, self).__init__(name, **kargs)
        self._new_shape = new_shape # always (new_H, new_W) order
        self._align_corners = align_corners
        self._halfpixel_centers = halfpixel_centers

    @property
    def new_shape(self):
        return self._new_shape

    def get_output_size(self, in_dims):
        in_dims = self.clone_dim_with_hints(in_dims)
        if self.transpose_in and self.transpose_in[0]:
            out_dim = in_dims[0].calc_transpose(self.transpose_in[0])
        else:
            out_dim = in_dims[0]
        out_dim.h = self.new_shape[0]
        out_dim.w = self.new_shape[1]
        if self.transpose_out and self.transpose_out[0]:
            out_dim = out_dim.calc_transpose(self.transpose_out[0])
        return [out_dim]

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return "Resizer {} {} {}".format(
            self.op_name,
            Transposable.__str__(self),
            self.at_options
        )

class NearestNeighborResizerParameters(ResizerParameters):
    @property
    def op_name(self):
        return "nearest_neighbor"

    def compute_load(self):
        return self.new_shape[0] * self.new_shape[1] * 3 # 2 rounding (2op) + 1 ass

class BilinearResizerParameters(ResizerParameters):

    @property
    def op_name(self):
        return "bilinear"

    def compute_load(self):
        return self.new_shape[0] * self.new_shape[1] * 19
