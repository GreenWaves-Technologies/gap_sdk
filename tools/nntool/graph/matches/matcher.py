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
from abc import ABC, abstractmethod
from typing import Generator, Sequence
from utils.node_id import NodeId

from utils.graph import GraphView, MatchNode, Node

LOG = logging.getLogger("nntool." + __name__)


class MatchNodeType(MatchNode):
    def __init__(self, name, node_class):
        super().__init__(name)
        self.__node_class = node_class

    def _match(self, G, node, edge):
        del G, edge
        return isinstance(node, self.__node_class)


class Matcher(ABC):
    NAME = None
    DESCRIPTION = None
    NEEDS_VALID_DIMENSION = False
    MODIFIES_DIMENSIONS = True
    RUN_BEFORE = []
    RUN_AFTER = []
    GROUPS = []
    RUN_AGAIN_ON_MATCH = []
    RUN_QTUNE_ON_MATCH = False
    RUN_ADJUST_ON_MATCH = False

    def __init__(self, identity: str = None):
        if identity is None:
            self._identity = self.NAME
        else:
            self._identity = identity

    @property
    def name(self):
        return self.NAME

    @staticmethod
    def remove_quantization(G, node):
        if G.quantization:
            nid = NodeId(node)
            if nid in G.quantization:
                del G.quantization[nid]

    def set_identity(self, G):
        if hasattr(G, 'graph_identity') and self._identity is not None:
            G.graph_identity.fusions.append(self._identity)

    @abstractmethod
    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        pass

    def match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        return self._match(G, set_identity, **kwargs)

    @staticmethod
    def match_name(val):
        return Matcher.property_register("NAME", val)

    @staticmethod
    def description(val):
        return Matcher.property_register("DESCRIPTION", val)

    @staticmethod
    def needs_valid_dimension(val):
        return Matcher.property_register("DESCRIPTION", val)

    @staticmethod
    def modifies_dimensions(val):
        return Matcher.property_register("MODIFIES_DIMENSIONS", val)

    @staticmethod
    def run_before(*args):
        return Matcher.property_register("RUN_BEFORE", args)

    @staticmethod
    def run_after(*args):
        return Matcher.property_register("RUN_AFTER", args)

    @staticmethod
    def run_again_on_match(*args):
        return Matcher.property_register("RUN_AGAIN_ON_MATCH", args)

    @staticmethod
    def run_qtune_on_match(val):
        return Matcher.property_register("RUN_QTUNE_ON_MATCH", val)

    @staticmethod
    def run_adjust_on_match(val):
        return Matcher.property_register("RUN_ADJUST_ON_MATCH", val)

    @staticmethod
    def groups(*args):
        return Matcher.property_register("GROUPS", args)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

match_name = Matcher.match_name
description = Matcher.description
needs_valid_dimension = Matcher.needs_valid_dimension
modifies_dimensions = Matcher.modifies_dimensions
run_before = Matcher.run_before
run_after = Matcher.run_after
run_again_on_match = Matcher.run_again_on_match
run_qtune_on_match = Matcher.run_qtune_on_match
run_adjust_on_match = Matcher.run_adjust_on_match

groups = Matcher.groups

class DontReplaceError(Exception):
    pass


class DefaultMatcher(Matcher):
    @abstractmethod
    def match_function(self, G: GraphView) -> Generator[GraphView, None, None]:
        pass

    @abstractmethod
    def replace_function(self, G: GraphView, subgraph: GraphView) -> Node:
        pass

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        replaced = True
        has_modified_graph = False
        while replaced:
            replaced = False
            for subgraph in self.match_function(G):
                # TODO - Save in and out edges here since the replace function may modify the
                # subgraph
                in_edges = [in_edge for input_node in subgraph.inputs()
                            for in_edge in G.in_edges(input_node.name)]
                out_edges = [out_edge for output_node in subgraph.outputs()
                             for out_edge in G.out_edges(output_node.name)]
                try:
                    replacement, edge_in_mapping, edge_out_mapping = self.replace_function(
                        G, subgraph)
                    if replacement is None:
                        G.remove_fragment(subgraph)
                        has_modified_graph = True
                    elif isinstance(replacement, Node):
                        # use saved  in and out edges
                        G.replace_fragment(subgraph,
                                           replacement,
                                           frag_in_edges=in_edges,
                                           frag_out_edges=out_edges,
                                           edge_in_mapping=edge_in_mapping,
                                           edge_out_mapping=edge_out_mapping)
                        has_modified_graph = True
                    else:
                        raise TypeError(
                            "unexcepted return value from replace_function")
                    replaced = True
                    break
                except DontReplaceError:
                    pass

        if set_identity:
            self.set_identity(G)

        return has_modified_graph


# This can be used to define groups of matches to be selected
# from the command line
# It also can be inherited to group matches together
class MatchGroup(Matcher):

    def __init__(self, *args: Sequence[Matcher], identity: str = None):
        super().__init__(identity)
        self.matches = list(args)

    def add_match(self, match: Matcher):
        self.matches.append(match)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        # Note: assumption is that dimensions are valid when a match is called
        found_match = True
        dimensions_set = True
        while found_match:
            found_match = False
            for match_instance in self.matches:
                LOG.debug("fusions - start %s", match_instance.name)
                if match_instance.NEEDS_VALID_DIMENSION and not dimensions_set:
                    G.add_dimensions(quiet=True)
                    dimensions_set = True
                has_modified_graph = match_instance.match(
                    G, set_identity=False, group_identity=self._identity)
                if has_modified_graph:
                    LOG.info("++ fusion %s modified graph", match_instance.name)
                    found_match = True
                    G.add_dimensions(quiet=True)
                if dimensions_set and has_modified_graph:
                    dimensions_set = False
        if set_identity:
            self.set_identity(G)


def find_forward(G: GraphView, edge, find_node_classes, skip_node_classes=None, find_skip=None):
    if find_skip is None:
        find_skip = [find_node_classes, skip_node_classes]
        for idx, elem in enumerate(find_skip):
            if elem is not None and not isinstance(elem, tuple):
                if isinstance(elem, list):
                    find_skip[idx] = tuple(elem)
                else:
                    find_skip[idx] = tuple([elem])
    if isinstance(edge.to_node, find_skip[0]):
        return [[edge]]
    if skip_node_classes and isinstance(edge.to_node, find_skip[0]):
        res = []
        for out_edge in G.out_edges(edge.to_node.name):
            edge_lists = find_forward(G, out_edge, find_node_classes,
                                      find_skip=find_skip)
            if not edge_lists:
                continue
            res.extend([[edge] + edge_list for edge_list in edge_lists])
        return res
    return []
