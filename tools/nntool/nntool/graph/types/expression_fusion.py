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
from collections import Counter
from copy import deepcopy

from nntool.expressions.symbolic.iteration_space import Assignments
from nntool.expressions.symbolic.symbol import Constant, Variable
from nntool.graph.manipulations.dump_refs import DumpNodeRef

from ..dim import Dim
from .base import ComparableNodeMixin, cls_op_name
from .constant_input import ConstantInputNode
from .fusions import (FusionInputNode, FusionNodeBase, FusionOutputNode,
                      dont_quantize_internals)
from .tensor_arithmetic import BroadcastableNodeBase

LOG = logging.getLogger(__name__)


@dont_quantize_internals
@cls_op_name('expression')
class ExpressionFusionNode(FusionNodeBase, BroadcastableNodeBase, ComparableNodeMixin):
    fusion_op_name = "Expression"

    def __init__(self, *args, constant_inputs=None, qrecs=None, **kwargs):
        super(ExpressionFusionNode, self).__init__(*args, **kwargs)
        self._constant_inputs = constant_inputs
        self._input_symbols, self._output_symbols, self._func_col = self.decompose(
            qrecs=qrecs)
        self._qfunc_col = None

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.constant_inputs is not None:
            super_attrs['constant_inputs'] = [DumpNodeRef(node.name) for node in self.constant_inputs]
        return super_attrs

    @property
    def activation(self):
        return 'custom'

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
                    if isinstance(node, FusionInputNode)])

    @staticmethod
    def compose_expression(subgraph, node, inter_vars, variable=None, qrecs=None):
        # this is an assignment
        if variable:
            return (variable,
                    ExpressionFusionNode.compose_expression(subgraph, node, inter_vars, qrecs=qrecs))
        # this node is contained in a previous expression
        if node in inter_vars:
            return inter_vars[node]
        # if it is an output then it's an assignment
        if isinstance(node, FusionOutputNode):
            edge = subgraph.indexed_in_edges(node.name)[0]
            out_var = Variable(
                node.name, shape=edge.from_node.out_dims[edge.from_idx].shape)
            ExpressionFusionNode.set_min_max(qrecs, out_var, node)
            return (out_var,
                    ExpressionFusionNode.compose_expression(subgraph,
                                                                  edge.from_node,
                                                                  inter_vars,
                                                                  qrecs=qrecs))
        # input so variable with the same name
        if isinstance(node, FusionInputNode):
            out_sym = Variable(node.name, shape=node.dims.shape)
        # constant so get value - it's a scalar so flatten it
        elif isinstance(node, ConstantInputNode):
            out_sym = Constant(node.value.flatten())
        # return the symbol of the current node with all its arguments in it
        else:
            out_sym = node.EXPRESSION_OP_CLS(*(ExpressionFusionNode.compose_expression(subgraph,
                                                                                             edge.from_node,
                                                                                             inter_vars, qrecs=qrecs)
                                               for edge in subgraph.indexed_in_edges(node.name)),
                                             name=node.name)
        ExpressionFusionNode.set_min_max(qrecs, out_sym, node)
        return out_sym

    @staticmethod
    def set_min_max(qrecs, symbol, node):
        if not qrecs:
            return
        qrec = qrecs.get(node.name)
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
        if not isinstance(other, ExpressionFusionNode):
            return False
        if len(self.func_col) != 1 or len(other.func_col) != 1:
            return False
        if self.func_col[0][1].equivalent(other.func_col[0][1]):
            return True
        return False

    def decompose(self, qrecs=None):
        # assumption - only single output nodes for all nodes in an expression
        # sort intermediates by execution order so that any interdependencies are managed
        liveness = list(self.subgraph.topological_sort())
        intermediates = set([node for node in self.subgraph.nodes()
                             if len(self.subgraph.out_edges(node.name)) > 1 and not isinstance(node, (ConstantInputNode, FusionInputNode))])
        intermediates = sorted(list(intermediates), key=liveness.index)
        # outputs do not depend on each other so can be sorted by idx
        outputs = sorted([node for node in self.subgraph.nodes()
                          if isinstance(node, FusionOutputNode)], key=lambda x: x.idx)
        # inputs sorted by idx
        inputs = set([node for node in self.subgraph.nodes()
                      if isinstance(node, FusionInputNode)])
        inputs = sorted(list(inputs), key=lambda x: x.idx)

        LOG.info("expression decomposed into %s intermediate and %s output expressions",
                 len(intermediates), len(outputs))

        expressions = Assignments()
        # start with all the inputs
        all_vars = {node: Variable(
            node.name, shape=node.dims.shape) for node in inputs}
        for node in intermediates:
            # This assumes that all contained nodes output on idx 0 which is
            # ok for now
            variable = Variable(
                f'inter_{node.name}', shape=node.out_dims[0].shape)
            self.set_min_max(qrecs, variable, node)
            expr = self.compose_expression(self.subgraph,
                                           node,
                                           all_vars,
                                           variable=variable,
                                           qrecs=qrecs)
            all_vars[node] = variable
            expressions.add(*expr)

        for node in outputs:
            expr = self.compose_expression(
                self.subgraph, node, all_vars, qrecs=qrecs)
            expressions.add(*expr)

        return [node.name for node in inputs], [node.name for node in outputs], expressions

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
            self.func_col.update()

        out_dims = [Dim.unnamed(self.func_col.output_shapes[name]) for name in self.output_symbols]
        return out_dims

    def __str__(self):
        occurrences = Counter(
            [node.op_name for node in self.contained_nodes()])
        return ", ".join(["%s: %s" % (op, cnt) for op, cnt in occurrences.items()])
