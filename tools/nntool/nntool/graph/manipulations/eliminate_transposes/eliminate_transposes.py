# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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
from collections import Counter
from collections.abc import MutableSet
from copy import deepcopy
from typing import Iterator, Sequence

import numpy as np
from nntool.graph.manipulations.adjusts.concat import \
    HWC_CONCAT_SPLIT_AXIS_LIMIT
from nntool.graph.manipulations.adjusts.split import split_can_work_on_last
from nntool.graph.types import (ActivationNodeBase, BroadcastableNodeBase,
                                ConcatNode, ConstantInputNode, CopyNode,
                                InputNode, LinearFusionNode, LinearNode,
                                NNEdge, OutputNode, PadNode, PowOpNode,
                                ReshapeNode, ReverseNode,
                                SensitiveToOrderMixin, SplitNode,
                                StridedSliceNode, TransposeNode,
                                UnaryOpNodeBase)
from nntool.graph.types.tensor_arithmetic import (MatMulOpNode,
                                                  MatMulTransposedNode)
from nntool.utils.compatible_transposes import reverse_reshape
from nntool.utils.graph import Node

from .eliminate_transposes_actions import (Action, CantContinueError,
                                           DeleteReshapeAction,
                                           DeleteTransposeAction,
                                           EndActionDown, EndActionUp,
                                           InsertReshapeAction,
                                           InsertTransposeAction,
                                           MatMatTAction, ReorderConstantInput,
                                           ReorderInputDims,
                                           ReorderLinearAction,
                                           SetReshapeAction,
                                           SetTransposeAction,
                                           SwitchBatchLinearAction,
                                           TransposeConcat, TransposePad,
                                           TransposeReverse,
                                           TransposeSlidedSlice,
                                           TransposeSplit)
from .transpose_helpers import (apply_transpose, identity_transpose,
                                last_first_swap, reverse_transpose,
                                reverses_transpose_up, transpose_does_nothing)

LOG = logging.getLogger(__name__)


def info(msg):
    LOG.info(msg)


def debug(msg):
    LOG.debug(msg)


TRANSIENT_ACTIONS = {
    PadNode: TransposePad,
    ReverseNode: TransposeReverse,
    StridedSliceNode: TransposeSlidedSlice,
    ConcatNode: TransposeConcat,
    SplitNode: TransposeSplit,
}

NODES_TO_EXPLORE_UP = {
    ConcatNode,
    BroadcastableNodeBase
}


class TransposeHistory():
    def __init__(self, node, from_shape=None, transpose=None, to_shape=None) -> None:
        self.node = node
        self._from = from_shape
        self._transpose = transpose
        self._to = to_shape

    @property
    def from_shape(self):
        return self._from

    @property
    def to_shape(self):
        return self._to

    @property
    def transpose(self):
        return self._transpose


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
        return x in self._nodes and any(self._nodes[x][0])

    def __iter__(self) -> Iterator[Node]:
        return iter([node for node, visit in self._nodes.items() if visit[0]])

    def __len__(self) -> int:
        return len([node for node, visit in self._nodes.items() if visit[0]])

    def add(self, value: Node) -> None:
        self._nodes[value] = [set(), None]

    def discard(self, value: Node) -> None:
        del self._nodes[value]

    def visit_down(self, node, idx, transpose=None):
        val, trans_set = self._nodes.setdefault(node, [set(), {}])
        if transpose is not None:
            cur_trans = trans_set.setdefault(('down', idx), transpose)
            if cur_trans != transpose:
                raise CantContinueError(f"{node.name} transpose doesn't match")
        val.add(f'down{idx}')

    def visited_down(self, node, idx=None, not_idx=None) -> bool:
        visited, _ = self._nodes.get(node, [set(), {}])
        if idx is None and not_idx is None:
            return any(k.startswith('down') for k in visited)
        if not_idx is not None:
            return bool(set([visit[4:] for visit in visited]) - {str(not_idx)})
        return f'down{idx}' in visited or 'down*' in visited

    def visit_up(self, node, idx, transpose=None):
        val, trans_set = self._nodes.setdefault(node, [set(), {}])
        if transpose is not None:
            cur_trans = trans_set.setdefault(('up', idx), transpose)
            if cur_trans != transpose:
                raise CantContinueError(f"{node.name} transpose doesn't match")
        val.add(f'up{idx}')

    def visited_up(self, node, idx=None) -> set:
        visited, _ = self._nodes.get(node, [set(), {}])
        if idx is None:
            return any(k.startswith('up') for k in visited)
        return f'up{idx}' in visited or 'up*' in visited

    def visited_direction(self, direction, idx, node) -> bool:
        return f'{direction}{idx}' in self._nodes.get(node, [set(), {}])[0]

    def get_transpose(self, direction, idx, node):
        _, trans_set = self._nodes.get(node, [set(), {}])
        return trans_set.get((direction, idx))

    def copy(self):
        return VisitedNodes(nodes_dict=deepcopy(self.nodes))

    @staticmethod
    def merge(tset1, tset2):
        merged = tset1.copy()
        for dir_idx, trans in tset2.items():
            if dir_idx in tset1:
                if trans != merged[dir_idx]:
                    raise CantContinueError()
            else:
                merged[dir_idx] = trans
        return merged

    @staticmethod
    def __or_nodes(s, res):
        if isinstance(s, VisitedNodes):
            for node_name, (visited, trans_set) in s.nodes.items():
                if node_name in res:
                    try:
                        merged = VisitedNodes.merge(trans_set, res[node_name][1])
                    except CantContinueError as ex:
                        raise CantContinueError(
                            f"{node_name} transpose doesn't match") from ex
                    res[node_name] = [res[node_name][0]|visited, merged]
                else:
                    res[node_name] = [visited, trans_set]
        else:
            for node_name in s:
                res[node_name] = [{'up*', 'down*'}, None]

        return res

    def __or__(self, s: 'VisitedNodes') -> 'VisitedNodes':
        res = {k: [visited.copy(), trans_set]
               for k, (visited, trans_set) in self._nodes.items()}
        return VisitedNodes(nodes_dict=self.__or_nodes(s, res))

    def __ror__(self, s: 'VisitedNodes') -> 'VisitedNodes':
        res = {k: [visited.copy(), trans_set]
               for k, (visited, trans_set) in self._nodes.items()}
        return VisitedNodes(nodes_dict=self.__or_nodes(s, res))

    def __ior__(self, s) -> 'VisitedNodes':
        self._nodes = self.__or_nodes(s, self._nodes)
        return self

    def __repr__(self) -> str:
        return "{" + ",".join(f"{repr(node)}: {visited}" for node, (visited, trans_Set) in self._nodes.items()) + "}"


