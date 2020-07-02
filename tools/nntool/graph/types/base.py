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
from typing import Union
from abc import abstractmethod

from utils.graph import Edge, Node
from utils.option_list import OptionList
from generation.at_types.gen_ctrl import GenCtrl, CTRL_FEATURES

LOG = logging.getLogger("nntool." + __name__)


class ParameterError(Exception):
    pass


class CantPromoteQError(ParameterError):
    pass


class NodeOptions(OptionList):
    def __init__(self, *args, **kwargs):
        super(NodeOptions, self).__init__(*args, **kwargs)

# pylint: disable=too-many-instance-attributes


class Parameters(Node):
    op_name = "unknown"

    def __init__(self, name, *args, in_dims_hint=None, out_dims_hint=None, constant_store=None, **kwargs):
        super().__init__(name, *args, **kwargs)
        del args, kwargs
        self._in_dims = None
        self._out_dims = None
        self._in_dims_hint = in_dims_hint
        self._out_dims_hint = out_dims_hint
        self._step_idx = -1
        self._constant_store = constant_store
        self._valid_at_options = {"VCD_TRACE_ON": int, "DUMP_TENSORS": int,
                                  "OUT_HOME_MEM_LOC": str, "OUT_EXEC_MEM_LOC": str}
        self._at_options = NodeOptions(self._valid_at_options)

    def get_parameters(self):
        return {}

    def set_parameters(self, val):
        pass

    def get_gen_ctrl(self):
        return GenCtrl(self.at_options)

    @property
    def valid_at_options(self):
        return self._valid_at_options

    @property
    def at_options(self):
        return self._at_options

    @at_options.setter
    def at_options(self, val):
        self._at_options = val

    @property
    def in_dims_hint(self) -> list:
        return self._in_dims_hint

    @in_dims_hint.setter
    def in_dims_hint(self, val: list):
        self._in_dims_hint = val

    @property
    def out_dims_hint(self) -> list:
        return self._out_dims_hint

    @out_dims_hint.setter
    def out_dims_hint(self, val: list):
        self._out_dims_hint = val

    @staticmethod
    def format_hints_str(hints):
        if not hints:
            return 'none'
        return ','.join(['x'.join(hint) if hint is not None else "U" for hint in hints])

    @property
    def hints_str(self):
        return 'in: {} out: {}'.format(*[self.format_hints_str(hints)
                                         for hints in [self._in_dims_hint, self._out_dims_hint]])

    @property
    def step_idx(self):
        return self._step_idx

    @step_idx.setter
    def step_idx(self, val):
        self._step_idx = val

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, val):
        self._value = val

    @property
    def can_promoteq(self):
        return False

    def promoteq(self):
        raise CantPromoteQError()

    @property
    def in_dims(self):
        return self._in_dims

    @in_dims.setter
    def in_dims(self, value):
        assert isinstance(value, list), "in_dims should always be a list"
        LOG.debug("%s in dims set to %s", self.__class__.__name__, [str(val) for val in value])
        self._in_dims = value

    @property
    def out_dims(self):
        return self._out_dims

    @out_dims.setter
    def out_dims(self, value):
        LOG.debug("%s out dims set to %s", self.__class__.__name__, str(value))
        self._out_dims = value

    @abstractmethod
    def get_parameter_size(self):
        pass

    @abstractmethod
    def get_output_size(self, in_dims):
        pass

    @property
    @abstractmethod
    def can_equalize(self):
        pass

    @abstractmethod
    def clone(self, name, groupn=None):
        pass

    def clone_dim_with_hints(self, dims, hint_dir="in"):
        if hint_dir == "in":
            hints = self._in_dims_hint
        else:
            hints = self._out_dims_hint

        assert hints is None or len(dims) == len(hints), "incorrect dimensions length"
        cloned_dims = []
        for dim_idx, dim in enumerate(dims):
            if dim.is_named and all(k in dim.keys for k in ['c', 'h', 'w']):
                cloned_dims.append(dim.clone(['c', 'h', 'w']))
            else:
                cloned_dim = dim.clone()
                if hints and hints[dim_idx]:
                    cloned_dim.apply_naming_hints(hints[dim_idx])
                cloned_dims.append(cloned_dim)
        return cloned_dims

    def compute_load(self):
        return None

    @abstractmethod
    def __str__(self):
        pass


class SingleInputAndOutput():
    '''Mixin that indicates that node has a single input and output'''


class SensitiveToOrder():
    '''Mixin that indicates that the node must receive channel first input'''


class SameNumberOfDimensionsForInputs():
    '''Mixin that indicates that the node has multiple inputs that have the same dimension length'''


class NoSizeChangeParameters(Parameters):

    def get_output_size(self, in_dims):
        return in_dims

    @abstractmethod
    def clone(self, name, groupn=None):
        pass

    @abstractmethod
    def __str__(self):
        pass

#pylint: disable=abstract-method


class FilterLikeParameters(Parameters, SingleInputAndOutput):
    def __init__(self, *args, stride=None, padding=None,
                 pad_type="zero", **kwargs):
        assert stride and padding
        super(FilterLikeParameters, self).__init__(*args, **kwargs)
        self.stride = stride
        self.padding = padding
        self.pad_type = pad_type

    def has_at_zero_pad(self):
        if self.padding.has_at_pad():
            if self.pad_type != "zero":
                raise AttributeError("Padding is probably not compatible with AutoTiler")
            return True
        return False

    @property
    def can_equalize(self):
        return True


