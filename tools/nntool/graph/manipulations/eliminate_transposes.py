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
from typing import Sequence

from graph.types import (ConcatParameters, ConstantInputParameters,
                         FcParameters, InputParameters,
                         MatrixBroadcastedLinearOpParameters, OutputParameters,
                         PadParameters, ReshapeParameters, ReverseParameters,
                         SensitiveToOrder, SplitParameters,
                         StridedSliceParameters, Transposable)
from graph.types.fusions import MatMulOpFusionParameters
from graph.types.others import TransposeParameters
from graph.types.tensor_arithmetic import MatMulOpParameters
from utils.compatible_transposes import (find_all_compatible_transposes,
                                         find_combination)
from utils.node_id import NodeId

from .eliminate_transposes_actions import (Action, DeleteTranspose,
                                           EndActionDown, EndActionUp,
                                           InsertReshapeAction,
                                           InsertTransposeAction,
                                           ReorderConstantInput,
                                           ReorderInputDims, ReorderLinear,
                                           SetHintAction, SetReshapeAction,
                                           SetTranspose, StartActionDown,
                                           StartActionUp, SwitchBatchLinear,
                                           TransposePad, TransposeReverse,
                                           TransposeSlidedSlice)

LOG = logging.getLogger("nntool." + __name__)

TRANSIENT_ACTIONS = {
    PadParameters: TransposePad,
    ReverseParameters: TransposeReverse,
    StridedSliceParameters: TransposeSlidedSlice
}


def do_transpose(elems, trans):
    return [elems[i] for i in trans]


def reverse_reshape(trans, from_shape, to_shape):
    """reverses the effect of this reshape on the transpose"""
    return next(iter([t for t in find_all_compatible_transposes(find_combination(from_shape, to_shape), trans)
                      if len(t) == len(to_shape)]), None)
    # return find_compatible_transpose(find_combination(from_shape, to_shape), trans)


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


def requires_reshape(trans1, trans2, dim):
    """Checks if layout shape doesn't change but a reshape is necessary due to 1 position"""
    if tuple(dim.shape) != tuple(dim.layout_shape) and dim.layout_shape == dim.calc_transpose(trans1).calc_transpose(trans2).layout_shape:
        from_shape = dim.calc_transpose(trans1)
        to_shape = dim.calc_transpose(trans2)
        if from_shape.shape != to_shape.shape:
            return (from_shape, to_shape)
    return False


