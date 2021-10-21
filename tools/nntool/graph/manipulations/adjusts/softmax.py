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

from graph.types import SoftMaxParameters

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

@handles(SoftMaxParameters)
class SoftMaxAdjuster(AdjusterBase):
    def adjust(self, G, node):
        # if the softmax axis is already the last nothing to do
        last_axis = len(node.in_dims[0]) - 1
        if node.axis == last_axis:
            return False
        LOG.info("softmax %s: moving axis %s to %s and inserting transposes", node.name, node.axis, last_axis)
        trans = self.move_axis_to_last_trans(node.axis, node.out_dims[0].shape)
        self.apply_input_trans(G, node, trans)
        node.axis = last_axis
        self.apply_output_trans(G, node, self.invert(trans), index=0)
        return True
