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

from graph.dim import Dim
from graph.types.base import NNEdge
from graph.types.others import ReshapeParameters
from utils.node_id import NodeId

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

    def apply_trans(self, node, trans, direction, index=None):
        trans_key = 'transpose_' + direction
        dims_key = direction + '_dims'
        if getattr(node, trans_key) is None:
            setattr(node, trans_key, [None] * len(getattr(node, dims_key)))
        if index is None:
            setattr(node, trans_key, [trans.copy() if cur is None else self.trans_trans(
                cur, trans) for cur in getattr(node, trans_key)])
        else:
            getattr(node, trans_key)[index] = trans.copy() if getattr(node, trans_key)[index] is None else self.trans_trans(
                getattr(node, trans_key)[index], trans)

    def apply_input_trans(self, node, trans: list, index=None):
        self.apply_trans(node, trans, 'in', index=index)

    def apply_output_trans(self, node, trans: list, index=None):
        self.apply_trans(node, trans, 'out', index=index)

    @staticmethod
    def move_axis_to_zero_trans(axis: int, shape: list):
        return [axis] + [idx for idx in range(len(shape)) if idx != axis]

    @staticmethod
    def move_axis_to_last_trans(axis: int, shape: list):
        return [idx for idx in range(len(shape)) if idx != axis] + [axis]

    @staticmethod
    def verify_chw(node, names: list):
        if len(names) != 3:
            LOG.error("Conv %s: input has strange length %s", node.name, names)
            raise AdjusterException()
        if not all(elem in names for elem in ['c', 'h', 'w']):
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
    def handles(*args):
        return AdjusterBase.property_register("HANDLES", args)

    @classmethod
    def get_all_handlers(cls, opts):
        return {params_cls: handler_cls(opts) for handler_cls in cls.__subclasses__()
                for params_cls in handler_cls.HANDLES}

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

    def adjust_in_out_order(self, G, node, names, order):
        self.verify_chw(node, names)
        trans = self.get_trans(names, order)
        in_dim = node.in_dims[0]
        if in_dim.c != 1:
            self.apply_input_trans(node, trans, index=0)
        else:
            new_shape = {k: getattr(in_dim, k) for k in order}
            reshape = ReshapeParameters(
                f'{node.name}_r_{"".join(in_dim.order)}_{"".join(order)}',
                old_shape=in_dim.clone(),
                shape=Dim.named_ordered(**new_shape)
            )
            G.insert_node_before(
                reshape,
                node,
                edge_class=NNEdge
            )
            node.in_dims_hint[0] = order
            self.check_quantization(G, node, reshape)
        out_dim = node.out_dims[0]
        if out_dim.c != 1:
            self.apply_output_trans(node, self.invert(trans), index=0)
        else:
            old_shape = {k: getattr(out_dim, k) for k in order}
            node.out_dims_hint[0] = order
            reshape = ReshapeParameters(
                f'{node.name}_r_{"".join(names)}',
                old_shape=Dim.named_ordered(**old_shape),
                shape=out_dim.clone()
            )
            G.insert_node_after(
                node,
                reshape,
                edge_class=NNEdge
            )
            self.check_quantization(G, node, reshape, direction='out')


handles = AdjusterBase.handles
