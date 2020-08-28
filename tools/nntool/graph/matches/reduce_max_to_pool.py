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

# from graph.types import ReduceMaxParameters, GlobalPoolParameters
# from utils.graph import GraphView
# from .matcher import DefaultMatcher, MatchNode, DontReplaceError

# class MatchReduceMax(DefaultMatcher):
#     NAME = 'match_reduce_max_nodes'
#     DESCRIPTION = 'Match reduce max nodes and replace them with GlobalMaxPooling'

#     def match_function(self, G: GraphView):
#         sub = GraphView()
#         sub.add_node(MatchNode('0', matcher=lambda node:
#                                isinstance(node, ReduceMaxParameters)))
#         return G.match_fragment(sub)

#     def replace_function(self, G: GraphView, subgraph: GraphView):
#         reduce_max_node = list(subgraph.nodes())[0]

#         for idx, in_dim in enumerate(reduce_max_node.in_dims[0].shape):
#             if idx > 0 and idx not in reduce_max_node.axis and in_dim == 1:
#                 raise DontReplaceError()
#         return GlobalPoolParameters(reduce_max_node.name + "_GLOBAL_MAXPOOL", pool_type='max',
#                                     in_dims_hint=reduce_max_node.in_dims_hint,
#                                     out_dims_hint=reduce_max_node.out_dims_hint), None, None
