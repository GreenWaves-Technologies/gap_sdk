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

from nntool.expressions.symbolic.symbol import Symbol
from nntool.generation.at_types.gen_ctrl import CTRL_FEATURES, GenCtrl
from nntool.graph.dim import Dim, PadDim, StrideDim
from nntool.graph.manipulations.dump_refs import DumpTensorRef
from nntool.stats.ranges_utils import collect_stat
from nntool.utils.graph import Edge, Node, NodeRef
from nntool.utils.option_list import OptionList

LOG = logging.getLogger(__name__)


class NodeOptions(OptionList):
    pass

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

    def __getattr__(self, name):
        return getattr(self._node, name)

    def __setattr__(self, name, val):
        if name in ['_node', '_G', '_idx']:
            return super().__setattr__(name, val)
        return setattr(self._node, name, val)

    def __hasattr__(self, name):
        return self._node.__hasattr__(name)

    def __str__(self) -> str:
        return self._node.__str__()

    def __repr__(self) -> str:
        return self._node.__repr__()


def init_dims(dims):
    if dims is None:
        return dims
    return [
        Dim.unnamed(dim) if isinstance(dim, (list, tuple)) else dim for dim in dims
    ]


class NNNodeBase(Node):
    CLS_OP_NAME = None
    NARGS = {1}
    NOT_GENERATED = False

    def __init__(self, name, in_dims_hint=None, out_dims_hint=None, in_dims=None, out_dims=None, step_idx=-1,
                 ker_in_order=None, ker_out_order=None, meta=None, at_options=None, **kwargs):
        super().__init__(name, **kwargs)
        self._in_dims = init_dims(in_dims)
        self._out_dims = init_dims(out_dims)
        self._in_dims_hint = in_dims_hint
        self._out_dims_hint = out_dims_hint
        self._step_idx = step_idx
        if at_options is None:
            at_options = {}
        self._at_options = NodeOptions(
            at_options,
            valid_options={
                "VCD_TRACE_ON": int, "DUMP_TENSORS": int,
                "OUT_HOME_MEM_LOC": str, "OUT_EXEC_MEM_LOC": str,
                "NODE_CNAME": str
            })
        if meta is None:
            meta = {}
        self._meta = meta
        self._ker_in_order = ker_in_order
        self._ker_out_order = ker_out_order

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.step_idx != -1:
            super_attrs['step_idx'] = self.step_idx
        super_attrs.update({k: getattr(self, f'_{k}') for k in [
            "in_dims_hint", "out_dims_hint", "meta", "ker_in_order",
            "ker_out_order", "in_dims", "out_dims"
        ] if getattr(self, f'_{k}')})
        if self._at_options:
            super_attrs['at_options'] = dict(self._at_options)
        return super_attrs

    @abstractmethod
    def __str__(self):
        pass

    @property
    def _edge_class(self):
        return NNEdge

    @property
    def _noderef_class(self):
        return NNNodeRef

    @property
    def _nargs(self):
        return self.NARGS

    def __call__(self, *args, num_outputs=1, **kwargs):
        # set of number of args
        if isinstance(self._nargs, set):
            if '*' not in self._nargs and len(args) not in self._nargs:
                raise ValueError("incorrect number of arguments")

        # list of possible inputs passed in kwargs. Things passed in args get
        # copied to kwargs with their index from the names in _nargs
        elif isinstance(self._nargs, list):
            for idx, arg in enumerate(args):
                if idx >= len(self._nargs):
                    raise ValueError('Too many inputs for this node type')
                kwargs[self._nargs[idx]] = arg
            args = []
            for name in self._nargs:
                if name in kwargs:
                    ref = kwargs[name]
                    if not isinstance(ref, NNNodeRef):
                        raise ValueError("expecting NNNodeRef")
                    args.append(ref)
                else:
                    args.append(None)
            if args[0] is None:
                raise ValueError('Expecting at least an input')

        return super().__call__(*args, num_outputs=num_outputs)

    @property
    def no_model_code(self) -> bool:
        """Returns True if node results in no kernel, global or local generation in model

        Returns:
            bool: True if nothing generated
        """
        return self.NOT_GENERATED

    @property
    def exclude_from_generation(self) -> bool:
        return self.no_model_code

    @property
    def does_nothing(self) -> bool:
        """Returns True if the node does not modify its input in any way

        Returns:
            bool: True if node could be eliminated with no effect
        """
        return False

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
    def at_options(self) -> NodeOptions:
        """AutoTiler node options

        Returns:
            NodeOptions: AutoTiler node options
        """
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

    def details_collector(self, stats, stat, details):
        pass

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

    @staticmethod
    def cls_op_name(name):
        return NNNodeBase.property_register("CLS_OP_NAME", name)

    @staticmethod
    def nargs(name):
        return NNNodeBase.property_register("NARGS", name)

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


