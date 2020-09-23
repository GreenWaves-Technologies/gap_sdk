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
import math
from copy import deepcopy

import numpy as np
from sympy import (Add, Float, Integer, Max, Min, Mul, Pow, UnevaluatedExpr,
                   factor, log, sqrt)

from graph.types import (ActivationParameters, BinaryOpParameters,
                         ConstantInputParameters, FusionInputParameters,
                         FusionOutputParameters, MatrixAddParameters,
                         MatrixDivParameters, MatrixMulParameters,
                         MatrixSubParameters, UnaryOpParameters)
from utils.stats_funcs import calc_bits

from .expr_state import ExprState
from .misc import (check_mul_overflow, check_overflow, equalize_scales,
                   from_node)
from .sympy_types import ATLShift, ATNorm, ATScale, HSigmoid, HTanh

LOG = logging.getLogger("nntool." + __name__)

BINARY_OP_MAPPING = {
    "maximum": Max,
    "minimum": Min,
    "pow": Pow,
}


def generate_mul(var1, var2, scaling):
    if scaling:
        if var1.is_not_scaled and var2.is_not_scaled:
            return ExprState(Mul(var1.expr, var2.expr), 8)
        var1, var2 = check_mul_overflow(var1, var2)
        return ExprState(Mul(var1.expr, var2.expr),
                         var1.ibits + var2.ibits,
                         q=var1.q + var2.q,
                         scale=var1.scale * var2.scale)
    return ExprState(Mul(var1.expr, var2.expr), var1.ibits + var2.ibits)


