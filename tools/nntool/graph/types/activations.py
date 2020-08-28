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

from .base import NoSizeChangeParameters, SingleInputAndOutput

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
        if activation_type == "tanh":
            return TanHActivationParameters(name)
        raise ValueError("don't know how to create %s"%activation_type)

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

class ReluActivationParameters(ActivationParameters):
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

class HSigmoidActivationParameters(ActivationParameters):
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

class TanHActivationParameters(ActivationParameters):
    @property
    def op_name(self):
        return "tanh"

    def clone(self, name, groupn=None):
        return TanHActivationParameters(name)

    @property
    def can_equalize(self):
        return False

class SoftMaxParameters(NoSizeChangeParameters, SingleInputAndOutput):

    op_name = "softmax"

    def __init__(self, name, beta):
        super(SoftMaxParameters, self).__init__(name)
        self.beta = beta

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def __str__(self):
        return "BETA {} {}".format(
            self.beta,
            self.at_options
        )