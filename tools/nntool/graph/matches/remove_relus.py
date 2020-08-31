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

from graph.types import (ConcatParameters, ConstantInputParameters,
                         HSigmoidActivationParameters, MatrixAddParameters,
                         MatrixDivParameters, MatrixMulParameters, NNEdge,
                         PoolingParameters, ReluActivationParameters,
                         ReshapeParameters, TransposeParameters)
from utils.graph import GraphView
from utils.node_id import NodeId

from .matcher import Matcher


def reduce_edges(in_edges, visited_edges):
    status = [None, None]
    for edge in in_edges:
        edge_rec = visited_edges.get(edge)
        if edge_rec is None:
            return None
        if edge_rec[0] is False:
            return [False, False]
        status[0] = True
        if edge_rec[1] is False:
            status[1] = False
        elif status[1] is None or (status[1] is not False and edge_rec[1] > status[1]):
            status[1] = edge_rec[1]
    return status


def find_redundant_relus(G, node, visited_edges):
    # status 0 is relued
    # status 1 is upper bound
    status = reduce_edges(G.in_edges(node.name), visited_edges)
    if status is None:
        return []
    nodes_to_remove = []
    if isinstance(node, ReluActivationParameters):
        if status[0]:
            # this relu has an upper bound
            if node.upper_bound is not None:
                # if we are already relued less than or equal to that bound
                if status[1] is not False:
                    if status[1] <= node.upper_bound:
                        # remove this relu
                        nodes_to_remove.append(node)
                    else:
                        status[1] = node.upper_bound
                else:
                    # new bound
                    status[1] = node.upper_bound
            else:
                # we're already relued so this is redundant
                nodes_to_remove.append(node)
                if node.upper_bound is not None:
                    if status[1] is False or status[1] > node.upper_bound:
                        status[1] = node.upper_bound
        else:
            status[0] = True
            if node.upper_bound is not None:
                status[1] = node.upper_bound
    elif isinstance(node, HSigmoidActivationParameters):
        status[0] = True
        if status[1] is None or status[1] > 1:
            status[1] = 1
    elif isinstance(node, (MatrixAddParameters, MatrixDivParameters, MatrixMulParameters)):
        status[1] = False
    elif not isinstance(node, (ConstantInputParameters, ReshapeParameters, TransposeParameters, PoolingParameters, ConcatParameters)):
        status = [False, False]
    for edge in G.out_edges(node.name):
        visited_edges[edge] = status
        nodes_to_remove += find_redundant_relus(G, edge.to_node, visited_edges)
    return nodes_to_remove


class RemoveRelusMatch(Matcher):
    NAME = 'remove_relus'
    DESCRIPTION = 'Finds redundant relus in graph'

    def match(self, G: GraphView, set_identity: bool = True):
        visited_edges = {}
        nodes_to_remove = []
        has_modified_graph = False
        for node in G.inputs():
            # check if constantinput. if is then check if positive and check max value
            if isinstance(node, ConstantInputParameters):
                if node.value is not None:
                    if G.has_quantized_parameters:
                        qrec = G.quantization[NodeId(node)]
                        qtype = qrec.out_qs[0]
                        if hasattr(qtype, 'wrapped'):
                            qtype = qtype.wrapped
                        val = qtype.dequantize(node.value)
                    else:
                        val = node.value
                    if val.min() >= 0:
                        status = (True, val.max())
                    else:
                        status = (False, False)
            else:
                status = (False, False)

            for edge in G.out_edges(node.name):
                visited_edges[edge] = status
                nodes_to_remove += find_redundant_relus(G, edge.to_node, visited_edges)
        for node in nodes_to_remove:
            has_modified_graph = True
            # Only relus so only one in edge
            in_edge = G.in_edges(node.name)[0]
            for edge in G.out_edges(node.name):
                G.add_edge(NNEdge(from_node=in_edge.from_node,
                                  from_idx=in_edge.from_idx,
                                  to_node=edge.to_node,
                                  to_idx=edge.to_idx))
            G.remove(node)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
