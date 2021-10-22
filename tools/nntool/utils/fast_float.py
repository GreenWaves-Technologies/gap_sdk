# Copyright (C) 2021  GreenWaves Technologies, SAS

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
from bfloat16 import bfloat16

FASTRECIPROCAL_PARAMS = {
    np.float32: (np.uint32, np.uint32(0x5f375a86)),
    np.float16: (np.uint16, np.uint16(0x59b8))
}


def np_fastrsqrt(x, dtype=np.float16):
    int_dtype, factor = FASTRECIPROCAL_PARAMS[dtype]
    x = np.atleast_1d(x).astype(dtype)
    xhalf = dtype(-0.5) * x
    i = x.view(int_dtype)
    i = factor - (i >> 1)
    x = i.view(dtype)
    x *= dtype(1.5) + (xhalf * x * x)
    return x


def np_fastreciprocal(x, dtype=np.float16):
    x = np_fastrsqrt(x, dtype=dtype)
    return x * x


def div(x, y, dtype, no_div=False):
    if no_div:
        return x * np_fastreciprocal(y, dtype=dtype)
    return x/y


FASTLOG_DTYPE_PARAMS = {
    np.float32: (23, 127, np.uint32, np.float32),
    np.float16: (10, 15, np.uint16, np.float16),
    bfloat16: (7, 127, np.uint16, np.float16),
}


def np_fastlog2(x, dtype=np.float32, no_div=False):
    L, B, int_dtype, int_fdtype = FASTLOG_DTYPE_PARAMS[dtype]
    x_f = np.atleast_1d(x).astype(dtype)
    vx = x_f.view(int_dtype)
    mx = ((vx & ((int_dtype(1) << int_dtype(L))-int_dtype(1)))
          | (int_dtype(B-1) << int_dtype(L)))
    mxf = mx.view(dtype).astype(int_fdtype)
    y = vx.astype(int_fdtype) / int_fdtype(1 << L)
    return (y + int_fdtype(2.77455363) - int_fdtype(B)
            - int_fdtype(1.498030302) * mxf
            - div(int_fdtype(1.72587999), (int_fdtype(0.3520887068) + mxf), dtype, no_div=no_div)).astype(dtype)


def np_fastlog(x, dtype=np.float32):
    return 0.69314718 * np_fastlog2(x, dtype)


POW_DTYPE_PARAMS = {
    np.float32: (23, 127, 32, np.float32, 126),
    np.float16: (10, 15, 16, np.float16, 15),
    bfloat16: (7, 127, 16, np.float16, 126),
}


def np_fastpow2(p, dtype=np.float32, out_dtype=None, no_div=False):
    L, B, int_len, int_fdtype, clip_val = POW_DTYPE_PARAMS[dtype]
    if out_dtype is None:
        out_dtype = dtype
    elif dtype != out_dtype:
        L, B, _, _, clip_val = POW_DTYPE_PARAMS[out_dtype]

    p_f = np.atleast_1d(p).astype(dtype)
    offset = np.where(p_f < 0, dtype(1.0), dtype(0.0))
    # clip needs to be worked out for each
    clipp = np.minimum(np.maximum(p_f, -dtype(clip_val-0.1)), dtype(clip_val))
    z = int_fdtype(clipp - clipp.astype(np.int16 if int_len ==
                                        16 else np.int32) + offset)  # x - floor(x)
    a = clipp + int_fdtype(B - 5.7259162)
    b = div(int_fdtype(27.7280233), (int_fdtype(
        4.84252568) - z), dtype, no_div=no_div)
    c = - int_fdtype(1.49012907) * z
    c = (int_fdtype(1 << L) * (a+b+c)
         ).astype(np.uint16 if int_len == 16 else np.uint32)
    return c.view(out_dtype)


def np_fastpow(x, p, dtype=np.float32):
    return np_fastpow2(p * np_fastlog2(x, dtype=dtype), dtype=dtype)


def np_fastexp(p, dtype=np.float32, out_dtype=None):
    p_f = np.atleast_1d(p).astype(dtype)
    return np_fastpow2(dtype(1.442695040) * p_f, dtype=dtype, out_dtype=out_dtype)


def np_fasttanh(p, dtype=np.float32, doalt=True, no_div=False):
    p_f = np.atleast_1d(p).astype(dtype)
    int_fdtype = np.float16 if dtype == bfloat16 and doalt else dtype
    return (int_fdtype(-1.0) + div(
        int_fdtype(2.0),
        (int_fdtype(1.0) + np_fastexp(
            dtype(-2.0) * p_f,
            dtype=dtype,
            out_dtype=int_fdtype)),
        int_fdtype,
        no_div=no_div)).astype(dtype)


def np_fastsigmoid(p, dtype=np.float32, doalt=True, no_div=False):
    p_f = np.atleast_1d(p).astype(dtype)
    int_fdtype = np.float16 if dtype == bfloat16 and doalt else dtype
    return div(int_fdtype(1.0), (int_fdtype(1.0) + np_fastexp(-p_f, dtype=dtype, out_dtype=int_fdtype)), int_fdtype, no_div=no_div).astype(dtype)
