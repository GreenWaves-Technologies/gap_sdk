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

from execution.kernels.conv2d import conv2d
from execution.kernels.linear import linear
from execution.kernels.misc import activation, concat
from execution.kernels.pool import av_pool, max_pool
from graph.dim import (Conv2DFilterDim, DilationDim, Dim, FcFilterDim, PadDim,
                       PoolFilterDim, StrideDim)
from graph.types import (ActivationParameters, ConcatParameters,
                         Conv2DParameters, FcParameters, PoolingParameters,
                         MatrixAddParameters)
from quantization.qtype import QType
from quantization.quantization_record import (FilterQuantizationRecord,
                                              QuantizationRecord)

def test_conf2d_normal():
    weights = np.arange(9).reshape([1, 1, 3, 3])
    filt = Conv2DFilterDim(3, 3, 1, 1)
    stride = StrideDim(1)
    pad = PadDim(0)
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([1, 4, 4])
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    details = {}
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None, details=details)
    # assert details['max_acc'] == 438.0 and details['min_acc'] == 258.0
    assert np.array_equal(output_, [[[258, 294], [402, 438]]])

def test_conf2d_depth():
    # TF Lite depthwise convolution
    weights = np.arange(9).reshape([3, 3])
    weights = np.repeat(weights, 2).reshape([1, 3, 3, 2])
    filt = Conv2DFilterDim(3, 3, 2, 1).impose_order(["in_c", "h", "w", "out_c"])
    stride = StrideDim(1)
    pad = PadDim(0)
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              groups=1,
                              multiplier=2,
                              tf_depthwise=True,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([1, 4, 4])
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output1 = conv2d(params, in_dims, out_dims[0], input_, weights, None)
    assert np.array_equal(output1, [[[258, 294], [402, 438]], [[258, 294], [402, 438]]])
    output2 = conv2d(params, in_dims, out_dims[0], input_, weights, None, allow_faster=False)
    assert np.array_equal(output1, output2)

def test_conf2d_depth_q():
    calc_q = QType(32, 9, True)
    biases_q = acc_q = out_q = QType(16, 4, True)
    weights_q = QType(16, 4, True)
    in_q = QType(16, 5, True)
    # TF Lite depthwise convolution
    biases = np.full([2], 0.5)
    qbiases = biases_q.quantize(biases)
    weights = np.full([3, 3], 0.5)
    weights = np.repeat(weights, 2).reshape([1, 3, 3, 2])
    qweights = weights_q.quantize(weights)
    filt = Conv2DFilterDim(3, 3, 2, 1).impose_order(["in_c", "h", "w", "out_c"])
    stride = StrideDim(1)
    pad = PadDim(0)
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              groups=1,
                              multiplier=2,
                              tf_depthwise=True,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    qrec = FilterQuantizationRecord(in_qs=[in_q], out_qs=[out_q], weights_q=weights_q,
                                    biases_q=biases_q, acc_q=acc_q, calc_q=calc_q)
    input_ = np.full([1, 4, 4], 2)
    qinput_ = in_q.quantize(input_)
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, biases)
    qoutput_ = conv2d(params, in_dims, out_dims[0], qinput_, qweights, qbiases, qrec=qrec)
    dqoutput_ = out_q.dequantize(qoutput_)
    assert np.array_equal(output_, dqoutput_)

def test_conf2d_depth2():
    # TF Lite depthwise convolution
    weights = np.arange(9).reshape([3, 3])
    weights = np.repeat(weights, 4).reshape([1, 3, 3, 4])
    filt = Conv2DFilterDim(3, 3, 4, 1).impose_order(["in_c", "h", "w", "out_c"])
    stride = StrideDim(1)
    pad = PadDim(0)
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              groups=2,
                              multiplier=2,
                              tf_depthwise=True,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([4, 4])
    input_ = np.concatenate((input_, input_)).reshape([2, 4, 4])
    in_dims = Dim.named(c=2, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None)
    assert np.array_equal(output_, [[[258, 294], [402, 438]], [[258, 294], [402, 438]],\
        [[258, 294], [402, 438]], [[258, 294], [402, 438]]])

