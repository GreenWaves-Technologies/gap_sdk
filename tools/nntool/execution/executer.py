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

import sys
from typing import Sequence

import numpy as np

from graph.types import (ActivationParameters, Conv2DParameters, FcParameters,
                         InputParameters, OutputParameters, PadParameters,
                         PoolingParameters, SoftMaxParameters, ReshapeParameters,
                         MatrixAddParameters, ConcatParameters, TransposeParameters)
from quantization.quantization_record import (FilterQuantizationRecord,
                                              QuantizationRecord)

from .kernels.conv2d import conv2d
from .kernels.linear import linear
from .kernels.misc import activation, concat, softmax, reshape, add, transpose
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
            SoftMaxParameters: cls.execute_softmax,
            ReshapeParameters: cls.execute_reshape,
            MatrixAddParameters: cls.execute_add,
            ConcatParameters: cls.execute_concat,
            TransposeParameters: cls.execute_transpose
        }
        func = execute_switch.get(node.__class__)
        if func:
            return func(node, in_tensors, qrec)
        raise NotImplementedError("kernel for node %s is not implemented" % node.__class__)

    @staticmethod
    def execute_transpose(node: TransposeParameters,
                          in_tensors: Sequence[np.array],
                          qrec: QuantizationRecord = None):

        return [transpose(node, node.in_dims, node.out_dims, in_tensors[0], qrec)], None

    @staticmethod
    def execute_reshape(node: ReshapeParameters,
                        in_tensors: Sequence[np.array],
                        qrec: QuantizationRecord = None):

        return [reshape(node, node.in_dims, node.out_dims, in_tensors[0], qrec)], None

    @staticmethod
    def execute_softmax(node: SoftMaxParameters,
                        in_tensors: Sequence[np.array],
                        qrec: QuantizationRecord = None):

        return [softmax(node, node.in_dims, node.out_dims, in_tensors[0], qrec)], None

    @staticmethod
    def execute_pad(node: PadParameters,
                    in_tensors: Sequence[np.array],
                    qrec: QuantizationRecord = None):
        del qrec

        return [pad(in_tensors[0], node.in_dims[0], node.padding, "zero")], None

    @staticmethod
    def execute_concat(node: ConcatParameters,
                       in_tensors: Sequence[np.array],
                       qrec: QuantizationRecord = None):
        # TODO - check quantization
        # This assumes that the input tensors have been transposed to the correct
        # order for the concat
        del qrec
        return [concat(node, node.in_dims, node.out_dims[0], in_tensors)], None

    @staticmethod
    def execute_add(node: MatrixAddParameters,
                    in_tensors: Sequence[np.array],
                    qrec: QuantizationRecord = None):
        return [add(node, node.in_dims, node.out_dims, in_tensors, qrec)], None

    @staticmethod
    def execute_activation(node: ActivationParameters,
                           in_tensors: Sequence[np.array],
                           qrec: QuantizationRecord = None):

        return [activation(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec)], None

    @staticmethod
    def execute_input(node: InputParameters,
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
        if node.transpose_out:
            value = np.transpose(value, node.transpose_out)
        return [value], None

    @staticmethod
    def execute_output(node: OutputParameters,
                       in_tensors: Sequence[np.array],
                       qrec: QuantizationRecord = None):
        del qrec
        result = in_tensors[0]
        if node.transpose_in:
            result = np.transpose(result, node.transpose_in)

        return [result], None

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