cls_op_name = NNNodeBase.cls_op_name
nargs = NNNodeBase.nargs
not_generated = NNNodeBase.not_generated


class InsensitiveToQuantizationMixin():
    '''Mixin that indicates that node does not carry out arithmetic on tensor and is insenitive to quantization'''


class SingleInputAndOutputMixin():
    '''Mixin that indicates that node has a single input and output'''


class SensitiveToOrderMixin():
    '''Mixin that indicates that the node must receive channel first input'''


class SameNumberOfDimensionsForInputsMixin():
    '''Mixin that indicates that the node has multiple inputs that have the same dimension length'''


class CanFuseToExpressionMixin():
    '''Mixin that indicates that the node can fuse into an expression'''
    EXPRESSION_OP_CLS = None

    def should_fuse(self, node_set, qrec=None):
        return True

    @staticmethod
    def expression_op(op):
        def fuse_op(cls):
            if not issubclass(cls, CanFuseToExpressionMixin):
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
expression_op = CanFuseToExpressionMixin.expression_op


class NoSizeChangeNodeBase(NNNodeBase):

    def get_output_size(self, in_dims):
        return in_dims

    @abstractmethod
    def __str__(self):
        pass


class ComparableNodeMixin():
    ''' Mixin that indicates that this operation can be compared with another of the same type
    to determine redundancy. It is assumed that if A==B and B==C then A==C.'''

    def is_same_operation_as(self, G, other):
        return False

    def can_be_grouped_with(self, G, other):
        return False

#pylint: disable=abstract-method


class FilterLikeNodeBase(NNNodeBase, SingleInputAndOutputMixin):
    def __init__(self, *args, stride=None, padding=None,
                 pad_type="zero", ker_in_order=None, ker_out_order=None, **kwargs):
        if stride is None:
            stride = StrideDim(1, 1)
        if padding is None:
            padding = PadDim(0)
        if ker_in_order is None:
            ker_in_order = [['c', 'h', 'w']]
        if ker_out_order is None:
            ker_out_order = [['c', 'h', 'w']]

        super(FilterLikeNodeBase, self).__init__(
            *args,
            ker_in_order=ker_in_order, ker_out_order=ker_out_order,
            **kwargs)
        self.stride = stride
        self.padding = padding
        self.pad_type = pad_type

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.stride != 1:
            super_attrs['stride'] = self.stride
        if self.padding != 0:
            super_attrs['padding'] = self.padding
            super_attrs['pad_type'] = self.pad_type
        return super_attrs

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


class FilterNodeBase(NNNodeBase, SingleInputAndOutputMixin):

    def __init__(self, *args, filter_dim=None, has_bias=True, use_compressed=False, **kwargs):
        assert filter_dim
        super(FilterNodeBase, self).__init__(*args, **kwargs)
        self.has_bias = has_bias
        self._filter_dim = filter_dim
        self._use_compressed = use_compressed
        self.stats = None
        # self._weights = None
        # self._biases = None
        self.details = None
        self.at_options.update_valid_options(CTRL_FEATURES)

    @property
    def filter_dim(self):
        return self._filter_dim

    @filter_dim.setter
    def filter_dim(self, val):
        self._filter_dim = val

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, f'_{k}') for k in [
            "filter_dim", "use_compressed"
        ] if getattr(self, f'_{k}')})
        return super_attrs

    def details_collector(self, stats, stat, details):
        collect_stat(stat, 'range_acc', details, details_name='acc')


class MultiplicativeBiasNodeBase(FilterNodeBase):
    def __init__(self, *args, **kwargs):
        super(MultiplicativeBiasNodeBase, self).__init__(*args, **kwargs)
        self.has_mul_bias = False
        self._mul_biases = None

    def details_collector(self, stats, stat, details):
        super().details_collector(stats, stat, details)
        if self.mul_biases:
            collect_stat(stat, 'range_pre_mul_bias', details,
                         details_name='pre_mul_bias')

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.has_mul_bias:
            super_attrs['mul_biases'] = DumpTensorRef(f'{self.name}_mulbias')
        return super_attrs

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

    def __repr__(self) -> str:
        return f"EdgeParameters({self.creating_node.name}:{self.creating_node_idx})"


class NNEdge(Edge):
    def __init__(self, from_node: Union[str, Node], to_node: Union[str, Node],
                 params: NNNodeBase = None,
                 from_idx: int = 0, to_idx: int = 0):
        super().__init__(from_node, to_node, from_idx, to_idx)
        self.params = params
