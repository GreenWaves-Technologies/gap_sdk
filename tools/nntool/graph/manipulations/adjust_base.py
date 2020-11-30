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

from copy import Error
import logging
LOG = logging.getLogger("nntool." + __name__)


class AdjusterException(Error):
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

    def apply_trans(self, node, trans, key, index=None):
        if getattr(node, key) is None:
            setattr(node, key, [None] * len(node.in_dims))
        if index is None:
            setattr(node, key, [trans.copy() if cur is None else self.trans_trans(
                cur, trans) for cur in getattr(node, key)])
        else:
            getattr(node, key)[index] = trans.copy() if getattr(node, key)[index] is None else self.trans_trans(
                getattr(node, key)[index], trans)

    def apply_input_trans(self, node, trans: list, index=None):
        self.apply_trans(node, trans, 'transpose_in', index=index)

    def apply_output_trans(self, node, trans: list, index=None):
        self.apply_trans(node, trans, 'transpose_out', index=index)

    @staticmethod
    def move_axis_to_zero_trans(axis: int, shape: list):
        return [axis] + [idx for idx in range(len(shape)) if idx != axis]

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


handles = AdjusterBase.handles
