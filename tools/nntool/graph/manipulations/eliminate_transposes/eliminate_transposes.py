# Copyright (C) 2021  GreenWaves Technologies, SAS

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
from collections.abc import MutableSet
from copy import deepcopy
from typing import AbstractSet, Iterator, Sequence

import numpy as np
from graph.dim import Dim
from graph.types import (BinaryOpParameters, ConcatParameters,
                         ConstantInputParameters, FcParameters,
                         InputParameters, LinearFusionParameters,
                         OutputParameters, PadParameters, ReshapeParameters,
                         ReverseParameters, StridedSliceParameters,
                         TransposeParameters)
from graph.types.base import NNEdge, SensitiveToOrder
from graph.types.global_pooling import GlobalPoolingParameters
from graph.types.tensor_arithmetic import Broadcastable
from utils.compatible_transposes import (find_all_compatible_transposes,
                                         find_combination)
from utils.graph import Node
from utils.node_id import NodeId

from .eliminate_transposes_actions import (Action, DeleteReshapeAction,
                                           DeleteTransposeAction,
                                           EndActionDown, EndActionUp,
                                           InsertReshapeAction,
                                           InsertTransposeAction,
                                           ReorderConstantInput,
                                           ReorderInputDims,
                                           ReorderLinearAction,
                                           SetReshapeAction,
                                           SetTransposeAction,
                                           SwitchBatchLinearAction,
                                           TransposePad, TransposeReverse,
                                           TransposeSlidedSlice)
from .transpose_helpers import (apply_transpose, get_reshape_transpose,
                                reshape_is_transpose, reverse_transpose,
                                reverses_transpose, transpose_does_nothing)

LOG = logging.getLogger("nntool." + __name__)


def info(msg):
    LOG.info(msg)


TRANSIENT_ACTIONS = {
    PadParameters: TransposePad,
    ReverseParameters: TransposeReverse,
    StridedSliceParameters: TransposeSlidedSlice
}

NODES_TO_EXPLORE_UP = {
    ConcatParameters,
    BinaryOpParameters,
    Broadcastable
}


class CantContinueError(Exception):
    pass


class TransposeHistory():
    def __init__(self, node, from_shape, from_transpose, to_shape=None, to_transpose=None) -> None:
        # can be initialized with to_shape set to none in which case from == to
        # to_transpose == None means that the transpose has been eliminated by a reshape
        self.node = node
        self._from = (from_shape, from_transpose)
        if to_shape is None:
            self._to = (from_shape, from_transpose)
        else:
            self._to = (to_shape, to_transpose)

    @property
    def from_shape(self):
        return self._from[0]

    @property
    def to_shape(self):
        return self.to[0]

    @property
    def from_transpose(self):
        return self._from[1]

    @property
    def to_transpose(self):
        return self._to[1]


class VisitedNodes(MutableSet):
    def __init__(self, nodes_dict=None) -> None:
        super().__init__()
        if nodes_dict:
            self._nodes = nodes_dict
        else:
            self._nodes = {}

    @property
    def nodes(self):
        return self._nodes

    def __contains__(self, x: Node) -> bool:
        return x in self._nodes and any(self._nodes[x])

    def __iter__(self) -> Iterator[Node]:
        return iter([node for node, visit in self._nodes.items() if visit])

    def __len__(self) -> int:
        return len([node for node, visit in self._nodes.items() if visit])

    def add(self, value: Node) -> None:
        self._nodes[value] = set()

    def discard(self, value: Node) -> None:
        del self._nodes[value]

    def visit_down(self, node, idx):
        val = self._nodes.setdefault(node, set())
        val.add(f'down{idx}')

    def visited_down(self, node, idx=None) -> bool:
        visited = self._nodes.get(node, set())
        if idx is None:
            return any(k.startswith('down') for k in visited)
        return f'down{idx}' in visited or 'down*' in visited

    def visit_up(self, node, idx):
        val = self._nodes.setdefault(node, set())
        val.add('up{idx}')

    def visited_up(self, node, idx=None) -> set:
        visited = self._nodes.get(node, set())
        if idx is None:
            return any(k.startswith('up') for k in visited)
        return 'up{idx}' in visited or 'up*' in visited

    def visited_direction(self, direction, idx, node) -> bool:
        return f'{direction}{idx}' in self._nodes.get(node, set())

    def copy(self):
        return VisitedNodes(nodes_dict=deepcopy(self.nodes))

    @staticmethod
    def __or_nodes(s, res):
        if isinstance(s, VisitedNodes):
            for node, visited in s.nodes.items():
                if node in res:
                    res[node] |= visited
                else:
                    res[node] = visited
        else:
            for node in s:
                res[node] = {'up*', 'down*'}

        return res

    def __or__(self, s: 'VisitedNodes') -> 'VisitedNodes':
        res = {k: v.copy() for k, v in self._nodes.items()}
        return VisitedNodes(nodes_dict=self.__or_nodes(s, res))

    def __ior__(self, s) -> 'VisitedNodes':
        self._nodes = self.__or_nodes(s, self._nodes)
        return self

    def __repr__(self) -> str:
        return "{" + ",".join(f"{repr(node)}: {visited}" for node, visited in self._nodes.items()) + "}"


