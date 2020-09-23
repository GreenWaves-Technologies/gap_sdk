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

import importlib
import math

from sympy import Integer, srepr

from utils.json_serializable import JsonSerializable


def serialize_sympy(obj):
    if obj is None or isinstance(obj, (int, float, bool, str)):
        return obj
    cls_, args, state = obj.__reduce_ex__(1)
    return {
        'name': cls_.__name__,
        'module': cls_.__module__,
        'args': [serialize_sympy(sobj) for sobj in args],
        'state': state
    }

def deserialize_sympy(sobj):
    if sobj is None or isinstance(sobj, (int, float, bool, str)):
        return sobj
    module = importlib.import_module(sobj['module'])
    cls = getattr(module, sobj['name'])
    instance = cls(*[deserialize_sympy(ssobj) for ssobj in sobj['args']])
    instance.__setstate__(sobj['state'])
    return instance

class ExprState(JsonSerializable):
    def __init__(self, expr, ibits, q=None, scale=None):
        self.expr = expr
        self.ibits = ibits
        self.q = q
        self.scale = scale

    def _encapsulate(self):
        return {
            'expr': serialize_sympy(self.expr),
            'ibits': self.ibits,
            'q': self.q,
            'scale': self.scale
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(deserialize_sympy(val['expr']), val['ibits'], q=val['q'], scale=val['scale'])

    def quant_from(self, other: 'ExprState'):
        self.ibits = other.ibits
        self.q = other.q
        self.scale = other.scale
        return self

    def scale_from(self, other: 'ExprState'):
        self.scale = other.scale
        return self

    def equalize_to(self, other: 'ExprState'):
        scale = self.scale/other.scale
        if scale < 1:
            neg = -1
            scale = 1/scale
        else:
            neg = 1
        extra_bits = math.ceil(math.log(scale) / math.log(2))
        extra_bits *= neg

        self.scale = other.scale
        self.ibits += extra_bits
        self.q = other.q
        return self

    def q_from(self, other: 'ExprState'):
        self.q = other.q
        return self

    def set_to_constant(self, val):
        self.q = 0
        self.ibits = 1
        self.scale = float(val)
        self.expr = Integer(1)
        return self

    @property
    def is_not_scaled(self):
        if self.scale is None:
            assert self.is_number
            return True
        return False

    @property
    def is_number(self):
        return self.expr.is_number

    @property
    def length(self):
        if self.q is None:
            assert self.scale is None
            return self.ibits
        return self.q + self.ibits

    @length.setter
    def length(self, val):
        if val > self.length:
            self.q += val - self.length
        else:
            # TODO - ajust scale when q goes negative ?
            diff = self.length - val
            assert diff > 0
            if diff > self.q:
                left = diff - self.q
                self.q = self.q - diff
                self.ibits -= left
            else:
                self.q -= diff

    def __str__(self):
        return "%s.%s %s %s"%(self.ibits, self.q, self.scale, srepr(self.expr))
