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
    NAME = '__NOT_SET__'
    DESCRIPTION = '__NOT_SET__'
    def __init__(self, identity: str = None):
        if identity is None:
            self._identity = self.NAME
        else:
            self._identity = identity

    def set_identity(self, G):
        if hasattr(G, 'graph_identity') and self._identity != '__NOT_SET__':
            G.graph_identity.fusions.append(self._identity)

    @abstractmethod
    def match(self, G: GraphView, set_identity: bool = True):
        pass

class DontReplaceError(Exception):
    pass

class DefaultMatcher(Matcher):
    @abstractmethod
    def match_function(self, G: GraphView) -> Generator[GraphView, None, None]:
        pass

    @abstractmethod
    def replace_function(self, G: GraphView, subgraph: GraphView) -> Node:
        pass

    def match(self, G: GraphView, set_identity: bool = True):
        replaced = True
        while replaced:
            replaced = False
            for subgraph in self.match_function(G):
                try:
                    replacement = self.replace_function(G, subgraph)
                    if replacement is None:
                        G.remove_fragment(subgraph)
                    elif isinstance(replacement, Node):
                        G.replace_fragment(subgraph, replacement)
                    else:
                        raise TypeError("unexcepted return value from replace_function")
                    replaced = True
                    break
                except DontReplaceError:
                    pass

        if set_identity:
            self.set_identity(G)


# This can be used to define groups of matches to be selected
# from the command line
# It also can be inherited to group matches together
class MatchGroup(Matcher):

    def __init__(self, *args: Sequence[Matcher], identity: str = None):
        super().__init__(identity)
        self.matches = list(args)

    def add_match(self, match: Matcher):
        self.matches.append(match)

    def match(self, G: GraphView, set_identity: bool = True):
        for match_instance in self.matches:
            match_instance.match(G, False)
        if set_identity:
            self.set_identity(G)
