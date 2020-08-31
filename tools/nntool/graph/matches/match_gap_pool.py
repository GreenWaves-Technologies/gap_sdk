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
                         PoolingParameters)
from quantization.symmetric.symmetric_quantization import (
    SymmetricQuantizationRecord, SymmetricScalableFilterQuantizationRecord)
from quantization.multiplicative.mult_quantization import (
    MultQuantizationRecord, MultScalableFilterQuantizationRecord)
from quantization.float32.float32_quantization import (
    Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord)
from utils.graph import Edge, GraphView, MatchNode
from utils.node_id import NodeId

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)


class MatchGapPool(DefaultMatcher):
    NAME = 'fuse_gap_pool'
    DESCRIPTION = 'Fuse pooling layers and activations to match GAP AutoTiler operations'

    def valid_pool(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def valid_activation(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0',
                               matcher=lambda node:
                               isinstance(node, PoolingParameters) and
                               self.valid_pool(node)))
        sub.add_node(MatchNode('1', matcher=lambda node:
                               isinstance(node, ActivationParameters) and
                               self.valid_activation(node)))
        sub.add_edge(Edge('0', '1'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        step = 0
        for node in subgraph.nodes():
            node.step_idx = step
            step = step + 1
            if isinstance(node, PoolingParameters):
                pool_name = node.name + "_fusion"
                break
        LOG.debug("fused nodes %s", ",".join(
            (node.name for node in subgraph.nodes())))
        # simple node order is necessary because nodes() will not necessarily
        # be in order
        pnode = ConvFusionParameters(pool_name, fusion_type="pool_active", subgraph=subgraph)
        if G.quantization:
            qrecs = G.quantization.get_all(pnode.contained_nodes())
            if qrecs:
                if isinstance(qrecs[0], (SymmetricQuantizationRecord, SymmetricScalableFilterQuantizationRecord)):
                    prec = SymmetricQuantizationRecord(
                        in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                elif isinstance(qrecs[0], (MultQuantizationRecord, MultScalableFilterQuantizationRecord)):
                    prec = MultQuantizationRecord(in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                elif isinstance(qrecs[0], (Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord)):
                    prec = Float32QuantizationRecord(in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                for node in pnode.contained_nodes():
                    G.quantization.move_to_fusion(node, pnode)
                G.quantization[NodeId(pnode)] = prec
        return pnode, None, None
