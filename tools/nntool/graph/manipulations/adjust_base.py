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

from graph.manipulations.eliminate_transposes.transpose_helpers import apply_transpose
from graph.types.base import NNEdge
from graph.types.others import TransposeParameters
from utils.node_id import NodeId
from utils.subclasses import get_all_subclasses

LOG = logging.getLogger("nntool." + __name__)


class AdjusterException(Exception):
    pass


class AdjusterBase():
    HANDLES = []

    def __init__(self, opts=None) -> None:
        self.opts = opts

    @staticmethod
    def trans_names(names, trans):
        return [names[trans_elem] for trans_elem in trans]

    def trans_trans(self, cur, trans):
        return self.eliminate_noop([cur[trans_elem] for trans_elem in trans])

    @staticmethod
    def eliminate_noop(trans):
        if trans is None or all([trans[idx] < trans[idx + 1] for idx in range(len(trans) - 1)]):
            return None
        return trans

    def apply_input_trans(self, G, node, trans: list, index=None):
        if index is None:
            start = 0
            end = len(node.in_dims)
        else:
            start = index
            end = index + 1
        for idx in range(start, end):
            params = TransposeParameters(G.unique_name(f"{node.name}_trans_in{idx}"), transpose=trans)
            G.insert_node_before(
                params,
                node,
                to_idx=idx,
                edge_class=NNEdge
            )
            if node.in_dims_hint:
                node.in_dims_hint[idx] = apply_transpose(node.in_dims_hint[idx], trans)
            nid = NodeId(node)
            if G.quantization and nid in G.quantization:
                G.quantization.copy_qrec(node, 'in', idx, params)

    def apply_output_trans(self, G, node, trans: list, index=None):
        if index is None:
            start = 0
            end = len(node.out_dims)
        else:
            start = index
            end = index + 1
        for idx in range(start, end):
            params = TransposeParameters(G.unique_name(f"{node.name}_trans_out{idx}"), transpose=trans)
            G.insert_node_after(
                node,
                params,
                from_idx=idx,
                edge_class=NNEdge
            )
            if node.out_dims_hint:
                node.out_dims_hint[idx] = apply_transpose(node.out_dims_hint[idx], self.invert(trans))
            nid = NodeId(node)
            if G.quantization and nid in G.quantization:
                G.quantization.copy_qrec(node, 'out', idx, params)

    @staticmethod
    def move_axis_to_zero_trans(axis: int, shape: list):
        return [axis] + [idx for idx in range(len(shape)) if idx != axis]

    @staticmethod
    def move_axis_to_last_trans(axis: int, shape: list):
        return [idx for idx in range(len(shape)) if idx != axis] + [axis]

    @staticmethod
    def verify_chw(node, names: list, has_batch=False):
        if len(names) != 3 and not (has_batch and len == 4):
            LOG.error("Conv %s: input has strange length %s", node.name, names)
            raise AdjusterException()
        expected_dims = ['n', 'c', 'h', 'w'] if has_batch else ['c', 'h', 'w']
        if not all(elem in names for elem in expected_dims):
            LOG.error("Conv %s: input has strange elements %s", node.name, names)
            raise AdjusterException()

    @staticmethod
    def get_trans(names: list, order: list):
        return [names.index(elem) for elem in order]

    @staticmethod
    def maybe_transpose(cur, desired_order, tensor, reshape=None):
        if cur.order != desired_order:
            tensor = tensor.transpose(cur.transpose_to_order(desired_order))
        if reshape is not None:
            tensor = tensor.reshape(reshape)
        return tensor

    @staticmethod
    def invert(trans):
        return [trans.index(idx) for idx in range(len(trans))]

    @staticmethod
    def handles(*args, **kwargs):
        return AdjusterBase.property_register("HANDLES", (args, kwargs))

    @classmethod
    def get_all_handlers(cls, opts):
        handlers = {}
        for handler_cls in get_all_subclasses(cls):
            if not handler_cls.HANDLES:
                continue
            for params_cls in handler_cls.HANDLES[0]:
                handlers_for = handlers.setdefault(params_cls, [])
                handlers_for.append((handler_cls(opts), handler_cls.HANDLES[1]))
        return handlers

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

    @staticmethod
    def get_output_qrec_class(G):
        outs = next(iter(G.outputs()), None)
        if not outs:
            raise ValueError('no output nodes found in graph')
        qrec = G.quantization.get(NodeId(outs), None)
        if not qrec:
            raise ValueError('no output quantization found')
        return qrec.__class__

    def check_quantization(self, G, node, reshape, direction='in'):
        if G.quantization:
            qclass = self.get_output_qrec_class(G)
            node_qrec = G.quantization[NodeId(node)]
            qtype = getattr(node_qrec, f'{direction}_qs')[0]
            G.quantization[NodeId(reshape)] = qclass(
                in_qs=[deepcopy(qtype)], out_qs=[deepcopy(qtype)], ktype=node_qrec.ktype)

    def adjust_in_out_order(self, G, node, names, order, fusion=None, has_batch=False, check_chw=True):
        if check_chw:
            self.verify_chw(node, names, has_batch=has_batch)
        trans = self.get_trans(names, order)
        trans_node = fusion if fusion is not None else node
        self.apply_input_trans(G, trans_node, trans, index=0)
        self.apply_output_trans(G, trans_node, self.invert(trans), index=0)


handles = AdjusterBase.handles