def search_up_for_reverse(G, visited_edges, node, out_idx, edge, transpose, done_edges, edge_list, transpose_history):
    """Search up the graph for transpose sequences"""
    LOG.info("looking up at %s[%s] transpose %s",
             node.name, out_idx, transpose)
    if not isinstance(node, Transposable) and (len(G.out_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder)):
        LOG.info("rejected %s - sensitive to order or multi output", node.name)
        return []

    if isinstance(node, FcParameters):
        if node.batch_size > 1:
            if transpose == (1, 0):
                LOG.info(
                    "accepted %s - linear layer switch batch dimension", node.name)
                done_edges |= visited_edges
                return [SwitchBatchLinear(node), EndActionUp(node)]
            LOG.info("rejected %s - multibatch linear layer", node.name)
            return []
        LOG.info("accepted %s - linear layer reorder output", node.name)
        done_edges |= visited_edges
        qrec = G.quantization and G.quantization[NodeId(node)]
        return [ReorderLinear.out_from_history(node, transpose_history, qrec), EndActionUp(node)]

    extra_actions = []
    if isinstance(node, Transposable):
        if node.transpose_out:
            if transpose and reverses_transpose(node.transpose_out[out_idx], transpose, node.out_dims[out_idx]):
                LOG.info("accepted %s - transpose out", node.name)
                reshape = requires_reshape(
                    node.transpose_out[out_idx], transpose, node.out_dims[out_idx])
                if reshape:
                    LOG.info("requires reshape %s -> %s", *reshape)
                done_edges |= visited_edges
                return [SetHintAction(node, 'out', out_idx, transpose=reverse_transpose(transpose)),
                        DeleteTranspose(node, 'out', out_idx, reshape=reshape), EndActionUp(node)]
            else:
                LOG.info(
                    "rejected %s - transpose out - does not reverse", node.name)
                return []
        elif isinstance(node, InputParameters) and not node.fixed_order and G.num_out_edges(node.name) == 1:
            LOG.info(
                "accepted %s - input without fixed order - transpose input %s", node.name, transpose)
            done_edges |= visited_edges
            return [ReorderInputDims.from_history(node, transpose_history, transpose=transpose), EndActionUp(node)]
        elif isinstance(node, ConstantInputParameters):
            LOG.info(
                "accepted %s - constant input - transpose constant %s", node.name, transpose)
            done_edges |= visited_edges
            return [ReorderConstantInput.from_history(node, transpose_history, transpose=transpose), EndActionUp(node)]
        # elif isinstance(node, (MatMulOpParameters, MatMulOpFusionParameters)) and len(node.in_dims) == 2:
        #     if len(transpose) != 2:
        #         raise ValueError(f'strange transpose length into matmul node {node.name} {transpose}')
        #     extra_actions.append(TransposeMatMulOpAction(node))
        #     transpose = [transpose[idx] for idx in [1, 0]]
        elif not node.eliminate_transposes_pass_up:
            LOG.info("rejected %s - transpose out - cannot pass", node.name)
            return []

    # if the node is a concat/split then we cannot proceed further since the
    # concat/split must happen on axis 0 and the transposes were already set up for
    # this to happen
    if isinstance(node, (ConcatParameters, SplitParameters)):
        LOG.info("rejected %s - concat/split", node.name)
        return []
    # if the node is a reshape then moving up we transpose the shape with the current
    # transpose and calculate an equivalent transpose and transpose the old
    # shape with that.
    elif isinstance(node, ReshapeParameters):
        new_transpose = reverse_reshape(transpose, node.shape, node.old_shape)
        # if the upwards shape has one dimension we keep going since we want to find
        # nodes such as a linear layer that can reorder their output filters
        # This could be extended to recurrent layers for the inner dimension
        if new_transpose is None and len(node.old_shape) > 1:
            LOG.info("rejected %s - transpose in - does not reverse", node.name)
            return []
        if G.num_out_edges(node.name) > 1:
            # here we have a reshape with multiple edges that we can pass
            # insert transposes on all the other edges. We will try to eliminate those on the next pass
            LOG.info("%s - transpose in - reshape has multiple outputs", node.name)
            reversed_transpose = reverse_transpose(transpose)
            for out_edge in G.out_edges(node.name):
                if out_edge == edge:
                    continue
                # add the edge to visited so that we cannot have another match passing
                # this reshape
                visited_edges.add(edge)
                extra_actions.append(
                    InsertTransposeAction(node, edge=out_edge, transpose=reversed_transpose, block_search_up=True))
        # But we have to reverse what we store in the history since that is going to be
        # replayed going forwards
        transpose_history.insert(
            0, (node, (new_transpose, node.old_shape.clone()), (transpose, node.shape.clone())))

        # insert an action to rewrite the reshape shapes
        extra_actions.extend([
            SetReshapeAction(
                node,
                in_shape=node.old_shape.calc_transpose(
                    new_transpose) if new_transpose is not None else None,
                out_shape=node.shape.calc_transpose(
                    transpose) if transpose is not None else None
            ),
            SetHintAction(node, 'out', out_idx, transpose=transpose)
        ])

        if node.transpose_in:
            if node.transpose_in and len(node.transpose_in) > 1:
                LOG.info(
                    "rejected %s - cannot pass multi in edge transpose", node.name)
                return []
            if new_transpose is None:
                LOG.info(
                    "rejected %s - transpose is None - cannot pass another transpose", node.name)
                return []
            if reverses_transpose(node.transpose_in[0], new_transpose):
                LOG.info("accepted %s - transpose in (2)", node.name)
                done_edges |= visited_edges
                return [DeleteTranspose(node, 'in', 0)] + extra_actions + [EndActionUp(node)]

        transpose = new_transpose
    else:
        if transpose is not None:
            extra_actions.append(
                SetHintAction(node, 'out', out_idx, transpose=transpose)
            )
            if node.__class__ in TRANSIENT_ACTIONS:
                extra_actions.append(TRANSIENT_ACTIONS[node.__class__](
                    node, transpose))

    if isinstance(node, Transposable) and node.transpose_in:
        LOG.info("rejected %s - transposable", node.name)
        return []

    search_up = search_up_edges(G, visited_edges, node, transpose,
                                edge_list, done_edges, transpose_history=transpose_history)
    if search_up:
        return extra_actions + search_up
    return []


