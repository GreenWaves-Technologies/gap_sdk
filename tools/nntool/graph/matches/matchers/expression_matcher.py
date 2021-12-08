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

from expressions.symbolic.symbol import Symbol, SymbolStats
from graph.types import ConstantInputParameters, ExpressionFusionParameters
from graph.types.base import CanFuseToExpression, NNEdge
from graph.types.others import QuantizeParameters
from quantization.new_qrec import QRec
from quantization.quantizer.new_quantizer import NewQuantizer
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


def external_edges(G, sub_g):
    """ return the external nodes (in and out) of sub_g in G """
    nodes = sub_g.nodes()
    in_edges = {}
    out_edges = {}
    for node in nodes:
        for in_edge in G.in_edges(node.name):
            if in_edge.from_node not in nodes:
                add_edge(in_edges, in_edge)
        for out_edge in G.out_edges(node.name):
            if out_edge.to_node not in nodes:
                add_edge(out_edges, out_edge)
    return in_edges, out_edges


def walk_nodes(G, node, candidates, visited_nodes=None):
    """ find directly or indirectly connected nodes in candidates from node """
    if visited_nodes is None:
        visited_nodes = set()
    visited_nodes.add(node)
    # find nodes that we have not visited and are in the candidate set (don't use sets so the order does not change)
    connected_nodes = [cnode for cnode in G.connected_nodes(node.name) if cnode not in visited_nodes and cnode in candidates]
    results = [node]
    for next_node in connected_nodes:
        results.extend(walk_nodes(G, next_node, candidates,
                                  visited_nodes=visited_nodes))
    return results


def construct_subgraph(G, nodes):
    """ construct a subgraph from nodes """
    sub_g = GraphView()
    while nodes:
        node = nodes.pop(0)
        if node not in sub_g.nodes():
            sub_g.add_node(node)
        for edge in G.out_edges(node.name):
            if edge.to_node in nodes:
                sub_g.add_edge(edge.clone())
        for edge in G.in_edges(node.name):
            if edge.from_node in nodes:
                sub_g.add_edge(edge.clone())
    return sub_g


def find_node_down_via(G, edges, targets, visited_edges=None):
    """ find target nodes in targets connected from edges """
    if visited_edges is None:
        visited_edges = set()
    for edge in edges:
        if edge in visited_edges:
            continue
        visited_edges.add(edge)
        if edge.to_node in targets:
            return edge.to_node
        out_edges = G.out_edges(edge.to_node.name)
        if not out_edges:
            continue
        node = find_node_down_via(
            G, out_edges, targets, visited_edges=visited_edges)
        if node:
            return node
    return None


def validate_node(G, sub_g, node):
    """ find any node in sub_g connected to node via an edge that is not in sub_g but is in G """
    excluded_edges = set(G.out_edges(node.name)) - \
        set(sub_g.out_edges(node.name))
    if not excluded_edges:
        return None
    return find_node_down_via(G, excluded_edges, sub_g.nodes())


def split_down_from(cur_g, node, res_g=None):
    """ split cur_g into 2 graphs. Everything from node down and the rest """
    if res_g is None:
        res_g = GraphView()
    out_edges = cur_g.out_edges(node.name)
    cur_g.remove(node)
    if node not in res_g.nodes():
        res_g.add_node(node)
    for edge in out_edges:
        res_g.add_edge(edge.clone())
        split_down_from(cur_g, edge.to_node, res_g=res_g)
    return res_g


def validate_subgraph(G, sub_g):
    """ find edges that connect nodes in sub_g via nodes that are not in sub_g and
    split the graph on the nodes found """
    validate_nodes = []
    validated_graphs = []
    unvalidated_graphs = [sub_g]
    while unvalidated_graphs:
        cur_g = unvalidated_graphs.pop(0)
        for node in cur_g.nodes():
            connected_node = validate_node(G, cur_g, node)
            if connected_node:
                new_g = split_down_from(cur_g, connected_node)
                unvalidated_graphs.append(cur_g)
                unvalidated_graphs.append(new_g)
                break
            else:
                validate_nodes.append(node)
        else:
            validated_graphs.append(cur_g)
    return validated_graphs


def walk_down(subgraph, node):
    """ return all the nodes below node including node """
    res = [node]
    for edge in subgraph.out_edges(node.name):
        res.extend(walk_down(subgraph, edge.to_node))
    return res


def split_excluding(subgraph, node):
    """ splits a subgraph into 2 graphs at node excluding node """
    res = []
    nodes_to_include = set([node for edge in subgraph.out_edges(
        node.name) for node in walk_down(subgraph, edge.to_node)])
    subgraph.remove(node)
    while nodes_to_include:
        node = nodes_to_include.pop()
        subgroup = set(walk_nodes(subgraph, node, nodes_to_include))
        nodes_to_include -= subgroup
        res.append(construct_subgraph(subgraph, subgroup))
        for node in subgroup:
            subgraph.remove(node)
    if subgraph.nodes():
        res.append(subgraph)
    return res


