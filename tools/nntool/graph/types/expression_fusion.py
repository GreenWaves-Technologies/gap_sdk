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

from graph.types.input_output import ConstantInputParameters
import logging
from collections import Counter
from functools import cmp_to_key

import numpy as np
from sympy import Symbol
from sympy.simplify import simplify

from .expressions.compose_expression import compose_expression
from .expressions.expr_state import ExprState
from .expressions.expression import Expression
from .expressions.sympy_types import ATClip, ATScale
from .fusions import FusionBase, FusionInputParameters, FusionOutputParameters

LOG = logging.getLogger("nntool." + __name__)


class ExpressionFusionParameters(FusionBase):
    fusion_op_name = "expression"

    def __init__(self, *args, constant_inputs=None, **kwargs):
        super(ExpressionFusionParameters, self).__init__(*args, **kwargs)
        self._constant_inputs = constant_inputs
        self._input_symbols, self._expr_outputs, self._expr_inter = self.decompose()

    @property
    def constant_inputs(self):
        return self._constant_inputs

    @property
    def num_inputs(self):
        return len([node for node in self.subgraph.nodes()
                    if isinstance(node, FusionInputParameters)])

    def decompose(self, node_scale_map=None):
        LOG.debug("++++ DECOMPOSE ++++ scaled=%s", node_scale_map is not None)
        # assumption - only single output nodes for all nodes in an expression
        intermediates = set([node for node in self.subgraph.nodes()
                             if len(self.subgraph.out_edges(node.name)) > 1 and not isinstance(node, ConstantInputParameters)])
        outputs = sorted([node for node in self.subgraph.nodes()
                          if isinstance(node, FusionOutputParameters)], key=lambda x: x.idx)
        inputs = set([node for node in self.subgraph.nodes()
                      if isinstance(node, FusionInputParameters)])
        LOG.info("expression decomposed into %s intermediate and %s output expressions",
                 len(intermediates), len(outputs))
        if node_scale_map is not None:
            scaling = True
            node_symbol_map = {node: ExprState(
                Symbol(node.name), 7, q=0, scale=node_scale_map[node]) for node in (inputs)}
        else:
            scaling = False
            node_symbol_map = {node: ExprState(Symbol(node.name), 7) for node in (inputs)}

        expr_outputs = []
        expr_inter = []

        # generate the intermediate expressions
        for node in intermediates:
            LOG.debug("++++ INTERMEDIATE %s ++++", node.name)
            var = compose_expression(self.subgraph, node_symbol_map, node, scaling=scaling)
            var.expr = simplify(var.expr)
            expression = Expression(node.name, var, is_quant=node_scale_map is not None, is_intermediate=True)
            node_symbol_map[node] = expression.symbolic_var
            expr_inter.append(expression)
            LOG.info("intermediate function %s", expression)
            LOG.debug("---- INTERMEDIATE %s ----", node.name)
        # sort the intermediate expressions by the symbols that they use
        expr_inter.sort(key=cmp_to_key(
            lambda x, y: -1 if x.symbol not in y.expr.expr_free_symbols else 1))

        # generate the output expressions
        for node in outputs:
            LOG.debug("++++ OUTPUT %s ++++", node.name)
            var = compose_expression(self.subgraph, node_symbol_map,
                                     node, scaling=scaling)
            if scaling:
                out_scale = node_scale_map[node]
                # Note: this updates the scale map so remeber to check it when creating qrec
                LOG.debug("output %s scale %s Q%s.%s", node.name, var.scale, var.ibits, var.q)
                var = ExprState(
                    ATClip(ATScale.from_scales(simplify(var.expr), var.scale,
                                               out_scale, 30-var.length, from_q=var.q, to_q=0), 7),
                    7,
                    q=0,
                    scale=out_scale
                )

            expression = Expression(node.name, var, is_quant=node_scale_map is not None)
            expr_outputs.append(expression)
            LOG.info("output function %s", expression)
            LOG.debug("---- OUTPUT %s ----", node.name)
        # sort the inputs by idx
        input_nodes = sorted([node for node in self.subgraph.nodes() if isinstance(
            node, FusionInputParameters)], key=lambda x: x.idx)
        LOG.debug("---- DECOMPOSE ---- scaled=%s", node_scale_map is not None)
        return [Symbol(node.name) for node in input_nodes], expr_outputs, expr_inter

    def execute(self, input_tensors, input_symbols=None, expr_inter=None, expr_outputs=None, details=None, use_imps=True):
        if input_symbols is None:
            input_symbols = self._input_symbols
        if expr_inter is None:
            expr_inter = self._expr_inter
        if expr_outputs is None:
            expr_outputs = self._expr_outputs
        assert len(input_tensors) == len(input_symbols)
        # keep track of the symbols that need to be substituted
        subs = {symbol.name: input_tensors[idx] for idx, symbol in enumerate(input_symbols)}
        # execute the already ordered sub expressions
        if details is not None:
            detail = details.get('subexpressions')
            if not detail:
                detail = [{'min': float('inf'), 'max': -float('inf')} for _ in expr_inter]
                details['subexpressions'] = detail
        else:
            detail = None
        for idx, expression in enumerate(expr_inter):
            subs[expression.symbol.name] = expression.execute(subs, use_imps=use_imps)
            if detail:
                det = detail[idx]
                det['min'] = min(det['min'], np.min(subs[expression.symbol.name]))
                det['max'] = max(det['max'], np.max(subs[expression.symbol.name]))
                det['res'] = subs[expression.symbol.name]

        res = []
        # now execute the output expressions ordered by output idx
        for expression in expr_outputs:
            res.append(expression.execute(subs, use_imps=use_imps))
        return res

    def __str__(self):
        occurrences = Counter([node.op_name for node in self.contained_nodes()])
        return ", ".join(["%s: %s" % (op, cnt) for op, cnt in occurrences.items()])
