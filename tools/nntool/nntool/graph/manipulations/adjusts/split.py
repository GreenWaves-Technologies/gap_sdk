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

from nntool.graph.manipulations.adjusts.concat import HWC_CONCAT_SPLIT_AXIS_LIMIT, all_ones_before
from nntool.graph.types.input_output import OutputNode
from ...types import SplitNode

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger(__name__)


def has_unused(G, node):
    return any(isinstance(edges[0].to_node, OutputNode) and edges[0].to_node.not_used
               for edges in G.indexed_out_edges(node) if len(edges) == 1)


def split_can_work_on_last(G, node):
    return not has_unused(G, node) and len(node.out_dims) <= HWC_CONCAT_SPLIT_AXIS_LIMIT


@handles(SplitNode)
class SplitAdjuster(AdjusterBase):
    def needs_adjust(self, G, node):
        if node.axis == 0:
            return False
        if node.axis == (len(node.in_dims[0]) - 1) and split_can_work_on_last(G, node):
            return False
        return True

    def adjust(self, G, node):
        # if the split axis is already 0 nothing to do
        if node.axis == 0:
            return False
        # if all the axes before are ones then we still move because it will be a stack
        if (node.axis == (len(node.in_dims[0]) - 1) and
            split_can_work_on_last(G, node) and
                not all_ones_before(node.in_dims[0], node.axis)):
            return False
        LOG.info("split %s: moving axis %s to 0 and inserting transposes",
                 node.name, node.axis)
        trans = self.move_axis_to_zero_trans(node.axis, node.in_dims[0].shape)
        self.apply_input_trans(G, node, trans, index=0)
        node.transpose_params(trans)
        node.axis = 0
        self.apply_output_trans(G, node, self.invert(
            trans), check_unused_output=has_unused(G, node))
        return True
