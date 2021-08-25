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


from abc import ABC, abstractmethod
from inspect import isclass


class NodeMatcher(ABC):
    def __init__(self, *args) -> None:
        self._contents = args

    def reset(self):
        for matcher in self._contents:
            if isinstance(matcher, NodeMatcher):
                matcher.reset()

    def find(self, G, start_nodes=None):
        if start_nodes is None:
            start_nodes = G.nodes()
        start_nodes = set(start_nodes)
        res = []
        matched_nodes = set()
        while start_nodes:
            node = start_nodes.pop()
            self.reset()
            inc_match = False
            matched = None
            while not inc_match:
                matched, inc_match, inc_node = self._matches(G, node)
                if matched is None:
                    break
                if inc_node:
                    out_edges = G.out_edges(node.name)
                    if len(out_edges) != 1:
                        break
                    node = out_edges[0].to_node
            else:
                res.append(matched)
                matched_nodes |= set(matched)
                # don't match a node more than once
                start_nodes -= matched_nodes
        return res

    def matches(self, G, elem):
        return self._matches(G, elem)

    @abstractmethod
    def _matches(self, G, elem):
        """ Tries to match elem. Return list(matches), continue to next """


    @staticmethod
    def do_match(G, match_elem, elem):
        if isinstance(match_elem, NodeMatcher):
            return match_elem.matches(G, elem)
        if (isclass(match_elem) and isinstance(elem, match_elem)) or elem == match_elem:
            return [elem], True, True
        return None, None, None

class Sequence(NodeMatcher):
    def __init__(self, *args) -> None:
        super().__init__(*args)
        self._match_idx = 0
        self._match = []

    def reset(self):
        super().reset()
        self._match_idx = 0
        self._match = []

    def _matches(self, G, elem):
        assert self._match_idx < len(self._contents)
        while True:
            matched, inc_match, inc_node = self.do_match(G, self._contents[self._match_idx], elem)
            if matched is None:
                return None, None, None
            if matched:
                self._match.extend(matched)
            if inc_match:
                self._match_idx += 1
            if self._match_idx >= len(self._contents):
                return self._match, True, True
            if not inc_node:
                continue
            return [], False, True

class Optional(NodeMatcher):
    def _matches(self, G, elem):
        matched, _, _ = self.do_match(G, self._contents[0], elem)
        if matched:
            return matched, True, True
        else:
            return [], True, False

class AnyOf(NodeMatcher):
    def _matches(self, G, elem):
        for matcher in self._contents:
            matched, _, _ = self.do_match(G, matcher, elem)
            if matched:
                return matched, True, True
        return None, None, None

class OneOrMoreOf(AnyOf):
    def __init__(self, *args) -> None:
        super().__init__(*args)
        self._matched_first = False
        self._match = []

    def reset(self):
        super().reset()
        self._matched_first = False
        self._match = []

    def _matches(self, G, elem):
        if not self._matched_first:
            matched, _, _ = self.do_match(G, self._contents[0], elem)
            if matched is None:
                return None, None, None
            self._matched_first = True
        else:
            matched, _, _ = self.do_match(G, self._contents[0], elem)
            if matched is None:
                return self._match, True, False
        self._match.extend(matched)
        return [], False, True
