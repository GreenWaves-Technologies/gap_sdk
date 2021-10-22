# # This program is free software: you can redistribute it and/or modify
# # it under the terms of the GNU Affero General Public License as
# # published by the Free Software Foundation, either version 3 of the
# # License, or (at your option) any later version.

# # This program is distributed in the hope that it will be useful,
# # but WITHOUT ANY WARRANTY; without even the implied warranty of
# # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# # GNU Affero General Public License for more details.

# # You should have received a copy of the GNU Affero General Public License
# # along with this program.  If not, see <https://www.gnu.org/licenses/>.

# from graph.types import RNNBaseParameters
# from utils.graph import GraphView
# from utils.node_id import NodeId

# from ..matcher import Matcher, groups, match_name, description
# from .equalize_sym_mult_concats import propagate_qtype_up


# @groups('scaled')
# @match_name("propagate_up_rnn_in_qs")
# @description("After quantization of rnn their in_q and out_q are the same "
#              "so in_q may be changed and we need to propagate it up")
# class PropagateUpRNNInputQ(Matcher):

#     def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
#         if not G.quantization:
#             return
#         rnns = [node for node in G.nodes() if isinstance(
#             node, RNNBaseParameters)]
#         qrecs = [G.quantization[NodeId(node)] for node in rnns]
#         for rnn, qrec in zip(rnns, qrecs):
#             in_idx = rnn.INPUT_NAMES.index('input')
#             in_edge = [edge for edge in G.in_edges(
#                 rnn.name) if edge.to_idx == in_idx][0]
#             in_q = qrec.in_qs[in_idx]
#             propagate_qtype_up(G, in_q, in_edge)

#         if set_identity:
#             self.set_identity(G)

#         return False
