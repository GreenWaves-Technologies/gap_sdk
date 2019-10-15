# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from collections.abc import Iterable

import numpy as np

from graph.dim import Dim
from quantization.quantization_record import (FilterQuantizationRecord,
                                              QuantizationRecord)

def shape_dict(ord_, dims):
    return [dims[i] for i in ord_]

def srange(dim, **kwargs):
    slice_ = []
    for k in dim.order:
        if k in kwargs:
            v = kwargs[k]
            if isinstance(v, Iterable):
                slice_.append(slice(*v))
            elif isinstance(v, int):
                slice_.append(slice(v, v+1, 1))
        else:
            slice_.append(slice(None))
    return tuple(slice_)

def zeros(shape, qrec: QuantizationRecord, elem: str):
    dtype = getattr(qrec, elem).dtype if qrec else None
    return np.zeros(shape, dtype=dtype)

def pad(array: np.array, in_dim: Dim, padding: Dim, pad_type: str):
    if pad_type == "zero":
        return np.pad(array, padding.numpy_pad_shape(in_dim),\
            'constant', constant_values=0)
    raise NotImplementedError()

def prepare_acc(biases: np.array, out_dims: Dim, qrec: FilterQuantizationRecord):
    if biases is None:
        acc_tensor = zeros(out_dims.shape, qrec, 'acc_q')
    else:
        acc_tensor = zeros((out_dims.c, out_dims.h, out_dims.w), qrec, 'acc_q')
        if qrec and qrec.acc_q != qrec.biases_q:
            biases = qrec.acc_q.expand_from(biases, qrec.biases_q)
        for i in range(out_dims.c):
            acc_tensor[i, :] = biases[i]
        acc_tensor = acc_tensor.transpose(out_dims.transpose_from_order(('c', 'h', 'w')))
    return acc_tensor
