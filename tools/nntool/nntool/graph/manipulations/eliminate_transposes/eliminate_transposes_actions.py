# Copyright (C) 2020, 2022  GreenWaves Technologies, SAS

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
from abc import ABC, abstractmethod
from copy import deepcopy

import numpy as np
from nntool.graph.dim import Dim
from nntool.graph.manipulations.eliminate_transposes.transpose_helpers import (
    apply_transpose, reverse_transpose)
from nntool.graph.types import (ConcatNode, ConstantInputNode,
                                LinearFusionNode, MatMulOpNode,
                                MatMulTransposedNode, NNEdge, ReshapeNode,
                                SplitNode, TransposeNode)
from nntool.utils.graph import GraphView

LOG = logging.getLogger(__name__)


class CantContinueError(Exception):
    pass


def info(msg):
    LOG.info(msg)


def debug(msg):
    LOG.debug(msg)


class Action(ABC):
    def __init__(self, node) -> None:
        self.node = node

    @abstractmethod
    def _execute(self, node, G):
        pass

    def execute(self, G):
        self._execute(self.node, G)


class DebugActionBase(Action):
    def __init__(self, node, message=None) -> None:
        super(DebugActionBase, self).__init__(node)
        if message is None:
            message = ""
        self.message = message

    def _execute(self, node, G):
        debug(f"{self}")


class StartAction(DebugActionBase):
    def __str__(self) -> str:
        return "Start Action (%s): %s" % (self.message, self.node.name)


class StartActionUp(StartAction):
    def __init__(self, node) -> None:
        super(StartActionUp, self).__init__(node, "up")


class StartActionDown(StartAction):
    def __init__(self, node) -> None:
        super(StartActionDown, self).__init__(node, "down")


class EndAction(DebugActionBase):
    def __str__(self) -> str:
        return "End Action (%s): %s" % (self.message, self.node.name)


class EndActionUp(EndAction):
    def __init__(self, node) -> None:
        super(EndActionUp, self).__init__(node, "up")


class EndActionDown(EndAction):
    def __init__(self, node) -> None:
        super(EndActionDown, self).__init__(node, "down")


class InsertNodeAction(Action):
    def __init__(self, node, direction=None, idx=None, out_edge=None):
        super(InsertNodeAction, self).__init__(node)
        self.direction = direction
        self.idx = idx
        self.out_edge = out_edge

    def do_insert(self, node, G, params, direction=None):
        if direction is None:
            direction = self.direction
        if direction == 'in':
            G.insert_node_before(
                params, node, to_idx=self.idx, edge_class=NNEdge)
            if G.quantization:
                G.quantization.copy_qrec(node, 'in', self.idx, params)
        else:
            if self.out_edge and G.edge_in_graph(self.out_edge):
                G.insert_node_at_edge(params, self.out_edge, edge_class=NNEdge)
            else:
                G.insert_node_after(
                    node, params, from_idx=self.idx, edge_class=NNEdge)
            if G.quantization:
                G.quantization.copy_qrec(node, 'out', self.idx, params)


class InsertTransposeAction(InsertNodeAction):
    def __init__(self, node, transpose=None, reshape_from=None, reshape_to=None, **kwargs) -> None:
        super(InsertTransposeAction, self).__init__(node, **kwargs)
        self.transpose = transpose
        self.reshape_from = reshape_from
        self.reshape_to = reshape_to

    def _execute(self, node, G):
        debug(f"{self}")
        direction = self.direction
        if self.reshape_from is not None:
            params = ReshapeNode(G.unique_name(
                f'{node.name}_reshape'),
                old_shape=Dim.unnamed(self.reshape_from),
                shape=Dim.reshape_to(self.reshape_to))
            self.do_insert(node, G, params, direction=direction)
            node = params
            direction = "out"

        params = TransposeNode(G.unique_name(
            f'{node.name}_trans'), transpose=self.transpose)
        self.do_insert(node, G, params, direction=direction)

    def __str__(self) -> str:
        reshape = f" reshape {self.reshape_from} to {self.reshape_to} and " if self.reshape_from is not None else ""
        return f"insert {reshape} transpose at {self.node.name}:{self.direction}_{self.idx} trans {self.transpose}"


