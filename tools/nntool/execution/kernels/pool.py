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

import logging

import numpy as np

from .utils import pad, srange

LOG = logging.getLogger("nntool." + __name__)

# pylint: disable=too-many-arguments, too-many-locals


def av_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    # Prepare the quantization levels

    filter_sz = params.filter.h * params.filter.w
    if qrec:
        pool_factor = (1 << 16)//filter_sz
        dtype = np.int32
    else:
        pool_factor = 1.0/filter_sz
        dtype = None

    out_tensor = np.zeros(out_dims.shape, dtype=dtype)
    in_tensor = pad(in_tensor, in_dims, params.padding, params.pad_type)

    for in_c in range(out_dims.c):

        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter.h + params.padding.h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter.w + params.padding.w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                out_slice_args = srange(out_dims, c=in_c, h=out_h, w=out_w)
                in_slice_args =\
                    srange(in_dims,
                           c=[in_c, in_c + 1, 1],
                           h=[h_idx, h_idx + params.filter.h, 1],
                           w=[w_idx, w_idx + params.filter.w, 1])

                sum_ = np.sum(in_tensor[in_slice_args], dtype=dtype)
                prod_ = np.multiply(sum_, pool_factor, dtype=dtype)
                out_tensor[out_slice_args] = prod_
                out_w += 1
            out_h += 1

    if qrec:
        return qrec.out_qs[0].clip(out_tensor >> 16)

    return out_tensor


def max_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    dtype = qrec.out_qs[0].dtype if qrec else None
    out_tensor = np.zeros(out_dims.shape, dtype=dtype)

    if params.padding.h + params.padding.w > 0:
        in_tensor = pad(in_tensor, in_dims, params.padding, params.pad_type)

    for in_c in range(out_dims.c):
        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter.h + params.padding.h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter.w + params.padding.w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                out_slice_args = srange(out_dims, c=in_c, h=out_h, w=out_w)
                in_slice_args =\
                    srange(in_dims,
                           c=[in_c, in_c + 1, 1],
                           h=[h_idx, h_idx + params.filter.h, 1],
                           w=[w_idx, w_idx + params.filter.w, 1])

                out_tensor[out_slice_args] = np.max(in_tensor[in_slice_args].view(np.ndarray))
                out_w += 1
            out_h += 1

    return out_tensor


def gap_clb(sum_):
    '''Count Leading 0s or 1s'''
    sum_bin = [np.binary_repr(sum_elem, width=32) for sum_elem in sum_]
    return [len(s) - len(s.lstrip(s[0])) - 1 for s in sum_bin]


def av_global_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    # Prepare the quantization levels
    if qrec:
        dtype = np.int32
    else:
        dtype = None

    sum_by_chan = np.sum(in_tensor, dtype=dtype, axis=(
        in_dims.get_order_idx('w'), in_dims.get_order_idx('h')))

    if qrec:
        norm = (np.array([31], dtype=np.int32) - gap_clb(sum_by_chan)).astype(np.int32)
        inv_wh = (1 << norm) // (in_dims.h * in_dims.w)
        out_tensor = (inv_wh * sum_by_chan) >> norm
        return qrec.out_qs[0].clip(out_tensor).reshape(out_dims.shape)

    return (sum_by_chan / (in_dims.h * in_dims.w)).reshape(out_dims.shape)


def max_global_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    # Prepare the quantization levels
    if qrec:
        dtype = out_dims.dtype
    else:
        dtype = None

    return np.max(in_tensor,
                  dtype=dtype,
                  axis=(in_dims.get_order_idx('w'),
                        in_dims.get_order_idx('h')),
                  keepdims=True)
