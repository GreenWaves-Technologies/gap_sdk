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

from .base import Parameters, SameNumberOfDimensionsForInputs

LOG = logging.getLogger("nntool." + __name__)

class MatrixBroadcastedLinearOpParameters(Parameters, SameNumberOfDimensionsForInputs):
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
        raise NotImplementedError()

    def compute_load(self):
        return self.out_dims[0].size() * 2

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
        max_idx, _ = max(enumerate(self.in_dims), key=lambda x: x[1].size())
        return [self.in_dims[max_idx].clone()]

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
