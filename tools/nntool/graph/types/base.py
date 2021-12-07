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
from abc import abstractmethod
from typing import Sequence, Union

from expressions.symbolic.symbol import Symbol
from generation.at_types.gen_ctrl import CTRL_FEATURES, GenCtrl
from graph.dim import Dim, PadDim, StrideDim

from utils.graph import Edge, Node, NodeRef
from utils.option_list import OptionList

LOG = logging.getLogger("nntool." + __name__)


class ParameterError(Exception):
    pass


class CantPromoteQError(ParameterError):
    pass


class NodeOptions(OptionList):
    def __init__(self, *args, **kwargs):
        super(NodeOptions, self).__init__(*args, **kwargs)

# pylint: disable=too-many-instance-attributes


def clone_dims(dims: Sequence[Dim], hints: Sequence[Dim]):
    cloned_dims = []
    for dim_idx, dim in enumerate(dims):
        if dim is None:
            cloned_dims.append(None)
            continue
        assert hasattr(dim, 'clone'), "no clone attribute - probably not a dim"
        cloned_dim = dim.clone()
        if hints and len(hints) > dim_idx and hints[dim_idx]:
            hint = hints[dim_idx]
            # this is a nasty hack to cope with linear layers that can take in_dims
            # that are a different length to their hint which will be 'c'
            if len(hint) == len(cloned_dim):
                cloned_dim.apply_naming_hints(hints[dim_idx])
            elif not cloned_dim.is_named:
                raise ValueError(
                    f'hint length {hint} != dim length {cloned_dim}')
        cloned_dims.append(cloned_dim)
    return cloned_dims


class NNNodeRef(NodeRef):
    def __init__(self, node, idx, G) -> None:
        super(NNNodeRef, self).__init__(node)
        self._G = G
        self._idx = idx

    @property
    def G(self):
        return self._G

    @property
    def ref(self):
        return ((self._node, self._idx), self._G)

    def __getattr__(self, name):
        return getattr(self._node, name)

    def __setattr__(self, name, val):
        if name in ['_node', '_G', '_idx']:
            super().__setattr__(name, val)
        return setattr(self._node, name, val)

    def __hasattr__(self, name):
        return self._node.__hasattr__(name)

    def __str__(self) -> str:
        return self._node.__str__()

    def __repr__(self) -> str:
        return self._node.__repr__()

    def __eq__(self, o: object) -> bool:
        if isinstance(o, NNNodeRef):
            return super().__eq__(o)
        return self._node.__eq__(o)

    def __hash__(self) -> int:
        return self._node.__hash__()

    def __call__(self, *args, **kwargs):
        raise ValueError("this is already a reference")


