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

from graph.types.input_output import ConstantInputParameters

from utils.symbolic.function_collection import FunctionCollection
from utils.symbolic.symbol import Constant, Variable

from .fusions import FusionBase, FusionInputParameters, FusionOutputParameters

LOG = logging.getLogger("nntool." + __name__)


class ExpressionFusionParameters(FusionBase):
    fusion_op_name = "expression"

    def __init__(self, *args, constant_inputs=None, **kwargs):
        super(ExpressionFusionParameters, self).__init__(*args, **kwargs)
        self._constant_inputs = constant_inputs
        self._input_symbols, self._output_symbols, self._func_col = self.decompose()
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
    def compose_expression(subgraph, node, inter_vars, variable=None):
        # this is an assignment
        if variable:
            return (variable,
                    ExpressionFusionParameters.compose_expression(subgraph, node, inter_vars))
        # this node is contained in a previous expression
        if node in inter_vars:
            return inter_vars[node]
        # if it is an output then it's an assignment
        if isinstance(node, FusionOutputParameters):
            edge = subgraph.indexed_in_edges(node.name)[0]
            return (Variable(node.name, shape=edge.from_node.out_dims[edge.from_idx].shape),
                    ExpressionFusionParameters.compose_expression(subgraph,
                                                                  edge.from_node,
                                                                  inter_vars))
        # input so variable with the same name
        if isinstance(node, FusionInputParameters):
            return Variable(node.name, shape=node.dims.shape)
        # constant so get value - it's a scalar so flatten it
        if isinstance(node, ConstantInputParameters):
            return Constant(node.value.flatten())
        # return the symbol of the current node with all its arguments in it
        return node.EXPRESSION_OP_CLS(*(ExpressionFusionParameters.compose_expression(subgraph,
                                                                                      edge.from_node,
                                                                                      inter_vars)
                                        for edge in subgraph.indexed_in_edges(node.name)),
                                      name=node.name)

    def decompose(self):
        # assumption - only single output nodes for all nodes in an expression
        intermediates = set([node for node in self.subgraph.nodes()
                             if len(self.subgraph.out_edges(node.name)) > 1 and not isinstance(node, ConstantInputParameters)])
        outputs = sorted([node for node in self.subgraph.nodes()
                          if isinstance(node, FusionOutputParameters)], key=lambda x: x.idx)
        inputs = set([node for node in self.subgraph.nodes()
                      if isinstance(node, FusionInputParameters)])

        LOG.info("expression decomposed into %s intermediate and %s output expressions",
                 len(intermediates), len(outputs))

        expressions = []
        inter_vars = {}
        # TODO - Intermediates are not sorted here so there may be interdependences
        for node in intermediates:
            # This assumes that all contained nodes output on idx 0 which is
            # ok for now
            variable = Variable(node.name, shape=node.out_dims[0].shape)
            expr = self.compose_expression(self.subgraph,
                                           node,
                                           inter_vars,
                                           variable=variable)
            inter_vars[node] = variable
            expressions.append(expr)

        for node in outputs:
            expr = self.compose_expression(self.subgraph, node, inter_vars)
            expressions.append(expr)

        # sort the inputs by idx
        inputs = sorted([node for node in inputs], key=lambda x: x.idx)
        outputs = sorted([node for node in outputs], key=lambda x: x.idx)
        func_col = FunctionCollection(expressions)

        return [node.name for node in inputs], [node.name for node in outputs], func_col

    # def execute(self, input_tensors, input_symbols=None, expr_inter=None, expr_outputs=None, details=None, use_imps=True):
    #     if input_symbols is None:
    #         input_symbols = self._input_symbols
    #     if expr_inter is None:
    #         expr_inter = self._expr_inter
    #     if expr_outputs is None:
    #         expr_outputs = self._expr_outputs
    #     assert len(input_tensors) == len(input_symbols)
    #     # keep track of the symbols that need to be substituted
    #     subs = {symbol.name: input_tensors[idx] for idx, symbol in enumerate(input_symbols)}
    #     # execute the already ordered sub expressions
    #     if details is not None:
    #         detail = details.get('subexpressions')
    #         if not detail:
    #             detail = [{'min': float('inf'), 'max': -float('inf')} for _ in expr_inter]
    #             details['subexpressions'] = detail
    #     else:
    #         detail = None
    #     for idx, expression in enumerate(expr_inter):
    #         subs[expression.symbol.name] = expression.execute(subs, use_imps=use_imps)
    #         if detail:
    #             det = detail[idx]
    #             det['min'] = min(det['min'], np.min(subs[expression.symbol.name]))
    #             det['max'] = max(det['max'], np.max(subs[expression.symbol.name]))
    #             det['res'] = subs[expression.symbol.name]

    #     res = []
    #     # now execute the output expressions ordered by output idx
    #     for expression in expr_outputs:
    #         res.append(expression.execute(subs, use_imps=use_imps))
    #     return res

    def __str__(self):
        occurrences = Counter([node.op_name for node in self.contained_nodes()])
        return ", ".join(["%s: %s" % (op, cnt) for op, cnt in occurrences.items()])
