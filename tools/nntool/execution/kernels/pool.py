# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

import numpy as np

from quantization.qtype import QType

from .utils import srange, pad

LOG = logging.getLogger("nntool." + __name__)

# pylint: disable=too-many-arguments, too-many-locals
def av_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    # Prepare the quantization levels

    filter_sz = params.filter.h * params.filter.w
    if qrec:
        # When quantified the accumulation is done in double precision and the
        # division is in a Q equal to the bit width of the input
        # In the AT kernels it is unsigned but we don't care obout that here
        in_q = qrec.in_qs[0]
        pool_factor = (1<<in_q.bits)//filter_sz
        # Construct a QType representing the quantization of the accumulator
        # after the division
        calc_q = QType(bits=in_q.bits * 2, q=in_q.bits + in_q.q, signed=False)
        dtype = calc_q.dtype
    else:
        pool_factor = 1.0/filter_sz
        dtype = None

    out_tensor = np.zeros(out_dims.shape, dtype=dtype)
    in_tensor = pad(in_tensor, in_dims, params.padding, params.pad_type)

    if qrec:
        in_tensor = in_tensor.astype(calc_q.dtype)

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

                sum_ = np.sum(in_tensor[in_slice_args])
                prod_ = np.multiply(sum_, pool_factor)
                out_tensor[out_slice_args] = prod_
                out_w += 1
            out_h += 1


    if qrec:
        return qrec.out_qs[0].reduce_from(out_tensor, calc_q)

    return out_tensor

def max_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    dtype = qrec.out_qs[0].dtype if qrec else None
    out_tensor = np.zeros(out_dims.shape, dtype=dtype)

    if params.padding.size() > 0:
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
