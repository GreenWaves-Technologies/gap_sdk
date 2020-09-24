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
from sympy import Integer, Float
from utils.stats_funcs import calc_bits

def denumpy(*args):
    return tuple(int(arg.flatten()[0]) if isinstance(arg, np.ndarray) else arg for arg in args)


def denumpyf(*args):
    return tuple(arg.flatten()[0] if isinstance(arg, np.ndarray) else arg for arg in args)


def get_scale_value(scale, val, dtype=np.int8, q=7, scalar=False):
    if isinstance(val, Integer):
        val = int(val)
    elif isinstance(val, Float):
        val = float(val)
    if isinstance(scale, (Integer, Float)):
        scale = float(scale)
    if isinstance(q, (Integer, Float)):
        q = int(q)
    if isinstance(val, (float, int)):
        val = np.array([val], dtype=np.float32)
    iinfo = np.iinfo(dtype)
    qval = np.round(pow(2, q) * val/scale)
    qval = np.minimum(iinfo.max, np.maximum(iinfo.min, qval)).astype(dtype)
    if scalar:
        qval = qval.flatten()[0]
    return qval

def compute_scales(scale, available_bits):
    available_bits = min(available_bits, 8)
    scale = float(scale)
    bias, norm = math.frexp(scale)
    if 1 % bias == 0 and 2 % (1//bias) == 0:
        norm -= int((1//bias)//2)
        bias = 1
        return bias, -norm
    bias = int(round(bias * pow(2, available_bits)))
    norm -= available_bits
    return bias, -norm
