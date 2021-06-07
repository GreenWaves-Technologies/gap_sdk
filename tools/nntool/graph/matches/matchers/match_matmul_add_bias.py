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

# import logging

# from graph.types import (ActivationParameters, ConstantInputParameters,
#                          MatMulOpFusionParameters, MatMulOpParameters,
#                          MatrixAddParameters, NNEdge)
# from quantization.new_qrec import QRec
# from utils.graph import GraphView
# from utils.node_id import NodeId

# from ..matcher import Matcher, groups, match_name, description

# LOG = logging.getLogger("nntool." + __name__)


# class FusionMatch():
#     def __init__(self) -> None:
#         self.matmul = None
#         self.add = None
#         self.active = None
#         self.order = []

#     def add_node(self, params, G):
#         if isinstance(params, MatMulOpParameters):
#             if self.matmul:
#                 return None
#             self.order.append(params)
#             self.matmul = params
#             return self
#         elif isinstance(params, ActivationParameters):
#             if self.active:
#                 return None
#             self.order.append(params)
#             self.active = params
#             return self
#         elif isinstance(params, MatrixAddParameters):
#             if self.add or self.active:
#                 return None
#             can_fuse = False
#             for in_edge in G.in_edges(params.name):
#                 can_fuse = can_fuse or isinstance(
#                     in_edge.from_node, ConstantInputParameters)
#             if not can_fuse:
#                 return None
#             self.order.append(params)
#             self.add = params
#             return self
#         else:
#             return None

#     @property
#     def fusion_type(self):
#         return '_'.join(['matmul' if isinstance(params, MatMulOpParameters)
#                          else 'with_bias' if isinstance(params, MatrixAddParameters)
#                          else 'active' for params in self.order])

# @groups('*')
# @match_name("fuse_gap_matmul")
# @description('Fuse matmul layers with optional add and/or activations to match GAP AutoTiler operations')
# class MatchMatMulAddBias(Matcher):

#     def get_node_list(self, G, params, result=None):
#         if result is None:
#             result = FusionMatch()
#         if not result.add_node(params, G):
#             return result
#         out_edges = G.out_edges(params.name)
#         if len(out_edges) > 1:
#             return result
#         return self.get_node_list(G, out_edges[0].to_node, result=result)

#     def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
#         has_modified_graph = False
#         for matmul_node in [params for params in G.nodes() if isinstance(params, MatMulOpParameters)]:
#             node_list = self.get_node_list(G, matmul_node)
#             if node_list is None or len(node_list.order) < 2:
#                 continue
#             LOG.info("fusing nodes %s", ",".join(
#                 (node.name for node in node_list.order)))
#             has_modified_graph = True
#             subgraph = GraphView()
#             if node_list.active is not None:
#                 subgraph.add_edge(
#                     NNEdge(from_node=node_list.matmul, to_node=node_list.active))
#             input_mapping = [[(node_list.matmul, idx)] for idx in range(2)]
#             if node_list.add:
#                 input_mapping += [[(node_list.matmul, 2)]]
#             output_mapping = [(node_list.active, 0)] if node_list.active else [
#                 (node_list.matmul, 0)]
#             pnode = MatMulOpFusionParameters(
#                 node_list.matmul.name + '_fusion',
#                 fusion_type=node_list.fusion_type,
#                 subgraph=subgraph,
#                 input_mapping=input_mapping,
#                 output_mapping=output_mapping)
#             if G.quantization:
#                 # if there are quantization stats then clear them. They need to be created again
#                 G.quantization.stats = None
#                 qrecs = G.quantization.get_all(pnode.contained_nodes())
#                 if qrecs:
#                     prec = QRec.copy_ktype(
#                         qrecs[0], in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
#                     for node in pnode.contained_nodes():
#                         G.quantization.move_to_fusion(node, pnode)
#                     G.quantization[NodeId(pnode)] = prec
#             in_edges = G.in_edges(node_list.matmul.name)
#             if node_list.add:
#                 bias_edge = [add_edge for add_edge in G.in_edges(node_list.add.name) if isinstance(
#                     add_edge.from_node, ConstantInputParameters)][0]
#             out_edges = G.out_edges(node_list.order[-1].name)
#             for node in node_list.order:
#                 G.remove(node)
#             for edge in in_edges:
#                 G.add_edge(NNEdge(edge.from_node, pnode,
#                                   from_idx=edge.from_idx, to_idx=edge.to_idx))
#             if node_list.add:
#                 G.add_edge(NNEdge(bias_edge.from_node, pnode,
#                                   from_idx=bias_edge.from_idx, to_idx=2))
#             for edge in out_edges:
#                 G.add_edge(NNEdge(pnode, edge.to_node,
#                                   from_idx=edge.from_idx, to_idx=edge.to_idx))

#         if set_identity:
#             self.set_identity(G)

#         return has_modified_graph