def check_all_nodes_happy(G, subgraph):
    """ check that all nodes want to stay in suggraph. If one doesn't split the subgraph excluding it """
    for node in subgraph.nodes():
        nid = NodeId(node)
        qrec = G.quantization[NodeId(
            node)] if G.quantization and nid in G.quantization else None
        if not node.should_fuse(subgraph.nodes(), qrec=qrec):
            return split_excluding(subgraph, node)
    return [subgraph]


def add_constants(G, sub_g):
    """ adds scalar constants to the subgraphs. If a constant is used in more than one place then
    it is duplicated """
    for node in sub_g.nodes():
        for edge in G.in_edges(node.name):
            if not isinstance(edge.from_node, ConstantInputParameters) or edge.from_node.out_dims[0].size() > 1:
                continue
            const_node = edge.from_node
            out_edges = G.out_edges(const_node.name)
            # if constant is connected to more than one node then duplicate it
            if len(out_edges) > 1:
                new_const = ConstantInputParameters(
                    G.unique_name(f'{const_node}_dup'),
                    value=const_node.value.copy(),
                    dims=const_node.dims.clone())
                G.remove_edge(edge)
                G.add_edge(NNEdge(from_node=new_const,
                                  to_node=edge.to_node, to_idx=edge.to_idx))
                sub_g.add_edge(NNEdge(from_node=new_const,
                                      to_node=edge.to_node, to_idx=edge.to_idx))
            else:
                sub_g.add_edge(NNEdge(from_node=const_node,
                                      to_node=edge.to_node, to_idx=edge.to_idx))


def deterministic_order(graph_list):
    """ sorts the graph list by the sorted nodes in each graph """
    graph_list = list(graph_list)
    graph_list = sorted([(graph, "".join(sorted([node.name for node in graph.nodes()])))
                         for graph in graph_list], key=lambda x: x[1])
    return [graph_tuple[0] for graph_tuple in graph_list]


def find_connected_groups(G):
    """ first find all the subgraphs of connected nodes in candidates
        then split subgraphs where a node depends on a set of nodes not in the subgraph
        whose input is the result of a node in the graph
        finally verify that all the nodes want to stay in each subgraph
    """
    candidates = list(G.nodes(node_classes=CanFuseToExpression, sort=True))
    subgraphs = []
    seen = []
    while candidates:
        node = candidates.pop(0)
        if node in seen:
            continue
        subgroup = walk_nodes(G, node, candidates)
        seen.extend(subgroup)
        subgraph = construct_subgraph(G, subgroup)
        valid_subgraphs = validate_subgraph(G, subgraph)
        while valid_subgraphs:
            subgraph = valid_subgraphs.pop(0)
            final_subgraphs = check_all_nodes_happy(G, subgraph)
            if not final_subgraphs:
                continue
            if len(final_subgraphs) > 1:
                valid_subgraphs.extend(final_subgraphs)
                continue
            add_constants(G, final_subgraphs[0])
            subgraphs.append(final_subgraphs[0])

    # graphs must always be returned in the same order
    return deterministic_order(subgraphs)


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
        for frag in find_connected_groups(G):
            Symbol.set_default_control(SymbolStats())
            has_modified_graph = True
            in_edges, out_edges = external_edges(G, frag)
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
                     ",".join(node.__repr__() for node in frag.nodes()),
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
                nid = NodeId(expr)
                G.quantization[nid] = QRec(
                    in_qs=in_qs, out_qs=out_qs, expression=stats, ktype='scaled')
                if G.quantization.stats:
                    G.quantization.stats[nid] = {
                        'range_in': [{'min': qtype.min_val, 'max': qtype.max_val} for qtype in in_qs],
                        'range_out': [{'min': qtype.min_val, 'max': qtype.max_val} for qtype in out_qs],
                        'expression': stats.copy()
                    }

                # delete any quantize parameters on outputs to allow the quantizer
                # to fuse them into the expression
                out_edges = G.out_edges(expr.name)
                for edge in out_edges:
                    if isinstance(edge.to_node, QuantizeParameters):
                        G.remove_and_reconnect(edge.to_node, edge_class=NNEdge)
                        if NodeId(edge.to_node) in G.quantization:
                            del G.quantization[NodeId(edge.to_node)]
                to_quantize.append(expr)

            self._expr_num += 1

        if to_quantize:
            quantizer = NewQuantizer.from_quantized_graph(G)
            quantizer.quantize()

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