class Parameters(Node):
    CLS_OP_NAME = None
    NARGS = {1}
    NOT_GENERATED = False

    def __init__(self, name, *args, in_dims_hint=None, out_dims_hint=None, **kwargs):
        super().__init__(name, *args, **kwargs)
        del args, kwargs
        self._in_dims = None
        self._out_dims = None
        self._in_dims_hint = in_dims_hint
        self._out_dims_hint = out_dims_hint
        self._step_idx = -1
        self._valid_at_options = {"VCD_TRACE_ON": int, "DUMP_TENSORS": int,
                                  "OUT_HOME_MEM_LOC": str, "OUT_EXEC_MEM_LOC": str,
                                  "NODE_CNAME": str}
        self._at_options = NodeOptions(self._valid_at_options)
        self._meta = {}
        self._ker_in_order = None
        self._ker_out_order = None

    @abstractmethod
    def __str__(self):
        pass

    def __repr__(self):
        return f'{self.__class__.__name__}({self.name})'

    def __call__(self, *args, **kwargs):
        # set of number of args
        if isinstance(self.NARGS, set):
            if '*' not in self.NARGS and len(args) not in self.NARGS:
                raise ValueError("incorrect number of arguments")
            inputs, fragments = [], []
            for arg in args:
                if arg is not None and not isinstance(arg, NNNodeRef):
                    raise ValueError("expecting NNNodeRef")
                inputs.append(arg.ref[0] if arg else None)
                fragments.append(arg.ref[1] if arg else None)

        # list of possible inputs passed in kwargs. Things passed in args get
        # copied to kwargs with their index from the names in nargs
        elif isinstance(self.nargs, list):
            for idx, arg in enumerate(args):
                if idx >= len(self.nargs):
                    raise ValueError('Too many inputs for this node type')
                kwargs[self.nargs[idx]] = arg
            inputs = []
            fragments = []
            for name in self.nargs:
                if name in kwargs:
                    ref = kwargs[name]
                    if not isinstance(ref, NNNodeRef):
                        raise ValueError("expecting NNNodeRef")
                    inputs.append(ref[0])
                    fragments.append(ref[1])
                else:
                    inputs.append(None)
                    fragments.append(None)
            if inputs[0] is None:
                raise ValueError('Expecting at least an input')

        fragment = [frag for frag in fragments if frag is not None][0]
        if len(fragments) > 1:
            for other in fragments[1::]:
                if other is not None:
                    fragment.merge(other)
        for to_idx, from_tuple in enumerate(inputs):
            if from_tuple is not None:
                from_node, from_idx = from_tuple
                fragment.add_edge(NNEdge(from_node=from_node,
                                         from_idx=from_idx, to_node=self, to_idx=to_idx))
        return NNNodeRef(self, 0, fragment)

    @property
    def graph_label(self):
        """ Label used when drawing a graph. Should return a list of strings or lists of strings.
        The outer list defines rows and inner lists columns within a row
        """
        return [f'{self.CLS_OP_NAME}({self.name})']

    @property
    def graph_anon_label(self):
        """ Label used when drawing a graph with anonymous labels. Should return a list of strings or lists of strings.
        The outer list defines rows and inner lists columns within a row
        """
        return ["Op"]

    @property
    def meta(self):
        return self._meta

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
    def ker_in_order(self) -> list:
        return self._ker_in_order

    @ker_in_order.setter
    def ker_in_order(self, val: list):
        self._ker_in_order = val

    @property
    def ker_out_order(self) -> list:
        return self._ker_out_order

    @ker_out_order.setter
    def ker_out_order(self, val: list):
        self._ker_out_order = val

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
        LOG.debug("%s in dims set to %s", self.__class__.__name__,
                  [str(val) for val in value])
        self._in_dims = value

    @property
    def out_dims(self):
        return self._out_dims

    @out_dims.setter
    def out_dims(self, value):
        LOG.debug("%s out dims set to %s", self.__class__.__name__,
                  [str(val) for val in value])
        self._out_dims = value

    @property
    def node_cname(self):
        return self.at_options.node_cname

    @node_cname.setter
    def node_cname(self, val):
        self.at_options.node_cname = val

    @property
    @abstractmethod
    def can_equalize(self):
        pass

    @property
    def op_name(self):
        return self.CLS_OP_NAME

    def compute_load(self):
        return None

    @abstractmethod
    def get_parameter_size(self):
        pass

    @abstractmethod
    def get_output_size(self, in_dims):
        pass

    def set_input_size(self, dims: Sequence[Dim]):
        self.in_dims = clone_dims(dims, self.in_dims_hint)
        return self.in_dims

    def set_output_size(self, dims: Sequence[Dim]):
        self.out_dims = clone_dims(dims, self.out_dims_hint)
        return self.out_dims

    def verify(self, G):
        """ Override to cause this verification test to be run on all instances of this node.
        Should return a list of problem descriptions or an empty list if everything is OK
        """
        return []

    def get_parameters(self):
        return {}

    def set_parameters(self, val):
        pass

    def get_gen_ctrl(self):
        return GenCtrl(self.at_options)

    def clone_dim_with_hint(self, dim, hint_idx, hint_dir="in"):
        if hint_dir == "in":
            hints = self._in_dims_hint
        else:
            hints = self._out_dims_hint
        if dim.is_named and all(k in dim.keys for k in ['c', 'h', 'w']):
            return dim.clone(['c', 'h', 'w'])
        else:
            cloned_dim = dim.clone()
            hint = None if hints is None else hints[hint_idx]
            if hint:
                cloned_dim.apply_naming_hints(hint)
            return cloned_dim

    def clone_dims_with_hints(self, dims, hint_dir="in", hint_idx=None):
        if hint_dir == "in":
            hints = self._in_dims_hint
        else:
            hints = self._out_dims_hint

        assert hints is None or len(dims) == len(
            hints), "incorrect dimensions length"
        cloned_dims = []
        for dim_idx, dim in enumerate(dims):
            if dim.is_named and all(k in dim.keys for k in ['c', 'h', 'w']):
                cloned_dims.append(dim.clone(['c', 'h', 'w']))
            else:
                cloned_dim = dim.clone()
                hint = None if hints is None else hints[hint_idx] if hint_idx is not None else hints[dim_idx]
                if hint:
                    cloned_dim.apply_naming_hints(hint)
                cloned_dims.append(cloned_dim)
        return cloned_dims

    @property
    def is_not_generated(self):
        return self.NOT_GENERATED

    @staticmethod
    def cls_op_name(name):
        return Parameters.property_register("CLS_OP_NAME", name)

    @staticmethod
    def nargs(name):
        return Parameters.property_register("NARGS", name)

    @staticmethod
    def not_generated(cls):
        setattr(cls, "NOT_GENERATED", True)
        return cls

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco


