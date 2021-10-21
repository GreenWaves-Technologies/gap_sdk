#
# Copyright (c) 2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# Modified by Manuele Rusci - manuele.rusci@unibo.it
# Copyright (c) 2018 Università di Bologna
#

import torch
import torch.nn as nn
import math

from torch.nn.parameter import Parameter
from torch.nn.functional import pad
from torch.nn.modules import Module
from torch.nn.modules.utils import _single, _pair, _triple
import torch.nn.functional as F

###
# Utils
###

def symmetric_linear_quantization_scale_factor(num_bits, saturation_val):
    # Leave one bit for sign
    n = 2 ** (num_bits - 1) - 1
    return n / saturation_val


def asymmetric_linear_quantization_scale_factor(num_bits, saturation_min, saturation_max):
    n = 2 ** num_bits - 1
    return n / (saturation_max - saturation_min)


def clamp(input, min, max, inplace=False):
    if inplace:
        input.clamp_(min, max)
        return input
    return torch.clamp(input, min, max)


def linear_quantize(input, scale_factor, inplace=False):
    if inplace:
        input.mul_(scale_factor).floor_()
        return input
    return torch.floor(scale_factor * input)


def linear_quantize_clamp(input, scale_factor, clamp_min, clamp_max, inplace=False):
    output = linear_quantize(input, scale_factor, inplace)
    return clamp(output, clamp_min, clamp_max, inplace)


def linear_dequantize(input, scale_factor, inplace=False):
    if inplace:
        input.div_(scale_factor)
        return input
    return input / scale_factor


def get_tensor_max_abs(tensor):
    return max(abs(tensor.max().item()), abs(tensor.min().item()))


def get_tensor_avg_max_abs_across_batch(tensor):
    # Assume batch is at dim 0
    tv = tensor.view(tensor.size()[0], -1)
    avg_max = tv.max(dim=1)[0].mean().item()
    avg_min = tv.min(dim=1)[0].mean().item()
    return max(abs(avg_max), abs(avg_min))


def get_quantized_range(num_bits, signed=True):
    if signed:
        n = 2 ** (num_bits - 1)
        return -n, n - 1
    return 0, 2 ** num_bits - 1


###
# Clipping-based linear quantization (e.g. DoReFa, WRPN)
###


class LinearQuantizeSTE(torch.autograd.Function):
    @staticmethod
    def forward(ctx, input, scale_factor, dequantize, inplace):
        if inplace:
            ctx.mark_dirty(input)
        output = linear_quantize(input, scale_factor, inplace)
        if dequantize:
            output = linear_dequantize(output, scale_factor, inplace)
        return output

    @staticmethod
    def backward(ctx, grad_output):
        # Straight-through estimator
        return grad_output, None, None, None


class LearnedClippedLinearQuantizeSTE(torch.autograd.Function):
    @staticmethod
    def forward(ctx, input, clip_val, num_bits, dequantize, inplace):
        ctx.save_for_backward(input, clip_val)
        if inplace:
            ctx.mark_dirty(input)
        scale_factor = asymmetric_linear_quantization_scale_factor(num_bits, 0, clip_val.data[0])
        output = clamp(input, 0, clip_val.data[0], inplace)
        output = linear_quantize(output, scale_factor, inplace)
        if dequantize:
            output = linear_dequantize(output, scale_factor, inplace)
        return output

    @staticmethod
    def backward(ctx, grad_output):
        input, clip_val = ctx.saved_tensors
        grad_input = grad_output.clone()
        grad_input.masked_fill_(input.le(0), 0)
        grad_input.masked_fill_(input.ge(clip_val.data[0]), 0)

        grad_alpha = grad_output.clone()
        grad_alpha.masked_fill_(input.lt(clip_val.data[0]), 0)
        grad_alpha = grad_alpha.sum().expand_as(clip_val)

        # Straight-through estimator for the scale factor calculation
        return grad_input, grad_alpha, None, None, None


###
# Layers
###

