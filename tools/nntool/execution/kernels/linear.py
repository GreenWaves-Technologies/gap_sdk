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

from .utils import prepare_acc, srange

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=too-many-arguments
def linear(params, in_dims, out_dims, in_tensor, weights, biases,
           qrec=None, details=None):

    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")
        details['overflow_dot'] = 0
        details['overflow_acc'] = 0

    acc_tensor = prepare_acc(biases, out_dims, qrec)

    if qrec and qrec.calc_q.dtype != qrec.in_qs[0].dtype:
        # force the bit dimension of the input tensor to the bit width of the calc
        # so that the dot product occurs in this precision
        in_tensor = in_tensor.astype(qrec.calc_q.dtype)

    in_tensor = in_tensor.reshape((in_dims.size()))
    filt = params.filter.get_filter_dims()
    for out_c in range(out_dims.c):
        if qrec and qrec.calc_q != qrec.acc_q:
            # Expand and normalize the accumulator
            acc_tensor = qrec.calc_q.expand_from(acc_tensor, qrec.acc_q)
        w_slice_args = srange(filt,
                              out_c=out_c)
        w_slice = weights[w_slice_args].reshape((in_dims.size()))

        out_slice_args = srange(out_dims, c=out_c, h=0, w=0)
        res = np.dot(in_tensor, w_slice)
        if qrec:
            res64 = np.dot(in_tensor.astype(np.int64), w_slice.astype(np.int64))
            if res64 != res:
                has_warned = True
                if details is not None:
                    details['overflow_dot'] += 1
                else:
                    LOG.warning(
                        "overflow in linear dot product OutC=%d",
                        out_c)
            else:
                has_warned = False
            acc64 = acc_tensor[out_slice_args].astype(np.int64)
        elif details is not None:
            prod = in_tensor * w_slice
            details['min_acc'] = min(np.sum(prod[prod < 0]), details['min_acc'])
            details['max_acc'] = min(np.sum(prod[prod > 0]), details['max_acc'])

        acc_slice = acc_tensor[out_slice_args]
        acc_slice += res
        if qrec:
            if acc64 + res64 != acc_slice:
                if details is not None:
                    details['overflow_acc'] += 1
                elif not has_warned:
                    LOG.warning(
                        "accumulation overflow in linear OutC=%d",
                        out_c)
            if qrec.calc_q != qrec.acc_q:
                acc_tensor = qrec.acc_q.reduce_from(acc_tensor, qrec.calc_q)
        elif details is not None:
            details['min_acc'] = min(np.min(acc_slice), details['min_acc'])
            details['max_acc'] = max(np.max(acc_slice), details['max_acc'])

    if qrec and qrec.out_qs[0] != qrec.acc_q:
        return qrec.out_qs[0].reduce_from(acc_tensor, qrec.acc_q)

    return acc_tensor
