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

from utils.graph import GraphView, MatchNode, Node, Edge
from graph.types import ConcatParameters
from .matcher import DefaultMatcher


class ConcatMatcher1(MatchNode):
    def _match(self, G: GraphView, node: Node, edge: Edge):
        return isinstance(node, ConcatParameters) and G.num_in_edges(node.name) == 1


class RemoveUnusedConcats(DefaultMatcher):
    NAME = "unused_concats"
    DESCRIPTION = "Remove concats that only have one input"

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(ConcatMatcher1('0'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        return None, None, None
