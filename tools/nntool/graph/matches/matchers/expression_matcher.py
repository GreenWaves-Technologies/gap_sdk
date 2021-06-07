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
from collections.abc import MutableSequence

from expressions.symbolic.symbol import Symbol, SymbolStats
from graph.types import ConstantInputParameters, ExpressionFusionParameters
from graph.types.base import CanFuseToExpression, NNEdge, Transposable
from graph.types.others import QuantizeParameters
from quantization.new_qrec import QRec
from quantization.unified_quantizer import UnifiedQuantizer
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import (Matcher, description, match_name, needs_valid_dimension,
                       run_after)

LOG = logging.getLogger("nntool." + __name__)


def add_edge(edge_set, edge):
    key = (edge.from_node, edge.from_idx)
    con_group = edge_set.get(key)
    if not con_group:
        con_group = set()
        edge_set[key] = con_group
    con_group.add(edge)


def group_edges(G, node_set):
    internal_edges = set()
    in_edges = {}
    out_edges = {}
    for node in node_set:
        for in_edge in G.in_edges(node.name):
            if in_edge.from_node in node_set:
                internal_edges.add(in_edge)
            else:
                add_edge(in_edges, in_edge)
        for out_edge in G.out_edges(node.name):
            if out_edge.to_node in node_set:
                internal_edges.add(out_edge)
            else:
                add_edge(out_edges, out_edge)
    return in_edges, out_edges, internal_edges


def find_paths(G, edge, edge_paths, current_path):
    edge_paths[edge] = current_path.copy()
    node = edge.to_node
    if not all(in_edge in edge_paths for in_edge in G.in_edges(node.name)):
        return
    all_paths = set.union(*[edge_paths[in_edge]
                            for in_edge in G.in_edges(node.name)])
    all_paths.add(node)
    for out_edge in G.out_edges(node.name):
        find_paths(G, out_edge, edge_paths, all_paths)


def create_paths(G):
    edge_paths = {}
    for node in G.inputs():
        for edge in G.out_edges(node.name):
            find_paths(G, edge, edge_paths, set([node]))
    return edge_paths


class ListByObject(MutableSequence):
    """ A list that can become a set and compares by object hash"""

    def __init__(self, *args, created_by=None) -> None:
        self._list = [] if not args else list(args[0])
        self._created_by = created_by

    def __getitem__(self, key):
        return self._list.__getitem__(key)

    def __setitem__(self, key, val):
        return self._list.__setitem__(key, val)

    def __delitem__(self, key):
        return self._list.__delitem__(key)

    def __len__(self) -> int:
        return self._list.__len__()

    def insert(self, index: int, value) -> None:
        return self._list.insert(index, value)

    def __eq__(self, o: object) -> bool:
        return object.__eq__(self, o)

    def __hash__(self) -> int:
        return object().__hash__(self)

    def as_set(self):
        return set(self._list)


def create_groups(G, edge_paths):
    # breadth first search for groups of piecewise expressions in graph
    # copes with detecting broken cycles where a common ancestor node is not connected
    # to all branches to a subsequent 'join' node such as a binary op.

    groups = []                 # stores a reference to all the created groups of nodes that can be fused
    node_queue = G.inputs()     # queue of nodes to visit
    visited = set()             # set of already visited nodes
    current_sets = {}           # mapping from edge to group of nodes that can be fused
    while node_queue:
        node = node_queue.pop(0)
        visited.add(node)
        if isinstance(node, CanFuseToExpression) and (not isinstance(node, Transposable) or not node.has_transpose):
            incoming_sets = [current_sets.get(in_edge)
                             for in_edge in G.in_edges(node.name)]
            if len(incoming_sets) == 1:
                # one incoming set so can always inherit it
                current_set = incoming_sets[0]
            elif len(incoming_sets) > 1:
                # if all the branches have no common ancestor then we can merge
                # if they have common ancestors then if one set contains some of them then so
                # should the other
                incoming_paths = [edge_paths[in_edge]
                                  for in_edge in G.in_edges(node.name)]
                intersection = set.intersection(*incoming_paths)
                if not intersection:
                    # no common ancestors
                    can_merge = True
                else:
                    set_in_intersection = [incoming_set.as_set(
                    ) & intersection for incoming_set in incoming_sets]
                    can_merge = all(set_in_intersection[0] == other for other in set_in_intersection[1::])
                if can_merge:
                    # merge the incoming sets
                    current_set = ListByObject(set.union(
                        *[incoming_set.as_set() for incoming_set in incoming_sets]), created_by=node.name)
                    groups.append(current_set)
                    keys_to_update = [
                        k for k, v in current_sets.items() if v in incoming_sets]
                    for key in keys_to_update:
                        if current_sets[key] in groups:
                            groups.remove(current_sets[key])
                        current_sets[key] = current_set
                else:
                    current_set = ListByObject(created_by=node.name)
                    groups.append(current_set)
            else:
                raise ValueError('unexpected number of incoming sets')
            current_set.append(node)
        else:
            current_set = ListByObject(created_by=node.name)
            groups.append(current_set)

        for out_edge in G.out_edges(node.name):
            out_node = out_edge.to_node
            # if already visited then ignore this (can occur due to multiple inputs)
            if out_node in visited:
                continue
            # if all the inward nodes to this node have been visited then we can traverse it
            if all(in_edge.from_node in visited for in_edge in G.in_edges(out_node.name)):
                node_queue.append(out_node)
            # set the edge to the current set if it might be able to fuse otherwise give it a fresh set
            if isinstance(out_node, CanFuseToExpression) and (not isinstance(node, Transposable) or
                                                              not node.has_transpose):
                current_sets[out_edge] = current_set
            else:
                fresh_set = ListByObject(created_by=node.name)
                current_sets[out_edge] = fresh_set
                groups.append(fresh_set)

    # get rid of empty groups and duplicates
    groups = sorted([group.as_set() for group in groups if group],
                    key=lambda x: min(node.step_idx for node in x))
    return groups


