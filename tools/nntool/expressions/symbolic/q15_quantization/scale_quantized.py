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

import math

import numpy as np
from numpy.core.getlimits import iinfo
from expressions.symbolic.function import Function

from ..basic import Add, Cast, CompoundFunction, LShift, Mul, Sub, copy_props
from ..symbol import c_headers, nargs
from .clip_norm import Norm
from .q15_scale_q_rec import Q15ScaleQRec
from .quantized_constant import QuantizedConstant


@nargs(2)
@c_headers('"Gap.h"')
@copy_props('norm')
class MulRN(Function):
    def __init__(self, *args, norm=0, **kwargs):
        self._norm = norm
        super().__init__(*args, **kwargs)

    @property
    def norm(self):
        return self._norm

    def _impl(self, *args, **kwargs):
        factor = int(math.pow(2, self._norm - 1))
        si_args = [arg.astype(np.int16) for arg in args]
        return (np.multiply(*si_args, dtype=np.int32) + factor) >> self._norm

    def _py_expr(self, *args, **kwargs):
        factor = int(math.pow(2, self._norm - 1))
        return f'(np.multiply(({args[0]}).astype(np.int16), ({args[1]}).astype(np.int16), dtype=np.int32) + {factor})>>{self._norm}'

    def _c_expr(self, *args, **kwargs):
        return f"gap8_muluRN(({args[0]}),({args[1]}),{self._norm})"

@nargs(1)
@c_headers('"Gap.h"')
@copy_props('from_qrec', 'to_qrec', 'num_bits')
class ScaleQuantized(CompoundFunction):
    def __init__(self, *args, from_qrec=None, to_qrec=None, num_bits=8, **kwargs):
        self._from_qrec = from_qrec
        self._to_qrec = to_qrec
        self._qbias, self._qnorm = None, None
        self._num_bits = num_bits
        super().__init__(*args, qrec=self._to_qrec, **kwargs)

    @property
    def from_qrec(self):
        return self._from_qrec

    @property
    def to_qrec(self):
        return self._to_qrec

    @property
    def num_bits(self):
        return self._num_bits

    def _calc_bias(self):
        if self._qbias is None or self._qnorm is None:
            self._qbias, self._qnorm = Q15ScaleQRec.quantized_scale(
                self._from_qrec,
                self._to_qrec,
                self._num_bits)
        return self._qbias, self._qnorm

    def _eval(self, *args, **kwargs):
        sym = args[0]
        # if the argument is another scalequantized do the scaling in one step
        # this should be safe as we never go much above Q15 and the scaling step
        # is also a Q15
        if isinstance(sym, ScaleQuantized):
            return ScaleQuantized(*sym.contents, from_qrec=sym.from_qrec, to_qrec=self.to_qrec, num_bits=min(self._num_bits, sym.num_bits))
        # Check if we do nothing
        if self._from_qrec == self._to_qrec:
            return sym

        if sym.is_zero(self._from_qrec):
            return QuantizedConstant(self._to_qrec.zero_point, dtype=self._to_qrec.dtype)

        qbias, qnorm = self._calc_bias()
        # make sure we are in int32 before doing these operations
        if self._from_qrec.dtype != np.int32:
            sym = Cast(sym, dtype=np.int32)

        if self._from_qrec.zero_point != 0:
            sym = Sub(sym, QuantizedConstant(self._from_qrec.zero_point, dtype=np.int32))

        if qbias == 1:
            # its a left shift
            if qnorm < 0:
                sym = LShift(
                    sym,
                    #pylint: disable=invalid-unary-operand-type
                    QuantizedConstant(-qnorm, dtype=np.int8),
                    name=self.name,
                    dtype=self._to_qrec.dtype
                )
            elif qnorm > 0:
                sym = Norm(
                    sym,
                    QuantizedConstant(qnorm, dtype=np.int8),
                    name=self.name,
                    dtype=self._to_qrec.dtype
                )
            # if 0 do nothing
        elif qnorm < 0:
            sym = LShift(
                Mul(
                    sym,
                    QuantizedConstant(qbias, dtype=np.int32),
                    dtype=self._to_qrec.dtype
                ),
                #pylint: disable=invalid-unary-operand-type
                QuantizedConstant(-qnorm, dtype=np.int8),
                name=self.name,
                dtype=self._to_qrec.dtype
            )
        elif qnorm > 0:
            sym = Norm(
                Mul(
                    sym,
                    QuantizedConstant(qbias, dtype=np.int32),
                    dtype=self._to_qrec.dtype
                ),
                QuantizedConstant(qnorm, dtype=np.int8),
                name=self.name,
                dtype=self._to_qrec.dtype
            )
        else:
            sym = Mul(
                sym,
                QuantizedConstant(qbias, dtype=np.int32),
                name=self.name,
                dtype=self._to_qrec.dtype
            )

        if self._to_qrec.zero_point != 0:
            sym = Add(sym, QuantizedConstant(self._to_qrec.zero_point, dtype=np.int32))

        # TODO - Check if we need to clip here!!

        if self._to_qrec.dtype != np.int32:
            sym = Cast(sym, dtype=self._to_qrec.dtype)
        return sym

    def __repr__(self) -> str:
        return f"ScaleQuantized({self.contents[0]}, [{self._from_qrec}]->[{self._to_qrec}])"
