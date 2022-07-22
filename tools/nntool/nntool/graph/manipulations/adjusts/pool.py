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

from nntool.graph.types import (ActivationFusionNode, AveragePoolNode,
                         MaxPoolNode, PoolingNodeBase)

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger(__name__)


@handles(MaxPoolNode, AveragePoolNode)
class PoolAdjuster(AdjusterBase):
    def needs_adjust(self, G, node):
        names = node.in_dims[0].order
        return names != node.ker_in_order[0]

    def adjust(self, G, node):
        modified = False
        # check that the transposed input 0 matches autotiler order
        names = node.in_dims[0].order
        if names != node.ker_in_order[0]:
            self.adjust_in_out_order(G, node, names, node.ker_in_order[0])
            modified = True

        return modified


@handles(ActivationFusionNode)
class PoolFusionAdjuster(AdjusterBase):
    def needs_adjust(self, G, node):
        pool_node = next(iter([fnode for fnode in node.contained_nodes()
                               if isinstance(fnode, PoolingNodeBase)]), None)
        if pool_node is None:
            return False
        names = pool_node.in_dims[0].order
        return names != pool_node.ker_in_order[0]

    def adjust(self, G, node):
        modified = False
        fusion_node = node
        pool_node = next(iter([fnode for fnode in fusion_node.contained_nodes()
                               if isinstance(fnode, PoolingNodeBase)]), None)
        if pool_node is None:
            return modified
        # check that the transposed input 0 matches autotiler order
        names = pool_node.in_dims[0].order
        if names != pool_node.ker_in_order[0]:
            self.adjust_in_out_order(
                G, pool_node, names, pool_node.ker_in_order[0], fusion=fusion_node)
            modified = True

        return modified
