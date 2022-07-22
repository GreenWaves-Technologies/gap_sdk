# Copyright (C) 2020, 2022  GreenWaves Technologies, SAS

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
from copy import deepcopy

from nntool.graph.types import (ConcatNode, ConstantInputNode,
                         CopyNode, InputNode, NNEdge,
                         OutputNode, RNNNodeBase, SplitNode)
from nntool.quantization.new_qrec import QRec
from nntool.utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name,
                       needs_valid_dimension)

LOG = logging.getLogger(__name__)


class VisitEdge():
    def __init__(self, edge, direction) -> None:
        self.edge = edge
        self.direction = direction

    def __eq__(self, other: object) -> bool:
        return isinstance(other, VisitEdge) and self.edge == other.edge

    def __hash__(self) -> int:
        return self.edge.__hash__()


@match_name('insert_copies')
@description('insert copy nodes on edges that link stacked tensors between themselves or to inputs or outputs')
@groups('*')
@needs_valid_dimension(True)
class MatchInsertCopies(Matcher):
    @staticmethod
    def can_pass_node(node):
        # nodes that do not generate any kernel
        return node.exclude_from_generation

    def find_split_concat_down(self, G, edge):
        # search for a split or concat on any downward edge
        if isinstance(edge.to_node, (SplitNode, ConcatNode)) and edge.to_node.no_model_code:
            return True
        elif self.can_pass_node(edge.to_node):
            for out_edge in G.out_edges(edge.to_node):
                if self.find_split_concat_down(G, out_edge):
                    return True
        return False

    def search_up_for_duplicate(self, G, edge):
        # search up for multi out edges on the same edge idx
        # if found then search down for a concat or split ignoring ungenerated nodes
        # return the original out edge where the concat/split was found
        out_edges = G.indexed_out_edges(edge.from_node)[edge.from_idx]
        if len(out_edges) > 1:
            out_edges.remove(edge)
            for out_edge in out_edges:
                if self.find_split_concat_down(G, out_edge):
                    return out_edge
        elif self.can_pass_node(edge.from_node):
            return self.search_up_for_duplicate(G, G.in_edges(edge.from_node)[0])
        return None

    @staticmethod
    def insert_copy_at_edge(G, edge):
        copy_node = CopyNode(
            G.unique_name(f'{edge.from_node.name}_copy'))
        LOG.info(f'inserting copy between {edge.from_node.name}:{edge.from_idx} '
                 f'and {edge.to_node.name}:{edge.to_idx}')
        G.insert_node_at_edge(copy_node, edge, edge_class=NNEdge)
        nid = edge.from_node.name
        if G.quantization and nid in G.quantization:
            qrec = G.quantization[nid]
            qtype = deepcopy(qrec.out_qs[edge.from_idx])
            G.quantization[copy_node.name] = QRec.copy_ktype(
                qrec, in_qs=[qtype], out_qs=[qtype])

    def find_common_in_edges(self, G: GraphView):
        # Look for splits and concats that share a common in edge
        # the split is a stacked tensor and the concat is an alias in a different stack
        # a copy is always necessary
        # RNNBaseParameters are also here since they create a UserKernelGroup where their
        # input can already be in a stack so causes a tiler error
        nodes = G.nodes(node_classes=(SplitNode,
                                      ConcatNode, RNNNodeBase))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for in_edge in G.indexed_in_edges(node):
                if isinstance(node, RNNNodeBase) and in_edge.to_idx > 0:
                    break
                # find another edge that would be generated as the same edge
                # with a concat/split on it. If found then insert a copy
                # and search again on that node to find others
                dup_edge = self.search_up_for_duplicate(G, in_edge)
                if dup_edge is None:
                    continue
                has_modified_graph = True
                self.insert_copy_at_edge(G, dup_edge)
                nodes.append(node)
                break
        return has_modified_graph

    def search_up_for(self, G, edge, node_class):
        if isinstance(edge.from_node, node_class):
            return edge
        elif self.can_pass_node(edge.from_node):
            return self.search_up_for(G, G.in_edges(edge.from_node)[0], node_class)
        return None

    @staticmethod
    def filter_edges(edges, visited):
        return set(edges) - visited

    def on_same_edge_as(self, G, node, node_class, visited=None,
                        start_edge=None, check_function=None):
        if visited is None:
            visited = set()
            if start_edge:
                to_visit = {VisitEdge(start_edge, 'up')}
            else:
                to_visit = set(VisitEdge(edge, 'up')
                               for edge in G.in_edges(node))

        while to_visit:
            visited_edge = to_visit.pop()
            visit_node = visited_edge.edge.from_node if visited_edge.direction == "up" else visited_edge.edge.to_node
            visited.add(visited_edge.edge)
            # if node class is a tuple of class and direction then see if we are visiting that side of the class
            # this copes with splits and concats that are converted to stacks in the model.
            # A stack alias cannot be an output. The aliases will be the output side of a split and the input side
            # of a concat
            if isinstance(node_class[0], tuple):
                if visited_edge.direction == "up":
                    if any(pair[1] == "down" and isinstance(visit_node, pair[0]) for pair in node_class):
                        if check_function and not check_function(visit_node):
                            continue
                        return visited_edge.edge
                else:
                    if any(pair[1] == "up" and isinstance(visit_node, pair[0]) for pair in node_class):
                        if check_function and not check_function(visit_node):
                            continue
                        return visited_edge.edge
            elif isinstance(visit_node, node_class):
                if check_function and not check_function(visit_node):
                    continue
                return visited_edge.edge
            if visited_edge.direction == "up":
                if self.can_pass_node(visit_node):
                    # if arriving upwards on the node and can pass visit all its edges that we have not visited
                    to_visit |= (set(VisitEdge(edge, 'up')
                                     for edge in self.filter_edges(G.in_edges(visit_node), visited)))
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in self.filter_edges(G.out_edges(visit_node), visited)))
                else:
                    # can't pass but must still visit all the edges on the same out idx
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in self.filter_edges(
                                         G.indexed_out_edges(visit_node)[visited_edge.edge.from_idx], visited)))
            else:
                # the other up edges case is not here since if it is a concat it would already have
                # triggered the return
                if self.can_pass_node(visit_node):
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in self.filter_edges(G.out_edges(visit_node), visited)))
        return None

    COPIES_BETWEEN_CLSES = [
        # any stack -> any stack (different memory - must always copy)
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': (SplitNode, ConcatNode),
            'check_to': lambda node: node.no_model_code,
            'to_cls': (ConcatNode, SplitNode)},
        # stacked memory -> stack in user kernel group
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': (SplitNode, ConcatNode),
            'to_cls': RNNNodeBase},
        # stacked alias -> output (name is lost since it is an alias - could be fixed in AT)
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': ((SplitNode, 'down'),),
            'to_cls': OutputNode,
            'to_ignore': lambda node: node.not_used},
        # Input to a concat that is also an output must have a copy
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': ((ConcatNode, 'up'),),
            'to_cls': OutputNode},
        # If input is not allocated by autotiler then it cannot be part of a stack
        {
            'check_from': lambda node: not node.allocate,
            'from_cls': (InputNode,),
            'check_to': lambda node: node.no_model_code,
            'to_cls': SplitNode},
        # If output is not allocated by autotiler it cannot be part of a stack
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': ((SplitNode, 'up'),),
            'check_to': lambda node: not node.allocate,
            'to_cls': OutputNode},
        # If output is not allocated by autotiler and is used then a copy it cannot be part of a stack
        {
            'check_from': lambda node: node.no_model_code,
            'from_cls': ((ConcatNode, 'down'),),
            'check_to': lambda node: not node.allocate and not node.not_used,
            'to_cls': OutputNode},
        # # stacked tensor -> alias (different memory - must always copy)
        # input -> stacked alias (name is lost since it is an alias - could be fixed in AT)
        {
            'from_cls': (InputNode,),
            'check_to': lambda node: node.no_model_code,
            'to_cls': ConcatNode},
        # constant -> stacked alias (I guess the constant could be loaded here in some cases by AT)
        # constant -> stack (not sure if this works or not. Including for safety. If it doesn't work it could.)
        {
            'from_cls': (ConstantInputNode,),
            'to_cls': (ConcatNode, SplitNode),
            'check_to': lambda node: node.no_model_code},
    ]

    def insert_copies_between(self, G,
                              from_cls=None, to_cls=None,
                              check_from=False,
                              check_to=False,
                              to_ignore=None):
        # insert copys between splits and outputs or concats
        nodes = G.nodes(node_classes=to_cls)
        if check_to:
            nodes = list(filter(check_to, nodes))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            if to_ignore and to_ignore(node):
                continue
            edges = list(dict.fromkeys(G.indexed_in_edges(node)))
            while edges:
                edge = edges.pop()
                if edge is None:
                    continue
                found_edge = self.on_same_edge_as(
                    G, node, from_cls, start_edge=edge, check_function=check_from)
                if found_edge:
                    has_modified_graph = True
                    self.insert_copy_at_edge(G, edge)
        return has_modified_graph

    def _match(self, G: GraphView, **kwargs):
        has_modified_graph = False
        for clses in self.COPIES_BETWEEN_CLSES:
            has_modified_graph |= self.insert_copies_between(
                G, **clses)
        has_modified_graph |= self.find_common_in_edges(G)

        return has_modified_graph
