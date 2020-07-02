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
from quantization.multiplicative.symmetric.symmetric_mult_qtype import SymmetricMultQType

# Has internal scale and external scale
# Internal scale is the set scale at creation. This is assumed to be the scale of any contained parameters
# The set in_q and weights_q is the scale that will be used when quantized bias is requested

class SymmetricMultBiasesQType(SymmetricMultQType):
    def __init__(self, *args, init=None, **kwargs):
        super(SymmetricMultBiasesQType, self).__init__(*args, init=init, **kwargs)

    def link(self, weights_q, in_q):
        self.set_scale = weights_q.scale * in_q.scale

    @property
    def scale(self):
        return self.set_scale if self.set_scale is not None else super().scale

    @scale.setter
    def scale(self, val):
        self._info['scale'] = val

    @property
    def set_scale(self):
        return self._info.get('set_scale')

    @set_scale.setter
    def set_scale(self, val):
        self._info['set_scale'] = val

    def dequantize(self, arr):
        return self.dequantize_at_scale(arr, super().scale)

    def get_dequantized(self, arr, container_is_quantized=True):
        if container_is_quantized:
            return self.dequantize_at_scale(arr, super().scale)
        return arr

    def quantize(self, arr: np.array) -> np.array:
        return self.quantize_at_scale(arr, super().scale)

    def get_quantized(self, arr: np.array, container_is_quantized=True) -> np.array:
        if container_is_quantized:
            if self.set_scale is not None and not np.array_equal(self.set_scale, super().scale):
                return self.quantize_at_scale(self.dequantize_at_scale(arr, super().scale), self.set_scale)
            return arr
        else:
            if self.set_scale is not None and not np.array_equal(self.set_scale, super().scale):
                return self.quantize_at_scale(arr, self.set_scale)
            return self.quantize_at_scale(arr, super().scale)

    def _encapsulate(self):
        return {
            "min_val": self.min,
            "max_val": self.max,
            "scale": super().scale,
            "set_scale": self.set_scale,
            "dim": self.quantized_dimension,
            "narrow_range": self.narrow_range,
            "dtype": self.dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init={
            "min_val": val['min_val'],
            "max_val": val['max_val'],
            "scale": val['scale'],
            "set_scale": val['set_scale'] if 'set_scale' in val else None,
            "quantized_dimension": val['dim'] if 'dim' in val else None,
            "narrow_range": val['narrow_range'],
            "dtype": getattr(np, val['dtype'])
        })
