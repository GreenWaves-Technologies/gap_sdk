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


def spare_bits(params, dtype, bits):
    iinfo = np.iinfo(dtype)
    max_bits = math.ceil(math.log2(iinfo.max * params.filter.in_c * params.filter.h * params.filter.w))
    return bits - max_bits


def compute_mul_bias(scales):
    factors = np.array([math.frexp(scale) for scale in scales], dtype=[("scale", "f4"), ("norm", "i1")])
    qmults = np.floor(factors['scale'] * math.pow(2, 7) + 0.5)
    qnorms = -(factors["norm"] - 7)
    qnorms[qmults >= 128] += 1
    qmults[qmults >= 128] = 64
    return qmults, qnorms

def compute_32bit_mul_bias(scales):
    qmults = np.floor(scales * math.pow(2, 32) + 0.5)
    iinfo = np.iinfo(np.uint32)
    return np.minimum(qmults, iinfo.max).astype(np.uint32)
