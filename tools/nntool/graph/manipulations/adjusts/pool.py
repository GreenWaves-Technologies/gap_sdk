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

from graph.types import (ActivationFusion, AveragePoolParameters,
                         MaxPoolParameters, PoolingParameters)

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

@handles(MaxPoolParameters, AveragePoolParameters)
class PoolAdjuster(AdjusterBase):
    def adjust(self, G, node):
        modified = False
        # check that the transposed input 0 matches autotiler order
        names = node.in_dims[0].order
        if names != node.ker_in_order[0]:
            self.adjust_in_out_order(G, node, names, node.ker_in_order[0])
            modified = True

        return modified

@handles(ActivationFusion)
class PoolFusionAdjuster(AdjusterBase):
    def adjust(self, G, node):
        modified = False
        fusion_node = node
        pool_node = next(iter([node for node in fusion_node.contained_nodes() if isinstance(node, PoolingParameters)]), None)
        if pool_node is None:
            return modified
        # check that the transposed input 0 matches autotiler order
        names = pool_node.in_dims[0].order
        if names != pool_node.ker_in_order[0]:
            self.adjust_in_out_order(G, pool_node, names, pool_node.ker_in_order[0], fusion=fusion_node)
            modified = True

        return modified
