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

from graph.types import (ActivationParameters, BinaryOpParameters,
                         ConstantInputParameters, ExpressionFusionParameters,
                         MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters, NNEdge,
                         UnaryOpParameters)
from utils.graph import GraphView

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


FUSE_NODES = (ActivationParameters, MatrixDivParameters, MatrixSubParameters,
              MatrixAddParameters, MatrixMulParameters, NNEdge,
              UnaryOpParameters, BinaryOpParameters)


def group_node(G, node, node_sets):
    connected_nodes = (set([edge.from_node for edge in G.in_edges(node.name)]) |
                       set([edge.to_node for edge in G.out_edges(node.name)]))
    current_set = set([node])
    produced_sets = []
    for node_set in node_sets:
        if connected_nodes & node_set:
            current_set |= node_set
        else:
            produced_sets.append(node_set)
    produced_sets.append(current_set)
    return produced_sets


def group_nodes(G, nodes):
    node_sets = []
    for node in nodes:
        node_sets = group_node(G, node, node_sets)
    return node_sets


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


class ExpressionMatcher(Matcher):
    NAME = "expression_matcher"
    DESCRIPTION = "Groups piecewise expressions for kernel generation"
    NEEDS_VALID_DIMENSION = True

    def __init__(self, *args, **kwargs):
        super(ExpressionMatcher, self).__init__(*args, **kwargs)
        self._expr_num = 0

    def match(self, G: GraphView, set_identity: bool = True):
        has_modified_graph = False
        # collect connected node sets
        node_sets = group_nodes(G, [node for node in G.nodes() if isinstance(node, FUSE_NODES) or (
            isinstance(node, ConstantInputParameters) and node.out_dims[0].size() == 1)])
        # remove sets that are only ConstantInputs
        node_sets = [node_set for node_set in node_sets
                     if not all(isinstance(node, ConstantInputParameters)
                                for node in node_set)]
        for node_set in node_sets:
            has_modified_graph = True
            in_edges, out_edges, internal_edges = group_edges(G, node_set)
            frag = GraphView()
            for edge in internal_edges:
                frag.add_edge(edge)
            in_mapping = [[(edge.to_node, edge.to_idx) for edge in edge_group]
                          for edge_group in in_edges.values()]
            out_mapping = list(out_edges.keys())
            constant_inputs = [isinstance(node_edge_idx[0], ConstantInputParameters) for node_edge_idx in in_edges]
            LOG.info("constant_nodes %s", constant_inputs)
            LOG.info("in_edges %s", in_edges)
            expr = ExpressionFusionParameters("expr_%s" % self._expr_num,
                                              subgraph=frag,
                                              input_mapping=in_mapping,
                                              output_mapping=out_mapping,
                                              constant_inputs=constant_inputs)
            G.replace_fragment(frag,
                               expr,
                               frag_in_edges=list(set.union(*in_edges.values())),
                               frag_out_edges=list(set.union(*out_edges.values())),
                               edge_in_mapping=sorted(list(in_edges.keys()), key=lambda x: x[1]),
                               edge_out_mapping=[[(edge.to_node, edge.to_idx) for edge in edge_set] for edge_set in
                                                 out_edges.values()]
                               )

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
