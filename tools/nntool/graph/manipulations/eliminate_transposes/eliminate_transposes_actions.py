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
from abc import ABC, abstractmethod
from copy import deepcopy

import numpy as np
from graph.dim import Dim
from graph.manipulations.eliminate_transposes.transpose_helpers import (
    apply_transpose, reverse_transpose)
from graph.types.base import NNEdge
from graph.types.fusions import LinearFusionParameters
from graph.types.others import ReshapeParameters, TransposeParameters
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)
LOGL = LOG.info


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
        LOGL("%s", str(self))


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
        LOGL("%s", str(self))
        direction = self.direction
        if self.reshape_from is not None:
            params = ReshapeParameters(G.unique_name(
                f'{node.name}_reshape'),
                old_shape=Dim.unnamed(self.reshape_from),
                shape=Dim.reshape_to(self.reshape_to))
            self.do_insert(node, G, params, direction=direction)
            node = params
            direction = "out"

        params = TransposeParameters(G.unique_name(
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

    def _execute(self, node, G):
        LOGL("%s", str(self))
        params = ReshapeParameters(G.unique_name(
            f'{node.name}_reshape'), old_shape=self.in_shape, shape=self.out_shape)
        self.do_insert(node, G, params)

    def __str__(self) -> str:
        return f"insert reshape at {self.node.name}:{self.direction}_{self.idx} in {self.in_shape} out {self.out_shape}"


class SetReshapeAction(Action):
    def __init__(self, node, in_shape=None, out_shape=None) -> None:
        super(SetReshapeAction, self).__init__(node)
        self.in_shape = in_shape.clone() if in_shape is not None else None
        self.out_shape = out_shape.clone() if out_shape is not None else None

    def _execute(self, node, G):
        LOGL("%s", str(self))
        if self.in_shape is not None:
            node.old_shape = self.in_shape
        if self.out_shape is not None:
            node.shape = self.out_shape

    def __str__(self) -> str:
        return "%s set reshape in %s out %s" % (self.node.name, self.in_shape, self.out_shape)


class SwitchBatchLinearAction(Action):
    def _execute(self, node, G):
        LOGL("%s", str(self))
        self.node.batch_minor = not self.node.batch_minor

    def __str__(self) -> str:
        return f"{self.node.name} switch to batch {'major' if self.node.batch_minor else 'minor'}"


class TransposeSlidedSlice(Action):
    def __init__(self, node, transpose_in, dir=None, transpose_out=None) -> None:
        super(TransposeSlidedSlice, self).__init__(node)
        self.transpose_in = tuple(transpose_in)
        if transpose_out is None:
            self.transpose_out = self.transpose_in
        else:
            self.transpose_out = tuple(transpose_out)

    def _execute(self, node, G):
        LOGL("%s", str(self))
        node.act_slice = [node.act_slice[idx] for idx in self.transpose_in]
        node.out_shape = [node.out_shape[idx] for idx in self.transpose_out]

    def __str__(self) -> str:
        return "%s transpose slided slice parameters with %s/%s" % (self.node.name, self.transpose_in, self.transpose_out)


class TransposePad(Action):
    def __init__(self, node, transpose, dir=None) -> None:
        super(TransposePad, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        LOGL("%s", str(self))
        node.padding = [node.padding[idx] for idx in self.transpose]
        node.pad_vals = [node.pad_vals[idx] for idx in self.transpose]

    def __str__(self) -> str:
        return "%s transpose pad parameters with %s" % (self.node.name, self.transpose)


class TransposeReverse(Action):
    def __init__(self, node, transpose, dir=None) -> None:
        super(TransposeReverse, self).__init__(node)
        self.transpose = tuple(transpose)

    def _execute(self, node, G):
        LOGL("%s", str(self))
        node.axis = self.transpose[node.axis]

    def __str__(self) -> str:
        return "%s transpose reverse parameters with %s" % (self.node.name, self.transpose)


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
        LOGL("%s", str(self))
        node.dims.transpose(self.transpose)
        if node.out_dims_hint:
            node.out_dims_hint[0] = [node.out_dims_hint[0][idx]
                                     for idx in self.transpose]
            node.in_dims_hint = node.out_dims_hint

    def __str__(self) -> str:
        return "%s input dims with %s" % (self.node.name, self.transpose)


class ReorderConstantInput(TransposeInputBase):
    def _execute(self, node, G):
        LOGL("%s", str(self))
        if G.quantization:
            qrec = G.quantization.get(NodeId(node), None)
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
        LOGL("%s", str(self))
        if node.name not in G:
            return
        G.remove_and_reconnect(node, edge_class=NNEdge)

    def __str__(self) -> str:
        return f"delete reshape {self.node.name}"


class DeleteTransposeAction(Action):
    def __init__(self, node, reshape=None) -> None:
        super(DeleteTransposeAction, self).__init__(node)
        self.reshape = reshape

    def _execute(self, node, G):
        LOGL("%s", str(self))
        if node.name not in G:
            return
        if self.reshape:
            reshape_node = ReshapeParameters(G.unique_name(f"{node.name}_reshape"),
                                             old_shape=self.reshape[0],
                                             shape=self.reshape[1])
            G.replace_node(node.name, reshape_node)
            if G.quantization:
                G.quantization.copy_qrec(node, 'in', 0, reshape_node)
            LOG.info(
                f'transpose {node.name} replaced with reshape {reshape_node.name}')
        else:
            G.remove_and_reconnect(node, edge_class=NNEdge)
            nid = NodeId(node)
            if G.quantization and nid in G.quantization:
                del G.quantization[nid]

            LOG.info(f'transpose {node.name} removed')

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
        LOGL("%s", str(self))
        node.transpose = self.transpose

    def __str__(self) -> str:
        return f"{self.node.name} set transpose from {self.node.transpose} to {self.transpose}"


class ReorderLinearAction(Action):
    def __init__(self, node, direction, transpose, shape, qrec=None) -> None:
        super(ReorderLinearAction, self).__init__(node)
        self.direction = direction
        self.shape = shape
        self.transpose = tuple(transpose)
        self.qrec = qrec

    @classmethod
    def out_from_history(cls, node, history, qrec):
        # Find the first entry in the transpose history that actually has a transpose
        first_valid_entry = next(iter([rec
                                       for rec in reversed(history)
                                       if rec.from_transpose]))
        # in outwards direction the from_shape is the shape before the transpose and we
        # want to apply the transpose to get to the shape
        return cls(node, "out", tuple(first_valid_entry.from_transpose),
                   tuple(first_valid_entry.from_shape), qrec=qrec)

    @classmethod
    def in_from_history(cls, node, history, qrec):
        # Find the first entry in the transpose history that actually has a transpose
        first_valid_entry = next(iter([rec
                                       for rec in reversed(history)
                                       if rec.from_transpose]))
        # in down direction we are pushing the transpose into the FC so
        # the from_shape is the shape with the transpose applied and
        # we want to reverse the from_transpose to get back to the shape before it
        transpose = tuple(reverse_transpose(first_valid_entry.from_transpose))
        shape = tuple(first_valid_entry.from_shape)
        return cls(node, "in", transpose, shape, qrec=qrec)

    def _execute(self, node, G):
        LOGL("%s", str(self))
        filter_node = node.contained_filters()[0] if isinstance(node, LinearFusionParameters) else node
        in_edges = G.indexed_in_edges(node.name)
        weights_node = in_edges[1].from_node
        if self.direction == "in":
            weights_node.value = np.reshape(
                np.transpose(
                    np.reshape(
                        weights_node.value,
                        [filter_node.filter.out_c] + list(self.shape)
                    ),
                    [0] + [dim+1 for dim in self.transpose]
                ),
                weights_node.value.shape)
        else:
            in_c = filter_node.filter.size() // filter_node.filter.out_c
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
            nid = NodeId(node, filter_node) if isinstance(node, LinearFusionParameters) else NodeId(node)
            # since the output channel order has changed we need to make channel scaled qrec match this
            if G.quantization and nid in G.quantization:
                qrec = G.quantization[nid]
                if qrec.ktype == "scaled":
                    if len(qrec.in_qs) > 2:
                        bqrec = G.quantization[NodeId(biases_node)]
                        qrec.in_qs[2].reorder(self.transpose, self.shape)
                        bqrec.out_qs[0] = qrec.in_qs[2]
                    qrec.in_qs[1].reorder(self.transpose, self.shape)
                    wqrec = G.quantization[NodeId(weights_node)]
                    wqrec.out_qs[0] = qrec.in_qs[1]
                    if 'mul_biases_q' in qrec.cache:
                        qrec.cache['mul_biases_q'].reorder(
                            self.transpose, self.shape)
                    if isinstance(node, LinearFusionParameters):
                        fqrec = G.quantization[NodeId(node)]
                        fqrec.in_qs[1] = qrec.in_qs[1]
                        if len(qrec.in_qs) > 2:
                            fqrec.in_qs[2] = qrec.in_qs[2]



    def __str__(self) -> str:
        return "reorder linear layer %s %s with shape %s transposed %s" % (self.node.name, self.direction,
                                                                           self.shape, self.transpose)