class Transposable(Parameters):

    def __init__(self, *args, transpose_in=None, transpose_out=None, **kwargs):
        self._transpose_in = transpose_in
        self._transpose_out = transpose_out
        super(Transposable, self).__init__(*args, **kwargs)

    @staticmethod
    def first_last(dim):
        trans = list(range(len(dim.shape)))
        trans.append(trans.pop(0))
        return trans

    @staticmethod
    def last_first(dim):
        trans = list(range(len(dim.shape)))
        trans.insert(0, trans.pop())
        return trans

    @property
    def transpose_in(self):
        return self._transpose_in

    @transpose_in.setter
    def transpose_in(self, val):
        self._transpose_in = val

    @property
    def transpose_out(self):
        return self._transpose_out

    @transpose_out.setter
    def transpose_out(self, val):
        self._transpose_out = val

    @property
    def has_transpose(self):
        return bool(self.transpose_in or self.transpose_out)

    def __str__(self):
        trans = []
        if self.transpose_in:
            trans.append("t_in: %s"%",".join(str(trans) for trans in self.transpose_in))
        if self.transpose_out:
            trans.append("t_out: %s"%",".join(str(trans) for trans in self.transpose_out))
        return ", ".join(trans)

#pylint: disable=abstract-method


class FilterParameters(Parameters, SingleInputAndOutput):

    def __init__(self, *args, filt=None, has_bias=False, **kwargs):
        assert filt
        super(FilterParameters, self).__init__(*args, **kwargs)
        self.has_bias = has_bias
        self.filter = filt
        self.stats = None
        self._weights = None
        self._biases = None
        self.details = None
        self.at_options.update_valid_options(CTRL_FEATURES)

    @property
    def weights(self):
        if self._constant_store:
            return self._constant_store.get(self, 1)
        else:
            return self._weights

    @weights.setter
    def weights(self, val):
        if self._constant_store:
            self._constant_store.set(self, 1, val)
        else:
            self._weights = val

    @property
    def biases(self):
        if self._constant_store:
            return self._constant_store.get(self, 2)
        else:
            return self._biases

    @biases.setter
    def biases(self, val):
        if self._constant_store:
            self._constant_store.set(self, 2, val)
        else:
            self._biases = val

    def get_parameters(self):
        return {'weights': self.weights, 'biases': self.biases}

    def set_parameters(self, val):
        self.weights = val['weights']
        self.biases = val['biases']


class MultiplicativeBiasParameters(FilterParameters):
    def __init__(self, *args, **kwargs):
        super(MultiplicativeBiasParameters, self).__init__(*args, **kwargs)
        self.has_mul_bias = False
        self._mul_biases = None

    @property
    def mul_biases(self):
        if self._constant_store:
            return self._constant_store.get(self, 3)
        else:
            return self._mul_biases

    @mul_biases.setter
    def mul_biases(self, val):
        if self._constant_store:
            self._constant_store.set(self, 3, val)
        else:
            self._mul_biases = val

    def get_parameters(self):
        res = super().get_parameters()
        if self.has_mul_bias:
            res['mul_biases'] = self.mul_biases
        return res

    def set_parameters(self, val):
        super().set_parameters(val)
        if 'mul_biases' in val:
            self.mul_biases = val['mul_biases']


class EdgeParameters():
    # edge types are in, out or in_out
    def __init__(self, name, dims, creating_node, creating_node_idx, creating_step,
                 edge_type="in_out", edge_order=None):
        self._name = name
        self._dims = dims
        self._creating_node = creating_node
        self._creating_node_idx = creating_node_idx
        self._creating_step = creating_step
        self._edge_type = edge_type
        self._edge_order = edge_order
        self._is_alias = False

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def is_alias(self):
        return self._is_alias

    @is_alias.setter
    def is_alias(self, val):
        self._is_alias = val

    @property
    def dims(self):
        return self._dims

    @dims.setter
    def dims(self, val):
        self._dims = val

    @property
    def creating_node(self):
        return self._creating_node

    @creating_node.setter
    def creating_node(self, val):
        self._creating_node = val

    @property
    def creating_node_idx(self):
        return self._creating_node_idx

    @creating_node_idx.setter
    def creating_node_idx(self, val):
        self._creating_node_idx = val

    @property
    def creating_step(self):
        return self._creating_step

    @creating_step.setter
    def creating_step(self, val):
        self._creating_step = val

    @property
    def edge_type(self):
        return self._edge_type

    @edge_type.setter
    def edge_type(self, val):
        self._edge_type = val

    @property
    def edge_order(self):
        return self._edge_order

    @edge_order.setter
    def edge_order(self, val):
        self._edge_order = val


class NNEdge(Edge):
    def __init__(self, from_node: Union[str, Node], to_node: Union[str, Node],
                 params: Parameters = None,
                 from_idx: int = 0, to_idx: int = 0):
        super().__init__(from_node, to_node, from_idx, to_idx)
        self.params = params
