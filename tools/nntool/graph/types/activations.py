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

from expressions.symbolic.basic import HSigmoid, HTanh, Relu, Sigmoid, TanH

from .base import (CanFuseToExpression, NoSizeChangeParameters,
                   SingleInputAndOutput, Transposable, expression_op, cls_op_name)

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method
class ActivationParameters(NoSizeChangeParameters, SingleInputAndOutput):

    def __init__(self, name):
        super(ActivationParameters, self).__init__(name)

    @classmethod
    def get_activation(cls, activation_type: str, name: str):
        if activation_type == "relu":
            return ReluActivationParameters(name)
        if activation_type == "relu6":
            return ReluActivationParameters(name, upper_bound=6)
        actnames = {act_class.CLS_OP_NAME: act_class for act_class in ActivationParameters.__subclasses__()}
        if activation_type in actnames:
            return actnames[activation_type](name)
        raise ValueError("don't know how to create %s"%activation_type)

    @property
    def graph_label(self):
        return [self.name, self.activation.upper()]

    def should_fuse(self, node_set, qrec=None):
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
@cls_op_name('relu')
class ReluActivationParameters(ActivationParameters, CanFuseToExpression):
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

    @property
    def can_equalize(self):
        return self.activation == "relu"

@cls_op_name('leaky')
class LeakyActivationParameters(ActivationParameters):
    def __init__(self, name, leak_factor=0.01):
        super(LeakyActivationParameters, self).__init__(name)
        self._leak_factor = leak_factor

    @property
    def leak_factor(self):
        return self._leak_factor

    @property
    def can_equalize(self):
        return False

@expression_op(HSigmoid)
@cls_op_name('hsigmoid')
class HSigmoidActivationParameters(ActivationParameters, CanFuseToExpression):
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
    def can_equalize(self):
        return False

    def __str__(self):
        return "Activation {} offset={} {}".format(
            self.activation,
            self.offset,
            self.at_options
        )

@cls_op_name('hswish')
class HSwishActivationParameters(ActivationParameters):
    @property
    def activation(self):
        return "hswish"

    @property
    def can_equalize(self):
        return False

@expression_op(HTanh)
@cls_op_name('htanh')
class HTanHActivationParameters(ActivationParameters, CanFuseToExpression):
    def __init__(self, name):
        super(HTanHActivationParameters, self).__init__(name)
        self.at_options.valid_options['OUT_8BITS'] = int

    @property
    def can_equalize(self):
        return False

    def should_fuse(self, node_set, qrec=None):
        # TODO - HTanH is only supported in an expression currently
        return True

@expression_op(TanH)
@cls_op_name('tanh')
class TanHActivationParameters(ActivationParameters, CanFuseToExpression):

    @property
    def can_equalize(self):
        return False

    def should_fuse(self, node_set, qrec=None):
        # TODO - TanH is only supported in an expression currently
        return True

@expression_op(Sigmoid)
@cls_op_name('sigmoid')
class SigmoidActivationParameters(ActivationParameters, CanFuseToExpression):

    @property
    def can_equalize(self):
        return False

@cls_op_name('softmax')
class SoftMaxParameters(Transposable, SingleInputAndOutput):

    def __init__(self, name, beta=None, axis=None):
        super(SoftMaxParameters, self).__init__(name)
        self.beta = 0.0 if beta is None else beta
        self.axis = axis
        self.at_options.valid_options['OUT_8BITS'] = int

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def get_output_size(self, in_dims):
        if self.axis is None:
            self.axis = len(in_dims[0]) - 1
        if self.transpose_in:
            in_dims = [dim.calc_transpose(trans) if trans is not None else dim
                       for dim, trans in zip(in_dims, self.transpose_in)]
        out_dim = in_dims[0]
        if self.transpose_out and self.transpose_out[0]:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    def __str__(self):
        return f"Beta {self.beta} Axis {self.axis} Op: {self.at_options}"
