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
from abc import abstractclassmethod

from graph.nngraph import NNGraph
from graph.types import (ActivationFusion, ActivationParameters,
                         GlobalPoolParameters, MatrixAddParameters,
                         MatrixMulParameters, PoolingParameters)
from quantization.float32.float32_quantization import (
    Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord)
from quantization.multiplicative.mult_quantization import (
    MultQuantizationRecord, MultScalableFilterQuantizationRecord)
from quantization.symmetric.symmetric_quantization import (
    SymmetricQuantizationRecord, SymmetricScalableFilterQuantizationRecord)
from utils.graph import Edge, GraphView, MatchNode
from utils.node_id import NodeId

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)


class MatchOpActivation(DefaultMatcher):

    @abstractclassmethod
    def valid_node_classes(cls):
        pass

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0',
                               matcher=lambda node:
                               isinstance(node, self.valid_node_classes())))
        sub.add_node(MatchNode('1', matcher=lambda node:
                               isinstance(node, ActivationParameters)))
        sub.add_edge(Edge('0', '1'))
        return G.match_fragment(sub)

    def replace_function(self, G: NNGraph, subgraph: GraphView):
        nodes = list(subgraph.nodes())
        # map all inputs of first node to first node
        input_mapping = [[(nodes[0], idx)] for idx in range(len(G.in_edges(nodes[0].name)))]
        pnode = ActivationFusion(nodes[0].name + "fusion",
                                 fusion_type=nodes[0].op_name + "_active",
                                 subgraph=subgraph,
                                 input_mapping=input_mapping
                                 )
        nodes[0].step_idx = 0
        nodes[1].step_idx = 1
        LOG.debug("fused nodes %s", ",".join(
            (node.name for node in nodes)))
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


class MatchOpActivationScaleKernels(MatchOpActivation):
    NAME = 'fuse_op_activation_scale8'
    DESCRIPTION = 'Fuse non-filter nodes and activations to match GAP AutoTiler SQ8 kernels'
    @classmethod
    def valid_node_classes(cls):
        return (PoolingParameters, GlobalPoolParameters, MatrixAddParameters, MatrixMulParameters)


class MatchOpActivationPow2Kernels(MatchOpActivation):
    NAME = 'fuse_op_activation_pow2'
    DESCRIPTION = 'Fuse non-filter nodes and activations to match GAP AutoTiler POW2 kernels'
    @classmethod
    def valid_node_classes(cls):
        return (PoolingParameters, MatrixAddParameters, MatrixMulParameters)