def test_conf2d_q():
    calc_q = QType(32, 9, True)
    biases_q = acc_q = out_q = QType(16, 4, True)
    weights_q = QType(16, 4, True)
    in_q = QType(16, 5, True)
    biases = np.full([1], 0.5)
    qbiases = biases_q.quantize(biases)
    weights = np.full([3, 3], 0.5)
    weights = np.repeat(weights, 2).reshape([2, 3, 3, 1])
    qweights = weights_q.quantize(weights)
    filt = Conv2DFilterDim(3, 3, 1, 2).impose_order(["in_c", "h", "w", "out_c"])
    stride = StrideDim(1)
    pad = PadDim(0)
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    qrec = FilterQuantizationRecord(in_qs=[in_q], out_qs=[out_q], weights_q=weights_q,
                                    biases_q=biases_q, acc_q=acc_q, calc_q=calc_q)
    input_ = np.full([2, 4, 4], 2)
    qinput_ = in_q.quantize(input_)
    in_dims = Dim.named(c=2, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, biases)
    qoutput_ = conv2d(params, in_dims, out_dims[0], qinput_, qweights, qbiases, qrec=qrec)
    dqoutput_ = out_q.dequantize(qoutput_)
    assert np.array_equal(output_, dqoutput_)

def test_conf2d_pad():
    weights = np.arange(9).reshape([1, 1, 3, 3])
    filt = Conv2DFilterDim(3, 3, 1, 1)
    stride = StrideDim(1)
    pad = PadDim.same()
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([1, 4, 4])
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None)
    assert np.array_equal(output_, [[[73, 121, 154, 103], [171, 258, 294, 186],\
        [279, 402, 438, 270], [139, 187, 202, 113]]])

def test_conf2d_2_out_c_pad():
    weights = np.arange(9).reshape([1, 3, 3])
    weights = np.append(weights, weights, axis=0).reshape([2, 1, 3, 3])
    filt = Conv2DFilterDim(3, 3, 2, 1)
    stride = StrideDim(1)
    pad = PadDim.same()
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([1, 4, 4])
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None)
    assert np.array_equal(output_, [[[73, 121, 154, 103], [171, 258, 294, 186],\
        [279, 402, 438, 270], [139, 187, 202, 113]], [[73, 121, 154, 103], [171, 258, 294, 186],\
        [279, 402, 438, 270], [139, 187, 202, 113]]])

def test_conf2d_2_in_2_out_c():
    weights = np.arange(4).reshape([1, 2, 2])
    weights = np.append(weights, weights, axis=0)
    weights = np.append(weights, weights, axis=0)
    weights = weights.reshape([2, 2, 2, 2])
    filt = Conv2DFilterDim(2, 2, 2, 2)
    stride = StrideDim(1)
    pad = PadDim.valid()
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(9).reshape([1, 3, 3])
    input_ = np.append(input_, input_, axis=0)
    in_dims = Dim.named(c=2, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None, None)
    assert np.array_equal(output_, [[[38., 50.], [74., 86.]],\
        [[38., 50.], [74., 86.]]])

def test_conf2d_pad_dilate():
    weights = np.arange(9).reshape([1, 1, 3, 3])
    filt = Conv2DFilterDim(3, 3, 1, 1)
    stride = StrideDim(1)
    pad = PadDim.same()
    dilation = DilationDim(2)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    input_ = np.arange(16).reshape([1, 4, 4])
    in_dims = Dim.named(c=1, h=4, w=4).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None, None)
    assert np.array_equal(output_, [[[266., 206.], [98., 66.]]])

def test_conf2d_q2(caplog):
    caplog.set_level(logging.INFO)
    weights_q = QType(16, 1, True)
    weights = weights_q.quantize(np.full([1, 1, 2, 2], 1.0))
    filt = Conv2DFilterDim(2, 2, 1, 1)
    stride = StrideDim(1)
    pad = PadDim.valid()
    dilation = DilationDim(1)
    params = Conv2DParameters("test",
                              filt=filt,
                              stride=stride,
                              padding=pad,
                              dilation=dilation,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])
    in_q = QType(16, 0, True)
    calc_q = QType(weights_q.bits + in_q.bits, weights_q.q + in_q.q, True)
    qrec = FilterQuantizationRecord(in_qs=[in_q], out_qs=[in_q], weights_q=weights_q,
                                    acc_q=calc_q, calc_q=calc_q)
    input_ = in_q.quantize(np.full([1, 2, 2], 1.0))
    in_dims = Dim.named(c=1, h=2, w=2).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = conv2d(params, in_dims, out_dims[0], input_, weights, None, qrec=qrec)
    output_ = in_q.dequantize(output_)
    assert np.array_equal(output_, [[[4.]]])

