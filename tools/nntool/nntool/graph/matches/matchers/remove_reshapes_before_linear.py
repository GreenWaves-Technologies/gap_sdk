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

from nntool.graph.types import LinearNode, NNEdge, ReshapeNode
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, description, run_before, groups

LOG = logging.getLogger(__name__)

@match_name("remove_reshapes_before_linear")
@description("Remove unnecessary reshapes that flatten linear inputs")
@run_before('fuse_gap_linear')
@groups('*')
class RemoveReshapesBeforeLinear(Matcher):

    def _match(self, G: GraphView, **kwargs):
        modified_graph = False
        candidates = set(G.nodes(node_classes=(ReshapeNode,)))

        while candidates:
            node = candidates.pop()
            out_edges = G.out_edges(node.name)
            if len(out_edges) != 1 or not isinstance(out_edges[0].to_node, LinearNode) or out_edges[0].to_node.batch_size > 1:
                continue
            LOG.info(
                'removing unnecessary reshape before linear %s', node.name)
            G.remove_and_reconnect(node, edge_class=NNEdge)
            modified_graph = True
            nid = node.name
            if G.quantization and G.quantization.get(nid):
                del G.quantization[nid]
            modified_graph = True



        return modified_graph