def is_broadcasted(from_shape, to_shape):
    from_len = len(from_shape)
    to_len = len(to_shape)
    if from_len >= to_len:
        return False
    return tuple(([1] * (to_len - from_len)) + list(from_shape)) == tuple(to_shape)


def expand_to_len(trans, length):
    extra = length-len(trans)
    return tuple(list(range(extra)) + [dim + extra for dim in trans])


def reverse_reshape(trans, from_shape, to_shape):
    """reverses the effect of this reshape on the transpose"""
    # if the from_shape -> to_shape is actually a broadcast reshape
    # i.e. 4, 10, 1 -> 1, 4, 10, 1 we absolutely need to keep the order 4, 10, 1 in
    # the transpose however the 2 1s in the result are ambiguous so handle this as a
    # (simple) special case. Just expand the transpose with no transpose at the start
    # and expand_len + original transpose dim at the end
    if len(from_shape) == 0 or len(to_shape) == 0:
        return None
    if is_broadcasted(from_shape, to_shape):
        return expand_to_len(trans, len(to_shape))

    return next(iter([t for t in find_all_compatible_transposes(find_combination(from_shape, to_shape), trans)
                      if len(t) == len(to_shape)]), None)


def none_or_idx(trans, idx):
    return None if trans[idx] is None else idx


def requires_reshape(trans1, trans2, dim):
    """Checks if layout shape doesn't change but a reshape is necessary due to 1 position"""
    if tuple(dim.shape) != tuple(dim.layout_shape) and dim.layout_shape == dim.calc_transpose(trans1).calc_transpose(trans2).layout_shape:
        from_shape = dim.calc_transpose(trans1)
        to_shape = dim.calc_transpose(trans2)
        if from_shape.shape != to_shape.shape:
            return (from_shape, to_shape)
    return False


def strip_nones(trans):
    return [i for i in trans if i is not None]


def broadcast_reduce(out_shape, in_shape, transpose):
    """Looking at a broadcasted input that has a lower rank than out_shape find
    the equivalent transpose to the transpose on the broadcasted shape before
    the broadcast

    Args:
        out_shape (Sequence): The full shape of the output of the broadcasted operation
        in_shape (Sequence): The shape of the unbroadcasted input
        transpose (Sequence): The transpose on the output

    Returns:
        Tuple: The in shape, the broadcasted in shape, the equivalent transpose
    """
    diff_shape = len(out_shape) - len(in_shape)
    # broadcast shape with nones
    exp_in_shape = ([None] * diff_shape) + list(range(len(in_shape)))
    # apply the reverse of the transpose. now we have the broadcasted shape before the transpose
    transpose_exp_in_shape = apply_transpose(
        exp_in_shape, reverse_transpose(transpose))
    # strip the nones and reverse the result. This gives the transpose of the unbroadcasted shape
    new_transpose = reverse_transpose(strip_nones(transpose_exp_in_shape))
    new_shape = ([1] * diff_shape) + in_shape
    return in_shape, new_shape, new_transpose


def broadcast_expand(out_shape, in_shape, transpose):
    diff_shape = len(out_shape) - len(in_shape)
    exp_in_shape = ([None] * diff_shape) + list(range(len(in_shape)))
    transpose_exp_in_shape = apply_transpose(exp_in_shape, transpose)
    new_transpose = list(range(diff_shape)) + \
        [dim + diff_shape for dim in transpose]
    new_shape = ([1] * diff_shape) + in_shape
    return in_shape, new_shape, new_transpose


