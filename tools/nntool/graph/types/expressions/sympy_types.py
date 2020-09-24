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


import numpy as np
from sympy import Function, Max, Min, Mul, Pow, Rational, S, floor

from utils.at_norm import at_norm

from .utils import compute_scales, denumpy, get_scale_value


class SimpleFunction(Function):
    pass


class HTanh(SimpleFunction):
    nargs = 3

    @classmethod
    def eval(cls, *args):
        (expr, q, scale) = tuple(args[:3:])
        if scale:
            return Min(
                get_scale_value(scale, 1, dtype=np.int32, scalar=True, q=q),
                Max(
                    get_scale_value(scale, -1, dtype=np.int32, scalar=True, q=q),
                    expr
                )
            )
        return Min(1, Max(-1, expr))


class HSigmoid(SimpleFunction):
    nargs = 3

    @classmethod
    def eval(cls, *args):
        (expr, q, scale) = tuple(args[:3:])
        if scale:
            return Mul(
                Min
                (
                    get_scale_value(scale, 6, dtype=np.int32, scalar=True, q=q),
                    Max
                    (
                        0,
                        expr + get_scale_value(scale, 3, dtype=np.int32, scalar=True, q=q)
                    )
                ),
                get_scale_value(1, 1/6, dtype=np.int32, scalar=True, q=7)  # 1/6 in Q7
            )
        return Mul(Min(6, Max(0, expr + 3)), Rational(1, 6))


class ATLShift(SimpleFunction):
    nargs = 2

    @classmethod
    def eval(cls, *args):
        (val, shift) = tuple(args[:2:])
        (shift,) = denumpy(shift)
        if shift.is_number:
            if shift.is_number and shift.is_zero:
                return val
            if shift < 0:
                return ATNorm(val, -shift)
            if val.func == ATNorm:
                if shift == val.args[1]:
                    return val.args[0]
                elif shift < val.args[1]:
                    return val.func(val.args[0], val.args[1] - shift)
                else:
                    return ATLShift(val.args[0], shift - val.args[1])
            if val.func == ATLShift:
                shift += val.args[1]
                val = val.args[0]
                return ATLShift(val, shift)

        if val.is_number:
            if val.is_zero:
                return S.Zero
            if shift.is_number:
                return val * Pow(2, shift)

    @staticmethod
    def _imp_(arr: np.ndarray, shift):
        return arr.astype(np.int32) << shift


class ATNorm(SimpleFunction):
    nargs = 2

    @property
    def is_real(self):
        return True

    @property
    def is_complex(self):
        return False

    @classmethod
    def eval(cls, *args):
        (val, norm) = tuple(args[:2:])
        (norm,) = denumpy(norm)
        if norm.is_number:
            if norm.is_zero:
                return val
            if norm < 0:
                return ATLShift(val, -norm)
            if val.func == ATLShift:
                if norm == val.args[1]:
                    return val.args[0]
                elif norm < val.args[1]:
                    return val.func(val.args[0], val.args[1] - norm)
                else:
                    new_norm = norm - val.args[1]
                    return ATNorm(val.args[0], new_norm)
            if val.func == ATNorm:
                norm += val.args[1]
                val = val.args[0]
                return ATNorm(val, norm)

        if val.is_number:
            if val.is_zero:
                return S.Zero
            if norm.is_number:
                return floor((val + (Pow(2, norm - 1))) * Pow(2, - norm))

    @staticmethod
    def _imp_(arr: np.ndarray, norm):
        return at_norm(arr.astype(np.int32), norm)


class ATClip(SimpleFunction):
    nargs = 2

    @classmethod
    def eval(cls, *args):
        pass

    @staticmethod
    def _imp_(arr: np.ndarray, num_bits):
        limit = pow(2, num_bits)
        return np.clip(arr, -limit, limit - 1)


class ATScale(SimpleFunction):
    nargs = 4

    @classmethod
    def from_scales(cls, val, old_scale, new_scale, available_bits, to_q=0, from_q=0):
        return cls.from_rescale(val, old_scale / new_scale, available_bits, to_q=to_q, from_q=from_q)

    @classmethod
    def from_rescale(cls, val, scale, available_bits, to_q=0, from_q=0):
        qbias, qnorm = compute_scales(scale, available_bits)
        qnorm += from_q - to_q
        return cls(val, qbias, qnorm, available_bits)

    @classmethod
    def eval(cls, *args):
        (val, scale, scalen) = tuple(args[:3:])
        # suck scales/norms together
        if val.func == ATScale:
            new_rescale = scale * pow(2, -scalen) * val.args[1] * pow(2, -val.args[2])
            scale, scalen = compute_scales(new_rescale, val.args[3])
            val = val.args[0]
        if val.func == ATNorm:
            scalen += val.args[1]
            val = val.args[0]
        # sub out other patterns
        if val.is_Number:
            # zero is always 0
            if val.is_zero:
                return S.Zero
        if scale.is_number and scalen.is_number:
            # scale 1 is either nothing or just a shift
            if scale is S.One:
                if scalen.is_zero:
                    return val
                return ATNorm(val, scalen)

        return ATNorm((val * scale), scalen)

    @staticmethod
    def _imp_(arr: np.ndarray, scale, scalen):
        return at_norm(arr.astype(np.int32) * scale, scalen)
