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

import numpy as np
from bfloat16 import bfloat16
from graph.types.base import InsensitiveToQuantization, NNEdge
from graph.types.input_output import InputParameters, OutputParameters
from graph.types.others import QuantizeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, run_before

LOG = logging.getLogger("nntool." + __name__)

@match_name("remove_quantize_operators")
@description("Remove quantize, dequantize and casts where possible")
@run_before('*')
class RemoveQuantizeOperators(Matcher):

    def propagate_up(self, G, node, qtype, starting=True):
        if not starting and not isinstance(node, InsensitiveToQuantization):
            if isinstance(node, QuantizeParameters):
                node.to_qtype = deepcopy(qtype)
                if G.quantization:
                    G.quantization[NodeId(node)].out_qs[0] = deepcopy(qtype)
                return True
            return False
        for edge in G.in_edges(node.name):
            if not self.propagate_up(G, edge.from_node, qtype, starting=False):
                return False
        if G.quantization:
            qrec = G.quantization[NodeId(node)]
            for edge in G.in_edges(node.name):
                qrec.in_qs[edge.to_idx] = deepcopy(qtype)
                from_qrec = G.quantization[NodeId(edge.from_node)]
                from_qrec.out_qs[edge.from_idx] = deepcopy(qtype)
            if isinstance(node, InputParameters):
                qrec.in_qs[0] = deepcopy(qtype)
        return True

    def propagate_down(self, G, node, qtype, starting=True):
        if not starting and not isinstance(node, InsensitiveToQuantization):
            if isinstance(node, QuantizeParameters):
                node.from_qtype = deepcopy(qtype)
                if G.quantization:
                    G.quantization[NodeId(node)].in_qs[0] = deepcopy(qtype)
                return True
            return False
        for edge in G.out_edges(node.name):
            if not self.propagate_down(G, edge.to_node, qtype, starting=False):
                return False
        if G.quantization:
            qrec = G.quantization[NodeId(node)]
            for edge in G.out_edges(node.name):
                qrec.out_qs[edge.from_idx] = deepcopy(qtype)
                to_qrec = G.quantization[NodeId(edge.to_node)]
                to_qrec.in_qs[edge.to_idx] = deepcopy(qtype)
            if isinstance(node, OutputParameters):
                qrec.out_qs[0] = deepcopy(qtype)
        return True

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        nodes_removed = []
        modified_graph = False
        for node in G.nodes(node_classes=QuantizeParameters):
            if issubclass(node.from_qtype.dtype, (np.floating, bfloat16)):
                if issubclass(node.to_qtype.dtype, (np.floating, bfloat16)):
                    LOG.warning(
                        'node %s quantizes from floating type to floating type and cannot directly be removed',
                        node.name)
                    continue
                if self.propagate_up(G, node, node.to_qtype):
                    modified_graph = True
                    nodes_removed.append(node)
                    G.remove_and_reconnect(node, edge_class=NNEdge)
                    if G.quantization:
                        del G.quantization[NodeId(node)]
                else:
                    LOG.warning('unable to remove quantize node %s', node.name)
            else:
                if self.propagate_down(G, node, node.from_qtype):
                    modified_graph = True
                    nodes_removed.append(node)
                    G.remove_and_reconnect(node, edge_class=NNEdge)
                    if G.quantization:
                        del G.quantization[NodeId(node)]
                else:
                    LOG.warning('unable to remove quantize node %s', node.name)

        if set_identity:
            self.set_identity(G)

        return modified_graph
