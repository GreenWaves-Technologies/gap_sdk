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

from graph.types import (MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters)
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)


def AT_NORM(x, y):
    return x >> y


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
    elif params.activation == "hsigmoid":
        if qrec:
            calc_q = QType(bits=32, q=qrec.in_qs[0].q + 15, signed=True)
            c1 = 3 << qrec.in_qs[0].q
            c2 = (1 << 15) // 6
            ub = 6 << qrec.in_qs[0].q
            in_tensor = in_tensor.astype(np.int32)
            in_tensor = np.multiply(np.minimum(np.maximum(in_tensor + c1, 0), ub), c2, dtype=np.int32)
            qrec.out_qs[0].reduce_from(in_tensor, calc_q)
        return np.minimum(np.maximum(in_tensor + 3, 0), 6) / 6
    elif params.activation == "hswish":
        if qrec:
            calc_q = QType(bits=32, q=qrec.in_qs[0].q + 15, signed=True)
            c1 = 3 << qrec.in_qs[0].q
            c2 = (1 << 15) // 6
            ub = 6 << qrec.in_qs[0].q
            in_tensor = in_tensor.astype(np.int32)
            in_tensor = np.multiply(np.minimum(np.maximum(in_tensor + c1, 0), ub), in_tensor, dtype=np.int32) >> qrec.in_qs[0].q
            return qrec.out_qs[0].reduce_from(np.multiply(in_tensor, c2, dtype=np.int32), calc_q)
        return in_tensor * np.minimum(np.maximum(in_tensor + 3, 0), 6) / 6

    raise NotImplementedError(params.activation + " not supported in this version")


def concat(params, in_dims, out_dim, in_tensors, qrec=None):
    del in_dims, out_dim, qrec
    if params.transpose_in:
        in_tensors = [np.transpose(in_tensor, params.transpose_in) for in_tensor in in_tensors]
    out_tensor = np.concatenate(in_tensors, params.axis)
    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out)
    return out_tensor


def softmax_func(v):
    max_v = np.max(v)
    v = v - max_v
    return np.exp(v)/np.sum(np.exp(v))

# pylint: disable=unused-argument


def softmax(params, in_dims, out_dims, in_tensors, qrec=None):
    np.seterr(over='raise')
    if qrec:
        # TODO - Implement properly quantized version
        dqinput = qrec.in_qs[0].dequantize(in_tensors)
        return qrec.out_qs[0].quantize(softmax_func(dqinput))
    return softmax_func(in_tensors)


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


PIECEWISE_OPS = {
    MatrixAddParameters: {'op': lambda x, y, dtype: x + y, 'is_mult': False},
    MatrixMulParameters: {'op': lambda x, y, dtype: np.multiply(x, y, dtype=dtype), 'is_mult': True},
    MatrixSubParameters: {'op': lambda x, y, dtype: x - y, 'is_mult': False},
    MatrixDivParameters: {'op': lambda x, y, dtype: x / y, 'is_mult': True},
}


def piecewise(params, in_dims, out_dims, in_tensors, qrec=None):
    func = PIECEWISE_OPS[params.__class__]
    op = func['op']
    if qrec:
        if func['is_mult']:
            q_calc = QType(bits=32, q=qrec.in_qs[0].q+qrec.in_qs[1].q, signed=True)
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32)
            isum = op(i1, i2, np.int32)
            res = qrec.out_qs[0].reduce_from(isum, q_calc)
        else:
            off_in = abs(qrec.in_qs[0].q - qrec.in_qs[1].q)
            if qrec.in_qs[0].q > qrec.in_qs[1].q:
                i1 = in_tensors[0].astype(np.int32) >> off_in
                i2 = in_tensors[1].astype(np.int32)
            else:
                i1 = in_tensors[0].astype(np.int32)
                i2 = in_tensors[1].astype(np.int32) >> off_in
            q_calc = QType(bits=32, q=min(qrec.in_qs[0].q, qrec.in_qs[1].q), signed=True)
            isum = op(i1, i2, None)
            res = qrec.out_qs[0].reduce_from(isum, q_calc)
        return res
    return op(in_tensors[0], in_tensors[1], None)


def matscale3(params, in_dims, out_dims, in_tensors, qrec):
    assert qrec.in_qs[0].bits == qrec.in_qs[1].bits
    assert qrec.in_qs[1].bits == qrec.in_qs[2].bits
    if qrec.in_qs[0].bits == 8:
        q_calc = QType(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q + qrec.in_qs[2].q, signed=True)
        res = np.multiply(np.multiply(in_tensors[0], in_tensors[1],
                                      dtype=np.int32),
                          in_tensors[2],
                          dtype=np.int32)
        res = qrec.out_qs[0].reduce_from(res, q_calc)
    elif qrec.in_qs[0].bits == 16:
        q_calc = QType(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
        res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
        res = qrec.out_qs[0].reduce_from(res, q_calc)
        q_calc = QType(bits=32, q=qrec.in_qs[2].q + qrec.out_qs[0].q, signed=True)
        res = np.multiply(res, in_tensors[2], dtype=np.int32)
        res = qrec.out_qs[0].reduce_from(res, q_calc)
    return res


def matscale2(params, in_dims, out_dims, in_tensors, qrec=None):
    assert qrec.in_qs[0].bits == qrec.in_qs[1].bits
    q_calc = QType(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
    res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
    res = qrec.out_qs[0].reduce_from(res, q_calc)
    return res


def matscale(params, in_dims, out_dims, in_tensors, qrec=None):
    if qrec:
        LOG.debug("matscale input %s", ",".join([t.dtype.name for t in in_tensors]))
        if len(in_dims) == 3:
            return matscale3(params, in_dims, out_dims, in_tensors, qrec)
        return matscale2(params, in_dims, out_dims, in_tensors, qrec)
    if len(in_dims) == 3:
        return in_tensors[0] * in_tensors[1] * in_tensors[2]
    return in_tensors[0] * in_tensors[1]
