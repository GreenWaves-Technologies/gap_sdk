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
from nntool.graph.types import NNEdge, ReshapeNode, StridedSliceNode
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, description, run_before, groups

LOG = logging.getLogger(__name__)


@match_name("remove_slice")
@description("Removes slices that are doing nothing and may insert a reshape if necessary.")
@run_before('*')
@groups('symmetric', 'scaled')
class RemoveSlice(Matcher):

    def _match(self, G: GraphView, **kwargs) -> bool:
        has_modified_graph = False
        for node in [node for node in G.nodes(node_classes=StridedSliceNode)]:
            if node.slice_shape != tuple(node.in_dims[0].shape) or node.has_neg_stride:
                continue
            has_modified_graph = True
            nid = node.name
            if node.slice_shape == node.out_shape:
                LOG.info(f'removing strided slice {node.name} that does nothing')
                G.remove_and_reconnect(node, edge_class=NNEdge)
                if G.quantization and nid in G.quantization:
                    del G.quantization[nid]
            else:
                reshape = ReshapeNode(G.unique_name(f'{node.name}_reshape'), old_shape=node.slice_shape, shape=node.out_shape)
                LOG.info(f'replacing strided slice {node.name} with reshape {reshape.name}')
                G.replace_node(node, reshape)
                if G.quantization and nid in G.quantization:
                    G.quantization[reshape.name] = G.quantization[nid]
                    del G.quantization[nid]



        return has_modified_graph
