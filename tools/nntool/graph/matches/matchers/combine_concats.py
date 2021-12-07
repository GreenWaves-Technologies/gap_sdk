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
from graph.types import (ConcatParameters, CopyParameters, InputParameters,
                         NNEdge, ReshapeParameters, Parameters)
from utils.graph import GraphView, Node
from utils.node_id import NodeId

from ..matcher import (Matcher, description, groups, match_name,
                       modifies_dimensions, needs_valid_dimension, run_before)

LOG = logging.getLogger("nntool." + __name__)

class DummyInput(Parameters):
    def __init__(self, name: str, edge):
        super().__init__(name)
        self.edge = edge


def traverse_to_concat(G, edge, subgraph, edge_path=None):
    if edge_path is None:
        edge_path = []
    node = edge.from_node
    if isinstance(node, ConcatParameters):
        if node.axis != 0 or len(G.out_edges(node)) > 1:
            return []
        find_concats_up(G, node, subgraph)
        return edge_path + [edge]
    elif isinstance(node, (CopyParameters, ReshapeParameters)):
        if len(G.out_edges(node)) > 1:
            return []
        in_edge = G.in_edges(node)[0]
        return traverse_to_concat(G, in_edge, subgraph, edge_path=edge_path + [edge])
    else:
        return []

def find_concats_up(G, concat, subgraph: GraphView = None):
    # Produces a subgraph of concats operating on axis 0 separated by copys or reshapes.
    # the output node will be the final concat. the input nodes will be all the inputs
    # to a condensed concat that can replace this subgraph.
    if subgraph is None:
        subgraph = GraphView()
        edge_path = []
    for edge in G.indexed_in_edges(concat.name):
        edge_path = traverse_to_concat(G, edge, subgraph)
        if edge_path:
            for inter_edge in edge_path:
                subgraph.add_edge(inter_edge)
        else:
            subgraph.add_edge(
                NNEdge(
                    from_node=DummyInput(f"{edge.from_node.name}_{edge.from_idx}", edge),
                    to_node=edge.to_node,
                    to_idx=edge.to_idx
                )
            )
    return subgraph


# def find_concats_up(G, node, subgraph: GraphView = None, edge_path=None):
#     # Produces a subgraph of concats operating on axis 0 separated by copys or reshapes.
#     # the output node will be the final concat. the input nodes will be all the inputs
#     # to a condensed concat that can replace this subgraph.
#     if subgraph is None:
#         subgraph = GraphView()
#         edge_path = []
#     for edge in G.indexed_in_edges(node.name):
#         if isinstance(edge.from_node, ConcatParameters):
#             if len(G.out_edges(edge.from_node.name)) > 1 or edge.from_node.axis != 0:
#                 continue
#             edge_path.append(edge)
#             for traversed_edge in edge_path:
#                 subgraph.add_edge(traversed_edge.clone())
#             find_concats_up(G, edge.from_node, subgraph=subgraph, edge_path=[])
#         elif isinstance(edge.from_node, (CopyParameters, ReshapeParameters)):
#             if len(G.out_edges(edge.from_node.name)) > 1:
#                 continue
#             find_concats_up(G, edge.from_node, subgraph=subgraph,
#                             edge_path=edge_path + [edge])
#     return subgraph


def remove_internal_graph(G, subgraph):
    in_nodes = subgraph.inputs()
    concat_node = subgraph.outputs()[0]
    for in_node in in_nodes:
        if G.edge_in_graph(in_node.edge):
            G.remove_edge(in_node.edge)
        nodes = {in_node.edge.to_node}
        while nodes:
            node = nodes.pop()
            if node == concat_node:
                continue
            nodes |= set(edge.to_node for edge in subgraph.out_edges(node))
            if node in G:
                G.remove(node)
            nid = NodeId(node)
            if G.quantization and nid in G.quantization:
                del G.quantization[nid]


@ match_name("combine_concats")
@ description("Combine concats on 0 axis together")
@ run_before('insert_copies')
@ groups('*')
@ needs_valid_dimension(True)
@ modifies_dimensions(True)
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
                f"Combining concats {','.join([node.name for node in found])}")
            modified_graph = True
            concats -= found

            in_edges = [inp.edge for inp in subgraph.inputs()]
            in_dims = [
                edge.from_node.out_dims[edge.from_idx] for edge in in_edges
            ]
            nodes_to_remove = [node for node in subgraph.nodes() if node != concat and not isinstance(node, DummyInput)]
            for edge in in_edges:
                G.remove_edge(edge)
            for node in nodes_to_remove:
                if node.name in G:
                    G.remove(node)
                nid = NodeId(node)
                if G.quantization and nid in G.quantization:
                    del G.quantization[nid]

            # remove_internal_graph(G, subgraph)
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