@match_name("expression_matcher")
@description("Groups piecewise expressions for kernel generation")
@run_after('expand_transposes')
@needs_valid_dimension(True)
class ExpressionMatcher(Matcher):

    def __init__(self, *args, **kwargs):
        super(ExpressionMatcher, self).__init__(*args, **kwargs)
        self._expr_num = 0

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        to_quantize = []
        node_sets = self.find_sets(G)
        for node_set in node_sets:
            Symbol.set_default_control(SymbolStats())
            has_modified_graph = True
            in_edges, out_edges, internal_edges = group_edges(G, node_set)
            frag = GraphView()
            for node in node_set:
                frag.add_node(node)
            for edge in internal_edges:
                frag.add_edge(edge)
            in_mapping = [[(edge.to_node, edge.to_idx) for edge in edge_group]
                          for edge_group in in_edges.values()]
            in_dims = [from_node.out_dims[from_idx]
                       for from_node, from_idx in in_edges]
            out_dims = [from_node.out_dims[from_idx]
                        for from_node, from_idx in out_edges]
            out_mapping = list(out_edges.keys())
            constant_inputs = [node_edge_idx[0]
                               for node_edge_idx in in_edges
                               if isinstance(node_edge_idx[0], ConstantInputParameters)]
            LOG.debug("inputs coming from: %s",
                      ",".join(f"{from_node.__repr__()}:{from_idx}"
                               for from_node, from_idx in in_edges))
            LOG.info("fusing nodes: %s into expr_%s",
                     ",".join(node.__repr__() for node in node_set),
                     self._expr_num)
            expr = ExpressionFusionParameters(G.unique_name(f"expr_{self._expr_num}"),
                                              subgraph=frag,
                                              qrecs=G.quantization,
                                              input_mapping=in_mapping,
                                              output_mapping=out_mapping,
                                              in_dims=in_dims,
                                              out_dims=out_dims,
                                              constant_inputs=constant_inputs)
            in_edge_mapping = list(in_edges.keys())
            out_edge_mapping = [[(edge.to_node, edge.to_idx) for edge in edge_set] for edge_set in
                                out_edges.values()]
            G.replace_fragment(frag,
                               expr,
                               frag_in_edges=list(
                                   set.union(*in_edges.values())),
                               frag_out_edges=list(
                                   set.union(*out_edges.values())),
                               edge_in_mapping=in_edge_mapping,
                               edge_out_mapping=out_edge_mapping,
                               edge_class=NNEdge
                               )
            if G.quantization:
                qrecs = G.quantization
                in_qs = [qrecs[NodeId(in_map[0][0])].in_qs[in_map[0][1]]
                         for in_map in in_mapping]
                out_qs = [qrecs[NodeId(node)].out_qs[idx]
                          for node, idx in out_mapping]
                stats = Symbol.CURRENT_CONTROL.stats
                func_col = expr.func_col
                for idx, qtype in enumerate(in_qs):
                    symbol = func_col.variables[func_col.input_names[idx]]
                    stats[symbol.name] = {
                        'min': qtype.min_val, 'max': qtype.max_val}
                for idx, qtype in enumerate(out_qs):
                    symbol = func_col.variables[func_col.output_names[idx]]
                    stats[symbol.name] = {
                        'min': qtype.min_val, 'max': qtype.max_val}
                G.quantization[NodeId(expr)] = QRec(
                    in_qs=in_qs, out_qs=out_qs, expression=stats, ktype='scaled')
                # delete any quantize parameters on outputs to allow the quantizer
                # to fuse them into the expression
                out_edges = G.out_edges(expr.name)
                for edge in out_edges:
                    if isinstance(edge.to_node, QuantizeParameters):
                        G.remove_and_reconnect(edge.to_node)
                        if NodeId(edge.to_node) in G.quantization:
                            del G.quantization[NodeId(edge.to_node)]
                to_quantize.append(expr)

            self._expr_num += 1

        if to_quantize:
            quantizer = UnifiedQuantizer.from_quantized_graph(G)
            G.quantization = quantizer.quantize(G, start_nodes=to_quantize)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph

    @staticmethod
    def can_find_up(G, node, to_find):
        if node in to_find:
            return True
        in_edges = G.in_edges(node.name)
        if not in_edges:
            return False
        return any(ExpressionMatcher.can_find_up(G, edge.from_node, to_find)
                   for edge in in_edges)

    @staticmethod
    def add_constants(G, node_set):
        result = node_set.copy()
        for node in node_set:
            result.add(node)
            for edge in G.in_edges(node.name):
                if isinstance(edge.from_node, ConstantInputParameters) and edge.from_node.out_dims[0].size() == 1:
                    result.add(edge.from_node)
        return result

    def find_sets(self, G):
        edge_paths = create_paths(G)
        node_sets = create_groups(G, edge_paths)
        results = []
        for node_set in node_sets:
            while True:
                remove_node = None
                for node in node_set:
                    nid = NodeId(node)
                    qrec = G.quantization[NodeId(
                        node)] if G.quantization and nid in G.quantization else None
                    if not node.should_fuse(node_set, qrec=qrec):
                        remove_node = node
                        break
                if remove_node:
                    node_set.remove(remove_node)
                else:
                    if node_set:
                        results.append(self.add_constants(G, node_set))
                    break
        return results
