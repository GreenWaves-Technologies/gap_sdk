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

from graph.dim import Dim
from graph.types import NNEdge, ReshapeParameters
from graph.types.others import ConcatParameters, CopyParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import (Matcher, description, groups, match_name,
                       modifies_dimensions, needs_valid_dimension, run_before)

LOG = logging.getLogger("nntool." + __name__)


def find_concats_up(G, node, subgraph: GraphView = None):
    if subgraph is None:
        subgraph = GraphView()
    for edge in G.in_edges(node.name):
        if isinstance(edge.from_node, ConcatParameters):
            if len(G.out_edges(edge.from_node.name)) > 1 or edge.from_node.axis != 0:
                continue
        elif isinstance(edge.from_node, (CopyParameters, ReshapeParameters)):
            if len(G.out_edges(edge.from_node.name)) > 1:
                continue
        subgraph.add_edge(edge.clone())
        if isinstance(edge.from_node, (ConcatParameters, CopyParameters, ReshapeParameters)):
            find_concats_up(G, edge.from_node, subgraph=subgraph)
    return subgraph


def remove_internal_graph(G, subgraph):
    in_nodes = subgraph.inputs()
    concat_node = subgraph.outputs()[0]
    for in_node in in_nodes:
        edges = set(subgraph.out_edges(in_node.name))
        while edges:
            edge = edges.pop()
            # if the edge goes to the concat that we are keeping just remove it
            # we will add it back later. The node may have already been removed if it
            # is one of the internal concats.
            if edge.to_node == concat_node:
                if edge.from_node.name in G:
                    G.remove_edge(edge)
                continue
            edges |= set(subgraph.out_edges(edge.to_node.name))
            if edge.to_node.name in G:
                G.remove(edge.to_node)
                if G.quantization:
                    nid = NodeId(edge.to_node)
                    if nid in G.quantization:
                        del G.quantization[nid]


@match_name("combine_concats")
@description("Combine concats on 0 axis together")
@run_before('insert_copies')
@groups('*')
@needs_valid_dimension(True)
@modifies_dimensions(True)
class CombineConcats(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        modified_graph = False
        concats = set(G.nodes(node_classes=ConcatParameters))
        while concats:
            concat = concats.pop()
            if concat.axis != 0:
                continue
            subgraph = find_concats_up(G, concat)
            found = set(subgraph.nodes(node_classes=ConcatParameters))
            if len(found) <= 1:
                continue
            LOG.info(
                "Combining concats %s into %s",
                ",".join([node.name for node in found]),
                concat.name)
            modified_graph = True
            concats -= found
            remove_internal_graph(G, subgraph)
            in_nodes = subgraph.inputs()
            in_edges = [
                edge
                for node in in_nodes
                for edge_group in subgraph.indexed_out_edges(node.name)
                for edge in edge_group]
            in_dims = [
                edge.from_node.out_dims[edge.from_idx] for edge in in_edges
            ]
            out_dim = concat.out_dims[0]
            in_qs = []
            for idx, edge in enumerate(in_edges):
                from_node = edge.from_node
                from_idx = edge.from_idx
                if len(in_dims[idx]) > 1:
                    reshape = ReshapeParameters(
                        G.unique_name(f'{concat.name}_flat{idx}'),
                        old_shape=in_dims[idx],
                        shape=Dim.unnamed([in_dims[idx].size()]))
                    G.add_edge(
                        NNEdge(
                            from_node=from_node, from_idx=from_idx,
                            to_node=reshape))
                    from_node = reshape
                    from_idx = 0
                G.add_edge(
                    NNEdge(
                        from_node=from_node, from_idx=from_idx,
                        to_node=concat, to_idx=idx))
                if in_qs is not None and G.quantization:
                    nid = NodeId(edge.from_node)
                    if nid in G.quantization:
                        qrec = G.quantization[nid]
                        in_qs.append(qrec.out_qs[edge.from_idx])
                    else:
                        in_qs = None
                else:
                    in_qs = None
            if in_qs is not None and G.quantization:
                nid = NodeId(concat)
                if nid in G.quantization:
                    G.quantization[nid].in_qs = in_qs
            reshape = ReshapeParameters(
                G.unique_name(f'{concat.name}_expand'),
                old_shape=Dim.unnamed([out_dim.size()]),
                shape=out_dim)
            G.insert_node_after(concat, reshape, edge_class=NNEdge)

        if set_identity:
            self.set_identity(G)

        return modified_graph
