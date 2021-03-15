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

from graph.types import BilinearResizerParameters, NearestNeighborResizerParameters

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

@handles(BilinearResizerParameters, NearestNeighborResizerParameters)
class ResizerAdjuster(AdjusterBase):
    def adjust_input(self, G, node, names):
        self.verify_chw(node, names)
        trans = self.get_trans(names, ['c', 'h', 'w'])
        self.apply_input_trans(node, trans)
        self.apply_output_trans(node, self.invert(trans), index=0)

    def adjust(self, G, node):
        modified = False
        # check that the transposed input 0 matches autotiler order
        names = node.in_dims[0].order
        if node.transpose_in is not None and node.transpose_in[0] is not None:
            names = self.trans_names(names, node.transpose_in[0])
        if names != ['c', 'h', 'w']:
            self.adjust_input(G, node, names)
            modified = True

        return modified