def check_for_null_transpose(node, transpose):
    if transpose is None:
        raise CantContinueError(f"can't continue at {node.name}")  # @IgnoreException


def check_continue(visited_nodes: VisitedNodes, cur_visited_nodes: VisitedNodes, exclude_nodes, node, direction, idx):
    all_visited = visited_nodes | cur_visited_nodes
    if direction == 'up' and all_visited.visited_down(node):
        return True
    if direction == 'down' and all_visited.visited_up(node):
        raise CantContinueError()  # @IgnoreException
    if all_visited.visited_direction(direction, idx, node):
        raise CantContinueError()  # @IgnoreException
    if node in exclude_nodes:
        raise CantContinueError()  # @IgnoreException
    return False


def strip_leading_ones(shape, in_len):
    res = []
    seen_dim = False
    for dim in shape:
        if seen_dim:
            res.append(dim)
        elif dim != 1:
            res.append(dim)
            seen_dim = True
    return res

def compute_max_shape(dims):
    if len(dims) == 1:
        return dims[0]
    shapes = [dim.shape for dim in dims]
    return [max(dims) for dims in zip(*shapes)]

def search_down(G, node, exclude_nodes, visited_nodes: VisitedNodes, in_edge, transpose_history: Sequence[TransposeHistory]):
    """Searches down the graph for something that eliminates transpose

    Args:
        G : The graph
        node : The node to look at
        visited_nodes : Nodes already traversed
        in_edge : The edge we are arriving on at this node
        transpose_history : A history of the reshapes passed that did not allow us to determine the transpose
        transpose : The current transpose being propagated. Can be None to indicate that we cannot translate the transpose via that reshape

    Returns:
        A tuple of a list of actions and a list of nodes traversed
    """
    cur_visited_nodes = VisitedNodes()
    cur_visited_nodes.visit_down(node, in_edge.to_idx)

    transpose = transpose_history[-1].to_transpose
    in_shape = node.in_dims[in_edge.to_idx].shape
    if transpose is not None and len(transpose) == 1:
        return [EndActionDown(node)], []

    if isinstance(node, SensitiveToOrder) and transpose_does_nothing(transpose, in_shape):
        check_for_null_transpose(node, transpose)
        new_shape = apply_transpose(
            in_shape, transpose)
        # could be that the transpose does nothing to the data layout but still changes the positions of
        # the ones in the shape
        if new_shape == in_shape:
            return [EndActionDown(node)], cur_visited_nodes
        info(
            f'accepted {node.name} - transpose does nothing but requires reshape {new_shape}->{in_shape}')
        return [
            InsertReshapeAction(node, direction='in', idx=in_edge.to_idx,
                                in_shape=new_shape, out_shape=in_shape),
            EndActionDown(node)], cur_visited_nodes

    if isinstance(node, SensitiveToOrder):
        info(
            f'rejected {node.name}  - sensitive to order - inserting transpose {transpose}')
        return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose), EndActionDown(node)], cur_visited_nodes

    cur_actions = []

    # if arriiving on a broadcasted input the transpose needs to be expanded and a reshape inserted
    # TODO - Expand to handle multiple outputs on expressions
    if isinstance(node, Broadcastable) and node.in_dims[in_edge.to_idx].rank != node.out_dims[0].rank:
        check_for_null_transpose(node, transpose)
        max_shape = compute_max_shape(node.out_dims)
        in_shape, new_shape, new_transpose = broadcast_expand(
            max_shape, in_shape, transpose)
        from_shape = apply_transpose(in_shape, reverse_transpose(transpose))
        to_shape = apply_transpose(new_shape, reverse_transpose(new_transpose))
        info(f'{node.name} broadcasted input {in_edge.to_idx} requires reshape {from_shape}->{to_shape}')
        cur_actions.append(
            InsertReshapeAction(
                node, direction='in', idx=in_edge.to_idx,
                in_shape=from_shape,
                out_shape=to_shape))
        transpose = new_transpose
        in_shape = new_shape

    # on nodes where inputs must have the same transpose applied i.e. Binary arithmetic ops etc
    # TODO - Expand to handle multiple outputs on expressions
    if any(isinstance(node, cls) for cls in NODES_TO_EXPLORE_UP):
        check_for_null_transpose(node, transpose)
        max_shape = compute_max_shape(node.out_dims)
        for edge in set(G.in_edges(node.name)) - {in_edge}:
            if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.from_node, 'up', edge.from_idx):
                continue
            # if other edges are broadcasted they may need to be reshaped
            edge_in_shape, new_shape, new_transpose = broadcast_reduce(
                max_shape, node.in_dims[edge.to_idx].shape, transpose)
            new_history = [
                TransposeHistory(node, apply_transpose(
                    node.in_dims[edge.to_idx].shape, new_transpose), reverse_transpose(new_transpose))
            ]
            from_shape = apply_transpose(
                edge_in_shape, reverse_transpose(new_transpose))
            to_shape = apply_transpose(new_shape, reverse_transpose(transpose))
            if from_shape != to_shape:
                info(
                    f'{node.name} broadcasted input {edge.to_idx} requires reshape {from_shape}->{to_shape}')
                cur_actions.append(
                    InsertReshapeAction(
                        node, direction='in', idx=edge.to_idx,
                        in_shape=from_shape,
                        out_shape=strip_leading_ones(to_shape, len(from_shape))
                    ))
            new_actions, visited_up_nodes = search_up(
                G, edge.from_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge, new_history)
            cur_visited_nodes |= visited_up_nodes
            cur_actions += new_actions

    # Conditions that can absorb the Transpose

    # TODO - Handle FC in a Fusion
    if isinstance(node, (FcParameters, LinearFusionParameters)):
        filter_node = node.contained_filters()[0] if isinstance(node, LinearFusionParameters) else node
        if filter_node.batch_size > 1:
            info(
                f"rejected {node.name} - multibatch linear layer - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose), EndActionDown(node)], cur_visited_nodes
        info(
            f"accepted {node.name} - linear layer reorder input - {transpose}")
        qrec = G.quantization and G.quantization[NodeId(node)]
        return cur_actions + [ReorderLinearAction.in_from_history(node, transpose_history, qrec), EndActionDown(node)], cur_visited_nodes

    if isinstance(node, TransposeParameters):
        # TODO - Might be able to get rid of this and check history
        check_for_null_transpose(node, transpose)
        if reverses_transpose(transpose, node.transpose, node.out_dims[0]):
            info(
                f"accepted {node.name} - transpose {node.transpose} reversed in by {transpose} on {node.in_dims[0]}")
            reshape = requires_reshape(
                transpose, node.transpose, node.in_dims[0])
            if reshape:
                info(f"requires reshape {reshape[0]} -> {reshape[1]}")
            return [DeleteTransposeAction(node, reshape=reshape), EndActionDown(node)], cur_visited_nodes
        new_transpose = apply_transpose(transpose, node.transpose)
        info(
            f"rejected {node.name} - transpose - does not reverse - absorbing {transpose} into {node.transpose} -> {new_transpose}")
        return [SetTransposeAction(node, new_transpose), EndActionDown(node)], cur_visited_nodes

    if isinstance(node, OutputParameters):
        # TODO - Might be able to get rid of this and check history
        check_for_null_transpose(node, transpose)
        if node.fixed_order:
            info(
                f"rejected {node.name} - fixed order output - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose), EndActionDown(node)], cur_visited_nodes
        info(
            f"accepted {node.name} - output without fixed order - transpose output {transpose}")
        # No change here since the output dimensions will be computed by the shape inference
        return [EndActionDown(node)], cur_visited_nodes

    if node.__class__ in TRANSIENT_ACTIONS:
        check_for_null_transpose(node, transpose)
        cur_actions.append(TRANSIENT_ACTIONS[node.__class__](
            node, reverse_transpose(transpose), "down"))


    elif isinstance(node, ReshapeParameters):
        # TODO - Might be able to get rid of this and check history
        check_for_null_transpose(node, transpose)
        if reshape_is_transpose(node.old_shape.shape, node.shape.shape):
            # if the reshape looks like a transpose then treat it as one. THe reshape rewriter sometimes gets
            # the order wrong in this case
            old_transpose = get_reshape_transpose(
                node.old_shape.shape, node.shape.shape)
            new_transpose = apply_transpose(transpose, old_transpose)
        else:
            # the transpose that we are actually applying is the reverse of the transpose that we are propagating down
            # So we reverse the transpose before evaluating the reshape and then reverse the result
            new_transpose = reverse_transpose(reverse_reshape(
                reverse_transpose(transpose), node.old_shape, node.shape))

        info(
            f"pass reshape {node.name} down trans: old {transpose} new {new_transpose} shape: old {node.old_shape} new {node.shape}")
        if new_transpose is None and len(node.shape) > 1:
            info(
                f"rejected {node.name} - transpose out - does not reverse - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose), EndActionDown(node)], cur_visited_nodes
        # insert an action to rewrite the reshape shapes
        from_shape = node.old_shape.calc_transpose(
            reverse_transpose(transpose)) if transpose is not None else None
        to_shape = node.shape.calc_transpose(reverse_transpose(
            new_transpose)) if new_transpose is not None else None
        info(f"rewrite reshape to {from_shape}->{to_shape}")
        if from_shape is None or to_shape is None or from_shape.shape != to_shape.shape:
            cur_actions += [
                SetReshapeAction(
                    node,
                    in_shape=from_shape,
                    out_shape=to_shape
                )
            ]
        else:
            cur_actions += [
                DeleteReshapeAction(
                    node
                )
            ]

        transpose_history = transpose_history + \
            [TransposeHistory(node, node.old_shape.shape,
                              transpose, from_shape, new_transpose)]

        if new_transpose is None:
            try:
                return continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes.copy(), cur_actions.copy(), transpose_history, new_transpose)
            except CantContinueError as ex:
                if transpose is None:
                    raise ex
                info(
                    f"rejected {node.name} - transpose out - does not reverse - inserting transpose {transpose}")
                return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose), EndActionDown(node)], cur_visited_nodes
        transpose = new_transpose

    return continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose)


def continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose):
    for edge in G.out_edges(node.name):
        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.to_node, 'down', edge.to_idx):
            continue
        new_actions, visited_down_nodes = search_down(
            G, edge.to_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge, transpose_history)
        cur_visited_nodes |= visited_down_nodes
        cur_actions += new_actions
    return cur_actions, cur_visited_nodes


def search_up(G, node, exclude_nodes, visited_nodes, out_edge, transpose_history):
    transpose = transpose_history[-1].to_transpose
    cur_visited_nodes = VisitedNodes()
    cur_visited_nodes.visit_up(node, out_edge.from_idx)
    if transpose is not None and len(transpose) == 1:
        return [EndActionUp(node)], cur_visited_nodes
    if isinstance(node, SensitiveToOrder) and transpose_does_nothing(transpose, node.out_dims[out_edge.from_idx].shape):
        new_shape = apply_transpose(
            node.out_dims[out_edge.from_idx].shape, transpose)
        # could be that the transpose does nothing to the data layout but still changes the positions of
        # the ones in the shape
        if new_shape == node.out_dims[out_edge.from_idx].shape:
            return [EndActionUp(node)], cur_visited_nodes
        return [
            InsertReshapeAction(node, direction='out', idx=out_edge.from_idx,
                                in_shape=node.out_dims[out_edge.from_idx].shape, out_shape=new_shape),
            EndActionUp(node)], cur_visited_nodes

    if isinstance(node, SensitiveToOrder):
        check_for_null_transpose(node, transpose)
        info(
            f'rejected {node.name}  - sensitive to order - inserting transpose {transpose}')
        return [InsertTransposeAction(node, direction='out', idx=out_edge.from_idx, transpose=transpose), EndActionUp(node)], cur_visited_nodes

    cur_actions = []

    # first make sure that we visit all other out edges on this node
    # Only a split or an expression will have multiple output indexes otherwise this
    # is multiple edges from the same node.
    # Split will be SensitiveToOrder and Expressions can pass through

    for edge in set(G.out_edges(node.name)) - {out_edge}:
        check_for_null_transpose(node, transpose)
        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.to_node, 'down', edge.to_idx):
            continue
        # We are pushing the transpose up so this node will produce the transposed shape so
        # downward we want to find a transpose that restores the original shape i.e. something
        # that reverses the reversed transpose from the transposed shape.
        new_actions, visited_down_nodes = search_down(
            G, edge.to_node,
            exclude_nodes,
            visited_nodes | cur_visited_nodes,
            edge,
            [TransposeHistory(node, apply_transpose(node.out_dims[edge.from_idx], transpose), reverse_transpose(transpose))])
        cur_visited_nodes |= visited_down_nodes
        cur_actions += new_actions

    # Conditions that can absorb the Transpose

    # FCs can absorb the Transpose by shuffling their weights
    if isinstance(node, (FcParameters, LinearFusionParameters)):
        filter_node = node.contained_filters()[0] if isinstance(node, LinearFusionParameters) else node
        if filter_node.batch_size > 1:
            if transpose == (1, 0):
                info(
                    f"accepted {node.name} - linear layer switch batch dimension")
                return cur_actions + [SwitchBatchLinearAction(node), EndActionUp(node)], cur_visited_nodes
            info(f"rejected {node.name} - batched linear")
            return [InsertTransposeAction(node, direction='out', idx=out_edge.from_idx, transpose=reverse_transpose(transpose)), EndActionUp(node)], cur_visited_nodes
        info(f"accepted {node.name} - linear layer reorder output")
        qrec = G.quantization and G.quantization[NodeId(node)]
        return cur_actions + [ReorderLinearAction.out_from_history(node, transpose_history, qrec), EndActionUp(node)], cur_visited_nodes

    # Transpose may reverse the propagated transpose or be reordered
    if isinstance(node, TransposeParameters):
        check_for_null_transpose(node, transpose)
        # TODO - in_dims or out_dims - 99% sure in_dims
        if reverses_transpose(node.transpose, transpose, node.in_dims[0]):
            info(
                f"accepted {node.name} - transpose {node.transpose} reversed out by {transpose} on {node.in_dims[0]}")
            reshape = requires_reshape(
                node.transpose, transpose, node.out_dims[0])
            if reshape:
                info(f"requires reshape {reshape[0]} -> {reshape[1]}")
            return cur_actions + [DeleteTransposeAction(node, reshape=reshape), EndActionUp(node)], cur_visited_nodes
        # TODO - This should merge with the existing Transpose
        new_transpose = apply_transpose(node.transpose, transpose)
        info(
            f"rejected {node.name} - transpose - does not reverse - absorbing {transpose} into {node.transpose} -> {new_transpose}")
        return [SetTransposeAction(node, new_transpose), EndActionDown(node)], cur_visited_nodes

    # Input can be reordered if not frozen
    if isinstance(node, InputParameters):
        check_for_null_transpose(node, transpose)
        if node.fixed_order:
            info(f"rejected {node.name} - fixed order input")
            return [InsertTransposeAction(node, direction='out', idx=out_edge.from_idx, transpose=reverse_transpose(transpose)), EndActionUp(node)], cur_visited_nodes

        info(
            f"accepted {node.name} - input without fixed order - transpose input {transpose}")
        return cur_actions + [ReorderInputDims.from_history(node, transpose_history, transpose=transpose), EndActionUp(node)], cur_visited_nodes

    # Constant can be reordered
    if isinstance(node, ConstantInputParameters):
        check_for_null_transpose(node, transpose)
        info(
            f"accepted {node.name} - constant input - transpose constant {transpose}")
        return cur_actions + [ReorderConstantInput.from_history(node, transpose_history, transpose=transpose), EndActionUp(node)], cur_visited_nodes

    # Conditions that can pass through the Transpose
    if node.__class__ in TRANSIENT_ACTIONS:
        cur_actions.append(
            TRANSIENT_ACTIONS[node.__class__](node, transpose, "up"))

    elif isinstance(node, ReshapeParameters):
        check_for_null_transpose(node, transpose)  # TODO - may eliminate
        new_transpose = reverse_reshape(transpose, node.shape, node.old_shape)
        # if the upwards shape has one dimension we keep going since we want to find
        # nodes such as a linear layer that can reorder their output filters
        # This could be extended to recurrent layers for the inner dimension
        info(
            f"pass reshape {node.name} up trans: old {transpose} new {new_transpose} shape: old {node.old_shape} new {node.shape}")
        if new_transpose is None and len(node.old_shape) > 1:
            info(f"rejected {node.name} - transpose in - does not reverse")
            return [InsertTransposeAction(node, direction='out', idx=out_edge.from_idx, transpose=reverse_transpose(transpose)), EndActionUp(node)], cur_visited_nodes

        # insert an action to rewrite the reshape shapes
        from_shape = node.old_shape.calc_transpose(
            new_transpose) if new_transpose is not None else None
        to_shape = node.shape.calc_transpose(
            transpose) if transpose is not None else None
        transpose_history = transpose_history + \
            [TransposeHistory(node, node.shape, transpose,
                              to_shape, new_transpose)]
        info(f"rewrite reshape to {from_shape}->{to_shape}")
        if from_shape is None or to_shape is None or from_shape.shape != to_shape.shape:
            cur_actions.extend([
                SetReshapeAction(
                    node,
                    in_shape=from_shape,
                    out_shape=to_shape
                )
            ])
        else:
            cur_actions.extend([
                DeleteReshapeAction(
                    node
                )
            ])
        if new_transpose is None:
            try:
                return continue_up(G, node, exclude_nodes, visited_nodes, cur_visited_nodes.copy(), cur_actions.copy(), transpose_history, transpose) # @IgnoreException
            except CantContinueError as ex:
                if transpose is None:
                    raise ex
                info(f"rejected {node.name} - transpose in - does not reverse")
                return [InsertTransposeAction(node, direction='out', idx=out_edge.from_idx, transpose=reverse_transpose(transpose)), EndActionUp(node)], cur_visited_nodes
        transpose = new_transpose

    # Continue to visit upwards
    return continue_up(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose)


