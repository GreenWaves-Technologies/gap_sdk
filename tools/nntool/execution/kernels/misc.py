# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

import numpy as np

LOG = logging.getLogger("nntool." + __name__)

def activation(params, in_dims, out_dims, in_tensor, qrec=None):
    if params.activation == "relu":
        max_ = np.maximum(in_tensor, 0, in_tensor)
        if qrec and qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.out_qs[0].reduce_from(max_, qrec.in_qs[0])
        return max_
    raise NotImplementedError(params.activation + " not supported in this version")

def concat(params, in_dims, _, in_tensors):
    # TODO - different quantizations on inputs?
    axis = in_dims[0].get_order_idx(params.axis)
    return np.concatenate(in_tensors, axis)

# pylint: disable=unused-argument
def softmax(params, in_dims, out_dims, in_tensors, qrec=None):
    in_tensors = in_tensors
    if qrec:
        # TODO - Implement properly quantized version
        dqinput = qrec.in_qs[0].dequantize(in_tensors)
        return qrec.out_qs[0].quantize(np.exp(dqinput)/np.sum(np.exp(dqinput)))
    return np.exp(in_tensors)/np.sum(np.exp(in_tensors))
