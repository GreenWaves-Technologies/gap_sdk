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

from graph.types.base import SensitiveToOrder, Transposable
from graph.types.others import (ConcatParameters, ReshapeParameters, SplitParameters,
                                StridedSliceParameters)
from utils.compatible_transposes import (find_combination,
                                         find_compatible_transpose)

LOG = logging.getLogger("nntool." + __name__)


def reverse_reshape(trans, from_shape, to_shape):
    """reverses the effect of this reshape on the transpose"""
    return find_compatible_transpose(find_combination(from_shape, to_shape), trans)


def reverses_transpose(trans1, trans2, dim=None):
    """Checks if one transpose reverses another. If a dim is provided then
    look if the transpose sequence produces an equivalent dim to cope with 1s in
    dimensions."""
    if trans1 is None or trans2 is None:
        return False
    if dim and dim.layout_shape == dim.calc_transpose(trans1).calc_transpose(trans2).layout_shape:
        return True
    for idx, val in enumerate(trans1):
        if trans2[val] != idx:
            return False
    return True


def search_up_for_reverse(G, visited_edges, node, out_idx, transpose, edge_list):
    """Search up the graph for transpose sequences"""
    LOG.debug("looking up at %s", node.name)
    if not isinstance(node, Transposable) and (len(G.out_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder)):
        LOG.debug("rejected %s - sensitive to order or multi output", node.name)
        return []

    if isinstance(node, Transposable) and node.transpose_out:
        if reverses_transpose(node.transpose_out[out_idx], transpose, node.out_dims[out_idx]):
            LOG.debug("accepted %s - transpose out", node.name)
            return [(node, edge_list, 'out', out_idx)]
        else:
            LOG.debug("rejected %s - transpose out - does not reverse", node.name)
            return []

    # if the node is a concat/split then we cannot proceed further since the
    # concat/split must happen on axis 0 and the transposes were already set up for
    # this to happen
    if isinstance(node, (ConcatParameters, SplitParameters)):
        LOG.debug("rejected %s - concat/split", node.name)
        return []

    if isinstance(node, ReshapeParameters):
        new_transpose = reverse_reshape(transpose, node.shape, node.old_shape)
        if new_transpose is None:
            LOG.debug("rejected %s - transpose in - does not reverse", node.name)
            return []
        transpose = new_transpose
        if node.transpose_in and reverses_transpose(node.transpose_in[0], transpose):
            LOG.debug("accepted %s - transpose in", node.name)
            return [(node, edge_list, "in", 0)]

    if isinstance(node, Transposable) and node.transpose_in:
        LOG.debug("rejected %s - transposable", node.name)
        return []

    return search_up_edges(G, visited_edges, node, transpose, edge_list)


def search_up_edges(G, visited_edges, node, transpose, edge_list, start_edge=None):
    all_nodes = []
    for edge in ([start_edge] if start_edge else G.variable_in_edges(node.name)):
        if edge in visited_edges:
            return []
        next_res = search_up_for_reverse(
            G, visited_edges | {edge},
            edge.from_node,
            edge.from_idx,
            transpose,
            edge_list + [edge])
        if not next_res:
            return []
        all_nodes += next_res
    return all_nodes


def search_down_for_reverse(G, visited_edges, node, in_idx, transpose, edge_list=None):
    """Search down the graph for transpose sequences"""
    LOG.debug("looking down at %s", node.name)
    if not isinstance(node, (Transposable)):
        if len(G.variable_in_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder):
            LOG.debug("rejected %s - sensitive to order or multi input", node.name)
            return []

    if edge_list is None:
        edge_list = []

    if isinstance(node, Transposable):
        if node.transpose_in:
            if reverses_transpose(transpose, node.transpose_in[in_idx], node.in_dims[in_idx]):
                LOG.debug("accepted %s - transpose in", node.name)
                return [(node, edge_list, "in", in_idx)]
            else:
                LOG.debug("rejected %s - transpose in - does not reverse", node.name)
                return []
        elif len(transpose) == 1:
            LOG.debug("accepted %s transpose length 1 - transpose in", node.name)
            return [(node, edge_list, "in", in_idx)]

    # if the node is a concat/split then we cannot proceed further since the
    # concat/split must happen on axis 0 and the transposes were already set up for
    # this to happen
    if isinstance(node, (ConcatParameters, SplitParameters)):
        LOG.debug("rejected %s - concat/split", node.name)
        return []

    # if there is a reshape then the dimensionality of the transpose
    # that we are examining may change and that may or may not be compatible
    # with reversing the transpose
    if isinstance(node, ReshapeParameters):
        new_transpose = reverse_reshape(transpose, node.old_shape, node.shape)
        if new_transpose is None:
            LOG.debug("rejected %s - transpose out - does not reverse", node.name)
            return []
        transpose = new_transpose
        if node.transpose_out:
            if reverses_transpose(transpose, node.transpose_out[0]):
                LOG.debug("accepted %s - transpose out", node.name)
                return [(node, edge_list, "out", 0)]
            elif len(transpose) == 1:
                LOG.debug("accepted %s transpose length 1 - transpose out", node.name)
                return [(node, edge_list, "out", 0)]

    if isinstance(node, Transposable) and node.transpose_out:
        LOG.debug("rejected %s - transposable", node.name)
        return []

    return search_down_edges(G, visited_edges, node, transpose, edge_list)


