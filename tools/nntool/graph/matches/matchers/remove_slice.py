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
from graph.types import NNEdge
from graph.types.others import ReshapeParameters, StridedSliceParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, run_before, groups

LOG = logging.getLogger("nntool." + __name__)


@match_name("remove_slice")
@description("Removes slices that are doing nothing and may insert a reshape if necessary.")
@run_before('*')
@groups('symmetric', 'scaled')
class RemoveSlice(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        for node in [node for node in G.nodes(node_classes=StridedSliceParameters)]:
            if node.slice_shape != tuple(node.in_dims[0].shape):
                continue
            has_modified_graph = True
            nid = NodeId(node)
            if node.slice_shape == node.out_shape:
                LOG.info(f'removing strided slice {node.name} that does nothing')
                G.remove_and_reconnect(node, edge_class=NNEdge)
                if G.quantization and nid in G.quantization:
                    del G.quantization[nid]
            else:
                reshape = ReshapeParameters(G.unique_name(f'{node.name}_reshape'), old_shape=node.slice_shape, shape=node.out_shape)
                LOG.info(f'replacing strided slice {node.name} with reshape {reshape.name}')
                G.replace_node(node, reshape)
                if G.quantization and nid in G.quantization:
                    G.quantization[NodeId(reshape)] = G.quantization[nid]
                    del G.quantization[nid]

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