def continue_up(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose):
    for edge in G.in_edges(node.name):
        # if the node could be broadcasting check the edge and insert a reshape and modify the transpose to
        # get rid of the broadcasted axes.
        # If we've already visited then insert both a reshape and a transpose

        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.from_node, 'up', edge.from_idx):
            continue

        if isinstance(node, Broadcastable) and node.in_dims[edge.to_idx].rank != node.out_dims[0].rank:
            max_shape = compute_max_shape(node.out_dims)
            old_shape, shape, new_transpose = broadcast_reduce(
                max_shape, node.in_dims[edge.to_idx].shape, transpose)
            extra_history = [
                TransposeHistory(
                    node, node.in_dims[edge.to_idx].shape, transpose, shape, new_transpose)
            ]
            old_shape = apply_transpose(old_shape, new_transpose)
            shape = apply_transpose(shape, transpose)
            cur_actions.append(InsertReshapeAction(
                node, direction='in', idx=edge.to_idx,
                in_shape=Dim.unnamed(old_shape), out_shape=Dim.unnamed(shape)))
        else:
            new_transpose = transpose
            extra_history = []
        new_actions, visited_up_nodes = search_up(
            G, edge.from_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge,
            transpose_history + extra_history)
        cur_visited_nodes |= visited_up_nodes
        cur_actions += new_actions
    return cur_actions, cur_visited_nodes


