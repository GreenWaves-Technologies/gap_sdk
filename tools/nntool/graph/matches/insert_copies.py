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
                         NoOPParameters, ReshapeParameters, SplitParameters,
                         TransposeParameters)
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from utils.graph import GraphView
from utils.node_id import NodeId

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


class MatchInsertCopies(Matcher):
    NAME = 'insert_copies'
    DESCRIPTION = 'insert copy nodes on edges that link splits to concats'
    NEEDS_VALID_DIMENSION = True

    def find_concat_edge(self, G, edge):
        to_node = edge.to_node
        if isinstance(to_node, ConcatParameters):
            return [edge]
        if isinstance(to_node, ReshapeParameters):
            if to_node.transpose_in or to_node.transpose_out:
                return None
            return [edge] + self.find_concat_edge(G, G.out_edges(to_node.name)[0])
        if isinstance(to_node, NoOPParameters):
            return [edge] + self.find_concat_edge(G, G.out_edges(to_node.name)[0])
        if isinstance(to_node, TransposeParameters):
            _, real_transpose = to_node.real_shape()
            if len(real_transpose) <= 1:
                return [edge] + self.find_concat_edge(G, G.out_edges(to_node.name)[0])

        return []

    def find_split_concat(self, G, split_node):
        out_edges = G.indexed_out_edges(split_node.name)
        res = [[] for _ in range(len(out_edges))]
        found_some = False
        for edge_group in out_edges:
            for edge in edge_group:
                concat_edges = self.find_concat_edge(G, edge)
                if concat_edges:
                    found_some = True
                    res[edge.from_idx].append(concat_edges)
        return res if found_some else None

    def match(self, G: GraphView, set_identity: bool = True):
        split_nodes = [node for node in G.nodes() if isinstance(node, SplitParameters)]
        has_modified_graph = False
        for node in split_nodes:
            # traverse reshapes or transposes that do nothing - check gen
            # find edges connected to concats
            res = self.find_split_concat(G, node)
            if res is None:
                continue
            # TODO(martin) - group edges that have adjacent inputs and outputs
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
                    G.add_edge(NNEdge(copy_node, first_edge.to_node, to_idx=first_edge.to_idx))
                G.add_edge(NNEdge(node, copy_node, from_idx=idx))
                if G.quantization:
                    G.quantization[NodeId(copy_node)] = qrec.__class__(
                        in_qs=deepcopy(qrec.out_qs[idx]),
                        out_qs=deepcopy(qrec.out_qs[idx]))
        return has_modified_graph
