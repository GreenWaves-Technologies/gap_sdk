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
import sys
from typing import Sequence

import numpy as np

from graph.types import (ActivationParameters, ConcatParameters,
                         ConstantInputParameters, Conv2DParameters,
                         FcParameters, GlobalPoolParameters, InputParameters,
                         MatrixAddParameters,
                         MatrixBroadcastedLinearOpParameters,
                         MatrixDivParameters, MatrixMulParameters,
                         MatrixSubParameters, MatScaleFusionParameters,
                         OutputParameters, PadParameters, PoolingParameters,
                         ReshapeParameters, SoftMaxParameters,
                         TransposeParameters)
from quantization.quantization_record import (FilterQuantizationRecord,
                                              QuantizationRecord)

from .kernels.conv2d import conv2d
from .kernels.linear import linear
from .kernels.misc import (activation, concat, matscale, piecewise, reshape,
                           softmax, transpose)
from .kernels.pool import av_global_pool, av_pool, max_pool, max_global_pool
from .kernels.utils import pad

LOG = logging.getLogger("nntool." + __name__)


class Executer():

    @classmethod
    def execute(cls, node, in_tensors, qrec=False, detect_overflow=True):
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
            MatrixAddParameters: cls.execute_piecewise,
            MatrixDivParameters: cls.execute_piecewise,
            MatrixMulParameters: cls.execute_piecewise,
            MatrixSubParameters: cls.execute_piecewise,
            ConcatParameters: cls.execute_concat,
            TransposeParameters: cls.execute_transpose,
            ConstantInputParameters: cls.execute_constant,
            MatScaleFusionParameters: cls.execute_matscale,
            GlobalPoolParameters: cls.execute_globalpool,
        }
        func = execute_switch.get(node.__class__)
        if func:
            if func == cls.execute_conv2d:
                return func(node, in_tensors, qrec, detect_overflow)
            return func(node, in_tensors, qrec)
        raise NotImplementedError("kernel for node %s is not implemented" % node.__class__)

    @staticmethod
    def execute_globalpool(node: GlobalPoolParameters,
                           in_tensors: Sequence[np.array],
                           qrec: QuantizationRecord = None):
        if node.pool_type == "average":
            return [av_global_pool(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec=qrec)], None
        return [max_global_pool(node, node.in_dims[0], node.out_dims[0], in_tensors[0], qrec=qrec)], None
        

    @staticmethod
    def execute_matscale(node: MatScaleFusionParameters,
                         in_tensors: Sequence[np.array],
                         qrec: QuantizationRecord = None):

        return [matscale(node, node.in_dims, node.out_dims, in_tensors, qrec=qrec)], None

    @staticmethod
    def execute_constant(node: ConstantInputParameters,
                         in_tensors: Sequence[np.array],
                         qrec: QuantizationRecord = None):
        del in_tensors
        value = node.value
        if qrec:
            LOG.debug("executing quantized constant")
            value = qrec.out_qs[0].quantize(value)
        return [value], None

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
    def execute_piecewise(node: MatrixBroadcastedLinearOpParameters,
                          in_tensors: Sequence[np.array],
                          qrec: QuantizationRecord = None):
        return [piecewise(node, node.in_dims, node.out_dims, in_tensors, qrec)], None

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
        if value.size == node.dims.size():
            value = value.reshape(node.dims.shape)
        #else:
        #    value = resize(value, node.dims.shape)
        if qrec:
            value = qrec.out_qs[0].quantize(value)

        # ensure that the shape of the input is correct
        try:
            value = value.reshape(node.dims.shape)

        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s} try -W and -H flags \
                to force the input dimensions to specific value".format(ex)
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
                       qrec: FilterQuantizationRecord = None,
                       detect_overflow = True):

        if qrec:
            weights = qrec.weights_q.quantize(node.weights)
            biases = qrec.biases_q.quantize(node.biases) if node.has_bias else None
            mul_biases = qrec.mul_biases_q.quantize(node.mul_biases) if node.has_mul_bias else None
        else:
            weights = node.weights
            biases = node.biases
            mul_biases = node.mul_biases

        details = {}
        res = conv2d(node,
                     node.in_dims[0],
                     node.out_dims[0],
                     in_tensors[0],
                     weights,
                     biases,
                     mul_biases=mul_biases,
                     qrec=qrec,
                     details=details,
                     detect_overflow=detect_overflow)
        return [res], details
