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

from graph.types import (ConcatParameters, CopyParameters, InputParameters,
                         NNEdge, NoOPParameters, OutputParameters,
                         ReshapeParameters, SplitParameters,
                         TransposeParameters)
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import (Matcher, description, groups, match_name,
                       needs_valid_dimension, run_after)

LOG = logging.getLogger("nntool." + __name__)


def find_real_in_edge(G, edge):
    from_node = edge.from_node
    if isinstance(from_node, ReshapeParameters):
        res = find_real_in_edge(G, G.in_edges(from_node.name)[0])
        return res
    if isinstance(from_node, NoOPParameters):
        res = find_real_in_edge(G, G.in_edges(from_node.name)[0])
        return res
    if isinstance(from_node, TransposeParameters):
        _, real_transpose = from_node.real_shape()
        if len(real_transpose) <= 1:
            res = find_real_in_edge(G, G.in_edges(from_node.name)[0])
            return res
    return (edge.from_node, edge.from_idx)


@match_name('insert_copies')
@description('insert copy nodes on edges that link splits to concats')
@run_after('insert_transposes')
@groups('*')
@needs_valid_dimension(True)
class MatchInsertCopies(Matcher):
    @staticmethod
    def can_pass_node(node):
        return (isinstance(node, (ReshapeParameters, NoOPParameters)) or
                isinstance(node, TransposeParameters) and node.does_nothing)

    def find_split_concat_down(self, G, edge):
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
        elif self.can_pass_node(edge.to_node):
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
            QRec.copy_ktype(qrec, in_qs=[qtype], out_qs=[qtype])

    def find_common_in_edges(self, G: GraphView):
        # Look for splits and concats that share a common in edge where a copy is necessary
        nodes = G.nodes(node_classes=(SplitParameters, ConcatParameters))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for in_edge in G.in_edges(node):
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

    def insert_copy_split_to_output_or_concat(self, G):
        # insert copys between splits and outputs or concats
        nodes = G.nodes(node_classes=(ConcatParameters, OutputParameters))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for edge in G.in_edges(node):
                split_edge = self.search_up_for(G, edge, SplitParameters)
                if split_edge is None:
                    continue
                has_modified_graph = True
                self.insert_copy_at_edge(G, split_edge)
        return has_modified_graph

    def insert_copy_input_or_split_to_concat(self, G):
        # insert copies between inputs or splits and concats
        nodes = G.nodes(node_classes=(ConcatParameters))
        has_modified_graph = False
        while nodes:
            node = nodes.pop(0)
            for edge in G.in_edges(node):
                input_edge = self.search_up_for(
                    G, edge, (SplitParameters, InputParameters))
                if input_edge is None:
                    continue
                has_modified_graph = True
                self.insert_copy_at_edge(G, input_edge)
        return has_modified_graph

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = self.insert_copy_input_or_split_to_concat(G)
        has_modified_graph |= self.insert_copy_split_to_output_or_concat(G)
        has_modified_graph |= self.find_common_in_edges(G)

        return has_modified_graph