def search_down_edges(G, visited_edges, node, transpose, edge_list, start_edge=None):
    all_nodes = []
    for edge in ([start_edge] if start_edge else G.out_edges(node.name)):
        if edge in visited_edges:
            return []
        next_res = search_down_for_reverse(
            G, visited_edges | {edge}, edge.to_node, edge.to_idx, transpose, edge_list + [edge])
        if not next_res:
            return []
        all_nodes += next_res
    return all_nodes


def search_for_reverses(G):
    results = []
    # visited edges contains all edges included in found transpose pairs
    visited_edges = set()
    for transpose_node in [node for node in G.nodes() if isinstance(node, Transposable)]:
        # for each transpose node we look up and down from the transpose in and transpose out
        # respectively to see if another transpose reverses this one with nothing
        # inbetween that is transpose sensitive
        if transpose_node.transpose_in:
            for edge in G.in_edges(transpose_node.name):
                # this can be true in the case where a node has constant inputs
                # it probably should be eliminated and all nodes transposed uniformly
                if edge.to_idx >= len(transpose_node.transpose_in):
                    continue
                trans = transpose_node.transpose_in[edge.to_idx]
                if trans is None:
                    continue
                result = search_up_edges(G, visited_edges, transpose_node,
                                         transpose_node.transpose_in[edge.to_idx], [], start_edge=edge)
                for r in result:
                    visited_edges |= set(r[1])
                    # result is (from_node, from_transpose_dir, from_idx), (to_node, to_transpose_dir, to_idx),
                    # edge list, transpose (from_node)
                    results.append(
                        (
                            (r[0], r[2], r[3]),
                            (transpose_node, 'in', edge.to_idx),
                            r[1][::-1],
                            getattr(r[0], "transpose_" + r[2])[r[3]]
                        )
                    )
        if transpose_node.transpose_out:
            for edge in G.out_edges(transpose_node.name):
                trans = transpose_node.transpose_out[edge.from_idx]
                if trans is None:
                    continue
                result = search_down_edges(G, visited_edges, transpose_node,
                                           trans, [], start_edge=edge)
                for r in result:
                    visited_edges |= set(r[1])
                    results.append(
                        (
                            (transpose_node, 'out', edge.from_idx),
                            (r[0], r[2], r[3]),
                            r[1],
                            transpose_node.transpose_out[edge.from_idx]
                        )
                    )
    return results


def process_result(res):
    LOG.info("eliminating transpose between %s[%s] and %s[%s]",
             res[0][0].name, res[0][1], res[1][0].name, res[1][1]
             )
    transpose = res[3]
    for edge in res[2]:
        to_node = edge.to_node
        if isinstance(to_node, ReshapeParameters) and not to_node.transpose_in:
            LOG.info("eliminating input transpose on %s", to_node.name)
            transpose = reverse_reshape(transpose, to_node.old_shape, to_node.shape)
            to_node.shape.transpose(transpose)
        elif isinstance(to_node, StridedSliceParameters):
            LOG.info("transpose strided slice %s", to_node.name)
            to_node.act_slice = [to_node.act_slice[idx] for idx in transpose]
            to_node.out_shape = [to_node.out_shape[idx] for idx in transpose]

    for node, direction, edge_idx in [res[idx] for idx in range(2)]:
        trans = getattr(node, "transpose_"+direction)
        # This transpose node may have been selected because it only has
        # one dimension in which case there may not actually be a transpose
        if trans:
            trans[edge_idx] = None
            if all(elem is None for elem in trans):
                setattr(node, "transpose_"+direction, None)


def eliminate_transposes(G):
    """Eliminates unnecessary transposes from the graph. Valid transposes are those that have no
    nodes that are sensitive to order between them and where one reverses the other"""
    LOG.info("eliminating unnecessary transposes")
    while True:
        results = search_for_reverses(G)
        if not results:
            LOG.info("no further transpose sequences found")
            break
        for result in results:
            process_result(result)
        G.add_dimensions()
