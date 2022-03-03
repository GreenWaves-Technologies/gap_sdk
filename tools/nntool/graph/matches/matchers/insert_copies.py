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

from graph.types import (ConcatParameters, CopyParameters, InputParameters, RNNBaseParameters,
                         NNEdge, NoOPParameters, OutputParameters, ConstantInputParameters,
                         ReshapeParameters, SplitParameters,
                         TransposeParameters)
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import (Matcher, description, groups, match_name,
                       needs_valid_dimension, run_after)

LOG = logging.getLogger("nntool." + __name__)


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
        return node.no_model_code

    def find_split_concat_down(self, G, edge):
        # search for a split or concat on any downward edge
        if isinstance(edge.to_node, (SplitParameters, ConcatParameters)):
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
        copy_node = CopyParameters(
            G.unique_name(f'{edge.from_node.name}_copy'))
        LOG.info(f'inserting copy between {edge.from_node.name}:{edge.from_idx} '
                 f'and {edge.to_node.name}:{edge.to_idx}')
        G.insert_node_at_edge(copy_node, edge, edge_class=NNEdge)
        nid = NodeId(edge.from_node)
        if G.quantization and nid in G.quantization:
            qrec = G.quantization[nid]
            qtype = deepcopy(qrec.out_qs[edge.from_idx])
            G.quantization[NodeId(copy_node)] = QRec.copy_ktype(qrec, in_qs=[qtype], out_qs=[qtype])

    def find_common_in_edges(self, G: GraphView):
        # Look for splits and concats that share a common in edge
        # the split is a stacked tensor and the concat is an alias in a different stack
        # a copy is always necessary
        # RNNBaseParameters are also here since they create a UserKernelGroup where their
        # input can already be in a stack so causes a tiler error
        nodes = G.nodes(node_classes=(SplitParameters,
                                      ConcatParameters, RNNBaseParameters))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for in_edge in G.indexed_in_edges(node):
                if isinstance(node, RNNBaseParameters) and in_edge.to_idx > 0:
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

    def on_same_edge_as(self, G, node, node_class, visited=None, last_direction=None, start_edge=None):
        if visited is None:
            visited = set()
            if start_edge:
                to_visit = {VisitEdge(start_edge, 'up')}
            else:
                to_visit = set(VisitEdge(edge, 'up') for edge in G.in_edges(node))

        while to_visit:
            visited_edge = to_visit.pop()
            visit_node = visited_edge.edge.from_node if visited_edge.direction == "up" else visited_edge.edge.to_node
            visited.add(visited_edge)
            # if node class is a tuple of class and direction then see if we are visiting that side of the class
            # this copes with splits and concats that are converted to stacks in the model.
            # A stack alias cannot be an output. The aliases will be the output side of a split and the input side
            # of a concat
            if isinstance(node_class[0], tuple):
                if visited_edge.direction == "up":
                    if any(pair[1] == "down" and isinstance(visit_node, pair[0]) for pair in node_class):
                        return visited_edge.edge
                else:
                    if any(pair[1] == "up" and isinstance(visit_node, pair[0]) for pair in node_class):
                        return visited_edge.edge
            elif isinstance(visit_node, node_class):
                return visited_edge.edge
            if visited_edge.direction == "up":
                if self.can_pass_node(visit_node):
                    # if arriving upwards on the node and can pass visit all its edges that we have not visited
                    to_visit |= (set(VisitEdge(edge, 'up')
                                     for edge in G.in_edges(visit_node)) - visited)
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in G.out_edges(visit_node)) - visited)
                else:
                    # can't pass but must still visit all the edges on the same out idx
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in G.indexed_out_edges(visit_node)[visited_edge.edge.from_idx]) - visited)
            else:
                # the other up edges case is not here since if it is a concat it would already have
                # triggered the return
                if self.can_pass_node(visit_node):
                    to_visit |= (set(VisitEdge(edge, 'down')
                                     for edge in G.out_edges(visit_node)) - visited)
        return None

    COPIES_BETWEEN_CLSES = [
        # any stack -> any stack (different memory - must always copy)
        {'from': (SplitParameters, ConcatParameters), 'to': (
            ConcatParameters, SplitParameters)},
        # stacked memory -> stack in user kernel group
        {'from': (SplitParameters, ConcatParameters), 'to': (RNNBaseParameters,)},
        # stacked alias -> output (name is lost since it is an alias - could be fixed in AT)
        {'from': ((SplitParameters, 'down'), (ConcatParameters, 'up')), 'to': (OutputParameters,)},
        # # stacked tensor -> alias (different memory - must always copy)
        # input -> stacked tensor (can be fixed in AT)
        # input -> stacked alias (name is lost since it is an alias - could be fixed in AT)
        {'from': (InputParameters,), 'to': (
            ConcatParameters, SplitParameters)},
        # constant -> stacked alias (I guess the constant could be loaded here in some cases by AT)
        # constant -> stack (not sure if this works or not. Including for safety. If it doesn't work it could.)
        {'from': (ConstantInputParameters,), 'to': (
            ConcatParameters, SplitParameters)},
    ]

    def insert_copies_between(self, G, from_clses, to_clses):
        # insert copys between splits and outputs or concats
        nodes = G.nodes(node_classes=to_clses)
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for edge in G.indexed_in_edges(node):
                found_edge = self.on_same_edge_as(G, node, from_clses, start_edge=edge)
                if found_edge:
                    has_modified_graph = True
                    self.insert_copy_at_edge(G, found_edge)
        return has_modified_graph

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        for clses in self.COPIES_BETWEEN_CLSES:
            has_modified_graph |= self.insert_copies_between(
                G, clses['from'], clses['to'])
        has_modified_graph |= self.find_common_in_edges(G)

        return has_modified_graph