def count_eliminated(actions):
    if actions is None:
        return -1
    deleted = sum(1 for action in actions if isinstance(
        action, DeleteTransposeAction))
    added = sum(1 for action in actions if isinstance(
        action, InsertTransposeAction))
    return deleted - added


def apply_actions(G, results: Sequence[Action]):
    for action in results:
        action.execute(G)


def combine_transposes(G):
    transpose_seqs = [trans for trans in G.nodes(node_classes=TransposeParameters)
                      if len(G.out_edges(trans.name)) == 1 and
                      isinstance(G.out_edges(trans.name)[0].to_node, TransposeParameters)]
    for trans in transpose_seqs:
        to_trans = G.out_edges(trans.name)[0].to_node
        new_transpose = apply_transpose(trans.transpose, to_trans.transpose)
        info(
            f'combine transposes {trans.name} and {to_trans.name} {trans.transpose} & {to_trans.transpose} -> {new_transpose}')
        trans.transpose = new_transpose
        G.remove_and_reconnect(to_trans, edge_class=NNEdge)


def remove_silly_reshapes(G):
    silly_reshapes = [node for node in G.nodes(node_classes=ReshapeParameters) if tuple(
        node.old_shape.shape) == tuple(node.shape.shape)]
    for reshape in silly_reshapes:
        G.remove_and_reconnect(reshape, edge_class=NNEdge)


