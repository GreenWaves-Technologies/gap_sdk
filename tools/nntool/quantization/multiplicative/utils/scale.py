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

from utils.at_norm import at_norm

def compute_scales(scale, available_bits=None, dtype=np.uint8):
    if not isinstance(scale, np.ndarray):
        scale = np.array([scale], dtype=np.float32)
    iinfo = np.iinfo(dtype)
    signed = iinfo.min < 0
    max_bits = iinfo.bits - (1 if signed else 0)
    if available_bits is not None:
        max_bits = min(max_bits, available_bits)

    bits = np.full(scale.shape, max_bits)
    factors = np.array([math.frexp(scale) for scale in scale],
                       dtype=[("scale", "f4"), ("norm", "i1")])

    max_val = np.power(2, max_bits)
    while True:
        qbiases = np.floor(factors['scale'] * np.power(2, bits) + 0.5)
        qnorms = -(factors["norm"] - bits)
        # overflow in conversion
        max_exceeded = qbiases >= max_val
        # shifting away bits
        norms_too_high = qnorms > 32 - 8
        # mult * pow 2 then shift
        bias_pow2 = qbiases % 2 == 0
        should_move = np.logical_or(max_exceeded, np.logical_or(norms_too_high, bias_pow2))
        can_still_move = np.logical_and(qnorms > 0, bits > 0)
        overflow = np.logical_and(should_move, can_still_move)
        if not np.any(overflow):
            break
        bits[overflow] -= 1

    return qbiases.astype(dtype), qnorms.astype(np.int8)


def apply_scales(qbiases, qnorms, arr: np.ndarray, axis: int = None, calc_dtype=np.int32):
    if axis is None:
        mul_biases = qbiases
        mul_biases_norm = qnorms
        assert len(mul_biases) == 1 and len(
            mul_biases_norm) == 1, "no axis set. should have single scale"
    else:
        shape = [len(qbiases) if idx == axis else 1 for idx in range(len(arr.shape))]
        mul_biases = qbiases.reshape(shape)
        mul_biases_norm = qnorms.reshape(shape)
    return at_norm(np.multiply(arr.astype(calc_dtype), mul_biases, dtype=calc_dtype), mul_biases_norm).astype(np.int32)

def get_value_scale(min_val_or_val, max_val=None, dtype=np.int8, narrow_range=False):
    iinfo = np.iinfo(dtype)
    if max_val is None:
        if isinstance(min_val_or_val, np.ndarray):
            min_val_or_val = np.max(np.abs(min_val_or_val))
        # avoid silly scale
        if min_val_or_val == 0:
            min_val_or_val = 1
        # if single positive value force it to be properly represented
        if min_val_or_val > 0:
            narrow_range = True
    if narrow_range:
        container_range = iinfo.max - (iinfo.min + 1)
    else:
        container_range = iinfo.max - iinfo.min
    if max_val is None:
        value_range = np.abs(min_val_or_val) * 2
    else:
        value_range = max(np.max(np.abs(min_val_or_val)), np.max(np.abs(max_val))) * 2
    return value_range / container_range

def get_scaled_value(scale, val, dtype=np.int8, q=7, scalar=False):
    if isinstance(val, (float, int)):
        val = np.array([val], dtype=np.float32)
    iinfo = np.iinfo(dtype)
    val = np.round(pow(2, q) * val/scale)
    val = np.minimum(iinfo.max, np.maximum(iinfo.min, val)).astype(dtype)
    if scalar:
        val = val.flatten()[0]
    return val
