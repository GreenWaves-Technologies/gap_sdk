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
from copy import deepcopy

from graph.types.base import NNEdge
from graph.types.others import QuantizeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, run_before

LOG = logging.getLogger("nntool." + __name__)

@match_name("remove_unnecessary_quantize_operators")
@description("Remove quantize, dequantize and casts where duplicated")
@run_before('*')
class RemoveUnnecessaryQuantizeOperators(Matcher):

    @staticmethod
    def get_single_quantize_edge(G, node):
        out_edges = G.out_edges(node.name)
        if len(out_edges) != 1:
            return None
        out_edge = out_edges[0]
        if not isinstance(out_edge.to_node, QuantizeParameters):
            return None
        return out_edge.to_node

    # removes:
    # quantize -> quantize with singe edge between
    # quantize where qrec indicates that input and output quantization are the same
    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        modified_graph = False
        quantize_nodes = G.nodes(node_classes=QuantizeParameters)
        while quantize_nodes:
            node = quantize_nodes.pop(0)
            if G.quantization:
                qrec = G.quantization.get(NodeId(node))
                if not qrec:
                    continue
                if deepcopy(qrec.in_qs[0]) == qrec.out_qs[0]:
                    modified_graph = True
                    LOG.info(
                        'removing quantize node %s from %s to %s',
                        node.name, qrec.in_qs[0], qrec.out_qs[0])
                    G.remove_and_reconnect(node, edge_class=NNEdge)
                    del G.quantization[NodeId(node)]
                    continue

            next_node = self.get_single_quantize_edge(G, node)
            while next_node:
                LOG.info(
                    'removing quantize node %s and modifying node %s to output %s',
                    next_node.name, node.name, next_node.to_qtype)
                G.remove_and_reconnect(next_node, edge_class=NNEdge)
                node.to_qtype = next_node.to_qtype
                modified_graph = True
                if G.quantization:
                    this_rec = G.quantization[NodeId(node)]
                    next_rec = G.quantization[NodeId(next_node)]
                    this_rec.out_qs = next_rec.out_qs
                    del G.quantization[NodeId(next_node)]
                next_node = self.get_single_quantize_edge(G, node)

        if set_identity:
            self.set_identity(G)

        return modified_graph
