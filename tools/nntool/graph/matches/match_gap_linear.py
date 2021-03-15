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

from graph.types import (ActivationParameters, ConvFusionParameters,
                         FcParameters, NNEdge)
from quantization.float32.float32_quantization import (
    Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord)
from quantization.multiplicative.mult_quantization import (
    MultQuantizationRecord, MultScalableFilterQuantizationRecord)
from quantization.symmetric.symmetric_quantization import (
    SymmetricQuantizationRecord, SymmetricScalableFilterQuantizationRecord)
from utils.graph import GraphView
from utils.node_id import NodeId

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)

class FusionMatch():
    def __init__(self) -> None:
        self.linear = None
        self.active = None
        self.order = []

    def add_node(self, params):
        if isinstance(params, FcParameters):
            if self.linear:
                return None
            self.order.append(params)
            self.linear = params
            return self
        elif isinstance(params, ActivationParameters):
            if self.active:
                return None
            self.order.append(params)
            self.active = params
            return self
        else:
            return None

    @property
    def fusion_type(self):
        return '_'.join(['linear' if isinstance(params, FcParameters)
                         else 'active' for params in self.order])


class MatchGapLinear(Matcher):
    NAME = 'fuse_gap_linear'
    DESCRIPTION = 'Fuse linear layers and activations to match GAP AutoTiler operations'

    def get_node_list(self, G, params, result=None):
        if result is None:
            result = FusionMatch()
        if not result.add_node(params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, result=result)

    def match(self, G: GraphView, set_identity: bool = True):
        has_modified_graph = False
        for fc_node in [params for params in G.nodes() if isinstance(params, FcParameters)]:
            node_list = self.get_node_list(G, fc_node)
            if node_list is None or len(node_list.order) < 2:
                continue
            LOG.info("fusing nodes %s", ",".join((node.name for node in node_list.order)))
            has_modified_graph = True
            subgraph = GraphView()
            last_node = None
            for node in node_list.order:
                if last_node is not None:
                    subgraph.add_edge(NNEdge(from_node=last_node, to_node=node))
                last_node = node
            input_mapping = [[(node_list.linear, idx)] for idx in range(3)]
            output_mapping = [(last_node, 0)]
            pnode = ConvFusionParameters(
                node_list.linear.name + '_fusion',
                fusion_type=node_list.fusion_type,
                subgraph=subgraph,
                input_mapping=input_mapping,
                output_mapping=output_mapping)
            if G.quantization:
                qrecs = G.quantization.get_all(pnode.contained_nodes())
                if qrecs:
                    prec = None
                    if isinstance(qrecs[0], (SymmetricQuantizationRecord, SymmetricScalableFilterQuantizationRecord)):
                        prec = SymmetricQuantizationRecord(
                            in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                    elif isinstance(qrecs[0], (MultQuantizationRecord, MultScalableFilterQuantizationRecord)):
                        prec = MultQuantizationRecord(in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                    elif isinstance(qrecs[0], (Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord)):
                        prec = Float32QuantizationRecord(
                            in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                    for node in pnode.contained_nodes():
                        G.quantization.move_to_fusion(node, pnode)
                    G.quantization[NodeId(pnode)] = prec
            in_edges = G.in_edges(node_list.linear.name)
            out_edges = G.out_edges(last_node.name)
            for node in node_list.order:
                G.remove(node)
            for edge in in_edges:
                G.add_edge(NNEdge(edge.from_node, pnode, from_idx=edge.from_idx, to_idx=edge.to_idx))
            for edge in out_edges:
                G.add_edge(NNEdge(pnode, edge.to_node, from_idx=edge.from_idx, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
