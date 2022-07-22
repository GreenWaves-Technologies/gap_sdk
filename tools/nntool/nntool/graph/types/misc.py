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

from nntool.expressions.symbolic.basic import (NoOP)
from nntool.graph.dim import Dim

from .base import (CanFuseToExpressionMixin, ComparableNodeMixin,
                   InsensitiveToQuantizationMixin,
                   NoSizeChangeNodeBase, NNNodeBase, SingleInputAndOutputMixin, cls_op_name, expression_op)

LOG = logging.getLogger(__name__)


@cls_op_name('copy')
class CopyNode(NNNodeBase, InsensitiveToQuantizationMixin):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def __str__(self):
        return ""


@cls_op_name('quantize')
@expression_op(NoOP)
class QuantizeNode(NNNodeBase, ComparableNodeMixin, CanFuseToExpressionMixin):

    def __init__(self, *args, from_qtype=None, to_qtype=None,
                 inserted_by_quantizer=False, **kwargs):
        super().__init__(*args, **kwargs)

        self.from_qtype = from_qtype
        self.to_qtype = to_qtype
        self.inserted_by_quantizer = inserted_by_quantizer

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "from_qtype", "to_qtype", "inserted_by_quantizer"
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    def is_same_operation_as(self, G, other):
        return (isinstance(other, QuantizeNode) and
                self.from_qtype == other.from_qtype and
                self.to_qtype == other.to_qtype)

    @property
    def can_equalize(self):
        return False

    def get_output_size(self, in_dims):
        return [in_dims[0].clone()]

    def should_fuse(self, node_set, qrec=None):
        return super().should_fuse(node_set, qrec=qrec) and len(node_set) > 1

    def __str__(self):
        return f"{self.from_qtype} --> {self.to_qtype}"


@cls_op_name('pad')
class PadNode(NNNodeBase, SingleInputAndOutputMixin):

    def __init__(self, name, padding=None, pad_vals=None, in_dims_hint=None, out_dims_hint=None):

        super().__init__(
            name,
            in_dims_hint=in_dims_hint,
            out_dims_hint=out_dims_hint)
        self.padding = padding
        self.pad_vals = pad_vals

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "padding", "pad_vals"
        ] if getattr(self, k)})
        return super_attrs

    @property
    def graph_label(self):
        return [self.name, f'Pad {self.padding}']

    @property
    def graph_anon_label(self):
        return ['Pad', f'{self.padding}']

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        out_dim = in_dims[0].clone()
        for idx, vals in enumerate(self.padding):
            out_dim[idx] += sum(vals)
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def __str__(self):
        return "PAD {}".format(self.padding)


# pylint: disable=abstract-method

@cls_op_name('noop')
class NoOPNode(NoSizeChangeNodeBase, SingleInputAndOutputMixin, InsensitiveToQuantizationMixin):

    def __init__(self, name, desc=""):
        super().__init__(name)
        self._desc = desc

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    @property
    def no_model_code(self) -> bool:
        return True

    @property
    def does_nothing(self) -> bool:
        return True

    def compute_load(self):
        return 0

    def __str__(self):
        return "NOOP {}".format(
            self._desc
        )


class UnexecutableNodeBase(NNNodeBase):
    pass


@cls_op_name('UNSUPPORTED')
class UnconvertedOpNode(UnexecutableNodeBase):

    def __init__(self, name, indicated_op_name=None, expected_inputs=None,
                 indicated_outputs=None, info=None, **kwargs):
        super().__init__(name, **kwargs)
        self.info = info
        self.expected_inputs = expected_inputs
        self.indicated_outputs = indicated_outputs
        self.indicated_op_name = indicated_op_name

    def get_output_size(self, in_dims):
        if self.indicated_outputs:
            return self.indicated_outputs
        if len(in_dims) == 1:
            return [in_dims[0]]
        return [Dim.unknown()]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return "UNSUPPORTED OP: %s" % self.indicated_op_name


@cls_op_name('UNKNOWN')
class UnknownOpNode(UnexecutableNodeBase):

    def __init__(self, name, info):
        super().__init__(name)
        self.info = info

    def get_output_size(self, in_dims):
        if len(in_dims) == 1:
            return [in_dims[0]]
        return [Dim.unknown()]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return "Unknown"
