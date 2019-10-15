# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod
from typing import Generator, Sequence

from graph.graph_identity import GraphIdentity
from utils.graph import GraphView, Node, MatchNode

class MatchNodeType(MatchNode):
    def __init__(self, name, node_class):
        super().__init__(name)
        self.__node_class = node_class

    def _match(self, G, node, edge):
        del G, edge
        return isinstance(node, self.__node_class)

class Matcher(ABC):
    @abstractmethod
    def match(self, G: GraphView):
        pass

class DefaultMatcher(Matcher):
    @abstractmethod
    def match_function(self, G: GraphView) -> Generator[GraphView, None, None]:
        pass

    @abstractmethod
    def replace_function(self, G: GraphView, subgraph: GraphView) -> Node:
        pass

    def match(self, G: GraphView):
        replaced = True
        while replaced:
            replaced = False
            for subgraph in self.match_function(G):
                replacement = self.replace_function(G, subgraph)
                if not replacement:
                    G.remove_fragment(subgraph)
                elif isinstance(replacement, Node):
                    G.replace_fragment(subgraph, replacement)
                else:
                    raise TypeError("unexcepted return value from replace_function")
                replaced = True
                break

# This can be used to define groups of matches to be selected
# from the command line
# It also can be inherited to group matches together
class MatchGroup(Matcher):

    def __init__(self, *args: Sequence[Matcher], identity: GraphIdentity = None):
        self.matches = list(args)
        self.identity = identity

    def add_match(self, match: Matcher):
        self.matches.append(match)

    def match(self, G: GraphView):
        for match_instance in self.matches:
            match_instance.match(G)
        if self.identity is not None:
            G.graph_identity.fusions.append(self.identity)
