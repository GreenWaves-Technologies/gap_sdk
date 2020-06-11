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
from graph.types.others import ConcatParameters, ReshapeParameters

LOG = logging.getLogger("nntool." + __name__)


class Shape():
    def __init__(self, shape):
        self.shape = shape
        self.idx = 0
        self.inc = True
        self.cur = 1


def reverse_reshape(trans, from_shape, to_shape):
    """reverses the effect of this reshape on the transpose"""
    # The reshape goes from shape -> to shape. Find the equivalent transpose
    # that can be examined for things in to shape or return None if the transpose
    # cannot be converted. from shape may have smaller larger or the same dimensions
    # as to shape.
    shapes = [Shape(to_shape.shape),
              Shape(from_shape.shape)]
    # Build a mask containing the indexes of the from_shape in the
    # shape of to_shape. Here we are looking for continuous sequences of combinations
    # of the two masks
    trans_mask = [[] for _ in shapes[0].shape]
    filling_shape = None
    while all(shape.idx < len(shape.shape) for shape in shapes):
        # multiply the shapes of the indexes that have incremented
        for shape in shapes:
            if shape.inc:
                shape.cur *= shape.shape[shape.idx]
                shape.inc = False

        # add the transpose index to the mask
        trans_mask[shapes[0].idx].append(trans[shapes[1].idx])
        if shapes[0].cur == shapes[1].cur:
            # the shapes match so increment both indexes
            for shape in shapes:
                shape.idx += 1
                shape.cur = 1
                shape.inc = True
            filling_shape = None
        elif shapes[0].cur < shapes[1].cur:
            if filling_shape is None or filling_shape == 0:
                # look for a combination of axes in the shape before the reshape
                shapes[0].idx += 1
                shapes[0].inc = True
                filling_shape = 0
            else:
                return None
        else:
            if filling_shape is None or filling_shape == 1:
                # look for a combination of axes in the shape after the reshape
                shapes[1].idx += 1
                shapes[1].inc = True
                filling_shape = 1
            else:
                return None

    # Either the mask will be complete or one of the two shapes will not have been
    # consumed. Make sure that both shapes are fully used
    for i in [0, 1]:
        if shapes[i].idx < len(shapes[i].shape):
            # can only add shapes that are 1 in length
            if shapes[i].shape[shapes[i].idx] == 1:
                idxes = [shape.idx if shape.idx < len(shape.shape) else -1 for shape in shapes]
                trans_mask[idxes[0]].append(trans[idxes[1]])
                shapes[i].idx += 1
            else:
                # no solution found transpose is modified by the reshape
                return None
    # Make sure the mask is in ascending order
    trans_mask = [sorted(mask) for mask in trans_mask]

    # now we have a mask of the form [[1], [0], [0]] or [[2], [0, 1]]
    # turn this into [2, 0, 1] or [1, 0]
    # old in this case is the shape after reshape
    cur_old_idx = 0
    mask_idx = 0
    cur_new_idx = 0
    new_trans = []
    found_elem = False
    while len(new_trans) < len(shapes[0].shape):
        # if this mask element has not been consumed and its first element
        # matches the index after reshape then consume it
        if len(trans_mask[mask_idx]) > 0 and trans_mask[mask_idx][0] == cur_old_idx:
            new_trans.append(mask_idx)
            # the new old index is the last one in the mask
            cur_old_idx = trans_mask[mask_idx][-1]
            # consume the mask
            trans_mask[mask_idx] = []
            cur_new_idx += 1
            # continue to loop
            found_elem = True
        mask_idx += 1
        if mask_idx >= len(trans_mask):
            # if we didn't find anything then the reshape modifies the transpose
            mask_idx = 0
            cur_old_idx += 1
        if cur_old_idx >= len(shapes[1].shape):
            if not found_elem:
                return None
            found_elem = False
            cur_old_idx = 0


    return new_trans


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
    if len(G.out_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder):
        return []

    if isinstance(node, Transposable) and node.transpose_out:
        if reverses_transpose(node.transpose_out, transpose, node.out_dims[out_idx]):
            return [(node, edge_list, 'out')]
        else:
            return []

    if isinstance(node, ReshapeParameters):
        new_transpose = reverse_reshape(transpose, node.shape, node.old_shape)
        if new_transpose is None:
            return []
        transpose = new_transpose
        if node.transpose_in and reverses_transpose(node.transpose_in, transpose):
            return [(node, edge_list, "in")]

    if isinstance(node, Transposable) and node.transpose_in:
        return []

    return search_up_edges(G, visited_edges, node, transpose, edge_list)


def search_up_edges(G, visited_edges, node, transpose, edge_list):
    all_nodes = []
    for edge in G.in_edges(node.name):
        if edge in visited_edges:
            return []
        next_res = search_up_for_reverse(
            G, visited_edges | {edge}, edge.from_node, edge.from_idx, transpose, edge_list + [edge])
        if not next_res:
            return []
        all_nodes += next_res
    return all_nodes


def search_down_for_reverse(G, visited_edges, node, in_idx, transpose, edge_list=None):
    """Search down the graph for transpose sequences"""
    if len(G.in_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder):
        return []

    if edge_list is None:
        edge_list = []

    if isinstance(node, Transposable) and node.transpose_in:
        if reverses_transpose(transpose, node.transpose_in, node.in_dims[in_idx]):
            return [(node, edge_list, "in")]
        else:
            return []

    # if the node is a concat then we cannot proceed further since the
    # concat must happen on axis 0 and the transposes were already set up for
    # this to happen
    if isinstance(node, ConcatParameters):
        return []

    # if there is a reshape then the dimensionality of the transpose
    # that we are examining may change and that may or may not be compatible
    # with reversing the transpose
    if isinstance(node, ReshapeParameters):
        new_transpose = reverse_reshape(transpose, node.old_shape, node.shape)
        if new_transpose is None:
            return []
        transpose = new_transpose
        if node.transpose_out and reverses_transpose(transpose, node.transpose_out):
            return [(node, edge_list, "out")]

    if isinstance(node, Transposable) and node.transpose_out:
        return []

    return search_down_edges(G, visited_edges, node, transpose, edge_list)


def search_down_edges(G, visited_edges, node, transpose, edge_list):
    all_nodes = []
    for edge in G.out_edges(node.name):
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
            result = search_up_edges(G, visited_edges, transpose_node,
                                     transpose_node.transpose_in, [])
            for r in result:
                visited_edges |= set(r[1])
                results.append(((r[0], r[2]), (transpose_node, 'in'), r[1]
                                [::-1], getattr(r[0], "transpose_" + r[2])))
        if transpose_node.transpose_out:
            result = search_down_edges(G, visited_edges, transpose_node,
                                       transpose_node.transpose_out, [])
            for r in result:
                visited_edges |= set(r[1])
                results.append(
                    ((transpose_node, 'out'), (r[0], r[2]), r[1], transpose_node.transpose_out))
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

    for node, direction in [res[idx] for idx in range(2)]:
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
