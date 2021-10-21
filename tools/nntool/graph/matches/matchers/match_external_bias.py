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

import numpy as np
from graph.types import (ConstantInputParameters, FilterParameters,
                         MatrixAddParameters, MatrixMulParameters, NNEdge)
from graph.types.others import ReshapeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, groups, run_before

LOG = logging.getLogger("nntool." + __name__)

# TODO - verify with quantized constants

OPS = {
    MatrixAddParameters: (np.add, False),
    MatrixMulParameters: (np.multiply, True)
}

@match_name('fuse_external_bias')
@description('Fuse bias addition after filter with filter bias')
@groups('scaled', 'symmetric')
@run_before('match_op_activation', 'move_pooling_scale8', 'move_activations_pow2', 'move_activations_scale8')
class MatchExternalBias(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        filter_nodes = [node for node in G.nodes(
        ) if isinstance(node, FilterParameters)]
        for params in filter_nodes:
            filter_node = params
            seen_reshape = []
            while True:
                out_edges = G.out_edges(params.name)
                # can't fuse if there is a branch
                if len(out_edges) > 1:
                    break
                out_edge = out_edges[0]
                op_node = out_edge.to_node
                if isinstance(op_node, ReshapeParameters):
                    seen_reshape = [op_node]
                    params = op_node
                    continue
                # must be a valid matrix op
                if not isinstance(op_node, tuple(OPS.keys())):
                    break
                # other edge to the op must be a constant
                other_idx = 1 if out_edge.to_idx == 0 else 0
                other_in_edge = G.indexed_in_edges(op_node.name)[other_idx]
                if not isinstance(other_in_edge.from_node, ConstantInputParameters):
                    break
                const_node = other_in_edge.from_node
                remove_constant = len(G.out_edges(const_node.name))

                flat_value = const_node.dqvalue.flatten()
                out_c = filter_node.filter.out_c
                op, weights_and_biases = OPS[op_node.__class__]
                # it would be possible to support mult bias addition by out channel but only supporting a
                # scalar at present
                if len(flat_value) != 1 and (weights_and_biases or len(flat_value) != out_c):
                    LOG.warning('could not absorb %s into %s',
                                const_node.name, filter_node.name)
                    break
                # If there is quantization then essentially the output of the filter
                # takes the quantization of the output of the operation.
                # The biases will not change since their quantization depends on the weights
                # and input
                fnid = NodeId(filter_node)
                opnid = NodeId(op_node)
                if G.quantization and (fnid in G.quantization or opnid in G.quantization):
                    if not (fnid in G.quantization and opnid in G.quantization):
                        LOG.warning(
                            'could not absorb %s into %s - graph is partially quantized', const_node.name, filter_node.name)
                        break
                    fqrec = G.quantization[fnid]
                    opqrec = G.quantization[opnid]
                    fqrec.out_qs[0] = opqrec.out_qs[0]

                has_modified_graph = True
                LOG.info("fusing bias in %s into %s",
                         const_node.name, filter_node.name)
                self.fuse_bias(G, filter_node, other_idx, op, flat_value, 2)
                if weights_and_biases:
                    # TODO - need to adjust weights quantization here
                    LOG.info("fusing multiplicative bias in %s into %s",
                             const_node.name, filter_node.name)
                    self.fuse_bias(G, filter_node, other_idx,
                                   op, flat_value, 1)

                out_edges = G.out_edges(op_node.name)
                G.remove(op_node)
                if remove_constant:
                    G.remove(const_node)
                from_node = seen_reshape[-1] if seen_reshape else filter_node
                for edge in out_edges:
                    G.add_edge(NNEdge(from_node=from_node,
                                      to_node=edge.to_node, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph

    @staticmethod
    def fuse_bias(G, filter_node, other_idx, op, flat_value, idx):
        edge = G.indexed_in_edges(filter_node.name)[idx]
        node = edge.from_node
        # flat value is dequantized so we operate on the dequantized value
        # on the weights or biases node
        if other_idx == 0:
            node.dqvalue = op(flat_value, node.dqvalue)
        else:
            node.dqvalue = op(node.dqvalue, flat_value)