def transpose_moved(G, actions):
    if actions is None:
        return False
    delete_actions = [action for action in actions if isinstance(
        action, DeleteTransposeAction)]
    insert_actions = [action for action in actions if isinstance(
        action, InsertTransposeAction)]
    trans_nodes = []
    for action in insert_actions:
        if action.direction == 'in':
            edge = G.indexed_in_edges(action.node.name)[action.idx]
            if isinstance(edge.from_node, TransposeParameters):
                trans_nodes.append(edge.from_node)
        else:
            edges = G.indexed_out_edges(action.node.name)[action.idx]
            for edge in edges:
                if isinstance(edge.to_node, TransposeParameters):
                    trans_nodes.append(edge.to_node)
    return len([action for action in delete_actions if action.node not in trans_nodes]) > 0

    if not delete_actions or not insert_actions:
        return False


def eliminate_transposes(G, debug_function=None, one_cycle=False):
    info("eliminating unnecessary transposes")
    found_results = True
    while found_results:
        found_results = False
        visited_nodes = set()
        actions = []
        info("search for transposes")
        transposes = G.nodes(node_classes=TransposeParameters)
        while transposes:
            transpose_node = transposes.pop(0)
            if transpose_node in visited_nodes:
                continue

            # search up for elimination
            try:
                in_edge = G.in_edges(transpose_node.name)[0]
                if in_edge.from_node in visited_nodes:
                    raise CantContinueError()  # @IgnoreException
                info(f'trying to eliminate {transpose_node.name} upwards')
                cur_visited_up = VisitedNodes()
                cur_visited_up.visit_up(transpose_node, 0)
                cur_actions_up, cur_visited_up = search_up(
                    G,
                    in_edge.from_node,
                    visited_nodes,
                    cur_visited_up,
                    in_edge,
                    [TransposeHistory(transpose_node, transpose_node.in_dims[0], transpose_node.transpose)])
                # cur_visited_up.append(transpose_node)
            except CantContinueError:
                cur_actions_up = cur_visited_up = None
            if cur_actions_up is not None:
                cur_actions_up.insert(0, DeleteTransposeAction(transpose_node))
            # search down for elimination
            try:
                cur_visited_down = VisitedNodes()
                cur_visited_down.visit_down(transpose_node, 0)
                cur_actions_down = []
                info(f'trying to eliminate {transpose_node.name} downwards')
                for edge in G.out_edges(transpose_node.name):
                    if edge.to_node in visited_nodes:
                        raise CantContinueError()  # @IgnoreException
                    if edge.to_node in cur_visited_down:
                        continue
                    this_actions_down, this_visited_down = search_down(
                        G,
                        edge.to_node,
                        visited_nodes,
                        cur_visited_down,
                        edge,
                        [TransposeHistory(transpose_node, transpose_node.out_dims[0], transpose_node.transpose)])
                    cur_actions_down += this_actions_down
                    cur_visited_down |= this_visited_down
            except CantContinueError:
                cur_actions_down = cur_visited_down = None
            if cur_actions_down is not None:
                cur_actions_down.insert(
                    0, DeleteTransposeAction(transpose_node))

            # evaluate results
            up_count = count_eliminated(cur_actions_up)
            down_count = count_eliminated(cur_actions_down)
            # if the count is zero then the transpose has been eliminated however
            # 1 is better than 0 since another real transpose was deleted rather than a reorder etc
            # always choose up before down since up is where we will transpose constants rather than reshaping them
            if up_count > 0 and up_count >= down_count:
                info(
                    f'found elimination for {transpose_node.name} upwards - {up_count} eliminated')
                found_results = True
                actions += cur_actions_up
                visited_nodes |= set(cur_visited_up.nodes)
            # if transpose cannot be removed upwards movement push the transpose down if it actually moved
            elif down_count > 0 or (down_count == 0 and transpose_moved(G, cur_actions_down)):
                info(
                    f'found elimination for {transpose_node.name} downwards - {down_count} eliminated')
                found_results = True
                actions += cur_actions_down
                visited_nodes |= set(cur_visited_down.nodes)

        if found_results:
            info("eliminate transposes")
            apply_actions(G, actions)
        else:
            info("no transposes to eliminate found")
        remove_silly_reshapes(G)
        combine_transposes(G)
        G.add_dimensions()
        if debug_function:
            debug_function(G)
        if one_cycle:
            LOG.info("cycle complete")
            break
    LOG.info("no further transpose sequences found")