cls_op_name = Parameters.cls_op_name
nargs = Parameters.nargs
not_generated = Parameters.not_generated


class InsensitiveToQuantization():
    '''Mixin that indicates that node does not carry out arithmetic on tensor and is insenitive to quantization'''


class SingleInputAndOutput():
    '''Mixin that indicates that node has a single input and output'''


class SensitiveToOrder():
    '''Mixin that indicates that the node must receive channel first input'''


class SameNumberOfDimensionsForInputs():
    '''Mixin that indicates that the node has multiple inputs that have the same dimension length'''


class CanFuseToExpression():
    '''Mixin that indicates that the node can fuse into an expression'''
    EXPRESSION_OP_CLS = None

    def should_fuse(self, node_set, qrec=None):
        return True

    @staticmethod
    def expression_op(op):
        def fuse_op(cls):
            if not issubclass(cls, CanFuseToExpression):
                raise ValueError(
                    "a class decorated with expression_op must inherit CanFuseToExpression")
            setattr(cls, 'EXPRESSION_OP_CLS', op)
            return cls

        return fuse_op

    def get_expression(self, *args):
        if not isinstance(self.EXPRESSION_OP_CLS, Symbol):
            raise ValueError("class must be decorated with expression_op")
#pylint: disable=not-callable
        return self.EXPRESSION_OP_CLS(*args)


#pylint: disable=invalid-name
expression_op = CanFuseToExpression.expression_op


class NoSizeChangeParameters(Parameters):

    def get_output_size(self, in_dims):
        return in_dims

    @abstractmethod
    def __str__(self):
        pass


class ComparableParameters():
    ''' Mixin that indicates that this operation can be compared with another of the same type
    to determine redundancy. It is assumed that if A==B and B==C then A==C.'''

    def is_same_operation_as(self, G, other):
        return False

    def can_be_grouped_with(self, other):
        return False

#pylint: disable=abstract-method


class FilterLikeParameters(Parameters, SingleInputAndOutput):
    def __init__(self, *args, stride=None, padding=None,
                 pad_type="zero", **kwargs):
        if stride is None:
            stride = StrideDim(1, 1)
        if padding is None:
            padding = PadDim(0)

        super(FilterLikeParameters, self).__init__(*args, **kwargs)
        self.stride = stride
        self.padding = padding
        self.pad_type = pad_type
        self._ker_in_order = [['c', 'h', 'w']]
        self._ker_out_order = [['c', 'h', 'w']]

    @property
    def graph_anon_label(self):
        return ['Filt']

    def has_at_zero_pad(self):
        if self.padding.has_at_pad():
            if self.pad_type != "zero":
                raise AttributeError(
                    "Padding is probably not compatible with AutoTiler")
            return True
        return False

    @property
    def can_equalize(self):
        return True


#pylint: disable=abstract-method


class FilterParameters(Parameters, SingleInputAndOutput):

    def __init__(self, *args, filt=None, has_bias=True, use_compressed=False, **kwargs):
        assert filt
        super(FilterParameters, self).__init__(*args, **kwargs)
        self.has_bias = has_bias
        self.filter = filt
        self._use_compressed = use_compressed
        self.stats = None
        # self._weights = None
        # self._biases = None
        self.details = None
        self.at_options.update_valid_options(CTRL_FEATURES)


class MultiplicativeBiasParameters(FilterParameters):
    def __init__(self, *args, **kwargs):
        super(MultiplicativeBiasParameters, self).__init__(*args, **kwargs)
        self.has_mul_bias = False
        self._mul_biases = None

    @property
    def mul_biases(self):
        return self._mul_biases

    @mul_biases.setter
    def mul_biases(self, val):
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
