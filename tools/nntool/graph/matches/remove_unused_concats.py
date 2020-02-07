# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