class InsertReshapeAction(InsertNodeAction):
    def __init__(self, node, in_shape=None, out_shape=None, **kwargs) -> None:
        super(InsertReshapeAction, self).__init__(node, **kwargs)
        assert in_shape is not None and out_shape is not None, 'find test'
        if isinstance(in_shape, (list, tuple)):
            self.in_shape = Dim.unnamed(in_shape)
        else:
            self.in_shape = in_shape.clone() if in_shape is not None else None
        if isinstance(out_shape, (list, tuple)):
            self.out_shape = Dim.unnamed(out_shape)
        else:
            self.out_shape = out_shape.clone() if out_shape is not None else None

    def _get_constant_in_edge(self, node, G):
        if self.direction != 'in':
            return None
        in_edge = G.indexed_in_edges(node)[self.idx]
        if isinstance(in_edge.from_node, ConstantInputNode):
            return in_edge.from_node
        return None

    def _execute(self, node, G):
        debug(f"{self}")
        const = self._get_constant_in_edge(node, G)
        if const:
            const.reshape(self.out_shape)
        else:
            params = ReshapeNode(G.unique_name(
                f'{node.name}_reshape'), old_shape=self.in_shape, shape=self.out_shape)
            self.do_insert(node, G, params)

    def __str__(self) -> str:
        return f"insert reshape at {self.node.name}:{self.direction}_{self.idx} in {self.in_shape} out {self.out_shape}"


def make_dim(shape):
    if shape is None:
        return shape
    if isinstance(shape, Dim):
        return shape.clone()
    return Dim.unnamed(shape)


class SetReshapeAction(Action):
    def __init__(self, node, in_shape=None, out_shape=None) -> None:
        super(SetReshapeAction, self).__init__(node)
        self.in_shape = make_dim(in_shape)
        self.out_shape = make_dim(out_shape)

    def _execute(self, node, G):
        debug(f"{self}")
        if self.in_shape is not None:
            node.old_shape = self.in_shape
        if self.out_shape is not None:
            node.shape = self.out_shape

    def __str__(self) -> str:
        return "%s set reshape in %s out %s" % (self.node.name, self.in_shape, self.out_shape)


class SwitchBatchLinearAction(Action):
    def _execute(self, node, G):
        debug(f"{self}")
        self.node.batch_minor = not self.node.batch_minor

    def __str__(self) -> str:
        return f"{self.node.name} switch to batch {'major' if self.node.batch_minor else 'minor'}"

