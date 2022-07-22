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

from nntool.graph.types.base import NNEdge
from nntool.graph.types.misc import QuantizeNode
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, description, run_before

LOG = logging.getLogger(__name__)

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
        if not isinstance(out_edge.to_node, QuantizeNode):
            return None
        return out_edge.to_node

    # removes:
    # quantize -> quantize with singe edge between
    # quantize where qrec indicates that input and output quantization are the same
    def _match(self, G: GraphView, **kwargs):
        modified_graph = False
        quantize_nodes = G.nodes(node_classes=QuantizeNode)
        while quantize_nodes:
            node = quantize_nodes.pop(0)
            if G.quantization:
                qrec = G.quantization.get(node.name)
                if not qrec:
                    continue
                if deepcopy(qrec.in_qs[0]) == qrec.out_qs[0]:
                    modified_graph = True
                    LOG.info(
                        'removing quantize node %s from %s to %s',
                        node.name, qrec.in_qs[0], qrec.out_qs[0])
                    G.remove_and_reconnect(node, edge_class=NNEdge)
                    del G.quantization[node.name]
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
                    this_rec = G.quantization[node.name]
                    next_rec = G.quantization[next_node.name]
                    this_rec.out_qs = next_rec.out_qs
                    del G.quantization[next_node.name]
                next_node = self.get_single_quantize_edge(G, node)



        return modified_graph
