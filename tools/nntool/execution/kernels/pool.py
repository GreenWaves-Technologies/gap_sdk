# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging

import numpy as np

from .utils import pad, srange

LOG = logging.getLogger("nntool." + __name__)

# pylint: disable=too-many-arguments, too-many-locals
def av_pool(params, in_dims, out_dims, in_tensor, qrec=None):
    # Prepare the quantization levels

    filter_sz = params.filter.h * params.filter.w
    if qrec:
        pool_factor = (1<<16)//filter_sz
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
