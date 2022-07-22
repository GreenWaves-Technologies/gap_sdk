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

from nntool.expressions.symbolic.basic import HSigmoid, HTanh, Relu, Sigmoid, TanH
from nntool.generation.at_types.gen_ctrl import GenCtrl

from nntool.utils.subclasses import get_all_subclasses

from .base import (CanFuseToExpressionMixin, ComparableNodeMixin,
                   NoSizeChangeNodeBase, NNNodeBase, SensitiveToOrderMixin,
                   SingleInputAndOutputMixin, cls_op_name, expression_op)

LOG = logging.getLogger(__name__)

#pylint: disable=abstract-method


class ActivationNodeBase(NoSizeChangeNodeBase, SingleInputAndOutputMixin, ComparableNodeMixin):
    def __init__(self, name, always_fuse=False, **kwargs):
        super(ActivationNodeBase, self).__init__(name, **kwargs)
        self._always_fuse = always_fuse

    @classmethod
    def get_activation(cls, activation_type: str, name: str):
        if activation_type == "relu":
            return ReluNode(name)
        if activation_type == "relu6":
            return ReluNode(name, upper_bound=6)
        actnames = {
            act_class.CLS_OP_NAME: act_class for act_class in get_all_subclasses(ActivationNodeBase)}
        if activation_type in actnames:
            return actnames[activation_type](name)
        raise ValueError("don't know how to create %s" % activation_type)

    @property
    def graph_label(self):
        return [self.name, self.activation.upper()]

    def should_fuse(self, node_set, qrec=None):
        # activation should fuse into an expression if there are several elements
        return self._always_fuse or len(node_set) > 1

    @property
    def activation(self):
        return self.op_name

    def get_parameter_size(self):
        return 0

    def compute_load(self):
        return 0

    def is_same_operation_as(self, G, other):
        return isinstance(other, self.__class__)

    def __str__(self):
        return "Activation {} {}".format(
            self.op_name,
            self.at_options
        )


@expression_op(Relu)
@cls_op_name('relu')
class ReluNode(ActivationNodeBase, CanFuseToExpressionMixin):
    def __init__(self, name, lower_bound=0, upper_bound=None, **kwargs):
        super(ReluNode, self).__init__(name, **kwargs)
        self._lower_bound = lower_bound
        self._upper_bound = upper_bound

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, f'_{k}') for k in [
            "upper_bound", "lower_bound"
        ] if getattr(self, f'_{k}')})
        return super_attrs

    @property
    def op_name(self):
        if self._lower_bound == 0:
            if self._upper_bound == 6:
                return "relu6"
            if self._upper_bound is None:
                return "relu"
            return "relun"
        return "relunm"

    @property
    def lower_bound(self):
        return self._lower_bound

    @lower_bound.setter
    def lower_bound(self, val):
        self._lower_bound = val

    @property
    def upper_bound(self):
        return self._upper_bound

    @upper_bound.setter
    def upper_bound(self, val):
        self._upper_bound = val

    def get_expression(self, *args):
        return Relu(*args, lower_bound=self.lower_bound, upper_bound=self.upper_bound)

    def is_same_operation_as(self, G, other):
        return (isinstance(other, ReluNode) and
                self.lower_bound == other.lower_bound and
                self.upper_bound == other.upper_bound)

    @property
    def can_equalize(self):
        return self.activation == "relu"


@cls_op_name('leaky')
class LeakyNode(ActivationNodeBase):
    def __init__(self, name, leak_factor=0.01, **kwargs):
        super(LeakyNode, self).__init__(name, **kwargs)
        self._leak_factor = leak_factor

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.leak_factor != 0.01:
            super_attrs['leak_factor'] = self.leak_factor
        return super_attrs

    @property
    def leak_factor(self):
        return self._leak_factor

    @property
    def can_equalize(self):
        return False

    def is_same_operation_as(self, G, other):
        return (isinstance(other, LeakyNode) and
                self.leak_factor == other.leak_factor)


class HardSigSwiNodeBase(ActivationNodeBase):
    def __init__(self, name, offset=3, upper_bound=6, mult=1/6, **kwargs):
        super(HardSigSwiNodeBase, self).__init__(name, **kwargs)
        self._factors = [offset, upper_bound, mult]

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k, default in [
            ('offset', 3), ('upper_bound', 6), ('mult', 1/6)
        ] if getattr(self, k) != default})
        return super_attrs

    @property
    def factors(self):
        return self._factors

    @property
    def offset(self):
        return self._factors[0]

    @property
    def upper_bound(self):
        return self._factors[1]

    @property
    def mult(self):
        return self._factors[2]

    @offset.setter
    def offset(self, val):
        self._factors[0] = val

    @property
    def can_equalize(self):
        return False

    def is_same_operation_as(self, G, other):
        return (isinstance(other, self.__class__) and
                self.factors == other.factors)

    def __str__(self):
        return f"Activation {self.activation} offset={self.offset} {self.at_options}"


@expression_op(HSigmoid)
@cls_op_name('hsigmoid')
class HSigmoidNode(HardSigSwiNodeBase, CanFuseToExpressionMixin):

    @classmethod
    def from_alpha_beta(cls, name, alpha, beta):
        return cls(name, offset=beta/alpha, upper_bound=1/alpha, mult=alpha)

    @property
    def activation(self):
        return "hsigmoid"

    def get_gen_ctrl(self):
        gen_ctrl = GenCtrl(self.at_options)
        if self.offset != 3.0:
            gen_ctrl.relun = self.offset
        return gen_ctrl


@cls_op_name('hswish')
class HSwishNode(HardSigSwiNodeBase):
    @property
    def activation(self):
        return "hswish"


@expression_op(HTanh)
@cls_op_name('htanh')
class HTanHNode(ReluNode, CanFuseToExpressionMixin):
    def __init__(self, name, **kwargs):
        super(HTanHNode, self).__init__(
            name, lower_bound=-1, upper_bound=1, **kwargs)

    @property
    def can_equalize(self):
        return False


@expression_op(TanH)
@cls_op_name('tanh')
class TanHNode(ActivationNodeBase, CanFuseToExpressionMixin):

    @property
    def can_equalize(self):
        return False


@expression_op(Sigmoid)
@cls_op_name('sigmoid')
class SigmoidNode(ActivationNodeBase, CanFuseToExpressionMixin):

    @property
    def can_equalize(self):
        return False


@cls_op_name('softmax')
class SoftMaxNode(NNNodeBase, SingleInputAndOutputMixin, SensitiveToOrderMixin):

    def __init__(self, name, beta=None, axis=None, **kwargs):
        super(SoftMaxNode, self).__init__(name, **kwargs)
        self.beta = 0.0 if beta is None else beta
        self.axis = axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            'axis', 'beta'
        ] if getattr(self, k)})
        return super_attrs

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def get_output_size(self, in_dims):
        # TODO - This is not correct for a reducing softmax
        if self.axis is None:
            self.axis = len(in_dims[0]) - 1
        out_dim = in_dims[0]
        return [out_dim]

    def __str__(self):
        return f"Beta {self.beta} Axis {self.axis} Op: {self.at_options}"