def test_av_pool_normal():
    filt = PoolFilterDim(2, 2)
    stride = StrideDim(1)
    pad = PadDim(0)
    params = PoolingParameters("test",
                               filt=filt,
                               stride=stride,
                               padding=pad,
                               pool_type="average")
    input_ = np.arange(9).reshape([1, 3, 3])
    in_dims = Dim.named(c=1, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = av_pool(params, in_dims, out_dims[0], input_)
    assert np.array_equal(output_, [[[2, 3], [5, 6]]])

def test_av_pool_q():
    filt = PoolFilterDim(2, 2)
    stride = StrideDim(1)
    pad = PadDim(0)
    params = PoolingParameters("test",
                               filt=filt,
                               stride=stride,
                               padding=pad,
                               pool_type="average")
    in_q = QType(16, 0, True)
    qrec = QuantizationRecord([in_q], [in_q])
    input_ = in_q.quantize(np.arange(9)).reshape([1, 3, 3])
    in_dims = Dim.named(c=1, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = av_pool(params, in_dims, out_dims[0], input_, qrec=qrec)
    output_ = in_q.dequantize(output_)
    assert np.array_equal(output_, [[[2, 3], [5, 6]]])

def test_max_pool_normal():
    filt = PoolFilterDim(2, 2)
    stride = StrideDim(1)
    pad = PadDim(0)
    params = PoolingParameters("test",
                               filt=filt,
                               stride=stride,
                               padding=pad,
                               pool_type="max")
    input_ = np.arange(9).reshape([1, 3, 3])
    in_dims = Dim.named(c=1, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = max_pool(params, in_dims, out_dims[0], input_)
    assert np.array_equal(output_, [[[4, 5], [7, 8]]])

def test_max_pool_q():
    filt = PoolFilterDim(2, 2)
    stride = StrideDim(1)
    pad = PadDim(0)
    params = PoolingParameters("test",
                               filt=filt,
                               stride=stride,
                               padding=pad,
                               pool_type="max")
    in_q = QType(16, 0, True)
    qrec = QuantizationRecord([in_q], [in_q])
    input_ = in_q.quantize(np.arange(9)).reshape([1, 3, 3])
    in_dims = Dim.named(c=1, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])
    output_ = max_pool(params, in_dims, out_dims[0], input_)
    output_ = in_q.dequantize(output_)
    assert np.array_equal(output_, [[[4, 5], [7, 8]]])

def test_fc():
    filt = FcFilterDim(3, 3, 3, 1)
    params = FcParameters("test", filt=filt)
    weights_q = QType(16, 2, True)
    in_q = QType(16, 2, True)
    acc_q = QType(16, 4, True)
    calc_q = QType(16, 4, True)
    qrec = FilterQuantizationRecord(in_qs=[in_q], out_qs=[in_q], calc_q=calc_q, acc_q=acc_q, biases_q=None, weights_q=weights_q)
    weights = weights_q.quantize(np.full([3, 1, 3, 3], 1.0))
    input_ = in_q.quantize(np.arange(9)).reshape([1, 3, 3])
    in_dims = Dim.named(c=1, h=3, w=3).impose_order(['c', 'h', 'w'])
    out_dims = params.get_output_size([in_dims])

    output_ = linear(params, in_dims, out_dims[0], input_, weights, None, qrec=qrec)
    output_ = in_q.dequantize(output_)
    assert np.array_equal(output_, [[[36]], [[36]], [[36]]])

def test_activation():
    in_q = QType(16, 13, True)
    input_ = in_q.quantize(np.array([-1.2, 0.5, 0.5, -0.6])).reshape([4, 1, 1])
    in_dims = Dim.named(c=4, h=1, w=1).impose_order(['c', 'h', 'w'])
    params = ActivationParameters("test")
    qrec = QuantizationRecord([in_q], [in_q])
    out_dims = params.get_output_size([in_dims])
    output_ = activation(params, in_dims, out_dims[0], input_, qrec=qrec)
    output_ = in_q.dequantize(output_)
    assert np.array_equal(output_, [[[0]], [[0.5]], [[0.5]], [[0]]])

def test_concat():
    inputs = [np.full([1, 2, 2], 1.0), np.full([2, 2, 2], 2.0)]
    in_dims = [Dim.named(c=1, h=2, w=2).impose_order(['c', 'h', 'w']),
               Dim.named(c=2, h=2, w=2).impose_order(['c', 'h', 'w'])]
    params = ConcatParameters("test", axis=0)
    out_dims = params.get_output_size(in_dims)
    output_ = concat(params, in_dims, out_dims[0], inputs)
    assert isinstance(output_, np.ndarray) and np.array_equal(output_, np.concatenate(inputs, 0))

def test_concat_q():
    in_q = QType(16, 1, True)
    inputs = [in_q.quantize(np.full([1, 2, 2], 1.0)),
              in_q.quantize(np.full([2, 2, 2], 2.0))]
    in_dims = [Dim.named(c=1, h=2, w=2).impose_order(['c', 'h', 'w']),
               Dim.named(c=1, h=2, w=2).impose_order(['c', 'h', 'w'])]
    params = ConcatParameters("test", axis=0)
    out_dims = params.get_output_size(in_dims)
    output_ = concat(params, in_dims, out_dims[0], inputs)
    assert np.array_equal(output_, np.concatenate(inputs, 0))