# Missing online estimation of min/max  -- se below: TODO
class QuantMeasure(nn.Module):
    """docstring for QuantMeasure."""

    def __init__(self, num_bits=8, momentum=0.1):
        super(QuantMeasure, self).__init__()
        self.register_buffer('running_min', torch.zeros(1))
        self.register_buffer('running_max', torch.zeros(1))
        self.momentum = momentum
        self.num_bits = num_bits

    def forward(self, input):
        if self.training:
            min_value = input.detach().view(
                input.size(0), -1).min(-1)[0].mean()
            max_value = input.detach().view(
                input.size(0), -1).max(-1)[0].mean()
            self.running_min.mul_(self.momentum).add_(
                min_value * (1 - self.momentum))
            self.running_max.mul_(self.momentum).add_(
                max_value * (1 - self.momentum))
        else:
            min_value = self.running_min
            max_value = self.running_max
        return quantize(input, self.num_bits, min_value=float(min_value), max_value=float(max_value), num_chunks=16)


########### adapt this ####################
class ClippedLinearQuantization(nn.Module):
    def __init__(self, num_bits, clip_val, dequantize=True, inplace=False):
        super(ClippedLinearQuantization, self).__init__()
        self.num_bits = num_bits
        self.clip_val = clip_val
        self.scale_factor = asymmetric_linear_quantization_scale_factor(num_bits, 0, clip_val)
        self.dequantize = dequantize
        self.inplace = inplace

    def forward(self, input):
        input = clamp(input, 0, self.clip_val, self.inplace)
        input = LinearQuantizeSTE.apply(input, self.scale_factor, self.dequantize, self.inplace)
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(num_bits={1}, clip_val={2}{3})'.format(self.__class__.__name__, self.num_bits, self.clip_val,
                                                           inplace_str)


class LearnedClippedLinearQuantization(nn.Module):
    def __init__(self, num_bits, init_act_clip_val, dequantize=True, inplace=True):
        super(LearnedClippedLinearQuantization, self).__init__()
        self.num_bits = num_bits
        self.clip_val = nn.Parameter(torch.Tensor([init_act_clip_val]))
        self.dequantize = dequantize
        self.inplace = inplace

    def forward(self, input):
        input = LearnedClippedLinearQuantizeSTE.apply(input, self.clip_val, self.num_bits, self.dequantize, self.inplace)
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(num_bits={1}, clip_val={2}{3})'.format(self.__class__.__name__, self.num_bits, self.clip_val,
                                                           inplace_str)


## Quantized Layers for forward pass only: TODO
class ScaledClippedLinearQuantization(nn.Module):
    def __init__(self, clip_val=1, M=1,low_clip_val=0, inplace=True):
        super(ScaledClippedLinearQuantization, self).__init__()
        self.clip_val = clip_val
        self.low_clip_val = low_clip_val
        self.M_ZERO = M
        self.N_ZERO = M
        self.inplace = inplace

    def forward(self, input):
        input = input.mul(self.M_ZERO).mul(2**self.N_ZERO)
        input = input.floor()
        input = clamp(input, self.low_clip_val, self.clip_val, self.inplace)
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(M0={1}, N0={2}, clip_val={3}{4})'.format(self.__class__.__name__, self.M_ZERO, self.N_ZERO, self.clip_val,
                                                           inplace_str)


class ScaledClippedLinearQuantizationChannel(nn.Module):
    def __init__(self, n_channels, clip_val=1, M=1, inplace=True):
        super(ScaledClippedLinearQuantizationChannel, self).__init__()
        self.clip_val = clip_val
        self.M_ZERO = torch.Tensor(n_channels).fill_(M)
        self.N_ZERO = torch.Tensor(n_channels).fill_(M)
        self.inplace = inplace

    def forward(self, input):
        input = input.mul(self.M_ZERO.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(input.size()))
        input = input.mul(self.N_ZERO.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(input.size()))
        input = input.floor()
        input = clamp(input, 0, self.clip_val, self.inplace)
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(clip_val={1}{2})'.format(self.__class__.__name__, self.clip_val, inplace_str)



class LearnedClippedSymLinearQuantizeSTE(torch.autograd.Function):
    @staticmethod
    def forward(ctx, input, clip_val, num_bits, dequantize, inplace):
        ctx.save_for_backward(input, clip_val)
        if inplace:
            ctx.mark_dirty(input)
        scale_factor = symmetric_linear_quantization_scale_factor(num_bits, clip_val.data[0])
        output = clamp(input, -clip_val.data[0], clip_val.data[0], inplace)
        output = linear_quantize(output, scale_factor, inplace)
        if dequantize:
            output = linear_dequantize(output, scale_factor, inplace)
        return output

    @staticmethod
    def backward(ctx, grad_output):
        input, clip_val = ctx.saved_tensors
        grad_input = grad_output.clone()
        grad_input.masked_fill_(input.abs().ge(clip_val.data[0]), 0)

        grad_alpha = grad_output.clone()
        grad_alpha.masked_fill_(input.abs().lt(clip_val.data[0]), 0)
        grad_alpha = grad_alpha.sum().expand_as(clip_val)

        # Straight-through estimator for the scale factor calculation
        return grad_input, grad_alpha, None, None, None