def requires_reshape(trans1, trans2, dim):
    """Checks if layout shape doesn't change but a reshape is necessary due to 1 position"""
    if (tuple(dim.shape) != tuple(dim.layout_shape) and
            dim.layout_shape == dim.calc_transpose(trans1).calc_transpose(trans2).layout_shape):
        from_shape = dim.calc_transpose(trans1)
        to_shape = dim.calc_transpose(trans2)
        if from_shape.shape != to_shape.shape:
            return (from_shape, to_shape)
    return False


def tuple_equal(a, b):
    return tuple(a) == tuple(b)


def check_for_null_transpose(node, transpose):
    if transpose is None:
        raise CantContinueError(
            f"can't continue at {node.name}")  # @IgnoreException


def check_continue(visited_nodes: VisitedNodes, cur_visited_nodes: VisitedNodes, exclude_nodes, node, direction, idx, transpose=None):
    """Checks to see if we should skip visiting node on edge

    Args:
        visited_nodes (VisitedNodes): All nodes visited in previous eliminations
        cur_visited_nodes (VisitedNodes): Nodes visited on this branch
        exclude_nodes (Sequence[Parameters]): Don't visit these nodes
        node (Parameters): Node on edge
        direction (str): direction of visit 'down' or 'up'
        idx (int): edge index

    Raises:
        CantContinueError: Fail this transpose test

    Returns:
        bool: True if skip False if visit
    """
    all_visited = visited_nodes | cur_visited_nodes
    # if the node is sensitive to order then even if we have already visited it down
    # we must visit it up and vice versa so that we maybe insert a reshape/transpose after it
    if not isinstance(node, SensitiveToOrderMixin):
        if direction == 'up' and all_visited.visited_down(node):
            # trying to visit node that was already visited in the other direction.
            return True
        if direction == 'down':
            if all_visited.visited_up(node):
                # trying to visit node that was already visited in the other direction.
                return True
            if isinstance(node, BroadcastableNodeBase) and all_visited.visited_down(node, not_idx=idx):
                # visiting the other edge of a broadcastable node so we have already passed through
                # this node
                return True

    if all_visited.visited_direction(direction, idx, node):
        # if we are visiting an input or output we have already visited the transposes must match
        if transpose is None or transpose != all_visited.get_transpose(direction, idx, node):
            raise CantContinueError(
                f'all_visited {node.name} idx {idx} dir {direction}')  # @IgnoreException
        return True
    if node in exclude_nodes:
        raise CantContinueError(
            f'{node.name} in exclude nodes')  # @IgnoreException
    return False


def strip_leading_dim(shape, dim=1):
    res = list(shape.copy())
    while len(res) > 1 and res[0] == dim:
        res.pop(0)
    return tuple(res)


def compute_max_shape(dims):
    if len(dims) == 1:
        return dims[0]
    shapes = [dim.shape for dim in dims]
    return [max(dims) for dims in zip(*shapes)]


def broadcasted_axes(shape, full_shape):
    return tuple(range(len(full_shape) - len(shape)))


def reduce_dimension(axis, stripped_axes):
    """reduces axis by the number of stripped axes that are less than it"""
    return axis - sum(1 if stripped_axis < axis else 0 for stripped_axis in stripped_axes)


def expand_axes_in_transpose(transpose, num_new_axes):
    """increases axis by the number of new axes"""
    return tuple(list(range(num_new_axes)) + [axis + num_new_axes for axis in transpose])


def strip_axes_from_transpose(transpose, stripped_axes):
    return tuple(reduce_dimension(axis, stripped_axes) for axis in transpose if axis not in stripped_axes)


def insert_transpose_down(node, transpose, in_shape, cur_visited_nodes, in_edge):
    check_for_null_transpose(node, transpose)
    if transpose_does_nothing(reverse_transpose(transpose), in_shape):
        new_shape = apply_transpose(
            in_shape, reverse_transpose(transpose))
        # could be that the transpose does nothing to the data layout but still changes the positions of
        # the ones in the shape
        if new_shape == in_shape:
            return [EndActionDown(node)], cur_visited_nodes
        debug(
            f'accepted {node.name} - transpose does nothing but requires reshape {new_shape}->{in_shape}')
        return [
            InsertReshapeAction(node, direction='in', idx=in_edge.to_idx,
                                in_shape=new_shape, out_shape=in_shape),
            EndActionDown(node)], cur_visited_nodes

    debug(
        f'rejected {node.name}  - sensitive to order - inserting transpose {transpose}')
    return ([InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose),
             EndActionDown(node)], cur_visited_nodes)


