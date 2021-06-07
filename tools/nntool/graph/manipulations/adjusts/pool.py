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

from graph.types.pooling import AveragePoolParameters, MaxPoolParameters

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

@handles(MaxPoolParameters, AveragePoolParameters)
class PoolAdjuster(AdjusterBase):
    def adjust(self, G, node):
        modified = False
        # check that the transposed input 0 matches autotiler order
        names = node.in_dims[0].order
        # TODO - once pooling is integrated into NE16 kernel check for presence here
        if node.transpose_in is not None and node.transpose_in[0] is not None:
            names = self.trans_names(names, node.transpose_in[0])
        if names != node.ker_in_order[0]:
            self.adjust_in_out_order(G, node, names, node.ker_in_order[0])
            modified = True

        return modified