def search_up_edges(G, visited_edges, node, transpose, edge_list, done_edges,
                    transpose_history=None, start_edge=None, equalizing=False):
    all_nodes = []
    if transpose_history is None:
        transpose_history = []
    for edge in [start_edge] if start_edge else G.variable_in_edges(node.name):
        if edge in (visited_edges | done_edges):
            return []
        if start_edge:
            LOG.info("++ Starting up from %s[%s]",
                     edge.to_node.name, edge.to_idx)
        next_res = search_up_for_reverse(
            G, visited_edges | {edge},
            edge.from_node,
            edge.from_idx,
            edge,
            transpose,
            done_edges,
            edge_list + [edge],
            transpose_history.copy())
        if next_res:
            next_res = [SetHintAction(edge.to_node, 'in', edge.to_idx,
                                      transpose=transpose)] + next_res

            if start_edge:
                LOG.info(
                    "++ Found results for %s[%s]", edge.to_node.name, edge.to_idx)
                if equalizing:
                    next_res = [StartActionUp(edge.to_node),
                                SetTranspose(edge.to_node, 'in', edge.to_idx, reverse_transpose(transpose))] + next_res
                else:
                    next_res = [StartActionUp(edge.to_node),
                                DeleteTranspose(edge.to_node, 'in', edge.to_idx)] + next_res
        else:
            LOG.info("Rejected up edges from %s", node.name)
            return []
        all_nodes += next_res
    return all_nodes


def search_down_for_reverse(G, visited_edges, node, in_idx, transpose, done_edges, edge_list=None):
    """Search down the graph for transpose sequences"""
    LOG.info("looking down at %s[%s] transpose %s",
             node.name, in_idx, transpose)
    if not isinstance(node, (Transposable)):
        if len(G.variable_in_edges(node.name)) > 1 or isinstance(node, SensitiveToOrder):
            LOG.info("rejected %s - sensitive to order or multi input", node.name)
            return []

    if edge_list is None:
        edge_list = []

    if isinstance(node, FcParameters):
        if node.batch_size > 1:
            LOG.info("rejected %s - multibatch linear layer", node.name)
            return []
        LOG.info("accepted %s - linear layer reorder input", node.name)
        done_edges |= visited_edges
        return [ReorderLinear(node, 'in', reverse_transpose(transpose), node.in_dims[0]), EndActionDown(node)]

    if isinstance(node, Transposable):
        if node.transpose_in and node.transpose_in[in_idx]:
            if reverses_transpose(transpose, node.transpose_in[in_idx], node.in_dims[in_idx]):
                LOG.info("accepted %s - transpose in (1)", node.name)
                done_edges |= visited_edges
                reshape = requires_reshape(
                    transpose, node.transpose_in[in_idx], node.in_dims[in_idx])
                if reshape:
                    LOG.info("requires reshape %s -> %s", *reshape)
                    if hasattr(node, 'ker_in_order') and node.ker_in_order:
                        return [SetHintAction(node, 'in', in_idx, val=deepcopy(node.ker_in_order[in_idx])),
                                DeleteTranspose(node, 'in', in_idx, reshape=reshape), EndActionDown(node)]

                return [SetHintAction(node, 'in', in_idx, transpose=reverse_transpose(transpose)),
                        DeleteTranspose(node, 'in', in_idx, reshape=reshape), EndActionDown(node)]
            else:
                LOG.info(
                    "rejected %s - transpose in - does not reverse", node.name)
                return []
        elif isinstance(node, OutputParameters) and not node.fixed_order:
            LOG.info("accepted %s - output without fixed order - transpose output %s",
                     node.name, transpose)
            done_edges |= visited_edges
            return [SetHintAction(node, 'in', in_idx, transpose=transpose), EndActionDown(node)]
        elif isinstance(node, MatrixBroadcastedLinearOpParameters):
            assert node.transpose_in is None or node.transpose_in[in_idx] is None
            LOG.info(
                "accepted %s - propagated transpose to broadcasted linear op", node.name)
            done_edges |= visited_edges
            return [SetTranspose(node, 'in', in_idx, transpose), EndActionDown(node)]
        elif len(transpose) == 1:
            LOG.info("accepted %s transpose length 1 - transpose in", node.name)
            done_edges |= visited_edges
            return [EndActionDown(node)]
        elif not node.eliminate_transposes_pass_down:
            LOG.info("rejected %s - transpose in - cannot pass down", node.name)
            return []

    # if the node is a concat/split then we cannot proceed further since the
    # concat/split must happen on axis 0 and the transposes were already set up for
    # this to happen
    if isinstance(node, (ConcatParameters, SplitParameters)):
        LOG.info("rejected %s - concat/split", node.name)
        return []

    # if there is a reshape then the dimensionality of the transpose
    # that we are examining may change and that may or may not be compatible
    # with reversing the transpose
    if isinstance(node, ReshapeParameters):
        # the transpose that we are actually applying is the reverse of the transpose that we are propagating down
        # So we reverse the transpose before evaluating the reshape and then reverse the result
        new_transpose = reverse_transpose(reverse_reshape(
            reverse_transpose(transpose), node.old_shape, node.shape))
        LOG.info("reshape %s trans: old %s new %s shape: old %s new %s",
                 node.name, transpose, new_transpose, node.old_shape, node.shape)
        if new_transpose is None:
            LOG.info("rejected %s - transpose out - does not reverse", node.name)
            return []
        # insert an action to rewrite the reshape shapes
        extra_actions = [
            SetReshapeAction(
                node,
                in_shape=node.old_shape.calc_transpose(transpose),
                out_shape=node.shape.calc_transpose(new_transpose)
            ),
            SetHintAction(node, 'in', in_idx,
                          transpose=reverse_transpose(transpose))
        ]
        if node.transpose_out:
            if reverses_transpose(new_transpose, node.transpose_out[0]):
                LOG.info("accepted %s - transpose out", node.name)
                done_edges |= visited_edges
                return [DeleteTranspose(node, 'out', 0)] + extra_actions + [EndActionDown(node)]
            elif len(new_transpose) == 1:
                LOG.info(
                    "accepted %s transpose length 1 - transpose out", node.name)
                done_edges |= visited_edges
                return [DeleteTranspose(node, 'out', 0)] + extra_actions + [EndActionDown(node)]
        transpose = new_transpose
    else:
        extra_actions = [
            SetHintAction(node, 'in', in_idx,
                          transpose=reverse_transpose(transpose))
        ]
        if node.__class__ in TRANSIENT_ACTIONS:
            extra_actions.append(TRANSIENT_ACTIONS[node.__class__](
                node, reverse_transpose(transpose)))

    if isinstance(node, Transposable) and node.transpose_out:
        LOG.info("rejected %s - transposable", node.name)
        return []

    search_down = search_down_edges(
        G, visited_edges, node, transpose, edge_list, done_edges)
    if search_down:
        return extra_actions + search_down
    return []