def can_pass(G, node, transpose, check_classes=None):
    if check_classes is None:
        # at least cannot pass nodes marked sensitive to order
        check_classes = SensitiveToOrderMixin
    if isinstance(node, ConcatNode):
        # Concats with HWC_CONCAT_SPLIT_AXIS_LIMIT or less inputs can happen on last axis
        valid = len(G.in_edges(node)) <= HWC_CONCAT_SPLIT_AXIS_LIMIT
        shape = tuple(node.out_dims[0].shape)
    elif isinstance(node, SplitNode):
        # Splits with HWC_CONCAT_SPLIT_AXIS_LIMIT or less outputs and no unused outputs
        # can happen on last axis
        valid = split_can_work_on_last(G, node)
        shape = tuple(node.in_dims[0].shape)
    else:
        return not isinstance(node, check_classes)
    check_for_null_transpose(node, transpose)
    return (valid and
            not all(dim == 1 for dim in shape[1:]) and
            last_first_swap(reverse_transpose(transpose), node.axis, shape))


def ambiguous_reshape(shape_from, shape_to):
    if len(shape_from) >= len(shape_to):
        # TODO - needs more work here
        return False
    common = set(shape_from).intersection(set(shape_to))
    from_counts = Counter(shape_from)
    to_counts = Counter(shape_to)
    for shape in common:
        if from_counts[shape] < to_counts[shape]:
            return True
    return False


