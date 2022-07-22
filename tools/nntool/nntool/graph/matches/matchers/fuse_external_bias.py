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
from nntool.graph.types import (ConstantInputNode, FilterNodeBase,
                         MatrixAddNode, MatrixMulNode, NNEdge, ReshapeNode)
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, description, groups, run_before, run_qtune_on_match

LOG = logging.getLogger(__name__)

# TODO - verify with quantized constants

OPS = {
    MatrixAddNode: (np.add, False),
    MatrixMulNode: (np.multiply, True)
}


@match_name('fuse_external_bias')
@description('Fuse bias addition after filter with filter bias')
@groups('scaled', 'symmetric')
@run_before('fuse_gap_convs', 'fuse_op_activation_scale8', 'fuse_op_activation_pow2', 'move_pooling_scale8',
            'fuse_gap_linear',
            'move_activations_up')
@run_qtune_on_match
class MatchExternalBias(Matcher):

    def _match(self, G: GraphView, **kwargs):
        has_modified_graph = False
        filter_nodes = [node for node in G.nodes(
        ) if isinstance(node, FilterNodeBase)]
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
                if isinstance(op_node, ReshapeNode):
                    seen_reshape = [op_node]
                    params = op_node
                    continue
                # must be a valid matrix op
                if not isinstance(op_node, tuple(OPS.keys())):
                    break
                # other edge to the op must be a constant
                other_idx = 1 if out_edge.to_idx == 0 else 0
                other_in_edge = G.indexed_in_edges(op_node.name)[other_idx]
                if not isinstance(other_in_edge.from_node, ConstantInputNode):
                    break
                const_node = other_in_edge.from_node
                remove_constant = len(G.out_edges(const_node.name))

                flat_value = const_node.dqvalue.flatten()
                out_c = filter_node.filter_dim.out_c
                op, weights_and_biases = OPS[op_node.__class__]
                # it would be possible to support mult bias addition by out channel but only supporting a
                # scalar at present
                if len(flat_value) != 1 and (weights_and_biases or len(flat_value) != out_c):
                    LOG.warning('could not absorb %s into %s',
                                const_node.name, filter_node.name)
                    break

                has_modified_graph = True
                LOG.info("fusing bias in %s into %s",
                         const_node.name, filter_node.name)
                self.fuse_bias(G, filter_node, other_idx, op, flat_value, 2)
                if weights_and_biases:
                    LOG.info("fusing multiplicative bias in %s into %s",
                             const_node.name, filter_node.name)
                    self.fuse_bias(G, filter_node, other_idx,
                                   op, flat_value, 1)

                # save out edges and remove the mul or add
                out_edges = G.out_edges(op_node)
                G.remove(op_node)
                if remove_constant:
                    G.remove(const_node)

                # it's possible that there is a broadcast on the op from the constant
                # if there is insert a reshape since it will no longer happen
                in_shape = tuple(op_node.in_dims[out_edge.to_idx].shape)
                out_shape = tuple(op_node.out_dims[0].shape)
                from_node = seen_reshape[-1] if seen_reshape else filter_node
                if in_shape != out_shape:
                    reshape = ReshapeNode(G.unique_name(f'{op_node.name}_reshape'),
                                                old_shape=in_shape, shape=out_shape)
                    G.add_edge(NNEdge(from_node=from_node, to_node=reshape))
                    from_node = reshape

                # connect up the output nodes
                for edge in out_edges:
                    G.add_edge(NNEdge(from_node=from_node,
                                      to_node=edge.to_node, to_idx=edge.to_idx))



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