class LearnedClippedSymLinearQuantizationADD(nn.Module):
    def __init__(self, num_bits, init_act_clip_val, dequantize=True, inplace=False):
        super(LearnedClippedSymLinearQuantizationADD, self).__init__()
        self.num_bits = num_bits
        self.clip_val = nn.Parameter(torch.Tensor([init_act_clip_val]))
        self.dequantize = dequantize
        self.inplace = inplace

    def forward(self, input):
        output = LearnedClippedSymLinearQuantizeSTE.apply(input, self.clip_val, self.num_bits, self.dequantize, self.inplace)
        return output

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(num_bits={1}, clip_val={2}{3})'.format(self.__class__.__name__, self.num_bits, self.clip_val,
                                                           inplace_str)

class LearnedClippedSymLinearQuantization(nn.Module):
    def __init__(self, num_bits, init_act_clip_val, dequantize=True, inplace=False):
        super(LearnedClippedSymLinearQuantization, self).__init__()
        self.num_bits = num_bits
        self.clip_val = nn.Parameter(torch.Tensor([init_act_clip_val]))
        self.dequantize = dequantize
        self.inplace = inplace

    def forward(self, input):
        input = LearnedClippedSymLinearQuantizeSTE.apply(input, self.clip_val, self.num_bits, self.dequantize, self.inplace)
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(num_bits={1}, clip_val={2}{3})'.format(self.__class__.__name__, self.num_bits, self.clip_val,
                                                           inplace_str)


class ScaledClippedLinearQuantizationChannelBias(nn.Module):
    def __init__(self, n_channels, clip_val=1, M=1, inplace=True, bias=True):
        super(ScaledClippedLinearQuantizationChannelBias, self).__init__()
        self.clip_val = clip_val
        self.M_ZERO = torch.Tensor(n_channels).fill_(M)
        self.N_ZERO = torch.Tensor(n_channels).fill_(M)
        self.BIAS = torch.Tensor(n_channels).fill_(M)
        self.inplace = inplace
        self.bias = bias

    def forward(self, input):
        if self.bias is True:
            input = input.mul(self.M_ZERO.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(input.size()))
            input = input.mul(self.N_ZERO.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(input.size()))
            input = input.add(self.BIAS.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(input.size()))
            input = input.floor()
            input = clamp(input, 0, self.clip_val, self.inplace)
        else:
            input = input.mul(self.M_ZERO).mul(self.N_ZERO)
            
        return input

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(clip_val={1}{2})'.format(self.__class__.__name__, self.clip_val, inplace_str)

class Round(nn.Module):
    def __init__(self, inplace=True):
        super(Round, self).__init__()
        self.inplace = inplace

    def forward(self, input):
        if self.inplace is True:
            input.floor_()
            return input
        else:
            return input.floor()
            

## Quantized Layers for forward pass only: TODO
class ScaledThresholdsQuantization4d(nn.Module):
    def __init__(self, num_bits=8, M=1, inplace=True):
        super(ScaledThresholdsQuantization4d, self).__init__()
        self.n_thresholds = 2**num_bits -1
        #internal params
        self.thresholds = None
        self.signs = None
        #attributes
        self.M = M
        self.inplace = inplace
        self.num_bits = num_bits

    def forward(self, input):
        input = input.mul(self.M).floor()
        v0,v1,v2,v3 = input.size()
        input = input.unsqueeze(4).expand(v0,v1,v2,v3,self.n_thresholds)
        threshold_t = self.thresholds.unsqueeze(0).unsqueeze(2).unsqueeze(2)
        threshold_t = threshold_t.expand(v0,v1,v2,v3,self.n_thresholds)
        comp = input > threshold_t
        output_pos = comp.sum(4)
        comp = input <= threshold_t
        output_neg = comp.sum(4)
        sign_t = self.signs.unsqueeze(0).unsqueeze(2).unsqueeze(2).expand(v0,v1,v2,v3)
        output = torch.where(sign_t>0, output_pos, output_neg).type_as(input)
        return output

    def __repr__(self):
        inplace_str = ', inplace' if self.inplace else ''
        return '{0}(M={1}, n_thresholds={2}{3})'.format(self.__class__.__name__, self.M, self.n_thresholds,
                                                           inplace_str)