def search_down(G, node, exclude_nodes, visited_nodes: VisitedNodes, in_edge,
                transpose_history: Sequence[TransposeHistory]):
    """Searches down the graph for something that eliminates transpose

    Args:
        G : The graph
        node : The node to look at
        visited_nodes : Nodes already traversed
        in_edge : The edge we are arriving on at this node
        transpose_history : A history of the reshapes passed that did not allow us
                            to determine the transpose. Transposes
                            are in the downwards direction.

    Returns:
        A tuple of a list of actions and a list of nodes traversed
    """
    cur_visited_nodes = VisitedNodes()
    transpose = transpose_history[-1].transpose
    cur_visited_nodes.visit_down(node, in_edge.to_idx, transpose=transpose)

    in_shape = node.in_dims[in_edge.to_idx].shape
    debug(
        f'down at {node.name} from {in_edge.from_node.name}:{in_edge.from_idx} trans {transpose} shape {in_shape}')
    if transpose is not None and len(transpose) == 1:
        return [EndActionDown(node)], []

    if not can_pass(G, node, transpose):
        return insert_transpose_down(node, transpose, in_shape, cur_visited_nodes, in_edge)

    cur_actions = []

    if isinstance(node, (MatMulOpNode, MatMulTransposedNode)):
        check_for_null_transpose(node, transpose)
        in_edges = G.indexed_in_edges(node)
        # need to be on edge 1
        if in_edge == in_edges[1]:
            new_transpose = transpose[:-2] + \
                (transpose[-1],) + (transpose[-2],)
            # batch is not transposed
            if identity_transpose(new_transpose):
                return [
                    MatMatTAction(node),
                    EndActionDown(node)], cur_visited_nodes
            new_shape = list(in_shape[:-2]) + [in_shape[-1]] + [in_shape[-2]]
            # batch transpose is a reshape
            if transpose_does_nothing(reverse_transpose(new_transpose), new_shape):
                return [
                    InsertReshapeAction(node, direction='in', idx=in_edge.to_idx,
                                        in_shape=apply_transpose(
                                            new_shape, reverse_transpose(new_transpose)),
                                        out_shape=new_shape),
                    MatMatTAction(node),
                    EndActionDown(node)], cur_visited_nodes
            # a transpose is still there
            return [
                InsertTransposeAction(
                    node, transpose=new_transpose, direction='in', idx=1),
                MatMatTAction(node),
                EndActionDown(node)], cur_visited_nodes
        else:
            return insert_transpose_down(node, transpose, in_shape, cur_visited_nodes, in_edge)

    # if arriving on a broadcasted input the transpose needs to be expanded
    # since the transpose is only acting on the broadcasted dimensions no reshape is necessary
    if isinstance(node, (BroadcastableNodeBase, PowOpNode)):

        if len(in_shape) != node.out_dims[0].rank:
            check_for_null_transpose(node, transpose)
            # This could be an expression so need to broadcaset the output
            max_shape = compute_max_shape(node.out_dims)
            b_axes = broadcasted_axes(in_shape, max_shape)

            new_transpose = expand_axes_in_transpose(transpose, len(b_axes))
            new_shape = tuple(([1] * len(b_axes)) + in_shape)
            transpose_history += [
                TransposeHistory(node, in_shape,
                                 new_transpose,
                                 new_shape)
            ]
            transpose = new_transpose
            in_shape = new_shape

    # on nodes where inputs must have the same transpose applied i.e. Binary arithmetic ops etc
    if any(isinstance(node, cls) for cls in NODES_TO_EXPLORE_UP):
        check_for_null_transpose(node, transpose)
        max_shape = compute_max_shape(node.out_dims)
        for edge in G.in_edges(node.name):
            if edge == in_edge:
                continue
            do_continue = check_continue(
                visited_nodes, cur_visited_nodes, exclude_nodes, edge.from_node, 'up', edge.from_idx)
            # continue indicates that we do not need to visit this edge but it may still
            # need a reshape if we have not visited it down
            if do_continue and visited_nodes.visited_down(node, edge.to_idx):
                continue
            # if other edges are broadcasted they may need to be reshaped
            edge_in_shape = node.in_dims[edge.to_idx].shape
            if not edge_in_shape:
                edge_in_shape = [1]
            # no point following scalars
            if np.prod(edge_in_shape) == 1:
                continue
            # different rank so broadcasted
            if len(edge_in_shape) != len(max_shape):
                # strip the broadcasted axis from the transpose
                b_axes = broadcasted_axes(edge_in_shape, max_shape)
                # Transpose moving down through the broadcast and our current transpose is new to old (up)
                # - strip the broadcast off the reversed version (i.e. old to new)
                transpose_without_broadcast = reverse_transpose(strip_axes_from_transpose(
                    reverse_transpose(transpose), b_axes))
                # from shape will be the old shape with the reversed unbroadcasted transpose - i.e. going up
                from_shape = apply_transpose(
                    edge_in_shape, reverse_transpose(transpose_without_broadcast))
                # to shape is the broadcasted input shape with the reverse transpose with the leading ones removed
                broadcasted_shape = ([1] * len(b_axes)) + list(edge_in_shape)
                to_shape = strip_leading_dim(apply_transpose(
                    broadcasted_shape, reverse_transpose(transpose)))
                # if they are not equal insert a reshape
                if from_shape != to_shape:
                    debug(
                        f'{node.name} broadcasted input {edge.to_idx} requires reshape {from_shape}->{to_shape}')
                    cur_actions.append(
                        InsertReshapeAction(
                            node, direction='in', idx=edge.to_idx,
                            in_shape=from_shape,
                            out_shape=to_shape
                        ))
                new_transpose = transpose_without_broadcast
            else:
                new_transpose = transpose

            new_history = [
                TransposeHistory(node, edge_in_shape,
                                 new_transpose, edge_in_shape)
            ]
            if do_continue:
                # no need to visit this. We already visited the out idx that this is connected
                # to (probably on another connection)
                continue
            new_actions, visited_up_nodes = search_up(
                G, edge.from_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge, new_history)
            cur_visited_nodes |= visited_up_nodes
            cur_actions += new_actions

    # Conditions that can absorb the Transpose

    if isinstance(node, (LinearNode, LinearFusionNode)):
        filter_node = node.contained_filters()[0] if isinstance(
            node, LinearFusionNode) else node
        if filter_node.batch_size > 1:
            debug(
                f"rejected {node.name} - multibatch linear layer - inserting transpose {transpose}")
            return [
                InsertTransposeAction(
                    node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                EndActionDown(node)], cur_visited_nodes
        debug(
            f"accepted {node.name} - linear layer reorder input - {transpose}")
        qrec = G.quantization and G.quantization[node.name]
        return cur_actions + [
            ReorderLinearAction.in_from_history(node, transpose_history, qrec),
            EndActionDown(node)], cur_visited_nodes

    if isinstance(node, TransposeNode):
        check_for_null_transpose(node, transpose)
        reverses_transpose, old_shape = reverses_transpose_up(
            transpose, node.transpose, node.out_dims[0])
        if reverses_transpose:
            debug(
                f"accepted {node.name} - transpose {node.transpose} reversed in by {transpose} on {node.in_dims[0]}")
            if old_shape:
                reshape = (old_shape, node.out_dims[0].shape)
                debug(f"requires reshape {reshape[0]} -> {reshape[1]}")
            else:
                reshape = None
            return [DeleteTransposeAction(node, reshape=reshape), EndActionDown(node)], cur_visited_nodes
        new_transpose = apply_transpose(transpose, node.transpose)
        debug(
            f"rejected {node.name} - transpose - does not reverse - absorbing {transpose} "
            f"into {node.transpose} -> {new_transpose}")
        return [
            SetTransposeAction(node, new_transpose),
            EndActionDown(node)], cur_visited_nodes

    if isinstance(node, OutputNode):
        # In None has been reshaoed to something that is incompatible with current transpose
        # need to give up here
        check_for_null_transpose(node, transpose)
        if node.fixed_order:
            debug(
                f"rejected {node.name} - fixed order output - inserting transpose {transpose}")
            return [
                InsertTransposeAction(
                    node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                EndActionDown(node)], cur_visited_nodes
        debug(
            f"accepted {node.name} - output without fixed order - transpose output {transpose}")
        # No change here since the output dimensions will be computed by the shape inference
        return [EndActionDown(node)], cur_visited_nodes

    if isinstance(node, StridedSliceNode) and node.slice_shape != node.out_shape:
        # strided slice that is also reshaping
        check_for_null_transpose(node, transpose)
        if ambiguous_reshape(node.slice_shape, node.out_shape):
            debug(
                f"rejected {node.name} - cannot pass ambiguous reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                    EndActionDown(node)], cur_visited_nodes
        new_transpose, from_shape, to_shape = reverse_reshape(
            transpose, node.slice_shape, node.out_shape)
        if new_transpose is None:
            debug(
                f"rejected {node.name} - cannot pass slice reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                    EndActionDown(node)], cur_visited_nodes

        cur_actions.append(TransposeSlidedSlice(
            node, reverse_transpose(transpose), out_shape=to_shape, dir="down"))

        if identity_transpose(new_transpose):
            return cur_actions + [EndActionDown(node)], cur_visited_nodes

        transpose_history = transpose_history + \
            [TransposeHistory(node, node.slice_shape,
                              new_transpose, node.out_shape)]
        transpose = new_transpose
    elif node.__class__ in TRANSIENT_ACTIONS:
        check_for_null_transpose(node, transpose)
        cur_actions.append(TRANSIENT_ACTIONS[node.__class__](
            node, reverse_transpose(transpose), "down"))
    elif isinstance(node, ReshapeNode):
        # TODO - Might be able to get rid of this and check history
        check_for_null_transpose(node, transpose)
        if ambiguous_reshape(node.old_shape, node.shape):
            debug(
                f"rejected {node.name} - ambiguous reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                    EndActionDown(node)], cur_visited_nodes

        new_transpose, from_shape, to_shape = reverse_reshape(
            transpose, node.old_shape, node.shape)
        debug(
            f"pass reshape {node.name} down trans: old {transpose} new {new_transpose} "
            f"shape: old {node.old_shape} new {node.shape}")

        if new_transpose is None and len(node.shape) > 1:
            debug(
                f"rejected {node.name} - cannot pass reshape - inserting transpose {transpose}")
            return [
                InsertTransposeAction(
                    node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                EndActionDown(node)], cur_visited_nodes

        # insert an action to rewrite the reshape shapes
        debug(f"rewrite reshape to {from_shape}->{to_shape}")
        if from_shape is None or to_shape is None or from_shape != to_shape:
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

        if identity_transpose(new_transpose):
            return cur_actions + [EndActionDown(node)], cur_visited_nodes

        transpose_history = transpose_history + \
            [TransposeHistory(node, node.old_shape.shape,
                              new_transpose, node.shape.shape)]

        if new_transpose is None:
            try:
                return continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes.copy(),
                                     cur_actions.copy(), transpose_history, new_transpose)
            except CantContinueError as ex:
                if transpose is None:
                    raise ex
                debug(
                    f"rejected {node.name} - cannot continue {ex} - inserting transpose {transpose}")
                return [
                    InsertTransposeAction(
                        node, direction='in', idx=in_edge.to_idx, transpose=transpose),
                    EndActionDown(node)], cur_visited_nodes
        transpose = new_transpose

    return continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes,
                         cur_actions, transpose_history, transpose)


def continue_down(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose):
    for edge in G.out_edges(node.name):
        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.to_node, 'down', edge.to_idx):
            continue
        new_actions, visited_down_nodes = search_down(
            G, edge.to_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge, transpose_history.copy())
        cur_visited_nodes |= visited_down_nodes
        cur_actions += new_actions
    debug(f'-- finish down {node.name}')
    return cur_actions, cur_visited_nodes


def insert_transpose_up(node, transpose, cur_visited_nodes, out_edge):
    if transpose_does_nothing(reverse_transpose(transpose), node.out_dims[out_edge.from_idx].shape):
        new_shape = apply_transpose(
            node.out_dims[out_edge.from_idx].shape, reverse_transpose(transpose))
        # could be that the transpose does nothing to the data layout but still changes the positions of
        # the ones in the shape
        if new_shape == node.out_dims[out_edge.from_idx].shape:
            debug(
                f'accepted {node.name} - transpose does nothing')
            return [EndActionUp(node)], cur_visited_nodes

        debug(
            f'accepted {node.name} - transpose does nothing with reshape '
            f'{node.out_dims[out_edge.from_idx].shape} -> {new_shape}')
        return [
            InsertReshapeAction(node, direction='out', idx=out_edge.from_idx, out_edge=out_edge,
                                in_shape=node.out_dims[out_edge.from_idx].shape, out_shape=new_shape),
            EndActionUp(node)], cur_visited_nodes

    check_for_null_transpose(node, transpose)
    debug(
        f'rejected {node.name}  - sensitive to order - inserting transpose {transpose}')
    return [
        InsertTransposeAction(node, direction='out', idx=out_edge.from_idx,
                              out_edge=out_edge, transpose=reverse_transpose(transpose)),
        EndActionUp(node)], cur_visited_nodes


def search_up(G, node, exclude_nodes, visited_nodes, out_edge, transpose_history):
    transpose = transpose_history[-1].transpose
    debug(
        f'up at {node.name} from {out_edge.to_node.name}:{out_edge.to_idx} trans {transpose}')
    cur_visited_nodes = VisitedNodes()
    cur_visited_nodes.visit_up(node, out_edge.from_idx, transpose=transpose)
    if transpose is not None and len(transpose) == 1:
        debug(
            f'accepted {node.name} - single dimension transpose')
        return [EndActionUp(node)], cur_visited_nodes

    if not can_pass(G, node, transpose,
                    check_classes=(
                        SensitiveToOrderMixin, MatMulTransposedNode, MatMulOpNode)):
        return insert_transpose_up(node, transpose, cur_visited_nodes, out_edge)

    cur_actions = []

    # first make sure that we visit all other out edges on this node
    # Only a split or an expression will have multiple output indexes otherwise this
    # is multiple edges from the same node.
    # Split will be SensitiveToOrder and Expressions can pass through - Expressions always have their outputs expanded to
    # the max broadcast so we don't need to worry about them.

    for edge in G.out_edges(node.name):
        if edge == out_edge:
            continue
        check_for_null_transpose(node, transpose)
        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes, edge.to_node, 'down', edge.to_idx):
            continue
        new_actions, visited_down_nodes = search_down(
            G, edge.to_node,
            exclude_nodes,
            visited_nodes | cur_visited_nodes,
            edge,
            transpose_history + [
                TransposeHistory(node, node.out_dims[edge.from_idx], transpose,
                                 apply_transpose(node.out_dims[edge.from_idx], transpose))])
        cur_visited_nodes |= visited_down_nodes
        cur_actions += new_actions

    # Conditions that can absorb the Transpose

    # FCs can absorb the Transpose by shuffling their weights
    if isinstance(node, (LinearNode, LinearFusionNode)):
        filter_node = node.contained_filters()[0] if isinstance(
            node, LinearFusionNode) else node
        if filter_node.batch_size > 1:
            if transpose == (1, 0):
                debug(
                    f"accepted {node.name} - linear layer switch batch dimension")
                return cur_actions + [SwitchBatchLinearAction(node), EndActionUp(node)], cur_visited_nodes
            debug(f"rejected {node.name} - batched linear")
            return [
                InsertTransposeAction(node, direction='out', idx=out_edge.from_idx,
                                      out_edge=out_edge, transpose=reverse_transpose(transpose)),
                EndActionUp(node)], cur_visited_nodes
        debug(f"accepted {node.name} - linear layer reorder output")
        qrec = G.quantization and G.quantization[node.name]
        return cur_actions + [
            ReorderLinearAction.out_from_history(
                node, transpose_history, qrec),
            EndActionUp(node)], cur_visited_nodes

    # Transpose may reverse the propagated transpose or be reordered
    if isinstance(node, TransposeNode):
        check_for_null_transpose(node, transpose)
        if tuple(node.transpose) == tuple(transpose):
            debug(
                f"accepted {node.name} - transpose {node.transpose} equals {transpose} on {node.in_dims[0]}")
            reshape = requires_reshape(
                node.transpose, transpose, node.out_dims[0])
            if reshape:
                debug(f"requires reshape {reshape[0]} -> {reshape[1]}")
            return cur_actions + [
                DeleteTransposeAction(node, reshape=reshape), EndActionUp(node)], cur_visited_nodes

        # absorb transpose in a -> tranpose T1 -> b -> existing trans node T2 -> c
        # a -> TNew -> c
        # Apply reversed T1 to T2

        new_transpose = apply_transpose(
            node.transpose, reverse_transpose(transpose))
        debug(
            f"rejected {node.name} - transpose - does not reverse - absorbing "
            f"{transpose} into {node.transpose} -> {new_transpose}")
        return [SetTransposeAction(node, new_transpose), EndActionDown(node)], cur_visited_nodes

    # Input can be reordered if not frozen
    if isinstance(node, InputNode):
        check_for_null_transpose(node, transpose)
        if node.fixed_order:
            debug(f"rejected {node.name} - fixed order input")
            return [
                InsertTransposeAction(node, direction='out', idx=out_edge.from_idx,
                                      out_edge=out_edge, transpose=transpose), EndActionUp(node)], cur_visited_nodes

        debug(
            f"accepted {node.name} - input without fixed order - transpose input {reverse_transpose(transpose)}")
        return cur_actions + [
            ReorderInputDims.from_history(
                node, transpose_history, transpose=reverse_transpose(transpose)),
            EndActionUp(node)], cur_visited_nodes

    # Constant can be reordered
    if isinstance(node, ConstantInputNode):
        check_for_null_transpose(node, transpose)
        debug(
            f"accepted {node.name} - constant input - transpose constant {transpose}")
        return cur_actions + [
            ReorderConstantInput.from_history(
                node, transpose_history, transpose=reverse_transpose(transpose)),
            EndActionUp(node)], cur_visited_nodes

    # Conditions that can pass through the Transpose
    if isinstance(node, StridedSliceNode) and node.changes_shape:
        # special case for a strided slice that also has a reshape
        check_for_null_transpose(node, transpose)
        if ambiguous_reshape(node.out_shape, node.slice_shape):
            debug(
                f"rejected {node.name} - cannot pass ambiguous reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='out', idx=0, transpose=reverse_transpose(transpose)),
                    EndActionDown(node)], cur_visited_nodes
        new_transpose, from_shape, to_shape = reverse_reshape(
            transpose, node.slice_shape, node.out_shape, going_up=True)
        if new_transpose is None:
            debug(
                f"rejected {node.name} - cannot pass slice reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='out', idx=0, transpose=reverse_transpose(transpose)),
                    EndActionDown(node)], cur_visited_nodes

        cur_actions.append(TransposeSlidedSlice(
            node, reverse_transpose(transpose), out_shape=to_shape))

        if identity_transpose(new_transpose):
            return cur_actions + [EndActionUp(node)], cur_visited_nodes

        transpose_history = transpose_history + \
            [TransposeHistory(node, node.out_shape,
                              new_transpose, node.in_dims[0].shape)]
        transpose = new_transpose
    elif node.__class__ in TRANSIENT_ACTIONS:
        check_for_null_transpose(node, transpose)

        cur_actions.append(
            TRANSIENT_ACTIONS[node.__class__](node, reverse_transpose(transpose), "up"))

    elif isinstance(node, ReshapeNode):
        check_for_null_transpose(node, transpose)  # TODO - may eliminate
        if ambiguous_reshape(node.shape, node.old_shape):
            debug(
                f"rejected {node.name} - cannot pass ambiguous reshape - inserting transpose {transpose}")
            return [InsertTransposeAction(node, direction='out', idx=0, transpose=reverse_transpose(transpose)),
                    EndActionDown(node)], cur_visited_nodes

        # reversed transpose is being propagated up
        new_transpose, from_shape, to_shape = reverse_reshape(
            transpose, node.old_shape, node.shape, going_up=True)
        # if the upwards shape has one dimension we keep going since we want to find
        # nodes such as a linear layer that can reorder their output filters
        # This could be extended to recurrent layers for the inner dimension
        debug(
            f"pass reshape {node.name} up trans: old {transpose} new {new_transpose} "
            f"shape: {node.old_shape} -> {node.shape}")
        if new_transpose is None and len(node.old_shape) > 1:
            debug(
                f"rejected {node.name} - cannot pass reshape - inserting transpose {transpose}")
            # since we are going up the transpose is in the up direction so needs to be reversed
            return [
                InsertTransposeAction(node, direction='out', idx=out_edge.from_idx,
                                      out_edge=out_edge, transpose=reverse_transpose(transpose)),
                EndActionUp(node)], cur_visited_nodes

        # insert an action to rewrite the reshape shapes
        transpose_history = transpose_history + \
            [TransposeHistory(node, node.shape, new_transpose, node.old_shape)]
        debug(f"rewrite reshape to {from_shape}->{to_shape}")
        if from_shape is None or to_shape is None or from_shape != to_shape:
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

        if identity_transpose(new_transpose):
            return cur_actions + [EndActionUp(node)], cur_visited_nodes

        if new_transpose is None:
            try:
                return continue_up(  # @IgnoreException
                    G, node, exclude_nodes, visited_nodes, cur_visited_nodes.copy(),
                    cur_actions.copy(), transpose_history, transpose)
            except CantContinueError as ex:
                if transpose is None:
                    raise ex
                debug(
                    f"rejected {node.name} - cannot continue {ex} - inserting transpose {transpose}")
                return [
                    InsertTransposeAction(node, direction='out', idx=out_edge.from_idx,
                                          out_edge=out_edge, transpose=reverse_transpose(transpose)),
                    EndActionUp(node)], cur_visited_nodes
        transpose = new_transpose

    # Continue to visit upwards
    return continue_up(G, node, exclude_nodes, visited_nodes, cur_visited_nodes,
                       cur_actions, transpose_history, transpose)


def continue_up(G, node, exclude_nodes, visited_nodes, cur_visited_nodes, cur_actions, transpose_history, transpose):
    for edge in G.in_edges(node.name):
        # if the node could be broadcasting check the edge and insert a reshape and modify the transpose to
        # get rid of the broadcasted axes.
        # If we've already visited then insert both a reshape and a transpose

        edge_in_shape = node.in_dims[edge.to_idx].shape
        if not edge_in_shape:
            edge_in_shape = [1]
        if np.prod(edge_in_shape) == 1:
            continue
        if isinstance(node, (BroadcastableNodeBase, PowOpNode)) and len(edge_in_shape) != node.out_dims[0].rank:
            max_shape = compute_max_shape(node.out_dims)
            b_axes = broadcasted_axes(edge_in_shape, max_shape)

            # the broadcast is in the 'down' direction. Here we are moving up with a transpose that is in
            # the 'up' direction i.e. transforming new -> old shape so we need to convert this to a 'down' transpose to
            # remove the broadcasted axis
            broadcasted_transpose_down = reverse_transpose(transpose)
            # get rid of broadcasted axes
            transpose_without_broadcast_down = strip_axes_from_transpose(
                broadcasted_transpose_down,
                b_axes)
            # new up transpose
            transpose_without_broadcast = reverse_transpose(
                transpose_without_broadcast_down)

            # from shape on edge will be the old shape with the unbroadcasted old->new transpose
            from_shape = apply_transpose(
                edge_in_shape, transpose_without_broadcast_down)

            # put back the broadcast (as None's) in the in_shape
            broadcasted_shape = ([None] * len(b_axes)) + list(edge_in_shape)

            # old -> new transpose then strip Nones from start
            to_shape = strip_leading_dim(apply_transpose(
                broadcasted_shape, broadcasted_transpose_down), dim=None)
            # Other None's need to be 1's
            to_shape = [1 if dim is None else dim for dim in to_shape]

            # if they are not equal insert a reshape
            if not tuple_equal(from_shape, to_shape):
                debug(
                    f'{node.name} broadcasted input {edge.to_idx} requires reshape {from_shape}->{to_shape}')
                cur_actions.append(
                    InsertReshapeAction(
                        node, direction='in', idx=edge.to_idx,
                        in_shape=from_shape,
                        out_shape=to_shape
                    ))
            if not tuple_equal(transpose_without_broadcast, transpose):
                extra_history = [
                    TransposeHistory(
                        node, broadcasted_shape, transpose_without_broadcast, edge_in_shape)
                ]
        else:
            extra_history = []
        cur_history = transpose_history + extra_history
        transpose = cur_history[-1].transpose
        if check_continue(visited_nodes, cur_visited_nodes, exclude_nodes,
                          edge.from_node, 'up', edge.from_idx,
                          transpose=transpose):
            continue
        new_actions, visited_up_nodes = search_up(
            G, edge.from_node, exclude_nodes, visited_nodes | cur_visited_nodes, edge,
            cur_history)
        cur_visited_nodes |= visited_up_nodes
        cur_actions += new_actions
    debug(f'-- finish up {node.name}')
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


def search_edge_down(G, edge, pass_classes, stop_class):
    node = edge.to_node
    if isinstance(node, stop_class):
        return node
    elif isinstance(node, pass_classes):
        edges = G.out_edges(node)
        if len(edges) != 1:
            return None
        return search_edge_down(G, edges[0], pass_classes, stop_class)
    else:
        return None


def find_sequences(G, end_node_class, middle_node_classes):
    nodes = set(G.nodes(node_classes=end_node_class))
    pairs = []
    while nodes:
        node = nodes.pop()
        edges = G.out_edges(node)
        if len(edges) != 1:
            continue
        end_node = search_edge_down(
            G, edges[0], middle_node_classes, end_node_class)
        if end_node is None:
            continue
        pairs.append((node, end_node))
        if end_node in nodes:
            nodes.remove(end_node)
    return pairs


def combine_transposes(G):
    trans_pairs = find_sequences(
        G,
        TransposeNode,
        (CopyNode, UnaryOpNodeBase, ActivationNodeBase))

    for tstart, tend in trans_pairs:
        new_transpose = apply_transpose(tstart.transpose, tend.transpose)
        debug(
            f'combine transposes {tstart.name} and {tend.name} {tstart.transpose} & '
            f'{tend.transpose} -> {new_transpose}')
        tstart.transpose = new_transpose
        G.remove_and_reconnect(tend, edge_class=NNEdge)


def combine_reshapes(G):
    reshape_pairs = find_sequences(
        G,
        ReshapeNode,
        (CopyNode, UnaryOpNodeBase, ActivationNodeBase))

    for rstart, rend in reshape_pairs:
        debug(
            f'combine reshapes {rstart.name} and {rend.name} {rstart.shape} & {rend.shape}')
        rstart.shape = rend.shape
        G.remove_and_reconnect(rend, edge_class=NNEdge)


def remove_silly_reshapes(G):
    silly_reshapes = [node for node in G.nodes(node_classes=ReshapeNode) if tuple(
        node.old_shape.shape) == tuple(node.shape.shape)]
    for reshape in silly_reshapes:
        G.remove_and_reconnect(reshape, edge_class=NNEdge)


def transpose_moved(G, actions):
    if actions is None:
        return False
    # when this is called we will have an equal amount of deletes and inserts so figure out if we are actually moving
    # something downwards i.e. increase in step_idx
    insert_steps = sum(insert_step_idx(G, action)
                       for action in actions if isinstance(action, InsertTransposeAction))
    delete_steps = sum(delete_step_idx(G, action)
                       for action in actions if isinstance(action, DeleteTransposeAction))
    return insert_steps > delete_steps


def insert_step_idx(G, action: InsertTransposeAction):
    if action.direction == "in":
        edge = G.indexed_in_edges(action.node)[action.idx]
        # skip past transposes since this could be the one that we are deleting
        while isinstance(edge.from_node, TransposeNode):
            edge = G.in_edges(edge.from_node)[0]
        return edge.from_node.step_idx
    else:
        return action.node.step_idx


def delete_step_idx(G, action: DeleteTransposeAction):
    return G.in_edges(action.node)[0].from_node.step_idx


def eliminate_transposes(G, debug_function=None, steps=None, single_step=False, do_silly=True, only_up=False):
    debug("eliminating unnecessary transposes")
    found_results = True
    pass_count = 0
    # keep trying to eliminate until we can't do more
    # This should not loop since there is a bias in pushing transposes down
    while found_results:
        if steps is not None:
            if pass_count >= steps:
                break
        else:
            if pass_count >= 50:
                raise ValueError(
                    "Sorry, eliminate transposes seems to be stuck in a loop. Please report to GreenWaves.")
        pass_count += 1
        found_results = False
        visited_nodes = set()
        actions = []
        debug(f"search for transposes +++ STEP {pass_count}")
        transposes = sorted(
            G.nodes(node_classes=TransposeNode), key=lambda node: node.name)
        while transposes:
            transpose_node = transposes.pop(0)
            if transpose_node in visited_nodes:
                continue
            debug(f'++ trying to eliminate {transpose_node.name}')

            # search up for elimination
            try:
                in_edge = G.in_edges(transpose_node.name)[0]
                if in_edge.from_node in visited_nodes:
                    raise CantContinueError(
                        f'{in_edge.from_node.name} in visited nodes')  # @IgnoreException
                debug(f'trying to eliminate {transpose_node.name} upwards')
                cur_visited_up = VisitedNodes()
                cur_visited_up.visit_up(
                    transpose_node, 0, transpose=reverse_transpose(transpose_node.transpose))
                cur_actions_up, cur_visited_up = search_up(
                    G,
                    in_edge.from_node,
                    visited_nodes,
                    cur_visited_up,
                    in_edge,
                    [TransposeHistory(  # When going up transpose is reversed
                        transpose_node,
                        transpose_node.out_dims[0].shape,
                        reverse_transpose(transpose_node.transpose),
                        transpose_node.in_dims[0].shape)])
                # cur_visited_up.append(transpose_node)
            except CantContinueError as ex:
                debug(f"can't continue error - {ex}")
                cur_actions_up = cur_visited_up = None
            if cur_actions_up is not None:
                cur_actions_up.insert(0, DeleteTransposeAction(transpose_node))
            elif transpose_does_nothing(transpose_node.transpose, transpose_node.in_dims[0].shape):
                # if the transpose was not absorbed up (into a constant for example) and it
                # does nothing forget trying more
                continue
            # search down for elimination
            try:
                if only_up:
                    raise CantContinueError(f'only up set')
                cur_visited_down = VisitedNodes()
                cur_visited_down.visit_down(
                    transpose_node, 0, transpose=transpose_node.transpose)
                cur_actions_down = []
                debug(f'trying to eliminate {transpose_node.name} downwards')
                for edge in G.out_edges(transpose_node.name):
                    if edge.to_node in visited_nodes:
                        raise CantContinueError(
                            f'{edge.to_node.name} in visited nodes')  # @IgnoreException
                    if check_continue(visited_nodes, cur_visited_down, visited_nodes,
                                      edge.to_node, 'down', edge.to_idx):
                        continue
                    this_actions_down, this_visited_down = search_down(
                        G,
                        edge.to_node,
                        visited_nodes,
                        cur_visited_down,
                        edge,
                        [TransposeHistory(
                            transpose_node,
                            transpose_node.in_dims[0].shape,
                            transpose_node.transpose,
                            transpose_node.out_dims[0].shape)])
                    cur_actions_down += this_actions_down
                    cur_visited_down |= this_visited_down
            except CantContinueError as ex:
                debug(f"can't continue error - {ex}")
                cur_actions_down = cur_visited_down = None
            if cur_actions_down is not None:
                cur_actions_down.insert(
                    0, DeleteTransposeAction(transpose_node))

            debug(f'++ evaluate elimination of {transpose_node.name}')
            # evaluate results
            up_count = count_eliminated(cur_actions_up)
            down_count = count_eliminated(cur_actions_down)
            # if the count is zero then the transpose has been eliminated however
            # 1 is better than 0 since another real transpose was deleted rather than a reorder etc
            # always favor up before down since up is where we will transpose constants
            if up_count > 0 and up_count >= down_count:
                info(
                    f'found elimination for {transpose_node.name} upwards - {up_count} eliminated')
                found_results = True
                actions += cur_actions_up
                visited_nodes |= set(cur_visited_up.nodes)
                visited_nodes.add(transpose_node)
                if single_step or steps is not None:
                    break
            # if transpose cannot be removed upwards push the transpose down if it actually moved
            elif down_count > 0 or (down_count == 0 and transpose_moved(G, cur_actions_down)):
                info(
                    f'found elimination for {transpose_node.name} downwards - {down_count} eliminated')
                found_results = True
                actions += cur_actions_down
                visited_nodes |= set(cur_visited_down.nodes)
                visited_nodes.add(transpose_node)
                if single_step or steps is not None:
                    break
            else:
                debug(f'no elimination for {transpose_node.name} found')

        if found_results:
            info("eliminate transposes")
            apply_actions(G, actions)
        else:
            info("no transposes to eliminate found")
        if do_silly:
            G.add_dimensions()
            remove_silly_reshapes(G)
            combine_reshapes(G)
            combine_transposes(G)
        G.add_dimensions()
        if debug_function:
            debug_function(G)
    LOG.info("no further transpose sequences found")