def search_down_edges(G, visited_edges, node, transpose, edge_list, done_edges, start_edges=None):
    all_results = []
    for from_idx, edges in [start_edges] if start_edges else enumerate(G.indexed_out_edges(node.name)):
        if start_edges and edges:
            LOG.info("++ Starting down from %s[%s]",
                     edges[0].from_node.name, from_idx)
        sub_results = []
        sub_done_edges = done_edges.copy()
        success = False
        for edge in edges:
            if edge in (visited_edges | done_edges):
                success = False
                break
            next_res = search_down_for_reverse(
                G, visited_edges | {edge}, edge.to_node, edge.to_idx, transpose, sub_done_edges, edge_list + [edge])
            if next_res:
                success = True
                sub_results += next_res
            else:
                LOG.info(
                    "Rejected down edges from %s[%s]", node.name, from_idx)
                success = False
                break
        if success:
            sub_results = [SetHintAction(edges[0].from_node, 'out', from_idx,
                                         transpose=reverse_transpose(transpose))] + sub_results
            if start_edges:
                LOG.info(
                    "++ Found results for %s[%s]", edges[0].from_node.name, from_idx)
                sub_results = [StartActionDown(edges[0].from_node),
                               DeleteTranspose(edges[0].from_node, 'out', from_idx)] + sub_results
            done_edges |= sub_done_edges
            all_results += sub_results
    return all_results


