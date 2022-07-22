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

from functools import reduce

import numpy as np


class ProvisionalDim():
    def __init__(self, shape) -> None:
        self.shape = list(shape)

    @classmethod
    def from_onnx_shape(cls, onnx_shape, check_for_batch=None, substitutions=None):
        if substitutions is None:
            substitutions = {}
        if not onnx_shape.dim:
            shape = (1,)
        else:
            shape = [d.dim_value if (d.dim_value > 0 and d.dim_param == "") else
                    (substitutions[d.dim_param] if d.dim_param in substitutions else None)
                    for d in onnx_shape.dim]
        if check_for_batch is not None and shape[check_for_batch] == 1 and len(shape) > 1:
            shape[0] = None
        return cls(shape)

    @classmethod
    def from_tflite_shape(cls, tflite_shape, check_for_batch=None):
        tflite_shape = list(tflite_shape)
        if check_for_batch is not None and tflite_shape[check_for_batch] == 1 and len(tflite_shape) > 1:
            tflite_shape[0] = None
        return cls(tflite_shape)

    @property
    def known_shape(self):
        return [elem for elem in self.shape if elem is not None]

    @property
    def flatten(self):
        return reduce(
            lambda x, y: x +
            [y] if x is None or not y else y[:-1:] + [y[-1] * x],
            self.shape,
            [])

    def eliminate_dimension(self, dim_idx):
        self.shape[dim_idx] = None
        return self

    def infer_mapping(self, shape, allow_bad_length=False):
        assert allow_bad_length or len(shape) == len(
            self.known_shape), "shape cannot be inferred"
        new_shape = []
        idx = 0
        for elem in self.shape:
            if elem is None:
                new_shape.append(None)
            else:
                if idx >= len(shape):
                    break
                new_shape.append(shape[idx])
                idx += 1
        return self.__class__(new_shape)
