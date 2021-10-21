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

from graph.types import CopyParameters, NNEdge, TransposeParameters
from graph.types.others import QuantizeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import (Matcher, description, groups, match_name,
                       modifies_dimensions, run_after)

LOG = logging.getLogger("nntool." + __name__)


def valid_node(node):
    return (isinstance(node, TransposeParameters) and not node.does_nothing()) or isinstance(node, QuantizeParameters)


@match_name("remove_copies")
@description("Remove unnecessary copies")
@modifies_dimensions(True)
@groups('*')
@run_after('expand_transposes')
class RemoveCopies(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        nodes_to_remove = []
        for node in G.nodes(node_classes=CopyParameters):
            in_edge = G.in_edges(node.name)[0]
            if (valid_node(in_edge.from_node) and
                    not G.num_out_edges(in_edge.from_node.name) > 1):
                nodes_to_remove.append(node)
                continue
            out_edges = G.out_edges(node.name)
            if len(out_edges) != 1:
                continue
            out_edge = out_edges[0]
            if valid_node(out_edge.to_node):
                nodes_to_remove.append(node)
                continue
        for node in nodes_to_remove:
            LOG.info(
                "remove redundant copy %s",
                node.name)
            has_modified_graph = True
            G.remove_and_reconnect(node, edge_class=NNEdge)
            if G.quantization:
                nid = NodeId(node)
                if nid in G.quantization:
                    del G.quantization[nid]

        if set_identity:
            self.set_identity(G)
        return has_modified_graph
