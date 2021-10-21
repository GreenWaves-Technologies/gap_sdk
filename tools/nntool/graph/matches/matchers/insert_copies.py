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
from copy import deepcopy

from graph.types import (ConcatParameters, CopyParameters, NNEdge,
                         NoOPParameters, OutputParameters, ReshapeParameters,
                         SplitParameters, TransposeParameters)
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
    def find_concat_edge(self, G, edge):
        to_node = edge.to_node
        if isinstance(to_node, (ConcatParameters, SplitParameters)):
            return [edge]
        if isinstance(to_node, ReshapeParameters):
            res = self.find_concat_edge(G, G.out_edges(to_node.name)[0])
            return [edge] + res if res else res
        if isinstance(to_node, NoOPParameters):
            res = self.find_concat_edge(G, G.out_edges(to_node.name)[0])
            return [edge] + res if res else res
        if isinstance(to_node, TransposeParameters):
            _, real_transpose = to_node.real_shape()
            if len(real_transpose) <= 1:
                res = self.find_concat_edge(G, G.out_edges(to_node.name)[0])
                return [edge] + res if res else res

        return []

    def find_split_to_output_edge(self, G, edge):
        to_node = edge.to_node
        if isinstance(to_node, OutputParameters):
            return [edge]
        if isinstance(to_node, ReshapeParameters):
            res = self.find_split_to_output_edge(
                G, G.out_edges(to_node.name)[0])
            return [edge] + res if res else res
        if isinstance(to_node, NoOPParameters):
            res = self.find_split_to_output_edge(
                G, G.out_edges(to_node.name)[0])
            return [edge] + res if res else res
        if isinstance(to_node, TransposeParameters):
            _, real_transpose = to_node.real_shape()
            if len(real_transpose) <= 1:
                res = self.find_split_to_output_edge(
                    G, G.out_edges(to_node.name)[0])
                return [edge] + res if res else res

        return []

    def find_split_concat(self, G, split_node, find_output=True):
        out_edges = G.indexed_out_edges(split_node.name)
        res = [[] for _ in range(len(out_edges))]
        found_some = False
        for edge_group in out_edges:
            for edge in edge_group:
                concat_edges = self.find_concat_edge(G, edge)
                if find_output:
                    concat_edges += self.find_split_to_output_edge(G, edge)
                if concat_edges:
                    found_some = True
                    res[edge.from_idx].append(concat_edges)
        return res if found_some else None

    def insert_copy_on_common_concat_in(self, G, concat_nodes):
        # in every concat nodes collect all the in edges (from_node, from_idx)
        # if there are repetition of tuples, insert a copy in every repetition
        # different concats cannot have the same in edge (from_node, from_idx)
        concat_in_edges = []
        has_modified_graph = False
        for concat_node in concat_nodes:
            for idx, in_edge in enumerate(G.indexed_in_edges(concat_node.name)):
                real_in_edge = find_real_in_edge(G, in_edge)
                if real_in_edge in concat_in_edges:
                    has_modified_graph = True
                    copy_node = CopyParameters("%s_copy_%s" % (concat_node.name, idx))
                    G.remove_edge(in_edge)
                    LOG.info('common_concat: inserting copy between %s/%s and %s/%s',
                             in_edge.from_node.name, idx, concat_node.name, in_edge.to_idx)
                    G.add_edge(NNEdge(in_edge.from_node, copy_node, from_idx=in_edge.from_idx))
                    G.add_edge(NNEdge(copy_node, concat_node, to_idx=in_edge.to_idx))
                    if G.quantization:
                        qrec = G.quantization[NodeId(concat_node)]
                        G.quantization[NodeId(copy_node)] = QRec.copy_ktype(qrec,
                                                                            in_qs=[deepcopy(qrec.in_qs[idx])],
                                                                            out_qs=[deepcopy(qrec.in_qs[idx])])
                else:
                    concat_in_edges.append(real_in_edge)
        return has_modified_graph

    def find_direct_connects(self, G, node, has_modified_graph, find_output=True):
        # traverse reshapes or transposes that do nothing - check gen
        # find edges connected to concats
        res = self.find_split_concat(G, node, find_output=find_output)
        if res is None:
            return has_modified_graph
        if G.quantization:
            qrec = G.quantization[NodeId(node)]
        for idx, bundle in enumerate(res):
            if not bundle:
                continue
            has_modified_graph = True
            copy_node = CopyParameters("%s_copy_%s" % (node.name, idx))
            for edge_set in bundle:
                first_edge = edge_set[0]
                G.remove_edge(first_edge)
                LOG.info('inserting copy between %s/%s and %s/%s',
                         node.name, idx, first_edge.to_node.name, first_edge.to_idx)
                G.add_edge(NNEdge(copy_node, first_edge.to_node,
                                  to_idx=first_edge.to_idx))
            G.add_edge(NNEdge(node, copy_node, from_idx=idx))
            if G.quantization:
                G.quantization[NodeId(copy_node)] = QRec.copy_ktype(qrec,
                                                                    in_qs=[
                                                                        deepcopy(qrec.out_qs[idx])],
                                                                    out_qs=[deepcopy(qrec.out_qs[idx])])
        return True

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        split_nodes = [node for node in G.nodes(node_classes=SplitParameters)]
        has_modified_graph = False
        for node in split_nodes:
            has_modified_graph = self.find_direct_connects(
                G, node, has_modified_graph)
        concat_nodes = [node for node in G.nodes(
            node_classes=ConcatParameters)]
        for node in concat_nodes:
            has_modified_graph = self.find_direct_connects(
                G, node, has_modified_graph, find_output=False)
        has_modified_graph |= self.insert_copy_on_common_concat_in(G, concat_nodes)

        return has_modified_graph
