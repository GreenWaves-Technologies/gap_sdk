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

from copy import deepcopy
import logging
from collections import Counter

from expressions.symbolic.function_collection import FunctionCollection
from expressions.symbolic.symbol import Constant, Variable

from utils.node_id import NodeId

from .base import ComparableParameters, cls_op_name
from .constant_input import ConstantInputParameters
from .fusions import (FusionBase, FusionInputParameters,
                      FusionOutputParameters, dont_quantize_internals)
from .tensor_arithmetic import Broadcastable

LOG = logging.getLogger("nntool." + __name__)


@dont_quantize_internals
@cls_op_name('expression')
class ExpressionFusionParameters(FusionBase, Broadcastable, ComparableParameters):
    fusion_op_name = "Expression"

    def __init__(self, *args, constant_inputs=None, qrecs=None, **kwargs):
        super(ExpressionFusionParameters, self).__init__(*args, **kwargs)
        self._constant_inputs = constant_inputs
        self._input_symbols, self._output_symbols, self._func_col = self.decompose(
            qrecs=qrecs)
        self._qfunc_col = None

    @property
    def input_symbols(self):
        return self._input_symbols

    @property
    def input_shuffle(self):
        return [self._input_symbols.index(var_name) for var_name in self._func_col.input_names]

    @property
    def output_symbols(self):
        return self._output_symbols

    @property
    def output_shuffle(self):
        return [self._output_symbols.index(var_name) for var_name in self._func_col.output_names]

    @property
    def func_col(self):
        return self._func_col

    @property
    def qfunc_col(self):
        return self._qfunc_col

    @property
    def constant_inputs(self):
        return self._constant_inputs

    @property
    def num_inputs(self):
        return len([node for node in self.subgraph.nodes()
                    if isinstance(node, FusionInputParameters)])

    @staticmethod
    def compose_expression(subgraph, node, inter_vars, variable=None, qrecs=None):
        # this is an assignment
        if variable:
            return (variable,
                    ExpressionFusionParameters.compose_expression(subgraph, node, inter_vars, qrecs=qrecs))
        # this node is contained in a previous expression
        if node in inter_vars:
            return inter_vars[node]
        # if it is an output then it's an assignment
        if isinstance(node, FusionOutputParameters):
            edge = subgraph.indexed_in_edges(node.name)[0]
            out_var = Variable(
                node.name, shape=edge.from_node.out_dims[edge.from_idx].shape)
            ExpressionFusionParameters.set_min_max(qrecs, out_var, node)
            return (out_var,
                    ExpressionFusionParameters.compose_expression(subgraph,
                                                                  edge.from_node,
                                                                  inter_vars,
                                                                  qrecs=qrecs))
        # input so variable with the same name
        if isinstance(node, FusionInputParameters):
            out_sym = Variable(node.name, shape=node.dims.shape)
        # constant so get value - it's a scalar so flatten it
        elif isinstance(node, ConstantInputParameters):
            out_sym = Constant(node.value.flatten())
        # return the symbol of the current node with all its arguments in it
        else:
            out_sym = node.EXPRESSION_OP_CLS(*(ExpressionFusionParameters.compose_expression(subgraph,
                                                                                             edge.from_node,
                                                                                             inter_vars, qrecs=qrecs)
                                               for edge in subgraph.indexed_in_edges(node.name)),
                                             name=node.name)
        ExpressionFusionParameters.set_min_max(qrecs, out_sym, node)
        return out_sym

    @staticmethod
    def set_min_max(qrecs, symbol, node):
        if not qrecs:
            return
        qrec = qrecs.get(NodeId(node))
        if not qrec:
            return
        qtype = qrec.out_qs[0]
        symbol.control.add_min_max(symbol, qtype.min_val, qtype.max_val)

    def details_collector(self, stats, stat, details):
        if 'expression' in stat:
            stat = stat['expression']
            for sym_name, rec in details.items():
                if sym_name == "results":
                    continue
                stat_rec = stat.setdefault(
                    sym_name, {'min': float('inf'), 'max': float('-inf')})
                stat_rec['min'] = min(stat_rec['min'], rec['min'])
                stat_rec['max'] = max(stat_rec['max'], rec['max'])
        else:
            stat['expression'] = deepcopy(details)

    def is_same_operation_as(self, G, other):
        if not isinstance(other, ExpressionFusionParameters):
            return False
        if len(self.func_col.functions) != 1 or len(other.func_col.functions) != 1:
            return False
        if next(iter(self.func_col.functions.values())).equivalent(next(iter(other.func_col.functions.values()))):
            return True
        return False

    def decompose(self, qrecs=None):
        # assumption - only single output nodes for all nodes in an expression
        intermediates = set([node for node in self.subgraph.nodes()
                             if len(self.subgraph.out_edges(node.name)) > 1 and not isinstance(node, (ConstantInputParameters, FusionInputParameters))])
        outputs = sorted([node for node in self.subgraph.nodes()
                          if isinstance(node, FusionOutputParameters)], key=lambda x: x.idx)
        inputs = set([node for node in self.subgraph.nodes()
                      if isinstance(node, FusionInputParameters)])

        LOG.info("expression decomposed into %s intermediate and %s output expressions",
                 len(intermediates), len(outputs))

        expressions = []
        inter_vars = {node: Variable(
            node.name, shape=node.dims.shape) for node in inputs}
        # TODO - Intermediates are not sorted here so there may be interdependences
        for node in intermediates:
            # This assumes that all contained nodes output on idx 0 which is
            # ok for now
            variable = Variable(
                f'inter_{node.name}', shape=node.out_dims[0].shape)
            self.set_min_max(qrecs, variable, node)
            expr = self.compose_expression(self.subgraph,
                                           node,
                                           inter_vars,
                                           variable=variable,
                                           qrecs=qrecs)
            inter_vars[node] = variable
            expressions.append(expr)

        for node in outputs:
            expr = self.compose_expression(
                self.subgraph, node, inter_vars, qrecs=qrecs)
            expressions.append(expr)

        # sort the inputs by idx
        inputs = sorted([node for node in inputs], key=lambda x: x.idx)
        outputs = sorted([node for node in outputs], key=lambda x: x.idx)
        func_col = FunctionCollection(expressions)

        return [node.name for node in inputs], [node.name for node in outputs], func_col

    def get_output_size(self, in_dims):
        # the input shapes may have changed so the expression variables shapes could have
        # changed and the iterators will need to be recalculated
        dim_change = False
        in_vars = [self.func_col.variables[name] for name in self.input_symbols]
        for idx, dim in enumerate(in_dims):
            shape = tuple(dim.shape)
            if tuple(in_vars[idx].shape) != shape:
                in_vars[idx].shape = shape
                dim_change = True
        if dim_change:
            self.func_col.set_var_shapes()
        out_dims = super().get_output_size(in_dims)
        if dim_change: # if the input shapes haven't changed then the output shapes have not changed
            out_vars = [self.func_col.variables[name] for name in self.output_symbols]
            for idx, dim in enumerate(out_dims):
                out_vars[idx].shape = tuple(dim.shape)
            self.func_col.init_indexes() # recalculate the iterators
        return out_dims

    def __str__(self):
        occurrences = Counter(
            [node.op_name for node in self.contained_nodes()])
        return ", ".join(["%s: %s" % (op, cnt) for op, cnt in occurrences.items()])