class TransposeConcat(Action):
    def __init__(self, node: ConcatNode, transpose, dir=None) -> None:
        super(TransposeConcat, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        debug(f"{self}")
        if node.axis not in self.transpose:
            raise ValueError(f'{node.name} axis {node.axis} not in transpose {self.transpose}')
        node.axis = self.transpose.index(node.axis)

    def __str__(self) -> str:
        return f"{self.node.name} transpose concat parameters with {self.transpose}"

class TransposeSplit(Action):
    def __init__(self, node: SplitNode, transpose, dir=None) -> None:
        super(TransposeSplit, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        debug(f"{self}")
        node.act_slices = tuple([apply_transpose(sls, self.transpose) for sls in node.act_slices])
        node.out_shapes = tuple([apply_transpose(shape, self.transpose) for shape in node.out_shapes])
        node.axis = self.transpose.index(node.axis)

    def __str__(self) -> str:
        return f"{self.node.name} transpose split parameters with {self.transpose}"


class TransposeSlidedSlice(Action):
    def __init__(self, node, transpose, dir=None, out_shape=None) -> None:
        super(TransposeSlidedSlice, self).__init__(node)
        self.transpose = tuple(transpose)
        self.shape_out = out_shape

    def _execute(self, node, G):
        debug(f"{self}")
        node.act_slice = apply_transpose(node.act_slice, self.transpose)
        if self.shape_out is not None:
            node.out_shape = self.shape_out
        else:
            node.out_shape = apply_transpose(node.out_shape, self.transpose)

    def __str__(self) -> str:
        out_shape = "unchanged" if self.shape_out is None else f"changed to {self.shape_out}"
        return f"{self.node.name} transpose slided slice parameters with {self.transpose} out shape {out_shape}"


class TransposePad(Action):
    def __init__(self, node, transpose, dir=None) -> None:
        super(TransposePad, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        debug(f"{self}")
        node.padding = [node.padding[idx] for idx in self.transpose]
        node.pad_vals = [node.pad_vals[idx] for idx in self.transpose]

    def __str__(self) -> str:
        return "%s transpose pad parameters with %s" % (self.node.name, self.transpose)


class TransposeAxisBase(Action):
    def __init__(self, node, transpose, dir=None) -> None:
        super(TransposeAxisBase, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        debug(f"{self}")
        node.axis = self.transpose[node.axis]


class TransposeReverse(TransposeAxisBase):
    def __str__(self) -> str:
        return "%s transpose reverse parameters with %s" % (self.node.name, self.transpose)


class TransposeGlobalPool(TransposeAxisBase):
    def __str__(self) -> str:
        return "%s transpose global pool parameters with %s" % (self.node.name, self.transpose)


class TransposeInputBase(Action):
    def __init__(self, node, transpose, dir=None) -> None:
        super(TransposeInputBase, self).__init__(node)
        self.transpose = tuple(transpose)

    @abstractmethod
    def _execute(self, node, G):
        pass

    @classmethod
    def from_history(cls, node, history, transpose=None):
        if transpose is not None:
            return cls(node, transpose)
        # Find the first entry in the transpose history that actually has a transpose
        first_valid_entry = next(iter([entry
                                       for rec in history
                                       for entry in rec[1::] if entry[0]]))
        # The shapes in the transpose history are cloned to the reorder above
        # will not affect them.
        return cls(node, first_valid_entry[0])


class ReorderInputDims(TransposeInputBase):
    def _execute(self, node, G):
        debug(f"{self}")
        node.dims.transpose(self.transpose)
        if node.out_dims_hint:
            node.out_dims_hint[0] = [node.out_dims_hint[0][idx]
                                     for idx in self.transpose]
            node.in_dims_hint = node.out_dims_hint

    def __str__(self) -> str:
        return "%s input dims with %s" % (self.node.name, self.transpose)


class ReorderConstantInput(TransposeInputBase):
    def _execute(self, node, G):
        debug(f"{self}")
        if G.quantization:
            qrec = G.quantization.get(node.name, None)
        else:
            qrec = None
        node.value = np.transpose(node.value, self.transpose)
        node.dims = Dim.unnamed(node.value.shape)
        # update the quantized dimension if the constant is channel scaled
        if node.qtype and node.qtype.quantized_dimension is not None:
            cqtype = deepcopy(node.qtype)
            cqtype.quantized_dimension = self.transpose.index(
                cqtype.quantized_dimension)
            node.qtype = cqtype
        if qrec and qrec.out_qs[0].quantized_dimension is not None:
            cqtype = deepcopy(qrec.out_qs[0])
            cqtype.quantized_dimension = self.transpose.index(
                cqtype.quantized_dimension)
            qrec.out_qs[0] = cqtype

    def __str__(self) -> str:
        return "%s reorder constant input to %s" % (self.node.name, self.transpose)


class DeleteReshapeAction(Action):

    def _execute(self, node, G):
        debug(f"{self}")
        if node.name not in G:
            return
        G.remove_and_reconnect(node, edge_class=NNEdge)

    def __str__(self) -> str:
        return f"delete reshape {self.node.name}"

class MatMatTAction(Action):

    def _execute(self, node, G: GraphView):
        debug(f"{self}")
        new_node = MatMulOpNode(node.name) if isinstance(node, MatMulTransposedNode) else MatMulTransposedNode(node.name)
        G.replace_node(node, new_node)

    def __str__(self) -> str:
        op = "Mat->MatT" if isinstance(self.node, MatMulTransposedNode) else "MatT->Mat"
        return f"change {op} {self.node.name}"


class DeleteTransposeAction(Action):
    def __init__(self, node, reshape=None) -> None:
        super(DeleteTransposeAction, self).__init__(node)
        self.reshape = reshape

    def _execute(self, node, G):
        debug(f"{self}")
        if node.name not in G:
            return
        if self.reshape:
            reshape_node = ReshapeNode(G.unique_name(f"{node.name}_reshape"),
                                             old_shape=self.reshape[0],
                                             shape=self.reshape[1])
            G.replace_node(node.name, reshape_node)
            if G.quantization:
                G.quantization.copy_qrec(node, 'in', 0, reshape_node)
            LOG.debug(
                f'transpose {node.name} replaced with reshape {reshape_node.name}')
        else:
            G.remove_and_reconnect(node, edge_class=NNEdge)
            nid = node.name
            if G.quantization and nid in G.quantization:
                del G.quantization[nid]

            LOG.debug(f'transpose {node.name} removed')

    def __str__(self) -> str:
        reshape = f' and insert reshape {self.reshape[0]}->{self.reshape[1]}' if self.reshape else ''
        return f"delete transpose {self.node.name}{reshape}"


class SetTransposeAction(Action):
    def __init__(self, node, transpose) -> None:
        super(SetTransposeAction, self).__init__(node)
        self.transpose = tuple(transpose) if transpose is not None else None

    def __move_transpose(self, from_dir, to_dir):
        pass

    def _execute(self, node, G):
        debug(f"{self}")
        node.transpose = self.transpose

    def __str__(self) -> str:
        return f"{self.node.name} set transpose from {self.node.transpose} to {self.transpose}"


class ReorderLinearAction(Action):
    def __init__(self, node, direction, transpose, shape, set_reshape_shape=None, qrec=None) -> None:
        super(ReorderLinearAction, self).__init__(node)
        self.direction = direction
        self.shape = shape
        self.transpose = tuple(transpose)
        self.qrec = qrec
        self.set_reshape_shape = set_reshape_shape

    @classmethod
    def from_history(cls, node, history, qrec, direction):
        # Find the first entry in the transpose history that actually has a transpose
        entry_idx, first_valid_entry = next(iter([(idx, rec) for idx, rec in enumerate(reversed(history))
                                                  if rec.transpose]))
        # arriving from the top the transpose is in the down direction and from the
        # bottom in the up direction so in both cases we need to reverse it
        transpose = tuple(reverse_transpose(first_valid_entry.transpose))
        # shape closest to the node
        shape = tuple(first_valid_entry.to_shape)
        set_reshape_shape = None
        # if direction == "out":
        #     first_reshape = next(iter([elem.node for elem
        #                                in list(reversed(history))[:entry_idx] if isinstance(elem.node, ReshapeParameters)]), None)
        #     if first_reshape:
        #         if shape != tuple(first_reshape.shape.shape):
        #             raise CantContinueError(f'reshape {first_reshape.name} after linear {node.name} has '
        #                                     f'incorrect out shape {first_reshape.shape.shape} to apply transpose {transpose}')
        #         set_reshape_shape = (first_reshape, apply_transpose(first_reshape.shape.shape, transpose))

        return cls(node, direction, transpose, shape, set_reshape_shape=set_reshape_shape, qrec=qrec)

    @classmethod
    def out_from_history(cls, node, history, qrec):
        return cls.from_history(node, history, qrec, "out")

    @classmethod
    def in_from_history(cls, node, history, qrec):
        return cls.from_history(node, history, qrec, "in")

    def _execute(self, node, G):
        debug(f"{self}")
        filter_node = node.contained_filters()[0] if isinstance(
            node, LinearFusionNode) else node
        in_edges = G.indexed_in_edges(node.name)
        weights_node = in_edges[1].from_node
        if self.direction == "in":
            weights_node.value = np.reshape(
                np.transpose(
                    np.reshape(
                        weights_node.value,
                        [filter_node.filter_dim.out_c] + list(self.shape)
                    ),
                    [0] + [dim+1 for dim in self.transpose]
                ),
                weights_node.value.shape)
        else:
            in_c = filter_node.filter_dim.size() // filter_node.filter_dim.out_c
            weights_node.value = np.reshape(
                np.transpose(
                    np.reshape(
                        weights_node.value,
                        list(self.shape) + [in_c]
                    ),
                    list(self.transpose) + [len(self.transpose)]
                ),
                weights_node.value.shape)
            if len(in_edges) > 2:
                biases_node = in_edges[2].from_node
                biases_node.value = np.reshape(
                    np.transpose(
                        np.reshape(
                            biases_node.value,
                            self.shape
                        ),
                        list(self.transpose)
                    ),
                    biases_node.value.shape)
            nid = (node.name, filter_node.name) if isinstance(
                node, LinearFusionNode) else node.name
            if self.set_reshape_shape:
                self.set_reshape_shape[0].shape = Dim.unnamed(
                    self.set_reshape_shape[1])
            # since the output channel order has changed we need to make channel scaled qrec match this
            if G.quantization and nid in G.quantization:
                qrec = G.quantization[nid]
                if qrec.ktype == "scaled":
                    if len(qrec.in_qs) > 2:
                        bqrec = G.quantization[biases_node.name]
                        qrec.in_qs[2].reorder(self.transpose, self.shape)
                        bqrec.out_qs[0] = qrec.in_qs[2]
                    qrec.in_qs[1].reorder(self.transpose, self.shape)
                    wqrec = G.quantization[weights_node.name]
                    wqrec.out_qs[0] = qrec.in_qs[1]
                    if 'mul_biases_q' in qrec.cache:
                        qrec.cache['mul_biases_q'].reorder(
                            self.transpose, self.shape)
                    if isinstance(node, LinearFusionNode):
                        fqrec = G.quantization[node.name]
                        fqrec.in_qs[1] = qrec.in_qs[1]
                        if len(qrec.in_qs) > 2:
                            fqrec.in_qs[2] = qrec.in_qs[2]

    def __str__(self) -> str:
        return "reorder linear layer %s %s with shape %s transposed %s" % (self.node.name, self.direction,
                                                                           self.shape, self.transpose)
