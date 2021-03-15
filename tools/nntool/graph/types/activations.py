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

from utils.symbolic.basic import HSigmoid, Relu, HTanh

from .base import (CanFuseToExpression, NoSizeChangeParameters,
                   SingleInputAndOutput, Transposable, expression_op)

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method
class ActivationParameters(NoSizeChangeParameters, SingleInputAndOutput):

    def __init__(self, name):
        super(ActivationParameters, self).__init__(name)

    @classmethod
    def get_activation(cls, activation_type: str, name: str):
        if activation_type == "hsigmoid":
            return HSigmoidActivationParameters(name)
        if activation_type == "relu":
            return ReluActivationParameters(name)
        if activation_type == "relu6":
            return ReluActivationParameters(name, upper_bound=6)
        if activation_type == "hswish":
            return HSwishActivationParameters(name)
        if activation_type in ["tanh", "htanh"]:
            return TanHActivationParameters(name)
        raise ValueError("don't know how to create %s"%activation_type)

    def should_fuse(self, node_set):
        # activation should fuse into an expression if there are several elements
        return len(node_set) > 1

    @property
    def activation(self):
        return self.op_name

    def get_parameter_size(self):
        return 0

    def compute_load(self):
        return 0

    def __str__(self):
        return "Activation {} {}".format(
            self.op_name,
            self.at_options
        )

@expression_op(Relu)
class ReluActivationParameters(CanFuseToExpression, ActivationParameters):
    def __init__(self, name, lower_bound=0, upper_bound=None):
        super(ReluActivationParameters, self).__init__(name)
        self._lower_bound = lower_bound
        self._upper_bound = upper_bound

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

    def clone(self, name, groupn=None):
        return ReluActivationParameters(name, self._lower_bound, self._upper_bound)

    @property
    def can_equalize(self):
        return self.op_name == "relu"

class LeakyActivationParameters(ActivationParameters):
    def __init__(self, name, leak_factor=0.01):
        super(LeakyActivationParameters, self).__init__(name)
        self._leak_factor = leak_factor

    @property
    def leak_factor(self):
        return self._leak_factor

    @property
    def op_name(self):
        return "leaky"

    def clone(self, name, groupn=None):
        return LeakyActivationParameters(name, self._leak_factor)

    @property
    def can_equalize(self):
        return False

@expression_op(HSigmoid)
class HSigmoidActivationParameters(CanFuseToExpression, ActivationParameters):
    def __init__(self, name, offset=3):
        super(HSigmoidActivationParameters, self).__init__(name)
        self._offset = offset

    @property
    def offset(self):
        return self._offset

    @offset.setter
    def offset(self, val):
        self._offset = val

    @property
    def op_name(self):
        return "hsigmoid"

    def clone(self, name, groupn=None):
        return HSigmoidActivationParameters(name)

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "Activation {} offset={} {}".format(
            self.op_name,
            self.offset,
            self.at_options
        )

class HSwishActivationParameters(ActivationParameters):
    @property
    def op_name(self):
        return "hswish"

    def clone(self, name, groupn=None):
        return HSwishActivationParameters(name)

    @property
    def can_equalize(self):
        return False

@expression_op(HTanh)
class TanHActivationParameters(CanFuseToExpression, ActivationParameters):
    @property
    def op_name(self):
        return "tanh"

    def clone(self, name, groupn=None):
        return TanHActivationParameters(name)

    @property
    def can_equalize(self):
        return False

class SigmoidActivationParameters(CanFuseToExpression, ActivationParameters):
    @property
    def op_name(self):
        return "sigmoid"

    def clone(self, name, groupn=None):
        return SigmoidActivationParameters(name)

    @property
    def can_equalize(self):
        return False

class SoftMaxParameters(Transposable, SingleInputAndOutput):

    op_name = "softmax"

    def __init__(self, name, beta=None, axis=None):
        super(SoftMaxParameters, self).__init__(name)
        self.beta = 0.0 if beta is None else beta
        self.axis = axis

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def get_output_size(self, in_dims):
        self.in_dims = self.clone_dim_with_hints(in_dims)
        if self.axis is None:
            self.axis = len(in_dims[0]) - 1
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim
                       for dim, trans in zip(self.in_dims, self.transpose_in)]
        else:
            in_dims = self.in_dims
        out_dim = self.in_dims[0]
        if self.transpose_out and self.transpose_out[0]:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def __str__(self):
        return f"Beta {self.beta} Axis {self.axis} {self.at_options}"