# Tensorflow convolution with SAME padding
class _ConvNd(Module):

    def __init__(self, in_channels, out_channels, kernel_size, stride,
                 padding, dilation, transposed, output_padding, groups, bias):
        super(_ConvNd, self).__init__()
        if in_channels % groups != 0:
            raise ValueError('in_channels must be divisible by groups')
        if out_channels % groups != 0:
            raise ValueError('out_channels must be divisible by groups')
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.kernel_size = kernel_size
        self.stride = stride
        self.padding = padding
        self.dilation = dilation
        self.transposed = transposed
        self.output_padding = output_padding
        self.groups = groups
        if transposed:
            self.weight = Parameter(torch.Tensor(
                in_channels, out_channels // groups, *kernel_size))
        else:
            self.weight = Parameter(torch.Tensor(
                out_channels, in_channels // groups, *kernel_size))
        if bias:
            self.bias = Parameter(torch.Tensor(out_channels))
        else:
            self.register_parameter('bias', None)
        self.reset_parameters()

    def reset_parameters(self):
        n = self.in_channels
        for k in self.kernel_size:
            n *= k
        stdv = 1. / math.sqrt(n)
        self.weight.data.uniform_(-stdv, stdv)
        if self.bias is not None:
            self.bias.data.uniform_(-stdv, stdv)

    def __repr__(self):
        s = ('{name}({in_channels}, {out_channels}, kernel_size={kernel_size}'
             ', stride={stride}')
        if self.padding != (0,) * len(self.padding):
            s += ', padding={padding}'
        if self.dilation != (1,) * len(self.dilation):
            s += ', dilation={dilation}'
        if self.output_padding != (0,) * len(self.output_padding):
            s += ', output_padding={output_padding}'
        if self.groups != 1:
            s += ', groups={groups}'
        if self.bias is None:
            s += ', bias=False'
        s += ')'
        return s.format(name=self.__class__.__name__, **self.__dict__)


class Conv2d_SAME(_ConvNd):

    def __init__(self, in_channels, out_channels, kernel_size, stride=1,
                 padding=0, dilation=1, groups=1, bias=True):
        kernel_size = _pair(kernel_size)
        stride = _pair(stride)
        padding = _pair(padding)
        dilation = _pair(dilation)
        super(Conv2d_SAME, self).__init__(
            in_channels, out_channels, kernel_size, stride, padding, dilation,
            False, _pair(0), groups, bias)

    def forward(self, input):
        return conv2d_same_padding(input, self.weight, self.bias, self.stride,
                        self.padding, self.dilation, self.groups)


# custom con2d, because pytorch don't have "padding='same'" option.
def conv2d_same_padding(input, weight, bias=None, stride=1, padding=1, dilation=1, groups=1):

    input_rows = input.size(2)
    filter_rows = weight.size(2)
    effective_filter_size_rows = (filter_rows - 1) * dilation[0] + 1
    out_rows = (input_rows + stride[0] - 1) // stride[0]
    padding_needed = max(0, (out_rows - 1) * stride[0] + effective_filter_size_rows -
                  input_rows)
    padding_rows = max(0, (out_rows - 1) * stride[0] +
                        (filter_rows - 1) * dilation[0] + 1 - input_rows)
    rows_odd = (padding_rows % 2 != 0)
    padding_cols = max(0, (out_rows - 1) * stride[0] +
                        (filter_rows - 1) * dilation[0] + 1 - input_rows)
    cols_odd = (padding_rows % 2 != 0)

    #print('cols_odd', int(cols_odd), 'rows_odd', int(rows_odd), 'padding',(padding_rows // 2, padding_cols // 2), 'dialtion', dilation)
    if rows_odd or cols_odd:
        input = pad(input, [0, int(cols_odd), 0, int(rows_odd)])

    return F.conv2d(input, weight, bias, stride,
                  padding=(padding_rows // 2, padding_cols // 2),
                  dilation=dilation, groups=groups)