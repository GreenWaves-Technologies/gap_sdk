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
import typing
from typing import Sequence
from nntool.utils.graph import GraphView, MatchNode

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph

LOG = logging.getLogger(__name__)


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

    @property
    def run_again(self):
        return self.RUN_AGAIN_ON_MATCH

    @property
    def run_qtune(self):
        return self.RUN_QTUNE_ON_MATCH

    @property
    def run_adjust(self):
        return self.RUN_ADJUST_ON_MATCH

    @staticmethod
    def remove_quantization(G, node):
        if G.quantization:
            nid = node.name
            if nid in G.quantization:
                del G.quantization[nid]

    @abstractmethod
    def _match(self, G: GraphView, **kwargs) -> bool:
        pass

    def match(self, G: GraphView, **kwargs) -> bool:
        return self._match(G, **kwargs)

    @staticmethod
    def match_name(val):
        return Matcher.property_register("NAME", val)

    @staticmethod
    def description(val):
        return Matcher.property_register("DESCRIPTION", val)

    @staticmethod
    def needs_valid_dimension(val):
        return Matcher.property_register("NEEDS_VALID_DIMENSION", val)

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
    def run_qtune_on_match(cls):
        setattr(cls, 'RUN_QTUNE_ON_MATCH', True)
        return cls

    @staticmethod
    def run_adjust_on_match(cls):
        setattr(cls, 'RUN_ADJUST_ON_MATCH', True)
        return cls

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


# This can be used to define groups of matches to be selected
# from the command line
# It also can be inherited to group matches together
class MatchGroup(Matcher):

    def __init__(self, *args: Sequence[Matcher], identity: str = None):
        super().__init__(identity)
        self._matches = {match.name: match for match in args}
        self._matches_pending = []
        self._adjust_pending = False
        self._qtune_pending = False

    @property
    def run_again(self):
        return self._matches_pending

    @property
    def run_qtune(self):
        return self._qtune_pending

    @property
    def run_adjust(self):
        return self._adjust_pending

    @property
    def names(self):
        return list(self._matches)

    def exclude_matches(self, *match_names: Sequence[str]):
        for name in match_names:
            if name in self._matches:
                del self._matches[name]
        for match in self._matches.values():
            if isinstance(match, MatchGroup):
                match.exclude_matches(*match_names)

    def add_match(self, match: Matcher):
        self._matches.append(match)

    def _match(self, G: 'NNGraph', **kwargs):
        # Note: assumption is that dimensions are valid when a match is called
        found_match = True
        dimensions_set = True
        self._matches_pending = []
        self._adjust_pending = False
        self._qtune_pending = False
        any_found_match = False
        while found_match:
            found_match = False
            matches = list(self._matches.values())
            while matches:
                match = matches.pop(0)
                # if this match was queued by another match remove it since we run here
                if match.name in self._matches_pending:
                    self._matches_pending.remove(match.name)
                LOG.debug("fusions - start %s", match.name)
                if match.NEEDS_VALID_DIMENSION and not dimensions_set:
                    G.add_dimensions(quiet=True)
                    dimensions_set = True
                has_modified_graph = match.match(
                    G, group_identity=self._identity)
                if has_modified_graph:
                    LOG.info("++ fusion %s modified graph", match.name)
                    found_match = True
                    G.add_dimensions(quiet=True)
                    dimensions_set = True
                    for required_match in match.run_again:
                        if match not in self._matches_pending:
                            self._matches_pending.append(required_match)
                    self._adjust_pending = self._adjust_pending or match.run_adjust
                    if G.quantization:
                        self._qtune_pending = self._qtune_pending or match.run_qtune

                if dimensions_set and has_modified_graph:
                    dimensions_set = False
            if found_match:
                any_found_match = True 

        return any_found_match
