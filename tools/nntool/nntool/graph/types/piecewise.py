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

from nntool.expressions.symbolic.basic import (Abs, Ceil, Cos, Cosh, Erf, Exp, Log, Max, Min,
                                        Neg, Pow, Round, RSqrt, Sin, Sqrt)
from nntool.graph.types import InsensitiveToQuantizationMixin, NNNodeBase
from nntool.graph.types.base import (CanFuseToExpressionMixin, cls_op_name, expression_op,
                              nargs)


# @nargs({2})
# class BinaryOpNodeBase(CanFuseToExpressionMixin, NNNodeBase):

#     def __new__(cls, *args, op_type="maximum", **kwargs):
#         if cls is BinaryOpNodeBase:
#             for subcls in BinaryOpNodeBase.__subclasses__():
#                 if op_type == subcls.CLS_OP_NAME:
#                     return super(BinaryOpNodeBase, cls).__new__(subcls)
#             raise ValueError(f'binary op {op_type} not found')
#         return super(BinaryOpNodeBase, cls).__new__(cls, **kwargs)

#     def __init__(self, *args, op_type="maximum", **kwargs):
#         super(BinaryOpNodeBase, self).__init__(*args, **kwargs)
#         self._op_type = op_type

#     @property
#     def op_type(self):
#         return self._op_type

#     def get_output_size(self, in_dims):
#         assert len(in_dims) == 2
#         out_dim = in_dims[0].clone()
#         return [out_dim]

#     def get_parameter_size(self):
#         return 0

#     @property
#     def can_equalize(self):
#         return False

#     def __str__(self):
#         return "{} {}".format(
#             self._op_type,
#             self.at_options
#         )


# @cls_op_name('maximum')
# @expression_op(Max)
# class MaxOpNode(BinaryOpNodeBase, InsensitiveToQuantizationMixin):
#     pass


# @cls_op_name('minimum')
# @expression_op(Min)
# class MinOpNode(BinaryOpNodeBase, InsensitiveToQuantizationMixin):
#     pass


# @cls_op_name('pow')
# @expression_op(Pow)
# class PowOpNode(BinaryOpNodeBase):
#     pass


class UnaryOpNodeBase(CanFuseToExpressionMixin, NNNodeBase):

    def __new__(cls, *args, op_type="sqrt", **kwargs):
        if cls == UnaryOpNodeBase:
            for subcls in UnaryOpNodeBase.__subclasses__():
                if op_type == subcls.CLS_OP_NAME:
                    return super(UnaryOpNodeBase, cls).__new__(subcls)
            raise ValueError(f'unary op {op_type} not found')
        return super(UnaryOpNodeBase, cls).__new__(cls)

    def __init__(self, *args, op_type=None, **kwargs):
        super(UnaryOpNodeBase, self).__init__(*args, **kwargs)
        self._op_type = op_type

    @property
    def op_type(self):
        return self._op_type

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        out_dim = in_dims[0].clone()
        return [out_dim]

    def get_parameter_size(self):
        return 0

    @property
    def can_equalize(self):
        return False

    def __str__(self):
        return "{} {}".format(
            self._op_type,
            self.at_options
        )


@cls_op_name('round')
@expression_op(Round)
class RoundOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('ceil')
@expression_op(Ceil)
class CeilOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('sqrt')
@expression_op(Sqrt)
class SqrtOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('rsqrt')
@expression_op(RSqrt)
class RSqrtOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('exp')
@expression_op(Exp)
class ExpOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('log')
@expression_op(Log)
class LogOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('sin')
@expression_op(Sin)
class SinOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('cos')
@expression_op(Cos)
class CosOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('cosh')
@expression_op(Cosh)
class CoshOpNode(UnaryOpNodeBase):
    pass


@cls_op_name('abs')
@expression_op(Abs)
class AbsOpNode(UnaryOpNodeBase, InsensitiveToQuantizationMixin):
    pass


@cls_op_name('neg')
@expression_op(Neg)
class NegOpNode(UnaryOpNodeBase, InsensitiveToQuantizationMixin):
    pass


@cls_op_name('erf')
@expression_op(Erf)
class ErfOpNode(UnaryOpNodeBase, InsensitiveToQuantizationMixin):
    pass
