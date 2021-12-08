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

from graph.types.others import ReshapeParameters, TransposeParameters
import logging

from graph.dim import Dim
from graph.matches.matchers.remove_unnecessary_quantize_operators import \
    RemoveUnnecessaryQuantizeOperators
from graph.types import (ConstantInputParameters, MatrixAddParameters,
                         MatrixMulParameters, NNEdge)
from graph.types.tensor_arithmetic import MatMulOpParameters
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger("nntool." + __name__)


def reverse_matmul(G: GraphView, params):
    # reverse edges
    in_edges = G.indexed_in_edges(params.name)
    for edge in in_edges[0:2:]:
        G.remove_edge(edge)
    other_idx = 1
    for edge in in_edges[0:2:]:
        G.add_edge(NNEdge(from_node=edge.from_node, to_node=params,
                          from_idx=edge.from_idx, to_idx=other_idx))
        other_idx = 1 - other_idx
    nid = NodeId(params)
    if G.quantization and nid in G.quantization:
        qrec = G.quantization[nid]
        # swap qrecs
        qrec.in_qs[0], qrec.in_qs[1] = qrec.in_qs[1], qrec.in_qs[0]

    # add transposes
    in_nodes = []
    for idx in range(2):
        tin_params = TransposeParameters(G.unique_name(
            f"{params.name}_tin{idx}"), transpose=(1, 0))
        in_nodes.append(tin_params)
        G.insert_node_before(tin_params, params, to_idx=idx, edge_class=NNEdge)
    tout_params = TransposeParameters(G.unique_name(
        f"{params.name}_tout"), transpose=(1, 0))
    G.insert_node_after(params, tout_params)
    return in_nodes, tout_params

# this is largely a copy of the filter versiot but there are some subtle differences


@match_name('fuse_external_bias_matmul')
@description('Fuse bias addition after matmul')
@groups('scaled', 'symmetric')
@run_before('match_op_activation', 'move_pooling_scale8', 'move_activations_pow2', 'move_activations_scale8', 'fuse_op_activation_scale8', 'fuse_op_activation_pow2')
class MatchExternalBiasMatmul(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        has_transposed = False
        for params in G.nodes(node_classes=MatMulOpParameters):
            matmul = params
            seen_reshape = []
            while True:
                out_edges = G.out_edges(params.name)
                # can't fuse if there is a branch
                if len(out_edges) > 1:
                    break
                out_edge = out_edges[0]
                op_node = out_edge.to_node
                if isinstance(op_node, ReshapeParameters):
                    seen_reshape.append(op_node)
                    params = op_node
                    continue
                # must be a valid matrix op
                if not isinstance(op_node, (MatrixAddParameters, MatrixMulParameters)):
                    break
                # other edge to the op must be a constant
                other_idx = 1 if out_edge.to_idx == 0 else 0
                other_in_edge = G.indexed_in_edges(op_node.name)[other_idx]
                if not isinstance(other_in_edge.from_node, ConstantInputParameters):
                    break
                const_node = other_in_edge.from_node
                remove_constant = len(G.out_edges(const_node.name))

                flat_value = const_node.dqvalue.flatten()
                out_shape = matmul.out_dims[0].shape
                if len(out_shape) != 2:
                    raise ValueError(
                        f'strange outputs shape of {out_shape} for matmul {params.name}')
                if len(flat_value) != out_shape[0] and len(flat_value) != out_shape[1]:
                    LOG.info("can't fuse %s into %s - value shape is not correct for bias",
                             const_node.name, matmul.name)
                    break
                has_bias = len(matmul.in_dims) == 3
                in_nodes = [matmul]
                out_node = seen_reshape[-1] if seen_reshape else matmul
                if isinstance(op_node, MatrixAddParameters):
                    if has_bias:
                        if len(flat_value.shape) != len(matmul.in_dims[2]):
                            LOG.info(
                                "can't fuse %s into %s - bias shape is not the same", const_node.name, matmul.name)
                            break
                        bias_node = G.indexed_in_edges(matmul.name)[
                            2].from_node
                        LOG.info(
                            "folding additive bias from %s into existing bias on %s", op_node.name, matmul.name)
                        bias_node.value = bias_node.dqvalue + flat_value
                    else:
                        if len(flat_value) != out_shape[1]:
                            # matmul needs to be transposed to fuse this
                            in_nodes, trans_node = reverse_matmul(G, matmul)
                            if seen_reshape:
                                out_node = seen_reshape[-1]
                            else:
                                out_node = trans_node
                            has_transposed = True
                        bias_node = ConstantInputParameters(
                            G.unique_name(f'{matmul.name}_bias'),
                            value=flat_value, dims=Dim.unnamed(flat_value.shape))
                        G.add_edge(NNEdge(from_node=bias_node,
                                          to_node=matmul, to_idx=2))
                        LOG.info(
                            "folding additive bias from %s into new bias on %s", op_node.name, matmul.name)
                else:
                    params_in = G.indexed_in_edges(matmul.name)
                    consts = [isinstance(
                        edge.from_node, ConstantInputParameters) for edge in params_in]
                    if not any(consts):
                        break
                    mult_const_node = params_in[1].from_node if consts[1] else params_in[0].from_node
                    mult_const_node.value = mult_const_node.dqvalue * const_node.dqvalue
                    if has_bias:
                        bias_node = params_in[2].from_node
                        bias_node.value = bias_node.dqvalue * const_node.dqvalue

                    LOG.info(
                        "folding multaplicative bias from %s into new bias on %s", op_node.name, matmul.name)

                out_edges = G.out_edges(op_node.name)
                G.remove(op_node)
                if remove_constant:
                    G.remove(const_node)
                for edge in out_edges:
                    G.add_edge(NNEdge(from_node=out_node,
                                      to_node=edge.to_node, to_idx=edge.to_idx))
                G.add_dimensions()
                if G.quantization:
                    quantizer = NewQuantizer.from_quantized_graph(G)
                    quantizer.quantize()
                    RemoveUnnecessaryQuantizeOperators().match(G)

        if has_transposed:
            G.adjust_order()

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