def search_for_reverses(G):
    results = []
    # visited edges contains all edges included in found transpose pairs
    visited_edges = set()
    for transpose_node in [node for node in G.nodes() if isinstance(node, Transposable)]:
        # for each transpose node we look up and down from the transpose in and transpose out
        # respectively to see if another transpose reverses this one with nothing
        # inbetween that is transpose sensitive
        blocked_up = isinstance(transpose_node, TransposeParameters) and transpose_node.block_search_up
        blocked_down = isinstance(transpose_node, TransposeParameters) and transpose_node.block_search_down
        if not blocked_up and transpose_node.transpose_in:
            for edge in G.in_edges(transpose_node.name):
                # this can be true in the case where a node has constant inputs
                # it probably should be eliminated and all nodes transposed uniformly
                if edge.to_idx >= len(transpose_node.transpose_in):
                    continue
                trans = transpose_node.transpose_in[edge.to_idx]
                if trans is None:
                    continue
                done_edges = set()
                result = search_up_edges(G, visited_edges, transpose_node,
                                         transpose_node.transpose_in[edge.to_idx], [], done_edges, start_edge=edge)
                results.extend(result)
                visited_edges |= done_edges
        # don't look at the other side of a transpose node unless there are no results
        if not blocked_down and transpose_node.transpose_out or (not results and isinstance(transpose_node, TransposeParameters) and transpose_node.transpose_in):
            for from_idx, edges in enumerate(G.indexed_out_edges(transpose_node.name)):
                if not edges:
                    continue
                if isinstance(transpose_node, TransposeParameters):
                    trans = transpose_node.transpose_in[0]
                else:
                    trans = transpose_node.transpose_out[from_idx]
                if trans is None:
                    continue
                done_edges = set()
                result = search_down_edges(G, visited_edges, transpose_node,
                                           trans, [], done_edges, start_edges=(from_idx, edges))
                results.extend(result)
                visited_edges |= done_edges

    # Now try to work up from nodes that have multiple inputs so that we can pass through the
    # transpose
    VALID_EQUALIZATION_CANDIDATES = tuple(
        [MatrixBroadcastedLinearOpParameters])
    for transpose_node in [node for node in G.nodes()
                           if (isinstance(node, VALID_EQUALIZATION_CANDIDATES) and
                               node.transpose_in and
                               len(node.transpose_in) > 1)]:
        LOG.info("Equalizing multi input transposes on node %s",
                 transpose_node.name)
        max_len = max(len(dim) for dim in transpose_node.in_dims)
        transpose_candidates = set(expand_transpose(trans, max_len)
                                   for trans in transpose_node.transpose_in if trans is not None)
        for transpose_candidate in transpose_candidates:
            LOG.info("Trying candidate %s", transpose_candidate)
            this_node_results = []
            success = True
            all_match = True
            for edge in G.in_edges(transpose_node.name):
                if edge in visited_edges:
                    success = False
                    break
                trans_in = transpose_node.transpose_in[edge.to_idx]
                exp_trans_in = expand_transpose(trans_in, max_len)
                # check if this edge is the one we are trying to propagate
                if exp_trans_in and exp_trans_in == transpose_candidate:
                    continue
                all_match = False

                reverse_candidate = reverse_transpose(
                    transpose_node.derive_transpose(transpose_candidate, edge.to_idx))
                LOG.info("%s rev %s trans_in %s", transpose_candidate,
                         reverse_candidate, trans_in)
                if trans_in is None:
                    set_trans = reverse_candidate
                else:
                    # transpose to convert current transpose to candidate
                    set_trans = [trans_in.index(idx)
                                 for idx in reverse_candidate]
                done_edges = set()
                result = search_up_edges(G, visited_edges, transpose_node,
                                         set_trans, [], done_edges, start_edge=edge, equalizing=True)
                if not result:
                    success = False
                    break

                visited_edges |= done_edges
                this_node_results.extend(result)
            if success:
                LOG.info(
                    "Found equalization to %s for %s", transpose_candidate, transpose_node.name)
                if all_match:
                    results += [SetTranspose(transpose_node, 'in', None, None)]
                results += this_node_results
                break

    return results


def reduce_transpose(trans, act_len):
    if trans is None:
        return None
    extra = len(trans) - act_len
    if extra == 0:
        return trans
    return tuple([trans_idx - extra for idx, trans_idx in enumerate(trans) if idx >= extra])


def expand_transpose(trans, max_len):
    if trans is None:
        return None
    if len(trans) == max_len:
        return trans
    excess = max_len - len(trans)
    return tuple(list(range(excess)) + [idx + excess for idx in trans])


def reverse_transpose(trans):
    if trans is None:
        return trans
    return [trans.index(idx) for idx in range(len(trans))]


def process_results(results: Sequence[Action], G):
    for action in results:
        action.execute(G)


def eliminate_transposes(G, debug_function=None, one_cycle=False):
    """Eliminates unnecessary transposes from the graph. Valid transposes are those that have no
    nodes that are sensitive to order between them and where one reverses the other"""
    LOG.info("eliminating unnecessary transposes")
    while True:
        LOG.info("search for transposes")
        results = search_for_reverses(G)
        if results:
            LOG.info("eliminate transposes")
            process_results(results, G)
        else:
            LOG.info("no transposes to eliminate found")
        G.add_dimensions()
        if debug_function:
            debug_function(G)
        if one_cycle:
            LOG.info("cycle complete")
            break
        if not results:
            LOG.info("no further transpose sequences found")
            break