def generate_div(var1, var2, scaling):
    if scaling:
        if var1.is_not_scaled and var2.is_not_scaled:
            return ExprState(Mul(var1.expr, Pow(var2.expr, -1)), 8)
        if var2.is_not_scaled:
            # can it be turned into a shift
            val2 = var2.expr
            if val2.is_integer and val2 % 2 == 0:
                if val2 == 0:
                    raise ValueError("division by zero")
                norm = val2//2
                var1.length -= norm
                var1.expr = ATNorm(var1.expr, norm)
                return var1
        out_scale = (var1.scale * pow(2, -var1.q))/(var2.scale * pow(2, -var2.q))
        left_shift = min(31-var1.length, 16)
        right_shift = max(left_shift // 2, 8)
        # shift left available bits
        # shift back leaving at least end_bits of available
        # change in scale is pow(2, -left_shift + right_shift)
        # worst case length comes from divide by one
        return ExprState(
            ATNorm(
                Mul(
                    ATLShift(var1.expr, left_shift),
                    Pow(var2.expr, -1)
                ),
                right_shift),
            var1.ibits + left_shift - right_shift,
            q=0,
            scale=out_scale * pow(2, -left_shift + right_shift))
    return ExprState(Mul(var1.expr, Pow(var2.expr, -1)), min(var1.ibits - var2.ibits + 1, 0))


def generate_sqrt(var, scaling):
    if scaling:
        if var.is_not_scaled:
            return ExprState(sqrt(var.expr), var.ibits)
        # adjust to Q15 fraction
        shift = 15 - var.length
        if shift == 0:
            subexpr = sqrt(var.expr)
        else:
            subexpr = sqrt(ATLShift(var.expr, shift))
        # The number is now a Q15 fraction so the scale is multiplied by Q15 minus the correction
        cor_scale = math.sqrt(var.scale * 2**(15 - shift))
        # Now put the Q15 into the scale
        out_scale = cor_scale * pow(2, -15)
        return ExprState(subexpr, int(math.ceil((var.ibits + shift) / 2)), q=0, scale=out_scale)
    return ExprState(sqrt(var.expr), int(math.ceil(var.ibits / 2)))


def generate_log(var, scaling):
    if scaling:
        if var.is_not_scaled:
            return ExprState(math.log(var.expr), var.ibits)
        # adjust to Q15 fraction
        shift = 15 - var.length
        if shift == 0:
            subexpr = log(var.expr)
        else:
            subexpr = log(ATLShift(var.expr, shift))
        # The number is now a Q15 fraction so the scale is multiplied by Q15 minus the correction
        cor_scale = int(math.floor(math.log(var.scale * 2**(15-shift)) * pow(2, 15) + 0.5))
        # Scale is now 1 * Q15
        out_scale = pow(2, -15)
        return ExprState(UnevaluatedExpr(Add(subexpr, cor_scale)), int(math.ceil((var.ibits + shift) / 2)), q=0, scale=out_scale)
    return ExprState(log(var.expr), int(math.ceil(var.ibits / 2)))


def generate_tanh(var, scaling):
    if scaling:
        # what is the current maximum value of the input?
        # We want to: (a) represent (1) precisely
        #             (b) make sure that scaling to this rep does not overflow
        # Find the closest power of 2 greater than the current scale
        closest_repr = math.log2(var.scale)
        closest_repr = min(math.floor(closest_repr), -7)
        new_scale = pow(2, closest_repr)
        cur_max_val = math.ceil(pow(2, var.ibits) * var.scale)
        new_scaled_max_val = math.ceil(cur_max_val / new_scale)
        assert calc_bits(new_scaled_max_val) + var.q <= 31, "risk of overflow in htanh"
        new_q = 0
        return ExprState(
            HTanh(
                ATScale.from_scales(var.expr, var.scale, new_scale, 28 -
                                    var.length, to_q=new_q, from_q=var.q),
                new_q,
                new_scale
            ),
            abs(closest_repr) + 1,
            q=new_q,
            scale=new_scale
        )
    return ExprState(
        HTanh(var.expr, None, None),
        var.ibits
    )


def generate_sigmoid(var, scaling):
    if scaling:
        # sigmoid represents 1/6 in Q7 always
        var = check_overflow(var, 7)
        return ExprState(
            HSigmoid(var.expr, var.q, var.scale),
            var.ibits + 7,
            q=var.q,
            scale=var.scale * pow(2, -7)
        )
    return ExprState(
        HSigmoid(var.expr, None, None),
        var.ibits + 7
    )


def generate_pow(var1, var2, scaling):
    if scaling:
        shift1 = 15 - var1.length
        cor_scale1 = var1.scale * 2**(15 - shift1)
        subexpr1 = ATLShift(var1.expr, shift1) if shift1 != 0 else var1.expr
        if not var2.expr.is_number:
            raise NotImplementedError(
                "power is currently only supported with positive fractional constants, 0, 1, -2 or 2")
        if var1.is_number:
            return ExprState(var1.expr**var2.expr, 7)
        if var2.expr == 2:
            return generate_mul(var1, var1, scaling)
        if var2.expr == -2:
            return generate_sqrt(var1, scaling)
        if var2.expr == 1:
            return var1
        if var2.expr == 0:
            return ExprState(1, 1, q=0, scale=1)
        if var2.expr > 1 or var2.expr < 0:
            raise NotImplementedError(
                "power is currently only supported with positive fractional constants, 0, 1, -2 or 2")
        arg2 = int(math.floor(var2.expr * pow(2, 15) + 0.5))
        out_scale = pow(cor_scale1, var2.expr) * pow(2, -15)
        return ExprState(Pow(subexpr1, arg2), 15, q=0, scale=out_scale)
    return ExprState(Pow(var1.expr, var2.expr), 15)


def compose_expression(G, node_symbol_map, node, scaling=False, first=True) -> ExprState:
    """ Walks down an expression in the graph and returns a Sympy equivalent. If
    scaling is True then the quantization and necessary scaling is dynamically created.
    node_symbol_map contains a map of node to ExprState instances containing symbols and scaling."""
    if not first and node in node_symbol_map:
        var = deepcopy(node_symbol_map[node])
        LOG.info("using cached %s %s.%s %s", node.name, var.ibits, var.q, var.scale)
        return var

    if isinstance(node, ActivationParameters):
        var = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=0),
            scaling=scaling, first=False)

        LOG.debug("Node: %s \nInner: %s", node.name, var)
        if node.activation in ["tanh", "htanh"]:
            return generate_tanh(var, scaling)
        if node.activation in ["sigmoid", "hsigmoid"]:
            return generate_sigmoid(var, scaling)
        raise NotImplementedError()

    if isinstance(node, (MatrixAddParameters, MatrixSubParameters)):
        var1 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=0),
            scaling=scaling, first=False)
        var2 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=1),
            scaling=scaling, first=False)
        LOG.debug("Node: %s \nInner1: %s \nInner2: %s", node.name, var1, var2)
        if isinstance(node, MatrixSubParameters):
            var2.expr = Mul(var2.expr, -1)
        if scaling:
            if var1.is_not_scaled and var2.is_not_scaled:
                return ExprState(Add(var1.expr, var2.expr), 8)
            var1, var2 = equalize_scales(var1, var2, threshold=30)
            assert var1.q == var2.q, "safety check"
        ibits = max(var1.ibits, var2.ibits)
        if isinstance(node, MatrixSubParameters):
            return ExprState(factor(Add(var1.expr, Mul(var2.expr, -1 * pow(2, var2.q)))), ibits, q=var1.q, scale=var1.scale)
        # addition potentially adds a bit
        ibits += 1
        return ExprState(factor(Add(var1.expr, var2.expr)), ibits, q=var1.q, scale=var1.scale)

    if isinstance(node, MatrixMulParameters):
        var1 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=0),
            scaling=scaling, first=False)
        var2 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=1),
            scaling=scaling, first=False)
        LOG.debug("Node: %s \nInner1: %s \nInner2: %s", node.name, var1, var2)
        return generate_mul(var1, var2, scaling)

    if isinstance(node, MatrixDivParameters):
        var1 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=0),
            scaling=scaling, first=False)
        var2 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=1),
            scaling=scaling, first=False)
        LOG.debug("Node: %s \nInner1: %s \nInner2: %s", node.name, var1, var2)
        return generate_div(var1, var2, scaling)

    if isinstance(node, BinaryOpParameters):
        op = BINARY_OP_MAPPING.get(node.op_type)
        if op is None:
            raise NotImplementedError("%s is not implemented in expression fusion" % node.op_type)
        var1 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=0),
            scaling=scaling, first=False)
        var2 = compose_expression(
            G, node_symbol_map, from_node(G, node, idx=1),
            scaling=scaling, first=False)
        LOG.debug("Node: %s \nInner1: %s \nInner2: %s", node.name, var1, var2)
        if node.op_type == "maximum" or node.op_type == "minimum":
            if scaling:
                if var1.is_not_scaled and var2.is_not_scaled:
                    return ExprState(op(var1.expr, var2.expr), max(var1.ibits, var2.ibits))
                var1, var2 = equalize_scales(var1, var2)
            return ExprState(op(var1.expr, var2.expr), max(var1.ibits, var2.ibits), q=var1.q, scale=var1.scale)
        if node.op_type == "pow":
            return generate_pow(var1, var2, scaling)

    if isinstance(node, UnaryOpParameters):
        var = compose_expression(
            G, node_symbol_map, from_node(G, node),
            scaling=scaling, first=False)
        LOG.debug("Node: %s \nInner: %s", node.name, var)
        if node.op_type == "sqrt":
            return generate_sqrt(var, scaling)
        if node.op_type == "log":
            return generate_log(var, scaling)

        raise NotImplementedError("%s is not implemented in expression fusion" % node.op_type)

    if isinstance(node, FusionInputParameters):
        LOG.debug("Node: %s", node.name)
        return node_symbol_map[node]

    if isinstance(node, FusionOutputParameters):
        return compose_expression(G, node_symbol_map, from_node(G, node),
                                  scaling=scaling, first=False)

    if isinstance(node, ConstantInputParameters):
        # only scalars are absorbed into expression
        # cope with arrays, numpy arrays and scalars
        # whether scaling or not always return the real number
        # The scaling will occur later when the constant is used
        value = np.array(node.value).flatten()[0]
        LOG.debug("Node: %s Value: %s", node.name, value)
        if value == math.floor(value):
            return ExprState(Integer(value), 7)
        return ExprState(Float(value), 7)
    raise NotImplementedError("don't know how to handle node %s" % node.name)
