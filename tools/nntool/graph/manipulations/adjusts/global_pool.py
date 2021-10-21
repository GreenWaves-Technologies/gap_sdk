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

from graph.types import GlobalAveragePoolParameters, GlobalMaxPoolParameters, ActivationFusion, GlobalPoolingParameters, GlobalSumPoolParameters

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

class GlobalPoolAdjusterBase(AdjusterBase):
    def _do_adjust(self, G, node, pool_node):
        # make sure that node.axis axes are at the RHS of the tensor
        in_dim_len = len(pool_node.in_dims[0].shape)
        # how many dimensions of the tensor do we keep after reduction
        dim_keep = in_dim_len - len(pool_node.axis)
        # check that none of the reduction dimensions are in the first dim_keep
        # dimensions
        if all(red_axis not in range(dim_keep) for red_axis in pool_node.axis):
            return False
        LOG.info("global pool fusion %s: inserting transpose before operation", node.name)
        # we need a transpose [all not in axis] + axis
        transpose = [i for i in range(in_dim_len) if i not in pool_node.axis] + list(pool_node.axis)
        self.apply_input_trans(G, node, transpose)
        # if we keep dimensions then we need to transpose on output
        # this will be reduced to a reshape by eliminate transposes so no code
        # will be generated. But dimension calculation will fail without it.
        if pool_node.keep_dims:
            LOG.info("global pool fusion %s: inserting transpose after operation", node.name)
            self.apply_output_trans(G, node, self.invert(transpose), index=0)
        # move axis onto new home
        pool_node.axis = [transpose.index(i) for i in pool_node.axis]
        return True        

@handles(GlobalAveragePoolParameters, GlobalMaxPoolParameters, GlobalSumPoolParameters)
class GlobalPoolAdjuster(GlobalPoolAdjusterBase):
    def adjust(self, G, node):
        return self._do_adjust(G, node, node)

@handles(ActivationFusion)
class GlobalPoolFusionAdjuster(GlobalPoolAdjusterBase):
    def adjust(self, G, node):
        fusion_node = node
        pool_node = next(iter([node for node in fusion_node.contained_nodes() if isinstance(node, GlobalPoolingParameters)]), None)
        if pool_node is None:
            return False

        return self._do_adjust(G, node, pool_node)
