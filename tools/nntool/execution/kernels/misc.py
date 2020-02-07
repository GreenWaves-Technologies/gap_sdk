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

from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

def activation(params, in_dims, out_dims, in_tensor, qrec=None):
    del out_dims
    if params.activation == "sigmoid":
        if qrec:
            # TODO - Implement properly quantized version
            dqinput = qrec.in_qs[0].dequantize(in_tensor)
            return qrec.out_qs[0].quantize(1/(1 + np.exp(-dqinput)))
        return 1/(1 + np.exp(-in_tensor))
    elif params.activation == "relu":
        max_ = np.maximum(in_tensor, 0)
        if qrec and qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.out_qs[0].reduce_from(max_, qrec.in_qs[0])
        return max_
    elif params.activation == "relu6" or params.activation == "relun":
        if params.activation == "relu6":
            relu_n = 6
        else:
            relu_n = params.activation_params
        if isinstance(relu_n, list):
            assert len(relu_n) == in_dims.c, "mismatch of relu n length with input channels"
            relu_n = np.array(relu_n)
        else:
            relu_n = np.full([in_dims.c], relu_n)

        if qrec:
            relu_n = qrec.in_qs[0].quantize(relu_n)

        max_ = np.maximum(in_tensor, 0)
        for i in range(in_dims.c):
            max_[in_dims.srange(c=i)] = np.minimum(max_[in_dims.srange(c=i)], relu_n[i])
        if qrec and qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.out_qs[0].reduce_from(max_, qrec.in_qs[0])
        return max_

    raise NotImplementedError(params.activation + " not supported in this version")

def concat(params, in_dims, out_dim, in_tensors, qrec=None):
    del in_dims, out_dim, qrec
    if params.transpose_in:
        in_tensors = [np.transpose(in_tensor, params.transpose_in) for in_tensor in in_tensors]
    out_tensor = np.concatenate(in_tensors, params.axis)
    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out)
    return out_tensor

# pylint: disable=unused-argument
def softmax(params, in_dims, out_dims, in_tensors, qrec=None):
    if qrec:
        # TODO - Implement properly quantized version
        dqinput = qrec.in_qs[0].dequantize(in_tensors)
        return qrec.out_qs[0].quantize(np.exp(dqinput)/np.sum(np.exp(dqinput)))
    return np.exp(in_tensors)/np.sum(np.exp(in_tensors))

def reshape(params, in_dims, out_dims, in_tensor, qrec=None):
    del in_dims, out_dims
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in)
    in_tensor = np.reshape(in_tensor, params.shape)
    if params.transpose_out:
        in_tensor = np.transpose(in_tensor, params.transpose_out)
    return in_tensor

def transpose(params, in_dims, out_dims, in_tensor, qrec=None):
    del in_dims, out_dims
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in)
    return in_tensor

def add(params, in_dims, out_dims, in_tensors, qrec=None):
    if qrec:
        off_in = abs(qrec.in_qs[0].q - qrec.in_qs[1].q)
        if qrec.in_qs[0].q > qrec.in_qs[1].q:
            i1 = in_tensors[0].astype(np.int32) >> off_in
            i2 = in_tensors[1].astype(np.int32)
        else:
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32) >> off_in
        q_calc = QType(bits=32, q=min(qrec.in_qs[0].q, qrec.in_qs[1].q), signed=True)
        isum = i1 + i2
        res = qrec.out_qs[0].reduce_from(isum, q_calc)
        return res
    return in_tensors[0] + in_tensors[1]
