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

from .adjust_base import AdjusterBase

LOG = logging.getLogger("nntool." + __name__)

class GlobalPoolAdjuster(AdjusterBase):
    def adjust(self, G, node):
        # make sure that node.axis axes are at the RHS of the tensor
        in_dim_len = len(node.in_dims[0].shape)
        # how many dimensions of the tensor do we keep after reduction
        dim_keep = in_dim_len - len(node.axis)
        # check that none of the reduction dimensions are in the first dim_keep
        # dimensions
        if all(red_axis not in range(dim_keep) for red_axis in node.axis):
            return False
        LOG.info("global pool %s: inserting transpose before operation", node.name)
        # we need a transpose [all not in axis] + axis
        transpose = [i for i in range(in_dim_len) if i not in node.axis] + node.axis
        node.transpose_in = [transpose]
        # if we keep dimensions then we need to transpose on output
        # this will be reduced to a reshape by eliminate transposes so no code
        # will be generated. But dimension calculation will fail without it.
        if node.keep_dims:
            LOG.info("global pool %s: inserting transpose after operation", node.name)
            node.transpose_out = [self.invert(transpose)]
        # move axis onto new home
        node.axis = [transpose.index(i) for i in node.axis]
        return True
