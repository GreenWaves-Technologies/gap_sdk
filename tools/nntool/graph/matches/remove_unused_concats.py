# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from utils.graph import GraphView, MatchNode, Node, Edge
from graph.types import ConcatParameters
from .matcher import DefaultMatcher

class ConcatMatcher(MatchNode):
    def _match(self, G: GraphView, node: Node, edge: Edge):
        return isinstance(node, ConcatParameters) and G.num_in_edges(node.name) == 1

class RemoveUnusedConcats(DefaultMatcher):
    NAME = "unused_concats"
    DESCRIPTION = "Remove concats that only have one input"
    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(ConcatMatcher('0'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        return None
