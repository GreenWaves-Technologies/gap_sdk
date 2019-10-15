# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import sys
from typing import Sequence

import numpy as np

from graph.types import (ActivationParameters, Conv2DParameters, FcParameters,
                         InputParameters, OutputParameters, PadParameters,
                         PoolingParameters, SoftMaxParameters)
from quantization.quantization_record import (FilterQuantizationRecord,
                                              QuantizationRecord)

from .kernels.conv2d import conv2d
from .kernels.linear import linear
from .kernels.misc import activation, concat, softmax
from .kernels.pool import av_pool, max_pool
from .kernels.utils import pad


class Executer():

    @classmethod
    def execute(cls, node, in_tensors, qrec=False):
        execute_switch = {
            Conv2DParameters: cls.execute_conv2d,
            FcParameters: cls.execute_linear,
            PoolingParameters: cls.execute_pooling,
            InputParameters: cls.execute_input,
            OutputParameters: cls.execute_output,
            ActivationParameters: cls.execute_activation,
            PadParameters: cls.execute_pad,
            SoftMaxParameters: cls.execute_softmax
        }
        func = execute_switch.get(node.__class__)
        if func:
            return func(node, in_tensors, qrec)
        raise NotImplementedError("kernel for node %s is not implemented" % node.__class__)

    @staticmethod
    def execute_softmax(node: PoolingParameters,
                        in_tensors: Sequence[np.array],
                        qrec: QuantizationRecord = None):

        return [softmax(node, node.in_dims, node.out_dims, in_tensors[0], qrec)], None

    @staticmethod
    def execute_pad(node: PoolingParameters,
                    in_tensors: Sequence[np.array],
                    qrec: QuantizationRecord = None):
        del qrec

        return [pad(in_tensors[0], node.in_dims[0], node.padding, "zero")], None

    @staticmethod
    def execute_concat(node: PoolingParameters,
                       in_tensors: Sequence[np.array],
                       qrec: QuantizationRecord = None):
        # TODO - check quantization
        # TODO - Axis
        del qrec
        return [concat(node, node.in_dims, node.out_dims[0], in_tensors)], None

    @staticmethod
    def execute_activation(node: PoolingParameters,
                           in_tensors: Sequence[np.array],
                           qrec: QuantizationRecord = None):

        return [activation(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec)], None

    @staticmethod
    def execute_input(node: PoolingParameters,
                      in_tensors: Sequence[np.array],
                      qrec: QuantizationRecord = None):
        value = in_tensors[node.index]
        if qrec:
            value = qrec.out_qs[0].quantize(value)

        # ensure that the shape of the input is correct
        try:
            value = value.reshape(node.dims.shape)
        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s}".format(ex)
            ).with_traceback(trace_back)

        return [value], None

    @staticmethod
    def execute_output(node: PoolingParameters,
                       in_tensors: Sequence[np.array],
                       qrec: QuantizationRecord = None):
        del node, qrec
        return [in_tensors[0]], None

    @staticmethod
    def execute_pooling(node: PoolingParameters,
                        in_tensors: Sequence[np.array],
                        qrec: QuantizationRecord = None):
        if node.pool_type == "max":
            val = max_pool(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec)
        elif node.pool_type == "average":
            val = av_pool(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec)
        else:
            raise ValueError("Incorrect pooling type: %s" % node.pool_type)

        return [val], None

    @staticmethod
    def execute_linear(node: FcParameters,
                       in_tensors: Sequence[np.array],
                       qrec: FilterQuantizationRecord = None):
        if qrec:
            weights = qrec.weights_q.quantize(node.weights)
            biases = qrec.biases_q.quantize(node.biases)
        else:
            weights = node.weights
            biases = node.biases

        details = {}
        res = linear(node,
                     node.in_dims[0],
                     node.out_dims[0],
                     in_tensors[0],
                     weights,
                     biases,
                     qrec=qrec,
                     details=details)
        return [res], details

    @staticmethod
    def execute_conv2d(node: Conv2DParameters,
                       in_tensors: Sequence[np.array],
                       qrec: FilterQuantizationRecord = None):

        if qrec:
            weights = qrec.weights_q.quantize(node.weights)
            biases = qrec.biases_q.quantize(node.biases)
        else:
            weights = node.weights
            biases = node.biases

        details = {}
        res = conv2d(node,
                     node.in_dims[0],
                     node.out_dims[0],
                     in_tensors[0],
                     weights,
                     biases,
                     qrec=qrec,
                     details=details)
        return [res], details
